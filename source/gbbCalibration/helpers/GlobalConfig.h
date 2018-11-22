/*
 * GlobalConfig.h
 *
 *  Created on: Dec 12, 2016
 *      Author: rjacobs
 *  Modified by: aemerman
 */

#ifndef GLOBALCONFIG_H_
#define GLOBALCONFIG_H_

#include "TString.h"
#include <vector>
#include <map>
#include <iostream>

// Class used to standardize regions and naming conventions between
// the histogramming and fitting codes
// Initialized with config file containing list of systematics, flavour
// pairs, and bins in 'Key Value1,Value2,...' pairs.
class GlobalConfig {

 public:

  GlobalConfig(const TString& config_path);
  virtual ~GlobalConfig();

  std::vector<TString> GetSystematics() { return m_Systematics; }
  std::vector<TString> GetFlavourPairs() { return m_FlavourPairs; }

  std::vector<float> GetFatJetPtBins() { return m_FatJetPtBins; }
  std::vector<float> GetMuonJetPtBins() { return m_MuonJetPtBins; }
  std::vector<float> GetNonMuJetPtBins() { return m_NonMuJetPtBins; }

  std::vector<TString> GetFatJetRegions() { return m_FatJetRegions; }
  TString GetFatJetLabel(float fatJetPt) { return GetPtLabel(fatJetPt, m_FatJetPtBins, m_FatJetRegions); }
  std::vector<TString> GetTrkJetRegions();
  TString GetDiTrkJetLabel(float muJetPt, float nonMuJetPt) {
    return GetMuJetLabel(muJetPt)+"_"+GetNonMuJetLabel(nonMuJetPt);
  }
  TString GetMuJetLabel(float muJetPt) { return GetPtLabel(muJetPt, m_MuonJetPtBins, m_MuonJetRegions); }
  TString GetNonMuJetLabel(float nonMuJetPt) { 
    return GetPtLabel(nonMuJetPt, m_NonMuJetPtBins, m_NonMuJetRegions);
  }

  TString GetMCHistName(const TString sys, const TString ptLabel, const TString flav, const TString var);
  TString GetDataHistName(const TString ptLabel, const TString var) {
    return GetMCHistName("Data",ptLabel,"",var);
  }
  std::vector<TString> GetMCHistNamesBySys(const TString sys, const TString ptLabel, const TString var);
  std::map<TString,std::vector<TString> > GetMCHistNames(const TString ptLabel, const TString var);

 private:
  std::vector<TString> m_Systematics;
  std::vector<TString> m_FlavourPairs;

  std::vector<float> m_FatJetPtBins;
  std::vector<TString> m_FatJetRegions;
  std::vector<float> m_MuonJetPtBins;
  std::vector<float> m_NonMuJetPtBins;
  std::vector<TString> m_MuonJetRegions;
  std::vector<TString> m_NonMuJetRegions;

  std::vector<TString> MakeLabels(const std::vector<float> ptBins, const TString prefix);
  TString GetPtLabel(float pt, std::vector<float> ptBins, std::vector<TString> ptRegions);
  TString FloatToStr(const float val);
  
  std::vector<TString> SplitString(TString str, char delim);
  std::vector<float> SplitStringD(TString str, char delim);

};

#endif
