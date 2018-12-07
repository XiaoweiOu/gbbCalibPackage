/*
 * GbbFillHelp.cxx
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
#include <functional>

TString GbbTupleAna::makeMuJetPlotName(GbbCandidate* gbbcand, const TString var) {
  int muojet_truth = this->trkjet_truth->at(gbbcand->muojet_index);
  TString muo_flav = this->eve_isMC ? m_config->GetFlavour(muojet_truth) : TString("Data");
  TString ptlabel = m_config->GetMuJetLabel(this->trkjet_pt->at(gbbcand->muojet_index)/1e3);
  return m_config->GetMCHistName(m_SysVarName,ptlabel,muo_flav,var);
}

TString GbbTupleAna::makeNonMuJetPlotName(GbbCandidate* gbbcand, const TString var) {
  int nonmuojet_truth = this->trkjet_truth->at(gbbcand->nonmuojet_index);
  TString nonmuo_flav = this->eve_isMC ? m_config->GetFlavour(nonmuojet_truth) : TString("Data");
  TString ptlabel = m_config->GetNonMuJetLabel(this->trkjet_pt->at(gbbcand->nonmuojet_index)/1e3);
  return m_config->GetMCHistName(m_SysVarName,ptlabel,nonmuo_flav,var);
}

TString GbbTupleAna::makeDiJetPlotName(GbbCandidate* gbbcand, const TString var) {
  int muojet_truth = this->trkjet_truth->at(gbbcand->muojet_index);
  int nonmuojet_truth = this->trkjet_truth->at(gbbcand->nonmuojet_index);
  TString dijet_flav = this->eve_isMC ? m_config->GetFlavourPair(muojet_truth,nonmuojet_truth) : TString("Data");
  TString ptlabel = m_config->GetDiTrkJetLabel(this->trkjet_pt->at(gbbcand->muojet_index)/1e3,
                                               this->trkjet_pt->at(gbbcand->nonmuojet_index)/1e3);
  return m_config->GetMCHistName(m_SysVarName,ptlabel,dijet_flav,var);
}

TString GbbTupleAna::makeFatJetPlotName(GbbCandidate* gbbcand, const TString var) {
  int muojet_truth = this->trkjet_truth->at(gbbcand->muojet_index);
  int nonmuojet_truth = this->trkjet_truth->at(gbbcand->nonmuojet_index);
  TString dijet_flav = this->eve_isMC ? m_config->GetFlavourPair(muojet_truth,nonmuojet_truth) : TString("Data");
  TString ptlabel = m_config->GetFatJetLabel(this->fat_pt->at(gbbcand->fat_index)/1e3);
  return m_config->GetMCHistName(m_SysVarName,ptlabel,dijet_flav,var);
}

TString GbbTupleAna::makeInclDiJetPlotName(GbbCandidate* gbbcand, const TString var) {
  int muojet_truth = this->trkjet_truth->at(gbbcand->muojet_index);
  int nonmuojet_truth = this->trkjet_truth->at(gbbcand->nonmuojet_index);
  TString dijet_flav = this->eve_isMC ? m_config->GetFlavourPair(muojet_truth,nonmuojet_truth) : TString("Data");
  return m_config->GetMCHistName(m_SysVarName,"Incl",dijet_flav,var);
}

void GbbTupleAna::FillReweightInfo(int i_trig_jet, float event_weight, TString nametag){
  
  m_HistSvc->FastFillTH2D("reweight_trigjet_pt_eta_"+nametag,
   ";trigger-matched jet p_{T} [GeV];trigger-matched jet #eta",
   this->jet_pt->at(i_trig_jet)/1e3,this->jet_eta->at(i_trig_jet),75,0,2500.,10,-2.5,2.5,event_weight);

}

void GbbTupleAna::FillFatReweightInfo(float trigfat_pt,float trigfat_eta, float event_weight, TString nametag){
  
  m_HistSvc->FastFillTH2D("reweight_trigfatjet_pt_eta_"+nametag,
   ";large-R jet p_{T} [GeV];large-R jet #eta",
   trigfat_pt/1e3,trigfat_eta,75,0,2500.,10,-2.5,2.5,event_weight);

}

void GbbTupleAna::FillTrackJetProperties(GbbCandidate* gbbcand, float event_weight, TString nametag){

  if (!nametag.IsNull()) nametag.Prepend("_");

  // Plot track-jet kinematics with single-flavour label
  m_HistSvc->FastFillTH1D( makeMuJetPlotName(gbbcand,"mjpt"+nametag),";muon-jet p_{T} [GeV];Events/2 GeV;",
   this->trkjet_pt->at(gbbcand->muojet_index)/1e3,250,0.,500.,event_weight);
  m_HistSvc->FastFillTH1D( makeMuJetPlotName(gbbcand,"mjeta"+nametag),";muon-jet #eta;Events/0.2;",
   this->trkjet_eta->at(gbbcand->muojet_index)/1e3,100,-2.5,2.5,event_weight);

  m_HistSvc->FastFillTH1D( makeNonMuJetPlotName(gbbcand,"nmjpt"+nametag),";non-muon-jet p_{T} [GeV];Events/2 GeV;",
   this->trkjet_pt->at(gbbcand->nonmuojet_index)/1e3,250,0.,500.,event_weight);
  m_HistSvc->FastFillTH1D( makeNonMuJetPlotName(gbbcand,"nmjeta"+nametag),";non-muon-jet #eta;Events/0.2;",
   this->trkjet_eta->at(gbbcand->nonmuojet_index)/1e3,100,-2.5,2.5,event_weight);

  // Plot track-jet kinematics with di-flavour label
  m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"mjpt"+nametag),";muon-jet p_{T} [GeV];Events/2 GeV;",
   this->trkjet_pt->at(gbbcand->muojet_index)/1e3,250,0.,500.,event_weight);
  m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"nmjpt"+nametag),";non-muon-jet p_{T} [GeV];Events/2 GeV;",
   this->trkjet_pt->at(gbbcand->nonmuojet_index)/1e3,250,0.,500.,event_weight);
  
  m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"mjeta"+nametag),";muon-jet #eta;Events/0.2;",
   this->trkjet_eta->at(gbbcand->muojet_index)/1e3,100,-2.5,2.5,event_weight);
  m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"nmjeta"+nametag),";non-muon-jet #eta;Events/0.2;",
   this->trkjet_eta->at(gbbcand->nonmuojet_index)/1e3,100,-2.5,2.5,event_weight);

  m_HistSvc->FastFillTH2D( makeDiJetPlotName(gbbcand,"mjptVsnmjpt"+nametag),
    ";muon-jet p_{T} [GeV];non-muon jet p_{T} [GeV];",
    this->trkjet_pt->at(gbbcand->muojet_index)/1e3,this->trkjet_pt->at(gbbcand->nonmuojet_index)/1e3,
    250,0.,500.,250,0.,500.,event_weight
  );
}

void GbbTupleAna::FillSd0Plots(trkjetSd0Info muSd0Info, trkjetSd0Info nonmuSd0Info, float event_weight, std::function<TString (TString)> namingFunc) {
  // Using the giving plot naming function to make plots of all the requested s_d0 variables
  m_HistSvc->FastFillTH1D( namingFunc("mjmaxSd0"),";muon-jet leading |s_{d0}|;Events/2;",
   muSd0Info.maxSd0,60,-40.,80.,event_weight);
  m_HistSvc->FastFillTH1D( namingFunc("nmjmaxSd0"),";non-muon-jet leading |s_{d0}|;Events/2;",
   nonmuSd0Info.maxSd0,60,-40.,80.,event_weight);
  m_HistSvc->FastFillTH1D( namingFunc("mjsubSd0"),";muon-jet sub-leading |s_{d0}|;Events/2;",
   muSd0Info.subSd0,60,-40.,80.,event_weight);
  m_HistSvc->FastFillTH1D( namingFunc("nmjsubSd0"),";non-muon-jet sub-leading |s_{d0}|;Events/2;",
   nonmuSd0Info.subSd0,60,-40.,80.,event_weight);
  m_HistSvc->FastFillTH1D( namingFunc("mjthirdSd0"),";muon-jet third-leading |s_{d0}|;Events/2;",
   muSd0Info.thirdSd0,60,-40.,80.,event_weight);
  m_HistSvc->FastFillTH1D( namingFunc("nmjthirdSd0"),";non-muon-jet third-leading |s_{d0}|;Events/2;",
   nonmuSd0Info.thirdSd0,60,-40.,80.,event_weight);
  m_HistSvc->FastFillTH1D( namingFunc("mjmeanSd0"),";muon-jet mean |s_{d0}|;Events/2;",
   muSd0Info.meanSd0_ptSort,60,-40.,80.,event_weight);
  m_HistSvc->FastFillTH1D( namingFunc("nmjmeanSd0"),";non-muon-jet mean |s_{d0}|;Events/2;",
   nonmuSd0Info.meanSd0_ptSort,60,-40.,80.,event_weight);
  m_HistSvc->FastFillTH1D( namingFunc("mjmaxSd0DR"),
   ";muon-jet #Delta R(jet,track_{max |s_{d0}|});Events/0.02;",
   muSd0Info.maxSd0_dR,25,0.,0.5,event_weight);
  m_HistSvc->FastFillTH1D( namingFunc("nmjmaxSd0DR"),
   ";non-muon-jet #Delta R(jet,track_{max |s_{d0}|});Events/0.02;",
   nonmuSd0Info.maxSd0_dR,25,0.,0.5,event_weight);
}

void GbbTupleAna::FillTemplates(GbbCandidate* gbbcand, float event_weight,TString nametag){
  
  if (!nametag.IsNull()) nametag.Prepend("_");

  int muojet_truth=this->trkjet_truth->at(gbbcand->muojet_index);
  int nonmuojet_truth=this->trkjet_truth->at(gbbcand->nonmuojet_index);
  TString dijet_flav = this->eve_isMC ? m_config->GetFlavourPair(muojet_truth,nonmuojet_truth) : TString("Data");

  TString ptlabel = m_config->GetDiTrkJetLabel(this->trkjet_pt->at(gbbcand->muojet_index)/1e3,
                                               this->trkjet_pt->at(gbbcand->nonmuojet_index)/1e3);

  // Calculate sd0 variables 
  trkjetSd0Info muojet_sd0Info=this->getTrkjetAssocSd0Info(gbbcand->muojet_index,m_doTrackSmearing,"nominal",3);
  trkjetSd0Info nonmuojet_sd0Info=this->getTrkjetAssocSd0Info(gbbcand->nonmuojet_index,m_doTrackSmearing,"nominal",3);
  float muojet_maxsd0 = muojet_sd0Info.meanSd0_ptSort;
  float nonmuojet_maxsd0 = nonmuojet_sd0Info.meanSd0_ptSort;

  if(TMath::Abs(muojet_maxsd0+99)<1e-5 || TMath::Abs(nonmuojet_maxsd0+99)<1e-5) return;

  // Make inclusive plots of s_d0 variables
  // Custom lambda function defines names of plots
  FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight, 
   [this,gbbcand,nametag](TString var) { return this->makeInclDiJetPlotName(gbbcand,var+nametag); }
  );

  // Make plots in pT bins
  FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight, 
   [this,gbbcand,nametag](TString var) { return this->makeDiJetPlotName(gbbcand,var+nametag); }
  );

  // Make plots with randomized mu/non-mu jet
  if (m_doRandomSplitting) {
    
    if(m_doFillMujet) {
      m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"meanSd0"+nametag),";mean |s_{d0}|;Events/2;",
       muojet_sd0Info.meanSd0_ptSort,60,-40.,80.,event_weight);
    } else {
      m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"meanSd0"+nametag),";mean |s_{d0}|;Events/2;",
       nonmuojet_sd0Info.meanSd0_ptSort,60,-40.,80.,event_weight);
    }
    
  }

  // Make plots in fatjet pT bins
  FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight, 
   [this,gbbcand,nametag](TString var) { return this->makeFatJetPlotName(gbbcand,var+nametag); }
  );

  // Fill fatjet histograms for SF calculation if not done in FillFatJetProperties
  if (!(m_RunMode & RunMode::FILL_FATJET_PROPERTIES)) {
    m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"fjpt"+nametag),";large-R jet p_{T} [GeV];Events/4 GeV;",
     this->fat_pt->at(gbbcand->fat_index)/1e3,250,0.,1000.,event_weight);
    
    if(m_isMC) {
      m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"fjptsc"+nametag),";scaled large-R jet p_{T} [GeV];Events/4 GeV;",
       this->getScaledFatPt(this->fat_pt->at(gbbcand->fat_index)/1e3),250,0.,1000.,event_weight);
    } else {
      m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"fjptsc"+nametag),";scaled large-R jet p_{T} [GeV];Events/4 GeV;",
       this->fat_pt->at(gbbcand->fat_index)/1e3,250,0.,1000.,event_weight);
    }
  }
    
  if (m_isNominal && m_isMC) {

    auto makeSysNamingFunc = [config=this->m_config,dijet_flav,ptlabel,nametag](TString sys)
     -> std::function<TString (TString)> {
      return [config,sys,dijet_flav,ptlabel,nametag](TString var) {
        return config->GetMCHistName(sys,ptlabel,dijet_flav,var+nametag);
      };
    };

    // Calculate sd0 variables with up variation
    trkjetSd0Info  muojet_sd0Info_up=this->getTrkjetAssocSd0Info(gbbcand->muojet_index,m_doTrackSmearing,"up",3);
    trkjetSd0Info  nonmuojet_sd0Info_up=this->getTrkjetAssocSd0Info(gbbcand->nonmuojet_index,m_doTrackSmearing,"up",3);
    FillSd0Plots(muojet_sd0Info_up, nonmuojet_sd0Info_up, event_weight, makeSysNamingFunc("SD0Smear__1up"));

    // Calculate sd0 variables with down variation
    trkjetSd0Info  muojet_sd0Info_down=this->getTrkjetAssocSd0Info(gbbcand->muojet_index,m_doTrackSmearing,"down",3);
    trkjetSd0Info  nonmuojet_sd0Info_down=this->getTrkjetAssocSd0Info(gbbcand->nonmuojet_index,m_doTrackSmearing,"down",3);

    FillSd0Plots(muojet_sd0Info_down, nonmuojet_sd0Info_down, event_weight, makeSysNamingFunc("SD0Smear__1down"));
    
    bool hasConversion=false, hasHadMatInt=false, hasLightLongLived=false, hasNoTruthMu=false;
    this->getSystematicsFlags(gbbcand, hasConversion, hasHadMatInt, hasLightLongLived, hasNoTruthMu);

    if(hasConversion){
      FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight*1.1, makeSysNamingFunc("Conversion__1up"));
      FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight*0.9, makeSysNamingFunc("Conversion__1down"));
    }else{
      FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight, makeSysNamingFunc("Conversion__1up"));
      FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight, makeSysNamingFunc("Conversion__1down"));
    }

    if(hasHadMatInt){
      FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight*1.1, makeSysNamingFunc("HadMatInt__1up"));
      FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight*0.9, makeSysNamingFunc("HadMatInt__1down"));
    }else{
      FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight, makeSysNamingFunc("HadMatInt__1up"));
      FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight, makeSysNamingFunc("HadMatInt__1down"));
    }

    if(hasLightLongLived){
      FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight*1.1, makeSysNamingFunc("LightLongLived__1up"));
      FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight*0.9, makeSysNamingFunc("LightLongLived__1down"));
    }else{
      FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight, makeSysNamingFunc("LightLongLived__1up"));
      FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight, makeSysNamingFunc("LightLongLived__1down"));
    }

    if(hasNoTruthMu){
      FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight*3, makeSysNamingFunc("FakeMuons__1up"));
    }else{
      //FIXME: in older code this had same event_weight multiplier as other case. why?
      FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight, makeSysNamingFunc("FakeMuons__1up"));
    }
  } // End of extra systematics
}

void GbbTupleAna::FillMCStatsInfo(GbbCandidate* gbbcand, TString nametag){
  
  if (!nametag.IsNull()) nametag.Prepend("_");
  
  m_HistSvc->FastFillTH2D( makeInclDiJetPlotName(gbbcand,"mjpt_vs_nmjpt_unweighted"+nametag),
   ";muon-jet p_{T} [GeV];non-muon-jet p_{T} [GeV];",
   this->trkjet_pt->at(gbbcand->muojet_index)/1e3,this->trkjet_pt->at(gbbcand->nonmuojet_index)/1e3,
   50,0.,200.,50,0.,200.,1.
  );

}

void GbbTupleAna::FillFatJetProperties(GbbCandidate* gbbcand, float event_weight, TString nametag){

  if (!nametag.IsNull()) nametag.Prepend("_");
  
  int muojet_truth=this->trkjet_truth->at(gbbcand->muojet_index);
  int nonmuojet_truth=this->trkjet_truth->at(gbbcand->nonmuojet_index);
  TString dijet_flav = this->eve_isMC ? m_config->GetFlavourPair(muojet_truth,nonmuojet_truth) : TString("Data");

  TString ptlabel = m_config->GetDiTrkJetLabel(this->trkjet_pt->at(gbbcand->muojet_index)/1e3,
                                               this->trkjet_pt->at(gbbcand->nonmuojet_index)/1e3);
  
  //inclusive only for pt and eta
  if(m_isNominal) {
    m_HistSvc->FastFillTH1D(
     m_config->GetMCHistName(m_SysVarName,"Incl","Incl","fjpt"+nametag),";large-R jet p_{T} [GeV];Events/4 GeV;",
     this->fat_pt->at(gbbcand->fat_index)/1e3,250,0.,1000.,event_weight
    );
    m_HistSvc->FastFillTH1D(
     m_config->GetMCHistName(m_SysVarName,"Incl","Incl","fjeta"+nametag),";large-R jet #eta;Events/0.2;",
     this->fat_eta->at(gbbcand->fat_index),10,-2.5,2.5,event_weight
    );

    m_HistSvc->FastFillTH1D( makeInclDiJetPlotName(gbbcand,"fjpt"+nametag),";large-R jet p_{T} [GeV];Events/4 GeV;",
     this->fat_pt->at(gbbcand->fat_index)/1e3,250,0.,1000.,event_weight);
    //m_HistSvc->FastFillTH1D( makeInclDiJetPlotName(gbbcand,"fjptsc"+nametag),
    // ";scaled large-R jet p_{T} [GeV];Events/4 GeV;",
    // this->getScaledFatPt(this->fat_pt->at(gbbcand->fat_index)/1e3),250,0.,1000.,event_weight);
    m_HistSvc->FastFillTH1D( makeInclDiJetPlotName(gbbcand,"fjeta"+nametag),";large-R jet #eta;Events/0.2;",
     this->fat_eta->at(gbbcand->fat_index),10,-2.5,2.5,event_weight);
  }

  m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"fjpt"+nametag),";large-R jet p_{T} [GeV];Events/4 GeV;",
   this->fat_pt->at(gbbcand->fat_index)/1e3,250,0.,1000.,event_weight);

  if (m_isMC) {
    m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"fjptsc"+nametag),
     ";scaled large-R jet p_{T} [GeV];Events/4 GeV;",
     this->getScaledFatPt(this->fat_pt->at(gbbcand->fat_index)/1e3),250,0.,1000.,event_weight);
  } else {
    m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"fjptsc"+nametag),
     ";scaled large-R jet p_{T} [GeV];Events/4 GeV;",
     this->fat_pt->at(gbbcand->fat_index)/1e3,250,0.,1000.,event_weight);
  }
  
  m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"fjD2"+nametag),";large-R jet D2;Events/0.02;",
   this->fat_D2->at(gbbcand->fat_index),250,0.,5.,event_weight);
  m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"fjtau21"+nametag),";large-R jet #tau_{21};Events/0.005;",
   this->fat_tau21->at(gbbcand->fat_index),260,0.,1.3,event_weight);
  
  TLorentzVector fatjet;
  fatjet.SetPtEtaPhiE(this->fat_pt->at(gbbcand->fat_index),this->fat_eta->at(gbbcand->fat_index),this->fat_phi->at(gbbcand->fat_index),this->fat_E->at(gbbcand->fat_index));
  
  m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"fjm"+nametag),";large-R jet mass [GeV];Events/20 GeV;",
   fatjet.M()/1e3,25,0.,500.,event_weight);

}

void GbbTupleAna::FillAdvancedProperties(GbbCandidate* gbbcand, int i_trig_jet, float event_weight,TString nametag){
  
  TLorentzVector mujet,trigjet;
  
  if (!nametag.IsNull()) nametag.Prepend("_");
  
  //Topology: Plot DR(fatjet,muonjet)
  mujet.SetPtEtaPhiE(this->trkjet_pt->at(gbbcand->muojet_index),this->trkjet_eta->at(gbbcand->muojet_index),this->trkjet_phi->at(gbbcand->muojet_index),this->trkjet_E->at(gbbcand->muojet_index));
  
  trigjet.SetPtEtaPhiE(this->jet_pt->at(i_trig_jet),this->jet_eta->at(i_trig_jet),this->jet_phi->at(i_trig_jet),this->jet_E->at(i_trig_jet));
  
  m_HistSvc->FastFillTH1D(
   m_config->GetMCHistName(m_SysVarName,"Incl","Incl","trjmjDR"+nametag),
   ";#Delta R(trigger-matched jet, muon-jet);Events/0.1;",
   mujet.DeltaR(trigjet),50,0.,5.,event_weight
  );
  
  //Topology: Fill Trigger Jet Pt and Eta
  m_HistSvc->FastFillTH1D(
   m_config->GetMCHistName(m_SysVarName,"Incl","Incl","trjpt"+nametag),
   ";trigger-matched jet p_{T} [GeV];Events/4 GeV;",
   this->jet_pt->at(i_trig_jet)/1e3,250,0.,1000.,event_weight
  );
  m_HistSvc->FastFillTH1D(
   m_config->GetMCHistName(m_SysVarName,"Incl","Incl","trjeta"+nametag),
   ";trigger-matched #eta;Events/0.2;",
   this->jet_eta->at(i_trig_jet),10,-2.5,2.5,event_weight
  );

  m_HistSvc->FastFillTH1D(
   m_config->GetMCHistName(m_SysVarName,"Incl","Incl","trjfjptratio"+nametag),
   ";trig-jet p_{T} / large-R jet p_{T};Events/2;",
   this->jet_pt->at(i_trig_jet)/this->fat_pt->at(gbbcand->fat_index),50,0.,10.,event_weight
  );

  m_HistSvc->FastFillTH2D(
   m_config->GetMCHistName(m_SysVarName,"Incl","Incl","trjptVsfjpt"+nametag),
   ";trig-jet p_{T} [GeV];large-R jet p_{T} [GeV];",
   this->jet_pt->at(i_trig_jet)/1e3,this->fat_pt->at(gbbcand->fat_index)/1e3,
   250,0.,1000.,250,0.,1000.,event_weight
  );
}

void GbbTupleAna::FillTriggerTurnOnHistograms(int i_trigjet, float event_weight){

  //Fill Histograms for Trigger turn-on curves
 
  //if((this->eve_HLT_j60))m_HistSvc->FastFillTH1D("trigjet_pt_passHLT60",this->jet_pt->at(i_trigjet)/1e3,100,0.,1000.,event_weight);
  if((this->eve_HLT_j85))m_HistSvc->FastFillTH1D("trigjet_pt_passHLT85",this->jet_pt->at(i_trigjet)/1e3,100,0.,1000.,event_weight);
  if((this->eve_HLT_j110))m_HistSvc->FastFillTH1D("trigjet_pt_passHLT110",this->jet_pt->at(i_trigjet)/1e3,100,0.,1000.,event_weight);
  if((this->eve_HLT_j150))m_HistSvc->FastFillTH1D("trigjet_pt_passHLT150",this->jet_pt->at(i_trigjet)/1e3,100,0.,1000.,event_weight);
  if((this->eve_HLT_j175))m_HistSvc->FastFillTH1D("trigjet_pt_passHLT175",this->jet_pt->at(i_trigjet)/1e3,100,0.,1000.,event_weight);
  if((this->eve_HLT_j260))m_HistSvc->FastFillTH1D("trigjet_pt_passHLT260",this->jet_pt->at(i_trigjet)/1e3,100,0.,1000.,event_weight);
  if((this->eve_HLT_j360)) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT360",this->jet_pt->at(i_trigjet)/1e3,100,0.,1000.,event_weight);
  if((this->eve_HLT_j380)) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT380",this->jet_pt->at(i_trigjet)/1e3,100,0.,1000.,event_weight);
  m_HistSvc->FastFillTH1D("trigjet_pt_noHLTreq",this->jet_pt->at(i_trigjet)/1e3,100,0.,1000.,event_weight);

}
