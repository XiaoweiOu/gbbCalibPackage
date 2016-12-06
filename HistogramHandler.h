/*
 * HistogramHandler based on Plotter
 *
 *  Created on: Feb 23, 2016
 *      Author: rjacobs
 */

#ifndef HISTOGRAMHANDLER_H_
#define HISTOGRAMHANDLER_H_

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


struct HistogramStack{
  std::shared_ptr<TH1> full_data_hist;
  std::shared_ptr<TH1> full_mc_hist;
  std::vector<std::shared_ptr<TH1>> mc_stack_hists;

};

struct MultiHistogramStack{

  std::vector<std::shared_ptr<TH1>> mc_stack_hists;
  std::vector<std::shared_ptr<TH1>> data_stack_hists;


};

class HistogramHandler {

private:
  
  std::vector<TString> m_samples; //pair contains filename and SampleInfo

  SampleInfo *m_sampinfo;

  float m_lumi;

  std::vector<TString> m_stacked_hist_names; //Order:  BB,BC,BL,CB,CC,CL,LB,LC,LL
  TString m_hist_name;

  bool m_haveHistStack;


public:
  HistogramHandler();
  HistogramHandler(TString in_dir, SampleInfo* info,float lumi,TString name="");
  virtual ~HistogramHandler();
  
  std::vector<TString> *getSamplesPtr(){return &m_samples;}
  std::vector<TString> *getStackedHistsPtr(){return &m_stacked_hist_names;}
  void setStackedHists(std::vector<TString>& stacked_hists){m_stacked_hist_names=stacked_hists;}
 
  void setLumi(float lumi){
    m_lumi=lumi;
  }

 
  void ListFiles(TString in_dir);
  
  float getWeightSample(TString sample_name);
  
  HistogramStack readInHistStack(std::vector<TString>& hist_list, TString data_hist_name);
  MultiHistogramStack readInMultiHistStack(std::vector<TString>& mc_hist_list, std::vector<TString>& data_hist_list);


};

#endif /* HISTOGRAMHANDLER_H_ */
