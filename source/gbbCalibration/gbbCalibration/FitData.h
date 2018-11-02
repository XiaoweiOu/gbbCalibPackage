/*
 * FitData.h
 *
 *  Created on: Mar 29, 2016
 *      Author: rjacobs
 */

#ifndef FITDATA_H_
#define FITDATA_H_

#include "TFile.h"
#include "TString.h"
#include "TH1D.h"
#include <vector>
#include <map>
#include <iostream>
#include "TMinuit.h"
#include <utility>
#include <memory>
#include "TMath.h"
#include "TRandom3.h"
#include "TCanvas.h"
#include "TF1.h"

class FitData {

 private:
  std::shared_ptr<TFile> m_infile;
  std::shared_ptr<TRandom3> m_rndm;

  std::map<TString,std::shared_ptr<TH1D>> m_dataHistMap;
  std::map<TString,std::vector<std::shared_ptr<TH1D>>> m_templateHistsMap;

  std::map<TString,std::vector<double>> m_BootStrapDataMap;
  std::map<TString,std::vector<std::vector<double>>> m_BootStrapTemplateMap;

  std::map<TString,std::vector<TString>> m_MC_HistNamesMap;
  std::map<TString,TString> m_Data_HistNamesMap;

  std::vector<TString> m_chans;

 public:
  FitData();
  FitData(TString filename, std::vector<TString> chans);
  virtual ~FitData();

  void ResetHists(){
    m_dataHistMap.clear();
    m_templateHistsMap.clear();
    m_BootStrapDataMap.clear();
    m_BootStrapTemplateMap.clear();
    m_MC_HistNamesMap.clear();
    m_Data_HistNamesMap.clear();
    
  }

  void SetHistogramNames(TString &channel,TString& data_name, std::vector<TString>& mc_names){
    m_MC_HistNamesMap[channel]=mc_names;
    m_Data_HistNamesMap[channel]=data_name;
  }

  void ReadInHistograms(TString &channel, int smooth_Ntimes=0);
  void ReadInHistograms(int smooth_Ntimes=0){
    for(auto chan : m_chans) ReadInHistograms(chan, smooth_Ntimes);
  }

  std::shared_ptr<TH1D> GetDataHist(TString &channel){ return m_dataHistMap[channel]; }
  std::vector<std::shared_ptr<TH1D>> GetMCHists(TString &channel){ return m_templateHistsMap[channel]; }


  void CheckHistograms(TString &channel);
  std::vector<double> AutoRebinHistograms(TString &channel, double statThr=0.5,int N_force=1); //N_force number of bins to force merging, 1: no merging
  void FixHistogramBins(TString &channel, std::vector<double>& bins);

  void MergeTemplates(int tmp1, int tmp2); //replace template tmp1 with tmp1+tmp2, delete tmp2
  void RescaleTemplate(int tmp1, double factor, bool doKeepNorm=false);

  void KernelSmoothTemplates(float scale);
  void FitTemplateSlopes(float lowgap, float highgap);

  double GetFlavourFraction(TString &channel,int i_flav_pair);
  double GetFlavourNorm(TString &channel,int i_flav_pair);
  double GetDataNorm(TString &channel);
  double GetChanCorrelation(TString channel1, TString channel2, int i_flav_pair);

  double NegLogSingleBinPoisson(int i,TString& channel, double *par,bool doRandom=false, bool doRandomMC=false);
  double NegLogSingleBinPoissonWithNP(int i,TString& channel, double *par,bool doRandom=false);


  double NegLogLikelihood(double *par);
  double NegLogLikelihoodWithNP(double *par);
  double NegLogLikelihoodRndmData(double *par);
  double NegLogLikelihoodRndmDataWithNP(double *par);

  double NegLogLikelihoodRndmTemplate(double *par);

  void MakeBootStrapData();
  void MakeBootStrapTemplates();

  void PrintHistograms(TFile* file);
  float GetParamStartValue();

  void GetMCStatsNPInitVal(TString& channel,std::vector<double> &par, std::vector<double>& par_err);
    

};

#endif /* FITDATA_H_ */
