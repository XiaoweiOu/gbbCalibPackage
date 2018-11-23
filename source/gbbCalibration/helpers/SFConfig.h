/*
 * SFConfig.h
 *
 *  Created on: Dec 12, 2016
 *      Author: rjacobs
 */

#ifndef SFCONFIG_H_
#define SFCONFIG_H_

#include "TString.h"
#include <vector>
#include <iostream>
#include <map>
#include "GlobalConfig.h"

class SFConfig{

 private:
  GlobalConfig* m_global_config;
  TString m_inputfile;
  int m_NParams;
  int m_N_pseudo_exp;
  int m_Ntimes_smooth;

  double m_statThr_Rebin;

  bool m_Debug;
  bool m_doMCStatsNP;
  bool m_doFitInFatJetPtBins;
  bool m_doControlPlots;
  bool m_doCalibSequence;
  
  std::vector<TString> m_chans;
  std::vector<TString> m_params_names;

  std::map<TString,std::vector<float>> m_binning;
  std::vector<TString> m_plotvariables;

  std::vector<float> m_params_in;
  std::vector<float> m_params_low;
  std::vector<float> m_params_high;
  
  TString m_xlabel;
  TString m_ylabel;
  TString m_plot_label;
  TString m_sub_label;
  TString m_subsub_label;

 public:

  SFConfig(TString& config_path);
  SFConfig();
  virtual ~SFConfig();
  
  std::vector<TString> SplitString(TString str, char delim);
  std::vector<float> SplitStringD(TString str, char delim);

  TString GetInfileName(){ return m_inputfile; }
  bool DoDebug(){ return m_Debug; }
  bool DoMCStatsNP(){ return m_doMCStatsNP; }
  bool DoFitInFatJetPtBins(){return m_doFitInFatJetPtBins;}
  bool DoControlPlots(){return m_doControlPlots;}
  bool DoCalibSequence(){return m_doCalibSequence;}

  std::vector<float> GetBins(TString& var) { return m_global_config->GetBinning(var); }

  std::vector<float> GetFatJetPtBins() { return m_global_config->GetFatJetPtBins(); }
  std::vector<float> GetMuonJetPtBins() { return m_global_config->GetMuonJetPtBins(); }
  std::vector<float> GetNonMuJetPtBins() { return m_global_config->GetNonMuJetPtBins(); }

  int GetSmoothNtimes(){return m_Ntimes_smooth; }

  std::vector<TString> GetFlavourPairs() { return m_global_config->GetFlavourPairs(); }
  std::vector<TString> GetChans(){ return m_chans; };
  std::vector<TString> GetParamNames(){ return m_params_names; }
  std::vector<TString> GetPlotVariables(){ return m_plotvariables; }

  int GetNPseudoExp(){return m_N_pseudo_exp;}

  std::vector<float> GetParamStartValues(){ return m_params_in; }
  std::vector<float> GetParamLowValues(){ return m_params_low; }
  std::vector<float> GetParamHighValues(){ return m_params_high; }

  TString GetXLabel(){return m_xlabel;}
  TString GetYLabel(){return m_ylabel;}
  TString GetPlotLabel(){return m_plot_label;}
  TString GetSubLabel(){return m_sub_label;}
  TString GetSubSubLabel(){return m_subsub_label;}
  
  double GetRebinStatThr(){return m_statThr_Rebin;}

  std::vector<TString> GetSystematics() { return m_global_config->GetSystematics(); }
  std::vector<TString> GetAllRegions() {
    if (m_doFitInFatJetPtBins) return m_global_config->GetFatJetRegions();
    else return m_global_config->GetTrkJetRegions();
  }

  std::vector<TString> GetMCHistNames(TString& pt_region,TString &sys, TString &variable) {
    return m_global_config->GetMCHistNamesBySys(sys,pt_region,variable);
  }
  TString GetDataHistName(TString& pt_region, TString &variable) {
    return m_global_config->GetDataHistName(pt_region,variable);
  }

};


#endif
