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
#include <stdexcept>
#include <unordered_set>
#include <functional>
#include <string>

namespace std {
  template <> struct hash<TString> {
    size_t operator()(const TString& str) const {
      return hash<std::string>()(str.Data());
    }
  };
}

//
// Class used to standardize regions and naming conventions between
// the histogramming and fitting codes
// Initialized with config file containing list of systematics, flavour
// pairs, and bins in 'Key Value1,Value2,...' pairs.
//
class GlobalConfig {

 public:

  GlobalConfig(const TString& config_path);
  virtual ~GlobalConfig();

  bool GetIsR20p7() { return m_isR20p7; }
  bool UseLeadingJets() { return m_useLeadingJets; }
  bool MergeFlavours() { return m_mergeFlavours; }

  //
  // Functions to return lists of systematics
  // Most systematics are defined as separate trees in the input files,
  // but sd0 and btagging systematics are treated in the histogram-making
  //
  std::vector<TString> GetSystematics() { return m_isR20p7 ? m_Systematics_R20p7 : m_Systematics_R21; }
  std::vector<TString> GetSystematics_Sd0() { return m_Systematics_Sd0; }
  std::vector<TString> GetSystematics_WeightVar() { return m_Systematics_WeightVar; }

  //
  // Functions to return the set of all flavour strings
  // or the string describing given track-jet(s)
  //
  char GetFlavour(int truthType);
  std::vector<TString> GetFatJetFlavours() { return m_FatJetFlavours; }
  TString GetFatJetFlavour(int jetTruth1, int jetTruth2, int jetTruth3);

  //
  // Functions to get information about flavour merging scheme
  //
  std::map<TString, std::unordered_set<TString> > GetMergeMap() { return m_MergeMap; }
  std::vector<TString> GetFlavourGroups() { return m_FlavourGroups; }
  std::unordered_set<TString> GetFlavoursInGroup(const TString key);
  TString GetFlavourGroup(const TString key);

  //
  // Functions to return integer bin edges for the pT regions
  //
  std::vector<float> GetFatJetPtBins() { return m_FatJetPtBins; }
  std::vector<float> GetTrkJetPtBins(unsigned int i) { return m_TrkJetPtBins[i]; }
  std::vector<float> GetMuonJetPtBins() { return m_TrkJetPtBins[0]; }
  std::vector<float> GetNonMuJetPtBins() { return m_TrkJetPtBins[1]; }

  //
  // Functions to return the set of all pT regions (as strings)
  // or the pT region describing given jet(s)
  //
  std::vector<TString> GetFatJetRegions() { return m_FatJetRegions; }
  TString GetFatJetLabel(float fatJetPt) { return GetPtLabel(fatJetPt, m_FatJetPtBins, m_FatJetRegions); }
  std::vector<TString> GetDiTrkJetRegions();
  TString GetDiTrkJetLabel(float muJetPt, float nonMuJetPt) {
    return GetMuJetLabel(muJetPt)+"_"+GetNonMuJetLabel(nonMuJetPt);
  }
  std::vector<TString> GetTrkJetRegions(unsigned int nJets);
  TString GetTrkJetLabel(float trkJetPt, unsigned int i) {
    if ( i >= m_TrkJetPtBins.size())
      throw std::out_of_range("Request labels for trkjet %i that doesn't exist");
    return GetPtLabel(trkJetPt, m_TrkJetPtBins[i], m_TrkJetRegions[i]);
  }
  TString GetMuJetLabel(float muJetPt) {
    return GetPtLabel(muJetPt, m_TrkJetPtBins[0], m_TrkJetRegions[0]);
  }
  TString GetNonMuJetLabel(float nonMuJetPt) {
    return GetPtLabel(nonMuJetPt, m_TrkJetPtBins[1], m_TrkJetRegions[1]);
  }

  //
  // Functions returning the list of variables to use for the
  // template fitting, the list of variables to use for control plots
  // and the binning to use for control plots
  //
  std::vector<TString> GetTemplateVariables() { return m_TemplateVariables; }
  std::vector<TString> GetPlotVariables() { return m_PlotVariables; }
  std::vector<double> GetBinning(const TString var);

  //
  // Functions to enforce the naming conventions in the histogram-making
  // code, and to correctly call those histograms in fitting and plotting code
  //
  TString GetMCHistName(const TString sys, const TString ptLabel, const TString flav, const TString var);
  TString GetDataHistName(const TString ptLabel, const TString var) {
    return GetMCHistName("Nom",ptLabel,"Data",var);
  }
  std::vector<TString> GetMCHistNamesBySys(const TString sys, const TString ptLabel, const TString var);
  std::map<TString,std::vector<TString> > GetMCHistNames(const TString ptLabel, const TString var);

  //
  // If useLeadingJets is set then track-jets
  // are leading/subleading otherwise they
  // are muon/non-muon. These functions allow
  // plot names and labels to be changed accordingly.
  //
  TString GetTrkJetName(unsigned int i) {
    if (!m_useLeadingJets) {
      if ( i==0 ) {
        return "mj";
      } else if ( i==1 ) {
        return "nmj";
      } else return Form("j%u",i+1);
    } else return Form("j%u",i+1);
  }
  TString GetTrkJetLabel(unsigned int i) {
    if ( i==0 ) {
      return m_useLeadingJets ? "leading jet" : "muon-jet";
    } else if ( i==1 ) {
      return m_useLeadingJets ? "subleading jet" : "non-muon-jet";
    } else if ( i==2 ) {
      return "third jet";
    } else return Form("jet %u",i+1);
  }

 private:
  bool m_isR20p7;
  std::vector<TString> m_Systematics_R20p7;
  std::vector<TString> m_Systematics_R21;
  std::vector<TString> m_Systematics_Sd0;
  std::vector<TString> m_Systematics_WeightVar;
  std::vector<TString> m_FatJetFlavours;
  std::vector<TString> m_FlavourGroups;
  std::map<TString, std::unordered_set<TString> > m_MergeMap;
  bool m_mergeFlavours;
  bool m_useLeadingJets;

  std::vector<float> m_FatJetPtBins;
  std::vector<TString> m_FatJetRegions;
  std::vector<std::vector<float> > m_TrkJetPtBins;
  std::vector<std::vector<TString> > m_TrkJetRegions;

  std::vector<TString> m_TemplateVariables;
  std::vector<TString> m_PlotVariables;
  std::map<TString, std::vector<double> > m_PlotBinning;

  std::vector<TString> MakeLabels(const std::vector<float> ptBins, const TString prefix);
  TString GetPtLabel(float pt, std::vector<float> ptBins, std::vector<TString> ptRegions);
  TString FloatToStr(const float val);

};

#endif
