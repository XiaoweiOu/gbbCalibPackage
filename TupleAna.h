//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Feb 17 13:09:15 2016 by ROOT version 6.02/12
// from TTree FlavourTagging_Nominal/FlavourTagging_Nominal
// found on file: data15_13TeV.00284154.physics_Main.merge.DAOD_FTAG1.f643_m1518_p2528.root
//////////////////////////////////////////////////////////

#ifndef TupleAna_h
#define TupleAna_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"
#include "vector"
#include "vector"

using namespace std;

class TupleAna {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           eve_num;
   Int_t           eve_run;
   Float_t           eve_mu;
   Int_t           eve_mc_num;
   Int_t           eve_mc_chan;
   Float_t         eve_mc_w;
   Float_t         eve_pu_w;
   Float_t         eve_BtagSFWeightNom;
   vector<float>   *eve_BtagSFWeightSys;
   Bool_t          eve_isMC;
   Int_t           eve_hasPV;
   Int_t           eve_isCleanEvent;
   Int_t           eve_passGRL;
   Int_t           eve_HLT_j15;
   Int_t           eve_HLT_j25;
   Int_t           eve_HLT_j35;
   Int_t           eve_HLT_j45;
   Int_t           eve_HLT_j55;
   Int_t           eve_HLT_j60;
   Int_t           eve_HLT_j85;
   Int_t           eve_HLT_j100;
   Int_t           eve_HLT_j110;
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
   Int_t           eve_HLT_j440;
   Int_t           eve_HLT_j460;
   vector<float>   *jet_pt;
   vector<float>   *jet_phi;
   vector<float>   *jet_eta;
   vector<float>   *jet_E;
   vector<int>     *jet_ind;
   vector<float>   *jet_JVT;
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
   vector<std::vector<float>> *trkjet_assocTrk_d0_smear;
   vector<std::vector<float>> *trkjet_assocTrk_z0_smear;
   vector<std::vector<float>> *trkjet_assocTrk_d0_smear_up;
   vector<std::vector<float>> *trkjet_assocTrk_z0_smear_up;
   vector<std::vector<float>> *trkjet_assocTrk_d0_smear_down;
   vector<std::vector<float>> *trkjet_assocTrk_z0_smear_down;
   // vector<vector<float> > *trkjet_assocTrk_z0wrtPV;
   vector<vector<float> > *trkjet_assocTrk_theta;
   vector<vector<float> > *trkjet_assocTrk_qoverp;
   vector<vector<float> > *trkjet_assocTrk_d0err;
   vector<vector<float> > *trkjet_assocTrk_z0err;
   vector<vector<float> > *trkjet_assocTrk_thetaerr;
   vector<vector<float> > *trkjet_assocTrk_phi0err;
   vector<vector<int> > *trkjet_assocTrk_nPixelHits;
   vector<vector<int> > *trkjet_assocTrk_nSCTHits;
   vector<vector<int> > *trkjet_assocTrk_nIBLHits;
   vector<std::vector<int>> *trkjet_assocTrk_expectIBLHit;
   vector<std::vector<int>> *trkjet_assocTrk_nBLayerHits;
   vector<std::vector<int>> *trkjet_assocTrk_expectBLayerHit;
   vector<std::vector<int>> *trkjet_assocTrk_isLoose;
   vector<std::vector<int>> *trkjet_assocTrk_isTightPrimary;
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
   vector<int>   *fat_isGbbJet;
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
   vector<int>     *muo_quality;
   vector<float>   *muo_z0;
   vector<float>   *muo_z0sintheta;
   vector<float>   *muo_d0;
   vector<int>     *muo_acceptedTool;
   vector<float>   *trkjet_pt;
   vector<float>   *trkjet_phi;
   vector<float>   *trkjet_eta;
   vector<float>   *trkjet_E;
   vector<int>     *trkjet_ind;
   vector<float>   *trkjet_MV2c00;
   vector<float>   *trkjet_MV2c10;
   vector<float>   *trkjet_MV2c20;
   vector<float>   *trkjet_MVb;
   vector<int>     *trkjet_truth;
   vector<int>     *trkjet_isMuonJet;
   vector<int>     *trkjet_isNonMuonJet;
   vector<vector<float> > *trkjet_assocMuon_dR;
   vector<vector<int> > *trkjet_assocMuon_index;
   vector<int>     *trkjet_assocMuon_n;
   vector<float>   *trigjet_pt;
   vector<float>   *trigjet_phi;
   vector<float>   *trigjet_eta;

   // List of branches
   TBranch        *b_eve_num;   //!
   TBranch        *b_eve_run;   //!
   TBranch        *b_eve_mu;   //!
   TBranch        *b_eve_mc_num;   //!
   TBranch        *b_eve_mc_chan;   //!
   TBranch        *b_eve_mc_w;   //!
   TBranch        *b_eve_pu_w;   //!
   TBranch        *b_eve_isMC;   //!
   TBranch        *b_eve_BtagSFWeightNom; //!
   TBranch        *b_eve_BtagSFWeightSys; //!
   TBranch        *b_eve_hasPV;   //!
   TBranch        *b_eve_isCleanEvent;   //!
   TBranch        *b_eve_passGRL;   //!
   TBranch        *b_eve_HLT_j15;   //!
   TBranch        *b_eve_HLT_j25;   //!
   TBranch        *b_eve_HLT_j35;   //!
   TBranch        *b_eve_HLT_j45;   //!
   TBranch        *b_eve_HLT_j55;   //!
   TBranch        *b_eve_HLT_j60;   //!
   TBranch        *b_eve_HLT_j85;   //!
   TBranch        *b_eve_HLT_j100;   //!
   TBranch        *b_eve_HLT_j110;   //!
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
   TBranch        *b_eve_HLT_j440;   //!
   TBranch        *b_eve_HLT_j460;   //!
   TBranch        *b_jet_pt;   //!
   TBranch        *b_jet_phi;   //!
   TBranch        *b_jet_eta;   //!
   TBranch        *b_jet_E;   //!
   TBranch        *b_jet_ind;   //!
   TBranch        *b_jet_JVT;   //!
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
   TBranch        *b_trkjet_assocTrk_d0_smear; //!
   TBranch        *b_trkjet_assocTrk_z0_smear; //!
   TBranch        *b_trkjet_assocTrk_d0_smear_up; //!
   TBranch        *b_trkjet_assocTrk_z0_smear_up; //!
   TBranch        *b_trkjet_assocTrk_d0_smear_down; //!
   TBranch        *b_trkjet_assocTrk_z0_smear_down; //!
   //   TBranch        *b_trkjet_assocTrk_z0wrtPV;   //!
   TBranch        *b_trkjet_assocTrk_theta;   //!
   TBranch        *b_trkjet_assocTrk_qoverp;   //!
   TBranch        *b_trkjet_assocTrk_d0err;   //!
   TBranch        *b_trkjet_assocTrk_z0err;   //!
   TBranch        *b_trkjet_assocTrk_thetaerr;   //!
   TBranch        *b_trkjet_assocTrk_phi0err;   //!
   TBranch        *b_trkjet_assocTrk_nPixelHits;   //!                                                                                                                                                                                     
   TBranch        *b_trkjet_assocTrk_nSCTHits;   //!
   TBranch        *b_trkjet_assocTrk_nIBLHits;   //!
   TBranch        *b_trkjet_assocTrk_expectIBLHit; //!
   TBranch        *b_trkjet_assocTrk_nBLayerHits;//!
   TBranch        *b_trkjet_assocTrk_expectBLayerHit;//!
   TBranch        *b_trkjet_assocTrk_isLoose;//!
   TBranch        *b_trkjet_assocTrk_isTightPrimary;//!
   TBranch        *b_fat_E; //!
   TBranch        *b_fat_pt;   //!
   TBranch        *b_fat_phi;   //!
   TBranch        *b_fat_eta;   //!
   TBranch        *b_fat_ind;   //!
   TBranch        *b_fat_jvt;   //!
   TBranch        *b_fat_tau21;   //!
   TBranch        *b_fat_tau32;   //!
   TBranch        *b_fat_C2;   //!
   TBranch        *b_fat_D2;   //!
   TBranch        *b_fat_isGbbJet; //!
   TBranch        *b_fat_assocTrkjetDR_ind; //!
   TBranch        *b_fat_assocTrkjet_ind;   //!
   TBranch        *b_fat_assocTrk_pt;   //!
   TBranch        *b_fat_assocTrk_eta;   //!
   TBranch        *b_fat_assocTrk_phi;   //!
   TBranch        *b_muo_pt;   //!
   TBranch        *b_muo_phi;   //!
   TBranch        *b_muo_eta;   //!
   TBranch        *b_muo_E;   //!
   TBranch        *b_muo_ind;   //!
   TBranch        *b_muo_quality;   //!
   TBranch        *b_muo_z0;   //!
   TBranch        *b_muo_z0sintheta;   //!
   TBranch        *b_muo_d0;   //!
   TBranch        *b_muo_acceptedTool;   //!                                                                                                                                                                                               
   TBranch        *b_trkjet_pt;   //!
   TBranch        *b_trkjet_phi;   //!
   TBranch        *b_trkjet_eta;   //!
   TBranch        *b_trkjet_E;   //!
   TBranch        *b_trkjet_ind;   //!
   TBranch        *b_trkjet_MV2c00;   //!
   TBranch        *b_trkjet_MV2c10;   //!
   TBranch        *b_trkjet_MV2c20;   //!
   TBranch        *b_trkjet_MVb;   //!
   TBranch        *b_trkjet_truth;   //!
   TBranch        *b_trkjet_assocMuon_dR;   //!
   TBranch        *b_trkjet_assocMuon_index;   //!
   TBranch        *b_trkjet_assocMuon_n;   //!
   TBranch        *b_trkjet_isMuonJet; //!
   TBranch        *b_trkjet_isNonMuonJet; //!
   TBranch        *b_trigjet_pt; //!
   TBranch        *b_trigjet_eta;//!
   TBranch        *b_trigjet_phi; //!

   TupleAna();
   TupleAna(TTree *tree);
   virtual ~TupleAna();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);


};

#endif

#ifdef TupleAna_cxx

TupleAna::TupleAna() : fChain(0) {

}

TupleAna::TupleAna(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("./testfile/data15_13TeV.00284154.physics_Main.merge.DAOD_FTAG1.f643_m1518_p2528.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("./testfile/data15_13TeV.00284154.physics_Main.merge.DAOD_FTAG1.f643_m1518_p2528.root");
      }
      f->GetObject("FlavourTagging_Nominal",tree);

   }
   Init(tree);
}

TupleAna::~TupleAna()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t TupleAna::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t TupleAna::LoadTree(Long64_t entry)
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

void TupleAna::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   eve_BtagSFWeightSys = 0;
   jet_pt = 0;
   jet_phi = 0;
   jet_eta = 0;
   jet_E = 0;
   jet_ind = 0;
   jet_JVT = 0;
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
   trkjet_assocTrk_d0_smear = 0;
   trkjet_assocTrk_z0_smear = 0;
   trkjet_assocTrk_d0_smear_up = 0;
   trkjet_assocTrk_z0_smear_up = 0;
   trkjet_assocTrk_d0_smear_down = 0;
   trkjet_assocTrk_z0_smear_down = 0;
   //trkjet _assocTrk_z0wrtPV = 0;
   trkjet_assocTrk_theta = 0;
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
   trkjet_isNonMuonJet = 0;
   trkjet_isMuonJet = 0;
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
   fat_assocTrkjetDR_ind=0;
   fat_assocTrkjet_ind = 0;
   fat_assocTrk_pt = 0;
   fat_assocTrk_eta = 0;
   fat_assocTrk_phi = 0;
   muo_pt = 0;
   muo_phi = 0;
   muo_eta = 0;
   muo_E = 0;
   muo_ind = 0;
   muo_quality = 0;
   muo_z0 = 0;
   muo_d0 = 0;
   muo_z0sintheta = 0;
   muo_acceptedTool =0;
   trkjet_pt = 0;
   trkjet_phi = 0;
   trkjet_eta = 0;
   trkjet_E = 0;
   trkjet_ind = 0;
   trkjet_MV2c00 = 0;
   trkjet_MV2c10 = 0;
   trkjet_MV2c20 = 0;
   trkjet_MVb = 0;
   trkjet_truth = 0;
   trkjet_assocMuon_dR = 0;
   trkjet_assocMuon_index = 0;
   trkjet_assocMuon_n = 0;
   trigjet_pt=0;
   trigjet_eta=0;
   trigjet_phi=0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("eve_num", &eve_num, &b_eve_num);
   fChain->SetBranchAddress("eve_run", &eve_run, &b_eve_run);
   fChain->SetBranchAddress("eve_mu", &eve_mu, &b_eve_mu);
   fChain->SetBranchAddress("eve_mc_num", &eve_mc_num, &b_eve_mc_num);
   fChain->SetBranchAddress("eve_mc_chan", &eve_mc_chan, &b_eve_mc_chan);
   fChain->SetBranchAddress("eve_mc_w", &eve_mc_w, &b_eve_mc_w);
   fChain->SetBranchAddress("eve_pu_w", &eve_pu_w, &b_eve_pu_w);
   fChain->SetBranchAddress("eve_isMC", &eve_isMC, &b_eve_isMC);
   fChain->SetBranchAddress("eve_BtagSFWeightNom", &eve_BtagSFWeightNom, &b_eve_BtagSFWeightNom);
   fChain->SetBranchAddress("eve_BtagSFWeightSys", &eve_BtagSFWeightSys, &b_eve_BtagSFWeightSys);
   fChain->SetBranchAddress("eve_hasPV", &eve_hasPV, &b_eve_hasPV);
   fChain->SetBranchAddress("eve_isCleanEvent", &eve_isCleanEvent, &b_eve_isCleanEvent);
   fChain->SetBranchAddress("eve_passGRL", &eve_passGRL, &b_eve_passGRL);
   fChain->SetBranchAddress("eve_HLT_j15", &eve_HLT_j15, &b_eve_HLT_j15);
   fChain->SetBranchAddress("eve_HLT_j25", &eve_HLT_j25, &b_eve_HLT_j25);
   fChain->SetBranchAddress("eve_HLT_j35", &eve_HLT_j35, &b_eve_HLT_j35);
   fChain->SetBranchAddress("eve_HLT_j45", &eve_HLT_j45, &b_eve_HLT_j45);
   fChain->SetBranchAddress("eve_HLT_j55", &eve_HLT_j55, &b_eve_HLT_j55);
   fChain->SetBranchAddress("eve_HLT_j60", &eve_HLT_j60, &b_eve_HLT_j60);
   fChain->SetBranchAddress("eve_HLT_j85", &eve_HLT_j85, &b_eve_HLT_j85);
   fChain->SetBranchAddress("eve_HLT_j100", &eve_HLT_j100, &b_eve_HLT_j100);
   fChain->SetBranchAddress("eve_HLT_j110", &eve_HLT_j110, &b_eve_HLT_j110);
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
   fChain->SetBranchAddress("eve_HLT_j440", &eve_HLT_j440, &b_eve_HLT_j440);
   fChain->SetBranchAddress("eve_HLT_j460", &eve_HLT_j460, &b_eve_HLT_j460);
   fChain->SetBranchAddress("jet_pt", &jet_pt, &b_jet_pt);
   fChain->SetBranchAddress("jet_phi", &jet_phi, &b_jet_phi);
   fChain->SetBranchAddress("jet_eta", &jet_eta, &b_jet_eta);
   fChain->SetBranchAddress("jet_E", &jet_E, &b_jet_E);
   fChain->SetBranchAddress("jet_ind", &jet_ind, &b_jet_ind);
   fChain->SetBranchAddress("jet_JVT", &jet_JVT, &b_jet_JVT);
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
   fChain->SetBranchAddress("trkjet_assocTrk_d0_smear", &trkjet_assocTrk_d0_smear, &b_trkjet_assocTrk_d0_smear);
   fChain->SetBranchAddress("trkjet_assocTrk_z0_smear", &trkjet_assocTrk_z0_smear, &b_trkjet_assocTrk_z0_smear);
   fChain->SetBranchAddress("trkjet_assocTrk_d0_smear_up", &trkjet_assocTrk_d0_smear_up, &b_trkjet_assocTrk_d0_smear_up);
   fChain->SetBranchAddress("trkjet_assocTrk_z0_smear_up", &trkjet_assocTrk_z0_smear_up, &b_trkjet_assocTrk_z0_smear_up);
   fChain->SetBranchAddress("trkjet_assocTrk_d0_smear_down", &trkjet_assocTrk_d0_smear_down, &b_trkjet_assocTrk_d0_smear_down);
   fChain->SetBranchAddress("trkjet_assocTrk_z0_smear_down", &trkjet_assocTrk_z0_smear_down, &b_trkjet_assocTrk_z0_smear_down);
   //fChain->SetBranchAddress("trkjet_assocTrk_z0wrtPV", &trkjet_assocTrk_z0wrtPV, &b_trkjet_assocTrk_z0wrtPV);
   fChain->SetBranchAddress("trkjet_assocTrk_theta", &trkjet_assocTrk_theta, &b_trkjet_assocTrk_theta);
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
   fChain->SetBranchAddress("fat_assocTrkjetDR_ind", &fat_assocTrkjetDR_ind, &b_fat_assocTrkjetDR_ind);
   fChain->SetBranchAddress("fat_assocTrkjet_ind", &fat_assocTrkjet_ind, &b_fat_assocTrkjet_ind);
   fChain->SetBranchAddress("fat_assocTrk_pt", &fat_assocTrk_pt, &b_fat_assocTrk_pt);
   fChain->SetBranchAddress("fat_assocTrk_eta", &fat_assocTrk_eta, &b_fat_assocTrk_eta);
   fChain->SetBranchAddress("fat_assocTrk_phi", &fat_assocTrk_phi, &b_fat_assocTrk_phi);
   fChain->SetBranchAddress("muo_pt", &muo_pt, &b_muo_pt);
   fChain->SetBranchAddress("muo_phi", &muo_phi, &b_muo_phi);
   fChain->SetBranchAddress("muo_eta", &muo_eta, &b_muo_eta);
   fChain->SetBranchAddress("muo_E", &muo_E, &b_muo_E);
   fChain->SetBranchAddress("muo_ind", &muo_ind, &b_muo_ind);
   fChain->SetBranchAddress("muo_quality", &muo_quality, &b_muo_quality);
   fChain->SetBranchAddress("muo_z0", &muo_z0, &b_muo_z0);
   fChain->SetBranchAddress("muo_z0sintheta", &muo_z0sintheta, &b_muo_z0sintheta);
   fChain->SetBranchAddress("muo_d0", &muo_d0, &b_muo_d0);
   fChain->SetBranchAddress("muo_acceptedTool", &muo_acceptedTool, &b_muo_acceptedTool);
   fChain->SetBranchAddress("trkjet_pt", &trkjet_pt, &b_trkjet_pt);
   fChain->SetBranchAddress("trkjet_phi", &trkjet_phi, &b_trkjet_phi);
   fChain->SetBranchAddress("trkjet_eta", &trkjet_eta, &b_trkjet_eta);
   fChain->SetBranchAddress("trkjet_E", &trkjet_E, &b_trkjet_E);
   fChain->SetBranchAddress("trkjet_ind", &trkjet_ind, &b_trkjet_ind);
   fChain->SetBranchAddress("trkjet_MV2c00", &trkjet_MV2c00, &b_trkjet_MV2c00);
   fChain->SetBranchAddress("trkjet_MV2c10", &trkjet_MV2c10, &b_trkjet_MV2c10);
   fChain->SetBranchAddress("trkjet_MV2c20", &trkjet_MV2c20, &b_trkjet_MV2c20);
   fChain->SetBranchAddress("trkjet_MVb", &trkjet_MVb, &b_trkjet_MVb);
   fChain->SetBranchAddress("trkjet_truth", &trkjet_truth, &b_trkjet_truth);
   fChain->SetBranchAddress("trkjet_assocMuon_dR", &trkjet_assocMuon_dR, &b_trkjet_assocMuon_dR);
   fChain->SetBranchAddress("trkjet_assocMuon_index", &trkjet_assocMuon_index, &b_trkjet_assocMuon_index);
   fChain->SetBranchAddress("trkjet_assocMuon_n", &trkjet_assocMuon_n, &b_trkjet_assocMuon_n);
   fChain->SetBranchAddress("trkjet_isMuonJet", &trkjet_isMuonJet, &b_trkjet_isMuonJet);
   fChain->SetBranchAddress("trkjet_isNonMuonJet", &trkjet_isNonMuonJet, &b_trkjet_isNonMuonJet);
   fChain->SetBranchAddress("trigjet_pt", &trigjet_pt, &b_trigjet_pt);
   fChain->SetBranchAddress("trigjet_eta", &trigjet_eta, &b_trigjet_eta);
   fChain->SetBranchAddress("trigjet_phi", &trigjet_phi, &b_trigjet_phi);
   Notify();
}

Bool_t TupleAna::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void TupleAna::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t TupleAna::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef TupleAna_cxx
