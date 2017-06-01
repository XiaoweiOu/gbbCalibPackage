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
#include "FlavourFracCorrector.h"
#include "TrackSmearer.h"
#include "TEnv.h"
#include "TRandom3.h"

struct GbbCandidate{

  unsigned int fat_index;
  unsigned int muojet_index;
  unsigned int nonmuojet_index;
  float fat_pt;
  bool hasleading2trackjets;
};




class GbbTupleAna : public TupleAna {

public:
  GbbTupleAna();
	GbbTupleAna(TString& infilename, TString& treename, TString &outfilename, TString &configname);
	virtual ~GbbTupleAna();

	void ReadConfig(TString& config_path);

	void Loop();
	void Finalize();

	void setDebug(bool value){ m_Debug=value; }
	void setDoEvenOddTemplates(bool value){m_doEvenOddTemplates=value; }
	void setReweightHisto(TString filename,TString trigger_passed);

	void setDiTrkJetCategories(std::vector<TString>& vect){ m_ditrkjet_cat=vect; }   
        void setPtBins(std::vector<float>& muojet_vect,std::vector<float>& nonmuojet_vect){ m_muojet_pt_bins=muojet_vect; m_nonmuojet_pt_bins=nonmuojet_vect; }

 

private:
	HistogramService *m_HistogramService; //!
	TString m_Outputname; //! name of output file
	
	TString m_RunMode; // RunMode="Reweighting","FitInputs","PostFitPlots" "MCStatsTest"
	
	bool m_Debug;

	bool m_doJetPtReweighting;
	TString m_JetPtReweightFile;
	
	bool m_isNominal;
	TString m_SysVarName;
		
	float m_SumWeightTuple;
	int m_nevtTuple;
	
	std::map<TString,std::shared_ptr<TH2D>> m_reweightHistos;
	bool m_doEvenOddTemplates;

	bool m_doFlavFracCorrection; 
	
	TString m_FlavFracCorrectorFile;
	FlavourFracCorrector* m_FlavFracCorrector; 
	
	bool m_doTrackSmearing;
	
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
	
	
	bool Processgbb(int i_evt);
	
	bool passR4CaloJetCuts(unsigned int i_jet);
	bool passR10CaloJetCuts(unsigned int i_jet);
	bool passR2TrackJetCuts(unsigned int i_jet);
	unsigned int getLeadingObjIndex(std::vector<float> *quantity);
	int getAssocObjIndex(std::vector<int>* part_ind, int assoc_index);
	
	bool hasBadJet();
	bool passMuonSelection(unsigned int i_muon);

	GbbCandidate constructGbbCandidate();
	int getTruthType(int label);
	int getCategoryNumber(int muo_truth, int nonmuo_truth, bool doMerged=false);
	TString getPtLabel(float muojet_pt, float non_muojetpt);
	TString getFatjetPtLabel(float fatjet_pt);

	float getTrigJetWeight(int i_trig_jet, TString trigger_passed);
	
	void FillReweightInfo(int i_trig_jet, float event_weight,TString nametag="");
	void FillMCStatsInfo(GbbCandidate* gbbcand, TString nametag="");
	void FillTriggerTurnOnHistograms(int i_trigjet, float event_weight);
	
	
	void FillTrackJetProperties(GbbCandidate* gbbcand, float event_weight,TString nametag="");
	void FillFatJetProperties(GbbCandidate* gbbcand, float event_weight,TString nametag="");
	void FillTemplates(GbbCandidate* gbbcand, float event_weight,TString nametag="");
	void FillAdvancedProperties(GbbCandidate* gbbcand, int i_trig_jet, float event_weight,TString nametag="");
	
	bool isLargeWeightEvent(int DSID,float evt_weight, float max_evt_weight);
	
	float getTrkjetAssocTrkMaxSd0(unsigned int i_jet,bool doSmeared=false, TString sys="nominal");
	bool passAssocTrkSelection(unsigned int i_trk, unsigned int i_jet);
  
	float getSd0(unsigned int i_trk, unsigned int i_jet);
  float getSd0_smeared(unsigned int i_trk, unsigned int i_jet);
  float getSd0_smeared_sys_up(unsigned int i_trk, unsigned int i_jet);
  float getSd0_smeared_sys_down(unsigned int i_trk, unsigned int i_jet);
	
	float getAssocTrackOldSelectiond0AndErr(unsigned int i_jet,float & d0, float& err_d0);
	float getAssocTrackNewSelectiond0AndErr(unsigned int i_jet,float & d0, float& err_d0);
	
	void getBtagSFWeights(float &btag_SF_nom, float &btag_SF_tot_up, float &btag_SF_tot_down);
	void getBtagSysWeights(float &btag_SF_tot_up, float &btag_SF_tot_down);                         
	
	std::vector<TString> SplitString(TString str, char delim);
	std::vector<float> SplitStringD(TString str, char delim);
	
	//bitstring cutflow (based on CxAODReader_VHbb)
	void updateFlag(unsigned long int &flag, const unsigned int cutPosition, const bool passCut);
	unsigned int bitmask(const unsigned int cut, const std::vector<unsigned int> &excludeCuts);
	bool passSpecificCuts(const unsigned long int flag, const std::vector<unsigned int> &cuts);
	bool passAllCutsUpTo(const unsigned long int flag, const unsigned int cut, const std::vector<unsigned int> &excludeCuts);



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
  };
  
}




#endif /* GBBTUPLEANA_H_ */
