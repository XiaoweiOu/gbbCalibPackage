/*
 * GbbTupleAna.cxx
 *
 *  Created on: Nov 2, 2018
 *      Author: aemerman
 *  Modified from GbbTupleAna.cxx,
 *  created by: rjacobs
 */


#include "GbbTupleAna.h"
#include <iostream>
#include <TMath.h>
#include <TLorentzVector.h>

// Return codes:
//  1 = double-tagged, -1 = double-anti-tagged, 0 = single-tagged (for MV2c10 case)
// -99 = unrecognized b-tag
int GbbTupleAna::passBTagCut(const GbbCandidate& gbbcand) {
  if ( m_BTagWP.Contains("MV2c10") ) {
    return passMV2c10Cut(gbbcand);
  } else if ( m_BTagWP.Contains("XbbScore") ) {
    return passXbbScoreCut(gbbcand);
  } else return -99;
}

int GbbTupleAna::passMV2c10Cut(const GbbCandidate& gbbcand) {
  float taggerCut = -999.;
  if ( m_BTagWP.Contains("FixedCutBEff_60") ) {
    if (m_useVRTrkJets) taggerCut = 0.92;
    else taggerCut = 0.86;

  } else if ( m_BTagWP.Contains("FixedCutBEff_70") ) {
    if (m_useVRTrkJets) taggerCut = 0.79;
    else taggerCut = 0.66;

  } else if ( m_BTagWP.Contains("FixedCutBEff_77") ) {
    if (m_useVRTrkJets) taggerCut = 0.58;
    else taggerCut = 0.38;

  } else if ( m_BTagWP.Contains("FixedCutBEff_85") ) {
    if (m_useVRTrkJets) taggerCut = 0.05;
    else taggerCut = -0.15;

  } else if ( m_BTagWP.Contains("FixedCutBEff_70_R20p7") ) {
    taggerCut = 0.6455;

  } else return -99;
  // Double-b-tagged
  if(this->trkjet_MV2c10->at(gbbcand.muojet_index) > taggerCut &&
     this->trkjet_MV2c10->at(gbbcand.nonmuojet_index) > taggerCut) {
    return 1;
  // 1 b-tag
  } else if(this->trkjet_MV2c10->at(gbbcand.muojet_index) > taggerCut ||
            this->trkjet_MV2c10->at(gbbcand.nonmuojet_index) > taggerCut) {
    return 0;
  // 0 b-tags
  } else return -1;
}


int GbbTupleAna::passXbbScoreCut(const GbbCandidate& gbbcand) {
  float taggerCut = -999.;
  float taggerVar = -999.;
  if ( m_BTagWP.Contains("fp2") ) {
    taggerVar = std::log( this->fat_XbbScoreHiggs->at(gbbcand.fat_index) /
     (0.8*this->fat_XbbScoreQCD->at(gbbcand.fat_index) + 0.2*this->fat_XbbScoreTop->at(gbbcand.fat_index)) );
  } else if ( m_BTagWP.Contains("f0") ) {
    taggerVar = std::log( this->fat_XbbScoreHiggs->at(gbbcand.fat_index) /
                 this->fat_XbbScoreQCD->at(gbbcand.fat_index) );
  } else return -99;

  if ( m_BTagWP.Contains("FixedCutBEff_50") ) {
    if ( m_BTagWP.Contains("fp2") ) taggerCut = 4.5;
    else if ( m_BTagWP.Contains("f0") ) taggerCut = 5.1;

  } else if ( m_BTagWP.Contains("FixedCutBEff_60") ) {
    if ( m_BTagWP.Contains("fp2") ) taggerCut = 3.6;
    else if ( m_BTagWP.Contains("f0") ) taggerCut = 4.5;

  } else if ( m_BTagWP.Contains("FixedCutBEff_70") ) {
    if ( m_BTagWP.Contains("fp2") ) taggerCut = 3.0;
    else if ( m_BTagWP.Contains("f0") ) taggerCut = 3.9;

  } else if ( m_BTagWP.Contains("HybBEff_60") ) {
    if ( m_BTagWP.Contains("fp2") ) taggerCut = XbbTag_antiQCDtop_flat60eff(this->fat_pt->at(gbbcand.fat_index));
    else if ( m_BTagWP.Contains("f0") ) taggerCut = XbbTag_antiQCD_flat60eff(this->fat_pt->at(gbbcand.fat_index));

  } else return -99;
  // Double-b-tagged
  if(taggerVar > taggerCut) {
    return 1;
  // Not double-b-tagged
  } else return -1;
}

double GbbTupleAna::XbbTag_antiQCD_flat60eff(double x) {

  const int fNp = 12, fKstep = 0;
  const double fDelta = -1, fXmin = 0, fXmax = 2750;
  const double fX[12] = { 0, 250, 500, 750, 1000,
                       1250, 1500, 1750, 2000, 2250,
                       2500, 2750 };
  const double fY[12] = { 0, 4.75, 4.875, 4.875, 4.5,
                       4, 3, 1.25, -1.375, -5,
                       -10.375, -14.125 };
  const double fB[12] = { 0.0360118, 0.00586912, -0.000988246, -0.000416141, -0.00184719,
                       -0.00269509, -0.00537245, -0.00881512, -0.0118671, -0.0187165,
                       -0.0212667, -0.00571655 };
  const double fC[12] = { -8.35705e-05, -3.7e-05, 9.57053e-06, -7.28211e-06, 1.5579e-06,
                       -4.94949e-06, -5.75995e-06, -8.01073e-06, -4.19714e-06, -2.32007e-05,
                       1.3e-05, 250 };
  const double fD[12] = { 6.2094e-08, 6.2094e-08, -2.24702e-08, 1.17867e-08, -8.67652e-09,
                       -1.08061e-09, -3.00104e-09, 5.08479e-09, -2.53381e-08, 4.82676e-08,
                       4.82676e-08, 116.025 };
  int klow=0;
  if(x<=fXmin) klow=0;
  else if(x>=fXmax) klow=fNp-1;
  else {
    if(fKstep) {
      // Equidistant knots, use histogramming
      klow = int((x-fXmin)/fDelta);
      if (klow < fNp-1) klow = fNp-1;
    } else {
      int khig=fNp-1, khalf;
      // Non equidistant knots, binary search
      while(khig-klow>1) {
        if(x>fX[khalf=(klow+khig)/2]) klow=khalf;
        else khig=khalf;
      }
    }
  }
  // Evaluate now
  double dx=x-fX[klow];
  return (fY[klow]+dx*(fB[klow]+dx*(fC[klow]+dx*fD[klow])));
}

double GbbTupleAna::XbbTag_antiQCDtop_flat60eff(double x) {
   // 20% fraction for top
   const int fNp = 12, fKstep = 0;
   const double fDelta = -1, fXmin = 0, fXmax = 2750;
   const double fX[12] = { 0, 250, 500, 750, 1000,
                        1250, 1500, 1750, 2000, 2250,
                        2500, 2750 };
   const double fY[12] = { 0, 4, 4.125, 4.125, 3.625,
                        2.875, 1.875, 0.5, -1.625, -5,
                        -10.25, -15.75 };
   const double fB[12] = { 0.0302896, 0.0049802, -0.000710406, -0.000638579, -0.00273528,
                        -0.0034203, -0.00458351, -0.00674566, -0.0104339, -0.0175189,
                        -0.0229906, -0.0195189 };
   const double fC[12] = { -7.02376e-05, -3.1e-05, 8.23756e-06, -7.95025e-06, -4.36552e-07,
                        -2.30354e-06, -2.34929e-06, -6.2993e-06, -8.45352e-06, -1.98866e-05,
                        -2e-06, 250 };
   const double fD[12] = { 5.23168e-08, 5.23168e-08, -2.15838e-08, 1.00183e-08, -2.48932e-09,
                        -6.10022e-11, -5.26667e-09, -2.8723e-09, -1.52441e-08, 2.38488e-08,
                        2.38488e-08, 116.025 };
   int klow=0;
   if(x<=fXmin) klow=0;
   else if(x>=fXmax) klow=fNp-1;
   else {
     if(fKstep) {
       // Equidistant knots, use histogramming
       klow = int((x-fXmin)/fDelta);
       if (klow < fNp-1) klow = fNp-1;
     } else {
       int khig=fNp-1, khalf;
       // Non equidistant knots, binary search
       while(khig-klow>1)
         if(x>fX[khalf=(klow+khig)/2]) klow=khalf;
         else khig=khalf;
     }
   }
   // Evaluate now
   double dx=x-fX[klow];
   return (fY[klow]+dx*(fB[klow]+dx*(fC[klow]+dx*fD[klow])));
}

double XbbTag_spline_flatQCDrejection_mH_0(double x) {
   const int fNp = 11, fKstep = 0;
   const double fDelta = -1, fXmin = 0, fXmax = 185;
   const double fX[11] = { 0, 50, 65, 80, 95,
                        110, 125, 140, 155, 170,
                        185 };
   const double fY[11] = { 0, 2.875, 3.625, 4.75, 5.25,
                        5.5, 5.375, 4.75, 4.5, 3.75,
                        3.625 };
   const double fB[11] = { 0.137238, 0.0346611, 0.0704599, 0.0584995, 0.0205421,
                        0.00933219, -0.0328708, -0.0278489, -0.0307337, -0.0492165,
                        0.0525997 };
   const double fC[11] = { -0.00273274, 0.000681202, 0.00170538, -0.00250274, -2.77557e-05,
                        -0.00071957, -0.00209396, 0.00242876, -0.00262108, 0.00138889,
                        15 };
   const double fD[11] = { 2.27596e-05, 2.27596e-05, -9.35139e-05, 5.49997e-05, -1.53737e-05,
                        -3.05421e-05, 0.000100505, -0.000112219, 8.91104e-05, 8.91104e-05,
                        6.96152 };
   int klow=0;
   if(x<=fXmin) klow=0;
   else if(x>=fXmax) klow=fNp-1;
   else {
     if(fKstep) {
       // Equidistant knots, use histogramming
       klow = int((x-fXmin)/fDelta);
       if (klow < fNp-1) klow = fNp-1;
     } else {
       int khig=fNp-1, khalf;
       // Non equidistant knots, binary search
       while(khig-klow>1)
         if(x>fX[khalf=(klow+khig)/2]) klow=khalf;
         else khig=khalf;
     }
   }
   // Evaluate now
   double dx=x-fX[klow];
   return (fY[klow]+dx*(fB[klow]+dx*(fC[klow]+dx*fD[klow])));
}

bool GbbTupleAna::isCleanEvt(const float total_evt_weight) {

  if (!(this->eve_isMC)) {

    if(!(this->eve_isCleanEvent && this->eve_passGRL)) return false;
    if(this->jet_pt->size()<2) return false;

  } else { //MC: throw out event with large weights out of Pt range

    m_HistSvc->FastFillTH1D("truthjet_pt",";truth jet p_{T} [GeV];Events/50 GeV;",
     this->truthjet_pt->at(0)/1e3,100,0.,5000.,total_evt_weight);

    // MC quality cut necessary for low-pT JZXW samples
    // see https://twiki.cern.ch/twiki/pub/AtlasProtected/JetStudies2012/JLacey_MC12_JZXW_weights_and_CleaningCuts.pdf
    if(this->jet_pt->size()>=2 && this->truthjet_pt->size()){

      double mc_jet_ratio=0.5*(this->jet_pt->at(0)+this->jet_pt->at(1))/this->truthjet_pt->at(0);

      if(0.6>mc_jet_ratio || mc_jet_ratio>1.4){
        std::cout<<"Have event with funny pt(reco)/pt(truth) ratio!"<<std::endl;
        return false;
      }

    }else{
      std::cout<<"Have less than 2 jets"<<std::endl;
      return false;
    }
  }
  return true;
}

bool GbbTupleAna::hasBadJet(){

  for(unsigned int i=0; i<this->jet_pt->size(); i++){
    if(!(this->jet_clean->at(i))) return true;
  }
  return false;
}

int GbbTupleAna::getTrigMatch() {
  //Trigger matching (trigger jet == offline jet matched to leading trigger-level jet
  TLorentzVector trigjet_trlvl,jet_reco;
  double DRmin_trigmatch=999.;
  int i_trigjet=-1;
  if (m_2016) {
    trigjet_trlvl.SetPtEtaPhiM(this->trigjet_a4_pt->at(0),this->trigjet_a4_eta->at(0),
        this->trigjet_a4_phi->at(0),0.);
  } else if (m_2017 || m_2018) {
    trigjet_trlvl.SetPtEtaPhiM(this->trigjet_a4IS_pt->at(0),this->trigjet_a4IS_eta->at(0),
        this->trigjet_a4IS_phi->at(0),0.);
  }
  for(unsigned int i=0; i<this->jet_pt->size(); i++){
    jet_reco.SetPtEtaPhiM(this->jet_pt->at(i),this->jet_eta->at(i),this->jet_phi->at(i),0.);
    if(jet_reco.DeltaR(trigjet_trlvl)<0.4 && jet_reco.DeltaR(trigjet_trlvl)<DRmin_trigmatch && this->passR4CaloJetCuts(i)){
      DRmin_trigmatch=jet_reco.DeltaR(trigjet_trlvl);
      i_trigjet=i;
    }
  }
  return i_trigjet;
}


bool GbbTupleAna::passTrigger(const float trigjet_pt, float& prescale, TString& trigger_passed) {
  trigger_passed="none";
  prescale=1.;

  /*if(m_RunMode & RunMode::FILL_TRIGGER){
    prescale=1.;
  }else if(trjet_pt>500e3 && this->eve_HLT_j380) trigger_passed="HLT_j380";
  else return false;*/


  /*  if(m_RunMode & RunMode::FILL_TRIGGER){
    prescale=1.;
  }else if(trjet_pt>198e3 && trjet_pt<=229e3 && this->eve_HLT_j150){
    trigger_passed="HLT_j150";
    if(this->eve_isMC) prescale=7.633e-4;
  }else if(trjet_pt>229e3 && trjet_pt<=334e3 && this->eve_HLT_j175){
    trigger_passed="HLT_j175";
    if(this->eve_isMC) prescale=3.945e-3;
  }else if(trjet_pt>334e3 && trjet_pt<=430e3 && this->eve_HLT_j260){
    trigger_passed="HLT_j260";
    if(this->eve_isMC) prescale=2.453e-2;
  }else if(trjet_pt>430e3 && this->eve_HLT_j380) trigger_passed="HLT_j380";
  else return false;*/

  /*  if(m_RunMode & RunMode::FILL_TRIGGER){
    prescale=1.;
  }else if(trjet_pt>238e3 && trjet_pt<=275e3 && this->eve_HLT_j150){
    trigger_passed="HLT_j150";
    if(this->eve_isMC) prescale=7.633e-4;
  }else if(trjet_pt>275e3 && trjet_pt<=400e3 && this->eve_HLT_j175){
    trigger_passed="HLT_j175";
    if(this->eve_isMC) prescale=3.945e-3;
  }else if(trjet_pt>400e3 && trjet_pt<=516e3 && this->eve_HLT_j260){
    trigger_passed="HLT_j260";
    if(this->eve_isMC) prescale=2.453e-2;
  }else if(trjet_pt>516e3 && this->eve_HLT_j380) trigger_passed="HLT_j380";
  else return false;
  */

  /*if(largeRtrigpt>250e3 && largeRtrigpt<=280e3 && this->eve_HLT_j150){
    trigger_passed="HLT_j150";
    if(this->eve_isMC) prescale=7.633e-4;
  }else if(largeRtrigpt>280e3 && largeRtrigpt<=380e3 && this->eve_HLT_j175){
    trigger_passed="HLT_j175";
    if(this->eve_isMC) prescale=3.945e-3;
  }else if(largeRtrigpt>380e3 && largeRtrigpt<=500e3 && this->eve_HLT_j260){
    trigger_passed="HLT_j260";
    if(this->eve_isMC) prescale=2.453e-2;
  }else if(largeRtrigpt>500e3 && this->eve_HLT_j380){
    trigger_passed="HLT_j380";
  }else{
    //std::cout<<"Event discarded: pt(R=0.4): "<<trjet_pt<<" pt(R=1.0): "<<largeRtrigpt<<std::endl;
    return false;
    }*/

  if (m_2016 && trigjet_pt > 500e3 && this->eve_HLT_j380) { //demand that full efficiency of turnon curve has been reached
    trigger_passed="HLT_j380";
  } else if (m_2017 && trigjet_pt > 500e3 && this->eve_HLT_j420) { //demand that full efficiency of turnon curve has been reached
    trigger_passed="HLT_j420";
  } else {
    return false;
  }
  return true;
}


bool GbbTupleAna::cutTriggerBias(const float gbbcandpt, const TString trigger_passed) {
  //cut away region where jet bias is introduced
  if (gbbcandpt < 500e3) {
    return false;
  }
  //if (!trigger_passed.EqualTo("HLT_j380")) {
  //  std::cout<<"Event with (R=1.0) pT "<<gbbcandpt<<" that didn't pass j380!"<<std::endl;
  //}

  /*  if(gbbcandpt>250e3 && gbbcandpt<=280e3 && trigger_passed.EqualTo("HLT_j150")){
  }else if(gbbcandpt>280e3 && gbbcandpt<=380e3 && trigger_passed.EqualTo("HLT_j175")){
  }else if(gbbcandpt>380e3 && gbbcandpt<=500e3 && trigger_passed.EqualTo("HLT_j260")){
  }else if(gbbcandpt>500e3 && trigger_passed.EqualTo("HLT_j380")){
  }else{
    std::cout<<"Event discarded: pt(R=0.4): "<<trjet_pt<<" pt(R=1.0): "<<gbbcandpt<<std::endl;
    return false;
    }*/
  return true;
}


bool GbbTupleAna::passR4CaloJetCuts(unsigned int i_jet){

  //JVT default Working point: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/JVTCalibration
  bool passJVT=(this->jet_pt->at(i_jet)>60e3 || TMath::Abs(this->jet_eta->at(i_jet))>2.4 || this->jet_JVT->at(i_jet)>0.59);

  return (this->jet_pt->at(i_jet)>20e3 && TMath::Abs(this->jet_eta->at(i_jet))<2.5 && passJVT);
}

bool GbbTupleAna::passR10CaloJetCuts(unsigned int i_jet){

  return (this->fat_pt->at(i_jet)>250e3 && TMath::Abs(this->fat_eta->at(i_jet))<2.0);
}


bool GbbTupleAna::passR2TrackJetCuts(unsigned int i_jet){

  return (this->trkjet_pt->at(i_jet)>10e3 && TMath::Abs(this->trkjet_eta->at(i_jet))<2.5);
}

bool GbbTupleAna::passMuonSelection(unsigned int i_muon){
  bool id=false, track=false, kin=false;

  // tight = 0, medium = 1, loose = 2, veryloose = 3
  //if(this->muo_quality->at(i_muon)<=2) id=true;
  if(this->muo_acceptedTool->at(i_muon)) id=true;

  if(TMath::Abs(this->muo_d0->at(i_muon))<2. && TMath::Abs(this->muo_z0sintheta->at(i_muon))<2.) track=true;

  if(this->muo_pt->at(i_muon)>5e3 && TMath::Abs(this->muo_eta->at(i_muon))<2.4 ) kin=true;

  if(m_Debug && !id) std::cout<<"passMuonSelection: Did not pass quality requirement, quality is"<<this->muo_quality->at(i_muon)<<std::endl;
  if(m_Debug && !track) std::cout<<"passMuonSelection: Did not pass track requirement"<<std::endl;
  if(m_Debug && !kin)std::cout<<"passMuonSelection: Did not pass kinematics requirement"<<std::endl;

  return (id && track && kin);
}

bool GbbTupleAna::passR21AssocTrkSelection(unsigned int i_trk, unsigned int i_jet){

  if (!this->trkjet_assocTrk_isLoose->at(i_jet).at(i_trk)) return false;
  if (this->trkjet_assocTrk_pt->at(i_jet).at(i_trk)/1e3 < 0.5) return false;
  if (fabs(this->trkjet_assocTrk_d0->at(i_jet).at(i_trk)) > 2) return false;
  if (fabs( (this->trkjet_assocTrk_z0->at(i_jet).at(i_trk)  - this->eve_PVz
                     +this->trkjet_assocTrk_vz->at(i_jet).at(i_trk)) *
             TMath::Sin(this->trkjet_assocTrk_theta->at(i_jet).at(i_trk))) > 3) return false;
  return true;
}

//FIXME: these is the hard-coded Summer 2017 Loose Electron track cuts. Should put definition in tuple-making
bool GbbTupleAna::passAssocTrkSelection(unsigned int i_trk, unsigned int i_jet){

  if( (this->trkjet_assocTrk_nPixelHits->at(i_jet).at(i_trk)<1) || (this->trkjet_assocTrk_nPixelHits->at(i_jet).at(i_trk)+this->trkjet_assocTrk_nSCTHits->at(i_jet).at(i_trk))<7){

  if(m_Debug){
  std::cout<<"passAssocTrkSelection(): fail Silicon Hits cut "<<this->trkjet_assocTrk_nPixelHits->at(i_jet).at(i_trk)+this->trkjet_assocTrk_nSCTHits->at(i_jet).at(i_trk)<<std::endl;
  std::cout<<"Pixel hits:"<<this->trkjet_assocTrk_nPixelHits->at(i_jet).at(i_trk)<<std::endl;
  std::cout<<"SCT hits:"<<this->trkjet_assocTrk_nSCTHits->at(i_jet).at(i_trk)<<std::endl;
  }
  return false;
 }
 return true;
}

// a function to check specific cuts
bool GbbTupleAna::passSpecificCuts(const unsigned long int flag, const std::vector<unsigned int> &cuts) {
  unsigned int mask = 0;
  // Make the bit mask
  for (auto cut : cuts) mask = mask | 1 << cut;
  // Check if the flag matches the bit mask
  return (flag & mask) == mask;
}
