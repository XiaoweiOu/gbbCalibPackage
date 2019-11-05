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
  int j1_truth = this->trkjet_truth->at(gbbcand->ind_j1);
  TString muo_flav = this->eve_isMC ? m_config->GetFlavour(j1_truth) : TString("Data");
  TString ptlabel = m_config->GetMuJetLabel(this->trkjet_pt->at(gbbcand->ind_j1)/1e3);
  return m_config->GetMCHistName(m_SysVarName,ptlabel,muo_flav,var);
}

TString GbbTupleAna::makeNonMuJetPlotName(GbbCandidate* gbbcand, const TString var) {
  int j2_truth = this->trkjet_truth->at(gbbcand->ind_j2);
  TString j2_flav = this->eve_isMC ? m_config->GetFlavour(j2_truth) : TString("Data");
  TString ptlabel = m_config->GetNonMuJetLabel(this->trkjet_pt->at(gbbcand->ind_j2)/1e3);
  return m_config->GetMCHistName(m_SysVarName,ptlabel,j2_flav,var);
}

TString GbbTupleAna::makeDiJetPlotName(GbbCandidate* gbbcand, const TString var) {
  int j1_truth = this->trkjet_truth->at(gbbcand->ind_j1);
  int j2_truth = this->trkjet_truth->at(gbbcand->ind_j2);
  TString dijet_flav = this->eve_isMC ? m_config->GetFlavourPair(j1_truth,j2_truth) : TString("Data");
  TString ptlabel = m_config->GetDiTrkJetLabel(this->trkjet_pt->at(gbbcand->ind_j1)/1e3,
                                               this->trkjet_pt->at(gbbcand->ind_j2)/1e3);
  return m_config->GetMCHistName(m_SysVarName,ptlabel,dijet_flav,var);
}

TString GbbTupleAna::makeFatJetPlotName(GbbCandidate* gbbcand, const TString var) {
  int j1_truth = this->trkjet_truth->at(gbbcand->ind_j1);
  int j2_truth = this->trkjet_truth->at(gbbcand->ind_j2);
  TString dijet_flav = this->eve_isMC ? m_config->GetFlavourPair(j1_truth,j2_truth) : TString("Data");
  TString ptlabel = m_config->GetFatJetLabel(this->fat_pt->at(gbbcand->ind_fj)/1e3);
  return m_config->GetMCHistName(m_SysVarName,ptlabel,dijet_flav,var);
}

TString GbbTupleAna::makeInclDiJetPlotName(GbbCandidate* gbbcand, const TString var) {
  int j1_truth = this->trkjet_truth->at(gbbcand->ind_j1);
  int j2_truth = this->trkjet_truth->at(gbbcand->ind_j2);
  TString dijet_flav = this->eve_isMC ? m_config->GetFlavourPair(j1_truth,j2_truth) : TString("Data");
  return m_config->GetMCHistName(m_SysVarName,"Incl",dijet_flav,var);
}

TString GbbTupleAna::makeFlavInclPlotName(const TString var) {
  TString flav = this->eve_isMC ? TString("Incl") : TString("Data");
  return m_config->GetMCHistName(m_SysVarName,"Incl",flav,var);
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

TString GbbTupleAna::getTrkjetName(unsigned int i) {
  if (!m_useLeadingJets) {
    if ( i==0 ) {
      return "mj";
    } else if ( i==1 ) {
      return "nmj";
    } else return Form("j%u",i);
  } else return Form("j%u",i);
}
TString GbbTupleAna::getTrkjetLabel(unsigned int i) {
  if ( i==0 ) {
    return m_useLeadingJets ? "leading jet" : "muon-jet";
  } else if ( i==1 ) {
    return m_useLeadingJets ? "subleading jet" : "non-muon-jet";
  } else if ( i==2 ) {
    return "third jet";
  } else return Form("jet %u",i);
}

void GbbTupleAna::FillTrackJetProperties(GbbCandidate* gbbcand, float event_weight, TString nametag){

  if(m_Debug) std::cout<<"processgbb(): Fill trackjet properties with tag "<<nametag.Data()<<std::endl;
  if (!nametag.IsNull()) nametag.Prepend("_");

  // Plot track-jet kinematics with single-flavour label
  m_HistSvc->FastFillTH1D( makeMuJetPlotName(gbbcand,"j1pt"+nametag),";muon-jet p_{T} [GeV];Events/2 GeV;",
   this->trkjet_pt->at(gbbcand->ind_j1)/1e3,250,0.,500.,event_weight);
  m_HistSvc->FastFillTH1D( makeMuJetPlotName(gbbcand,"j1eta"+nametag),";muon-jet #eta;Events/0.2;",
   this->trkjet_eta->at(gbbcand->ind_j1)/1e3,100,-2.5,2.5,event_weight);

  m_HistSvc->FastFillTH1D( makeNonMuJetPlotName(gbbcand,"j2pt"+nametag),";non-muon-jet p_{T} [GeV];Events/2 GeV;",
   this->trkjet_pt->at(gbbcand->ind_j2)/1e3,250,0.,500.,event_weight);
  m_HistSvc->FastFillTH1D( makeNonMuJetPlotName(gbbcand,"j2eta"+nametag),";non-muon-jet #eta;Events/0.2;",
   this->trkjet_eta->at(gbbcand->ind_j2)/1e3,100,-2.5,2.5,event_weight);

  // Plot track-jet kinematics with di-flavour label
  m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"j1pt"+nametag),";muon-jet p_{T} [GeV];Events/2 GeV;",
   this->trkjet_pt->at(gbbcand->ind_j1)/1e3,250,0.,500.,event_weight);
  m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"j2pt"+nametag),";non-muon-jet p_{T} [GeV];Events/2 GeV;",
   this->trkjet_pt->at(gbbcand->ind_j2)/1e3,250,0.,500.,event_weight);

  m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"j1eta"+nametag),";muon-jet #eta;Events/0.2;",
   this->trkjet_eta->at(gbbcand->ind_j1)/1e3,100,-2.5,2.5,event_weight);
  m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"j2eta"+nametag),";non-muon-jet #eta;Events/0.2;",
   this->trkjet_eta->at(gbbcand->ind_j2)/1e3,100,-2.5,2.5,event_weight);

  // Plot track-jet kinematics with di-flavour label inclusive in pt
  m_HistSvc->FastFillTH1D( makeInclDiJetPlotName(gbbcand,"j1pt"+nametag),";muon-jet p_{T} [GeV];Events/2 GeV;",
   this->trkjet_pt->at(gbbcand->ind_j1)/1e3,250,0.,500.,event_weight);
  m_HistSvc->FastFillTH1D( makeInclDiJetPlotName(gbbcand,"j2pt"+nametag),";non-muon-jet p_{T} [GeV];Events/2 GeV;",
   this->trkjet_pt->at(gbbcand->ind_j2)/1e3,250,0.,500.,event_weight);

  m_HistSvc->FastFillTH1D( makeInclDiJetPlotName(gbbcand,"j1eta"+nametag),";muon-jet #eta;Events/0.2;",
   this->trkjet_eta->at(gbbcand->ind_j1)/1e3,100,-2.5,2.5,event_weight);
  m_HistSvc->FastFillTH1D( makeInclDiJetPlotName(gbbcand,"j2eta"+nametag),";non-muon-jet #eta;Events/0.2;",
   this->trkjet_eta->at(gbbcand->ind_j2)/1e3,100,-2.5,2.5,event_weight);

  m_HistSvc->FastFillTH2D( makeInclDiJetPlotName(gbbcand,"j1ptVsj2pt"+nametag),
    ";muon-jet p_{T} [GeV];non-muon jet p_{T} [GeV];",
    this->trkjet_pt->at(gbbcand->ind_j1)/1e3,this->trkjet_pt->at(gbbcand->ind_j2)/1e3,
    250,0.,500.,250,0.,500.,event_weight
  );

  // Plot the pt fraction of the three leading tracks in j1 and j2
  std::vector<float> j1_trk_pt = this->trkjet_assocTrk_pt->at(gbbcand->ind_j1);
  std::sort(j1_trk_pt.begin(),j1_trk_pt.end(),std::greater<float>());

  std::vector<float> j2_trk_pt = this->trkjet_assocTrk_pt->at(gbbcand->ind_j2);
  std::sort(j2_trk_pt.begin(),j2_trk_pt.end(),std::greater<float>());

  float j1frac = ( j1_trk_pt.at(0) + j1_trk_pt.at(1) + j1_trk_pt.at(2) ) / this->trkjet_pt->at(gbbcand->ind_j1);
  float j2frac = ( j2_trk_pt.at(0) + j2_trk_pt.at(1) + j2_trk_pt.at(2) ) / this->trkjet_pt->at(gbbcand->ind_j2);

  m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"j1ptfrac"+nametag),";muon-jet p_{T} fraction (3 lead trks);Events;",
   j1frac,100,0.,1.,event_weight);
  m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"j2ptfrac"+nametag),";non-muon-jet p_{T} fraction (3 lead trks);Events;",
   j2frac,100,0.,1.,event_weight);

  m_HistSvc->FastFillTH1D( makeInclDiJetPlotName(gbbcand,"j1ptfrac"+nametag),";muon-jet p_{T} fraction (3 lead trks);Events;",
   j1frac,100,0.,1.,event_weight);
  m_HistSvc->FastFillTH1D( makeInclDiJetPlotName(gbbcand,"j2ptfrac"+nametag),";non-muon-jet p_{T} fraction (3 lead trks);Events;",
   j2frac,100,0.,1.,event_weight);

  // Plot pt fraction vs. trk multiplicity for j1 and j2
  m_HistSvc->FastFillTH2D( makeDiJetPlotName(gbbcand,"j1ptfracVsj1trkmult"+nametag),
    ";muon-jet p_{T} fraction (3 lead trks);muon-jet track multiplicity;",
    j1frac,this->trkjet_assocTrk_pt->at(gbbcand->ind_j1).size(),
    20,0.,1.,25,0.,50.,event_weight );

  m_HistSvc->FastFillTH2D( makeDiJetPlotName(gbbcand,"j2ptfracVsj2trkmult"+nametag),
    ";non-muon-jet p_{T} fraction (3 lead trks);non-muon-jet track multiplicity;",
    j2frac,this->trkjet_assocTrk_pt->at(gbbcand->ind_j2).size(),
    20,0.,1.,25,0.,50.,event_weight );

  m_HistSvc->FastFillTH2D( makeInclDiJetPlotName(gbbcand,"j1ptfracVsj1trkmult"+nametag),
    ";muon-jet p_{T} fraction (3 lead trks);muon-jet track multiplicity;",
    j1frac,this->trkjet_assocTrk_pt->at(gbbcand->ind_j1).size(),
    20,0.,1.,25,0.,50.,event_weight );

  m_HistSvc->FastFillTH2D( makeInclDiJetPlotName(gbbcand,"j2ptfracVsj2trkmult"+nametag),
    ";non-muon-jet p_{T} fraction (3 lead trks);non-muon-jet track multiplicity;",
    j2frac,this->trkjet_assocTrk_pt->at(gbbcand->ind_j2).size(),
    20,0.,1.,25,0.,50.,event_weight );

  // Plot the amount of times j1 and j2 are the two leading jets in the candidate
  m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"hasLeadTrkJets"+nametag),";hasLeadTrkJets;Events;",
   gbbcand->hasLeadTrkJets,2,0.,2.,event_weight);

  m_HistSvc->FastFillTH1D( makeInclDiJetPlotName(gbbcand,"hasLeadTrkJets"+nametag),";hasLeadTrkJets;Events;",
   gbbcand->hasLeadTrkJets,2,0.,2.,event_weight);

  m_HistSvc->FastFillTH1D( makeFatJetPlotName(gbbcand,"j1pt"+nametag),";muon-jet p_{T} [GeV];Events/2 GeV;",
   this->trkjet_pt->at(gbbcand->ind_j1)/1e3,250,0.,500.,event_weight);
  m_HistSvc->FastFillTH1D( makeFatJetPlotName(gbbcand,"j2pt"+nametag),";non-muon-jet p_{T} [GeV];Events/2 GeV;",
   this->trkjet_pt->at(gbbcand->ind_j2)/1e3,250,0.,500.,event_weight);
}

void GbbTupleAna::FillSd0Plots(std::vector<trkjetSd0Info> sd0InfoVec, float event_weight, std::function<TString (TString)> namingFunc) {
  // Using the giving plot naming function to make plots of all the requested s_d0 variables

  for (unsigned int i=0; i < sd0InfoVec.size(); i++) {
    auto sd0Info = sd0InfoVec[i];
    // Name goes in the plot name, label goes on the plot axis
    TString name = getTrkjetName(i);
    TString label = getTrkjetLabel(i);

    //m_HistSvc->FastFillTH1D( namingFunc(name+"maxSd0"),";"+label+" leading s_{d0};Events/1.5;",
    // sd0Info.maxSd0,80,-40.,80.,event_weight);
    //m_HistSvc->FastFillTH1D( namingFunc(name+"subSd0"),";"+label+" sub-leading s_{d0};Events/1.5;",
    // sd0Info.subSd0,80,-40.,80.,event_weight);
    //m_HistSvc->FastFillTH1D( namingFunc(name+"thirdSd0"),";"+label+" third-leading s_{d0};Events/1.5;",
    // sd0Info.thirdSd0,80,-40.,80.,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc(name+"meanSd0"),";"+label+" mean s_{d0};Events/1.5;",
     sd0Info.meanSd0_pt,80,-40.,80.,event_weight);

    // Using the giving plot naming function to make plots of extra d0 and sd0 variables
    if(m_RunMode & RunMode::FILL_ADV_PROPERTIES && m_isNominal) {
      m_HistSvc->FastFillTH1D( namingFunc(name+"maxSd0DR"),
       ";"+label+" #Delta R(jet,track_{max s_{d0}});Events/0.02;",
       sd0Info.maxSd0_dR,25,0.,0.5,event_weight);

      m_HistSvc->FastFillTH1D( namingFunc(name+"maxSd0_pt"),";"+label+" leading track s_{d0};Events/1.5;",
       sd0Info.maxSd0_pt,80,-40.,80.,event_weight);
      m_HistSvc->FastFillTH1D( namingFunc(name+"subSd0_pt"),";"+label+" sub-leading track s_{d0};Events/1.5;",
       sd0Info.subSd0_pt,80,-40.,80.,event_weight);
      m_HistSvc->FastFillTH1D( namingFunc(name+"thirdSd0_pt"),";"+label+" third-leading track s_{d0};Events/1.5;",
       sd0Info.thirdSd0_pt,80,-40.,80.,event_weight);
      //m_HistSvc->FastFillTH1D( namingFunc(name+"meanSd0_sd0"),";"+label+" mean s_{d0};Events/1.5;",
      // sd0Info.meanSd0_sd0,80,-40.,80.,event_weight);

      //m_HistSvc->FastFillTH1D( namingFunc(name+"maxd0"),";"+label+" leading d0;Events/0.005;",
      // sd0Info.maxd0,60,-0.15,0.15,event_weight);
      //m_HistSvc->FastFillTH1D( namingFunc(name+"subd0"),";"+label+" sub-leading d0;Events/0.005;",
      // sd0Info.subd0,60,-0.15,0.15,event_weight);
      //m_HistSvc->FastFillTH1D( namingFunc(name+"thirdd0"),";"+label+" third-leading d0;Events/0.005;",
      // sd0Info.thirdd0,60,-0.15,0.15,event_weight);
      //m_HistSvc->FastFillTH1D( namingFunc(name+"meand0_sd0"),";"+label+" mean d0;Events/0.005;",
      // sd0Info.meand0_sd0,60,-0.15,0.15,event_weight);

      m_HistSvc->FastFillTH1D( namingFunc(name+"maxd0_pt"),";"+label+" leading track d0;Events/0.005;",
       sd0Info.maxd0_pt,60,-0.15,0.15,event_weight);
      m_HistSvc->FastFillTH1D( namingFunc(name+"subd0_pt"),";"+label+" sub-leading track d0;Events/0.005;",
       sd0Info.subd0_pt,60,-0.15,0.15,event_weight);
      m_HistSvc->FastFillTH1D( namingFunc(name+"thirdd0_pt"),";"+label+" third-leading track d0;Events/0.005;",
       sd0Info.thirdd0_pt,60,-0.15,0.15,event_weight);
      m_HistSvc->FastFillTH1D( namingFunc(name+"meand0_pt"),";"+label+" mean d0;Events/0.005;",
       sd0Info.meand0_pt,60,-0.15,0.15,event_weight);

      m_HistSvc->FastFillTH1D( namingFunc(name+"max_z0sintheta_pt"),";"+label+" leading track z0sin#theta;Events/0.01;",
       sd0Info.max_z0sintheta_pt,80,-0.4,0.4,event_weight);
      m_HistSvc->FastFillTH1D( namingFunc(name+"sub_z0sintheta_pt"),";"+label+" sub-leading track z0sin#theta;Events/0.01;",
       sd0Info.sub_z0sintheta_pt,80,-0.4,0.4,event_weight);
      m_HistSvc->FastFillTH1D( namingFunc(name+"third_z0sintheta_pt"),";"+label+" third-leading track z0sin#theta;Events/0.01;",
       sd0Info.third_z0sintheta_pt,80,-0.4,0.4,event_weight);

      //m_HistSvc->FastFillTH1D( namingFunc(name+"maxd0err"),";"+label+" leading #sigma_{d0};Events/0.002;",
      // sd0Info.maxd0err,50,0.,0.1,event_weight);
      //m_HistSvc->FastFillTH1D( namingFunc(name+"subd0err"),";"+label+" sub-leading #sigma_{d0};Events/0.002;",
      // sd0Info.subd0err,50,0.,0.1,event_weight);
      //m_HistSvc->FastFillTH1D( namingFunc(name+"thirdd0err"),";"+label+" third-leading #sigma_{d0};Events/0.002;",
      // sd0Info.thirdd0err,50,0.,0.1,event_weight);

      m_HistSvc->FastFillTH1D( namingFunc(name+"maxd0err_pt"),";"+label+" leading track #sigma_{d0};Events/0.002;",
       sd0Info.maxd0err_pt,50,0.,0.1,event_weight);
      m_HistSvc->FastFillTH1D( namingFunc(name+"subd0err_pt"),";"+label+" sub-leading track #sigma_{d0};Events/0.002;",
       sd0Info.subd0err_pt,50,0.,0.1,event_weight);
      m_HistSvc->FastFillTH1D( namingFunc(name+"thirdd0err_pt"),";"+label+" third-leading track #sigma_{d0};Events/0.002;",
       sd0Info.thirdd0err_pt,50,0.,0.1,event_weight);
      }
    }
}

//Tong
void GbbTupleAna::FillSd02DPlots(trkjetSd0Info j1Sd0Info, trkjetSd0Info j2Sd0Info, float event_weight, std::function<TString (TString)> namingFunc) {
  m_HistSvc->FastFillTH2D( namingFunc("j1maxSd0_j2maxSd0"),";muon-jet leading track s_{d0};non-muon-jet leading track s_{d0};",
     j1Sd0Info.maxSd0,j2Sd0Info.maxSd0, 80,-40,80, 80,-40,80,event_weight);

  m_HistSvc->FastFillTH2D( namingFunc("j1meanSd0_j2meanSd0"),";muon-jet mean s_{d0};non-muon-jet mean s_{d0};",
     j1Sd0Info.meanSd0_pt,j2Sd0Info.meanSd0_pt, 80,-40,80, 80,-40,80,event_weight);

  m_HistSvc->FastFillTH2D( namingFunc("j1subSd0_j2subSd0"),";muon-jet sub leading track s_{d0};non-muon-jet sub leading track s_{d0};",
     j1Sd0Info.subSd0,j2Sd0Info.subSd0, 80,-40,80, 80,-40,80,event_weight);

  m_HistSvc->FastFillTH2D( namingFunc("j1thirdSd0_j2thirdSd0"),";muon-jet third leading track s_{d0};non-muon-jet third leading track s_{d0};",
     j1Sd0Info.thirdSd0,j2Sd0Info.thirdSd0, 80,-40,80, 80,-40,80,event_weight);

  m_HistSvc->FastFillTH2D( namingFunc("j1maxSd0_j1subSd0"),";muon-jet leading track s_{d0};muon-jet sub leading track s_{d0};",
     j1Sd0Info.maxSd0,j1Sd0Info.subSd0, 80,-40,80, 80,-40,80,event_weight);

  m_HistSvc->FastFillTH2D( namingFunc("j1maxSd0_j1thirdSd0"),";muon-jet leading track s_{d0};muon-jet third leading track s_{d0};",
     j1Sd0Info.maxSd0,j1Sd0Info.thirdSd0, 80,-40,80, 80,-40,80,event_weight);

  m_HistSvc->FastFillTH2D( namingFunc("j1subSd0_j1thirdSd0"),";muon-jet sub leading track s_{d0};muon-jet third leading track s_{d0};",
     j1Sd0Info.subSd0,j1Sd0Info.thirdSd0, 80,-40,80, 80,-40,80,event_weight);

  m_HistSvc->FastFillTH2D( namingFunc("j2maxSd0_j2subSd0"),";non-muon-jet leading track s_{d0};non-muon-jet sub leading track s_{d0};",
     j2Sd0Info.maxSd0,j2Sd0Info.subSd0, 80,-40,80, 80,-40,80,event_weight);

  m_HistSvc->FastFillTH2D( namingFunc("j2maxSd0_j2thirdSd0"),";non-muon-jet leading track s_{d0};non-muon-jet third leading track s_{d0};",
     j2Sd0Info.maxSd0,j2Sd0Info.thirdSd0, 80,-40,80, 80,-40,80,event_weight);

  m_HistSvc->FastFillTH2D( namingFunc("j2subSd0_j2thirdSd0"),";non-muon-jet sub leading track s_{d0};non-muon-jet third leading track s_{d0};",
     j2Sd0Info.subSd0,j2Sd0Info.thirdSd0, 80,-40,80, 80,-40,80,event_weight);
}

void GbbTupleAna::FillTemplates(GbbCandidate* gbbcand, float event_weight,TString nametag){

  if(m_Debug) std::cout<<"processgbb(): Fill templates with tag "<<nametag.Data()<<std::endl;
  if (!nametag.IsNull()) nametag.Prepend("_");

  int j1_truth=this->trkjet_truth->at(gbbcand->ind_j1);
  int j2_truth=this->trkjet_truth->at(gbbcand->ind_j2);
  TString dijet_flav = this->eve_isMC ? m_config->GetFlavourPair(j1_truth,j2_truth) : TString("Data");

  TString ptlabel = m_config->GetDiTrkJetLabel(this->trkjet_pt->at(gbbcand->ind_j1)/1e3,
                                               this->trkjet_pt->at(gbbcand->ind_j2)/1e3);

  // Calculate sd0 variables
  trkjetSd0Info j1_sd0Info=this->getTrkjetAssocSd0Info(gbbcand->ind_j1,m_doTrackSmearing,"nominal","nominal",3);
  trkjetSd0Info j2_sd0Info=this->getTrkjetAssocSd0Info(gbbcand->ind_j2,m_doTrackSmearing,"nominal","nominal",3);
  float j1_maxsd0 = j1_sd0Info.meanSd0_pt;
  float j2_maxsd0 = j2_sd0Info.meanSd0_pt;

  if(TMath::Abs(j1_maxsd0+99)<1e-5 || TMath::Abs(j2_maxsd0+99)<1e-5) return;

  // Make inclusive plots of s_d0 variables
  // Custom lambda function defines names of plots
  FillSd0Plots({j1_sd0Info, j2_sd0Info}, event_weight,
   [this,gbbcand,nametag](TString var) { return this->makeInclDiJetPlotName(gbbcand,var+nametag); }
  );

  // Make plots in pT bins
  FillSd0Plots({j1_sd0Info, j2_sd0Info}, event_weight,
   [this,gbbcand,nametag](TString var) { return this->makeDiJetPlotName(gbbcand,var+nametag); }
  );

  ////Tong: Fill TH2D on 2 template variables for checking correlation (inclusive)
  //FillSd02DPlots(j1_sd0Info, j2_sd0Info, event_weight,
  // [this,gbbcand,nametag](TString var) {return this->makeInclDiJetPlotName(gbbcand,var+nametag);}
  //);

  //// Fill TH2D in pT bins
  //FillSd02DPlots(j1_sd0Info, j2_sd0Info, event_weight,
  // [this,gbbcand,nametag](TString var) {return this->makeDiJetPlotName(gbbcand,var+nametag);}
  //);

  // Make plots with randomized mu/non-mu jet
  if (m_doRandomSplitting) {

    if(m_doFillMujet) {
      m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"meanSd0"+nametag),";mean |s_{d0}|;Events/1.5;",
       j1_sd0Info.meanSd0_pt,80,-40.,80.,event_weight);
    } else {
      m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"meanSd0"+nametag),";mean |s_{d0}|;Events/1.5;",
       j2_sd0Info.meanSd0_pt,80,-40.,80.,event_weight);
    }

  }

  // Make plots in fatjet pT bins
  FillSd0Plots({j1_sd0Info, j2_sd0Info}, event_weight,
   [this,gbbcand,nametag](TString var) { return this->makeFatJetPlotName(gbbcand,var+nametag); }
  );

  // Fill some extra histograms used by scale-factor code if they aren't
  // requested elsewhere. Ensures that only FILL_TEMPLATES is necessary
  if (!(m_RunMode & RunMode::FILL_FATJET_PROPERTIES)) {
    m_HistSvc->FastFillTH1D( makeFatJetPlotName(gbbcand,"fjpt"+nametag),";large-R jet p_{T} [GeV];Events/5 GeV;",
     this->fat_pt->at(gbbcand->ind_fj)/1e3,100,500.,1000.,event_weight);
    m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"fjpt"+nametag),";large-R jet p_{T} [GeV];Events/5 GeV;",
     this->fat_pt->at(gbbcand->ind_fj)/1e3,100,500.,1000.,event_weight);

    if(m_isMC) {
      m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"fjptsc"+nametag),";scaled large-R jet p_{T} [GeV];Events/5 GeV;",
       this->getScaledFatPt(this->fat_pt->at(gbbcand->ind_fj)/1e3),100,500.,1000.,event_weight);
    } else {
      m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"fjptsc"+nametag),";scaled large-R jet p_{T} [GeV];Events/5 GeV;",
       this->fat_pt->at(gbbcand->ind_fj)/1e3,100,500.,1000.,event_weight);
    }
  }
  if (!(m_RunMode & RunMode::FILL_TRKJET_PROPERTIES)) {
    m_HistSvc->FastFillTH1D( makeInclDiJetPlotName(gbbcand,"j1pt"+nametag),";muon-jet p_{T} [GeV];Events/2 GeV;",
     this->trkjet_pt->at(gbbcand->ind_j1)/1e3,250,0.,500.,event_weight);
    m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"j1pt"+nametag),";muon-jet p_{T} [GeV];Events/2 GeV;",
     this->trkjet_pt->at(gbbcand->ind_j1)/1e3,250,0.,500.,event_weight);
  }

  if (m_isNominal && m_isMC && m_doSd0Systematics) {

    auto makeSysNamingFunc = [config=this->m_config,dijet_flav,ptlabel,nametag](TString sys)
     -> std::function<TString (TString)> {
      return [config,sys,dijet_flav,ptlabel,nametag](TString var) {
        return config->GetMCHistName(sys,ptlabel,dijet_flav,var+nametag);
      };
    };

    // Calculate sd0 variables with up variation
    trkjetSd0Info  j1_sd0Info_d0up=this->getTrkjetAssocSd0Info(gbbcand->ind_j1,m_doTrackSmearing,"up","nominal",3);
    trkjetSd0Info  j2_sd0Info_d0up=this->getTrkjetAssocSd0Info(gbbcand->ind_j2,m_doTrackSmearing,"up","nominal",3);
    FillSd0Plots({j1_sd0Info_d0up, j2_sd0Info_d0up}, event_weight, makeSysNamingFunc("D0Smear__1up"));

    trkjetSd0Info  j1_sd0Info_z0up=this->getTrkjetAssocSd0Info(gbbcand->ind_j1,m_doTrackSmearing,"nominal","up",3);
    trkjetSd0Info  j2_sd0Info_z0up=this->getTrkjetAssocSd0Info(gbbcand->ind_j2,m_doTrackSmearing,"nominal","up",3);
    FillSd0Plots({j1_sd0Info_z0up, j2_sd0Info_z0up}, event_weight, makeSysNamingFunc("Z0Smear__1up"));

    // Calculate sd0 variables with down variation
    trkjetSd0Info  j1_sd0Info_d0down=this->getTrkjetAssocSd0Info(gbbcand->ind_j1,m_doTrackSmearing,"down","nominal",3);
    trkjetSd0Info  j2_sd0Info_d0down=this->getTrkjetAssocSd0Info(gbbcand->ind_j2,m_doTrackSmearing,"down","nominal",3);
    FillSd0Plots({j1_sd0Info_d0down, j2_sd0Info_d0down}, event_weight, makeSysNamingFunc("D0Smear__1down"));

    trkjetSd0Info  j1_sd0Info_z0down=this->getTrkjetAssocSd0Info(gbbcand->ind_j1,m_doTrackSmearing,"nominal","down",3);
    trkjetSd0Info  j2_sd0Info_z0down=this->getTrkjetAssocSd0Info(gbbcand->ind_j2,m_doTrackSmearing,"nominal","down",3);
    FillSd0Plots({j1_sd0Info_z0down, j2_sd0Info_z0down}, event_weight, makeSysNamingFunc("Z0Smear__1down"));

    bool hasConversion=false, hasHadMatInt=false, hasLightLongLived=false, hasNoTruthMu=false;
    this->getSystematicsFlags(gbbcand, hasConversion, hasHadMatInt, hasLightLongLived, hasNoTruthMu);

    if (hasConversion) {
      FillSd0Plots({j1_sd0Info, j2_sd0Info}, event_weight*1.1, makeSysNamingFunc("Conversion__1up"));
      FillSd0Plots({j1_sd0Info, j2_sd0Info}, event_weight*0.9, makeSysNamingFunc("Conversion__1down"));
    } else {
      FillSd0Plots({j1_sd0Info, j2_sd0Info}, event_weight, makeSysNamingFunc("Conversion__1up"));
      FillSd0Plots({j1_sd0Info, j2_sd0Info}, event_weight, makeSysNamingFunc("Conversion__1down"));
    }

    if (hasHadMatInt) {
      FillSd0Plots({j1_sd0Info, j2_sd0Info}, event_weight*1.1, makeSysNamingFunc("HadMatInt__1up"));
      FillSd0Plots({j1_sd0Info, j2_sd0Info}, event_weight*0.9, makeSysNamingFunc("HadMatInt__1down"));
    } else {
      FillSd0Plots({j1_sd0Info, j2_sd0Info}, event_weight, makeSysNamingFunc("HadMatInt__1up"));
      FillSd0Plots({j1_sd0Info, j2_sd0Info}, event_weight, makeSysNamingFunc("HadMatInt__1down"));
    }

    if (hasLightLongLived) {
      FillSd0Plots({j1_sd0Info, j2_sd0Info}, event_weight*1.3, makeSysNamingFunc("LightLongLived__1up"));
      FillSd0Plots({j1_sd0Info, j2_sd0Info}, event_weight*0.7, makeSysNamingFunc("LightLongLived__1down"));
    } else {
      FillSd0Plots({j1_sd0Info, j2_sd0Info}, event_weight, makeSysNamingFunc("LightLongLived__1up"));
      FillSd0Plots({j1_sd0Info, j2_sd0Info}, event_weight, makeSysNamingFunc("LightLongLived__1down"));
    }

    if (hasNoTruthMu && m_FilterType.EqualTo("MuFiltered")) {
      FillSd0Plots({j1_sd0Info, j2_sd0Info}, event_weight*2, makeSysNamingFunc("FakeMuons__1up"));
    } else {
      FillSd0Plots({j1_sd0Info, j2_sd0Info}, event_weight, makeSysNamingFunc("FakeMuons__1up"));
    }
  } // End of extra systematics
}

void GbbTupleAna::FillMCStatsInfo(GbbCandidate* gbbcand, TString nametag){

  if (!nametag.IsNull()) nametag.Prepend("_");

  m_HistSvc->FastFillTH2D( makeInclDiJetPlotName(gbbcand,"j1pt_vs_j2pt_unweighted"+nametag),
   ";muon-jet p_{T} [GeV];non-muon-jet p_{T} [GeV];",
   this->trkjet_pt->at(gbbcand->ind_j1)/1e3,this->trkjet_pt->at(gbbcand->ind_j2)/1e3,
   50,0.,200.,50,0.,200.,1.
  );

}

void GbbTupleAna::FillFatJetProperties(GbbCandidate* gbbcand, float event_weight, TString nametag){

  if(m_Debug) std::cout<<"processgbb(): Fill fatjet properties with tag "<<nametag.Data()<<std::endl;
  if (!nametag.IsNull()) nametag.Prepend("_");

  int j1_truth=this->trkjet_truth->at(gbbcand->ind_j1);
  int j2_truth=this->trkjet_truth->at(gbbcand->ind_j2);
  TString dijet_flav = this->eve_isMC ? m_config->GetFlavourPair(j1_truth,j2_truth) : TString("Data");

  TString ptlabel = m_config->GetDiTrkJetLabel(this->trkjet_pt->at(gbbcand->ind_j1)/1e3,
                                               this->trkjet_pt->at(gbbcand->ind_j2)/1e3);

  //inclusive only for pt and eta
  if(m_isNominal) {
    m_HistSvc->FastFillTH1D(
     makeFlavInclPlotName("fjpt"+nametag),";large-R jet p_{T} [GeV];Events/5 GeV;",
     this->fat_pt->at(gbbcand->ind_fj)/1e3,100,500.,1000.,event_weight
    );
    m_HistSvc->FastFillTH1D(
     makeFlavInclPlotName("fjeta"+nametag),";large-R jet #eta;Events/0.2;",
     this->fat_eta->at(gbbcand->ind_fj),10,-2.5,2.5,event_weight
    );

    m_HistSvc->FastFillTH1D( makeInclDiJetPlotName(gbbcand,"fjpt"+nametag),";large-R jet p_{T} [GeV];Events/5 GeV;",
     this->fat_pt->at(gbbcand->ind_fj)/1e3,100,500.,1000.,event_weight);
    //m_HistSvc->FastFillTH1D( makeInclDiJetPlotName(gbbcand,"fjptsc"+nametag),
    // ";scaled large-R jet p_{T} [GeV];Events/4 GeV;",
    // this->getScaledFatPt(this->fat_pt->at(gbbcand->ind_fj)/1e3),250,0.,1000.,event_weight);
    m_HistSvc->FastFillTH1D( makeInclDiJetPlotName(gbbcand,"fjeta"+nametag),";large-R jet #eta;Events/0.2;",
     this->fat_eta->at(gbbcand->ind_fj),10,-2.5,2.5,event_weight);

  }

  m_HistSvc->FastFillTH1D( makeFatJetPlotName(gbbcand,"fjpt"+nametag),";large-R jet p_{T} [GeV];Events/5 GeV;",
   this->fat_pt->at(gbbcand->ind_fj)/1e3,100,500.,1000.,event_weight);
  m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"fjpt"+nametag),";large-R jet p_{T} [GeV];Events/5 GeV;",
   this->fat_pt->at(gbbcand->ind_fj)/1e3,100,500.,1000.,event_weight);

  if (m_isMC) {
    m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"fjptsc"+nametag),
     ";scaled large-R jet p_{T} [GeV];Events/5 GeV;",
     this->getScaledFatPt(this->fat_pt->at(gbbcand->ind_fj)/1e3),100,500.,1000.,event_weight);
  } else {
    m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"fjptsc"+nametag),
     ";scaled large-R jet p_{T} [GeV];Events/5 GeV;",
     this->fat_pt->at(gbbcand->ind_fj)/1e3,100,500.,1000.,event_weight);
  }

  m_HistSvc->FastFillTH1D( makeFatJetPlotName(gbbcand,"fjD2"+nametag),";large-R jet D2;Events/0.02;",
   this->fat_D2->at(gbbcand->ind_fj),250,0.,5.,event_weight);
  m_HistSvc->FastFillTH1D( makeFatJetPlotName(gbbcand,"fjtau21"+nametag),";large-R jet #tau_{21};Events/0.005;",
   this->fat_tau21->at(gbbcand->ind_fj),260,0.,1.3,event_weight);
  m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"fjD2"+nametag),";large-R jet D2;Events/0.02;",
   this->fat_D2->at(gbbcand->ind_fj),250,0.,5.,event_weight);
  m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"fjtau21"+nametag),";large-R jet #tau_{21};Events/0.005;",
   this->fat_tau21->at(gbbcand->ind_fj),260,0.,1.3,event_weight);

  TLorentzVector fatjet;
  fatjet.SetPtEtaPhiE(this->fat_pt->at(gbbcand->ind_fj),this->fat_eta->at(gbbcand->ind_fj),this->fat_phi->at(gbbcand->ind_fj),this->fat_E->at(gbbcand->ind_fj));

  m_HistSvc->FastFillTH1D( makeFatJetPlotName(gbbcand,"fjm"+nametag),";large-R jet mass [GeV];Events/20 GeV;",
   fatjet.M()/1e3,25,0.,500.,event_weight);
  m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"fjm"+nametag),";large-R jet mass [GeV];Events/20 GeV;",
   fatjet.M()/1e3,25,0.,500.,event_weight);

}

void GbbTupleAna::FillAdvancedProperties(GbbCandidate* gbbcand, int i_trig_jet, float event_weight,TString nametag){

  if(m_Debug) std::cout<<"processgbb(): Fill advanced properties with tag "<<nametag.Data()<<std::endl;
  TLorentzVector mujet,j2,ditrkjet,fatjet,trigjet,muon;

  if (!nametag.IsNull()) nametag.Prepend("_");

  //Topology: Plot DR(fatjet,muonjet)
  mujet.SetPtEtaPhiE(this->trkjet_pt->at(gbbcand->ind_j1),this->trkjet_eta->at(gbbcand->ind_j1),this->trkjet_phi->at(gbbcand->ind_j1),this->trkjet_E->at(gbbcand->ind_j1));
  j2.SetPtEtaPhiE(this->trkjet_pt->at(gbbcand->ind_j2),this->trkjet_eta->at(gbbcand->ind_j2),this->trkjet_phi->at(gbbcand->ind_j2),this->trkjet_E->at(gbbcand->ind_j2));

  ditrkjet=mujet+j2;
  fatjet.SetPtEtaPhiE(this->fat_pt->at(gbbcand->ind_fj),this->fat_eta->at(gbbcand->ind_fj),this->fat_phi->at(gbbcand->ind_fj),this->fat_E->at(gbbcand->ind_fj));
  float DRditrkjetfatjet = fatjet.DeltaR(ditrkjet);

  trigjet.SetPtEtaPhiE(this->jet_pt->at(i_trig_jet),this->jet_eta->at(i_trig_jet),this->jet_phi->at(i_trig_jet),this->jet_E->at(i_trig_jet));


  if(m_isNominal) {

    m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"DRditrkjetfatjet"+nametag),
     ";#Delta R(fatjet, inv. sum of track-jets);Events/0.002",
     DRditrkjetfatjet,250,0.,0.5,event_weight
    );

    for (unsigned int i_trk=0; i_trk<trkjet_assocTrk_pt->at(gbbcand->ind_j1).size(); i_trk++) {
      m_HistSvc->FastFillTH2D(
       m_config->GetMCHistName("Nom","Incl","Incl","trjptVstrkd0"+nametag),
       ";track |d_{0}|;track-jet p_{T} [GeV];",
       fabs(getd0(i_trk,gbbcand->ind_j1,m_doTrackSmearing,"nominal")),
       trkjet_assocTrk_pt->at(gbbcand->ind_j1).at(i_trk)/1e3,
       14,1,15,100,0.,500.,event_weight
      );
      m_HistSvc->FastFillTH2D(
       m_config->GetMCHistName("Nom","Incl","Incl","trjptVstrkz0sintheta"+nametag),
       ";track |z_{0}sin#theta|;track-jet p_{T} [GeV];",
       fabs((getz0(i_trk,gbbcand->ind_j1,m_doTrackSmearing,"nominal") - this->eve_PVz
          +this->trkjet_assocTrk_vz->at(gbbcand->ind_j1).at(i_trk)) *
          TMath::Sin(this->trkjet_assocTrk_theta->at(gbbcand->ind_j1).at(i_trk))),
       trkjet_pt->at(gbbcand->ind_j1)/1e3,
       14,1,15,100,0.,500.,event_weight
      );
      m_HistSvc->FastFillTH2D(
       m_config->GetMCHistName("Nom","Incl","Incl","trkd0errVStrkd0"+nametag),
       ";track #sigma_{d0};track |d_{0}|;",
       this->trkjet_assocTrk_d0err->at(gbbcand->ind_j1).at(i_trk),
       fabs(getd0(i_trk,gbbcand->ind_j1,m_doTrackSmearing,"nominal")),
       100,0.,0.1,30,-0.15,0.15,event_weight
      );
      m_HistSvc->FastFillTH2D(
       m_config->GetMCHistName("Nom","Incl","Incl","trkd0errVStrkSd0"+nametag),
       ";track #sigma_{d0};track S_{d0};",
       this->trkjet_assocTrk_d0err->at(gbbcand->ind_j1).at(i_trk),
       getSd0(i_trk,gbbcand->ind_j1,m_doTrackSmearing,"nominal"),
       100,0.,0.1,80,-40,80,event_weight
      );
    }
    for (unsigned int i_trk=0; i_trk<trkjet_assocTrk_pt->at(gbbcand->ind_j2).size(); i_trk++) {
      m_HistSvc->FastFillTH2D(
       m_config->GetMCHistName("Nom","Incl","Incl","trjptVstrkd0"+nametag),
       ";track |d_{0}|;track-jet p_{T} [GeV];",
       fabs(getd0(i_trk,gbbcand->ind_j2,m_doTrackSmearing,"nominal")),
       trkjet_pt->at(gbbcand->ind_j2)/1e3,
       14,1,15,100,0.,500.,event_weight
      );
      m_HistSvc->FastFillTH2D(
       m_config->GetMCHistName("Nom","Incl","Incl","trjptVstrkz0sintheta"+nametag),
       ";track |z_{0}sin#theta|;track-jet p_{T} [GeV];",
       fabs((getz0(i_trk,gbbcand->ind_j2,m_doTrackSmearing,"nominal") - this->eve_PVz
          +this->trkjet_assocTrk_vz->at(gbbcand->ind_j2).at(i_trk)) *
          TMath::Sin(this->trkjet_assocTrk_theta->at(gbbcand->ind_j2).at(i_trk))),
       trkjet_pt->at(gbbcand->ind_j2)/1e3,
       14,1,15,100,0.,500.,event_weight
      );
      m_HistSvc->FastFillTH2D(
       m_config->GetMCHistName("Nom","Incl","Incl","trkd0errVStrkd0"+nametag),
       ";track #sigma_{d0};track |d_{0}|;",
       this->trkjet_assocTrk_d0err->at(gbbcand->ind_j2).at(i_trk),
       fabs(getd0(i_trk,gbbcand->ind_j2,m_doTrackSmearing,"nominal")),
       100,0.,0.1,30,-0.15,0.15,event_weight
      );
      m_HistSvc->FastFillTH2D(
       m_config->GetMCHistName("Nom","Incl","Incl","trkd0errVStrkSd0"+nametag),
       ";track #sigma_{d0};track S_{d0};",
       this->trkjet_assocTrk_d0err->at(gbbcand->ind_j2).at(i_trk),
       getSd0(i_trk,gbbcand->ind_j2,m_doTrackSmearing,"nominal"),
       100,0.,0.1,80,-40,80,event_weight
      );
    }
  }

  m_HistSvc->FastFillTH1D(
   makeFlavInclPlotName("trjj1DR"+nametag),
   ";#Delta R(trigger-matched jet, muon-jet);Events/0.1;",
   mujet.DeltaR(trigjet),50,0.,5.,event_weight
  );

  //Topology: Fill Trigger Jet Pt and Eta
  m_HistSvc->FastFillTH1D(
   makeFlavInclPlotName("trjpt"+nametag),
   ";trigger-matched jet p_{T} [GeV];Events/4 GeV;",
   this->jet_pt->at(i_trig_jet)/1e3,250,0.,1000.,event_weight
  );
  m_HistSvc->FastFillTH1D(
   makeFlavInclPlotName("trjeta"+nametag),
   ";trigger-matched #eta;Events/0.2;",
   this->jet_eta->at(i_trig_jet),10,-2.5,2.5,event_weight
  );

  m_HistSvc->FastFillTH1D(
   makeFlavInclPlotName("trjfjptratio"+nametag),
   ";trig-jet p_{T} / large-R jet p_{T};Events/2;",
   this->jet_pt->at(i_trig_jet)/this->fat_pt->at(gbbcand->ind_fj),50,0.,10.,event_weight
  );

  m_HistSvc->FastFillTH2D(
   makeFlavInclPlotName("trjptVsfjpt"+nametag),
   ";trig-jet p_{T} [GeV];large-R jet p_{T} [GeV];",
   this->jet_pt->at(i_trig_jet)/1e3,this->fat_pt->at(gbbcand->ind_fj)/1e3,
   250,0.,1000.,250,0.,1000.,event_weight
  );

  // Topology: Muon Pt and Eta
  if (gbbcand->ind_j1_mu != 999) {
    muon.SetPtEtaPhiE(this->muo_pt->at(gbbcand->ind_j1_mu),this->muo_eta->at(gbbcand->ind_j1_mu),this->muo_phi->at(gbbcand->ind_j1_mu),this->muo_E->at(gbbcand->ind_j1_mu));
    m_HistSvc->FastFillTH1D(
     makeFlavInclPlotName("mupt"+nametag),
     ";muon p_{T} [GeV];Events/4 GeV;",
     this->muo_pt->at(gbbcand->ind_j1_mu)/1e3,100,0.,400.,event_weight
    );
    m_HistSvc->FastFillTH1D(
     makeFlavInclPlotName("muphi"+nametag),
     ";muon #phi;Events/0.4;",
     this->muo_phi->at(gbbcand->ind_j1_mu),16,-3.2,3.2,event_weight
    );
    m_HistSvc->FastFillTH1D(
     makeFlavInclPlotName("mueta"+nametag),
     ";muon #eta;Events/0.5;",
     this->muo_eta->at(gbbcand->ind_j1_mu),10,-2.5,2.5,event_weight
    );
    m_HistSvc->FastFillTH1D(
     makeFlavInclPlotName("muE"+nametag),
     ";muon E [GeV];Events/4 GeV;",
     this->muo_E->at(gbbcand->ind_j1_mu)/1e3,100,0.,400.,event_weight
    );
    m_HistSvc->FastFillTH1D(
     makeFlavInclPlotName("mud0"+nametag),
     ";muon d0;Events/0.001;",
     this->muo_d0->at(gbbcand->ind_j1_mu),30,-0.15,0.15,event_weight
    );
    m_HistSvc->FastFillTH1D(
     makeFlavInclPlotName("muz0"+nametag),
     ";muon z0;Events/0.001;",
     this->muo_z0->at(gbbcand->ind_j1_mu),90,-0.4,0.4,event_weight
    );
    m_HistSvc->FastFillTH1D(
     makeFlavInclPlotName("muz0sintheta"+nametag),
     ";muon z0sintheta;Events/0.001;",
     this->muo_z0sintheta->at(gbbcand->ind_j1_mu),90,-0.4,0.4,event_weight
    );
    m_HistSvc->FastFillTH1D(
     makeInclDiJetPlotName(gbbcand, "muptrel"+nametag),
     ";muon p_{T}-rel [GeV];Events/0.25 GeV;",
     muon.Pt(mujet.Vect())/1e3,40,0.,10.,event_weight
    );
  }

  //b-taggers
  m_HistSvc->FastFillTH1D(makeMuJetPlotName(gbbcand, "j1MV2c10"+nametag),";muon-jet MV2c10;Events;",
   this->trkjet_MV2c10->at(gbbcand->ind_j1),100,-1.,1.,event_weight);
  m_HistSvc->FastFillTH1D(makeNonMuJetPlotName(gbbcand, "j2MV2c10"+nametag),";non-muon-jet MV2c10;Events;",
   this->trkjet_MV2c10->at(gbbcand->ind_j2),100,-1.,1.,event_weight);

  if(!m_config->GetIsR20p7()){

    m_HistSvc->FastFillTH1D(makeMuJetPlotName(gbbcand, "j1Ntrks"+nametag),";muon-jet N_{tracks};Events;",
     this->trkjet_nTrks->at(gbbcand->ind_j1),100,0.,50.,event_weight);
    m_HistSvc->FastFillTH1D(makeNonMuJetPlotName(gbbcand, "j2Ntrks"+nametag),";non-muon-jet N_{tracks};Events;",
     this->trkjet_nTrks->at(gbbcand->ind_j2),100,0.,50.,event_weight);

    m_HistSvc->FastFillTH2D(
     makeFlavInclPlotName("NtrksVsMu"+nametag),
     ";<mu>;track-jet N_{tracks}",
     this->eve_mu,this->trkjet_nTrks->at(gbbcand->ind_j1),
     80,0.,80.,50,0.,50.,event_weight
    );
    m_HistSvc->FastFillTH2D(
     makeFlavInclPlotName("NtrksVsMu"+nametag),
     ";trig-jet p_{T} [GeV];large-R jet p_{T} [GeV];",
     this->eve_mu,this->trkjet_nTrks->at(gbbcand->ind_j2),
     250,0.,1000.,50,0.,50.,event_weight
    );

    m_HistSvc->FastFillTH1D(makeDiJetPlotName(gbbcand, "XbbScoreHiggs"+nametag),";XbbScoreHiggs;Events;",
     this->fat_XbbScoreHiggs->at(gbbcand->ind_fj),100,-1.,1.,event_weight);
    m_HistSvc->FastFillTH1D(makeDiJetPlotName(gbbcand, "XbbScoreQCD"+nametag),";XbbScoreQCD;Events;",
     this->fat_XbbScoreQCD->at(gbbcand->ind_fj),100,-1.,1.,event_weight);
    m_HistSvc->FastFillTH1D(makeDiJetPlotName(gbbcand, "XbbScoreTop"+nametag),";XbbScoreTop;Events;",
     this->fat_XbbScoreTop->at(gbbcand->ind_fj),100,-1.,1.,event_weight);
    m_HistSvc->FastFillTH1D(makeDiJetPlotName(gbbcand, "XbbScoreRatiof0"+nametag),
     ";log(XbbScoreHiggs/XbbScoreQCD);Events;",
     std::log( this->fat_XbbScoreHiggs->at(gbbcand->ind_fj) /
      this->fat_XbbScoreQCD->at(gbbcand->ind_fj) ),
     100,-1.,1.,event_weight
    );
    m_HistSvc->FastFillTH1D(makeDiJetPlotName(gbbcand, "XbbScoreRatiofp2"+nametag),
     ";log(XbbScoreHiggs/(0.8*XbbScoreQCD+0.2*XbbScoreTop));Events;",
     std::log( this->fat_XbbScoreHiggs->at(gbbcand->ind_fj) /
      (0.8*this->fat_XbbScoreQCD->at(gbbcand->ind_fj) + 0.2*this->fat_XbbScoreTop->at(gbbcand->ind_fj)) ),
     100,-1.,1.,event_weight
    );
  }
}

void GbbTupleAna::FillTriggerTurnOnHistograms(int i_trigjet, float event_weight){

  if(m_Debug) std::cout<<"processgbb(): Fill trigger turn-on"<<std::endl;
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
