/*
 * HistPlotter.h
 *
 *  Created on: Feb 23, 2016
 *      Author: rjacobs
 */

#ifndef HISTPLOTTER_H_
#define HISTPLOTTER_H_

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
#include "TGraphAsymmErrors.h"
#include "FlavourFracCorrector.h"

enum ColorStack{LL=kOrange,CL=kGreen-9,BL=kAzure-4,LC=kRed-3,CC=kCyan+3,BC=kGray+1,LB=kYellow-10,CB=kCyan-9,BB=kBlue+1,def=1};

class HistPlotter {

private:
  
  TString m_base_dir; //base directory

  SampleInfo *m_sample_info;

  FlavourFracCorrector* m_corrector;
  std::vector<TString> m_fit_ptbins; 

  std::shared_ptr<TCanvas> m_canv;
  std::shared_ptr<TPad> m_pad1;
  std::shared_ptr<TPad> m_pad2; 

  TString m_xtitle;

  bool m_doGrid;
  bool m_Rebin;
  bool m_haveHistStack;

  bool m_showFitSys;
  bool m_showBtagSys;
  bool m_showTotalSys;

  bool m_doNormalized;

  std::vector<TString> m_syslist_up;
  std::vector<TString> m_syslist_down;
  std::vector<TString> m_syslist_1side;

  float m_lumi;

  float m_rebin_factor;
  float m_xh;
  float m_xl;
  float m_yh;
  float m_yl;

public:
  HistPlotter();
  HistPlotter(TString in_dir, SampleInfo* info,TString name="");
  virtual ~HistPlotter();
  
	std::shared_ptr<TCanvas> getCanvas(){return m_canv;}
	std::shared_ptr<TPad> getPad1(){return m_pad1;}
	std::shared_ptr<TPad> getPad2(){return m_pad2;}
	
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

  
	void prepareCanvas();
	void makeDataStackedMCPlot(std::vector<TString>& hist_list, TString data_hist_name);
	void makeDataMCPlot(TString hist_name);
	void makeDataMCBtagEffPlot(TString var);
	void makeComparisonPlot(std::vector<TString>& hist_list, bool doNormalized=0);
	
	void makeLegend();
	void addLabel(int type=0);

	void setSysConfig(bool showFitSys, bool showBtagSys, bool showTotSys, TString fit_file="", std::vector<TString> *fit_ptbins=0);
	
	void setSysLists(std::vector<TString> sys_list_up, std::vector<TString> sys_list_down, std::vector<TString> sys_list_1side){
	  m_syslist_up=sys_list_up; m_syslist_down=sys_list_down; m_syslist_1side=sys_list_1side;

	}

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
	
	void setDoNormalizedTrue(){
	  m_doNormalized=true;
	}

	void h2_style(TH1 *h);
	void ratio_style(TH1 *h);
	void leg_style(TLegend *leg);
	ColorStack getColor(TString hist_name);

	TGraphAsymmErrors* getFitSys(std::vector<TString> &base_hist_names);
	TGraphAsymmErrors* getBTagSys(std::vector<TString> &base_hist_names);
	TGraphAsymmErrors* getMCStat(TH1* full_mc);
	TGraphAsymmErrors* getTotalJetSys( std::vector<TString>& base_hist_names);
	TGraphAsymmErrors*  getTotalSys(TGraphAsymmErrors* fitsysgraph, TGraphAsymmErrors* btagsysgraph, TGraphAsymmErrors* jetsysgraph,  TGraphAsymmErrors* mcstatgraph);



};

#endif /* HISTPLOTTER_H_ */
