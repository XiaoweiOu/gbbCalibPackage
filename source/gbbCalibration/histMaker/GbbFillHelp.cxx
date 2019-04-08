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

  // Plot track-jet kinematics with di-flavour label inclusive in pt
  m_HistSvc->FastFillTH1D( makeInclDiJetPlotName(gbbcand,"mjpt"+nametag),";muon-jet p_{T} [GeV];Events/2 GeV;",
   this->trkjet_pt->at(gbbcand->muojet_index)/1e3,250,0.,500.,event_weight);
  m_HistSvc->FastFillTH1D( makeInclDiJetPlotName(gbbcand,"nmjpt"+nametag),";non-muon-jet p_{T} [GeV];Events/2 GeV;",
   this->trkjet_pt->at(gbbcand->nonmuojet_index)/1e3,250,0.,500.,event_weight);

  m_HistSvc->FastFillTH1D( makeInclDiJetPlotName(gbbcand,"mjeta"+nametag),";muon-jet #eta;Events/0.2;",
   this->trkjet_eta->at(gbbcand->muojet_index)/1e3,100,-2.5,2.5,event_weight);
  m_HistSvc->FastFillTH1D( makeInclDiJetPlotName(gbbcand,"nmjeta"+nametag),";non-muon-jet #eta;Events/0.2;",
   this->trkjet_eta->at(gbbcand->nonmuojet_index)/1e3,100,-2.5,2.5,event_weight);

  m_HistSvc->FastFillTH2D( makeDiJetPlotName(gbbcand,"mjptVsnmjpt"+nametag),
    ";muon-jet p_{T} [GeV];non-muon jet p_{T} [GeV];",
    this->trkjet_pt->at(gbbcand->muojet_index)/1e3,this->trkjet_pt->at(gbbcand->nonmuojet_index)/1e3,
    250,0.,500.,250,0.,500.,event_weight
  );
}

void GbbTupleAna::FillSd0Plots(trkjetSd0Info muSd0Info, trkjetSd0Info nonmuSd0Info, float event_weight, std::function<TString (TString)> namingFunc) {
  // Using the giving plot naming function to make plots of all the requested s_d0 variables
  m_HistSvc->FastFillTH1D( namingFunc("mjmaxSd0"),";muon-jet leading s_{d0};Events/1.5;",
   muSd0Info.maxSd0,80,-40.,80.,event_weight);
  m_HistSvc->FastFillTH1D( namingFunc("nmjmaxSd0"),";non-muon-jet leading s_{d0};Events/1.5;",
   nonmuSd0Info.maxSd0,80,-40.,80.,event_weight);
  m_HistSvc->FastFillTH1D( namingFunc("mjsubSd0"),";muon-jet sub-leading s_{d0};Events/1.5;",
   muSd0Info.subSd0,80,-40.,80.,event_weight);
  m_HistSvc->FastFillTH1D( namingFunc("nmjsubSd0"),";non-muon-jet sub-leading s_{d0};Events/1.5;",
   nonmuSd0Info.subSd0,80,-40.,80.,event_weight);
  m_HistSvc->FastFillTH1D( namingFunc("mjthirdSd0"),";muon-jet third-leading s_{d0};Events/1.5;",
   muSd0Info.thirdSd0,80,-40.,80.,event_weight);
  m_HistSvc->FastFillTH1D( namingFunc("nmjthirdSd0"),";non-muon-jet third-leading s_{d0};Events/1.5;",
   nonmuSd0Info.thirdSd0,80,-40.,80.,event_weight);
  m_HistSvc->FastFillTH1D( namingFunc("mjmeanSd0"),";muon-jet mean s_{d0};Events/1.5;",
   muSd0Info.meanSd0_pt,80,-40.,80.,event_weight);
  m_HistSvc->FastFillTH1D( namingFunc("nmjmeanSd0"),";non-muon-jet mean s_{d0};Events/1.5;",
   nonmuSd0Info.meanSd0_pt,80,-40.,80.,event_weight);

  // Using the giving plot naming function to make plots of extra d0 and sd0 variables
  if(m_RunMode & RunMode::FILL_ADV_PROPERTIES && m_isNominal) {
    m_HistSvc->FastFillTH1D( namingFunc("mjmaxSd0DR"),
     ";muon-jet #Delta R(jet,track_{max s_{d0}});Events/0.02;",
     muSd0Info.maxSd0_dR,25,0.,0.5,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("nmjmaxSd0DR"),
     ";non-muon-jet #Delta R(jet,track_{max s_{d0}});Events/0.02;",
     nonmuSd0Info.maxSd0_dR,25,0.,0.5,event_weight);

    m_HistSvc->FastFillTH1D( namingFunc("mjmaxSd0_pt"),";muon-jet leading track s_{d0};Events/1.5;",
     muSd0Info.maxSd0_pt,80,-40.,80.,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("nmjmaxSd0_pt"),";non-muon-jet leading track s_{d0};Events/1.5;",
     nonmuSd0Info.maxSd0_pt,80,-40.,80.,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("mjsubSd0_pt"),";muon-jet sub-leading track s_{d0};Events/1.5;",
     muSd0Info.subSd0_pt,80,-40.,80.,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("nmjsubSd0_pt"),";non-muon-jet sub-leading track s_{d0};Events/1.5;",
     nonmuSd0Info.subSd0_pt,80,-40.,80.,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("mjthirdSd0_pt"),";muon-jet third-leading track s_{d0};Events/1.5;",
     muSd0Info.thirdSd0_pt,80,-40.,80.,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("nmjthirdSd0_pt"),";non-muon-jet third-leading track s_{d0};Events/1.5;",
     nonmuSd0Info.thirdSd0_pt,80,-40.,80.,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("mjmeanSd0_sd0"),";muon-jet mean s_{d0};Events/1.5;",
     muSd0Info.meanSd0_sd0,80,-40.,80.,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("nmjmeanSd0_sd0"),";non-muon-jet mean s_{d0};Events/1.5;",
     nonmuSd0Info.meanSd0_sd0,80,-40.,80.,event_weight);

    m_HistSvc->FastFillTH1D( namingFunc("mjmaxd0"),";muon-jet leading d0;Events/0.005;",
     muSd0Info.maxd0,60,-0.15,0.15,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("nmjmaxd0"),";non-muon-jet leading d0;Events/0.005;",
     nonmuSd0Info.maxd0,60,-0.15,0.15,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("mjsubd0"),";muon-jet sub-leading d0;Events/0.005;",
     muSd0Info.subd0,60,-0.15,0.15,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("nmjsubd0"),";non-muon-jet sub-leading d0;Events/0.005;",
     nonmuSd0Info.subd0,60,-0.15,0.15,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("mjthirdd0"),";muon-jet third-leading d0;Events/0.005;",
     muSd0Info.thirdd0,60,-0.15,0.15,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("nmjthirdd0"),";non-muon-jet third-leading d0;Events/0.005;",
     nonmuSd0Info.thirdd0,60,-0.15,0.15,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("mjmeand0_sd0"),";muon-jet mean d0;Events/0.005;",
     muSd0Info.meand0_sd0,60,-0.15,0.15,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("nmjmeand0_sd0"),";non-muon-jet mean d0;Events/0.005;",
     nonmuSd0Info.meand0_sd0,60,-0.15,0.15,event_weight);

    m_HistSvc->FastFillTH1D( namingFunc("mjmaxd0_pt"),";muon-jet leading track d0;Events/0.005;",
     muSd0Info.maxd0_pt,60,-0.15,0.15,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("nmjmaxd0_pt"),";non-muon-jet leading track d0;Events/0.005;",
     nonmuSd0Info.maxd0_pt,60,-0.15,0.15,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("mjsubd0_pt"),";muon-jet sub-leading track d0;Events/0.005;",
     muSd0Info.subd0_pt,60,-0.15,0.15,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("nmjsubd0_pt"),";non-muon-jet sub-leading track d0;Events/0.005;",
     nonmuSd0Info.subd0_pt,60,-0.15,0.15,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("mjthirdd0_pt"),";muon-jet third-leading track d0;Events/0.005;",
     muSd0Info.thirdd0_pt,60,-0.15,0.15,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("nmjthirdd0_pt"),";non-muon-jet third-leading track d0;Events/0.005;",
     nonmuSd0Info.thirdd0_pt,60,-0.15,0.15,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("mjmeand0_pt"),";muon-jet mean d0;Events/0.005;",
     muSd0Info.meand0_pt,60,-0.15,0.15,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("nmjmeand0_pt"),";non-muon-jet mean d0;Events/0.005;",
     nonmuSd0Info.meand0_pt,60,-0.15,0.15,event_weight);

    m_HistSvc->FastFillTH1D( namingFunc("mjmaxd0err"),";muon-jet leading #sigma_{d0};Events/0.002;",
     muSd0Info.maxd0err,50,0.,0.1,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("nmjmaxd0err"),";non-muon-jet leading #sigma_{d0};Events/0.002;",
     nonmuSd0Info.maxd0err,50,0.,0.1,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("mjsubd0err"),";muon-jet sub-leading #sigma_{d0};Events/0.002;",
     muSd0Info.subd0err,50,0.,0.1,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("nmjsubd0err"),";non-muon-jet sub-leading #sigma_{d0};Events/0.002;",
     nonmuSd0Info.subd0err,50,0.,0.1,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("mjthirdd0err"),";muon-jet third-leading #sigma_{d0};Events/0.002;",
     muSd0Info.thirdd0err,50,0.,0.1,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("nmjthirdd0err"),";non-muon-jet third-leading #sigma_{d0};Events/0.002;",
     nonmuSd0Info.thirdd0err,50,0.,0.1,event_weight);

    m_HistSvc->FastFillTH1D( namingFunc("mjmaxd0err_pt"),";muon-jet leading track #sigma_{d0};Events/0.002;",
     muSd0Info.maxd0err_pt,50,0.,0.1,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("nmjmaxd0err_pt"),";non-muon-jet leading track #sigma_{d0};Events/0.002;",
     nonmuSd0Info.maxd0err_pt,50,0.,0.1,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("mjsubd0err_pt"),";muon-jet sub-leading track #sigma_{d0};Events/0.002;",
     muSd0Info.subd0err_pt,50,0.,0.1,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("nmjsubd0err_pt"),";non-muon-jet sub-leading track #sigma_{d0};Events/0.002;",
     nonmuSd0Info.subd0err_pt,50,0.,0.1,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("mjthirdd0err_pt"),";muon-jet third-leading track #sigma_{d0};Events/0.002;",
     muSd0Info.thirdd0err_pt,50,0.,0.1,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("nmjthirdd0err_pt"),";non-muon-jet third-leading track #sigma_{d0};Events/0.002;",
     nonmuSd0Info.thirdd0err_pt,50,0.,0.1,event_weight);
  }
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
  float muojet_maxsd0 = muojet_sd0Info.meanSd0_pt;
  float nonmuojet_maxsd0 = nonmuojet_sd0Info.meanSd0_pt;

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
      m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"meanSd0"+nametag),";mean |s_{d0}|;Events/1.5;",
       muojet_sd0Info.meanSd0_pt,80,-40.,80.,event_weight);
    } else {
      m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"meanSd0"+nametag),";mean |s_{d0}|;Events/1.5;",
       nonmuojet_sd0Info.meanSd0_pt,80,-40.,80.,event_weight);
    }

  }

  // Make plots in fatjet pT bins
  FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight,
   [this,gbbcand,nametag](TString var) { return this->makeFatJetPlotName(gbbcand,var+nametag); }
  );

  // Fill fatjet histograms for SF calculation if not done in FillFatJetProperties
  if (!(m_RunMode & RunMode::FILL_FATJET_PROPERTIES)) {
    m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"fjpt"+nametag),";large-R jet p_{T} [GeV];Events/4 GeV;",
     this->fat_pt->at(gbbcand->fat_index)/1e3,125,500.,1000.,event_weight);

    if(m_isMC) {
      m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"fjptsc"+nametag),";scaled large-R jet p_{T} [GeV];Events/4 GeV;",
       this->getScaledFatPt(this->fat_pt->at(gbbcand->fat_index)/1e3),125,500.,1000.,event_weight);
    } else {
      m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"fjptsc"+nametag),";scaled large-R jet p_{T} [GeV];Events/4 GeV;",
       this->fat_pt->at(gbbcand->fat_index)/1e3,125,500.,1000.,event_weight);
    }
  }

  if (m_isNominal && m_isMC && m_doSd0Systematics) {

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

    if (hasConversion) {
      FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight*1.1, makeSysNamingFunc("Conversion__1up"));
      FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight*0.9, makeSysNamingFunc("Conversion__1down"));
    } else {
      FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight, makeSysNamingFunc("Conversion__1up"));
      FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight, makeSysNamingFunc("Conversion__1down"));
    }

    if (hasHadMatInt) {
      FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight*1.1, makeSysNamingFunc("HadMatInt__1up"));
      FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight*0.9, makeSysNamingFunc("HadMatInt__1down"));
    } else {
      FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight, makeSysNamingFunc("HadMatInt__1up"));
      FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight, makeSysNamingFunc("HadMatInt__1down"));
    }

    if (hasLightLongLived) {
      FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight*1.3, makeSysNamingFunc("LightLongLived__1up"));
      FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight*0.7, makeSysNamingFunc("LightLongLived__1down"));
    } else {
      FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight, makeSysNamingFunc("LightLongLived__1up"));
      FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight, makeSysNamingFunc("LightLongLived__1down"));
    }

    if (hasNoTruthMu && m_FilterType.EqualTo("MuFiltered")) {
      FillSd0Plots(muojet_sd0Info, nonmuojet_sd0Info, event_weight*2, makeSysNamingFunc("FakeMuons__1up"));
    } else {
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
     this->fat_pt->at(gbbcand->fat_index)/1e3,125,500.,1000.,event_weight
    );
    m_HistSvc->FastFillTH1D(
     m_config->GetMCHistName(m_SysVarName,"Incl","Incl","fjeta"+nametag),";large-R jet #eta;Events/0.2;",
     this->fat_eta->at(gbbcand->fat_index),10,-2.5,2.5,event_weight
    );

    m_HistSvc->FastFillTH1D( makeInclDiJetPlotName(gbbcand,"fjpt"+nametag),";large-R jet p_{T} [GeV];Events/4 GeV;",
     this->fat_pt->at(gbbcand->fat_index)/1e3,125,500.,1000.,event_weight);
    //m_HistSvc->FastFillTH1D( makeInclDiJetPlotName(gbbcand,"fjptsc"+nametag),
    // ";scaled large-R jet p_{T} [GeV];Events/4 GeV;",
    // this->getScaledFatPt(this->fat_pt->at(gbbcand->fat_index)/1e3),250,0.,1000.,event_weight);
    m_HistSvc->FastFillTH1D( makeInclDiJetPlotName(gbbcand,"fjeta"+nametag),";large-R jet #eta;Events/0.2;",
     this->fat_eta->at(gbbcand->fat_index),10,-2.5,2.5,event_weight);
  }

  m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"fjpt"+nametag),";large-R jet p_{T} [GeV];Events/4 GeV;",
   this->fat_pt->at(gbbcand->fat_index)/1e3,125,500.,1000.,event_weight);

  if (m_isMC) {
    m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"fjptsc"+nametag),
     ";scaled large-R jet p_{T} [GeV];Events/4 GeV;",
     this->getScaledFatPt(this->fat_pt->at(gbbcand->fat_index)/1e3),125,500.,1000.,event_weight);
  } else {
    m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"fjptsc"+nametag),
     ";scaled large-R jet p_{T} [GeV];Events/4 GeV;",
     this->fat_pt->at(gbbcand->fat_index)/1e3,125,500.,1000.,event_weight);
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

  TLorentzVector mujet,trigjet, muon;

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

  // Topology: Muon Pt and Eta
  muon.SetPtEtaPhiE(this->muo_pt->at(gbbcand->muo_index),this->muo_eta->at(gbbcand->muo_index),this->muo_phi->at(gbbcand->muo_index),this->muo_E->at(gbbcand->muo_index));
  m_HistSvc->FastFillTH1D(
   m_config->GetMCHistName(m_SysVarName,"Incl","Incl","mupt"+nametag),
   ";muon p_{T} [GeV];Events/4 GeV;",
   this->muo_pt->at(gbbcand->muo_index)/1e3,100,0.,400.,event_weight
  );
  m_HistSvc->FastFillTH1D(
   m_config->GetMCHistName(m_SysVarName,"Incl","Incl","mueta"+nametag),
   ";muon #eta;Events/0.2;",
   this->muo_eta->at(gbbcand->muo_index),10,-2.5,2.5,event_weight
  );
  m_HistSvc->FastFillTH1D(
   makeInclDiJetPlotName(gbbcand, "muptrel"+nametag),
   ";muon p_{T}-rel [GeV];Events/0.25 GeV;",
   muon.Pt(mujet.Vect())/1e3,40,0.,10.,event_weight
  );

  //b-taggers
  m_HistSvc->FastFillTH1D(makeMuJetPlotName(gbbcand, "mjMV2c10"+nametag),";muon-jet MV2c10;Events;",
   this->trkjet_MV2c10->at(gbbcand->muojet_index),100,-1.,1.,event_weight);
  m_HistSvc->FastFillTH1D(makeNonMuJetPlotName(gbbcand, "nmjMV2c10"+nametag),";non-muon-jet MV2c10;Events;",
   this->trkjet_MV2c10->at(gbbcand->nonmuojet_index),100,-1.,1.,event_weight);

  if(!m_config->GetIsR20p7()){

    m_HistSvc->FastFillTH1D(makeMuJetPlotName(gbbcand, "mjNtrks"+nametag),";muon-jet N_{tracks};Events;",
     this->trkjet_nTrks->at(gbbcand->muojet_index),100,0.,50.,event_weight);
    m_HistSvc->FastFillTH1D(makeNonMuJetPlotName(gbbcand, "nmjNtrks"+nametag),";non-muon-jet N_{tracks};Events;",
     this->trkjet_nTrks->at(gbbcand->nonmuojet_index),100,0.,50.,event_weight);

    m_HistSvc->FastFillTH2D(
     m_config->GetMCHistName(m_SysVarName,"Incl","Incl","NtrksVsMu"+nametag),
     ";<mu>;track-jet N_{tracks}",
     this->eve_mu,this->trkjet_nTrks->at(gbbcand->muojet_index),
     80,0.,80.,50,0.,50.,event_weight
    );
    m_HistSvc->FastFillTH2D(
     m_config->GetMCHistName(m_SysVarName,"Incl","Incl","NtrksVsMu"+nametag),
     ";trig-jet p_{T} [GeV];large-R jet p_{T} [GeV];",
     this->eve_mu,this->trkjet_nTrks->at(gbbcand->nonmuojet_index),
     250,0.,1000.,50,0.,50.,event_weight
    );

    m_HistSvc->FastFillTH1D(makeDiJetPlotName(gbbcand, "XbbScoreHiggs"+nametag),";XbbScoreHiggs;Events;",
     this->fat_XbbScoreHiggs->at(gbbcand->fat_index),100,-1.,1.,event_weight);
    m_HistSvc->FastFillTH1D(makeDiJetPlotName(gbbcand, "XbbScoreQCD"+nametag),";XbbScoreQCD;Events;",
     this->fat_XbbScoreQCD->at(gbbcand->fat_index),100,-1.,1.,event_weight);
    m_HistSvc->FastFillTH1D(makeDiJetPlotName(gbbcand, "XbbScoreTop"+nametag),";XbbScoreTop;Events;",
     this->fat_XbbScoreTop->at(gbbcand->fat_index),100,-1.,1.,event_weight);
    m_HistSvc->FastFillTH1D(makeDiJetPlotName(gbbcand, "XbbScoreRatiof0"+nametag),
     ";log(XbbScoreHiggs/XbbScoreQCD);Events;",
     std::log( this->fat_XbbScoreHiggs->at(gbbcand->fat_index) /
      this->fat_XbbScoreQCD->at(gbbcand->fat_index) ),
     100,-1.,1.,event_weight
    );
    m_HistSvc->FastFillTH1D(makeDiJetPlotName(gbbcand, "XbbScoreRatiofp2"+nametag),
     ";log(XbbScoreHiggs/(0.8*XbbScoreQCD+0.2*XbbScoreTop));Events;",
     std::log( this->fat_XbbScoreHiggs->at(gbbcand->fat_index) /
      (0.8*this->fat_XbbScoreQCD->at(gbbcand->fat_index) + 0.2*this->fat_XbbScoreTop->at(gbbcand->fat_index)) ),
     100,-1.,1.,event_weight
    );
  }
}

void GbbTupleAna::FillTriggerTurnOnHistograms(int i_trigjet, float event_weight){

  //Fill Histograms for Trigger turn-on curves
  if (eve_HLT_j150) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j150", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j175) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j175", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j200) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j200", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j260) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j260", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j300) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j300", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j320) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j320", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j360) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j360", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j380) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j380", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j400) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j400", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j420) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j420", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j450) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j450", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j300_a10_lcw_L1J75) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j300_a10_lcw_L1J75", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j360_a10_lcw_L1J100) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j360_a10_lcw_L1J100", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j400_a10_lcw_L1J100) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j400_a10_lcw_L1J100", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j420_a10_lcw_L1J100) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j420_a10_lcw_L1J100", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j260_a10_lcw_subjes_L1J75) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j260_a10_lcw_subjes_L1J75", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j420_a10_lcw_subjes_L1J100) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j420_a10_lcw_subjes_L1J100", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j440_a10_lcw_subjes_L1J100) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j440_a10_lcw_subjes_L1J100", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j460_a10_lcw_subjes_L1J100) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j460_a10_lcw_subjes_L1J100", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j260_a10t_lcw_jes_L1J75) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j260_a10t_lcw_jes_L1J75", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j440_a10t_lcw_jes_L1J100) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j440_a10t_lcw_jes_L1J100", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j460_a10t_lcw_jes_L1J100) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j460_a10t_lcw_jes_L1J100", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  m_HistSvc->FastFillTH1D("trigjet_pt_noHLTreq",this->jet_pt->at(i_trigjet)/1e3,100,0.,1000.,event_weight);

}
