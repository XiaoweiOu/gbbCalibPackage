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
#include <stdexcept>

TString GbbTupleAna::makeInclTrkJetPlotName(GbbCandidate* gbbcand, unsigned int i, const TString var) {
  if (i >= gbbcand->ind_tj.size()) throw std::out_of_range("Requested track-jet does not exist");
  unsigned int ind_tj = gbbcand->ind_tj.at(i);
  int truth = trkjet_truth->at(ind_tj);
  TString flav = this->eve_isMC ? m_config->GetFlavour(truth) : TString("Data");
  return m_config->GetMCHistName(m_SysVarName,"Incl",flav,var);
}

TString GbbTupleAna::makeDiJetPlotName(GbbCandidate* gbbcand, const TString var) {
  if (gbbcand->ind_tj.size() < 2) throw std::out_of_range("Requested track-jet does not exist");
  int j1_truth = this->trkjet_truth->at(gbbcand->ind_tj.at(0));
  int j2_truth = this->trkjet_truth->at(gbbcand->ind_tj.at(1));
  TString dijet_flav = this->eve_isMC ? m_config->GetFlavourPair(j1_truth,j2_truth) : TString("Data");
  TString ptlabel = m_config->GetDiTrkJetLabel(this->trkjet_pt->at(gbbcand->ind_tj.at(0))/1e3,
                                               this->trkjet_pt->at(gbbcand->ind_tj.at(1))/1e3);
  return m_config->GetMCHistName(m_SysVarName,ptlabel,dijet_flav,var);
}

TString GbbTupleAna::makeFatJetPlotName(GbbCandidate* gbbcand, const TString var) {
  if (gbbcand->ind_tj.size() < 2) throw std::out_of_range("Requested track-jet does not exist");
  int j1_truth = this->trkjet_truth->at(gbbcand->ind_tj.at(0));
  int j2_truth = this->trkjet_truth->at(gbbcand->ind_tj.at(1));
  TString dijet_flav = this->eve_isMC ? m_config->GetFlavourPair(j1_truth,j2_truth) : TString("Data");
  TString ptlabel = m_config->GetFatJetLabel(this->fat_pt->at(gbbcand->ind_fj)/1e3);
  return m_config->GetMCHistName(m_SysVarName,ptlabel,dijet_flav,var);
}

TString GbbTupleAna::makeInclDiJetPlotName(GbbCandidate* gbbcand, const TString var) {
  if (gbbcand->ind_tj.size() < 2) throw std::out_of_range("Requested track-jet does not exist");
  int j1_truth = this->trkjet_truth->at(gbbcand->ind_tj.at(0));
  int j2_truth = this->trkjet_truth->at(gbbcand->ind_tj.at(1));
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

void GbbTupleAna::FillTrackJetProperties(GbbCandidate* gbbcand, float event_weight, TString nametag){

  if(m_Debug) std::cout<<"processgbb(): Fill trackjet properties with tag "<<nametag.Data()<<std::endl;
  if (!nametag.IsNull()) nametag.Prepend("_");

  for (unsigned int i=0; i < gbbcand->ind_tj.size(); i++) {
    unsigned int ind_tj = gbbcand->ind_tj.at(i);

    // Name goes in the plot name, label goes on the plot axis
    TString name = m_config->GetTrkJetName(i);
    TString label = m_config->GetTrkJetLabel(i);

    // Plot track-jet kinematics with single-flavour label
    m_HistSvc->FastFillTH1D( makeInclTrkJetPlotName(gbbcand,i,name+"pt"+nametag),";"+label+" p_{T} [GeV];Events/2 GeV;",
     this->trkjet_pt->at(ind_tj)/1e3,250,0.,500.,event_weight);
    m_HistSvc->FastFillTH1D( makeInclTrkJetPlotName(gbbcand,i,name+"eta"+nametag),";"+label+" #eta;Events/0.2;",
     this->trkjet_eta->at(ind_tj),100,-2.5,2.5,event_weight);

    // Plot track-jet kinematics with di-flavour label
    m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,name+"pt"+nametag),";"+label+" p_{T} [GeV];Events/2 GeV;",
     this->trkjet_pt->at(ind_tj)/1e3,250,0.,500.,event_weight);
    m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,name+"eta"+nametag),";"+label+" #eta;Events/0.2;",
     this->trkjet_eta->at(ind_tj),100,-2.5,2.5,event_weight);

    m_HistSvc->FastFillTH1D( makeInclDiJetPlotName(gbbcand,name+"pt"+nametag),";"+label+" p_{T} [GeV];Events/2 GeV;",
     this->trkjet_pt->at(ind_tj)/1e3,250,0.,500.,event_weight);
    m_HistSvc->FastFillTH1D( makeInclDiJetPlotName(gbbcand,name+"eta"+nametag),";"+label+" #eta;Events/0.2;",
     this->trkjet_eta->at(ind_tj),100,-2.5,2.5,event_weight);

    m_HistSvc->FastFillTH1D( makeFatJetPlotName(gbbcand,name+"pt"+nametag),";"+label+" p_{T} [GeV];Events/2 GeV;",
     this->trkjet_pt->at(ind_tj)/1e3,250,0.,500.,event_weight);
    m_HistSvc->FastFillTH1D( makeFatJetPlotName(gbbcand,name+"eta"+nametag),";"+label+" #eta;Events/0.2;",
     this->trkjet_eta->at(ind_tj),100,-2.5,2.5,event_weight);

    // Plot the pt fraction of the tracks used to calculate meanSd0
    std::vector<float> trk_pts = this->trkjet_assocTrk_pt->at(ind_tj);
    if (trk_pts.size() > 2) {
      std::sort(trk_pts.begin(),trk_pts.end(),std::greater<float>());
      float ptfrac = ( trk_pts.at(0) + trk_pts.at(1) + trk_pts.at(2) ) / this->trkjet_pt->at(ind_tj);
      m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,name+"ptfrac"+nametag),";"+label+" p_{T} fraction (3 lead trks);Events;",
       ptfrac,100,0.,1.,event_weight);
      m_HistSvc->FastFillTH1D( makeInclDiJetPlotName(gbbcand,name+"ptfrac"+nametag),";"+label+" p_{T} fraction (3 lead trks);Events;",
       ptfrac,100,0.,1.,event_weight);
      // Plot pt fraction vs. trk multiplicity for j1 and j2
      m_HistSvc->FastFillTH2D( makeDiJetPlotName(gbbcand,name+"ptfracVs"+name+"trkmult"+nametag),
        ";"+label+" p_{T} fraction (3 lead trks);"+label+" track multiplicity;",
        ptfrac,this->trkjet_assocTrk_pt->at(ind_tj).size(),
        20,0.,1.,25,0.,50.,event_weight );
      m_HistSvc->FastFillTH2D( makeInclDiJetPlotName(gbbcand,name+"ptfracVs"+name+"trkmult"+nametag),
        ";"+label+" p_{T} fraction (3 lead trks);"+label+" track multiplicity;",
        ptfrac,this->trkjet_assocTrk_pt->at(ind_tj).size(),
        20,0.,1.,25,0.,50.,event_weight );
    }

    for (unsigned int j=i+1; j < gbbcand->ind_tj.size(); j++) {
      unsigned int ind_tj2 = gbbcand->ind_tj.at(j);

      // Name goes in the plot name, label goes on the plot axis
      TString name2 = m_config->GetTrkJetName(j);
      TString label2 = m_config->GetTrkJetLabel(j);

      // Make pt correlation plot
      m_HistSvc->FastFillTH2D( makeInclDiJetPlotName(gbbcand,name+"ptVs"+name2+"pt"+nametag),
        ";"+label+" p_{T} [GeV];"+label2+" jet p_{T} [GeV];",
        this->trkjet_pt->at(ind_tj)/1e3,this->trkjet_pt->at(ind_tj2)/1e3,
        250,0.,500.,250,0.,500.,event_weight
      );
      TLorentzVector tj_vec, tj2_vec;
      tj_vec.SetPtEtaPhiE( this->trkjet_pt->at(ind_tj)/1e3,
                           this->trkjet_eta->at(ind_tj),
                           this->trkjet_phi->at(ind_tj),
                           this->trkjet_E->at(ind_tj)/1e3);
      tj2_vec.SetPtEtaPhiE( this->trkjet_pt->at(ind_tj2)/1e3,
                            this->trkjet_eta->at(ind_tj2),
                            this->trkjet_phi->at(ind_tj2),
                            this->trkjet_E->at(ind_tj2)/1e3);
      m_HistSvc->FastFillTH1D(
       makeFatJetPlotName(gbbcand,"dR_"+name+"_"+name2+nametag),
       ";#Delta R(muon-jet,non-muon jet);Events/0.01;",
       tj_vec.DeltaR(tj2_vec),100,0.,1.0,event_weight
      );
    }
  }

  // Plot the amount of times j1 and j2 are the two leading jets in the candidate
  m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"hasLeadTrkJets"+nametag),";hasLeadTrkJets;Events;",
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

    //m_HistSvc->FastFillTH1D( namingFunc(name+"maxSd0"),";"+label+" leading s_{d0};Events/1;",
    // sd0Info.maxSd0,120,-40.,80.,event_weight);
    //m_HistSvc->FastFillTH1D( namingFunc(name+"subSd0"),";"+label+" sub-leading s_{d0};Events/1;",
    // sd0Info.subSd0,120,-40.,80.,event_weight);
    //m_HistSvc->FastFillTH1D( namingFunc(name+"thirdSd0"),";"+label+" third-leading s_{d0};Events/1;",
    // sd0Info.thirdSd0,120,-40.,80.,event_weight);
    m_HistSvc->FastFillTH1D( namingFunc(name+"meanSd0"),";"+label+" mean s_{d0};Events/1;",
     sd0Info.meanSd0_pt,120,-40.,80.,event_weight);

    // Using the giving plot naming function to make plots of extra d0 and sd0 variables
    if(m_RunMode & RunMode::FILL_ADV_PROPERTIES && m_isNominal) {
      m_HistSvc->FastFillTH1D( namingFunc(name+"maxSd0DR"),
       ";"+label+" #Delta R(jet,track_{max s_{d0}});Events/0.02;",
       sd0Info.maxSd0_dR,25,0.,0.5,event_weight);

      m_HistSvc->FastFillTH1D( namingFunc(name+"maxSd0_pt"),";"+label+" leading track s_{d0};Events/1;",
       sd0Info.maxSd0_pt,120,-40.,80.,event_weight);
      m_HistSvc->FastFillTH1D( namingFunc(name+"subSd0_pt"),";"+label+" sub-leading track s_{d0};Events/1;",
       sd0Info.subSd0_pt,120,-40.,80.,event_weight);
      m_HistSvc->FastFillTH1D( namingFunc(name+"thirdSd0_pt"),";"+label+" third-leading track s_{d0};Events/1;",
       sd0Info.thirdSd0_pt,120,-40.,80.,event_weight);
      if (sd0Info.meanSd0_pt < 0) {
        m_HistSvc->FastFillTH1D( namingFunc(name+"maxSd0_pt_NEGMEANSD0"),";"+label+" leading track s_{d0};Events/1;",
         sd0Info.maxSd0_pt,120,-40.,80.,event_weight);
        m_HistSvc->FastFillTH1D( namingFunc(name+"subSd0_pt_NEGMEANSD0"),";"+label+" sub-leading track s_{d0};Events/1;",
         sd0Info.subSd0_pt,120,-40.,80.,event_weight);
        m_HistSvc->FastFillTH1D( namingFunc(name+"thirdSd0_pt_NEGMEANSD0"),";"+label+" third-leading track s_{d0};Events/1;",
         sd0Info.thirdSd0_pt,120,-40.,80.,event_weight);
      }
      //m_HistSvc->FastFillTH1D( namingFunc(name+"meanSd0_sd0"),";"+label+" mean s_{d0};Events/1;",
      // sd0Info.meanSd0_sd0,120,-40.,80.,event_weight);

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
     j1Sd0Info.maxSd0,j2Sd0Info.maxSd0, 120,-40.,80., 120,-40.,80.,event_weight);

  m_HistSvc->FastFillTH2D( namingFunc("j1meanSd0_j2meanSd0"),";muon-jet mean s_{d0};non-muon-jet mean s_{d0};",
     j1Sd0Info.meanSd0_pt,j2Sd0Info.meanSd0_pt, 120,-40.,80., 120,-40.,80.,event_weight);

  m_HistSvc->FastFillTH2D( namingFunc("j1subSd0_j2subSd0"),";muon-jet sub leading track s_{d0};non-muon-jet sub leading track s_{d0};",
     j1Sd0Info.subSd0,j2Sd0Info.subSd0, 120,-40.,80., 120,-40.,80.,event_weight);

  m_HistSvc->FastFillTH2D( namingFunc("j1thirdSd0_j2thirdSd0"),";muon-jet third leading track s_{d0};non-muon-jet third leading track s_{d0};",
     j1Sd0Info.thirdSd0,j2Sd0Info.thirdSd0, 120,-40.,80., 120,-40.,80.,event_weight);

  m_HistSvc->FastFillTH2D( namingFunc("j1maxSd0_j1subSd0"),";muon-jet leading track s_{d0};muon-jet sub leading track s_{d0};",
     j1Sd0Info.maxSd0,j1Sd0Info.subSd0, 120,-40.,80., 120,-40.,80.,event_weight);

  m_HistSvc->FastFillTH2D( namingFunc("j1maxSd0_j1thirdSd0"),";muon-jet leading track s_{d0};muon-jet third leading track s_{d0};",
     j1Sd0Info.maxSd0,j1Sd0Info.thirdSd0, 120,-40.,80., 120,-40.,80.,event_weight);

  m_HistSvc->FastFillTH2D( namingFunc("j1subSd0_j1thirdSd0"),";muon-jet sub leading track s_{d0};muon-jet third leading track s_{d0};",
     j1Sd0Info.subSd0,j1Sd0Info.thirdSd0, 120,-40.,80., 120,-40.,80.,event_weight);

  m_HistSvc->FastFillTH2D( namingFunc("j2maxSd0_j2subSd0"),";non-muon-jet leading track s_{d0};non-muon-jet sub leading track s_{d0};",
     j2Sd0Info.maxSd0,j2Sd0Info.subSd0, 120,-40.,80., 120,-40.,80.,event_weight);

  m_HistSvc->FastFillTH2D( namingFunc("j2maxSd0_j2thirdSd0"),";non-muon-jet leading track s_{d0};non-muon-jet third leading track s_{d0};",
     j2Sd0Info.maxSd0,j2Sd0Info.thirdSd0, 120,-40.,80., 120,-40.,80.,event_weight);

  m_HistSvc->FastFillTH2D( namingFunc("j2subSd0_j2thirdSd0"),";non-muon-jet sub leading track s_{d0};non-muon-jet third leading track s_{d0};",
     j2Sd0Info.subSd0,j2Sd0Info.thirdSd0, 120,-40.,80., 120,-40.,80.,event_weight);
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

  // Require at least 2 track-jets for now
  if(sd0InfoVec.size() < 2) return;
  if(sd0InfoVec[0].meanSd0_pt == -99. || sd0InfoVec[1].meanSd0_pt == -99.) return;

  int j1_truth=this->trkjet_truth->at(gbbcand->ind_tj.at(0));
  int j2_truth=this->trkjet_truth->at(gbbcand->ind_tj.at(1));
  TString dijet_flav = this->eve_isMC ? m_config->GetFlavourPair(j1_truth,j2_truth) : TString("Data");

  TString ptlabel = m_config->GetDiTrkJetLabel(this->trkjet_pt->at(gbbcand->ind_tj.at(0))/1e3,
                                               this->trkjet_pt->at(gbbcand->ind_tj.at(1))/1e3);

  // Make inclusive plots of s_d0 variables
  // Custom lambda function defines names of plots
  FillSd0Plots(sd0InfoVec, event_weight,
   [this,gbbcand,nametag](TString var) { return this->makeInclDiJetPlotName(gbbcand,var+nametag); }
  );

  // Make plots in pT bins
  FillSd0Plots(sd0InfoVec, event_weight,
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

  // Make plots in fatjet pT bins
  FillSd0Plots(sd0InfoVec, event_weight,
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
    m_HistSvc->FastFillTH1D( makeInclDiJetPlotName(gbbcand,"mjpt"+nametag),";muon-jet p_{T} [GeV];Events/2 GeV;",
     this->trkjet_pt->at(gbbcand->ind_tj.at(0))/1e3,250,0.,500.,event_weight);
    m_HistSvc->FastFillTH1D( makeDiJetPlotName(gbbcand,"mjpt"+nametag),";muon-jet p_{T} [GeV];Events/2 GeV;",
     this->trkjet_pt->at(gbbcand->ind_tj.at(0))/1e3,250,0.,500.,event_weight);
  }

  if (m_isNominal && m_isMC && m_doSd0Systematics) {

    auto makeSysNamingFunc = [config=this->m_config,dijet_flav,ptlabel,nametag](TString sys)
     -> std::function<TString (TString)> {
      return [config,sys,dijet_flav,ptlabel,nametag](TString var) {
        return config->GetMCHistName(sys,ptlabel,dijet_flav,var+nametag);
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

void GbbTupleAna::FillMCStatsInfo(GbbCandidate* gbbcand, TString nametag){

  if (!nametag.IsNull()) nametag.Prepend("_");

  m_HistSvc->FastFillTH2D( makeInclDiJetPlotName(gbbcand,"j1pt_vs_j2pt_unweighted"+nametag),
   ";muon-jet p_{T} [GeV];non-muon-jet p_{T} [GeV];",
   this->trkjet_pt->at(gbbcand->ind_tj.at(0))/1e3,this->trkjet_pt->at(gbbcand->ind_tj.at(1))/1e3,
   50,0.,200.,50,0.,200.,1.
  );

}

void GbbTupleAna::FillFatJetProperties(GbbCandidate* gbbcand, float event_weight, TString nametag){

  if(m_Debug) std::cout<<"processgbb(): Fill fatjet properties with tag "<<nametag.Data()<<std::endl;
  if (!nametag.IsNull()) nametag.Prepend("_");

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

  TLorentzVector fatjet, trkjet, combtrkjet, trigjet, muon;
  fatjet.SetPtEtaPhiE(this->fat_pt->at(gbbcand->ind_fj),this->fat_eta->at(gbbcand->ind_fj),this->fat_phi->at(gbbcand->ind_fj),this->fat_E->at(gbbcand->ind_fj));
  trigjet.SetPtEtaPhiE(this->jet_pt->at(i_trig_jet),this->jet_eta->at(i_trig_jet),this->jet_phi->at(i_trig_jet),this->jet_E->at(i_trig_jet));

  if (!nametag.IsNull()) nametag.Prepend("_");

  if (!m_isNominal) return;

  //Topology: Plot DR(fatjet,muonjet)
  for (unsigned int i=0; i < gbbcand->ind_tj.size(); i++) {
    unsigned int ind_tj = gbbcand->ind_tj.at(i);

    // Name goes in the plot name, label goes on the plot axis
    TString name = m_config->GetTrkJetName(i);
    TString label = m_config->GetTrkJetLabel(i);

    trkjet.SetPtEtaPhiE(this->trkjet_pt->at(ind_tj),this->trkjet_eta->at(ind_tj),this->trkjet_phi->at(ind_tj),this->trkjet_E->at(ind_tj));

    combtrkjet += trkjet;

    for (unsigned int i_trk=0; i_trk<trkjet_assocTrk_pt->at(ind_tj).size(); i_trk++) {
      m_HistSvc->FastFillTH2D(
       m_config->GetMCHistName("Nom","Incl","Incl","trjptVstrkd0"+nametag),
       ";track |d_{0}|;track-jet p_{T} [GeV];",
       fabs(getd0(i_trk,ind_tj,m_doTrackSmearing,"nominal")),
       trkjet_assocTrk_pt->at(ind_tj).at(i_trk)/1e3,
       14,1,15,100,0.,500.,event_weight
      );
      m_HistSvc->FastFillTH2D(
       m_config->GetMCHistName("Nom","Incl","Incl","trjptVstrkz0sintheta"+nametag),
       ";track |z_{0}sin#theta|;track-jet p_{T} [GeV];",
       fabs((getz0(i_trk,ind_tj,m_doTrackSmearing,"nominal") - this->eve_PVz
          +this->trkjet_assocTrk_vz->at(ind_tj).at(i_trk)) *
          TMath::Sin(this->trkjet_assocTrk_theta->at(ind_tj).at(i_trk))),
       trkjet_pt->at(ind_tj)/1e3,
       14,1,15,100,0.,500.,event_weight
      );
      m_HistSvc->FastFillTH2D(
       m_config->GetMCHistName("Nom","Incl","Incl","trkd0errVStrkd0"+nametag),
       ";track #sigma_{d0};track |d_{0}|;",
       this->trkjet_assocTrk_d0err->at(ind_tj).at(i_trk),
       fabs(getd0(i_trk,ind_tj,m_doTrackSmearing,"nominal")),
       100,0.,0.1,30,-0.15,0.15,event_weight
      );
      m_HistSvc->FastFillTH2D(
       m_config->GetMCHistName("Nom","Incl","Incl","trkd0errVStrkSd0"+nametag),
       ";track #sigma_{d0};track S_{d0};",
       this->trkjet_assocTrk_d0err->at(ind_tj).at(i_trk),
       getSd0(i_trk,ind_tj,m_doTrackSmearing,"nominal"),
       100,0.,0.1,80,-40,80,event_weight
      );
    }

    m_HistSvc->FastFillTH1D(makeInclTrkJetPlotName(gbbcand, i, name+"MV2c10"+nametag),
     ";"+label+" MV2c10;Events;",
     this->trkjet_MV2c10->at(ind_tj),100,-1.,1.,event_weight);

    if(!m_config->GetIsR20p7()){

      m_HistSvc->FastFillTH1D(makeInclTrkJetPlotName(gbbcand, i, name+"Ntrks"+nametag),
       ";"+label+" N_{tracks};Events;",
       this->trkjet_nTrks->at(ind_tj),100,0.,50.,event_weight);

      m_HistSvc->FastFillTH2D(
       makeFlavInclPlotName("NtrksVsMu"+nametag),
       ";<mu>;track-jet N_{tracks}",
       this->eve_mu,this->trkjet_nTrks->at(ind_tj),
       80,0.,80.,50,0.,50.,event_weight);
    }

    // Topology: Muon Pt and Eta
    unsigned int ind_mu = gbbcand->ind_tj_mu.at(i);
    if (ind_mu != 999) {
      muon.SetPtEtaPhiE(this->muo_pt->at(ind_mu),this->muo_eta->at(ind_mu),this->muo_phi->at(ind_mu),this->muo_E->at(ind_mu));
      m_HistSvc->FastFillTH1D(
       makeInclDiJetPlotName(gbbcand, name+"mupt"+nametag),
       ";"+label+" leading muon p_{T} [GeV];Events/4 GeV;",
       this->muo_pt->at(ind_mu)/1e3,100,0.,400.,event_weight
      );
      m_HistSvc->FastFillTH1D(
       makeInclDiJetPlotName(gbbcand, name+"muphi"+nametag),
       ";"+label+" leading muon #phi;Events/0.4;",
       this->muo_phi->at(ind_mu),16,-3.2,3.2,event_weight
      );
      m_HistSvc->FastFillTH1D(
       makeInclDiJetPlotName(gbbcand, name+"mueta"+nametag),
       ";"+label+" leading muon #eta;Events/0.5;",
       this->muo_eta->at(ind_mu),10,-2.5,2.5,event_weight
      );
      m_HistSvc->FastFillTH1D(
       makeInclDiJetPlotName(gbbcand, name+"muE"+nametag),
       ";"+label+" leading muon E [GeV];Events/4 GeV;",
       this->muo_E->at(ind_mu)/1e3,100,0.,400.,event_weight
      );
      m_HistSvc->FastFillTH1D(
       makeInclDiJetPlotName(gbbcand, name+"mud0"+nametag),
       ";"+label+" leading muon d0;Events/0.001;",
       this->muo_d0->at(ind_mu),30,-0.15,0.15,event_weight
      );
      m_HistSvc->FastFillTH1D(
       makeInclDiJetPlotName(gbbcand, name+"mud0sig"+nametag),
       ";"+label+" leading muon Sd0;Events/1.5;",
       this->muo_d0sig->at(ind_mu),80,-40,80,event_weight
      );
      m_HistSvc->FastFillTH1D(
       makeInclDiJetPlotName(gbbcand, name+"mud0sigBL"+nametag),
       ";"+label+" leading muon Sd0_{BL};Events/1.5;",
       this->muo_d0sigBL->at(ind_mu),80,-40,80,event_weight
      );
      m_HistSvc->FastFillTH1D(
       makeInclDiJetPlotName(gbbcand, name+"muz0"+nametag),
       ";"+label+" leading muon z0;Events/0.001;",
       this->muo_z0->at(ind_mu),90,-0.4,0.4,event_weight
      );
      m_HistSvc->FastFillTH1D(
       makeInclDiJetPlotName(gbbcand, name+"muz0sintheta"+nametag),
       ";"+label+" leading muon z0sintheta;Events/0.001;",
       this->muo_z0sintheta->at(ind_mu),90,-0.4,0.4,event_weight
      );
      m_HistSvc->FastFillTH1D(
       makeInclDiJetPlotName(gbbcand, name+"muptrel"+nametag),
       ";"+label+" leading muon p_{T}-rel [GeV];Events/0.25 GeV;",
       muon.Pt(trkjet.Vect())/1e3,40,0.,10.,event_weight
      );
    }
  }

  m_HistSvc->FastFillTH1D( makeFlavInclPlotName("dR_fj_tjs"+nametag),
   ";#Delta R(fatjet, inv. sum of track-jets);Events/0.02",
   fatjet.DeltaR(combtrkjet),25,0.,0.5,event_weight
  );
  m_HistSvc->FastFillTH1D(
   makeFlavInclPlotName("dR_fj_trj"+nametag),
   ";#Delta R(trigger-matched jet, muon-jet);Events/0.1;",
   fatjet.DeltaR(trigjet),50,0.,5.,event_weight
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

  if(!m_config->GetIsR20p7()){

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
  if (eve_HLT_j260_a10_lcw_L1J75) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j260_a10_lcw_L1J75", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j260_a10_lcw_subjes_L1J75) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j260_a10_lcw_subjes_L1J75", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j260_a10t_lcw_jes_L1J75) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j260_a10t_lcw_jes_L1J75", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j400_a10_lcw_L1J100) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j400_a10_lcw_L1J100", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j420_a10_lcw_L1J100) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j420_a10_lcw_L1J100", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j420_a10t_lcw_jes_35smcINF_L1J100) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j420_a10t_lcw_jes_35smcINF_L1J100", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j420_a10t_lcw_jes_40smcINF_L1J100) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j420_a10t_lcw_jes_40smcINF_L1J100", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j440_a10_lcw_subjes_L1J100) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j440_a10_lcw_subjes_L1J100", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j440_a10t_lcw_jes_L1J100) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j440_a10t_lcw_jes_L1J100", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j460_a10_lcw_subjes_L1J100) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j460_a10_lcw_subjes_L1J100", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_j460_a10t_lcw_jes_L1J100) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_j460_a10t_lcw_jes_L1J100", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_2j330_a10t_lcw_jes_35smcINF_L1J100) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_2j330_a10t_lcw_jes_35smcINF_L1J100", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_2j330_a10t_lcw_jes_40smcINF_L1J100) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_2j330_a10t_lcw_jes_40smcINF_L1J100", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_mu20_iloose_L1MU15) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_mu20_iloose_L1MU15", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_mu24_ivarloose) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_mu24_ivarloose", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_mu24_ivarloose_L1MU15) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_mu24_ivarloose_L1MU15", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  if (eve_HLT_mu26_ivarmedium) m_HistSvc->FastFillTH1D("trigjet_pt_passHLT_mu26_ivarmedium", jet_pt->at(i_trigjet)/1e3, 100, 0., 1000., event_weight);
  m_HistSvc->FastFillTH1D("trigjet_pt_noHLTreq",this->jet_pt->at(i_trigjet)/1e3,100,0.,1000.,event_weight);

}
