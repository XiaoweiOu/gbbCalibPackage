/*
 * Plotter.h
 *
 *  Created on: Feb 23, 2016
 *      Author: rjacobs
 */

#ifndef PLOTTER_H_
#define PLOTTER_H_

#include "SampleInfo.h"
#include "TCanvas.h"
#include "TPad.h"
#include <vector>
#include <utility>
#include <memory>
#include <algorithm>
#include "TString.h"
#include "TH1.h"
#include "TFile.h"

enum ColorStack{LL=kOrange,CL=kGreen-9,BL=kAzure-4,LC=kRed-3,CC=kCyan+3,BC=kGray+1,LB=kYellow-10,CB=kCyan-9,BB=kBlue+1,def=1};

struct HistogramStack{
  std::shared_ptr<TH1> full_data_hist;
  std::shared_ptr<TH1> full_mc_hist;
  std::vector<std::shared_ptr<TH1>> mc_stack_hists;

};

struct MultiHistogramStack{

  std::vector<std::shared_ptr<TH1>> mc_stack_hists;
  std::vector<std::shared_ptr<TH1>> data_stack_hists;


};

class Plotter {

private:
  
  std::vector<TString> m_samples; //pair contains filename and SampleInfo

  SampleInfo *m_sampinfo;

  TString m_in_dir; //input directory name

  std::vector<TString> m_stacked_hist_names; //Order:  BB,BC,BL,CB,CC,CL,LB,LC,LL
  TString m_hist_name;

  //TCanvas *m_canv;
  //TPad *m_pad1;
  //TPad *m_pad2;

  std::shared_ptr<TCanvas> m_canv;
  std::shared_ptr<TPad> m_pad1;
  std::shared_ptr<TPad> m_pad2; 


  TString m_xtitle;

  bool m_doGrid;
  bool m_Rebin;
  bool m_haveHistStack;

  bool m_doSys;
  std::vector<TString> m_SysNames;

  float m_lumi;

  float m_rebin_factor;
  float m_xh;
  float m_xl;
  float m_yh;
  float m_yl;

public:
	Plotter();
	Plotter(TString in_dir, SampleInfo* info,TString name="");
	virtual ~Plotter();

	std::shared_ptr<TCanvas> getCanvas(){return m_canv;}
	std::shared_ptr<TPad> getPad1(){return m_pad1;}
	std::shared_ptr<TPad> getPad2(){return m_pad2;}

	std::vector<TString> *getSamplesPtr(){return &m_samples;}
	std::vector<TString> *getStackedHistsPtr(){return &m_stacked_hist_names;}
	void setStackedHists(std::vector<TString>& stacked_hists){m_stacked_hist_names=stacked_hists;}

	void setPad1Borders(double x1, double y1, double x2, double y2){m_pad1->SetPad(x1,y1,x2,y2);}

    void Save(TString filename){
    	m_canv->SaveAs(filename.Data());
    }

    void Save(TFile* file,TString name){
      
      if(!file) return;
      m_canv->SetName(name.Data());
      file->cd();
      m_canv->Write();

      std::cout<<"Write canvas "<<m_canv->GetName()<<" to file "<<file->GetName()<<std::endl;
    }

    void ListFiles(TString in_dir);

	float getWeightSample(TString sample_name);
	
	HistogramStack readInHistStack(std::vector<TString>& hist_list, TString data_hist_name);
	MultiHistogramStack readInMultiHistStack(std::vector<TString>& mc_hist_list, std::vector<TString>& data_hist_list);


	void prepareCanvas();
	void makeDataStackedMCPlot(std::vector<TString>& hist_list, TString data_hist_name);
	void makeDataMCPlot(TString hist_name);
	void makeDataMCBtagEffPlot();
	void makeComparisonPlot(std::vector<TString>& hist_list, bool doNormalized=0);
	void makeReweightHisto(TString hist_name, TFile *outfile);
	
	void printTemplateCorrelations(std::vector<TString>& hist_names, TFile *outfile=0 ); 
	void makeFitTemplates(std::vector<TString>& hist_list,std::vector<TString>& data_hist_names, TFile* outfile, TString sys_ext="Nom");


	void makeLegend();
	void addLabel(int type=0);

	void setSysList(std::vector<TString>& syslist ){m_doSys=true; m_SysNames=syslist;};
	void addSysShades(TH1* nominal_hist); //sys_name

	void setAtlasStyle();
	void setXaxisLimits(float xl, float xh){
		m_xh=xh;
		m_xl=xl;
	}
	void setYaxisLimits(float yl, float yh){
		m_yh=yh;
		m_yl=yl;
	}
    void setXaxisTitle(TString title){
    	m_xtitle=title;
    }
    void setLumi(float lumi){
    	m_lumi=lumi;
    }
    void setRebinFactor(int rebin_factor){
    	m_Rebin=true;
    	m_rebin_factor=rebin_factor;
    }
    void setGrid(bool doGrid){
    	m_doGrid=doGrid;
    }
    void h2_style(TH1 *h);
    void ratio_style(TH1 *h);
    void leg_style(TLegend *leg);
    ColorStack getColor(TString hist_name);



};

#endif /* PLOTTER_H_ */
