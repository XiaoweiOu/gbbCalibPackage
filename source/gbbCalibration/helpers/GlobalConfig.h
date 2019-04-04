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

  bool GetIsR20p7() { return m_isR20p7; }

  std::vector<TString> GetSystematics() { return m_isR20p7 ? m_Systematics_R20p7 : m_Systematics_R21; }
  std::vector<TString> GetSystematics_Sd0() { return m_Systematics_Sd0; }
  std::vector<TString> GetSystematics_WeightVar() { return m_Systematics_WeightVar; }
  std::vector<TString> GetFlavourPairs() { return m_FlavourPairs; }
  char GetFlavour(int truthType);
  TString GetFlavourPair(int muJetTruth, int nonMuJetTruth);

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

  std::vector<TString> GetTemplateVariables() { return m_TemplateVariables; }
  std::vector<TString> GetPlotVariables() { return m_PlotVariables; }
  std::vector<double> GetBinning(const TString var);

  TString GetMCHistName(const TString sys, const TString ptLabel, const TString flav, const TString var);
  TString GetDataHistName(const TString ptLabel, const TString var) {
    return GetMCHistName("Nom",ptLabel,"Data",var);
  }
  std::vector<TString> GetMCHistNamesBySys(const TString sys, const TString ptLabel, const TString var);
  std::map<TString,std::vector<TString> > GetMCHistNames(const TString ptLabel, const TString var);

 private:
  bool m_isR20p7;
  std::vector<TString> m_Systematics_R20p7;
  std::vector<TString> m_Systematics_R21;
  std::vector<TString> m_Systematics_Sd0;
  std::vector<TString> m_Systematics_WeightVar;
  std::vector<TString> m_FlavourPairs;
  bool m_doMergeFlavours;

  std::vector<float> m_FatJetPtBins;
  std::vector<TString> m_FatJetRegions;
  std::vector<float> m_MuonJetPtBins;
  std::vector<float> m_NonMuJetPtBins;
  std::vector<TString> m_MuonJetRegions;
  std::vector<TString> m_NonMuJetRegions;

  std::vector<TString> m_TemplateVariables;
  std::vector<TString> m_PlotVariables;
  std::map<TString, std::vector<double> > m_PlotBinning;

  std::vector<TString> MakeLabels(const std::vector<float> ptBins, const TString prefix);
  TString GetPtLabel(float pt, std::vector<float> ptBins, std::vector<TString> ptRegions);
  TString FloatToStr(const float val);

  std::vector<TString> SplitString(TString str, char delim);
  std::vector<float> SplitStringD(TString str, char delim);

};

#endif
