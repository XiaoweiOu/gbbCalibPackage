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

//
// Helper functions used to standardize plot naming schemes
//

// Get full truth flavor label
TString GbbTupleAna::getTriJetFlav(GbbCandidate* gbbcand) {
  int j1_truth = this->trkjet_truth->at(gbbcand->ind_tj.at(0));
  int j2_truth(99), j3_truth(99);
  if (gbbcand->ind_tj.size() > 1) j2_truth = this->trkjet_truth->at(gbbcand->ind_tj.at(1));
  if (gbbcand->ind_tj.size() > 2) j3_truth = this->trkjet_truth->at(gbbcand->ind_tj.at(2));
  return m_config->GetFatJetFlavour(j1_truth,j2_truth,j3_truth);
}

// Label with truth flavor and dijet pt bin
TString GbbTupleAna::makeDiJetPlotName(GbbCandidate* gbbcand, const TString var) {
  TString trijet_flav = this->eve_isMC ? getTriJetFlav(gbbcand) : TString("Data");
  TString ptlabel = m_config->GetDiTrkJetLabel(this->trkjet_pt->at(gbbcand->ind_tj.at(0))/1000.,
                                               this->trkjet_pt->at(gbbcand->ind_tj.at(1))/1000.);
  return m_config->GetMCHistName(m_SysVarName,ptlabel,trijet_flav,var);
}

// Label with truth flavor and fatjet pt bin
TString GbbTupleAna::makeFatJetPlotName(GbbCandidate* gbbcand, const TString var) {
  TString trijet_flav = this->eve_isMC ? getTriJetFlav(gbbcand) : TString("Data");
  TString ptlabel = m_config->GetFatJetLabel(this->fat_pt->at(gbbcand->ind_fj)/1000.);
  return m_config->GetMCHistName(m_SysVarName,ptlabel,trijet_flav,var);
}

// Label with truth flavor and no pt bin
TString GbbTupleAna::makePtInclPlotName(GbbCandidate* gbbcand, const TString var) {
  TString trijet_flav = this->eve_isMC ? getTriJetFlav(gbbcand) : TString("Data");
  return m_config->GetMCHistName(m_SysVarName,"Incl",trijet_flav,var);
}

// Label with truth flavor of the specified track-jet and no pt bin
TString GbbTupleAna::makePtInclTrkJetPlotName(GbbCandidate* gbbcand, unsigned int i, const TString var) {
  if (i >= gbbcand->ind_tj.size()) throw std::out_of_range("Requested track-jet does not exist");
  unsigned int ind_tj = gbbcand->ind_tj.at(i);
  int truth = trkjet_truth->at(ind_tj);
  TString flav = this->eve_isMC ? m_config->GetFlavour(truth) : TString("Data");
  return m_config->GetMCHistName(m_SysVarName,"Incl",flav,var);
}

// Label with no truth flavor and no pt bin
TString GbbTupleAna::makeInclPlotName(const TString var) {
  TString flav = this->eve_isMC ? TString("Incl") : TString("Data");
  return m_config->GetMCHistName(m_SysVarName,"Incl",flav,var);
}

//
// Begin plotting functions
// These each contain a set of related plots
// The functions called depend on the flags given in the config file
//

// Plots for reweighting the trigger jet pt and eta (not used)
void GbbTupleAna::FillReweightInfo(int i_trig_jet, float event_weight, TString nametag){

  m_HistSvc->FastFillTH2D("reweight_trigjet_pt_eta_"+nametag,
   ";trigger-matched jet p_{T} [GeV];trigger-matched jet #eta",
   this->jet_pt->at(i_trig_jet)/1000.,this->jet_eta->at(i_trig_jet),75,0,2500.,10,-2.5,2.5,event_weight);

}

// Plots for reweighting the fatjet pt and eta (not used)
void GbbTupleAna::FillFatReweightInfo(float trigfat_pt,float trigfat_eta, float event_weight, TString nametag){

  m_HistSvc->FastFillTH2D("reweight_trigfatjet_pt_eta_"+nametag,
   ";large-R jet p_{T} [GeV];large-R jet #eta",
   trigfat_pt/1000.,trigfat_eta,75,0,2500.,10,-2.5,2.5,event_weight);

}

// Plots of kinematic properties of the track-jets
void GbbTupleAna::FillTrackJetProperties(GbbCandidate* gbbcand, float event_weight, TString nametag){

  if(m_Debug) std::cout<<"processgbb(): Fill trackjet properties with tag "<<nametag.Data()<<std::endl;
  if (!nametag.IsNull()) nametag.Prepend("_");


  auto FillTrackJetPlots = [this, gbbcand, event_weight](unsigned int i, std::function<TString (TString)> namingFunc) {

    unsigned int ind_tj = gbbcand->ind_tj.at(i);
    // Name goes in the plot name, label goes on the plot axis
    TString name = m_config->GetTrkJetName(i);
    TString label = m_config->GetTrkJetLabel(i);

    TLorentzVector trkjet;
    trkjet.SetPtEtaPhiE(this->trkjet_pt->at(ind_tj)/1000.,
                        this->trkjet_eta->at(ind_tj),
                        this->trkjet_phi->at(ind_tj),
                        this->trkjet_E->at(ind_tj)/1000.);

    m_HistSvc->FastFillTH1D( namingFunc(name+"pt"),
     ";"+label+" p_{T} [GeV];Events/10 GeV;",
     trkjet.Pt(),50,0.,500.,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc(name+"m"),
     ";"+label+" mass [GeV];Events/10 GeV;",
     trkjet.M(),25,0.,250.,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc(name+"eta"),
     ";"+label+" #eta;Events/0.2;",
     trkjet.Eta(),30,-3,3,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc(name+"phi"),
     ";"+label+" #phi;Events/0.2;",
     trkjet.Phi(),32,-3.2,3.2,event_weight);
  };

  for (unsigned int i=0; i < gbbcand->ind_tj.size(); i++) {
    unsigned int ind_tj = gbbcand->ind_tj.at(i);

    // Name goes in the plot name, label goes on the plot axis
    TString name = m_config->GetTrkJetName(i);
    TString label = m_config->GetTrkJetLabel(i);

    //if (gbbcand->ind_tj.size() > 1) {
    //  FillTrackJetPlots(i,
    //    [this,gbbcand,nametag](TString var) { return this->makeDiJetPlotName(gbbcand,var+nametag); }
    //    );
    //}

    FillTrackJetPlots(i,
      [this,gbbcand,nametag](TString var) { return this->makeFatJetPlotName(gbbcand,var+nametag); }
      );

    if (m_isMC) {
      // Plots labelled with only this trkjets flavor
      FillTrackJetPlots(i,
        [this,gbbcand,nametag,i](TString var) { return this->makePtInclTrkJetPlotName(gbbcand,i,var+nametag); }
        );

      FillTrackJetPlots(i,
        [this,gbbcand,nametag](TString var) { return this->makePtInclPlotName(gbbcand,var+nametag); }
        );
    }

    FillTrackJetPlots(i,
      [this,gbbcand,nametag](TString var) { return this->makeInclPlotName(var+nametag); }
      );

    // Correlation plots (2D pt and deltaR)
    for (unsigned int j=i+1; j < gbbcand->ind_tj.size(); j++) {
      unsigned int ind_tj2 = gbbcand->ind_tj.at(j);

      // Name goes in the plot name, label goes on the plot axis
      TString name2 = m_config->GetTrkJetName(j);
      TString label2 = m_config->GetTrkJetLabel(j);

      // Make pt correlation plot
      m_HistSvc->FastFillTH2D( makeInclPlotName(name+"ptVs"+name2+"pt"+nametag),
        ";"+label+" p_{T} [GeV];"+label2+" jet p_{T} [GeV];",
        this->trkjet_pt->at(ind_tj)/1000.,this->trkjet_pt->at(ind_tj2)/1000.,
        250,0.,500.,250,0.,500.,event_weight
      );
      TLorentzVector tj_vec, tj2_vec;
      tj_vec.SetPtEtaPhiE( this->trkjet_pt->at(ind_tj)/1000.,
                           this->trkjet_eta->at(ind_tj),
                           this->trkjet_phi->at(ind_tj),
                           this->trkjet_E->at(ind_tj)/1000.);
      tj2_vec.SetPtEtaPhiE( this->trkjet_pt->at(ind_tj2)/1000.,
                            this->trkjet_eta->at(ind_tj2),
                            this->trkjet_phi->at(ind_tj2),
                            this->trkjet_E->at(ind_tj2)/1000.);
      m_HistSvc->FastFillTH1D(
       makeFatJetPlotName(gbbcand,"dR_"+name+"_"+name2+nametag),
       ";#Delta R(muon-jet,non-muon jet);Events/0.01;",
       tj_vec.DeltaR(tj2_vec),100,0.,1.0,event_weight
      );
    }
  }

  // Plot the amount of times j1 and j2 are the two leading jets in the candidate
  m_HistSvc->FastFillTH1D( makeFatJetPlotName(gbbcand,"hasLeadTrkJets"+nametag),";hasLeadTrkJets;Events;",
   gbbcand->hasLeadTrkJets,2,0.,2.,event_weight);

}

void GbbTupleAna::FillSd0Plots(std::vector<trkjetSd0Info> sd0InfoVec, float event_weight, std::function<TString (TString)> namingFunc) {
  // Using the giving plot naming function to make plots of all the requested s_d0 variables

  for (unsigned int i=0; i < sd0InfoVec.size(); i++) {
    auto sd0Info = sd0InfoVec[i];
    if (sd0Info.meanSd0_pt == -99.) continue;

    // Name goes in the plot name, label goes on the plot axis
    TString name = m_config->GetTrkJetName(i);
    TString label = m_config->GetTrkJetLabel(i);

    //m_HistSvc->FastFillTH1D( namingFunc(name+"maxSd0"),";"+label+" leading s_{d0};Events/2;",
    // sd0Info.maxSd0,60,-40.,80.,event_weight);
    //m_HistSvc->FastFillTH1D( namingFunc(name+"subSd0"),";"+label+" sub-leading s_{d0};Events/2;",
    // sd0Info.subSd0,60,-40.,80.,event_weight);
    //m_HistSvc->FastFillTH1D( namingFunc(name+"thirdSd0"),";"+label+" third-leading s_{d0};Events/2;",
    // sd0Info.thirdSd0,60,-40.,80.,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc(name+"meanSd0"),";"+label+" mean s_{d0};Events/2;",
     sd0Info.meanSd0_pt,60,-40.,80.,event_weight);

    // Using the giving plot naming function to make plots of extra d0 and sd0 variables
    if(m_RunMode & RunMode::FILL_ADV_PROPERTIES && m_isNominal && !m_minimalTree) {
      m_HistSvc->FastFillTH1D( namingFunc(name+"maxSd0DR"),
       ";"+label+" #Delta R(jet,track_{max s_{d0}});Events/0.02;",
       sd0Info.maxSd0_dR,25,0.,0.5,event_weight);

      m_HistSvc->FastFillTH1D( namingFunc(name+"maxSd0_pt"),";"+label+" leading track s_{d0};Events/2;",
       sd0Info.maxSd0_pt,60,-40.,80.,event_weight);
      m_HistSvc->FastFillTH1D( namingFunc(name+"subSd0_pt"),";"+label+" sub-leading track s_{d0};Events/2;",
       sd0Info.subSd0_pt,60,-40.,80.,event_weight);
      m_HistSvc->FastFillTH1D( namingFunc(name+"thirdSd0_pt"),";"+label+" third-leading track s_{d0};Events/2;",
       sd0Info.thirdSd0_pt,60,-40.,80.,event_weight);
      //m_HistSvc->FastFillTH1D( namingFunc(name+"meanSd0_sd0"),";"+label+" mean s_{d0};Events/2;",
      // sd0Info.meanSd0_sd0,60,-40.,80.,event_weight);

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
      //m_HistSvc->FastFillTH1D( namingFunc(name+"subd0_pt"),";"+label+" sub-leading track d0;Events/0.005;",
      // sd0Info.subd0_pt,60,-0.15,0.15,event_weight);
      //m_HistSvc->FastFillTH1D( namingFunc(name+"thirdd0_pt"),";"+label+" third-leading track d0;Events/0.005;",
      // sd0Info.thirdd0_pt,60,-0.15,0.15,event_weight);
      //m_HistSvc->FastFillTH1D( namingFunc(name+"meand0_pt"),";"+label+" mean d0;Events/0.005;",
      // sd0Info.meand0_pt,60,-0.15,0.15,event_weight);

      m_HistSvc->FastFillTH1D( namingFunc(name+"max_z0sintheta_pt"),";"+label+" leading track z0sin#theta;Events/0.01;",
       sd0Info.max_z0sintheta_pt,80,-0.4,0.4,event_weight);
      //m_HistSvc->FastFillTH1D( namingFunc(name+"sub_z0sintheta_pt"),";"+label+" sub-leading track z0sin#theta;Events/0.01;",
      // sd0Info.sub_z0sintheta_pt,80,-0.4,0.4,event_weight);
      //m_HistSvc->FastFillTH1D( namingFunc(name+"third_z0sintheta_pt"),";"+label+" third-leading track z0sin#theta;Events/0.01;",
      // sd0Info.third_z0sintheta_pt,80,-0.4,0.4,event_weight);

      //m_HistSvc->FastFillTH1D( namingFunc(name+"maxd0err"),";"+label+" leading #sigma_{d0};Events/0.002;",
      // sd0Info.maxd0err,50,0.,0.1,event_weight);
      //m_HistSvc->FastFillTH1D( namingFunc(name+"subd0err"),";"+label+" sub-leading #sigma_{d0};Events/0.002;",
      // sd0Info.subd0err,50,0.,0.1,event_weight);
      //m_HistSvc->FastFillTH1D( namingFunc(name+"thirdd0err"),";"+label+" third-leading #sigma_{d0};Events/0.002;",
      // sd0Info.thirdd0err,50,0.,0.1,event_weight);

      m_HistSvc->FastFillTH1D( namingFunc(name+"maxd0err_pt"),";"+label+" leading track #sigma_{d0};Events/0.002;",
       sd0Info.maxd0err_pt,50,0.,0.1,event_weight);
      //m_HistSvc->FastFillTH1D( namingFunc(name+"subd0err_pt"),";"+label+" sub-leading track #sigma_{d0};Events/0.002;",
      // sd0Info.subd0err_pt,50,0.,0.1,event_weight);
      //m_HistSvc->FastFillTH1D( namingFunc(name+"thirdd0err_pt"),";"+label+" third-leading track #sigma_{d0};Events/0.002;",
      // sd0Info.thirdd0err_pt,50,0.,0.1,event_weight);
      }
    }
}

/*
//Tong
void GbbTupleAna::FillSd02DPlots(trkjetSd0Info j1Sd0Info, trkjetSd0Info j2Sd0Info, float event_weight, std::function<TString (TString)> namingFunc) {
  m_HistSvc->FastFillTH2D( namingFunc("j1maxSd0_j2maxSd0"),";muon-jet leading track s_{d0};non-muon-jet leading track s_{d0};",
     j1Sd0Info.maxSd0,j2Sd0Info.maxSd0, 60,-40.,80., 60,-40.,80.,event_weight);

  m_HistSvc->FastFillTH2D( namingFunc("j1meanSd0_j2meanSd0"),";muon-jet mean s_{d0};non-muon-jet mean s_{d0};",
     j1Sd0Info.meanSd0_pt,j2Sd0Info.meanSd0_pt, 60,-40.,80., 60,-40.,80.,event_weight);

  m_HistSvc->FastFillTH2D( namingFunc("j1subSd0_j2subSd0"),";muon-jet sub leading track s_{d0};non-muon-jet sub leading track s_{d0};",
     j1Sd0Info.subSd0,j2Sd0Info.subSd0, 60,-40.,80., 60,-40.,80.,event_weight);

  m_HistSvc->FastFillTH2D( namingFunc("j1thirdSd0_j2thirdSd0"),";muon-jet third leading track s_{d0};non-muon-jet third leading track s_{d0};",
     j1Sd0Info.thirdSd0,j2Sd0Info.thirdSd0, 60,-40.,80., 60,-40.,80.,event_weight);

  m_HistSvc->FastFillTH2D( namingFunc("j1maxSd0_j1subSd0"),";muon-jet leading track s_{d0};muon-jet sub leading track s_{d0};",
     j1Sd0Info.maxSd0,j1Sd0Info.subSd0, 60,-40.,80., 60,-40.,80.,event_weight);

  m_HistSvc->FastFillTH2D( namingFunc("j1maxSd0_j1thirdSd0"),";muon-jet leading track s_{d0};muon-jet third leading track s_{d0};",
     j1Sd0Info.maxSd0,j1Sd0Info.thirdSd0, 60,-40.,80., 60,-40.,80.,event_weight);

  m_HistSvc->FastFillTH2D( namingFunc("j1subSd0_j1thirdSd0"),";muon-jet sub leading track s_{d0};muon-jet third leading track s_{d0};",
     j1Sd0Info.subSd0,j1Sd0Info.thirdSd0, 60,-40.,80., 60,-40.,80.,event_weight);

  m_HistSvc->FastFillTH2D( namingFunc("j2maxSd0_j2subSd0"),";non-muon-jet leading track s_{d0};non-muon-jet sub leading track s_{d0};",
     j2Sd0Info.maxSd0,j2Sd0Info.subSd0, 60,-40.,80., 60,-40.,80.,event_weight);

  m_HistSvc->FastFillTH2D( namingFunc("j2maxSd0_j2thirdSd0"),";non-muon-jet leading track s_{d0};non-muon-jet third leading track s_{d0};",
     j2Sd0Info.maxSd0,j2Sd0Info.thirdSd0, 60,-40.,80., 60,-40.,80.,event_weight);

  m_HistSvc->FastFillTH2D( namingFunc("j2subSd0_j2thirdSd0"),";non-muon-jet sub leading track s_{d0};non-muon-jet third leading track s_{d0};",
     j2Sd0Info.subSd0,j2Sd0Info.thirdSd0, 60,-40.,80., 60,-40.,80.,event_weight);
}
*/

void GbbTupleAna::FillTemplates(GbbCandidate* gbbcand, float event_weight,TString nametag){

  if(m_Debug) std::cout<<"processgbb(): Fill templates with tag "<<nametag.Data()<<std::endl;
  if (!nametag.IsNull()) nametag.Prepend("_");

  // Calculate sd0 variables
  unsigned int ntrk = 3;
  std::vector<trkjetSd0Info> sd0InfoVec;
  for (unsigned int i=0; i < gbbcand->ind_tj.size(); i++) {
    sd0InfoVec.push_back(getTrkjetAssocSd0Info(gbbcand->ind_tj.at(i), m_doTrackSmearing,
                                               "nominal", "nominal", ntrk));
  }

  //// Require at least 1 track-jet
  if(sd0InfoVec.size() == 0) return;

  TString trijet_flav = this->eve_isMC ? getTriJetFlav(gbbcand) : TString("Data");

  // Make inclusive plots of s_d0 variables
  // Custom lambda function defines names of plots
  FillSd0Plots(sd0InfoVec, event_weight,
   [this,gbbcand,nametag](TString var) { return this->makePtInclPlotName(gbbcand,var+nametag); }
  );

  // Make plots in pT bins
  if (gbbcand->ind_tj.size() > 1) {
    FillSd0Plots(sd0InfoVec, event_weight,
     [this,gbbcand,nametag](TString var) { return this->makeDiJetPlotName(gbbcand,var+nametag); }
    );
  }

  ////Tong: Fill TH2D on 2 template variables for checking correlation (inclusive)
  //FillSd02DPlots(j1_sd0Info, j2_sd0Info, event_weight,
  // [this,gbbcand,nametag](TString var) {return this->makePtInclPlotName(gbbcand,var+nametag);}
  //);

  //// Fill TH2D in pT bins
  //FillSd02DPlots(j1_sd0Info, j2_sd0Info, event_weight,
  // [this,gbbcand,nametag](TString var) {return this->makeDiJetPlotName(gbbcand,var+nametag);}
  //);

  // Make plots in fatjet pT bins
  FillSd0Plots(sd0InfoVec, event_weight,
   [this,gbbcand,nametag](TString var) { return this->makeFatJetPlotName(gbbcand,var+nametag); }
  );

  if (m_isNominal && m_isMC && m_doSd0Systematics) {

    TString ptlabel = m_config->GetFatJetLabel(this->fat_pt->at(gbbcand->ind_fj)/1000.);
    auto makeSysNamingFunc = [config=this->m_config,trijet_flav,ptlabel,nametag](TString sys)
     -> std::function<TString (TString)> {
      return [config,sys,trijet_flav,ptlabel,nametag](TString var) {
        return config->GetMCHistName(sys,ptlabel,trijet_flav,var+nametag);
      };
    };

    // Calculate sd0 variables with up variation
    std::vector<trkjetSd0Info> sd0InfoVec_d0up, sd0InfoVec_z0up;
    for (unsigned int i=0; i < gbbcand->ind_tj.size(); i++) {
      sd0InfoVec_d0up.push_back(getTrkjetAssocSd0Info(gbbcand->ind_tj.at(i), m_doTrackSmearing,
                                                 "up", "nominal", ntrk));
    }
    FillSd0Plots(sd0InfoVec_d0up, event_weight, makeSysNamingFunc("D0Smear__1up"));

    for (unsigned int i=0; i < gbbcand->ind_tj.size(); i++) {
      sd0InfoVec_z0up.push_back(getTrkjetAssocSd0Info(gbbcand->ind_tj.at(i), m_doTrackSmearing,
                                                 "nominal", "up", ntrk));
    }
    FillSd0Plots(sd0InfoVec_z0up, event_weight, makeSysNamingFunc("Z0Smear__1up"));

    // Calculate sd0 variables with down variation
    std::vector<trkjetSd0Info> sd0InfoVec_d0down, sd0InfoVec_z0down;
    for (unsigned int i=0; i < gbbcand->ind_tj.size(); i++) {
      sd0InfoVec_d0down.push_back(getTrkjetAssocSd0Info(gbbcand->ind_tj.at(i), m_doTrackSmearing,
                                                 "down", "nominal", ntrk));
    }
    FillSd0Plots(sd0InfoVec_d0down, event_weight, makeSysNamingFunc("D0Smear__1down"));

    for (unsigned int i=0; i < gbbcand->ind_tj.size(); i++) {
      sd0InfoVec_z0down.push_back(getTrkjetAssocSd0Info(gbbcand->ind_tj.at(i), m_doTrackSmearing,
                                                 "nominal", "down", ntrk));
    }
    FillSd0Plots(sd0InfoVec_z0down, event_weight, makeSysNamingFunc("Z0Smear__1down"));

    bool hasConversion=false, hasHadMatInt=false, hasLightLongLived=false, hasNoTruthMu=false;
    this->getSystematicsFlags(gbbcand, hasConversion, hasHadMatInt, hasLightLongLived, hasNoTruthMu);

    if (hasConversion) {
      FillSd0Plots(sd0InfoVec, event_weight*1.1, makeSysNamingFunc("Conversion__1up"));
      FillSd0Plots(sd0InfoVec, event_weight*0.9, makeSysNamingFunc("Conversion__1down"));
    } else {
      FillSd0Plots(sd0InfoVec, event_weight, makeSysNamingFunc("Conversion__1up"));
      FillSd0Plots(sd0InfoVec, event_weight, makeSysNamingFunc("Conversion__1down"));
    }

    if (hasHadMatInt) {
      FillSd0Plots(sd0InfoVec, event_weight*1.1, makeSysNamingFunc("HadMatInt__1up"));
      FillSd0Plots(sd0InfoVec, event_weight*0.9, makeSysNamingFunc("HadMatInt__1down"));
    } else {
      FillSd0Plots(sd0InfoVec, event_weight, makeSysNamingFunc("HadMatInt__1up"));
      FillSd0Plots(sd0InfoVec, event_weight, makeSysNamingFunc("HadMatInt__1down"));
    }

    if (hasLightLongLived) {
      FillSd0Plots(sd0InfoVec, event_weight*1.3, makeSysNamingFunc("LightLongLived__1up"));
      FillSd0Plots(sd0InfoVec, event_weight*0.7, makeSysNamingFunc("LightLongLived__1down"));
    } else {
      FillSd0Plots(sd0InfoVec, event_weight, makeSysNamingFunc("LightLongLived__1up"));
      FillSd0Plots(sd0InfoVec, event_weight, makeSysNamingFunc("LightLongLived__1down"));
    }

    if (hasNoTruthMu && m_FilterType.EqualTo("MuFiltered")) {
      FillSd0Plots(sd0InfoVec, event_weight*2, makeSysNamingFunc("FakeMuons__1up"));
    } else {
      FillSd0Plots(sd0InfoVec, event_weight, makeSysNamingFunc("FakeMuons__1up"));
    }
  } // End of extra systematics
}

// Plots of kinematic properties of the fatjet
void GbbTupleAna::FillFatJetProperties(GbbCandidate* gbbcand, float event_weight, TString nametag){

  if(m_Debug) std::cout<<"processgbb(): Fill fatjet properties with tag "<<nametag.Data()<<std::endl;
  if (!nametag.IsNull()) nametag.Prepend("_");

  auto FillFatJetPlots = [this, gbbcand, event_weight](std::function<TString (TString)> namingFunc) {

    TLorentzVector fatjet;
    fatjet.SetPtEtaPhiE(this->fat_pt->at(gbbcand->ind_fj)/1000.,
                        this->fat_eta->at(gbbcand->ind_fj),
                        this->fat_phi->at(gbbcand->ind_fj),
                        this->fat_E->at(gbbcand->ind_fj)/1000.);

    m_HistSvc->FastFillTH1D( namingFunc("fjpt"),";large-R jet p_{T} [GeV];Events/10 GeV;",
     fatjet.Pt(),100,250.,1250.,event_weight);
    //if (this->m_isMC) {
    //  m_HistSvc->FastFillTH1D( namingFunc("fjptsc"),
    //   ";scaled large-R jet p_{T} [GeV];Events/10 GeV;",
    //   this->getScaledFatPt(fatjet.Pt()),100,250.,1250.,event_weight);
    //}
    m_HistSvc->FastFillTH1D( namingFunc("fjm"),";large-R jet mass [GeV];Events/10 GeV;",
     fatjet.M(),50,0.,500.,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("fjeta"),";large-R jet #eta;Events/0.2;",
     fatjet.Eta(),30,-3,3,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("fjphi"),";large-R jet #phi;Events/0.2;",
     fatjet.Phi(),32,-3.2,3.2,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("fjD2"),";large-R jet D2;Events/0.2;",
     this->fat_D2->at(gbbcand->ind_fj),50,0.,10.,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("fjtau21"),";large-R jet #tau_{21};Events/0.05;",
     this->fat_tau21->at(gbbcand->ind_fj),20,0.,1.,event_weight);
  };

  if (gbbcand->ind_tj.size() > 1) {
    FillFatJetPlots([this,gbbcand,nametag](TString var) { return this->makeDiJetPlotName(gbbcand,var+nametag); }
      );
  }

  //FillFatJetPlots([this,gbbcand,nametag](TString var) { return this->makeFatJetPlotName(gbbcand,var+nametag); }
  //  );

  if (m_isMC) {
    FillFatJetPlots([this,gbbcand,nametag](TString var) { return this->makePtInclPlotName(gbbcand,var+nametag); }
      );
  }

  FillFatJetPlots([this,gbbcand,nametag](TString var) { return this->makeInclPlotName(var+nametag); }
    );

}

// Plots of kinematic properties of the muons
void GbbTupleAna::FillMuonProperties(GbbCandidate* gbbcand, float event_weight, TString nametag){

  if(m_Debug) std::cout<<"processgbb(): Fill muon properties with tag "<<nametag.Data()<<std::endl;
  if (!nametag.IsNull()) nametag.Prepend("_");

  auto FillMuonPlots = [this, gbbcand, event_weight](unsigned int i_tj, unsigned int ind_mu, std::function<TString (TString)> namingFunc) {

    // Name goes in the plot name, label goes on the plot axis
    TString name = m_config->GetTrkJetName(i_tj);
    TString label = m_config->GetTrkJetLabel(i_tj);

    TLorentzVector muon;
    muon.SetPtEtaPhiE(this->muo_pt->at(ind_mu)/1000.,
                      this->muo_eta->at(ind_mu),
                      this->muo_phi->at(ind_mu),
                      this->muo_E->at(ind_mu)/1000.);

    m_HistSvc->FastFillTH1D( namingFunc(name+"mupt"),
     ";"+label+" leading muon p_{T} [GeV];Events/10 GeV;",
     muon.Pt(),50,0.,500.,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc(name+"mueta"),
     ";"+label+" leading muon #eta;Events/0.2;",
     muon.Eta(),30,-3,3,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc(name+"muphi"),
     ";"+label+" leading muon #phi;Events/0.2;",
     muon.Phi(),32,-3.2,3.2,event_weight);

    m_HistSvc->FastFillTH1D( namingFunc(name+"muSd0"),
     ";"+label+" leading muon s_{d0};Events/2;",
     this->muo_d0sig->at(ind_mu),60,-40,80,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc(name+"muSd0BL"),
     ";"+label+" leading muon s_{d0}^{BL};Events/2;",
     this->muo_d0sigBL->at(ind_mu),60,-40,80,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc(name+"muz0sintheta"),
     ";"+label+" leading muon z_{0}sin#theta;Events/0.02;",
     this->muo_z0sintheta->at(ind_mu),40,-0.4,0.4,event_weight);
  };

  for (unsigned int i=0; i < gbbcand->ind_tj.size(); i++) {
    unsigned int ind_mu = gbbcand->ind_tj_mu.at(i);
    if (ind_mu == 999) continue;

    if (gbbcand->ind_tj.size() > 1) {
      FillMuonPlots(i, ind_mu,
        [this,gbbcand,nametag](TString var) { return this->makeDiJetPlotName(gbbcand,var+nametag); }
        );
    }

    FillMuonPlots(i, ind_mu,
      [this,gbbcand,nametag](TString var) { return this->makeFatJetPlotName(gbbcand,var+nametag); }
      );

    if (m_isMC) {
      FillMuonPlots(i, ind_mu,
        [this,gbbcand,nametag,i](TString var) { return this->makePtInclTrkJetPlotName(gbbcand,i,var+nametag); }
        );

      FillMuonPlots(i, ind_mu,
        [this,gbbcand,nametag](TString var) { return this->makePtInclPlotName(gbbcand,var+nametag); }
        );
    }

    FillMuonPlots(i, ind_mu,
      [this,gbbcand,nametag](TString var) { return this->makeInclPlotName(var+nametag); }
      );
  }
}

// Plots of the Xbb variables
void GbbTupleAna::FillXbbProperties(GbbCandidate* gbbcand, float event_weight, TString nametag){
  if(m_Debug) std::cout<<"processgbb(): Fill xbb properties with tag "<<nametag.Data()<<std::endl;
  if (!nametag.IsNull()) nametag.Prepend("_");

  auto FillXbbPlots = [this, gbbcand, event_weight](std::function<TString (TString)> namingFunc) {
    m_HistSvc->FastFillTH1D( namingFunc("SubjetBScore_Higgs"),";SubjetBScore_Higgs;Events/0.02;",
     this->fat_SubjetBScore_Higgs->at(gbbcand->ind_fj),50,0.,1.,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("SubjetBScore_QCD"),";SubjetBScore_QCD;Events/0.02;",
     this->fat_SubjetBScore_QCD->at(gbbcand->ind_fj),50,0.,1.,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("SubjetBScore_Top"),";SubjetBScore_Top;Events/0.02;",
     this->fat_SubjetBScore_Top->at(gbbcand->ind_fj),50,0.,1.,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc("SubjetBScore_f0"),
     ";log(SubjetBScore_Higgs/SubjetBScore_QCD);Events/0.2;",
     std::log( this->fat_SubjetBScore_Higgs->at(gbbcand->ind_fj) /
      this->fat_SubjetBScore_QCD->at(gbbcand->ind_fj) ),
     100,-10.,10.,event_weight
    );
    m_HistSvc->FastFillTH1D( namingFunc("SubjetBScore_f0p25"),
     ";log(SubjetBScore_Higgs/(0.75*SubjetBScore_QCD+0.25*SubjetBScore_Top));Events/0.2;",
     std::log( this->fat_SubjetBScore_Higgs->at(gbbcand->ind_fj) /
      (0.75*this->fat_SubjetBScore_QCD->at(gbbcand->ind_fj) + 0.25*this->fat_SubjetBScore_Top->at(gbbcand->ind_fj)) ),
     100,-10.,10.,event_weight
    );
  };

  FillXbbPlots([this,gbbcand,nametag](TString var) { return this->makeFatJetPlotName(gbbcand,var+nametag); }
    );

  FillXbbPlots([this,gbbcand,nametag](TString var) { return this->makePtInclPlotName(gbbcand,var+nametag); }
    );

}

// Plots of track variables
void GbbTupleAna::FillTrackProperties(GbbCandidate* gbbcand, float event_weight,TString nametag){
  if(m_Debug) std::cout<<"processgbb(): Fill track properties with tag "<<nametag.Data()<<std::endl;
  if (!nametag.IsNull()) nametag.Prepend("_");

  for (unsigned int i=0; i < gbbcand->ind_tj.size(); i++) {
    unsigned int ind_tj = gbbcand->ind_tj.at(i);
    // Name goes in the plot name, label goes on the plot axis
    TString name = m_config->GetTrkJetName(i);
    TString label = m_config->GetTrkJetLabel(i);

    for (unsigned int i_trk=0; i_trk<trkjet_assocTrk_pt->at(ind_tj).size(); i_trk++) {
      m_HistSvc->FastFillTH2D(
       makeInclPlotName("trjptVstrkd0"+nametag),
       ";track |d_{0}|;track-jet p_{T} [GeV];",
       fabs(getd0(i_trk,ind_tj,m_doTrackSmearing,"nominal")),
       trkjet_assocTrk_pt->at(ind_tj).at(i_trk)/1000.,
       14,1,15,20,0.,500.,event_weight
      );
      m_HistSvc->FastFillTH2D(
       makeInclPlotName("trjptVstrkz0sintheta"+nametag),
       ";track |z_{0}sin#theta|;track-jet p_{T} [GeV];",
       fabs((getz0(i_trk,ind_tj,m_doTrackSmearing,"nominal") - this->eve_PVz
          +this->trkjet_assocTrk_vz->at(ind_tj).at(i_trk)) *
          TMath::Sin(this->trkjet_assocTrk_theta->at(ind_tj).at(i_trk))),
       trkjet_pt->at(ind_tj)/1000.,
       14,1,15,20,0.,500.,event_weight
      );
      m_HistSvc->FastFillTH2D(
       makeInclPlotName("trkd0errVStrkd0"+nametag),
       ";track #sigma_{d0};track |d_{0}|;",
       this->trkjet_assocTrk_d0err->at(ind_tj).at(i_trk),
       fabs(getd0(i_trk,ind_tj,m_doTrackSmearing,"nominal")),
       50,0.,0.1,30,-0.15,0.15,event_weight
      );
      m_HistSvc->FastFillTH2D(
       makeInclPlotName("trkd0errVStrkSd0"+nametag),
       ";track #sigma_{d0};track S_{d0};",
       this->trkjet_assocTrk_d0err->at(ind_tj).at(i_trk),
       getSd0(i_trk,ind_tj,m_doTrackSmearing,"nominal"),
       50,0.,0.1,80,-40,80,event_weight
      );
    }
    // Plot the pt fraction of the tracks used to calculate meanSd0
    std::vector<float> trk_pts = this->trkjet_assocTrk_pt->at(ind_tj);
    if (trk_pts.size() > 2) {
      std::sort(trk_pts.begin(),trk_pts.end(),std::greater<float>());
      float ptfrac = ( trk_pts.at(0) + trk_pts.at(1) + trk_pts.at(2) ) / this->trkjet_pt->at(ind_tj);
      m_HistSvc->FastFillTH1D( makeFatJetPlotName(gbbcand,name+"ptfrac"+nametag),";"+label+" p_{T} fraction (3 lead trks);Events;",
       ptfrac,100,0.,1.,event_weight);
      m_HistSvc->FastFillTH1D( makePtInclPlotName(gbbcand,name+"ptfrac"+nametag),";"+label+" p_{T} fraction (3 lead trks);Events;",
       ptfrac,100,0.,1.,event_weight);
      // Plot pt fraction vs. trk multiplicity for j1 and j2
      m_HistSvc->FastFillTH2D( makeFatJetPlotName(gbbcand,name+"ptfracVs"+name+"trkmult"+nametag),
        ";"+label+" p_{T} fraction (3 lead trks);"+label+" track multiplicity;",
        ptfrac,trkjet_nTrks->at(ind_tj),
        20,0.,1.,25,0.,50.,event_weight );
      m_HistSvc->FastFillTH2D( makePtInclPlotName(gbbcand,name+"ptfracVs"+name+"trkmult"+nametag),
        ";"+label+" p_{T} fraction (3 lead trks);"+label+" track multiplicity;",
        ptfrac,trkjet_nTrks->at(ind_tj),
        20,0.,1.,25,0.,50.,event_weight );
    }
  }
}

void GbbTupleAna::FillAdvancedProperties(GbbCandidate* gbbcand, int i_trig_jet, float event_weight,TString nametag){

  if(m_Debug) std::cout<<"processgbb(): Fill advanced properties with tag "<<nametag.Data()<<std::endl;
  if (!nametag.IsNull()) nametag.Prepend("_");

  TLorentzVector fatjet, trkjet, combtrkjet, trigjet, muon;
  fatjet.SetPtEtaPhiE(this->fat_pt->at(gbbcand->ind_fj)/1000.,
                      this->fat_eta->at(gbbcand->ind_fj),
                      this->fat_phi->at(gbbcand->ind_fj),
                      this->fat_E->at(gbbcand->ind_fj)/1000.);
  trigjet.SetPtEtaPhiE(this->jet_pt->at(i_trig_jet)/1000.,
                       this->jet_eta->at(i_trig_jet),
                       this->jet_phi->at(i_trig_jet),
                       this->jet_E->at(i_trig_jet)/1000.);


  // Extra track-jet plots
  for (unsigned int i=0; i < gbbcand->ind_tj.size(); i++) {
    unsigned int ind_tj = gbbcand->ind_tj.at(i);

    // Name goes in the plot name, label goes on the plot axis
    TString name = m_config->GetTrkJetName(i);
    TString label = m_config->GetTrkJetLabel(i);

    trkjet.SetPtEtaPhiE(this->trkjet_pt->at(ind_tj)/1000.,
                        this->trkjet_eta->at(ind_tj),
                        this->trkjet_phi->at(ind_tj),
                        this->trkjet_E->at(ind_tj)/1000.);

    combtrkjet += trkjet;

    m_HistSvc->FastFillTH1D(makePtInclTrkJetPlotName(gbbcand, i, name+"MV2c10"+nametag),
     ";"+label+" MV2c10;Events;",
     this->trkjet_MV2c10->at(ind_tj),100,-1.,1.,event_weight);

    m_HistSvc->FastFillTH1D(makePtInclTrkJetPlotName(gbbcand, i, name+"Ntrks"+nametag),
     ";"+label+" N_{tracks};Events;",
     this->trkjet_nTrks->at(ind_tj),100,0.,50.,event_weight);

    m_HistSvc->FastFillTH2D(
     makeInclPlotName("NtrksVsMu"+nametag),
     ";<mu>;track-jet N_{tracks}",
     this->eve_mu,this->trkjet_nTrks->at(ind_tj),
     80,0.,80.,50,0.,50.,event_weight);

  }

  m_HistSvc->FastFillTH1D( makeInclPlotName("dR_fj_tjs"+nametag),
   ";#Delta R(fatjet, inv. sum of track-jets);Events/0.02",
   fatjet.DeltaR(combtrkjet),25,0.,0.5,event_weight
  );
  m_HistSvc->FastFillTH1D(
   makeInclPlotName("dR_fj_trj"+nametag),
   ";#Delta R(trigger-matched jet, fat-jet);Events/0.1;",
   fatjet.DeltaR(trigjet),50,0.,5.,event_weight
  );

  //Topology: Fill Trigger Jet Pt and Eta
  m_HistSvc->FastFillTH1D(
   makeInclPlotName("trjpt"+nametag),
   ";trigger-matched jet p_{T} [GeV];Events/4 GeV;",
   this->jet_pt->at(i_trig_jet)/1000.,250,0.,1000.,event_weight
  );
  m_HistSvc->FastFillTH1D(
   makeInclPlotName("trjeta"+nametag),
   ";trigger-matched #eta;Events/0.2;",
   this->jet_eta->at(i_trig_jet),10,-2.5,2.5,event_weight
  );

  m_HistSvc->FastFillTH1D(
   makeInclPlotName("trjfjptratio"+nametag),
   ";trig-jet p_{T} / large-R jet p_{T};Events/2;",
   this->jet_pt->at(i_trig_jet)/this->fat_pt->at(gbbcand->ind_fj),50,0.,10.,event_weight
  );

  m_HistSvc->FastFillTH2D(
   makeInclPlotName("trjptVsfjpt"+nametag),
   ";trig-jet p_{T} [GeV];large-R jet p_{T} [GeV];",
   this->jet_pt->at(i_trig_jet)/1000.,this->fat_pt->at(gbbcand->ind_fj)/1000.,
   250,0.,1000.,250,0.,1000.,event_weight
  );

}

void GbbTupleAna::FillTriggerTurnOnHistograms(int i_trigjet, float event_weight){

  if(m_Debug) std::cout<<"processgbb(): Fill trigger turn-on"<<std::endl;
  //Fill Histograms for Trigger turn-on curves
  if (eve_HLT_j150) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j150", jet_pt->at(i_trigjet)/1000., 100, 0., 1000., event_weight);
  if (eve_HLT_j175) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j175", jet_pt->at(i_trigjet)/1000., 100, 0., 1000., event_weight);
  if (eve_HLT_j200) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j200", jet_pt->at(i_trigjet)/1000., 100, 0., 1000., event_weight);
  if (eve_HLT_j260) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j260", jet_pt->at(i_trigjet)/1000., 100, 0., 1000., event_weight);
  if (eve_HLT_j300) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j300", jet_pt->at(i_trigjet)/1000., 100, 0., 1000., event_weight);
  if (eve_HLT_j320) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j320", jet_pt->at(i_trigjet)/1000., 100, 0., 1000., event_weight);
  if (eve_HLT_j360) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j360", jet_pt->at(i_trigjet)/1000., 100, 0., 1000., event_weight);
  if (eve_HLT_j380) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j380", jet_pt->at(i_trigjet)/1000., 100, 0., 1000., event_weight);
  if (eve_HLT_j400) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j400", jet_pt->at(i_trigjet)/1000., 100, 0., 1000., event_weight);
  if (eve_HLT_j420) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j420", jet_pt->at(i_trigjet)/1000., 100, 0., 1000., event_weight);

  if (eve_HLT_j260_a10_lcw_L1J75) m_HistSvc->FastFillTH1D("fatjet_pt_passHLT_j260_a10_lcw_L1J75", fat_pt->at(0)/1000., 100, 0., 1000., event_weight);
  if (eve_HLT_j260_a10_lcw_subjes_L1J75) m_HistSvc->FastFillTH1D("fatjet_pt_passHLT_j260_a10_lcw_subjes_L1J75", fat_pt->at(0)/1000., 100, 0., 1000., event_weight);
  if (eve_HLT_j260_a10t_lcw_jes_L1J75) m_HistSvc->FastFillTH1D("fatjet_pt_passHLT_j260_a10t_lcw_jes_L1J75", fat_pt->at(0)/1000., 100, 0., 1000., event_weight);
  if (eve_HLT_j400_a10_lcw_L1J100) m_HistSvc->FastFillTH1D("fatjet_pt_passHLT_j400_a10_lcw_L1J100", fat_pt->at(0)/1000., 100, 0., 1000., event_weight);
  if (eve_HLT_j420_a10_lcw_L1J100) m_HistSvc->FastFillTH1D("fatjet_pt_passHLT_j420_a10_lcw_L1J100", fat_pt->at(0)/1000., 100, 0., 1000., event_weight);
  if (eve_HLT_j440_a10_lcw_subjes_L1J100) m_HistSvc->FastFillTH1D("fatjet_pt_passHLT_j440_a10_lcw_subjes_L1J100", fat_pt->at(0)/1000., 100, 0., 1000., event_weight);
  if (eve_HLT_j440_a10t_lcw_jes_L1J100) m_HistSvc->FastFillTH1D("fatjet_pt_passHLT_j440_a10t_lcw_jes_L1J100", fat_pt->at(0)/1000., 100, 0., 1000., event_weight);
  if (eve_HLT_j460_a10_lcw_subjes_L1J100) m_HistSvc->FastFillTH1D("fatjet_pt_passHLT_j460_a10_lcw_subjes_L1J100", fat_pt->at(0)/1000., 100, 0., 1000., event_weight);
  if (eve_HLT_j460_a10t_lcw_jes_L1J100) m_HistSvc->FastFillTH1D("fatjet_pt_passHLT_j460_a10t_lcw_jes_L1J100", fat_pt->at(0)/1000., 100, 0., 1000., event_weight);

  TLorentzVector fatjet1, fatjet2;
  fatjet1.SetPtEtaPhiE(this->fat_pt->at(0),this->fat_eta->at(0),this->fat_phi->at(0),this->fat_E->at(0));
  if (eve_HLT_2j330_a10t_lcw_jes_35smcINF_L1J100) m_HistSvc->FastFillTH1D("fatjet_m_passHLT_2j330_a10t_lcw_jes_35smcINF_L1J100", fatjet1.M()/1000., 100, 0., 100., event_weight);
  if (eve_HLT_2j330_a10t_lcw_jes_40smcINF_L1J100) m_HistSvc->FastFillTH1D("fatjet_m_passHLT_2j330_a10t_lcw_jes_40smcINF_L1J100", fatjet1.M()/1000., 100, 0., 100., event_weight);

  if (eve_HLT_mu20_iloose_L1MU15) m_HistSvc->FastFillTH1D("mu_pt_passHLT_mu20_iloose_L1MU15", muo_pt->at(0)/1000., 50, 0., 50., event_weight);
  if (eve_HLT_mu24_ivarloose) m_HistSvc->FastFillTH1D("mu_pt_passHLT_mu24_ivarloose", muo_pt->at(0)/1000., 50, 0., 50., event_weight);
  if (eve_HLT_mu24_ivarloose_L1MU15) m_HistSvc->FastFillTH1D("mu_pt_passHLT_mu24_ivarloose_L1MU15", muo_pt->at(0)/1000., 50, 0., 50., event_weight);
  if (eve_HLT_mu26_ivarmedium) m_HistSvc->FastFillTH1D("mu_pt_passHLT_mu26_ivarmedium", muo_pt->at(0)/1000., 50, 0., 50., event_weight);

  m_HistSvc->FastFillTH1D("trigjet_pt_noHLTreq",this->jet_pt->at(i_trigjet)/1000.,100,0.,1000.,event_weight);
  m_HistSvc->FastFillTH1D("mu_pt_noHLTreq",muo_pt->at(0)/1000.,50,0.,50.,event_weight);
  m_HistSvc->FastFillTH1D("fatjet_pt_noHLTreq",fat_pt->at(0)/1000.,100,0.,1000.,event_weight);
  if (fatjet1.M()/1000. > 50) {
    if (eve_HLT_j420_a10t_lcw_jes_35smcINF_L1J100) m_HistSvc->FastFillTH1D("fatjet_pt_passHLT_j420_a10t_lcw_jes_35smcINF_L1J100", fat_pt->at(0)/1000., 100, 0., 1000., event_weight);
    if (eve_HLT_j420_a10t_lcw_jes_40smcINF_L1J100) m_HistSvc->FastFillTH1D("fatjet_pt_passHLT_j420_a10t_lcw_jes_40smcINF_L1J100", fat_pt->at(0)/1000., 100, 0., 1000., event_weight);
    m_HistSvc->FastFillTH1D("fatjet_pt_noHLTreq_m50",fat_pt->at(0)/1000.,100,0.,1000.,event_weight);
  }
  if (fatjet1.Pt()/1000. > 450) {
    if (eve_HLT_j420_a10t_lcw_jes_35smcINF_L1J100) m_HistSvc->FastFillTH1D("fatjet_m_passHLT_j420_a10t_lcw_jes_35smcINF_L1J100", fatjet1.M()/1000., 100, 0., 100., event_weight);
    if (eve_HLT_j420_a10t_lcw_jes_40smcINF_L1J100) m_HistSvc->FastFillTH1D("fatjet_m_passHLT_j420_a10t_lcw_jes_40smcINF_L1J100", fatjet1.M()/1000., 100, 0., 100., event_weight);
    m_HistSvc->FastFillTH1D("fatjet_m_noHLTreq_pt450",fatjet1.M()/1000.,100,0.,100.,event_weight);
  }

  if (fat_pt->size() > 1) {
    fatjet2.SetPtEtaPhiE(this->fat_pt->at(1),this->fat_eta->at(1),this->fat_phi->at(1),this->fat_E->at(1));
    if (fatjet1.M()/1000. > 50 && fatjet2.M()/1000. > 50) {
      if (eve_HLT_2j330_a10t_lcw_jes_35smcINF_L1J100) m_HistSvc->FastFillTH1D("fatjet_pt1_passHLT_2j330_a10t_lcw_jes_35smcINF_L1J100", fat_pt->at(0)/1000., 100, 0., 1000., event_weight);
      if (eve_HLT_2j330_a10t_lcw_jes_40smcINF_L1J100) m_HistSvc->FastFillTH1D("fatjet_pt1_passHLT_2j330_a10t_lcw_jes_40smcINF_L1J100", fat_pt->at(0)/1000., 100, 0., 1000., event_weight);
      if (eve_HLT_2j330_a10t_lcw_jes_35smcINF_L1J100) m_HistSvc->FastFillTH1D("fatjet_pt2_passHLT_2j330_a10t_lcw_jes_35smcINF_L1J100", fat_pt->at(1)/1000., 100, 0., 1000., event_weight);
      if (eve_HLT_2j330_a10t_lcw_jes_40smcINF_L1J100) m_HistSvc->FastFillTH1D("fatjet_pt2_passHLT_2j330_a10t_lcw_jes_40smcINF_L1J100", fat_pt->at(1)/1000., 100, 0., 1000., event_weight);
      m_HistSvc->FastFillTH1D("fatjet_pt1_noHLTreq_m50",fat_pt->at(0)/1000.,100,0.,1000.,event_weight);
      m_HistSvc->FastFillTH1D("fatjet_pt2_noHLTreq_m50",fat_pt->at(1)/1000.,100,0.,1000.,event_weight);
    }
    if (fatjet1.Pt()/1000. > 350 && fatjet2.Pt()/1000. > 350) {
      if (eve_HLT_2j330_a10t_lcw_jes_35smcINF_L1J100) m_HistSvc->FastFillTH1D("fatjet_m1_passHLT_2j330_a10t_lcw_jes_35smcINF_L1J100", fatjet1.M()/1000., 100, 0., 100., event_weight);
      if (eve_HLT_2j330_a10t_lcw_jes_40smcINF_L1J100) m_HistSvc->FastFillTH1D("fatjet_m1_passHLT_2j330_a10t_lcw_jes_40smcINF_L1J100", fatjet1.M()/1000., 100, 0., 100., event_weight);
      if (eve_HLT_2j330_a10t_lcw_jes_35smcINF_L1J100) m_HistSvc->FastFillTH1D("fatjet_m2_passHLT_2j330_a10t_lcw_jes_35smcINF_L1J100", fatjet2.M()/1000., 100, 0., 100., event_weight);
      if (eve_HLT_2j330_a10t_lcw_jes_40smcINF_L1J100) m_HistSvc->FastFillTH1D("fatjet_m2_passHLT_2j330_a10t_lcw_jes_40smcINF_L1J100", fatjet2.M()/1000., 100, 0., 100., event_weight);
      m_HistSvc->FastFillTH1D("fatjet_m1_noHLTreq_pt350",fatjet1.M()/1000.,100,0.,100.,event_weight);
      m_HistSvc->FastFillTH1D("fatjet_m2_noHLTreq_pt350",fatjet2.M()/1000.,100,0.,100.,event_weight);
    }
  }

}
