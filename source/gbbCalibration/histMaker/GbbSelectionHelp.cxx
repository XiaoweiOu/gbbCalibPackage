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
        std::cout<<"Event with funny pt(reco)/pt(truth) ratio: "<<mc_jet_ratio<<"!"<<std::endl;
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

  if(this->muo_pt->at(i_muon)>10e3 && TMath::Abs(this->muo_eta->at(i_muon))<2.4 ) kin=true;

  if(m_Debug && !id) std::cout<<"passMuonSelection: Did not pass quality requirement, quality is"<<this->muo_quality->at(i_muon)<<std::endl;
  if(m_Debug && !track) std::cout<<"passMuonSelection: Did not pass track requirement"<<std::endl;
  if(m_Debug && !kin)std::cout<<"passMuonSelection: Did not pass kinematics requirement"<<std::endl;

  return (id && track && kin);
}

bool GbbTupleAna::passR21AssocTrkSelection(unsigned int i_trk, unsigned int i_jet){

  if (!this->trkjet_assocTrk_isLoose->at(i_jet).at(i_trk)) return false;
  if (this->trkjet_assocTrk_pt->at(i_jet).at(i_trk)/1e3 < 0.5) return false;
  if (fabs(this->trkjet_assocTrk_d0->at(i_jet).at(i_trk)) > 5) return false;
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
