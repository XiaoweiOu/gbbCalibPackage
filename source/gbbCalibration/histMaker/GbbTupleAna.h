
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
#include <helpers/FlavourFracCorrector.h>
#include <helpers/GlobalConfig.h>
#include "TEnv.h"
#include "TRandom3.h"
#include <map>

struct GbbCandidate{
  
  unsigned int fat_index;
  unsigned int muojet_index;
  unsigned int nonmuojet_index;
  float fat_pt;
  bool hasleading2trackjets;
  bool hasTruthMuon;
};

struct trkjetSd0Info {
  float maxSd0;
  float maxSd0_dR;
  float subSd0;
  float subSd0_dR;
  float thirdSd0;
  float thirdSd0_dR;
  float meanSd0_ptSort;
  float meanSd0_sd0Sort;
  int nMean;
};

class GbbTupleAna : public TupleAna {

public:
  GbbTupleAna();
  GbbTupleAna(const std::vector<TString>& infiles, const TString& outfilename, const TString& configname);
  virtual ~GbbTupleAna();

  void ReadConfig(const TString& config_path);

  void Loop();
  void Finalize();

  void setDebug(bool value){ m_Debug=value; }
  void setDoEvenOddTemplates(bool value){m_doEvenOddTemplates=value; }
  void setReweightHisto(TString filename,TString trigger_passed);

  void setDiTrkJetCategories(std::vector<TString>& vect){ m_ditrkjet_cat=vect; }   
        void setPtBins(std::vector<float>& muojet_vect,std::vector<float>& nonmuojet_vect){ m_muojet_pt_bins=muojet_vect; m_nonmuojet_pt_bins=nonmuojet_vect; }

 

private:
  GlobalConfig *m_config;
  HistogramService *m_HistSvc; //!
  TString m_Outputname; //! name of output file
  std::map<TString, TChain*> m_chains;

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
  
  typedef enum {
    MV2C10_WP70,
    XBB_WP60,
  } BTagType;

  bool m_Debug;

  bool m_doJetPtReweighting;
  TString m_JetPtReweightFile;
  TString m_JetPtReweightFileInclusive;
  
  bool m_isMC;
  bool m_isNominal;
  TString m_SysVarName;
  TString m_GeneratorName;
    
  float m_SumWeightTuple;
  int m_nevtTuple;
  
  std::map<TString,std::shared_ptr<TH2D>> m_reweightHistos;
  bool m_doEvenOddTemplates;

  bool m_doFlavFracCorrection; 
  
  TString m_FlavFracCorrectorFile;
  FlavourFracCorrector* m_FlavFracCorrector; 
  
  bool m_doTrackSmearing;

  bool m_doInclusiveGbb;
  
  bool m_doApplyBTaggingSF;
  
  bool m_doMergeDiTrkjetCat;
  std::vector<TString> m_ditrkjet_cat;
  std::vector<TString> m_trkjet_cat;
  std::vector<float> m_muojet_pt_bins;
  std::vector<float> m_nonmuojet_pt_bins;
  
  std::shared_ptr<TRandom3> m_random;
  bool m_doRandomSplitting;
  bool m_doFillMujet;

  std::vector<float> m_fatjet_pt_bins;
  
  bool m_doPostfitPtReweighting;
  TString m_PostfitPtReweightingFile;
  TH1D* m_postfit_reweight_hist;

  void Loop(const TString& sys);
  bool Processgbb(int i_evt);
  
  void GetGeneratorName(TString url);
  bool passR4CaloJetCuts(unsigned int i_jet);
  bool passR10CaloJetCuts(unsigned int i_jet);
  bool passR2TrackJetCuts(unsigned int i_jet);
  int passBTagCut(const GbbCandidate& gbbcand, const GbbTupleAna::BTagType tagType);
  unsigned int getLeadingObjIndex(std::vector<float> *quantity);
  unsigned int getNthLeadingObjIndex(unsigned int n, std::vector<float> *quantity);
  int getAssocObjIndex(std::vector<int>* part_ind, int assoc_index);
  
  bool isCleanEvt(const float total_evt_weight);
  bool hasBadJet();
  bool passMuonSelection(unsigned int i_muon);
  int getTrigMatch();
  bool passTrigger(const float trigjet_pt, float& prescale, TString& trigger_passed);
  bool cutTriggerBias(const float gbbcandpt, const TString trigger_passed);

  GbbCandidate constructGbbCandidate();
  GbbCandidate constructGbbCandidateAlternative();
  GbbCandidate constructGbbCandidateInclusive();

  TString makeMuJetPlotName(GbbCandidate* gbbcand, const TString var);
  TString makeNonMuJetPlotName(GbbCandidate* gbbcand, const TString var);
  TString makeDiJetPlotName(GbbCandidate* gbbcand, const TString var);
  TString makeFatJetPlotName(GbbCandidate* gbbcand, const TString var);
  TString makeInclDiJetPlotName(GbbCandidate* gbbcand, const TString var);

  float getTrigJetWeight(int i_trig_jet, TString trigger_passed);
  float getTrigFatJetWeight(float trigfjpt, float trigfjeta,TString trigger_passed);
  
  void FillReweightInfo(int i_trig_jet, float event_weight,TString nametag="");
  void FillFatReweightInfo(float trigfat_pt,float trigfat_eta, float event_weight,TString nametag="");

  void FillMCStatsInfo(GbbCandidate* gbbcand, TString nametag="");
  void FillTriggerTurnOnHistograms(int i_trigjet, float event_weight);
  
  
  void FillTrackJetProperties(GbbCandidate* gbbcand, float event_weight,TString nametag="");
  void FillFatJetProperties(GbbCandidate* gbbcand, float event_weight,TString nametag="");
  void FillTemplates(GbbCandidate* gbbcand, float event_weight,TString nametag="");
  void FillAdvancedProperties(GbbCandidate* gbbcand, int i_trig_jet, float event_weight,TString nametag="");
  // Helper function for FillTemplates
  void FillSd0Plots(trkjetSd0Info muSd0Info, trkjetSd0Info nonmuSd0Info, float event_weight, std::function<TString (TString)> namingFunc);
  
  bool isLargeWeightEvent(int DSID,float evt_weight, float max_evt_weight);
  
  bool passAssocTrkSelection(unsigned int i_trk, unsigned int i_jet);
        trkjetSd0Info getTrkjetAssocSd0Info(unsigned int i_jet, bool doSmeared=false, TString sys="nominal", int n=3);
  
  float getd0(unsigned int i_trk, unsigned int i_jet, bool doSmeared=false, TString sys="nominal");
  float getSd0(unsigned int i_trk, unsigned int i_jet, bool doSmeared=false, TString sys="nominal");
  
  void getBtagSFWeights(float &btag_SF_nom, float &btag_SF_tot_up, float &btag_SF_tot_down);
  void getBtagSysWeights(float &btag_SF_tot_up, float &btag_SF_tot_down);                         
  
  void getSystematicsFlags(GbbCandidate *, bool &hasConversion, bool &hasHadMatInt, bool &hasLightLongLived, bool &hasNoTruthMu);

  std::vector<TString> SplitString(TString str, char delim);
  std::vector<float> SplitStringD(TString str, char delim);
  
  //bitstring cutflow (based on CxAODReader_VHbb)
  void updateFlag(unsigned long int &flag, const unsigned int cutPosition, const bool passCut);
  bool passSpecificCuts(const unsigned long int flag, const std::vector<unsigned int> &cuts);

  float getScaledFatPt(float fjpt){
    return fjpt*(0.9725+(1e-5)*fjpt);
    //    return fjpt*(0.95+((2.86e-5)*fjpt));
  }
};

namespace GbbCuts {
  
  enum Cuts {
    
    AllNtup=0,
    EventCleaning,
    TriggerJet,
    GbbCandidate,
    GoodSd0Tracks,
    DRTrigJetMuJet,
    MuNonMu1Btag,
    MuNonMu2Btags,
    MuNonMuAnti2Btags,
    MuNonMuUntagged
  };
  
}




#endif /* GBBTUPLEANA_H_ */
