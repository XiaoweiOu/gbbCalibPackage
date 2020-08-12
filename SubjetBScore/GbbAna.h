#ifndef GbbAna_h
#define GbbAna_h

#include "State.h"
#include <vector>
using namespace std;

char GetFlavour(int truthType);
TString GetFlavourPair(int muJetTruth, int nonMuJetTruth);

class GbbAna {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           eve_num;
   Int_t           eve_run;
   Float_t         eve_mu;
   Float_t         eve_avg_mu;
   Int_t           eve_mc_num;
   Int_t           eve_mc_chan;
   Float_t         eve_mc_w;
   Float_t         eve_pu_w;
   vector<float>   *eve_pu_w_sys;
   Bool_t          eve_isMC;
   Int_t           eve_hasPV;
   Int_t           eve_isCleanEvent;
   Int_t           eve_passGRL;
   Float_t         eve_PVz;
   Float_t         eve_nVtx2Trks;
   Float_t         eve_pu_density;
   Float_t         eve_beam_sigma_x;
   Float_t         eve_beam_sigma_y;
   Float_t         eve_beam_sigma_xy;
   Int_t           eve_HLT_j150;
   Int_t           eve_HLT_j175;
   Int_t           eve_HLT_j200;
   Int_t           eve_HLT_j260;
   Int_t           eve_HLT_j300;
   Int_t           eve_HLT_j320;
   Int_t           eve_HLT_j360;
   Int_t           eve_HLT_j380;
   Int_t           eve_HLT_j400;
   Int_t           eve_HLT_j420;
   Int_t           eve_HLT_j260_a10_lcw_L1J75;
   Int_t           eve_HLT_j260_a10_lcw_subjes_L1J75;
   Int_t           eve_HLT_j260_a10t_lcw_jes_L1J75;
   Int_t           eve_HLT_j400_a10_lcw_L1J100;
   Int_t           eve_HLT_j420_a10_lcw_L1J100;
   Int_t           eve_HLT_j420_a10t_lcw_jes_35smcINF_L1J100;
   Int_t           eve_HLT_j420_a10t_lcw_jes_40smcINF_L1J100;
   Int_t           eve_HLT_j440_a10_lcw_subjes_L1J100;
   Int_t           eve_HLT_j440_a10t_lcw_jes_L1J100;
   Int_t           eve_HLT_j460_a10_lcw_subjes_L1J100;
   Int_t           eve_HLT_j460_a10t_lcw_jes_L1J100;
   Int_t           eve_HLT_2j330_a10t_lcw_jes_35smcINF_L1J100;
   Int_t           eve_HLT_2j330_a10t_lcw_jes_40smcINF_L1J100;
   Int_t           eve_HLT_mu20_iloose_L1MU15;
   Int_t           eve_HLT_mu24_ivarloose;
   Int_t           eve_HLT_mu24_ivarloose_L1MU15;
   Int_t           eve_HLT_mu26_ivarmedium;
   Float_t         eve_HLT_j150_ps;
   Float_t         eve_HLT_j175_ps;
   Float_t         eve_HLT_j200_ps;
   Float_t         eve_HLT_j260_ps;
   Float_t         eve_HLT_j300_ps;
   Float_t         eve_HLT_j320_ps;
   Float_t         eve_HLT_j380_ps;
   Float_t         eve_HLT_j400_ps;
   Float_t         eve_HLT_j420_ps;
   Float_t         eve_HLT_j260_a10_lcw_L1J75_ps;
   Float_t         eve_HLT_j260_a10_lcw_subjes_L1J75_ps;
   Float_t         eve_HLT_j260_a10t_lcw_jes_L1J75_ps;
   vector<float>   *jet_pt;
   vector<float>   *jet_phi;
   vector<float>   *jet_eta;
   vector<float>   *jet_E;
   vector<int>     *jet_ind;
   vector<float>   *jet_JVT;
   vector<float>   *jet_JvtSF;
   vector<int>     *jet_clean;
   vector<float>   *truthjet_pt;
   vector<float>   *truthjet_phi;
   vector<float>   *truthjet_eta;
   vector<float>   *truthjet_E;
   vector<vector<float> > *trkjet_assocTrk_pt;
   vector<vector<float> > *trkjet_assocTrk_phi;
   vector<vector<float> > *trkjet_assocTrk_eta;
   vector<vector<float> > *trkjet_assocTrk_d0;
   vector<vector<float> > *trkjet_assocTrk_z0;
   vector<vector<float> > *trkjet_assocTrk_theta;
   vector<vector<float> > *trkjet_assocTrk_vz;
   vector<vector<float> > *trkjet_assocTrk_phi0;
   vector<vector<float> > *trkjet_assocTrk_qoverp;
   vector<vector<float> > *trkjet_assocTrk_d0err;
   vector<vector<float> > *trkjet_assocTrk_z0err;
   vector<vector<float> > *trkjet_assocTrk_thetaerr;
   vector<vector<float> > *trkjet_assocTrk_phi0err;
   vector<vector<int> > *trkjet_assocTrk_nPixelHits;
   vector<vector<int> > *trkjet_assocTrk_nSCTHits;
   vector<vector<int> > *trkjet_assocTrk_nIBLHits;
   vector<vector<int> > *trkjet_assocTrk_expectIBLHit;
   vector<vector<int> > *trkjet_assocTrk_nBLayerHits;
   vector<vector<int> > *trkjet_assocTrk_expectBLayerHit;
   vector<vector<int> > *trkjet_assocTrk_isLoose;
   vector<vector<int> > *trkjet_assocTrk_isTightPrimary;
   vector<float>   *fat_E;
   vector<float>   *fat_pt;
   vector<float>   *fat_phi;
   vector<float>   *fat_eta;
   vector<int>     *fat_ind;
   vector<float>   *fat_jvt;
   vector<float>   *fat_tau21;
   vector<float>   *fat_tau32;
   vector<float>   *fat_C2;
   vector<float>   *fat_D2;
   vector<int>     *fat_isGbbJet;
   vector<float>   *fat_HbbScore;
   vector<float>   *fat_XbbScoreHiggs;
   vector<float>   *fat_XbbScoreTop;
   vector<float>   *fat_XbbScoreQCD;
   vector<float>   *fat_SubjetBScore_Higgs;
   vector<float>   *fat_SubjetBScore_Top;
   vector<float>   *fat_SubjetBScore_QCD;
   vector<int>     *fat_truth;
   vector<int>     *fat_BHad_n;
   vector<int>     *fat_CHad_n;
   vector<int>     *fat_BHad_n_GA;
   vector<int>     *fat_CHad_n_GA;
   vector<vector<int> > *fat_assocTrkjet_ind;
   vector<vector<int> > *fat_assocTrkjetDR_ind;
   vector<vector<float> > *fat_assocTrk_pt;
   vector<vector<float> > *fat_assocTrk_eta;
   vector<vector<float> > *fat_assocTrk_phi;
   vector<float>   *muo_pt;
   vector<float>   *muo_phi;
   vector<float>   *muo_eta;
   vector<float>   *muo_E;
   vector<int>     *muo_ind;
   vector<float>   *muo_z0;
   vector<float>   *muo_d0;
   vector<float>   *muo_d0sig;
   vector<float>   *muo_d0sigBL;
   vector<float>   *muo_z0sintheta;
   vector<int>     *muo_quality;
   vector<int>     *muo_acceptedTool;
   vector<float>   *muo_TTVAEffSF;
   vector<float>   *muo_LooseIDSF;
   vector<float>   *muo_MediumIDSF;
   vector<float>   *muo_TightIDSF;
   vector<int>     *muo_hasTruth;
   vector<float>   *trkjet_pt;
   vector<float>   *trkjet_phi;
   vector<float>   *trkjet_eta;
   vector<float>   *trkjet_E;
   vector<int>     *trkjet_ind;
   vector<float>   *trkjet_MV2c00;
   vector<float>   *trkjet_MV2c10;
   vector<float>   *trkjet_MV2c20;
   vector<float>   *trkjet_MVb;
   vector<float>   *trkjet_SV1_masssvx;
   vector<int>     *trkjet_truth;
   vector<int>     *trkjet_nTrks;
   vector<vector<float> > *trkjet_assocMuon_dR;
   vector<vector<int> > *trkjet_assocMuon_index;
   vector<int>     *trkjet_assocMuon_n;
   vector<int>     *trkjet_isMuonJet;
   vector<int>     *trkjet_isNonMuonJet;
   vector<int>     *trkjet_hasKShort;
   vector<int>     *trkjet_hasLambda;
   vector<int>     *trkjet_hasConversion;
   vector<int>     *trkjet_hasHadMatInt;
   vector<int>     *trkjet_BHad_n;
   vector<int>     *trkjet_CHad_n;
   vector<int>     *trkjet_BHad_n_GA;
   vector<int>     *trkjet_CHad_n_GA;
   vector<int>     *trkjet_BHad_pdgId;
   vector<float>   *trkjet_BHad_pt;
   vector<float>   *trkjet_BHad_eta;
   vector<float>   *trkjet_BHad_phi;
   vector<float>   *trkjet_BHad_e;
   vector<vector<int> > *trkjet_BHadDecays_pdgId;
   vector<vector<float> > *trkjet_BHadDecays_pt;
   vector<vector<float> > *trkjet_BHadDecays_eta;
   vector<vector<float> > *trkjet_BHadDecays_phi;
   vector<vector<float> > *trkjet_BHadDecays_e;
   vector<float>   *trigjet_a4_pt;
   vector<float>   *trigjet_a4_phi;
   vector<float>   *trigjet_a4_eta;
   vector<float>   *trigjet_a4IS_pt;
   vector<float>   *trigjet_a4IS_phi;
   vector<float>   *trigjet_a4IS_eta;
   vector<float>   *trigjet_a10_pt;
   vector<float>   *trigjet_a10_phi;
   vector<float>   *trigjet_a10_eta;
   vector<float>   *trigjet_a10t_pt;
   vector<float>   *trigjet_a10t_phi;
   vector<float>   *trigjet_a10t_eta;

   // List of branches
   TBranch        *b_eve_num;   //!
   TBranch        *b_eve_run;   //!
   TBranch        *b_eve_mu;   //!
   TBranch        *b_eve_avg_mu;   //!
   TBranch        *b_eve_mc_num;   //!
   TBranch        *b_eve_mc_chan;   //!
   TBranch        *b_eve_mc_w;   //!
   TBranch        *b_eve_pu_w;   //!
   TBranch        *b_eve_pu_w_sys;   //!
   TBranch        *b_eve_isMC;   //!
   TBranch        *b_eve_hasPV;   //!
   TBranch        *b_eve_isCleanEvent;   //!
   TBranch        *b_eve_passGRL;   //!
   TBranch        *b_eve_PVz;   //!
   TBranch        *b_eve_nVtx2Trks;   //!
   TBranch        *b_eve_pu_density;   //!
   TBranch        *b_eve_beam_sigma_x;   //!
   TBranch        *b_eve_beam_sigma_y;   //!
   TBranch        *b_eve_beam_sigma_xy;   //!
   TBranch        *b_eve_HLT_j150;   //!
   TBranch        *b_eve_HLT_j175;   //!
   TBranch        *b_eve_HLT_j200;   //!
   TBranch        *b_eve_HLT_j260;   //!
   TBranch        *b_eve_HLT_j300;   //!
   TBranch        *b_eve_HLT_j320;   //!
   TBranch        *b_eve_HLT_j360;   //!
   TBranch        *b_eve_HLT_j380;   //!
   TBranch        *b_eve_HLT_j400;   //!
   TBranch        *b_eve_HLT_j420;   //!
   TBranch        *b_eve_HLT_j260_a10_lcw_L1J75;   //!
   TBranch        *b_eve_HLT_j260_a10_lcw_subjes_L1J75;   //!
   TBranch        *b_eve_HLT_j260_a10t_lcw_jes_L1J75;   //!
   TBranch        *b_eve_HLT_j400_a10_lcw_L1J100;   //!
   TBranch        *b_eve_HLT_j420_a10_lcw_L1J100;   //!
   TBranch        *b_eve_HLT_j420_a10t_lcw_jes_35smcINF_L1J100;   //!
   TBranch        *b_eve_HLT_j420_a10t_lcw_jes_40smcINF_L1J100;   //!
   TBranch        *b_eve_HLT_j440_a10_lcw_subjes_L1J100;   //!
   TBranch        *b_eve_HLT_j440_a10t_lcw_jes_L1J100;   //!
   TBranch        *b_eve_HLT_j460_a10_lcw_subjes_L1J100;   //!
   TBranch        *b_eve_HLT_j460_a10t_lcw_jes_L1J100;   //!
   TBranch        *b_eve_HLT_2j330_a10t_lcw_jes_35smcINF_L1J100;   //!
   TBranch        *b_eve_HLT_2j330_a10t_lcw_jes_40smcINF_L1J100;   //!
   TBranch        *b_eve_HLT_mu20_iloose_L1MU15;   //!
   TBranch        *b_eve_HLT_mu24_ivarloose;   //!
   TBranch        *b_eve_HLT_mu24_ivarloose_L1MU15;   //!
   TBranch        *b_eve_HLT_mu26_ivarmedium;   //!
   TBranch        *b_eve_HLT_j150_ps;   //!
   TBranch        *b_eve_HLT_j175_ps;   //!
   TBranch        *b_eve_HLT_j200_ps;   //!
   TBranch        *b_eve_HLT_j260_ps;   //!
   TBranch        *b_eve_HLT_j300_ps;   //!
   TBranch        *b_eve_HLT_j320_ps;   //!
   TBranch        *b_eve_HLT_j380_ps;   //!
   TBranch        *b_eve_HLT_j400_ps;   //!
   TBranch        *b_eve_HLT_j420_ps;   //!
   TBranch        *b_eve_HLT_j260_a10_lcw_L1J75_ps;   //!
   TBranch        *b_eve_HLT_j260_a10_lcw_subjes_L1J75_ps;   //!
   TBranch        *b_eve_HLT_j260_a10t_lcw_jes_L1J75_ps;   //!
   TBranch        *b_jet_pt;   //!
   TBranch        *b_jet_phi;   //!
   TBranch        *b_jet_eta;   //!
   TBranch        *b_jet_E;   //!
   TBranch        *b_jet_ind;   //!
   TBranch        *b_jet_JVT;   //!
   TBranch        *b_jet_JvtSF;   //!
   TBranch        *b_jet_clean;   //!
   TBranch        *b_truthjet_pt;   //!
   TBranch        *b_truthjet_phi;   //!
   TBranch        *b_truthjet_eta;   //!
   TBranch        *b_truthjet_E;   //!
   TBranch        *b_trkjet_assocTrk_pt;   //!
   TBranch        *b_trkjet_assocTrk_phi;   //!
   TBranch        *b_trkjet_assocTrk_eta;   //!
   TBranch        *b_trkjet_assocTrk_d0;   //!
   TBranch        *b_trkjet_assocTrk_z0;   //!
   TBranch        *b_trkjet_assocTrk_theta;   //!
   TBranch        *b_trkjet_assocTrk_vz;   //!
   TBranch        *b_trkjet_assocTrk_phi0;   //!
   TBranch        *b_trkjet_assocTrk_qoverp;   //!
   TBranch        *b_trkjet_assocTrk_d0err;   //!
   TBranch        *b_trkjet_assocTrk_z0err;   //!
   TBranch        *b_trkjet_assocTrk_thetaerr;   //!
   TBranch        *b_trkjet_assocTrk_phi0err;   //!
   TBranch        *b_trkjet_assocTrk_nPixelHits;   //!
   TBranch        *b_trkjet_assocTrk_nSCTHits;   //!
   TBranch        *b_trkjet_assocTrk_nIBLHits;   //!
   TBranch        *b_trkjet_assocTrk_expectIBLHit;   //!
   TBranch        *b_trkjet_assocTrk_nBLayerHits;   //!
   TBranch        *b_trkjet_assocTrk_expectBLayerHit;   //!
   TBranch        *b_trkjet_assocTrk_isLoose;   //!
   TBranch        *b_trkjet_assocTrk_isTightPrimary;   //!
   TBranch        *b_fat_E;   //!
   TBranch        *b_fat_pt;   //!
   TBranch        *b_fat_phi;   //!
   TBranch        *b_fat_eta;   //!
   TBranch        *b_fat_ind;   //!
   TBranch        *b_fat_jvt;   //!
   TBranch        *b_fat_tau21;   //!
   TBranch        *b_fat_tau32;   //!
   TBranch        *b_fat_C2;   //!
   TBranch        *b_fat_D2;   //!
   TBranch        *b_fat_isGbbJet;   //!
   TBranch        *b_fat_HbbScore;   //!
   TBranch        *b_fat_XbbScoreHiggs;   //!
   TBranch        *b_fat_XbbScoreTop;   //!
   TBranch        *b_fat_XbbScoreQCD;   //!
   TBranch        *b_fat_SubjetBScore_Higgs;   //!
   TBranch        *b_fat_SubjetBScore_Top;   //!
   TBranch        *b_fat_SubjetBScore_QCD;   //!
   TBranch        *b_fat_truth;   //!
   TBranch        *b_fat_BHad_n;   //!
   TBranch        *b_fat_CHad_n;   //!
   TBranch        *b_fat_BHad_n_GA;   //!
   TBranch        *b_fat_CHad_n_GA;   //!
   TBranch        *b_fat_assocTrkjet_ind;   //!
   TBranch        *b_fat_assocTrkjetDR_ind;   //!
   TBranch        *b_fat_assocTrk_pt;   //!
   TBranch        *b_fat_assocTrk_eta;   //!
   TBranch        *b_fat_assocTrk_phi;   //!
   TBranch        *b_muo_pt;   //!
   TBranch        *b_muo_phi;   //!
   TBranch        *b_muo_eta;   //!
   TBranch        *b_muo_E;   //!
   TBranch        *b_muo_ind;   //!
   TBranch        *b_muo_z0;   //!
   TBranch        *b_muo_d0;   //!
   TBranch        *b_muo_d0sig;   //!
   TBranch        *b_muo_d0sigBL;   //!
   TBranch        *b_muo_z0sintheta;   //!
   TBranch        *b_muo_quality;   //!
   TBranch        *b_muo_acceptedTool;   //!
   TBranch        *b_muo_TTVAEffSF;   //!
   TBranch        *b_muo_LooseIDSF;   //!
   TBranch        *b_muo_MediumIDSF;   //!
   TBranch        *b_muo_TightIDSF;   //!
   TBranch        *b_muo_hasTruth;   //!
   TBranch        *b_trkjet_pt;   //!
   TBranch        *b_trkjet_phi;   //!
   TBranch        *b_trkjet_eta;   //!
   TBranch        *b_trkjet_E;   //!
   TBranch        *b_trkjet_ind;   //!
   TBranch        *b_trkjet_MV2c00;   //!
   TBranch        *b_trkjet_MV2c10;   //!
   TBranch        *b_trkjet_MV2c20;   //!
   TBranch        *b_trkjet_MVb;   //!
   TBranch        *b_trkjet_SV1_masssvx;   //!
   TBranch        *b_trkjet_truth;   //!
   TBranch        *b_trkjet_nTrks;   //!
   TBranch        *b_trkjet_assocMuon_dR;   //!
   TBranch        *b_trkjet_assocMuon_index;   //!
   TBranch        *b_trkjet_assocMuon_n;   //!
   TBranch        *b_trkjet_isMuonJet;   //!
   TBranch        *b_trkjet_isNonMuonJet;   //!
   TBranch        *b_trkjet_hasKShort;   //!
   TBranch        *b_trkjet_hasLambda;   //!
   TBranch        *b_trkjet_hasConversion;   //!
   TBranch        *b_trkjet_hasHadMatInt;   //!
   TBranch        *b_trkjet_BHad_n;   //!
   TBranch        *b_trkjet_CHad_n;   //!
   TBranch        *b_trkjet_BHad_n_GA;   //!
   TBranch        *b_trkjet_CHad_n_GA;   //!
   TBranch        *b_trkjet_BHad_pdgId;   //!
   TBranch        *b_trkjet_BHad_pt;   //!
   TBranch        *b_trkjet_BHad_eta;   //!
   TBranch        *b_trkjet_BHad_phi;   //!
   TBranch        *b_trkjet_BHad_e;   //!
   TBranch        *b_trkjet_BHadDecays_pdgId;   //!
   TBranch        *b_trkjet_BHadDecays_pt;   //!
   TBranch        *b_trkjet_BHadDecays_eta;   //!
   TBranch        *b_trkjet_BHadDecays_phi;   //!
   TBranch        *b_trkjet_BHadDecays_e;   //!
   TBranch        *b_trigjet_a4_pt;   //!
   TBranch        *b_trigjet_a4_phi;   //!
   TBranch        *b_trigjet_a4_eta;   //!
   TBranch        *b_trigjet_a4IS_pt;   //!
   TBranch        *b_trigjet_a4IS_phi;   //!
   TBranch        *b_trigjet_a4IS_eta;   //!
   TBranch        *b_trigjet_a10_pt;   //!
   TBranch        *b_trigjet_a10_phi;   //!
   TBranch        *b_trigjet_a10_eta;   //!
   TBranch        *b_trigjet_a10t_pt;   //!
   TBranch        *b_trigjet_a10t_phi;   //!
   TBranch        *b_trigjet_a10t_eta;   //!

   GbbAna(TString TypeName);
   virtual ~GbbAna();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TString FileName,int count);
   virtual void     Loop();
   virtual void     EndFile(int count);
   virtual void     Finish(int count);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);

   MakeHist *myHist;
   bool isMC;
   bool isdata;
   bool isRead=true;
};

#endif

#ifdef GbbAna_cxx
GbbAna::GbbAna(TString TypeName) : fChain(0) 
{
   if(TypeName=="MC")isMC=true;
   else if(TypeName=="data")isdata=true;
   else {cout<<"ERROR: ntuple type is prohibited."<<endl;exit(0);}

   //initial histogram!
   myHist = new MakeHist();
   TString HistName=TypeName+"_result.root";
   myHist->BookHist(HistName);
}

GbbAna::~GbbAna()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t GbbAna::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t GbbAna::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void GbbAna::EndFile(int count)
{
 if(isRead==false)
 {
  return;
 }

 //output:free the current tree
 cout<<"Running: Free Rootfile"<<count<<endl;

 if(!fChain)
 {
  cout<<"ERROR:"<<count<<" file doesn't have tree."<<endl;
  return;
 }

 delete fChain->GetCurrentFile();
}

void GbbAna::Init(TString FileName,int count)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   //tips for begin reading
   cout<<"Running:Starting Rootfile"<<count<<endl;

   TTree *tree=NULL;
   TFile *f = new TFile(FileName);
   f->GetObject("FlavourTagging_Nominal",tree);

   // Set object pointer
   eve_pu_w_sys = 0;
   jet_pt = 0;
   jet_phi = 0;
   jet_eta = 0;
   jet_E = 0;
   jet_ind = 0;
   jet_JVT = 0;
   jet_JvtSF = 0;
   jet_clean = 0;
   truthjet_pt = 0;
   truthjet_phi = 0;
   truthjet_eta = 0;
   truthjet_E = 0;
   trkjet_assocTrk_pt = 0;
   trkjet_assocTrk_phi = 0;
   trkjet_assocTrk_eta = 0;
   trkjet_assocTrk_d0 = 0;
   trkjet_assocTrk_z0 = 0;
   trkjet_assocTrk_theta = 0;
   trkjet_assocTrk_vz = 0;
   trkjet_assocTrk_phi0 = 0;
   trkjet_assocTrk_qoverp = 0;
   trkjet_assocTrk_d0err = 0;
   trkjet_assocTrk_z0err = 0;
   trkjet_assocTrk_thetaerr = 0;
   trkjet_assocTrk_phi0err = 0;
   trkjet_assocTrk_nPixelHits = 0;
   trkjet_assocTrk_nSCTHits = 0;
   trkjet_assocTrk_nIBLHits = 0;
   trkjet_assocTrk_expectIBLHit = 0;
   trkjet_assocTrk_nBLayerHits = 0;
   trkjet_assocTrk_expectBLayerHit = 0;
   trkjet_assocTrk_isLoose = 0;
   trkjet_assocTrk_isTightPrimary = 0;
   fat_E = 0;
   fat_pt = 0;
   fat_phi = 0;
   fat_eta = 0;
   fat_ind = 0;
   fat_jvt = 0;
   fat_tau21 = 0;
   fat_tau32 = 0;
   fat_C2 = 0;
   fat_D2 = 0;
   fat_isGbbJet = 0;
   fat_HbbScore = 0;
   fat_XbbScoreHiggs = 0;
   fat_XbbScoreTop = 0;
   fat_XbbScoreQCD = 0;
   fat_SubjetBScore_Higgs = 0;
   fat_SubjetBScore_Top = 0;
   fat_SubjetBScore_QCD = 0;
   fat_truth = 0;
   fat_BHad_n = 0;
   fat_CHad_n = 0;
   fat_BHad_n_GA = 0;
   fat_CHad_n_GA = 0;
   fat_assocTrkjet_ind = 0;
   fat_assocTrkjetDR_ind = 0;
   fat_assocTrk_pt = 0;
   fat_assocTrk_eta = 0;
   fat_assocTrk_phi = 0;
   muo_pt = 0;
   muo_phi = 0;
   muo_eta = 0;
   muo_E = 0;
   muo_ind = 0;
   muo_z0 = 0;
   muo_d0 = 0;
   muo_d0sig = 0;
   muo_d0sigBL = 0;
   muo_z0sintheta = 0;
   muo_quality = 0;
   muo_acceptedTool = 0;
   muo_TTVAEffSF = 0;
   muo_LooseIDSF = 0;
   muo_MediumIDSF = 0;
   muo_TightIDSF = 0;
   muo_hasTruth = 0;
   trkjet_pt = 0;
   trkjet_phi = 0;
   trkjet_eta = 0;
   trkjet_E = 0;
   trkjet_ind = 0;
   trkjet_MV2c00 = 0;
   trkjet_MV2c10 = 0;
   trkjet_MV2c20 = 0;
   trkjet_MVb = 0;
   trkjet_SV1_masssvx = 0;
   trkjet_truth = 0;
   trkjet_nTrks = 0;
   trkjet_assocMuon_dR = 0;
   trkjet_assocMuon_index = 0;
   trkjet_assocMuon_n = 0;
   trkjet_isMuonJet = 0;
   trkjet_isNonMuonJet = 0;
   trkjet_hasKShort = 0;
   trkjet_hasLambda = 0;
   trkjet_hasConversion = 0;
   trkjet_hasHadMatInt = 0;
   trkjet_BHad_n = 0;
   trkjet_CHad_n = 0;
   trkjet_BHad_n_GA = 0;
   trkjet_CHad_n_GA = 0;
   trkjet_BHad_pdgId = 0;
   trkjet_BHad_pt = 0;
   trkjet_BHad_eta = 0;
   trkjet_BHad_phi = 0;
   trkjet_BHad_e = 0;
   trkjet_BHadDecays_pdgId = 0;
   trkjet_BHadDecays_pt = 0;
   trkjet_BHadDecays_eta = 0;
   trkjet_BHadDecays_phi = 0;
   trkjet_BHadDecays_e = 0;
   trigjet_a4_pt = 0;
   trigjet_a4_phi = 0;
   trigjet_a4_eta = 0;
   trigjet_a4IS_pt = 0;
   trigjet_a4IS_phi = 0;
   trigjet_a4IS_eta = 0;
   trigjet_a10_pt = 0;
   trigjet_a10_phi = 0;
   trigjet_a10_eta = 0;
   trigjet_a10t_pt = 0;
   trigjet_a10t_phi = 0;
   trigjet_a10t_eta = 0;
   // Set branch addresses and branch pointers
   if (!tree || tree->GetEntries()==0)
   {
    isRead = false;
    cout<<"WARNING:this file has no FlavourTagging_Nominal tree, skip to next!"<<endl;
    return;
   }
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("eve_num", &eve_num, &b_eve_num);
   fChain->SetBranchAddress("eve_run", &eve_run, &b_eve_run);
   fChain->SetBranchAddress("eve_mu", &eve_mu, &b_eve_mu);
   fChain->SetBranchAddress("eve_avg_mu", &eve_avg_mu, &b_eve_avg_mu);
   fChain->SetBranchAddress("eve_mc_num", &eve_mc_num, &b_eve_mc_num);
   fChain->SetBranchAddress("eve_mc_chan", &eve_mc_chan, &b_eve_mc_chan);
   fChain->SetBranchAddress("eve_mc_w", &eve_mc_w, &b_eve_mc_w);
   fChain->SetBranchAddress("eve_pu_w", &eve_pu_w, &b_eve_pu_w);
   fChain->SetBranchAddress("eve_pu_w_sys", &eve_pu_w_sys, &b_eve_pu_w_sys);
   fChain->SetBranchAddress("eve_isMC", &eve_isMC, &b_eve_isMC);
   fChain->SetBranchAddress("eve_hasPV", &eve_hasPV, &b_eve_hasPV);
   fChain->SetBranchAddress("eve_isCleanEvent", &eve_isCleanEvent, &b_eve_isCleanEvent);
   fChain->SetBranchAddress("eve_passGRL", &eve_passGRL, &b_eve_passGRL);
   fChain->SetBranchAddress("eve_PVz", &eve_PVz, &b_eve_PVz);
   fChain->SetBranchAddress("eve_nVtx2Trks", &eve_nVtx2Trks, &b_eve_nVtx2Trks);
   fChain->SetBranchAddress("eve_pu_density", &eve_pu_density, &b_eve_pu_density);
   fChain->SetBranchAddress("eve_beam_sigma_x", &eve_beam_sigma_x, &b_eve_beam_sigma_x);
   fChain->SetBranchAddress("eve_beam_sigma_y", &eve_beam_sigma_y, &b_eve_beam_sigma_y);
   fChain->SetBranchAddress("eve_beam_sigma_xy", &eve_beam_sigma_xy, &b_eve_beam_sigma_xy);
   fChain->SetBranchAddress("eve_HLT_j150", &eve_HLT_j150, &b_eve_HLT_j150);
   fChain->SetBranchAddress("eve_HLT_j175", &eve_HLT_j175, &b_eve_HLT_j175);
   fChain->SetBranchAddress("eve_HLT_j200", &eve_HLT_j200, &b_eve_HLT_j200);
   fChain->SetBranchAddress("eve_HLT_j260", &eve_HLT_j260, &b_eve_HLT_j260);
   fChain->SetBranchAddress("eve_HLT_j300", &eve_HLT_j300, &b_eve_HLT_j300);
   fChain->SetBranchAddress("eve_HLT_j320", &eve_HLT_j320, &b_eve_HLT_j320);
   fChain->SetBranchAddress("eve_HLT_j360", &eve_HLT_j360, &b_eve_HLT_j360);
   fChain->SetBranchAddress("eve_HLT_j380", &eve_HLT_j380, &b_eve_HLT_j380);
   fChain->SetBranchAddress("eve_HLT_j400", &eve_HLT_j400, &b_eve_HLT_j400);
   fChain->SetBranchAddress("eve_HLT_j420", &eve_HLT_j420, &b_eve_HLT_j420);
   fChain->SetBranchAddress("eve_HLT_j260_a10_lcw_L1J75", &eve_HLT_j260_a10_lcw_L1J75, &b_eve_HLT_j260_a10_lcw_L1J75);
   fChain->SetBranchAddress("eve_HLT_j260_a10_lcw_subjes_L1J75", &eve_HLT_j260_a10_lcw_subjes_L1J75, &b_eve_HLT_j260_a10_lcw_subjes_L1J75);
   fChain->SetBranchAddress("eve_HLT_j260_a10t_lcw_jes_L1J75", &eve_HLT_j260_a10t_lcw_jes_L1J75, &b_eve_HLT_j260_a10t_lcw_jes_L1J75);
   fChain->SetBranchAddress("eve_HLT_j400_a10_lcw_L1J100", &eve_HLT_j400_a10_lcw_L1J100, &b_eve_HLT_j400_a10_lcw_L1J100);
   fChain->SetBranchAddress("eve_HLT_j420_a10_lcw_L1J100", &eve_HLT_j420_a10_lcw_L1J100, &b_eve_HLT_j420_a10_lcw_L1J100);
   fChain->SetBranchAddress("eve_HLT_j420_a10t_lcw_jes_35smcINF_L1J100", &eve_HLT_j420_a10t_lcw_jes_35smcINF_L1J100, &b_eve_HLT_j420_a10t_lcw_jes_35smcINF_L1J100);
   fChain->SetBranchAddress("eve_HLT_j420_a10t_lcw_jes_40smcINF_L1J100", &eve_HLT_j420_a10t_lcw_jes_40smcINF_L1J100, &b_eve_HLT_j420_a10t_lcw_jes_40smcINF_L1J100);
   fChain->SetBranchAddress("eve_HLT_j440_a10_lcw_subjes_L1J100", &eve_HLT_j440_a10_lcw_subjes_L1J100, &b_eve_HLT_j440_a10_lcw_subjes_L1J100);
   fChain->SetBranchAddress("eve_HLT_j440_a10t_lcw_jes_L1J100", &eve_HLT_j440_a10t_lcw_jes_L1J100, &b_eve_HLT_j440_a10t_lcw_jes_L1J100);
   fChain->SetBranchAddress("eve_HLT_j460_a10_lcw_subjes_L1J100", &eve_HLT_j460_a10_lcw_subjes_L1J100, &b_eve_HLT_j460_a10_lcw_subjes_L1J100);
   fChain->SetBranchAddress("eve_HLT_j460_a10t_lcw_jes_L1J100", &eve_HLT_j460_a10t_lcw_jes_L1J100, &b_eve_HLT_j460_a10t_lcw_jes_L1J100);
   fChain->SetBranchAddress("eve_HLT_2j330_a10t_lcw_jes_35smcINF_L1J100", &eve_HLT_2j330_a10t_lcw_jes_35smcINF_L1J100, &b_eve_HLT_2j330_a10t_lcw_jes_35smcINF_L1J100);
   fChain->SetBranchAddress("eve_HLT_2j330_a10t_lcw_jes_40smcINF_L1J100", &eve_HLT_2j330_a10t_lcw_jes_40smcINF_L1J100, &b_eve_HLT_2j330_a10t_lcw_jes_40smcINF_L1J100);
   fChain->SetBranchAddress("eve_HLT_mu20_iloose_L1MU15", &eve_HLT_mu20_iloose_L1MU15, &b_eve_HLT_mu20_iloose_L1MU15);
   fChain->SetBranchAddress("eve_HLT_mu24_ivarloose", &eve_HLT_mu24_ivarloose, &b_eve_HLT_mu24_ivarloose);
   fChain->SetBranchAddress("eve_HLT_mu24_ivarloose_L1MU15", &eve_HLT_mu24_ivarloose_L1MU15, &b_eve_HLT_mu24_ivarloose_L1MU15);
   fChain->SetBranchAddress("eve_HLT_mu26_ivarmedium", &eve_HLT_mu26_ivarmedium, &b_eve_HLT_mu26_ivarmedium);
   fChain->SetBranchAddress("eve_HLT_j150_ps", &eve_HLT_j150_ps, &b_eve_HLT_j150_ps);
   fChain->SetBranchAddress("eve_HLT_j175_ps", &eve_HLT_j175_ps, &b_eve_HLT_j175_ps);
   fChain->SetBranchAddress("eve_HLT_j200_ps", &eve_HLT_j200_ps, &b_eve_HLT_j200_ps);
   fChain->SetBranchAddress("eve_HLT_j260_ps", &eve_HLT_j260_ps, &b_eve_HLT_j260_ps);
   fChain->SetBranchAddress("eve_HLT_j300_ps", &eve_HLT_j300_ps, &b_eve_HLT_j300_ps);
   fChain->SetBranchAddress("eve_HLT_j320_ps", &eve_HLT_j320_ps, &b_eve_HLT_j320_ps);
   fChain->SetBranchAddress("eve_HLT_j380_ps", &eve_HLT_j380_ps, &b_eve_HLT_j380_ps);
   fChain->SetBranchAddress("eve_HLT_j400_ps", &eve_HLT_j400_ps, &b_eve_HLT_j400_ps);
   fChain->SetBranchAddress("eve_HLT_j420_ps", &eve_HLT_j420_ps, &b_eve_HLT_j420_ps);
   fChain->SetBranchAddress("eve_HLT_j260_a10_lcw_L1J75_ps", &eve_HLT_j260_a10_lcw_L1J75_ps, &b_eve_HLT_j260_a10_lcw_L1J75_ps);
   fChain->SetBranchAddress("eve_HLT_j260_a10_lcw_subjes_L1J75_ps", &eve_HLT_j260_a10_lcw_subjes_L1J75_ps, &b_eve_HLT_j260_a10_lcw_subjes_L1J75_ps);
   fChain->SetBranchAddress("eve_HLT_j260_a10t_lcw_jes_L1J75_ps", &eve_HLT_j260_a10t_lcw_jes_L1J75_ps, &b_eve_HLT_j260_a10t_lcw_jes_L1J75_ps);
   fChain->SetBranchAddress("jet_pt", &jet_pt, &b_jet_pt);
   fChain->SetBranchAddress("jet_phi", &jet_phi, &b_jet_phi);
   fChain->SetBranchAddress("jet_eta", &jet_eta, &b_jet_eta);
   fChain->SetBranchAddress("jet_E", &jet_E, &b_jet_E);
   fChain->SetBranchAddress("jet_ind", &jet_ind, &b_jet_ind);
   fChain->SetBranchAddress("jet_JVT", &jet_JVT, &b_jet_JVT);
   fChain->SetBranchAddress("jet_JvtSF", &jet_JvtSF, &b_jet_JvtSF);
   fChain->SetBranchAddress("jet_clean", &jet_clean, &b_jet_clean);
   fChain->SetBranchAddress("truthjet_pt", &truthjet_pt, &b_truthjet_pt);
   fChain->SetBranchAddress("truthjet_phi", &truthjet_phi, &b_truthjet_phi);
   fChain->SetBranchAddress("truthjet_eta", &truthjet_eta, &b_truthjet_eta);
   fChain->SetBranchAddress("truthjet_E", &truthjet_E, &b_truthjet_E);
   fChain->SetBranchAddress("trkjet_assocTrk_pt", &trkjet_assocTrk_pt, &b_trkjet_assocTrk_pt);
   fChain->SetBranchAddress("trkjet_assocTrk_phi", &trkjet_assocTrk_phi, &b_trkjet_assocTrk_phi);
   fChain->SetBranchAddress("trkjet_assocTrk_eta", &trkjet_assocTrk_eta, &b_trkjet_assocTrk_eta);
   fChain->SetBranchAddress("trkjet_assocTrk_d0", &trkjet_assocTrk_d0, &b_trkjet_assocTrk_d0);
   fChain->SetBranchAddress("trkjet_assocTrk_z0", &trkjet_assocTrk_z0, &b_trkjet_assocTrk_z0);
   fChain->SetBranchAddress("trkjet_assocTrk_theta", &trkjet_assocTrk_theta, &b_trkjet_assocTrk_theta);
   fChain->SetBranchAddress("trkjet_assocTrk_vz", &trkjet_assocTrk_vz, &b_trkjet_assocTrk_vz);
   fChain->SetBranchAddress("trkjet_assocTrk_phi0", &trkjet_assocTrk_phi0, &b_trkjet_assocTrk_phi0);
   fChain->SetBranchAddress("trkjet_assocTrk_qoverp", &trkjet_assocTrk_qoverp, &b_trkjet_assocTrk_qoverp);
   fChain->SetBranchAddress("trkjet_assocTrk_d0err", &trkjet_assocTrk_d0err, &b_trkjet_assocTrk_d0err);
   fChain->SetBranchAddress("trkjet_assocTrk_z0err", &trkjet_assocTrk_z0err, &b_trkjet_assocTrk_z0err);
   fChain->SetBranchAddress("trkjet_assocTrk_thetaerr", &trkjet_assocTrk_thetaerr, &b_trkjet_assocTrk_thetaerr);
   fChain->SetBranchAddress("trkjet_assocTrk_phi0err", &trkjet_assocTrk_phi0err, &b_trkjet_assocTrk_phi0err);
   fChain->SetBranchAddress("trkjet_assocTrk_nPixelHits", &trkjet_assocTrk_nPixelHits, &b_trkjet_assocTrk_nPixelHits);
   fChain->SetBranchAddress("trkjet_assocTrk_nSCTHits", &trkjet_assocTrk_nSCTHits, &b_trkjet_assocTrk_nSCTHits);
   fChain->SetBranchAddress("trkjet_assocTrk_nIBLHits", &trkjet_assocTrk_nIBLHits, &b_trkjet_assocTrk_nIBLHits);
   fChain->SetBranchAddress("trkjet_assocTrk_expectIBLHit", &trkjet_assocTrk_expectIBLHit, &b_trkjet_assocTrk_expectIBLHit);
   fChain->SetBranchAddress("trkjet_assocTrk_nBLayerHits", &trkjet_assocTrk_nBLayerHits, &b_trkjet_assocTrk_nBLayerHits);
   fChain->SetBranchAddress("trkjet_assocTrk_expectBLayerHit", &trkjet_assocTrk_expectBLayerHit, &b_trkjet_assocTrk_expectBLayerHit);
   fChain->SetBranchAddress("trkjet_assocTrk_isLoose", &trkjet_assocTrk_isLoose, &b_trkjet_assocTrk_isLoose);
   fChain->SetBranchAddress("trkjet_assocTrk_isTightPrimary", &trkjet_assocTrk_isTightPrimary, &b_trkjet_assocTrk_isTightPrimary);
   fChain->SetBranchAddress("fat_E", &fat_E, &b_fat_E);
   fChain->SetBranchAddress("fat_pt", &fat_pt, &b_fat_pt);
   fChain->SetBranchAddress("fat_phi", &fat_phi, &b_fat_phi);
   fChain->SetBranchAddress("fat_eta", &fat_eta, &b_fat_eta);
   fChain->SetBranchAddress("fat_ind", &fat_ind, &b_fat_ind);
   fChain->SetBranchAddress("fat_jvt", &fat_jvt, &b_fat_jvt);
   fChain->SetBranchAddress("fat_tau21", &fat_tau21, &b_fat_tau21);
   fChain->SetBranchAddress("fat_tau32", &fat_tau32, &b_fat_tau32);
   fChain->SetBranchAddress("fat_C2", &fat_C2, &b_fat_C2);
   fChain->SetBranchAddress("fat_D2", &fat_D2, &b_fat_D2);
   fChain->SetBranchAddress("fat_isGbbJet", &fat_isGbbJet, &b_fat_isGbbJet);
   fChain->SetBranchAddress("fat_HbbScore", &fat_HbbScore, &b_fat_HbbScore);
   fChain->SetBranchAddress("fat_XbbScoreHiggs", &fat_XbbScoreHiggs, &b_fat_XbbScoreHiggs);
   fChain->SetBranchAddress("fat_XbbScoreTop", &fat_XbbScoreTop, &b_fat_XbbScoreTop);
   fChain->SetBranchAddress("fat_XbbScoreQCD", &fat_XbbScoreQCD, &b_fat_XbbScoreQCD);
   fChain->SetBranchAddress("fat_SubjetBScore_Higgs", &fat_SubjetBScore_Higgs, &b_fat_SubjetBScore_Higgs);
   fChain->SetBranchAddress("fat_SubjetBScore_Top", &fat_SubjetBScore_Top, &b_fat_SubjetBScore_Top);
   fChain->SetBranchAddress("fat_SubjetBScore_QCD", &fat_SubjetBScore_QCD, &b_fat_SubjetBScore_QCD);
   fChain->SetBranchAddress("fat_truth", &fat_truth, &b_fat_truth);
   fChain->SetBranchAddress("fat_BHad_n", &fat_BHad_n, &b_fat_BHad_n);
   fChain->SetBranchAddress("fat_CHad_n", &fat_CHad_n, &b_fat_CHad_n);
   fChain->SetBranchAddress("fat_BHad_n_GA", &fat_BHad_n_GA, &b_fat_BHad_n_GA);
   fChain->SetBranchAddress("fat_CHad_n_GA", &fat_CHad_n_GA, &b_fat_CHad_n_GA);
   fChain->SetBranchAddress("fat_assocTrkjet_ind", &fat_assocTrkjet_ind, &b_fat_assocTrkjet_ind);
   fChain->SetBranchAddress("fat_assocTrkjetDR_ind", &fat_assocTrkjetDR_ind, &b_fat_assocTrkjetDR_ind);
   fChain->SetBranchAddress("fat_assocTrk_pt", &fat_assocTrk_pt, &b_fat_assocTrk_pt);
   fChain->SetBranchAddress("fat_assocTrk_eta", &fat_assocTrk_eta, &b_fat_assocTrk_eta);
   fChain->SetBranchAddress("fat_assocTrk_phi", &fat_assocTrk_phi, &b_fat_assocTrk_phi);
   fChain->SetBranchAddress("muo_pt", &muo_pt, &b_muo_pt);
   fChain->SetBranchAddress("muo_phi", &muo_phi, &b_muo_phi);
   fChain->SetBranchAddress("muo_eta", &muo_eta, &b_muo_eta);
   fChain->SetBranchAddress("muo_E", &muo_E, &b_muo_E);
   fChain->SetBranchAddress("muo_ind", &muo_ind, &b_muo_ind);
   fChain->SetBranchAddress("muo_z0", &muo_z0, &b_muo_z0);
   fChain->SetBranchAddress("muo_d0", &muo_d0, &b_muo_d0);
   fChain->SetBranchAddress("muo_d0sig", &muo_d0sig, &b_muo_d0sig);
   fChain->SetBranchAddress("muo_d0sigBL", &muo_d0sigBL, &b_muo_d0sigBL);
   fChain->SetBranchAddress("muo_z0sintheta", &muo_z0sintheta, &b_muo_z0sintheta);
   fChain->SetBranchAddress("muo_quality", &muo_quality, &b_muo_quality);
   fChain->SetBranchAddress("muo_acceptedTool", &muo_acceptedTool, &b_muo_acceptedTool);
   fChain->SetBranchAddress("muo_TTVAEffSF", &muo_TTVAEffSF, &b_muo_TTVAEffSF);
   fChain->SetBranchAddress("muo_LooseIDSF", &muo_LooseIDSF, &b_muo_LooseIDSF);
   fChain->SetBranchAddress("muo_MediumIDSF", &muo_MediumIDSF, &b_muo_MediumIDSF);
   fChain->SetBranchAddress("muo_TightIDSF", &muo_TightIDSF, &b_muo_TightIDSF);
   fChain->SetBranchAddress("muo_hasTruth", &muo_hasTruth, &b_muo_hasTruth);
   fChain->SetBranchAddress("trkjet_pt", &trkjet_pt, &b_trkjet_pt);
   fChain->SetBranchAddress("trkjet_phi", &trkjet_phi, &b_trkjet_phi);
   fChain->SetBranchAddress("trkjet_eta", &trkjet_eta, &b_trkjet_eta);
   fChain->SetBranchAddress("trkjet_E", &trkjet_E, &b_trkjet_E);
   fChain->SetBranchAddress("trkjet_ind", &trkjet_ind, &b_trkjet_ind);
   fChain->SetBranchAddress("trkjet_MV2c00", &trkjet_MV2c00, &b_trkjet_MV2c00);
   fChain->SetBranchAddress("trkjet_MV2c10", &trkjet_MV2c10, &b_trkjet_MV2c10);
   fChain->SetBranchAddress("trkjet_MV2c20", &trkjet_MV2c20, &b_trkjet_MV2c20);
   fChain->SetBranchAddress("trkjet_MVb", &trkjet_MVb, &b_trkjet_MVb);
   fChain->SetBranchAddress("trkjet_SV1_masssvx", &trkjet_SV1_masssvx, &b_trkjet_SV1_masssvx);
   fChain->SetBranchAddress("trkjet_truth", &trkjet_truth, &b_trkjet_truth);
   fChain->SetBranchAddress("trkjet_nTrks", &trkjet_nTrks, &b_trkjet_nTrks);
   fChain->SetBranchAddress("trkjet_assocMuon_dR", &trkjet_assocMuon_dR, &b_trkjet_assocMuon_dR);
   fChain->SetBranchAddress("trkjet_assocMuon_index", &trkjet_assocMuon_index, &b_trkjet_assocMuon_index);
   fChain->SetBranchAddress("trkjet_assocMuon_n", &trkjet_assocMuon_n, &b_trkjet_assocMuon_n);
   fChain->SetBranchAddress("trkjet_isMuonJet", &trkjet_isMuonJet, &b_trkjet_isMuonJet);
   fChain->SetBranchAddress("trkjet_isNonMuonJet", &trkjet_isNonMuonJet, &b_trkjet_isNonMuonJet);
   fChain->SetBranchAddress("trkjet_hasKShort", &trkjet_hasKShort, &b_trkjet_hasKShort);
   fChain->SetBranchAddress("trkjet_hasLambda", &trkjet_hasLambda, &b_trkjet_hasLambda);
   fChain->SetBranchAddress("trkjet_hasConversion", &trkjet_hasConversion, &b_trkjet_hasConversion);
   fChain->SetBranchAddress("trkjet_hasHadMatInt", &trkjet_hasHadMatInt, &b_trkjet_hasHadMatInt);
   fChain->SetBranchAddress("trkjet_BHad_n", &trkjet_BHad_n, &b_trkjet_BHad_n);
   fChain->SetBranchAddress("trkjet_CHad_n", &trkjet_CHad_n, &b_trkjet_CHad_n);
   fChain->SetBranchAddress("trkjet_BHad_n_GA", &trkjet_BHad_n_GA, &b_trkjet_BHad_n_GA);
   fChain->SetBranchAddress("trkjet_CHad_n_GA", &trkjet_CHad_n_GA, &b_trkjet_CHad_n_GA);
   fChain->SetBranchAddress("trkjet_BHad_pdgId", &trkjet_BHad_pdgId, &b_trkjet_BHad_pdgId);
   fChain->SetBranchAddress("trkjet_BHad_pt", &trkjet_BHad_pt, &b_trkjet_BHad_pt);
   fChain->SetBranchAddress("trkjet_BHad_eta", &trkjet_BHad_eta, &b_trkjet_BHad_eta);
   fChain->SetBranchAddress("trkjet_BHad_phi", &trkjet_BHad_phi, &b_trkjet_BHad_phi);
   fChain->SetBranchAddress("trkjet_BHad_e", &trkjet_BHad_e, &b_trkjet_BHad_e);
   fChain->SetBranchAddress("trkjet_BHadDecays_pdgId", &trkjet_BHadDecays_pdgId, &b_trkjet_BHadDecays_pdgId);
   fChain->SetBranchAddress("trkjet_BHadDecays_pt", &trkjet_BHadDecays_pt, &b_trkjet_BHadDecays_pt);
   fChain->SetBranchAddress("trkjet_BHadDecays_eta", &trkjet_BHadDecays_eta, &b_trkjet_BHadDecays_eta);
   fChain->SetBranchAddress("trkjet_BHadDecays_phi", &trkjet_BHadDecays_phi, &b_trkjet_BHadDecays_phi);
   fChain->SetBranchAddress("trkjet_BHadDecays_e", &trkjet_BHadDecays_e, &b_trkjet_BHadDecays_e);
   fChain->SetBranchAddress("trigjet_a4_pt", &trigjet_a4_pt, &b_trigjet_a4_pt);
   fChain->SetBranchAddress("trigjet_a4_phi", &trigjet_a4_phi, &b_trigjet_a4_phi);
   fChain->SetBranchAddress("trigjet_a4_eta", &trigjet_a4_eta, &b_trigjet_a4_eta);
   fChain->SetBranchAddress("trigjet_a4IS_pt", &trigjet_a4IS_pt, &b_trigjet_a4IS_pt);
   fChain->SetBranchAddress("trigjet_a4IS_phi", &trigjet_a4IS_phi, &b_trigjet_a4IS_phi);
   fChain->SetBranchAddress("trigjet_a4IS_eta", &trigjet_a4IS_eta, &b_trigjet_a4IS_eta);
   fChain->SetBranchAddress("trigjet_a10_pt", &trigjet_a10_pt, &b_trigjet_a10_pt);
   fChain->SetBranchAddress("trigjet_a10_phi", &trigjet_a10_phi, &b_trigjet_a10_phi);
   fChain->SetBranchAddress("trigjet_a10_eta", &trigjet_a10_eta, &b_trigjet_a10_eta);
   fChain->SetBranchAddress("trigjet_a10t_pt", &trigjet_a10t_pt, &b_trigjet_a10t_pt);
   fChain->SetBranchAddress("trigjet_a10t_phi", &trigjet_a10t_phi, &b_trigjet_a10t_phi);
   fChain->SetBranchAddress("trigjet_a10t_eta", &trigjet_a10t_eta, &b_trigjet_a10t_eta);
   Notify();
}

Bool_t GbbAna::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void GbbAna::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t GbbAna::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
void GbbAna::Finish(int count)
{
   if(myHist)
   {
    myHist->SaveHist();
    delete myHist;
   }
   
   cout<<endl<<"Running: histograms saved!"<<endl;
}
#endif // #ifdef GbbAna_cxx
