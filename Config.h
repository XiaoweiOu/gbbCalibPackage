/*
 * Config.h
 *
 *  Created on: Dec 12, 2016
 *      Author: rjacobs
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "TString.h"
#include <vector>
#include <iostream>

class Config{

 private:
  TString m_inputfile;
  int m_NParams;
  int m_N_pseudo_exp;

  bool m_Debug;
  bool m_doMCStatsNP;
  bool m_doFitInFatJetPtBins;
  
  std::vector<TString> m_pairs;
  std::vector<TString> m_systematics;
  std::vector<TString> m_chans;
  std::vector<TString> m_params_names;

  std::vector<float> m_mutrackjetbins;
  std::vector<float> m_nonmutrackjetbins; 

  std::vector<float> m_fatjetbins;
  std::vector<float> m_params_in;
  std::vector<float> m_params_low;
  std::vector<float> m_params_high;
  
  TString m_xlabel;
  TString m_ylabel;
  TString m_plot_label;
  TString m_sub_label;
  TString m_subsub_label;

 public:

  Config(TString& cfg_path);
  Config();
  virtual ~Config();
  
  std::vector<TString> SplitString(TString str, char delim);
  std::vector<float> SplitStringD(TString str, char delim);

  TString GetInfileName(){ return m_inputfile; }
  bool DoDebug(){ return m_Debug; }
  bool DoMCStatsNP(){ return m_doMCStatsNP; }
  bool DoFitInFatJetPtBins(){return m_doFitInFatJetPtBins;}

  std::vector<float> GetMuTrackJetBins(){ return m_mutrackjetbins; }
  std::vector<float> GetNonmuTrackJetBins(){ return m_nonmutrackjetbins; }

  std::vector<float> GetFatJetBins(){ return m_fatjetbins; }

  std::vector<TString> GetPairs(){return m_pairs;}
  std::vector<TString> GetChans(){ return m_chans; };
  std::vector<TString> GetParamNames(){ return m_params_names; }

  int GetNPseudoExp(){return m_N_pseudo_exp;}

  std::vector<float> GetParamStartValues(){ return m_params_in; }
  std::vector<float> GetParamLowValues(){ return m_params_low; }
  std::vector<float> GetParamHighValues(){ return m_params_high; }

  TString GetXLabel(){return m_xlabel;}
  TString GetYLabel(){return m_ylabel;}
  TString GetPlotLabel(){return m_plot_label;}
  TString GetSubLabel(){return m_sub_label;}
  TString GetSubSubLabel(){return m_subsub_label;}


  std::vector<TString> GetSystematics(){ return m_systematics; }
  std::vector<TString> GetAllRegions();

  std::vector<TString> GetMCHistNames(TString& jettype, TString& pt_region,TString &sys, TString &variable);
  TString GetDataHistName(TString& jettype, TString& pt_region, TString &variable);

};


#endif
