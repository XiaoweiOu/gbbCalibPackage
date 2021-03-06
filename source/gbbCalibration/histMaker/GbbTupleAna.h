/*
 * GbbTupleAna.h
 *
 *  Created on: Feb 18, 2016
 *      Author: rjacobs
 */

#ifndef GBBTUPLEANA_H_
#define GBBTUPLEANA_H_

#include "TupleAna.h"
#include "HistogramService.h"
#include <memory>
#include <helpers/GlobalConfig.h>
#include "TEnv.h"
#include "TRandom3.h"
#include <map>
// wesley add for b tagging module
#include "BTagging/BJetTagger.h"
#include "GbbStructures.h"
#include <functional>


//
// The core class used to read our ntuple format and produce histogram inputs
// to the ScaleFactorCalculator. This class inherits from TupleAna, which was
// made with TTree::MakeClass and left largely untouched
//
class GbbTupleAna : public TupleAna {

public:
  GbbTupleAna();
  GbbTupleAna(const std::vector<TString> infiles, const TString outfilename, const TString treename, const TString configname);
  virtual ~GbbTupleAna();

  //
  // Read in the specified config file and GlobalConfig.cfg
  //
  void ReadConfig(const TString& config_path);

  //
  // Do the work and save the result
  //
  void Loop();
  void Finalize();

  void setDebug(bool value){ m_Debug=value; }
  void setDoEvenOddTemplates(bool value){m_doEvenOddTemplates=value; }
  void setReweightHisto(TString filename,TString trigger_passed);

private:
  GlobalConfig *m_config;
  HistogramService *m_HistSvc; //!
  TString m_Outputname; //! name of output file

  //
  // Set of flags to control which histograms to make. Set as a vector
  // of strings in the config file
  //
  int m_RunMode;
  typedef enum {
    FILL_TEMPLATES = 0x001,
    FILL_TRKJET_PROPERTIES = 0x002,
    FILL_FATJET_PROPERTIES = 0x004,
    FILL_ADV_PROPERTIES = 0x008,
    FILL_MC_STATS = 0x010,
    FILL_REWEIGHT = 0x020,
    FILL_PRW = 0x040,
    FILL_TRIGGER = 0x080,
    FOR_FIT_ONLY = 0x100,
    FILL_EXTRAP_INCL = 0x200,
    FILL_EXTRAP_MU = 0x400,
  } RunMode;

  // ImproveMe
  // String stores b-tagging options; Used in BTagCut
  // format: <bTag type>_FixedCutBEff_<cut efficiency number>_[systematics?]
  // example: MV2c10_FixedCutBEff_70_R20p7
  // example: XbbScore_FixedCutBEff_60_[fp2/f0]
  TString m_BTagWP;

  bool m_Debug;

  bool m_doJetPtReweighting;
  TString m_JetPtReweightFile;
  TString m_JetPtReweightFileInclusive;

  bool m_isMC;
  bool m_2016;
  bool m_2017;
  bool m_2018;
  bool m_isNominal;
  TString m_SysVarName;
  TString m_GeneratorName;
  TString m_FilterType;

  float m_SumWeightTuple;
  int m_nevtTuple;

  std::map<TString,std::shared_ptr<TH2D>> m_reweightHistos;
  bool m_doEvenOddTemplates;

  bool m_doTrackSmearing;
  bool m_noMuonReq;
  bool m_doApplyBTaggingSF;

  bool m_doSd0Systematics;

  bool m_useVRTrkJets;

  std::shared_ptr<TRandom3> m_random;
  bool m_doRandomSplitting;
  bool m_doFillMujet;

  std::vector<float> m_fatjet_pt_bins;

  bool m_doPostfitPtReweighting;
  TString m_PostfitPtReweightingFile;
  TH1D* m_postfit_reweight_hist;


  // wesley:
  // this for cutting the gbb candidate
  // in b-tagging step
  BJetTagger* m_bTagger;

  void Loop(const TString& sys);
  bool Processgbb(int i_evt);

  void GetFilterType(TString url);

  //
  // Functions for kinematic cuts. Defined in GbbSelectionHelp.cxx
  //
  bool passR4CaloJetCuts(unsigned int i_jet);
  bool passR10CaloJetCuts(unsigned int i_jet);
  bool passR2TrackJetCuts(unsigned int i_jet);
  bool passR21AssocTrkSelection(unsigned int i_trk, unsigned int i_jet);
  bool passAssocTrkSelection(unsigned int i_trk, unsigned int i_jet);
  bool passMuonSelection(unsigned int i_muon);

  //
  // Functions for b-tagging cuts
  //
  int passBTagCut(const GbbCandidate& gbbcand);
  int passMV2c10Cut(const GbbCandidate& gbbcand);
  int passXbbScoreCut(const GbbCandidate& gbbcand);
  double XbbTag_antiQCD_flat60eff(double x);
  double XbbTag_antiQCDtop_flat60eff(double x);

  //
  // Functions to get an objects location in the branch based on
  // the stored index. Must be used to get the correct index
  // for associated track-jets and muons
  //
  unsigned int getLeadingObjIndex(std::vector<float> *quantity);
  unsigned int getNthLeadingObjIndex(unsigned int n, std::vector<float> *quantity);
  int getAssocObjIndex(std::vector<int>* part_ind, int assoc_index);

  //
  // Functions for basic event cleaning cuts
  //
  bool isCleanEvt(const float total_evt_weight);
  bool hasBadJet();
  int getTrigMatch();
  bool passTrigger(const float trigjet_pt, float& prescale, TString& trigger_passed);
  bool cutTriggerBias(const float gbbcandpt, const TString trigger_passed);

  //
  // Functions to construct the GbbCandidate struct
  //
  std::vector<GbbCandidate> constructGbbCandidates(bool useLeading = false);

  //
  // Wrapper functions for the MakePlotName functions from the
  // GlobalConfig class. Defined in GbbFillHelp.cxx
  //
  TString makeInclTrkJetPlotName(GbbCandidate* gbbcand, unsigned int i, const TString var);
  TString makeDiJetPlotName(GbbCandidate* gbbcand, const TString var);
  TString makeFatJetPlotName(GbbCandidate* gbbcand, const TString var);
  TString makeInclDiJetPlotName(GbbCandidate* gbbcand, const TString var);
  TString makeFlavInclPlotName(const TString var);

  float getTrigJetWeight(int i_trig_jet, TString trigger_passed);
  float getTrigFatJetWeight(float trigfjpt, float trigfjeta,TString trigger_passed);

  //
  // Functions to fill histograms. Defined in GbbFillHelp.cxx
  // Call the function FastFillTH1D from the HistogramService class
  //
  void FillReweightInfo(int i_trig_jet, float event_weight,TString nametag="");
  void FillFatReweightInfo(float trigfat_pt,float trigfat_eta, float event_weight,TString nametag="");
  void FillMCStatsInfo(GbbCandidate* gbbcand, TString nametag="");
  void FillTriggerTurnOnHistograms(int i_trigjet, float event_weight);
  void FillTrackJetProperties(GbbCandidate* gbbcand, float event_weight,TString nametag="");
  void FillFatJetProperties(GbbCandidate* gbbcand, float event_weight,TString nametag="");
  void FillTemplates(GbbCandidate* gbbcand, float event_weight,TString nametag="");
  void FillAdvancedProperties(GbbCandidate* gbbcand, int i_trig_jet, float event_weight,TString nametag="");
  // Helper function for FillTemplates
  void FillSd0Plots(std::vector<trkjetSd0Info> sd0Info, float event_weight, std::function<TString (TString)> namingFunc);
  //Tong: Fill 2D histogram for correlation check
  void FillSd02DPlots(trkjetSd0Info j1Sd0Info, trkjetSd0Info j2Sd0Info, float event_weight, std::function<TString (TString)> namingFunc);

  bool isLargeWeightEvent(int DSID,float evt_weight, float max_evt_weight);

  //
  // Function to construct the trkjetSd0Info struct
  //
  trkjetSd0Info getTrkjetAssocSd0Info(unsigned int i_jet, bool doSmeared=false, TString d0sys="nominal", TString z0sys="nominal", int n=3);

  float getd0(unsigned int i_trk, unsigned int i_jet, bool doSmeared=false, TString sys="nominal");
  float getz0(unsigned int i_trk, unsigned int i_jet, bool doSmeared=false, TString sys="nominal");
  float getSd0(unsigned int i_trk, unsigned int i_jet, bool doSmeared=false, TString sys="nominal");

  //void getBtagSFWeights(float &btag_SF_nom, float &btag_SF_tot_up, float &btag_SF_tot_down);
  //void getBtagSysWeights(float &btag_SF_tot_up, float &btag_SF_tot_down);

  void getSystematicsFlags(GbbCandidate *, bool &hasConversion, bool &hasHadMatInt, bool &hasLightLongLived, bool &hasNoTruthMu);

  float getScaledFatPt(float fjpt){
    return fjpt*(0.9725+(1e-5)*fjpt);
    //    return fjpt*(0.95+((2.86e-5)*fjpt));
  }

};

#endif /* GBBTUPLEANA_H_ */
