/*
 * GbbTupleAna.cxx
 *
 *  Created on: Feb 18, 2016
 *      Author: rjacobs
 *  Modified by: aemerman
 */


#include "GbbTupleAna.h"
#include <TH2.h>
#include <iostream>
#include <TMath.h>
#include <TLorentzVector.h>
#include "TObjString.h"
#include "TTree.h"
#include "TChain.h"
#include "PathResolver/PathResolver.h"
#include "TSystem.h"

struct track {

  float d0;
  float sd0;
  float pt;
  float dr; //delta-R with associated track-jet

};

struct by_pt {
  bool operator()(track const &a, track const &b){
    return a.pt>b.pt;
  }

};

struct by_abs_sd0 {
  bool operator()(track const &a, track const &b){
    return TMath::Abs(a.sd0)>TMath::Abs(b.sd0);
  }
  
};


GbbTupleAna::GbbTupleAna() : TupleAna(),m_Debug(false),m_SumWeightTuple(0),m_nevtTuple(0) {
  // TODO Auto-generated constructor stub

  m_config=nullptr;
  m_HistogramService=new HistogramService();
}

GbbTupleAna::~GbbTupleAna() {
  // TODO Auto-generated destructor stub

  if(m_FlavFracCorrector) delete m_FlavFracCorrector;
  if(m_HistogramService) delete m_HistogramService;
  if(m_config) delete m_config;
}


void GbbTupleAna::ReadConfig(const TString &config_path){
 
  std::cout<<"=============================================="<<std::endl;

  TString m_config_path = config_path;
  //if ( !(gSystem->AccessPathName(m_config_path.Data())) )
  m_config_path = PathResolverFindCalibFile(m_config_path.Data());

  if (config_path == "") {
    std::cout << "Cannot find settings file " + config_path + "\n  also searched in " + m_config_path << std::endl;
    abort();
  } else std::cout << "Config file is set to: " << m_config_path << std::endl;

  TEnv* config = new TEnv("env");
  if (config->ReadFile(m_config_path.Data(),EEnvLevel(0)) == -1) {
    std::cout << "Could not read config file " << m_config_path.Data() << std::endl;
    abort();
  }
  //config->Print();
  
  m_Debug = config->GetValue("doDebug",false);
  std::cout<<"doDebug: "<<m_Debug<<std::endl;

  m_config = new GlobalConfig(PathResolverFindCalibFile("gbbCalibration/configs/GlobalConfig.cfg"));
  std::cout<<"Loaded GlobalConfig"<<std::endl;

  std::vector<TString> tempRunMode = SplitString(config->GetValue("RunMode",""),',');
  std::cout<<"RunModes: "<<config->GetValue("RunMode","")<<std::endl;
  m_RunMode = 0;
  for (TString mode : tempRunMode) {
    if (mode.Contains("FillTemplates") && !(m_RunMode & RunMode::FILL_TEMPLATES))
      m_RunMode += RunMode::FILL_TEMPLATES;
    else if (mode.Contains("FillTrkJetProperties") && !(m_RunMode & RunMode::FILL_TRKJET_PROPERTIES))
      m_RunMode += RunMode::FILL_TRKJET_PROPERTIES;
    else if (mode.Contains("FillFatJetProperties") && !(m_RunMode & RunMode::FILL_FATJET_PROPERTIES))
      m_RunMode += RunMode::FILL_FATJET_PROPERTIES;
    else if (mode.Contains("FillAdvancedProperties") && !(m_RunMode & RunMode::FILL_ADV_PROPERTIES))
      m_RunMode += RunMode::FILL_ADV_PROPERTIES;
    else if (mode.Contains("FillMCStats") && !(m_RunMode & RunMode::FILL_MC_STATS))
      m_RunMode += RunMode::FILL_MC_STATS;
    else if (mode.Contains("FillReweight") && !(m_RunMode & RunMode::FILL_REWEIGHT))
      m_RunMode += RunMode::FILL_REWEIGHT;
    else if (mode.Contains("FillPRW") && !(m_RunMode & RunMode::FILL_PRW))
      m_RunMode += RunMode::FILL_PRW;
    else if (mode.Contains("FillTriggerTurnOn") && !(m_RunMode & RunMode::FILL_TRIGGER))
      m_RunMode += RunMode::FILL_TRIGGER;
    else if (mode.Contains("ForFitOnly") && !(m_RunMode & RunMode::FOR_FIT_ONLY))
      m_RunMode += RunMode::FOR_FIT_ONLY;
    else {
      std::cout<<"Unrecognized RunMode: "<<mode.Data()<<". Ignoring"<<std::endl;
    }
  }
  if (m_Debug) std::cout<<"RunMode flag: "<<m_RunMode<<std::endl;
  
  m_doJetPtReweighting = config->GetValue("doJetPtEtaReweighting",true);
  std::cout<<"doJetPtEtaReweighting: "<<m_doJetPtReweighting<<std::endl;

  m_JetPtReweightFile = config->GetValue("JetPtEtaReweightingFile","");
  std::cout<<"JetPtEtaReweightingFile: "<<m_JetPtReweightFile<<std::endl;

  m_JetPtReweightFileInclusive = config->GetValue("JetPtEtaReweightingFileInclusive","");
  std::cout<<"JetPtEtaReweightingFileInclusive: "<<m_JetPtReweightFileInclusive<<std::endl;

  m_doTrackSmearing = config->GetValue("doTrackSmearing",false);
  std::cout<<"doTrackSmearing: "<<m_doTrackSmearing<<std::endl;

  m_doApplyBTaggingSF = config->GetValue("doApplyBTaggingSF",false);
  std::cout<<"doApplyBTaggingSF "<<m_doApplyBTaggingSF<<std::endl;

  m_doFlavFracCorrection = config->GetValue("doFlavFracCorrection",false);
  std::cout<<"doFlavFracCorrection: "<<m_doFlavFracCorrection<<std::endl;

  m_FlavFracCorrectorFile = config->GetValue("FlavFracCorrectorFile","./data/160714_FitResults_allsys.root");
  std::cout<<"FlavFracCorrectorFile: "<<m_FlavFracCorrectorFile<<std::endl;  

  m_doMergeDiTrkjetCat=config->GetValue("doMergeDiTrkjetCat",true);
  std::cout<<"m_doMergeDiTrkjetCat: "<< m_doMergeDiTrkjetCat<<std::endl;

  m_doEvenOddTemplates=config->GetValue("doEvenOddTemplates",false);
  std::cout<<"m_doEvemOddTemplates: "<< m_doEvenOddTemplates<<std::endl;

  m_trkjet_cat={"B","C","L","other"};
  m_ditrkjet_cat={"BB","BC","BL","CB","CC","CL","LB","LC","LL","other"};
  
  m_doRandomSplitting=config->GetValue("doRandomSplitting",false);
 std::cout<<"m_doRandomSplitting: "<< m_doRandomSplitting<<std::endl;

 m_doPostfitPtReweighting=config->GetValue("doPostfitPtReweighting",false);
 std::cout<<"m_doPostfitPtReweighting: "<<m_doPostfitPtReweighting<<std::endl;

 m_PostfitPtReweightingFile=config->GetValue("PostfitPtReweightingFile","DEFAULT.root");
 std::cout<<"m_PostfitPtReweightingFile: "<<m_PostfitPtReweightingFile<<std::endl;

  std::cout<<"=============================================="<<std::endl;
}

GbbTupleAna::GbbTupleAna(const std::vector<TString>& infiles, const TString& outfilename, const TString& configname) :
  TupleAna(),
  m_Debug(false),
  m_isMC(false),
  m_SumWeightTuple(0),
  m_nevtTuple(0)
{
  TH1::AddDirectory(0);

  //=========================================
  // Initialize 
  //=========================================
  ReadConfig(configname);
  m_HistogramService=new HistogramService();

  TH1D* metahist(nullptr), *metahist_tmp(nullptr);
  m_chains.emplace("Nom", new TChain("FlavourTagging_Nominal"));
  for (TString sys : m_config->GetSystematics()) {
    m_chains.emplace(sys, new TChain("FlavourTagging_"+sys));
  }
  //TChain* friendCh = nullptr;
  for (TString filename : infiles) {
    TFile* f = (TFile*)gROOT->GetListOfFiles()->FindObject(filename.Data());
    if (!f || !f->IsOpen()) f = new TFile(filename.Data(),"READ");
    if (!f || !f->IsOpen()) {
      std::cout<<"FATAL: could not open file "<<filename.Data()<<std::endl;
      abort();
    }
    // Read metadata
    if (metahist == nullptr) {
      f->GetObject("MetaData_EventCount",metahist);
    } else {
      f->GetObject("MetaData_EventCount",metahist_tmp);
      metahist->Add(metahist_tmp);
    }  

    //if (!treename.EqualTo("FlavourTagging_Nominal")) friendCh = new TChain("FlavourTagging_Nominal");
    //GetGeneratorName(filename.Data());
    for (auto iter : m_chains) {
      iter.second->Add(filename.Data());
    }
    //if (friendCh != nullptr) friendCh->Add(filename.Data());
  }
  //if (friendCh != nullptr) fChain->AddFriend(friendCh);
  if (!metahist) std::cout<<"FATAL: no metadata found!"<<std::endl; 

  // Copy metadata
  m_HistogramService->FastFillTH1D("Hist_BookKeeping",1,6,0.5,6.5,metahist->GetBinContent(1));
  ((TH1D*) m_HistogramService->GetHisto("Hist_BookKeeping")) -> GetXaxis() -> SetBinLabel(1, "nEvents AOD");
  
  m_HistogramService->FastFillTH1D("Hist_BookKeeping",2,6,0.5,6.5,metahist->GetBinContent(2));
  ((TH1D*) m_HistogramService->GetHisto("Hist_BookKeeping")) -> GetXaxis() -> SetBinLabel(2, "nEvents DAOD");
  
  ((TH1D*) m_HistogramService->GetHisto("Hist_BookKeeping")) -> GetXaxis() -> SetBinLabel(3, "nEvents TUPLE");
  
  m_HistogramService->FastFillTH1D("Hist_BookKeeping",4,6,0.5,6.5,metahist->GetBinContent(4));
  ((TH1D*) m_HistogramService->GetHisto("Hist_BookKeeping")) -> GetXaxis() -> SetBinLabel(4, "sumWeights AOD");

  m_HistogramService->FastFillTH1D("Hist_BookKeeping",5,6,0.5,6.5,metahist->GetBinContent(5));
  ((TH1D*) m_HistogramService->GetHisto("Hist_BookKeeping")) -> GetXaxis() -> SetBinLabel(5, "sumWeights DAOD");
  
  ((TH1D*) m_HistogramService->GetHisto("Hist_BookKeeping")) -> GetXaxis() -> SetBinLabel(6, "sumWeights TUPLE");

  m_Outputname = outfilename;
  
  std::cout<<"Generator is:"<<m_GeneratorName<<std::endl;

  //running on the nominal sample?
  //if (treename.Contains("Nominal")) {
  //  m_isNominal = true;
  //  m_SysVarName = "Nom";
  //} else {
  //  m_isNominal = false;
  //  m_SysVarName = treename(15,treename.Length());
  //}

  //if (m_isNominal) std::cout<<"Running on Nominal sample!"<<std::endl;
  //else std::cout<<"Running on Systematics sample!"<<std::endl;


  //=========================================
  //Initialize Correction and Smearing
  //=========================================
   
  //Flavour Fraction correction
  if(m_doFlavFracCorrection) m_FlavFracCorrector=new FlavourFracCorrector(m_FlavFracCorrectorFile); 
  else m_FlavFracCorrector=0;
  
  //=========================================
  //Initialize Reweighting histograms
  //=========================================
  
  //std::vector<TString> trig_slices={"HLT_j110","HLT_j150","HLT_j175","HLT_j200","HLT_j260","HLT_j300","HLT_j320","HLT_j360"};
  //std::vector<TString> trig_slices={"HLT_j150","HLT_j175","HLT_j260","HLT_j380"};
  std::vector<TString> trig_slices={"HLT_j380"};  

  if(m_doJetPtReweighting){
    if(m_GeneratorName.EqualTo("Pythia")){
      for(auto& elem : trig_slices) this->setReweightHisto(m_JetPtReweightFileInclusive,elem);
      std::cout<<"Do Pythia reweighting" << m_JetPtReweightFileInclusive << std::endl;
    }else if(m_GeneratorName.EqualTo("Herwig")){ //Herwig Reweighting
      for(auto& elem : trig_slices) this->setReweightHisto(m_JetPtReweightFile,elem);
      std::cout<<"Do Herwig reweighting"<<m_JetPtReweightFile<<std::endl;
    }
    else{
      std::cout<<"Unknown Generator: Do no reweighting"<<m_JetPtReweightFile<<std::endl;
    }
  }

  //========================================
  //Get PostfitPtReweightingHistogram
  //=======================================

  if(m_doPostfitPtReweighting){
    TFile* file=TFile::Open(m_PostfitPtReweightingFile.Data(),"READ");
    m_postfit_reweight_hist=(TH1D*)file->Get("scale_factors");
    m_postfit_reweight_hist->SetDirectory(0);
    file->Close();
   }

  //=========================================
  //Initialize Random Splitting
  //=========================================

  m_random = std::shared_ptr<TRandom3>(new TRandom3());
  m_random.get()->SetSeed(0);
   
  //Init(tree);
}


void GbbTupleAna::Finalize() {
  
  m_HistogramService->FastFillTH1D("Hist_BookKeeping",6,6,0.5,6.5,m_SumWeightTuple);
  m_HistogramService->FastFillTH1D("Hist_BookKeeping",3,6,0.5,6.5,m_nevtTuple);  
  m_HistogramService->WriteRootFile(m_Outputname);
  
}


void GbbTupleAna::Loop() {
  Loop("Nom");
  if (m_isMC) {
    for (TString sys : m_config->GetSystematics()) {
      Loop(sys);
    }
  }
}

void GbbTupleAna::Loop(const TString& sys) {

  Init(m_chains[sys.Data()]);
  m_isNominal = sys.EqualTo("Nom");
  m_SysVarName = sys;

  if (fChain == 0) return;

  Long64_t nentries = fChain->GetEntries();
  int real_entries = 0;

  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;

    if(!(jentry%1000)) std::cout<<"Processing event: "<<jentry<<" of "<<nentries<<std::endl;
    // if (Cut(ientry) < 0) continue;
    m_nevtTuple++;

    if (jentry == 0 && m_isNominal) m_isMC = this->eve_isMC;

    m_doFillMujet=false;
    if(m_random->Uniform()>=0.5) m_doFillMujet=true;

    Processgbb(jentry);
    real_entries++;
  }

  std::cout<<"real entries processed"<<real_entries<<std::endl;
}


bool GbbTupleAna::Processgbb(int i_evt){
  int icut=0;

  if(m_Debug) std::cout<<"processgbb(): Start!"<<std::endl;

  // Initialize eventFlag - all cuts are initialized to false by default                                             
  unsigned long int eventFlag = 0;

  bool IsLargeWeightEvent=false;

  updateFlag(eventFlag,GbbCuts::AllNtup,true);

  //=========================================
  //1.) Event cleaning & weight calculation
  //=========================================
 
  float total_evt_weight=(this->eve_mc_w*this->eve_pu_w);
  float original_evt_weight=total_evt_weight;
  //if(original_evt_weight>100) return false;

  icut++;
  m_HistogramService->FastFillTH1D(Form("CutFlow_%s",m_SysVarName.Data()),icut,15,0.5,15.5,total_evt_weight);

  //m_SumWeightTuple+=total_evt_weight;
  m_SumWeightTuple+=this->eve_pu_w;

  double mc_jet_ratio=1.;

  if (!this->isCleanEvt(total_evt_weight)) return false;

  if (this->hasBadJet()) return false;

  updateFlag(eventFlag,GbbCuts::EventCleaning,true);

  icut++;
  m_HistogramService->FastFillTH1D(Form("CutFlow_%s",m_SysVarName.Data()),icut,15,0.5,15.5,total_evt_weight);

  //=========================================                                                                          
  //2.) Trigger jet selection (leading AntiKt4EMTopo Jet)
  //=========================================

  std::vector<float> selected_jets_pt;
  std::vector<int> selected_jets_ind;  

  for(unsigned int i=0; i<this->jet_pt->size(); i++){
    
    if(this->passR4CaloJetCuts(i)){
      selected_jets_pt.push_back(this->jet_pt->at(i));
      selected_jets_ind.push_back(i);
    }
  }
  
  if(selected_jets_pt.size()==0){
    if(m_Debug) std::cout<<"processgbb(): No reco R=0.4 jets passing selection!"<<std::endl;
    return false;
  }

  if(m_Debug) std::cout<<"processgbb(): Get Trigger Jet!"<<std::endl;

  //int i_trigjet=selected_jets_ind.at(this->getLeadingObjIndex(&selected_jets_pt));
  int i_sublsmallRjet=-1;
  if(selected_jets_pt.size()>=2) i_sublsmallRjet=selected_jets_ind.at(this->getNthLeadingObjIndex(2,&selected_jets_pt));

  int i_trigjet = this->getTrigMatch();
  if (i_trigjet<0) return false; //no successful trigger match

  updateFlag(eventFlag,GbbCuts::TriggerJet,true);
  if(m_Debug) std::cout<<"processgbb(): Got Trigger Jet!"<<std::endl;
 
  icut++;
  m_HistogramService->FastFillTH1D(Form("CutFlow_%s",m_SysVarName.Data()),icut,15,0.5,15.5,total_evt_weight);

  //FILL TRIGGER TURNONHISTS
  if(m_RunMode & RunMode::FILL_TRIGGER) FillTriggerTurnOnHistograms(i_trigjet,total_evt_weight);


  //Trigger requirements, normalize lower trigger jet pt slice Lumi to HLT_j360 Lumi (see entry 12/04/16 in log book)
  //event must pass trigger which is fully efficient for respective trigger jet

  if(m_Debug) std::cout<<"processgbb(): Got Trigger Jet index!"<<i_trigjet<<std::endl;
  if(m_Debug) std::cout<<"processgbb(): Pt size!"<<this->jet_pt->size()<<std::endl;

  if(m_Debug){

    for(auto &pt : *(this->jet_pt)) std::cout<<"Jet pt: "<<pt<<std::endl;
    for(auto &pt : *(this->jet_ind)) std::cout<<"Jet ind: "<<pt<<std::endl;
    for(auto &pt : selected_jets_pt) std::cout<<"Sel. Jet pt: "<<pt<<std::endl;
    for(auto &pt : selected_jets_ind) std::cout<<"Sel. Jet ind: "<<pt<<std::endl;

  }
  
  float trjet_pt=this->jet_pt->at(i_trigjet);
  float prescale;
  TString trigger_passed;
  if (!passTrigger(trjet_pt, prescale, trigger_passed)) return false;

  //total_evt_weight*=prescale;

  if(m_Debug) std::cout<<"processgbb(): Passed trigger jet requirement"<<std::endl;

  icut++;
  m_HistogramService->FastFillTH1D(Form("CutFlow_%s",m_SysVarName.Data()),icut,15,0.5,15.5,total_evt_weight);

  //FILL REWEIGHT HISTOGRAMS
  if(m_isNominal && m_RunMode & RunMode::FILL_REWEIGHT){
    //FIXME: do we want to trigger on large-R jet?
    this->FillReweightInfo(i_trigjet,total_evt_weight,trigger_passed);
    //this->FillFatReweightInfo(largeRtrigpt,largeRtrigeta,total_evt_weight,trigger_passed);
  }

  //Fill Pileup reweighting histograms
  if(m_isNominal && m_RunMode & RunMode::FILL_PRW) m_HistogramService->FastFillTH1D("h"+m_SysVarName+"_evemu",this->eve_mu,12,0.,60.,total_evt_weight);

  // TRIGGER JET REWEIGHTING
  float trig_weight;
  if(m_doJetPtReweighting && !(m_RunMode & RunMode::FILL_REWEIGHT)  && m_reweightHistos[trigger_passed].get() && this->eve_isMC){
    trig_weight=this->getTrigJetWeight(i_trigjet,trigger_passed);
    //std::cout<<"Trigger weight is:"<<trig_weight<<std::endl;
    //float trig_weight=this->getTrigFatJetWeight(largeRtrigpt,largeRtrigeta,trigger_passed);

    if(TMath::Abs(trig_weight)<1e-10) trig_weight=1.; //avoid weights of zero //FIXME:shouldn't this set trig_weight to 1e-10?
    total_evt_weight*=trig_weight; //Temporary!!! put back in

  }

  // std::cout<<"prescale: "<<prescale<<" trig weight: "<<trig_weight<<std::endl;

   icut++;
   m_HistogramService->FastFillTH1D(Form("CutFlow_%s",m_SysVarName.Data()),icut,15,0.5,15.5,total_evt_weight);

  //=========================================                                                                                               
  //3.) Gbb candidate selection & definition                                                                                                              
  //=========================================

  if(m_Debug) std::cout<<"processgbb(): Constructing Gbb candidate..."<<std::endl;

  GbbCandidate gbbcand=constructGbbCandidate();

  if(gbbcand.fat_index==999){
    if(m_Debug) std::cout<<"processgbb(): No gbb candidate found"<<std::endl;
    return false;
  }
  icut++;
  m_HistogramService->FastFillTH1D(Form("CutFlow_%s",m_SysVarName.Data()),icut,15,0.5,15.5,total_evt_weight);

  if(m_Debug) std::cout<<"constructGbbCandidate(): Finish Gbb construction!"<<std::endl;
  TLorentzVector muojet_vec, nonmuojet_vec;
  muojet_vec.SetPtEtaPhiM( this->trkjet_pt->at(gbbcand.muojet_index)/1e3,
                           this->trkjet_eta->at(gbbcand.muojet_index),
                           this->trkjet_phi->at(gbbcand.muojet_index),
                           0.);
  nonmuojet_vec.SetPtEtaPhiM( this->trkjet_pt->at(gbbcand.nonmuojet_index)/1e3,
                              this->trkjet_eta->at(gbbcand.nonmuojet_index),
                              this->trkjet_phi->at(gbbcand.nonmuojet_index),
                              0.);
  m_HistogramService->FastFillTH1D("h_dRtrkjets",muojet_vec.DeltaR(nonmuojet_vec),100,0,1.0,total_evt_weight);
  float muojet_minVR = std::max( 0.02, std::min(0.4, 30.0e3 / this->trkjet_pt->at(gbbcand.muojet_index)) );
  float nonmuojet_minVR = std::max( 0.02, std::min(0.4, 30.0e3 / this->trkjet_pt->at(gbbcand.nonmuojet_index)) );

  if ( TMath::Log(muojet_vec.DeltaR(nonmuojet_vec)/std::min(muojet_minVR, nonmuojet_minVR)) < 0 ) {
    if(m_Debug) std::cout<<"constructGbbCandidate(): Removed event with  overlapping VR trackjets"<<std::endl;
    return false;
  }
  icut++;
  m_HistogramService->FastFillTH1D(Form("CutFlow_%s",m_SysVarName.Data()),icut,15,0.5,15.5,total_evt_weight);
  //if(gbbcand.muojet_index == gbbcand.nonmuojet_index) {
  //  if(m_Debug) std::cout<<"constructGbbCandidate(): Muon and non-muon jet have same index!"<<std::endl;
  //  return false;
  //}

  if(m_Debug){
    std::cout<<"processgbb(): Finished constructing Gbb candidate!"<<std::endl;
    std::cout<<"processgbb(): gbbcand.fat_index "<<gbbcand.fat_index<<std::endl;
    std::cout<<"processgbb(): gbbcand.muojet_index "<<gbbcand.nonmuojet_index<<std::endl;
    std::cout<<"processgbb(): gbbcand.nonmuojet_index "<<gbbcand.nonmuojet_index<<std::endl;
  }

  updateFlag(eventFlag, GbbCuts::GbbCandidate, true);

  //define flavour truth labels & pt labels
  int muojet_truth=this->trkjet_truth->at(gbbcand.muojet_index);
  int nonmuojet_truth=this->trkjet_truth->at(gbbcand.nonmuojet_index);

  if(m_Debug) std::cout<<"processgbb(): ditrkjet_cat.size "<<m_ditrkjet_cat.size()<<std::endl;
  TString dijet_name=this->eve_isMC ? m_ditrkjet_cat.at(this->getCategoryNumber(muojet_truth,nonmuojet_truth,m_doMergeDiTrkjetCat)) : TString("Data");

  double muojet_pt=this->trkjet_pt->at(gbbcand.muojet_index);
  double nonmuojet_pt=this->trkjet_pt->at(gbbcand.nonmuojet_index);
  
  if(m_Debug) std::cout<<"processgbb(): getting ditrk pt label"<<std::endl;
  TString ptlabel=m_config->GetDiTrkJetLabel(muojet_pt/1e3,nonmuojet_pt/1e3);

  if(m_Debug) std::cout<<"processgbb(): Got labels."<<std::endl;

  float gbbcandpt=this->fat_pt->at(gbbcand.fat_index);
  //  std::cout<<"trigjet pt is: "<<this->jet_pt->at(i_trigjet)<<" "<<gbbcandpt<<std::endl;

  //cut away region where jet bias is introduced
  if (!cutTriggerBias(gbbcandpt, trigger_passed)) return false;
  if(m_Debug) std::cout<<"processgbb(): Pass pt cuts."<<std::endl;
  icut++;
  m_HistogramService->FastFillTH1D(Form("CutFlow_%s",m_SysVarName.Data()),icut,15,0.5,15.5,total_evt_weight);


  /*  if(gbbcandpt>250e3 && gbbcandpt<=280e3 && trigger_passed.EqualTo("HLT_j150")){
  }else if(gbbcandpt>280e3 && gbbcandpt<=380e3 && trigger_passed.EqualTo("HLT_j175")){
  }else if(gbbcandpt>380e3 && gbbcandpt<=500e3 && trigger_passed.EqualTo("HLT_j260")){
  }else if(gbbcandpt>500e3 && trigger_passed.EqualTo("HLT_j380")){
  }else{
    std::cout<<"Event discarded: pt(R=0.4): "<<trjet_pt<<" pt(R=1.0): "<<gbbcandpt<<std::endl;  
    return false;
    }*/

  //FILL REWEIGHT HISTOGRAMS
  //if(m_isNominal && m_RunMode & RunMode::FILL_REWEIGHT) this->FillReweightInfo(i_trigjet,total_evt_weight,trigger_passed);


  //=========================================                                                                                               
  //4.) Tracks passing requirements for templates                                                                                                                
  //=========================================    

  trkjetSd0Info  muojet_sd0Info=this->getTrkjetAssocSd0Info(gbbcand.muojet_index,m_doTrackSmearing,"nominal",3);
  trkjetSd0Info  nonmuojet_sd0Info=this->getTrkjetAssocSd0Info(gbbcand.nonmuojet_index,m_doTrackSmearing,"nominal",3);
  float muojet_maxsd0 = muojet_sd0Info.meanSd0_ptSort;
  float nonmuojet_maxsd0 = nonmuojet_sd0Info.meanSd0_ptSort;

  if(TMath::Abs(muojet_maxsd0+99.)<1e-5 || TMath::Abs(nonmuojet_maxsd0+99.)<1e-5) return false; //associated tracks do not fulfil selection cuts  

  updateFlag(eventFlag,GbbCuts::GoodSd0Tracks,true);
  if(m_Debug) std::cout<<"processgbb(): Good sd0."<<std::endl;

  icut++;
  m_HistogramService->FastFillTH1D(Form("CutFlow_%s",m_SysVarName.Data()),icut,15,0.5,15.5,total_evt_weight);

  //=========================================                                                                                               
  //5.) Topo cut                                                                                                                
  //=========================================    

  TLorentzVector mujet,trigjet;

  mujet.SetPtEtaPhiE(this->trkjet_pt->at(gbbcand.muojet_index),this->trkjet_eta->at(gbbcand.muojet_index),this->trkjet_phi->at(gbbcand.muojet_index),this->trkjet_E->at(gbbcand.muojet_index));

  trigjet.SetPtEtaPhiE(this->jet_pt->at(i_trigjet),this->jet_eta->at(i_trigjet),this->jet_phi->at(i_trigjet),this->jet_E->at(i_trigjet));

  if(mujet.DeltaR(trigjet)<1.5) return false;

  updateFlag(eventFlag,GbbCuts::DRTrigJetMuJet,true);

  //DR 2-Track jets 
  TLorentzVector ditrkjet,nonmujet, fatjet;
  nonmujet.SetPtEtaPhiE(this->trkjet_pt->at(gbbcand.nonmuojet_index),this->trkjet_eta->at(gbbcand.nonmuojet_index),this->trkjet_phi->at(gbbcand.nonmuojet_index),this->trkjet_E->at(gbbcand.nonmuojet_index));

  ditrkjet=mujet+nonmujet;
  fatjet.SetPtEtaPhiE(this->fat_pt->at(gbbcand.fat_index),this->fat_eta->at(gbbcand.fat_index),this->fat_phi->at(gbbcand.fat_index),this->fat_E->at(gbbcand.fat_index));

  float DRditrkjetfatjet=fatjet.DeltaR(ditrkjet);

  if(m_Debug) std::cout<<"processgbb(): Pass topo cuts"<<std::endl;
  icut++;
  m_HistogramService->FastFillTH1D(Form("CutFlow_%s",m_SysVarName.Data()),icut,15,0.5,15.5,total_evt_weight);

  //=========================================
  //6.) Trigjet reweighting
  //=========================================  


  //  //If mode is: FillReweightHists Fill possible new reweighting histogram before event_weight is changed
  //if(m_isNominal && m_RunMode & RunMode::FILL_REWEIGHT) this->FillReweightInfo(i_trigjet,total_evt_weight,trigger_passed);


  /*  if(m_doPostfitPtReweighting && this->eve_isMC){
     double weight=1;
     if((this->fat_pt->at(gbbcand.fat_index)/1e3 < 250.) || (this->fat_pt->at(gbbcand.fat_index)/1e3 > 1000.)) weight=1.;
     else weight=m_postfit_reweight_hist->GetBinContent(m_postfit_reweight_hist->FindBin(this->fat_pt->at(gbbcand.fat_index)/1e3));
     //std::cout<<"postfit weight: "<<weight<<std::endl;

     total_evt_weight*= weight;
     }*/
    

  //}


  //=========================================
  //7.) B-Tagging
  //=========================================  

  //Benchmark Numbers Flavour Tagging Group (AntiKt2PV0TrackJets @ 70% eff)
  //https://twiki.cern.ch/twiki/bin/view/AtlasProtected/BTaggingBenchmarks#MV2c20_tagger_AntiKt2PV0TrackJet
  //  this->FillFtagInfo(&gbbcand,total_evt_weight_corr,ftagtag);
  //if(this->trkjet_MV2c20->at(gbbcand.muojet_index)<-0.3098 || this->trkjet_MV2c20->at(gbbcand.nonmuojet_index)<-0.3098) return false;
  //Moved to MV2c10 at 70% efficiency

  //1 b-tag
  if(this->trkjet_MV2c10->at(gbbcand.muojet_index)>0.6455 || this->trkjet_MV2c10->at(gbbcand.nonmuojet_index)>0.6455) updateFlag(eventFlag,GbbCuts::MuNonMu1Btag,true);  
  
  //2 b-tags
  if(this->trkjet_MV2c10->at(gbbcand.muojet_index)>0.6455 && this->trkjet_MV2c10->at(gbbcand.nonmuojet_index)>0.6455) updateFlag(eventFlag,GbbCuts::MuNonMu2Btags,true);

  //2Anti-b-tags
  if(this->trkjet_MV2c10->at(gbbcand.muojet_index)<0.6455 && this->trkjet_MV2c10->at(gbbcand.nonmuojet_index)<0.6455) updateFlag(eventFlag,GbbCuts::MuNonMuAnti2Btags,true);

  //2Anti-b-tags
  if(this->trkjet_MV2c10->at(gbbcand.muojet_index)<0.6455 || this->trkjet_MV2c10->at(gbbcand.nonmuojet_index)<0.6455) updateFlag(eventFlag,GbbCuts::MuNonMuUntagged,true);
  

  //=========================================
  //8.) Fill histograms
  //=========================================  
  //DEFINE cuts


  std::vector<unsigned int> CutsNoBtag={GbbCuts::AllNtup,GbbCuts::EventCleaning,GbbCuts::TriggerJet,GbbCuts::GbbCandidate,GbbCuts::GoodSd0Tracks,GbbCuts::DRTrigJetMuJet};
  std::vector<unsigned int> CutsWith1Btag={GbbCuts::AllNtup,GbbCuts::EventCleaning,GbbCuts::TriggerJet,GbbCuts::GbbCandidate,GbbCuts::GoodSd0Tracks,GbbCuts::DRTrigJetMuJet,GbbCuts::MuNonMu1Btag};  
  std::vector<unsigned int> CutsWith2Btags={GbbCuts::AllNtup,GbbCuts::EventCleaning,GbbCuts::TriggerJet,GbbCuts::GbbCandidate,GbbCuts::GoodSd0Tracks,GbbCuts::DRTrigJetMuJet,GbbCuts::MuNonMu2Btags};
  std::vector<unsigned int> CutsWithAnti2Btags={GbbCuts::AllNtup,GbbCuts::EventCleaning,GbbCuts::TriggerJet,GbbCuts::GbbCandidate,GbbCuts::GoodSd0Tracks,GbbCuts::DRTrigJetMuJet,GbbCuts::MuNonMuAnti2Btags};
  std::vector<unsigned int> CutsWithUntag={GbbCuts::AllNtup,GbbCuts::EventCleaning,GbbCuts::TriggerJet,GbbCuts::GbbCandidate,GbbCuts::GoodSd0Tracks,GbbCuts::DRTrigJetMuJet,GbbCuts::MuNonMuUntagged};


  //1.) Before Fit and b-tagging
  if(passSpecificCuts(eventFlag, CutsNoBtag)){
    
    m_HistogramService->FastFillTH1D("Hist_MCStatsUnc",1,1,0.5,1.5,total_evt_weight);
    
    if(m_RunMode & RunMode::FILL_TEMPLATES) FillTemplates(&gbbcand,total_evt_weight);
    if(m_RunMode & RunMode::FILL_TRKJET_PROPERTIES) FillTrackJetProperties(&gbbcand,total_evt_weight);
    if(m_RunMode & RunMode::FILL_FATJET_PROPERTIES) FillFatJetProperties(&gbbcand,total_evt_weight);
    if(m_RunMode & RunMode::FILL_ADV_PROPERTIES) FillAdvancedProperties(&gbbcand,i_trigjet,total_evt_weight);
    if(m_RunMode & RunMode::FILL_MC_STATS) FillMCStatsInfo(&gbbcand);


    //for crosscheck: Fill Even and Odd Templates
    if(this->eve_isMC && m_doEvenOddTemplates && m_RunMode & RunMode::FILL_TEMPLATES){
      if(!(i_evt%2)) this->FillTemplates(&gbbcand,total_evt_weight,"EVEN");
      if(!(i_evt%2)) this->FillTemplates(&gbbcand,total_evt_weight,"ODD");      
    }
    
    m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,dijet_name,ptlabel,"trjpt",""),
     trjet_pt/1e3,250,0.,1000.,total_evt_weight);
    m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,dijet_name,ptlabel,"DRditrkjetfatjet",""),
     DRditrkjetfatjet,250,0.,0.5,total_evt_weight);
    if(m_isNominal) m_HistogramService->FastFillTH2D("h_DRditrkjetfatjetVSfjpt",DRditrkjetfatjet,fatjet.Pt()/1e3,25,0.,0.5,50,0.,1000.,total_evt_weight);

    m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,dijet_name,ptlabel,"trjptgbbcandratio",""),
     trjet_pt/fatjet.Pt(),50,0.,10.,total_evt_weight);

    //m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,dijet_name,ptlabel,"trjptfjptratio",""),
    // ptratio,50,-1.,1.,total_evt_weight);

    if(m_isNominal){

      m_HistogramService->FastFillTH2D("h_2DtrjptVsfjpt",trjet_pt/1e3,fatjet.Pt()/1e3,100,0.,1000.,100,0.,1000.,total_evt_weight);
      m_HistogramService->FastFillTH2D( makePlotName("Nom",dijet_name,"Incl","2DmjptVsnmjpt",""),
        muojet_pt/1e3,nonmuojet_pt/1e3,100,0.,1000.,100,0.,1000.,total_evt_weight);
    }    


    TLorentzVector smallRJet;
    if(i_sublsmallRjet>=0){
      smallRJet.SetPtEtaPhiE(this->jet_pt->at(i_sublsmallRjet),this->jet_eta->at(i_sublsmallRjet),this->jet_phi->at(i_sublsmallRjet),this->jet_E->at(i_sublsmallRjet));
      if(smallRJet.DeltaR(trigjet)>1.5) {
        m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,dijet_name,ptlabel,"slR4jpt",""),
         trjet_pt/1e3,250,0.,1000.,total_evt_weight);
      }
    }

    m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,dijet_name,ptlabel,"evemu",""),
     this->eve_mu,60,0.,60.,total_evt_weight);
    
    m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,dijet_name,ptlabel,"srjN",""),
     this->jet_pt->size(),10,0.,10.,total_evt_weight);

    for(int i=0; i<this->jet_pt->size(); i++){
      m_HistogramService->FastFillTH1D( makePlotName(m_SysVarName,dijet_name,ptlabel,"allsrjpt",""),
       this->jet_pt->at(i)/1e3,250,0.,1000.,total_evt_weight);
    }

  }
  
  //2.) Before Fit and after b-tagging  
  float btag_SF_nom=1., btag_SF_up=1., btag_SF_down=1.; 
  if(this->eve_isMC && m_doApplyBTaggingSF) this->getBtagSFWeights(btag_SF_nom,btag_SF_up,btag_SF_down);

  if(passSpecificCuts(eventFlag, CutsWith2Btags)){  
    //std::cout<<"Filling post-tag plots for"<<m_SysVarName<<"with btag_SF :"<<btag_SF_nom<<std::endl;
    
    if(m_RunMode & RunMode::FILL_TEMPLATES && !(m_RunMode & RunMode::FOR_FIT_ONLY)) this->FillTemplates(&gbbcand,total_evt_weight*btag_SF_nom,"PREFITPOSTTAG");
    if(m_RunMode & RunMode::FILL_TRKJET_PROPERTIES) this->FillTrackJetProperties(&gbbcand,total_evt_weight*btag_SF_nom,"PREFITPOSTTAG");
    if(m_RunMode & RunMode::FILL_FATJET_PROPERTIES) this->FillFatJetProperties(&gbbcand,total_evt_weight*btag_SF_nom,"PREFITPOSTTAG");
    if(m_RunMode & RunMode::FILL_MC_STATS) FillMCStatsInfo(&gbbcand,"PREFITPOSTTAG");

    if(m_isNominal){
      
      if(m_RunMode & RunMode::FILL_TEMPLATES && !(m_RunMode & RunMode::FOR_FIT_ONLY)) this->FillTemplates(&gbbcand,total_evt_weight*btag_SF_up,"PREFITPOSTTAG_BTAGUP");
      if(m_RunMode & RunMode::FILL_TRKJET_PROPERTIES) this->FillTrackJetProperties(&gbbcand,total_evt_weight*btag_SF_up,"PREFITPOSTTAG_BTAGUP");
      if(m_RunMode & RunMode::FILL_FATJET_PROPERTIES) this->FillFatJetProperties(&gbbcand,total_evt_weight*btag_SF_up,"PREFITPOSTTAG_BTAGUP");

      if(m_RunMode & RunMode::FILL_TEMPLATES && !(m_RunMode & RunMode::FOR_FIT_ONLY)) this->FillTemplates(&gbbcand,total_evt_weight*btag_SF_down,"PREFITPOSTTAG_BTAGDOWN");
      if(m_RunMode & RunMode::FILL_TRKJET_PROPERTIES) this->FillTrackJetProperties(&gbbcand,total_evt_weight*btag_SF_down,"PREFITPOSTTAG_BTAGDOWN");
      if(m_RunMode & RunMode::FILL_FATJET_PROPERTIES) this->FillFatJetProperties(&gbbcand,total_evt_weight*btag_SF_down,"PREFITPOSTTAG_BTAGDOWN");

      
    }

    m_HistogramService->FastFillTH1D( makePlotName("Nom",dijet_name,ptlabel,"trjpt","PREFITPOSTTAG"),
     trjet_pt/1e3,250,0.,1000.,total_evt_weight);
    m_HistogramService->FastFillTH1D( makePlotName("Nom",dijet_name,ptlabel,"DRditrkjetfatjet","PREFITPOSTTAG"),
     DRditrkjetfatjet,250,0.,0.5,total_evt_weight);
    


  }


  //3a)Fill fat jet info for AntiBtag
  /*if(passSpecificCuts(eventFlag, CutsWithAnti2Btags)){  
  
    if(m_RunMode & RunMode::FILL_FATJET_PROPERTIES) this->FillFatJetProperties(&gbbcand,total_evt_weight*btag_SF_nom,"PREFITANTITAG");

    }*/

  //3b) Fill untagged info, for MC uncertainty calculation
  if(m_isNominal && passSpecificCuts(eventFlag, CutsWithUntag)){
    if(m_RunMode & RunMode::FILL_TRKJET_PROPERTIES) this->FillTrackJetProperties(&gbbcand,total_evt_weight*btag_SF_nom,"PREFITUNTAG");
    if(m_RunMode & RunMode::FILL_FATJET_PROPERTIES) this->FillFatJetProperties(&gbbcand,total_evt_weight*btag_SF_nom,"PREFITUNTAG");

  }

  //4.) Histograms for Fit-Error calculation (pre- & post-b-tagging in fit pt-bins, for nominal only)

  if(m_doFlavFracCorrection){
    
    if(m_isNominal && this->eve_isMC && passSpecificCuts(eventFlag, CutsNoBtag)){
      this->FillTrackJetProperties(&gbbcand,total_evt_weight,ptlabel+"_FITERR");
      this->FillFatJetProperties(&gbbcand,total_evt_weight,ptlabel+"_FITERR");
    }

    if(m_isNominal && this->eve_isMC && passSpecificCuts(eventFlag, CutsWith2Btags)){
      this->FillTrackJetProperties(&gbbcand,total_evt_weight,"PREFITPOSTTAG_"+ptlabel+"_FITERR");
      this->FillFatJetProperties(&gbbcand,total_evt_weight,"PREFITPOSTTAG_"+ptlabel+"_FITERR");
    }
    
  }
  
  //3.) After Fit and before/after b-tagging
  double fffit_fact=1.;
  
  if(m_doFlavFracCorrection && this->eve_isMC){
    
    if(m_isNominal)fffit_fact=m_FlavFracCorrector->GetCorrectionFactorNom(m_ditrkjet_cat[this->getCategoryNumber(muojet_truth,nonmuojet_truth,m_doMergeDiTrkjetCat)], muojet_pt/1e3, nonmuojet_pt/1e3);
    else fffit_fact=m_FlavFracCorrector->GetCorrectionFactorSys(m_ditrkjet_cat[this->getCategoryNumber(muojet_truth,nonmuojet_truth,m_doMergeDiTrkjetCat)], muojet_pt/1e3, nonmuojet_pt/1e3,m_SysVarName);
    
    if(passSpecificCuts(eventFlag, CutsNoBtag)){
      this->FillTemplates(&gbbcand,total_evt_weight*fffit_fact,"POSTFIT");
      this->FillTrackJetProperties(&gbbcand,total_evt_weight*fffit_fact,"POSTFIT");
      this->FillFatJetProperties(&gbbcand,total_evt_weight*fffit_fact,"POSTFIT");
    }

    if(passSpecificCuts(eventFlag, CutsWith2Btags)){
      this->FillTemplates(&gbbcand,total_evt_weight*fffit_fact*btag_SF_nom,"POSTFITPOSTTAG");
      this->FillTrackJetProperties(&gbbcand,total_evt_weight*fffit_fact*btag_SF_nom,"POSTFITPOSTTAG");
      this->FillFatJetProperties(&gbbcand,total_evt_weight*fffit_fact*btag_SF_nom,"POSTFITPOSTTAG");


      if(m_isNominal){

  //fill histograms with b-tagging systematics applied
  this->FillTrackJetProperties(&gbbcand,total_evt_weight*btag_SF_up,"POSTFITPOSTTAG_BTAGUP");
  this->FillFatJetProperties(&gbbcand,total_evt_weight*btag_SF_up,"POSTFITPOSTTAG_BTAGUP");
  
  this->FillTrackJetProperties(&gbbcand,total_evt_weight*btag_SF_down,"POSTFITPOSTTAG_BTAGDOWN");
  this->FillFatJetProperties(&gbbcand,total_evt_weight*btag_SF_down,"POSTFITPOSTTAG_BTAGDOWN");
      }
      
    }

  }
  
  /*  if(total_evt_weight > 50){
    std::cout<<"original weight: "<<original_evt_weight<<" total_evt_weight: "<<total_evt_weight<<std::endl;
    std::cout<<"mc weight: "<<this->eve_mc_w<<std::endl;
    std::cout<<"PU weight: "<<this->eve_pu_w<<std::endl;
    } */


  return true;
}

//FIXME: find better way to do this
void GbbTupleAna::GetGeneratorName(TString url){
  if(url.Contains("3610") || url.Contains("42700") || url.Contains("42710")) m_GeneratorName="Pythia";
  else if (url.Contains("4260") || url.Contains("42711") ) m_GeneratorName="Herwig";
  else m_GeneratorName="Unknown";
}

void GbbTupleAna::setReweightHisto(TString filename, TString trigger_passed){
  
  TH2D *hist;

  TString hist_name="reweight_trigjet_pt_eta_"+trigger_passed;
  //TString hist_name="reweight_trigfatjet_pt_eta_"+trigger_passed;
  
  std::cout<<"Looking for hist: "<<hist_name<<std::endl;

  TFile* file=TFile::Open(filename.Data(),"READ");
  file->GetObject(hist_name,hist);
  hist->SetDirectory(0);
  m_reweightHistos[trigger_passed]=std::shared_ptr<TH2D>(new TH2D(*hist));
  m_reweightHistos[trigger_passed].get()->SetDirectory(0);

  if(m_Debug) std::cout<<"GbbTupleAna::setReweightHisto: Found reweighting histogram "<<m_reweightHistos[trigger_passed].get()->GetName()<<std::endl;
  
  file->Close();
}


bool GbbTupleAna::isLargeWeightEvent(int DSID, float evt_weight, float max_evt_weight){

  float samp_weight=0.;
  
  if(DSID==361020)samp_weight=81651.8; //TODO change this
  else if(DSID==361021)samp_weight=81651.8;
  else if(DSID==361022)samp_weight=1088.42;
  //else if(DSID==361023)samp_weight=11.85;
  else if(DSID==361023)samp_weight=3.19418;
  //else if(DSID==361024)samp_weight=0.19;
  if(DSID==361024)samp_weight=0.050298;
  //else if(DSID==361025)samp_weight=0.0061;
  else if(DSID==361025)samp_weight=0.00156614;
  //else if(DSID==361026)samp_weight=0.00036;
  if(DSID==361026)samp_weight=0.00038026;
  else if(DSID==361027)samp_weight=1.08e-5;
  //if(DSID==361026)samp_weight=1.06101e-5;
  else if(DSID==361028)samp_weight=1.09e-5;
  else if(DSID==361029)samp_weight=3.9e-7;
  else if(DSID==361030)samp_weight=1.05e-8;
  else if(DSID==361031)samp_weight=1.6e-10;
        else if(DSID==361032)samp_weight=6.3e-13;

  if((samp_weight*evt_weight)<max_evt_weight) return false;

  return true;
}

unsigned int GbbTupleAna::getLeadingObjIndex(std::vector<float> *quantity){

  return std::distance(quantity->begin(), std::max_element(quantity->begin(),quantity->end()));

}

unsigned int GbbTupleAna::getNthLeadingObjIndex(unsigned int n, std::vector<float> *quantity){
  std::vector<float> tmp = *quantity;
  std::nth_element(tmp.begin(), tmp.begin()+(n-1), tmp.end(), std::greater<int>());

  return std::distance(quantity->begin(), std::find(quantity->begin(),quantity->end(),tmp[n-1]));

}

int GbbTupleAna::getAssocObjIndex(std::vector<int>* part_ind, int assoc_index){
  
  std::vector<int>::iterator it;
  it=find(part_ind->begin(),part_ind->end(),assoc_index);

  if(it==part_ind->end()) return -1;

  return std::distance(part_ind->begin(),it);

}

GbbCandidate GbbTupleAna::constructGbbCandidate(){

  GbbCandidate gbbcand;
  gbbcand.fat_pt=0.;
  gbbcand.muojet_index=999;
  gbbcand.nonmuojet_index=999;
  gbbcand.fat_index=999;


  unsigned int muonjet_index=999, nonmuonjet_index=999;

  for(unsigned int i_jet=0; i_jet<this->fat_pt->size(); i_jet++){
    
    if(!passR10CaloJetCuts(i_jet)) continue;
    
    if(this->fat_assocTrkjet_ind->at(i_jet).size()<2){
      //if(m_Debug) std::cout<<"constructGbbCandidate(): Fat Jet has less than 2 associated track jets"<<std::endl;
      continue; 
    }

    int assocTJ_ind=-1;
    
    std::vector<unsigned int> nonmuon_cand_index;
    
    muonjet_index=999;
    nonmuonjet_index=999;
  
    for(unsigned int i=0; i<this->fat_assocTrkjet_ind->at(i_jet).size(); i++){
      
      assocTJ_ind=this->getAssocObjIndex(this->trkjet_ind,fat_assocTrkjet_ind->at(i_jet).at(i)); //find position of associated track jet in ntup vector (add. selection applied after association)

      if(assocTJ_ind<0){
        if(m_Debug) std::cout<<"constructGbbCandidate(): Track Jet did not pass cuts!"<<std::endl;
        continue; //track jet didn't pass selection in CxAODFramework (Tuple Maker)
      }
     
    
      //Track jet selection cuts already applied in CxAODFramework (Tuple Maker)
      if(!(this->passR2TrackJetCuts(assocTJ_ind))) continue;
      // else continue;

      //if(muonjet_index != 999){ //next leading pt track jet is non-muon jet
      //nonmuonjet_index=assocTJ_ind;
      //  break;
      //}
  
      int n_assoc_selmuon=0;
      bool hasTruthMuon=false;

      for(unsigned int j=0; j<this->trkjet_assocMuon_n->at(assocTJ_ind); j++){

        if(this->passMuonSelection(this->trkjet_assocMuon_index->at(assocTJ_ind).at(j))){
          n_assoc_selmuon++;
          if(this->muo_hasTruth->at(this->trkjet_assocMuon_index->at(assocTJ_ind).at(j))) hasTruthMuon=true;
          //TLorentzVector trkjet(this->trkjet_pt->at(assocTJ_ind),this->trkjet_eta->at(assocTJ_ind),this->trkjet_phi->at(assocTJ_ind),0);
          //TLorentzVector muon(this->muo_pt->at(this->trkjet_assocMuon_index->at(assocTJ_ind).at(j)),this->muo_eta->at(this->trkjet_assocMuon_index->at(assocTJ_ind).at(j)),this->muo_phi->at(this->trkjet_assocMuon_index->at(assocTJ_ind).at(j)),0);
          //std::cout<<"Delta R between muon and trackjet is: "<<trkjet.DeltaR(muon)<<std::endl;


        }
        
      }
      
      if(n_assoc_selmuon && muonjet_index==999){ //first muon associated track jet (assoc trkjets are sorted by pt)
        muonjet_index=assocTJ_ind;        
      }else{
        //if(m_Debug && !(this->trkjet_assocMuon_n->at(assocTJ_ind))) std::cout<<"constructGbbCandidate(): No Muon associated to track jet!"<<std::endl;
        if(m_Debug && this->trkjet_assocMuon_n->at(assocTJ_ind)) std::cout<<"constructGbbCandidate(): Muon associated to track jet did not pass cuts!"<<std::endl;

        nonmuon_cand_index.push_back(assocTJ_ind);
      }
      

    }

    //nonmuon-jet: leading associated track jet that is not the muon jet
    if(nonmuon_cand_index.size()) nonmuonjet_index=nonmuon_cand_index[0];
    
    //check if leading two associated track jets are used
    bool leading_2trackjets=false;
    if(nonmuon_cand_index.size() && muonjet_index!=999){

      if(nonmuon_cand_index.size()>1){
        leading_2trackjets=(muonjet_index<nonmuonjet_index || muonjet_index<nonmuon_cand_index[1] );
      } else leading_2trackjets=true;

    }

    if(muonjet_index != 999 && nonmuonjet_index != 999 && this->fat_pt->at(i_jet)>gbbcand.fat_pt){ //get leading pt Gbb candidate
      gbbcand.fat_pt=this->fat_pt->at(i_jet);
      gbbcand.muojet_index=muonjet_index;
      gbbcand.nonmuojet_index=nonmuonjet_index;
      gbbcand.fat_index=i_jet;
      gbbcand.hasleading2trackjets=leading_2trackjets;
    }
  }

  if(muonjet_index == 999 && m_Debug) std::cout<<"constructGbbCandidate(): Failed to find muon jet"<<std::endl;
  else if(nonmuonjet_index == 999 && m_Debug) std::cout<<"constructGbbCandidate(): Failed to find nonmuon jet"<<std::endl;
 
  if(nonmuonjet_index == 999 && muonjet_index == 999 && m_Debug)  std::cout<<"constructGbbCandidate(): Failed to find any subjet or fat jet"<<std::endl;

  return gbbcand; 
}

GbbCandidate GbbTupleAna::constructGbbCandidateAlternative(){

  GbbCandidate gbbcand;
  gbbcand.fat_pt=0.;
  gbbcand.muojet_index=999;
  gbbcand.nonmuojet_index=999;
  gbbcand.fat_index=999;


  unsigned int muonjet_index=999, nonmuonjet_index=999;

  for(unsigned int i_jet=0; i_jet<this->fat_pt->size(); i_jet++){
    
    if(!passR10CaloJetCuts(i_jet)) continue;
    
    if(this->fat_assocTrkjet_ind->at(i_jet).size()<2){
      //if(m_Debug) std::cout<<"constructGbbCandidate(): Fat Jet has less than 2 associated track jets"<<std::endl;
      continue; 
    }
    int assocTJ_ind=-1;
    
    std::vector<unsigned int> nonmuon_cand_index;
    
    muonjet_index=999;
    nonmuonjet_index=999;
  
    for(unsigned int i=0; i<this->fat_assocTrkjet_ind->at(i_jet).size(); i++){
      
      assocTJ_ind=this->getAssocObjIndex(this->trkjet_ind,fat_assocTrkjet_ind->at(i_jet).at(i)); //find position of associated track jet in ntup vector (add. selection applied after association)

      if(assocTJ_ind<0){
        if(m_Debug) std::cout<<"constructGbbCandidate(): Track Jet did not pass cuts!"<<std::endl;
        continue; //track jet didn't pass selection in CxAODFramework (Tuple Maker)
      }
     
    
      //Track jet selection cuts already applied in CxAODFramework (Tuple Maker)
      if(!(this->passR2TrackJetCuts(assocTJ_ind))) continue;
      // else continue;

      
      if(muonjet_index==999){ //first muon associated track jet (assoc trkjets are sorted by pt)
        muonjet_index=assocTJ_ind;        
      }else{
        nonmuon_cand_index.push_back(assocTJ_ind);
      }
      

    }

    //nonmuon-jet: leading associated track jet that is not the muon jet
    if(nonmuon_cand_index.size()) nonmuonjet_index=nonmuon_cand_index[0];
  
    //check if leading two associated track jets are used
    bool leading_2trackjets=false;
    if(nonmuon_cand_index.size() && muonjet_index!=999){

      if(nonmuon_cand_index.size()>1){
  leading_2trackjets=(muonjet_index<nonmuonjet_index || muonjet_index<nonmuon_cand_index[1] );

      }else leading_2trackjets=true;


    }

    if(muonjet_index != 999 && nonmuonjet_index != 999 && this->fat_pt->at(i_jet)>gbbcand.fat_pt){ //get leading pt Gbb candidate
      gbbcand.fat_pt=this->fat_pt->at(i_jet);
      gbbcand.muojet_index=muonjet_index;
      gbbcand.nonmuojet_index=nonmuonjet_index;
      gbbcand.fat_index=i_jet;
      gbbcand.hasleading2trackjets=leading_2trackjets;
    }

   

  }

  if(muonjet_index == 999 && m_Debug) std::cout<<"constructGbbCandidate(): Failed to find muon jet"<<std::endl;
  else if(nonmuonjet_index == 999 && m_Debug) std::cout<<"constructGbbCandidate(): Failed to find nonmuon jet"<<std::endl;
 
  if(nonmuonjet_index == 999 && muonjet_index == 999 && m_Debug)  std::cout<<"constructGbbCandidate(): Failed to find any subjet or fat jet"<<std::endl;

  return gbbcand; 
}


int GbbTupleAna::getTruthType(int label){

  label=TMath::Abs(label);
  
  if(label==0) return 2;
  else if(label==4) return 1;
  else if(label==5) return 0;

  return 3;
}

float GbbTupleAna::getTrigJetWeight(int i_trig_jet,TString trigger_passed){

  return m_reweightHistos[trigger_passed].get()->GetBinContent(m_reweightHistos[trigger_passed].get()->FindBin(this->jet_pt->at(i_trig_jet)/1e3,this->jet_eta->at(i_trig_jet)));

}

float GbbTupleAna::getTrigFatJetWeight(float trigfjpt, float trigfjeta,TString trigger_passed){

  return m_reweightHistos[trigger_passed].get()->GetBinContent(m_reweightHistos[trigger_passed].get()->FindBin(trigfjpt/1e3,trigfjeta));

}

int GbbTupleAna::getCategoryNumber(int muo_truth, int nonmuo_truth, bool doMerged){
  
  muo_truth=TMath::Abs(muo_truth);
  nonmuo_truth=TMath::Abs(nonmuo_truth);

  if(muo_truth==5){

    if(nonmuo_truth==5) return 0; 
    else if(nonmuo_truth==4 && !doMerged) return 1;
    else if(nonmuo_truth==4 && doMerged) return 2; //merge BC with BL
    else if (nonmuo_truth==0) return 2;

  }else if(muo_truth==4){

    if(nonmuo_truth==5 && !doMerged) return 3;
    else if(nonmuo_truth==5 && doMerged) return 5; //merge CB with CL
    else if(nonmuo_truth==4) return 4;
    else if (nonmuo_truth==0) return 5;

  }else if(muo_truth==0){

    if(nonmuo_truth==5 && !doMerged) return 6;
    else if(nonmuo_truth==5 && doMerged) return 8; //merge LB with LL
    else if(nonmuo_truth==4 && !doMerged) return 7;
    else if(nonmuo_truth==4 && doMerged) return 8; //merge LC with LL
    else if (nonmuo_truth==0) return 8;

  }
  
  return 9;
  
}

trkjetSd0Info GbbTupleAna::getTrkjetAssocSd0Info(unsigned int i_jet, bool doSmeared, TString sys, int n){

  int tracks_passed=0;
  TLorentzVector jet,trk;
  jet.SetPtEtaPhiM(this->trkjet_pt->at(i_jet),this->trkjet_eta->at(i_jet),this->trkjet_phi->at(i_jet),0);
  float px, py,pz;
  std::vector<track> tracks;
  trkjetSd0Info ret = {-99.,-99.,-99.,-99.,-99.,-99.,-99.,-99.,n};

  for(unsigned int i_trk=0; i_trk<this->trkjet_assocTrk_pt->at(i_jet).size(); i_trk++){
    //if(!this->passAssocTrkSelection(i_trk,i_jet)) continue;

    tracks_passed++;
    track tr;

    tr.d0 = getd0(i_trk,i_jet,doSmeared,sys);
    tr.sd0 = getSd0(i_trk,i_jet,doSmeared,sys);
    tr.pt=this->trkjet_assocTrk_pt->at(i_jet).at(i_trk);

    trk.SetPtEtaPhiM(this->trkjet_assocTrk_pt->at(i_jet).at(i_trk),this->trkjet_assocTrk_eta->at(i_jet).at(i_trk),this->trkjet_assocTrk_phi->at(i_jet).at(i_trk),0);
    tr.dr=jet.DeltaR(trk);

    tracks.push_back(tr);
  } // End loop over tracks

  if (tracks.size() < 3) return ret;
  std::sort(tracks.begin(),tracks.end(),by_abs_sd0());
  ret.maxSd0 = tracks.at(0).sd0;
  ret.maxSd0_dR = tracks.at(0).dr;
  ret.subSd0 = tracks.at(1).sd0;
  ret.subSd0_dR = tracks.at(1).dr;
  ret.thirdSd0 = tracks.at(2).sd0;
  ret.thirdSd0_dR = tracks.at(2).dr;

  if (tracks.size() < n) return ret;

  float sum=0.;
  for(int i=0; i<n; i++){
    //std::cout<<"track "<<i<<": pT"<<tracks.at(i).pt<<std::endl;
    float d0=tracks.at(i).d0;
    float sd0=tracks.at(i).sd0;
    //sum+=sd0<0 ? -1.*TMath::Abs(d0) : TMath::Abs(d0);
    sum+=sd0;
  }
  ret.meanSd0_sd0Sort = sum/n;

  std::sort(tracks.begin(),tracks.end(),by_pt());
  sum=0.;
  for(int i=0; i<n; i++){
    //std::cout<<"track "<<i<<": pT"<<tracks.at(i).pt<<std::endl;
    float d0=tracks.at(i).d0;
    float sd0=tracks.at(i).sd0;
    //sum+=sd0<0 ? -1.*TMath::Abs(d0) : TMath::Abs(d0);
    sum+=sd0;
  }
  ret.meanSd0_ptSort = sum/n;

  return ret;

}

float GbbTupleAna::getd0(unsigned int i_trk, unsigned int i_jet, bool doSmeared, TString sys){

  float d0=this->trkjet_assocTrk_d0->at(i_jet).at(i_trk);
  //std::cout<<"d0 is: "<<d0<<std::endl;
  if (doSmeared && this->eve_isMC) {
    if(sys.EqualTo("nominal")) d0=this->trkjet_assocTrk_d0_smear->at(i_jet).at(i_trk);
    else if(sys.EqualTo("up")) d0=this->trkjet_assocTrk_d0_smear_up->at(i_jet).at(i_trk);
    else if(sys.EqualTo("down")) d0=this->trkjet_assocTrk_d0_smear_down->at(i_jet).at(i_trk);
    else std::cout<<"ERROR: You have to specify if you want nominal or sys smeared Sd0!"<<std::endl;
  }
  //std::cout<<"smeared d0 is: "<<d0<<std::endl;
  return d0;
}

float GbbTupleAna::getSd0(unsigned int i_trk, unsigned int i_jet, bool doSmeared, TString sys){

  TLorentzVector jet;
  jet.SetPtEtaPhiM(this->trkjet_pt->at(i_jet),this->trkjet_eta->at(i_jet),this->trkjet_phi->at(i_jet),0.);

  float d0 = getd0(i_trk,i_jet,doSmeared,sys);
  //FIXME: does the d0err need to be corrected for smearing or systematics?
  float sd0=d0/this->trkjet_assocTrk_d0err->at(i_jet).at(i_trk);
  float det_sign=TMath::Sin(jet.Phi()-this->trkjet_assocTrk_phi->at(i_jet).at(i_trk))*d0;

  //std::cout<<"phi difference"<<jet.Phi()-this->trkjet_assocTrk_phi->at(i_jet).at(i_trk)<<std::endl;
  //std::cout<<"sign variables"<<det_sign<<"  "<< det_sign_other<<std::endl;

  float val = det_sign>=0 ? 1. : -1.;
  
  return val*TMath::Abs(sd0);
}

void GbbTupleAna::getBtagSFWeights(float &btag_SF_nom, float &btag_SF_tot_up, float &btag_SF_tot_down){
  
         btag_SF_nom=this->eve_BtagSFWeightNom;
   
   float SF_up=0,SF_down=0;
   this->getBtagSysWeights(SF_up, SF_down);
   btag_SF_tot_up=SF_up;
   btag_SF_tot_down=SF_down;

}

void GbbTupleAna::getBtagSysWeights(float &btag_SF_tot_up, float &btag_SF_tot_down){

  //Code adapted from Simon Viel
  
  float squared_weight_down=0.;
  float squared_weight_up=0.;
  float weight_nominal=this->eve_BtagSFWeightNom;

  //loop over b-tagging eigenvectors
  //add variations in the positive vs negative direction separately in quadrature
  //if the effect of both members of an eigenvector pair is in the same direction, take the bigger one

  //FIXME: Why doesn't this loop over the full vector?
  const unsigned int nbtagsys=this->eve_BtagSFWeightSys->size()/2;
  for(unsigned int s=0; s<nbtagsys; s+=2){
    float delta_up=(this->eve_BtagSFWeightSys->at(s)*weight_nominal-weight_nominal);
    float delta_down=(this->eve_BtagSFWeightSys->at(s+1)*weight_nominal-weight_nominal);
    /*std::cout<<"BtagSys "<<s<<std::endl;
    std::cout<<"Sys weight down:"<<this->eve_BtagSFWeightSys->at(s)<<std::endl;
    std::cout<<"Sys weight up:"<<this->eve_BtagSFWeightSys->at(s+1)<<std::endl;    
    std::cout<<"weight nominal:"<<weight_nominal<<std::endl;
    std::cout<<"delta_up"<<delta_up<<std::endl;
    std::cout<<"delta_down"<<delta_down<<std::endl;*/
    if(delta_up*delta_down<0){
     
      if(delta_up>0){
        squared_weight_down+=delta_down*delta_down;
        squared_weight_up+=delta_up*delta_up;
      }else{
        squared_weight_up+=delta_down*delta_down;
        squared_weight_down+=delta_up*delta_up;
      }      

    }else {

      float bigger_delta=std::max( TMath::Abs(delta_up), TMath::Abs(delta_down));
      if(delta_up>0) squared_weight_up+= bigger_delta*bigger_delta;
      else squared_weight_down+=bigger_delta*bigger_delta;

    }


  }

  //  btag_SF_tot_up=weight_nominal+TMath::Sqrt(squared_weight_up)*weight_nominal;
  // btag_SF_tot_down=weight_nominal-TMath::Sqrt(squared_weight_down)*weight_nominal;
  btag_SF_tot_up=weight_nominal+TMath::Sqrt(squared_weight_up);
  btag_SF_tot_down=weight_nominal-TMath::Sqrt(squared_weight_down);


  //std::cout<<"total delta_up"<<btag_SF_tot_up<<std::endl;
  //std::cout<<"total delta_down"<<btag_SF_tot_down<<std::endl;
  
  
}

std::vector<TString> GbbTupleAna::SplitString(TString str, char delim){

  std::vector<TString> tokens;
  TObjArray *Strings=str.Tokenize(delim);
  
  for(int i=0; i<Strings->GetEntriesFast(); i++){
    tokens.push_back(((TObjString*) (*Strings)[i])->GetString());
  }

  return tokens;
}

std::vector<float> GbbTupleAna::SplitStringD(TString str, char delim){

  std::vector<float> tokens;
  TObjArray *Strings=str.Tokenize(delim);
  
  for(int i=0; i<Strings->GetEntriesFast(); i++){
    tokens.push_back((((TObjString*) (*Strings)[i])->GetString()).Atof());
    std::cout<<"Token: "<<((TObjString*) (*Strings)[i])->GetString()<<" vs "<<tokens[i]<<std::endl;
  }

  return tokens;
}

void GbbTupleAna::getSystematicsFlags(GbbCandidate *gbbcand, bool &hasConversion, bool &hasHadMatInt, bool &hasLightLongLived, bool &hasNoTruthMuon){

  if(this->trkjet_hasHadMatInt->at(gbbcand->muojet_index) || this->trkjet_hasHadMatInt->at(gbbcand->nonmuojet_index)) hasHadMatInt=true;

  if(this->trkjet_hasConversion->at(gbbcand->muojet_index) || this->trkjet_hasConversion->at(gbbcand->nonmuojet_index)) hasConversion=true;
  
  if(this->trkjet_hasKShort->at(gbbcand->muojet_index) || this->trkjet_hasKShort->at(gbbcand->nonmuojet_index) || this->trkjet_hasLambda->at(gbbcand->muojet_index) || this->trkjet_hasLambda->at(gbbcand->nonmuojet_index)) hasLightLongLived=true;

  if(!gbbcand->hasTruthMuon) hasNoTruthMuon=true;

}


// a function to update the bit flag 
void GbbTupleAna::updateFlag(unsigned long int &flag, const unsigned int cutPosition, const bool passCut) {
  // Put bit passCut (true or false) at position cutPosition 
  flag = flag | passCut << cutPosition;
}
