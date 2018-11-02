/*
 * GbbTupleAna.cxx
 *
 *  Created on: Feb 18, 2016
 *      Author: rjacobs
 */


#include "GbbCalibration/GbbTupleAna.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>
#include <TMath.h>
#include <TLorentzVector.h>
#include "TObjString.h"
#include "TTree.h"
#include "TChain.h"
#include "PathResolver/PathResolver.h"
#include "TSystem.h"

// Method to standardize (and easily change) plot names
TString GbbTupleAna::makePlotName(const TString syst, const TString cat, const TString pt,
                                  const TString name, const TString tag) {
  TString mySyst = syst; if (mySyst != "") mySyst += "/";
  TString myPt = pt; if (myPt != "") myPt += "/";
  TString myCat = cat; if (myCat != "") myCat += "/";
  TString myTag = tag; if (myTag != "") myTag = "_"+myTag;
  return myPt+myCat+mySyst+name+tag;
}

TString GbbTupleAna::getPtLabel(float muojet_pt,float nonmuojet_pt){
  
  TString label="";

  if(muojet_pt<m_muojet_pt_bins[0]) label+=Form("mjpt_l%.0f_",m_muojet_pt_bins[0]);
  else if(muojet_pt>=m_muojet_pt_bins[m_muojet_pt_bins.size()-1]) label+=Form("mjpt_g%.0f_",m_muojet_pt_bins[m_muojet_pt_bins.size()-1]); 
  else{
    for(int i_m=0; i_m<m_muojet_pt_bins.size()-1; i_m++){
      if(muojet_pt>=m_muojet_pt_bins[i_m] && muojet_pt<m_muojet_pt_bins[i_m+1]){
        label+=Form("mjpt_g%.0fl%.0f_",m_muojet_pt_bins[i_m],m_muojet_pt_bins[i_m+1]);
      }
    }
  }
  
  if(nonmuojet_pt<m_nonmuojet_pt_bins[0]) label+=Form("nmjpt_l%.0f",m_nonmuojet_pt_bins[0]); 
  else if(nonmuojet_pt>=m_nonmuojet_pt_bins[m_nonmuojet_pt_bins.size()-1]) label+=Form("nmjpt_g%.0f",m_nonmuojet_pt_bins[m_nonmuojet_pt_bins.size()-1]);
  else{
    
    for(int i_nm=0; i_nm<m_nonmuojet_pt_bins.size()-1; i_nm++){
      if(nonmuojet_pt>=m_nonmuojet_pt_bins[i_nm] && nonmuojet_pt<m_nonmuojet_pt_bins[i_nm+1]){
        label+=Form("nmjpt_g%.0fl%.0f",m_nonmuojet_pt_bins[i_nm],m_nonmuojet_pt_bins[i_nm+1]);
      }
    }   
  }
  return label;
}


TString GbbTupleAna::getFatjetPtLabel(float fatjet_pt){
  TString label="";

  if(fatjet_pt<m_fatjet_pt_bins[0]) label+=Form("fjpt_l%.0f",m_fatjet_pt_bins[0]);
  else if(fatjet_pt>=m_fatjet_pt_bins[m_fatjet_pt_bins.size()-1]) label+=Form("fjpt_g%.0f",m_fatjet_pt_bins[m_fatjet_pt_bins.size()-1]); 
  else{
    
    for(int i_m=0; i_m<m_fatjet_pt_bins.size()-1; i_m++){
      if(fatjet_pt>=m_fatjet_pt_bins[i_m] && fatjet_pt<m_fatjet_pt_bins[i_m+1]){
        label+=Form("fjpt_g%.0fl%.0f",m_fatjet_pt_bins[i_m],m_fatjet_pt_bins[i_m+1]);
      }
    }
  }
  return label;
}


TString GbbTupleAna::getFatjetPhiLabel(float fatjet_phi){
  TString label="";

  std::vector<double> fj_phi_bins={-2.,-1.,0.,1.,2.};

  if(fatjet_phi<fj_phi_bins[0]) label+=Form("fjphi_l%.0f",fj_phi_bins[0]);
  else if(fatjet_phi>=fj_phi_bins[fj_phi_bins.size()-1]) label+=Form("fjphi_g%.0f",fj_phi_bins[fj_phi_bins.size()-1]); 
  else{
    
    for(int i_m=0; i_m<fj_phi_bins.size()-1; i_m++){
      if(fatjet_phi>=fj_phi_bins[i_m] && fatjet_phi<fj_phi_bins[i_m+1]){
        label+=Form("fjphi_g%.0fl%.0f",fj_phi_bins[i_m],fj_phi_bins[i_m+1]);
      }
    }
  }
  return label;
}


TString GbbTupleAna::getFatjetEtaLabel(float fatjet_eta){
  TString label="";

  std::vector<double> fj_eta_bins={-2.,-1.,0.,1.,2.};

  if(fatjet_eta<fj_eta_bins[0]) label+=Form("fjeta_l%.0f",fj_eta_bins[0]);
  else if(fatjet_eta>=fj_eta_bins[fj_eta_bins.size()-1]) label+=Form("fjeta_g%.0f",fj_eta_bins[fj_eta_bins.size()-1]); 
  else{
    
    for(int i_m=0; i_m<fj_eta_bins.size()-1; i_m++){
      if(fatjet_eta>=fj_eta_bins[i_m] && fatjet_eta<fj_eta_bins[i_m+1]){
        label+=Form("fjeta_g%.0fl%.0f",fj_eta_bins[i_m],fj_eta_bins[i_m+1]);
      }
    }
  }
  return label;
}

void GbbTupleAna::FillReweightInfo(int i_trig_jet, float event_weight, TString nametag){
  
  m_HistogramService->FastFillTH2D("reweight_trigjet_pt_eta_"+nametag,this->jet_pt->at(i_trig_jet)/1e3,this->jet_eta->at(i_trig_jet),75,0,2500.,10,-2.5,2.5,event_weight);

}

void GbbTupleAna::FillFatReweightInfo(float trigfat_pt,float trigfat_eta, float event_weight, TString nametag){
  
  m_HistogramService->FastFillTH2D("reweight_trigfatjet_pt_eta_"+nametag,trigfat_pt/1e3,trigfat_eta,75,0,2500.,10,-2.5,2.5,event_weight);

}

void GbbTupleAna::FillTrackJetProperties(GbbCandidate* gbbcand, float event_weight, TString nametag){

 if(!nametag.IsNull())nametag.Prepend("_");

 int muojet_truth=this->trkjet_truth->at(gbbcand->muojet_index);
 int nonmuojet_truth=this->trkjet_truth->at(gbbcand->nonmuojet_index);

 TString muo_hist_name=this->eve_isMC ? m_trkjet_cat.at(this->getTruthType(muojet_truth)) : TString("Data");
 TString nonmuo_hist_name=this->eve_isMC ? m_trkjet_cat.at(this->getTruthType(nonmuojet_truth)) : TString("Data");

 //m_HistogramService->FastFillTH1D("h"+muo_hist_name+m_SysVarName+"_mjpt"+nametag,this->trkjet_pt->at(gbbcand->muojet_index)/1e3,20,0.,500.,event_weight);
 // m_HistogramService->FastFillTH1D("h"+nonmuo_hist_name+m_SysVarName+"_nmjpt"+nametag,this->trkjet_pt->at(gbbcand->nonmuojet_index)/1e3,20,0.,500.,event_weight);

 // m_HistogramService->FastFillTH1D("h"+muo_hist_name+m_SysVarName+"_mjeta"+nametag,this->trkjet_eta->at(gbbcand->muojet_index),10,-2.5,2.5,event_weight);
 // m_HistogramService->FastFillTH1D("h"+nonmuo_hist_name+m_SysVarName+"_nmjeta"+nametag,this->trkjet_eta->at(gbbcand->nonmuojet_index),10,-2.5,2.5,event_weight);

  TString dijet_hist_name=this->eve_isMC ? m_ditrkjet_cat.at(this->getCategoryNumber(muojet_truth,nonmuojet_truth,m_doMergeDiTrkjetCat)) : TString("Data") ;
  
  if(dijet_hist_name.EqualTo("other")) return;

  //m_HistogramService->FastFillTH1D("h"+dijet_hist_name+m_SysVarName+"_mjpt"+nametag,this->trkjet_pt->at(gbbcand->muojet_index)/1e3,20,0.,500.,event_weight);
  //m_HistogramService->FastFillTH1D("h"+dijet_hist_name+m_SysVarName+"_nmjpt"+nametag,this->trkjet_pt->at(gbbcand->nonmuojet_index)/1e3,20,0.,500.,event_weight);

  TString ptlabel=this->getPtLabel(this->trkjet_pt->at(gbbcand->muojet_index)/1e3,this->trkjet_pt->at(gbbcand->nonmuojet_index)/1e3);

  m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,dijet_hist_name,ptlabel,"mjpt",nametag),
   this->trkjet_pt->at(gbbcand->muojet_index)/1e3,250,0.,500.,event_weight);
  m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,dijet_hist_name,ptlabel,"nmjpt",nametag),
   this->trkjet_pt->at(gbbcand->nonmuojet_index)/1e3,250,0.,500.,event_weight);
  
  m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,dijet_hist_name,ptlabel,"mjeta",nametag),
   this->trkjet_eta->at(gbbcand->muojet_index)/1e3,100,-2.5,2.5,event_weight);
  m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,dijet_hist_name,ptlabel,"nmjeta",nametag),
   this->trkjet_eta->at(gbbcand->nonmuojet_index)/1e3,100,-2.5,2.5,event_weight);
}

void GbbTupleAna::FillTemplates(GbbCandidate* gbbcand, float event_weight,TString nametag){
  
  if(!nametag.IsNull())nametag.Prepend("_");

  int muojet_truth=this->trkjet_truth->at(gbbcand->muojet_index);
  int nonmuojet_truth=this->trkjet_truth->at(gbbcand->nonmuojet_index);
  
  //inclusive in Pt
  TString hist_name=this->eve_isMC ? m_ditrkjet_cat.at(this->getCategoryNumber(muojet_truth,nonmuojet_truth,m_doMergeDiTrkjetCat)) : TString("Data");

  //calculate mean sd0 for leading three tracks
  trkjetSd0Info  muojet_sd0Info=this->getTrkjetAssocSd0Info(gbbcand->muojet_index,m_doTrackSmearing,"nominal",3);
  trkjetSd0Info  nonmuojet_sd0Info=this->getTrkjetAssocSd0Info(gbbcand->nonmuojet_index,m_doTrackSmearing,"nominal",3);
  float muojet_maxsd0 = muojet_sd0Info.meanSd0_ptSort;
  float nonmuojet_maxsd0 = nonmuojet_sd0Info.meanSd0_ptSort;

  float muojet_maxsd0DR = muojet_sd0Info.maxSd0_dR;
  float nonmuojet_maxsd0DR = nonmuojet_sd0Info.maxSd0_dR;

  if(TMath::Abs(muojet_maxsd0+99)<1e-5 || TMath::Abs(nonmuojet_maxsd0+99)<1e-5) return; //associated tracks do not fulfil selection cuts


  m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,hist_name,"Incl","mjmaxSd0",nametag),
   muojet_sd0Info.maxSd0,50,-40,80,event_weight);
  m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,hist_name,"Incl","nmjmaxSd0",nametag),
   nonmuojet_sd0Info.maxSd0,50,-40,80,event_weight);
  m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,hist_name,"Incl","mjmeanSd0",nametag),
   muojet_sd0Info.meanSd0_ptSort,50,-40,80,event_weight);
  m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,hist_name,"Incl","nmjmeanSd0",nametag),
   nonmuojet_sd0Info.meanSd0_ptSort,50,-40,80,event_weight);
  m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,hist_name,"Incl","mjsubSd0",nametag),
   muojet_sd0Info.subSd0,50,-40,80,event_weight);
  m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,hist_name,"Incl","nmjsubSd0",nametag),
   nonmuojet_sd0Info.subSd0,50,-40,80,event_weight);
  m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,hist_name,"Incl","mjthirdSd0",nametag),
   muojet_sd0Info.thirdSd0,50,-40,80,event_weight);
  m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,hist_name,"Incl","nmjthirdSd0",nametag),
   nonmuojet_sd0Info.thirdSd0,50,-40,80,event_weight);


  //In Pt Bins
  TString ptlabel=this->getPtLabel(this->trkjet_pt->at(gbbcand->muojet_index)/1e3,this->trkjet_pt->at(gbbcand->nonmuojet_index)/1e3);
  
  if(m_doRandomSplitting){
    
    if(m_doFillMujet) m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,hist_name,ptlabel,"_mjmaxSd0",nametag), muojet_maxsd0,80,-40,80,event_weight);
    else m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,hist_name,ptlabel,"_nmjmaxSd0",nametag), nonmuojet_maxsd0,80,-40,80,event_weight);
    
  }else{

    m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,hist_name,ptlabel,"_mjmaxSd0",nametag),
     muojet_maxsd0,80,-40,80,event_weight);
    m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,hist_name,ptlabel,"_nmjmaxSd0",nametag),
     nonmuojet_maxsd0,80,-40,80,event_weight);

    m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,hist_name,"","mjmaxSd0DR",nametag),
     muojet_maxsd0DR,25,0.,0.5,event_weight);
    m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,hist_name,"","nmjmaxSd0DR",nametag),
     nonmuojet_maxsd0DR,25,0.,0.5,event_weight);

  }

  //in Fat jet pt bins
  /*TString fatptlabel=this->getFatjetPtLabel(this->fat_pt->at(gbbcand->fat_index)/1e3);
  m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_"+fatptlabel+"_mjmaxSd0"+nametag,muojet_maxsd0,80,-10,10,event_weight);
  m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_"+fatptlabel+"_nmjmaxSd0"+nametag,nonmuojet_maxsd0,80,-10,10,event_weight);
  */

  //Fill fat jet histograms for SF calculation
  
  if(m_isNominal){
    
    m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,hist_name,ptlabel,"_fjpt",nametag),
     this->fat_pt->at(gbbcand->fat_index)/1e3,250,0.,1000.,event_weight);
    //m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_"+fatptlabel+"_fjpt"+nametag,this->fat_pt->at(gbbcand->fat_index)/1e3,250,0.,1000.,event_weight);
    
    if(this->eve_isMC && this->getScaledFatPt(this->fat_pt->at(gbbcand->fat_index)/1e3)>250.) {
      m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,hist_name,ptlabel,"_fjptsc",nametag),
       this->getScaledFatPt(this->fat_pt->at(gbbcand->fat_index)/1e3),250,0.,1000.,event_weight);
    } else if(!this->eve_isMC) {
      m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,hist_name,ptlabel,"_fjptsc",nametag),
       this->fat_pt->at(gbbcand->fat_index)/1e3,250,0.,1000.,event_weight);
    }
       

    
    if(this->eve_isMC && m_isNominal){
      
      //calculate mean sd0 for leading three tracks
      trkjetSd0Info  muojet_sd0Info_up=this->getTrkjetAssocSd0Info(gbbcand->muojet_index,m_doTrackSmearing,"up",3);
      trkjetSd0Info  nonmuojet_sd0Info_up=this->getTrkjetAssocSd0Info(gbbcand->nonmuojet_index,m_doTrackSmearing,"up",3);

      float muojet_maxsd0_up = muojet_sd0Info_up.meanSd0_ptSort;
      float nonmuojet_maxsd0_up = nonmuojet_sd0Info_up.meanSd0_ptSort;
 
      m_HistogramService->FastFillTH1D( makePlotName("SD0Smear__1up",hist_name,ptlabel,"mjmaxSd0",nametag),
       muojet_maxsd0_up,80,-40,80,event_weight);
      m_HistogramService->FastFillTH1D( makePlotName("SD0Smear__1up",hist_name,ptlabel,"nmjmaxSd0",nametag),
       nonmuojet_maxsd0_up,80,-40,80,event_weight);
      
      //calculate mean sd0 for leading three tracks
      trkjetSd0Info  muojet_sd0Info_down=this->getTrkjetAssocSd0Info(gbbcand->muojet_index,m_doTrackSmearing,"down",3);
      trkjetSd0Info  nonmuojet_sd0Info_down=this->getTrkjetAssocSd0Info(gbbcand->nonmuojet_index,m_doTrackSmearing,"down",3);

      float muojet_maxsd0_down = muojet_sd0Info_down.meanSd0_ptSort;
      float nonmuojet_maxsd0_down = nonmuojet_sd0Info_down.meanSd0_ptSort;
  
      m_HistogramService->FastFillTH1D( makePlotName("SD0SMEAR__1down",hist_name,ptlabel,"mjmaxSd0",nametag),
       muojet_maxsd0_down,80,-40,80,event_weight);
      m_HistogramService->FastFillTH1D( makePlotName("SD0SMEAR__1down",hist_name,ptlabel,"nmjmaxSd0",nametag),
       nonmuojet_maxsd0_down,80,-40,80,event_weight);
      
      bool hasConversion=false, hasHadMatInt=false, hasLightLongLived=false, hasNoTruthMu=false;
      this->getSystematicsFlags(gbbcand, hasConversion, hasHadMatInt, hasLightLongLived, hasNoTruthMu);
      if(hasConversion){
        m_HistogramService->FastFillTH1D( makePlotName("Conversion__1up",hist_name,ptlabel,"mjmaxSd0",nametag),
         muojet_maxsd0,80,-40,80,event_weight*1.1);
        m_HistogramService->FastFillTH1D( makePlotName("Conversion__1up",hist_name,ptlabel,"nmjmaxSd0",nametag),
         nonmuojet_maxsd0,80,-40,80,event_weight*1.1);
        m_HistogramService->FastFillTH1D( makePlotName("Conversion__1down",hist_name,ptlabel,"mjmaxSd0",nametag),
         muojet_maxsd0,80,-40,80,event_weight*0.9);
        m_HistogramService->FastFillTH1D( makePlotName("Conversion__1down",hist_name,ptlabel,"nmjmaxSd0",nametag),
         nonmuojet_maxsd0,80,-40,80,event_weight*0.9);  
      }else{
        m_HistogramService->FastFillTH1D( makePlotName("Conversion__1up",hist_name,ptlabel,"mjmaxSd0",nametag),
         muojet_maxsd0,80,-40,80,event_weight);
        m_HistogramService->FastFillTH1D( makePlotName("Conversion__1up",hist_name,ptlabel,"nmjmaxSd0",nametag),
         nonmuojet_maxsd0,80,-40,80,event_weight);
        m_HistogramService->FastFillTH1D( makePlotName("Conversion__1down",hist_name,ptlabel,"mjmaxSd0",nametag),
         muojet_maxsd0,80,-40,80,event_weight);
        m_HistogramService->FastFillTH1D( makePlotName("Conversion__1down",hist_name,ptlabel,"nmjmaxSd0",nametag),
         nonmuojet_maxsd0,80,-40,80,event_weight);
      }

      if(hasHadMatInt){
        m_HistogramService->FastFillTH1D( makePlotName("HadMatInt__1up",hist_name,ptlabel,"mjmaxSd0",nametag),
         muojet_maxsd0,80,-40,80,event_weight*1.1);
        m_HistogramService->FastFillTH1D( makePlotName("HadMatInt__1up",hist_name,ptlabel,"nmjmaxSd0",nametag),
         nonmuojet_maxsd0,80,-40,80,event_weight*1.1);
        m_HistogramService->FastFillTH1D( makePlotName("HadMatInt__1down",hist_name,ptlabel,"mjmaxSd0",nametag),
         muojet_maxsd0,80,-40,80,event_weight*0.9);
        m_HistogramService->FastFillTH1D( makePlotName("HadMatInt__1down",hist_name,ptlabel,"nmjmaxSd0",nametag),
         nonmuojet_maxsd0,80,-40,80,event_weight*0.9);  
      }else{
        m_HistogramService->FastFillTH1D( makePlotName("HadMatInt__1up",hist_name,ptlabel,"mjmaxSd0",nametag),
         muojet_maxsd0,80,-40,80,event_weight);
        m_HistogramService->FastFillTH1D( makePlotName("HadMatInt__1up",hist_name,ptlabel,"nmjmaxSd0",nametag),
         nonmuojet_maxsd0,80,-40,80,event_weight);
        m_HistogramService->FastFillTH1D( makePlotName("HadMatInt__1down",hist_name,ptlabel,"mjmaxSd0",nametag),
         muojet_maxsd0,80,-40,80,event_weight);
        m_HistogramService->FastFillTH1D( makePlotName("HadMatInt__1down",hist_name,ptlabel,"nmjmaxSd0",nametag),
         nonmuojet_maxsd0,80,-40,80,event_weight);
      }

      if(hasLightLongLived){
        m_HistogramService->FastFillTH1D( makePlotName("LightLongLived__1up",hist_name,ptlabel,"mjmaxSd0",nametag),
         muojet_maxsd0,80,-40,80,event_weight*1.1);
        m_HistogramService->FastFillTH1D( makePlotName("LightLongLived__1up",hist_name,ptlabel,"nmjmaxSd0",nametag),
         nonmuojet_maxsd0,80,-40,80,event_weight*1.1);
        m_HistogramService->FastFillTH1D( makePlotName("LightLongLived__1down",hist_name,ptlabel,"mjmaxSd0",nametag),
         muojet_maxsd0,80,-40,80,event_weight*0.9);
        m_HistogramService->FastFillTH1D( makePlotName("LightLongLived__1down",hist_name,ptlabel,"nmjmaxSd0",nametag),
         nonmuojet_maxsd0,80,-40,80,event_weight*0.9);  
      }else{
        m_HistogramService->FastFillTH1D( makePlotName("LightLongLived__1up",hist_name,ptlabel,"mjmaxSd0",nametag),
         muojet_maxsd0,80,-40,80,event_weight);
        m_HistogramService->FastFillTH1D( makePlotName("LightLongLived__1up",hist_name,ptlabel,"nmjmaxSd0",nametag),
         nonmuojet_maxsd0,80,-40,80,event_weight);
        m_HistogramService->FastFillTH1D( makePlotName("LightLongLived__1down",hist_name,ptlabel,"mjmaxSd0",nametag),
         muojet_maxsd0,80,-40,80,event_weight);
        m_HistogramService->FastFillTH1D( makePlotName("LightLongLived__1down",hist_name,ptlabel,"nmjmaxSd0",nametag),
         nonmuojet_maxsd0,80,-40,80,event_weight);
      }

      if(hasNoTruthMu){
        m_HistogramService->FastFillTH1D( makePlotName("FakeMuons__1up",hist_name,ptlabel,"mjmaxSd0",nametag),
         muojet_maxsd0,80,-40,80,event_weight*3.);
        m_HistogramService->FastFillTH1D( makePlotName("FakeMuons__1up",hist_name,ptlabel,"nmjmaxSd0",nametag),
         nonmuojet_maxsd0,80,-40,80,event_weight*3.);
      }else{
        m_HistogramService->FastFillTH1D( makePlotName("FakeMuons__1up",hist_name,ptlabel,"mjmaxSd0",nametag),
         muojet_maxsd0,80,-40,80,event_weight*3.);
        m_HistogramService->FastFillTH1D( makePlotName("FakeMuons__1up",hist_name,ptlabel,"nmjmaxSd0",nametag),
         nonmuojet_maxsd0,80,-40,80,event_weight*3.);
      }
    }
  }
}

void GbbTupleAna::FillMCStatsInfo(GbbCandidate* gbbcand, TString nametag){
  
  if(!nametag.IsNull())nametag.Prepend("_");

  int muojet_truth=this->trkjet_truth->at(gbbcand->muojet_index);
  int nonmuojet_truth=this->trkjet_truth->at(gbbcand->nonmuojet_index);
  
  TString dijet_hist_name=this->eve_isMC ? m_ditrkjet_cat.at(this->getCategoryNumber(muojet_truth,nonmuojet_truth,m_doMergeDiTrkjetCat)) : TString("Data");
  
  m_HistogramService->FastFillTH2D( makePlotName("",dijet_hist_name,"","mjpt_vs_nmjpt_unweighted",nametag),
   this->trkjet_pt->at(gbbcand->muojet_index)/1e3,this->trkjet_pt->at(gbbcand->nonmuojet_index)/1e3,50,0.,200.,50,0.,200.,1.);

}

void GbbTupleAna::FillFatJetProperties(GbbCandidate* gbbcand, float event_weight, TString nametag){

  if(!nametag.IsNull())nametag.Prepend("_");
  
  int muojet_truth=this->trkjet_truth->at(gbbcand->muojet_index);
  int nonmuojet_truth=this->trkjet_truth->at(gbbcand->nonmuojet_index);
  
  TString hist_name=this->eve_isMC ? m_ditrkjet_cat.at(this->getCategoryNumber(muojet_truth,nonmuojet_truth,m_doMergeDiTrkjetCat)) : TString("Data");

  //In Pt Bins
  TString ptlabel=this->getPtLabel(this->trkjet_pt->at(gbbcand->muojet_index)/1e3,this->trkjet_pt->at(gbbcand->nonmuojet_index)/1e3);
  
  //inclusive only for pt and eta
  if(m_isNominal) {
    m_HistogramService->FastFillTH1D( makePlotName("Nom","Incl","Incl","fjpt",nametag),
     this->fat_pt->at(gbbcand->fat_index)/1e3,250,0.,1000.,event_weight);
    m_HistogramService->FastFillTH1D( makePlotName("Nom","Incl","Incl","fjeta",nametag),
     this->fat_eta->at(gbbcand->fat_index),10,-2.5,2.5,event_weight);

    m_HistogramService->FastFillTH1D( makePlotName("Nom",hist_name,"Incl","fjpt",nametag),
     this->fat_pt->at(gbbcand->fat_index)/1e3,250,0.,1000.,event_weight);
    //m_HistogramService->FastFillTH1D( makePlotName("Nom",hist_name,"Incl","fjptsc",nametag),
    // this->getScaledFatPt(this->fat_pt->at(gbbcand->fat_index)/1e3),250,0.,1000.,event_weight);
    m_HistogramService->FastFillTH1D( makePlotName("Nom",hist_name,"Incl","fjeta",nametag),
     this->fat_eta->at(gbbcand->fat_index),10,-2.5,2.5,event_weight);
  }


  if(!m_isNominal){
    m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,hist_name,ptlabel,"fjpt",nametag),
     this->fat_pt->at(gbbcand->fat_index)/1e3,250,0.,1000.,event_weight);

    if(this->eve_isMC && this->getScaledFatPt(this->fat_pt->at(gbbcand->fat_index)/1e3)>250.) {
      m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,hist_name,ptlabel,"fjptsc",nametag),
       this->getScaledFatPt(this->fat_pt->at(gbbcand->fat_index)/1e3),250,0.,1000.,event_weight);
    } else if(!this->eve_isMC) {
      m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,hist_name,ptlabel,"fjptsc",nametag),
       this->fat_pt->at(gbbcand->fat_index)/1e3,250,0.,1000.,event_weight);
    }
  
  } else if(nametag.Contains("ANTITAG")) {
    m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,hist_name,ptlabel,"fjpt",nametag),
     this->fat_pt->at(gbbcand->fat_index)/1e3,250,0.,1000.,event_weight);
  }
  m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,hist_name,ptlabel,"fjD2",nametag),
   this->fat_D2->at(gbbcand->fat_index),250,0.,4.5,event_weight);
  m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,hist_name,ptlabel,"fjtau21",nametag),
   this->fat_tau21->at(gbbcand->fat_index),250,0.,0.9,event_weight);
  
  TLorentzVector fatjet;
  fatjet.SetPtEtaPhiE(this->fat_pt->at(gbbcand->fat_index),this->fat_eta->at(gbbcand->fat_index),this->fat_phi->at(gbbcand->fat_index),this->fat_E->at(gbbcand->fat_index));
  
  m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,hist_name,ptlabel,"fjm",nametag),
   fatjet.M()/1e3,25,0.,500.,event_weight);

  if(nametag.IsNull()){
    
    //EtaPhiBins
    //TString fatphilabel=getFatjetPhiLabel(this->fat_phi->at(gbbcand->fat_index));
    //TString fatetalabel=getFatjetEtaLabel(this->fat_eta->at(gbbcand->fat_index));
    //if(m_isNominal) m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_"+ptlabel+"_"+fatetalabel+"_"+fatphilabel+"_fjpt"+nametag,this->fat_pt->at(gbbcand->fat_index)/1e3,250,0.,1000.,event_weight);
    
  }

}

void GbbTupleAna::FillAdvancedProperties(GbbCandidate* gbbcand, int i_trig_jet, float event_weight,TString nametag){
  
  TLorentzVector mujet,trigjet;
  
  if(!nametag.IsNull())nametag.Prepend("_");
  
  //Topology: Plot DR(fatjet,muonjet)
  mujet.SetPtEtaPhiE(this->trkjet_pt->at(gbbcand->muojet_index),this->trkjet_eta->at(gbbcand->muojet_index),this->trkjet_phi->at(gbbcand->muojet_index),this->trkjet_E->at(gbbcand->muojet_index));
  
  trigjet.SetPtEtaPhiE(this->jet_pt->at(i_trig_jet),this->jet_eta->at(i_trig_jet),this->jet_phi->at(i_trig_jet),this->jet_E->at(i_trig_jet));
  
  m_HistogramService->FastFillTH1D("h_trjmjDR"+nametag,mujet.DeltaR(trigjet),50.,0.,5.,event_weight);
  
  //Topology: Fill Trigger Jet Pt and Eta
  m_HistogramService->FastFillTH1D("h_trjpt"+nametag,this->jet_pt->at(i_trig_jet)/1e3,25,0.,1000.,event_weight);
  m_HistogramService->FastFillTH1D("h_trjeta"+nametag,this->jet_eta->at(i_trig_jet),10,-2.5,2.5,event_weight);
  
}

void GbbTupleAna::FillTriggerTurnOnHistograms(int i_trigjet, float event_weight){

  //Fill Histograms for Trigger turn-on curves
 
  //if((this->eve_HLT_j60))m_HistogramService->FastFillTH1D("trigjet_pt_passHLT60",this->jet_pt->at(i_trigjet)/1e3,100,0.,1000.,event_weight);
  if((this->eve_HLT_j85))m_HistogramService->FastFillTH1D("trigjet_pt_passHLT85",this->jet_pt->at(i_trigjet)/1e3,100,0.,1000.,event_weight);
  if((this->eve_HLT_j110))m_HistogramService->FastFillTH1D("trigjet_pt_passHLT110",this->jet_pt->at(i_trigjet)/1e3,100,0.,1000.,event_weight);
  if((this->eve_HLT_j150))m_HistogramService->FastFillTH1D("trigjet_pt_passHLT150",this->jet_pt->at(i_trigjet)/1e3,100,0.,1000.,event_weight);
  if((this->eve_HLT_j175))m_HistogramService->FastFillTH1D("trigjet_pt_passHLT175",this->jet_pt->at(i_trigjet)/1e3,100,0.,1000.,event_weight);
  if((this->eve_HLT_j260))m_HistogramService->FastFillTH1D("trigjet_pt_passHLT260",this->jet_pt->at(i_trigjet)/1e3,100,0.,1000.,event_weight);
  if((this->eve_HLT_j360)) m_HistogramService->FastFillTH1D("trigjet_pt_passHLT360",this->jet_pt->at(i_trigjet)/1e3,100,0.,1000.,event_weight);
  if((this->eve_HLT_j380)) m_HistogramService->FastFillTH1D("trigjet_pt_passHLT380",this->jet_pt->at(i_trigjet)/1e3,100,0.,1000.,event_weight);
  m_HistogramService->FastFillTH1D("trigjet_pt_noHLTreq",this->jet_pt->at(i_trigjet)/1e3,100,0.,1000.,event_weight);

}
