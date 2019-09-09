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
  float d0err;
  float sd0;
  float z0sintheta;
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
  m_HistSvc=new HistogramService();
}

GbbTupleAna::~GbbTupleAna() {
  // TODO Auto-generated destructor stub

  if(m_FlavFracCorrector) delete m_FlavFracCorrector;
  if(m_HistSvc) delete m_HistSvc;
  if(m_config) delete m_config;

  // wesley xbb
  if(m_bTagger) delete m_bTagger;
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

  m_BTagWP = config->GetValue("BTagWP","");
  std::cout<<"BTagWP: "<<m_BTagWP<<std::endl;

  m_GeneratorName = config->GetValue("GeneratorName","Pythia");
  std::cout<<"GeneratorName: "<<m_GeneratorName<<std::endl;

  m_doJetPtReweighting = config->GetValue("doJetPtEtaReweighting",true);
  std::cout<<"doJetPtEtaReweighting: "<<m_doJetPtReweighting<<std::endl;

  m_JetPtReweightFile = config->GetValue("JetPtEtaReweightingFile","");
  std::cout<<"JetPtEtaReweightingFile: "<<m_JetPtReweightFile<<std::endl;

  m_JetPtReweightFileInclusive = config->GetValue("JetPtEtaReweightingFileInclusive","");
  std::cout<<"JetPtEtaReweightingFileInclusive: "<<m_JetPtReweightFileInclusive<<std::endl;

  m_doTrackSmearing = config->GetValue("doTrackSmearing",false);
  std::cout<<"doTrackSmearing: "<<m_doTrackSmearing<<std::endl;

  m_noMuonReq = config->GetValue("noMuonReq",false);
  std::cout<<"noMuonReq: "<<m_noMuonReq<<std::endl;

  m_useLeadingJets = config->GetValue("useLeadingJets",false);
  std::cout<<"useLeadingJets: "<<m_useLeadingJets<<std::endl;

  m_doApplyBTaggingSF = config->GetValue("doApplyBTaggingSF",false);
  std::cout<<"doApplyBTaggingSF "<<m_doApplyBTaggingSF<<std::endl;

  m_doSd0Systematics = config->GetValue("doSd0Systematics",false);
  std::cout<<"doSd0Systematics (will only work if nominal): "<<m_doSd0Systematics<<std::endl;

  m_doFlavFracCorrection = config->GetValue("doFlavFracCorrection",false);
  std::cout<<"doFlavFracCorrection: "<<m_doFlavFracCorrection<<std::endl;

  m_FlavFracCorrectorFile = config->GetValue("FlavFracCorrectorFile","./data/160714_FitResults_allsys.root");
  std::cout<<"FlavFracCorrectorFile: "<<m_FlavFracCorrectorFile<<std::endl;

  m_doEvenOddTemplates=config->GetValue("doEvenOddTemplates",false);
  std::cout<<"m_doEvemOddTemplates: "<< m_doEvenOddTemplates<<std::endl;

  m_doRandomSplitting=config->GetValue("doRandomSplitting",false);
  std::cout<<"m_doRandomSplitting: "<< m_doRandomSplitting<<std::endl;

  m_doPostfitPtReweighting=config->GetValue("doPostfitPtReweighting",false);
  std::cout<<"m_doPostfitPtReweighting: "<<m_doPostfitPtReweighting<<std::endl;

  m_PostfitPtReweightingFile=config->GetValue("PostfitPtReweightingFile","DEFAULT.root");
  std::cout<<"m_PostfitPtReweightingFile: "<<m_PostfitPtReweightingFile<<std::endl;

  m_useVRTrkJets = config->GetValue("useVRTrkJets",true);
  std::cout<<"useVRTrkJets: "<<m_useVRTrkJets<<std::endl;

  delete config;
  std::cout<<"=============================================="<<std::endl;
}

GbbTupleAna::GbbTupleAna(const std::vector<TString> infiles, const TString outfilename, const TString treename, const TString configname) :
  TupleAna(),
  m_config(nullptr),
  m_HistSvc(nullptr),
  m_Outputname(""),
  //m_chains(),
  m_RunMode(0),
  m_BTagWP(""),
  m_Debug(false),
  m_doJetPtReweighting(false),
  m_JetPtReweightFile(""),
  m_JetPtReweightFileInclusive(""),
  m_isMC(false),
  m_2016(false),
  m_2017(false),
  m_2018(false),
  m_isNominal(false),
  m_SysVarName(""),
  m_GeneratorName(""),
  m_FilterType(""),
  m_SumWeightTuple(0),
  m_nevtTuple(0),
  //m_reweightHistos(),
  m_doEvenOddTemplates(false),
  m_doFlavFracCorrection(false),
  m_FlavFracCorrectorFile(""),
  m_FlavFracCorrector(nullptr),
  m_doTrackSmearing(false),
  m_noMuonReq(false),
  m_doApplyBTaggingSF(false),
  m_doSd0Systematics(false),
  m_useVRTrkJets(true),
  //m_random(),
  m_doRandomSplitting(false),
  m_doFillMujet(false),
  //m_fatjet_pt_bins(),
  m_doPostfitPtReweighting(false),
  m_PostfitPtReweightingFile(""),
  m_postfit_reweight_hist(nullptr),
  // Wesley: Xbb score
  m_bTagger(nullptr) // used to be xbbscore f=0.2 eff =60
{
  TH1::AddDirectory(0);

  //=========================================
  // Initialize
  //=========================================
  ReadConfig(configname);
  m_HistSvc=new HistogramService();

  // if is not reweight mode; ie in calib mode, construct tagger
  // use configstring for btagging
  if(!(m_RunMode & RunMode::FILL_REWEIGHT)){
    try {
      m_bTagger = TaggerFactory::Create(m_BTagWP.Data());
    } catch (std::exception& e){
      // if the config string is bad, abort
      std::cerr << "exception: " << e.what() << std::endl;
      std::cerr << "Invalid configString: " << this->m_BTagWP.Data() << std::endl;
      exit(1);
    }
  }


  TH1D* metahist(nullptr), *metahist_tmp(nullptr);
  TChain *tree = new TChain(treename);
  TChain *fren = new TChain("FlavourTagging_Nominal");

  bool file1 = true;
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

    tree->Add(filename.Data());
    // Tracks only stored in nominal and track-systematics trees
    // from tuple version v00-07-09
    if (!treename.EqualTo("FlavourTagging_Nominal")) fren->Add(filename.Data());

    // getting filter type
    if (file1)
    {
      if (m_Debug) std::cout<<"Getting Filter Type.."<<std::endl;
      GetFilterType(filename.Data());
      file1 = false;
    }
  }
  if (!treename.EqualTo("FlavourTagging_Nominal")) tree->AddFriend(fren);

  if (!metahist) std::cout<<"FATAL: no metadata found!"<<std::endl;

  // Copy metadata
  m_HistSvc->FastFillTH1D("Hist_BookKeeping",1,6,0.5,6.5,metahist->GetBinContent(1));
  ((TH1D*) m_HistSvc->GetHisto("Hist_BookKeeping")) -> GetXaxis() -> SetBinLabel(1, "nEvents AOD");

  m_HistSvc->FastFillTH1D("Hist_BookKeeping",2,6,0.5,6.5,metahist->GetBinContent(2));
  ((TH1D*) m_HistSvc->GetHisto("Hist_BookKeeping")) -> GetXaxis() -> SetBinLabel(2, "nEvents DAOD");

  ((TH1D*) m_HistSvc->GetHisto("Hist_BookKeeping")) -> GetXaxis() -> SetBinLabel(3, "nEvents TUPLE");

  m_HistSvc->FastFillTH1D("Hist_BookKeeping",4,6,0.5,6.5,metahist->GetBinContent(4));
  ((TH1D*) m_HistSvc->GetHisto("Hist_BookKeeping")) -> GetXaxis() -> SetBinLabel(4, "sumWeights AOD");

  m_HistSvc->FastFillTH1D("Hist_BookKeeping",5,6,0.5,6.5,metahist->GetBinContent(5));
  ((TH1D*) m_HistSvc->GetHisto("Hist_BookKeeping")) -> GetXaxis() -> SetBinLabel(5, "sumWeights DAOD");

  ((TH1D*) m_HistSvc->GetHisto("Hist_BookKeeping")) -> GetXaxis() -> SetBinLabel(6, "sumWeights TUPLE");

  m_Outputname = outfilename;


  //running on the nominal sample?
  if (treename.Contains("Nominal")) {
    m_isNominal = true;
    m_SysVarName = "Nom";
  } else {
    m_isNominal = false;
    m_SysVarName = treename(15,treename.Length());
    m_doSd0Systematics = false;
  }

  if (m_isNominal) std::cout<<"Running on Nominal sample!"<<std::endl;
  else std::cout<<"Running on Systematics sample!"<<std::endl;


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

    std::cout << "Filter Type: " << m_FilterType <<std::endl;

    if(m_FilterType.EqualTo("Inclusive")){
      for(auto& elem : trig_slices) this->setReweightHisto(m_JetPtReweightFileInclusive,elem);
      std::cout<<"Do Inclusive Reweighting" << m_JetPtReweightFileInclusive << std::endl;
    }else if(m_FilterType.EqualTo("MuFiltered")){
      for(auto& elem : trig_slices) this->setReweightHisto(m_JetPtReweightFile,elem);
      std::cout<<"Do mu-Filtered Reweighting"<<m_JetPtReweightFile<<std::endl;
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

  Init(tree, m_SysVarName);

}


void GbbTupleAna::Finalize() {

  m_HistSvc->FastFillTH1D("Hist_BookKeeping",6,6,0.5,6.5,m_SumWeightTuple);
  m_HistSvc->FastFillTH1D("Hist_BookKeeping",3,6,0.5,6.5,m_nevtTuple);
  m_HistSvc->WriteRootFile(m_Outputname);

}


void GbbTupleAna::Loop() {

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

    if (jentry == 0) {
      m_isMC = this->eve_isMC;
      if (m_isMC) {
        m_2016 = (this->eve_run == 284500);
        m_2017 = (this->eve_run == 300000);
        m_2018 = (this->eve_run == 310000); //Not sure on this one
        if (!(m_2016 || m_2017 || m_2018)) {
          std::cout<<"Unknown MC run number: "<<this->eve_run<<std::endl;
        }
      } else {
        if (this->eve_run < 324320) m_2016 = true;
        else if (this->eve_run < 348197) m_2017 = true;
        else if (this->eve_run < 364486) m_2018 = true;
        else {
          std::cout<<"Unknown data run number: "<<this->eve_run<<std::endl;
        }
      }
      m_bTagger->setFlags({m_isMC, m_useVRTrkJets});
      m_bTagger->initialize(*this);
    }

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

  //bool IsLargeWeightEvent=false;

  updateFlag(eventFlag,GbbCuts::AllNtup,true);

  //=========================================
  //1.) Event cleaning & weight calculation
  //=========================================

  float total_evt_weight=(this->eve_mc_w*this->eve_pu_w);
  //float original_evt_weight=total_evt_weight;
  //if(original_evt_weight>100) return false;

  icut++;
  m_HistSvc->FastFillTH1D(Form("CutFlow_%s",m_SysVarName.Data()),icut,15,0.5,15.5,total_evt_weight);
  ((TH1D*) m_HistSvc->GetHisto(Form("CutFlow_%s",m_SysVarName.Data())))->GetXaxis()->SetBinLabel(icut, "nEvents total");

  m_SumWeightTuple+=total_evt_weight;
  //if (m_isNominal) m_SumWeightTuple+=this->eve_pu_w;

  m_HistSvc->FastFillTH1D("PUWeights",this->eve_pu_w,50,0.,5.,1.);
  m_HistSvc->FastFillTH1D("EventWeights",total_evt_weight,102,-1.,100.,1.);
  m_HistSvc->FastFillTH1D("EventMu",this->eve_mu,80,0.,80.,total_evt_weight);
  m_HistSvc->FastFillTH1D("EventAvgMu",this->eve_avg_mu,80,0.,80.,total_evt_weight);
  m_HistSvc->FastFillTH1D("EventPVz",this->eve_PVz,80,-150.,150.,total_evt_weight);
  if (!m_config->GetIsR20p7()) {
    m_HistSvc->FastFillTH1D("PUDensity",this->eve_pu_density,80,0.,80.,total_evt_weight);
  }

  //double mc_jet_ratio=1.;

  if (!this->isCleanEvt(total_evt_weight)) return false;

  if (this->hasBadJet()) return false;

  updateFlag(eventFlag,GbbCuts::EventCleaning,true);

  icut++;
  m_HistSvc->FastFillTH1D(Form("CutFlow_%s",m_SysVarName.Data()),icut,15,0.5,15.5,total_evt_weight);
  ((TH1D*) m_HistSvc->GetHisto(Form("CutFlow_%s",m_SysVarName.Data())))->GetXaxis()->SetBinLabel(icut, "pass cleaning");

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
  m_HistSvc->FastFillTH1D(Form("CutFlow_%s",m_SysVarName.Data()),icut,15,0.5,15.5,total_evt_weight);
  ((TH1D*) m_HistSvc->GetHisto(Form("CutFlow_%s",m_SysVarName.Data())))->GetXaxis()->SetBinLabel(icut, "pass trigger match");

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
  m_HistSvc->FastFillTH1D(Form("CutFlow_%s",m_SysVarName.Data()),icut,15,0.5,15.5,total_evt_weight);
  ((TH1D*) m_HistSvc->GetHisto(Form("CutFlow_%s",m_SysVarName.Data())))->GetXaxis()->SetBinLabel(icut, "pass trigger");

  //FILL REWEIGHT HISTOGRAMS
  if(m_isNominal && m_RunMode & RunMode::FILL_REWEIGHT){
    //FIXME: do we want to trigger on large-R jet?
    this->FillReweightInfo(i_trigjet,total_evt_weight,trigger_passed);
    //this->FillFatReweightInfo(largeRtrigpt,largeRtrigeta,total_evt_weight,trigger_passed);
  }

  //Fill Pileup reweighting histograms
  if(m_isNominal && m_RunMode & RunMode::FILL_PRW) {
    m_HistSvc->FastFillTH1D("h"+m_SysVarName+"_evemu",";<#mu>;Events/5;",
     this->eve_mu,12,0.,60.,total_evt_weight);
  }

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
   m_HistSvc->FastFillTH1D(Form("CutFlow_%s",m_SysVarName.Data()),icut,15,0.5,15.5,total_evt_weight);
  ((TH1D*) m_HistSvc->GetHisto(Form("CutFlow_%s",m_SysVarName.Data())))->GetXaxis()->SetBinLabel(icut, "trig-jet reweight");

  //=========================================
  //3.) Gbb candidate selection & definition
  //=========================================

  if(m_Debug) std::cout<<"processgbb(): Constructing Gbb candidate..."<<std::endl;

  std::vector<GbbCandidate> gbbcands = constructGbbCandidates(m_noMuonReq || m_useLeadingJets);

  if (gbbcands.size() == 0) {
    if(m_Debug) std::cout<<"processgbb(): No gbb candidate found"<<std::endl;
    return false;
  }
  // Use highest pt gbbcandidate that contains a muon. //FIXME: why aren't they pt-ordered already?
  GbbCandidate gbbcand = gbbcands.at(0);
  bool gbbInvalid = (gbbcand.nRecoMuons == 0 && !m_noMuonReq);
  for (auto& cand : gbbcands) {
    if (cand.nRecoMuons == 0 && !m_noMuonReq) continue;
    // if the first gbbcand has no muons then take the next regardless of pt
    if (fat_pt->at(cand.ind_fj) > fat_pt->at(gbbcand.ind_fj) || gbbInvalid) {
      gbbcand = cand;
      gbbInvalid = false;
    }
  }
  if (gbbcand.nRecoMuons == 0 && !m_noMuonReq) {
    if(m_Debug) std::cout<<"processgbb(): No gbb candidate found"<<std::endl;
    return false;
  }
  icut++;
  m_HistSvc->FastFillTH1D(Form("CutFlow_%s",m_SysVarName.Data()),icut,15,0.5,15.5,total_evt_weight);
  ((TH1D*) m_HistSvc->GetHisto(Form("CutFlow_%s",m_SysVarName.Data())))->GetXaxis()->SetBinLabel(icut, "has gbb candidate");

  if(m_Debug) std::cout<<"constructGbbCandidate(): Finish Gbb construction!"<<std::endl;
  if (m_useVRTrkJets) {
    TLorentzVector muojet_vec, nonmuojet_vec;
    muojet_vec.SetPtEtaPhiM( this->trkjet_pt->at(gbbcand.ind_mj)/1e3,
                             this->trkjet_eta->at(gbbcand.ind_mj),
                             this->trkjet_phi->at(gbbcand.ind_mj),
                             0.);
    nonmuojet_vec.SetPtEtaPhiM( this->trkjet_pt->at(gbbcand.ind_nmj)/1e3,
                                this->trkjet_eta->at(gbbcand.ind_nmj),
                                this->trkjet_phi->at(gbbcand.ind_nmj),
                                0.);
    m_HistSvc->FastFillTH1D(
     m_config->GetMCHistName(m_SysVarName,"Incl","Incl","DRtrkjets"),
     ";#Delta R(muon-jet,non-muon jet);Events/0.01;",
     muojet_vec.DeltaR(nonmuojet_vec),100,0.,1.0,total_evt_weight
    );
    float muojet_minVR = std::max( 0.02, std::min(0.4, 30.0e3 / this->trkjet_pt->at(gbbcand.ind_mj)) );
    float nonmuojet_minVR = std::max( 0.02, std::min(0.4, 30.0e3 / this->trkjet_pt->at(gbbcand.ind_nmj)) );
    for (unsigned int i_jet=0; i_jet < this->trkjet_pt->size(); i_jet++) {
      TLorentzVector jet_vec;
      jet_vec.SetPtEtaPhiM( this->trkjet_pt->at(i_jet)/1e3,
                            this->trkjet_eta->at(i_jet),
                            this->trkjet_phi->at(i_jet),
                            0.);
      float jet_minVR = std::max( 0.02, std::min(0.4, 30.0e3 / this->trkjet_pt->at(i_jet)) );
      if ( i_jet != gbbcand.ind_mj &&
           TMath::Log(muojet_vec.DeltaR(jet_vec)/std::min(muojet_minVR, jet_minVR)) < 0 ) {
        if(m_Debug) std::cout<<"constructGbbCandidate(): Removed event with  overlapping VR trackjets"<<std::endl;
        return false;
      }
      if ( i_jet != gbbcand.ind_nmj &&
           TMath::Log(nonmuojet_vec.DeltaR(jet_vec)/std::min(nonmuojet_minVR, jet_minVR)) < 0 ) {
        if(m_Debug) std::cout<<"constructGbbCandidate(): Removed event with  overlapping VR trackjets"<<std::endl;
        return false;
      }
    }
    icut++;
    m_HistSvc->FastFillTH1D(Form("CutFlow_%s",m_SysVarName.Data()),icut,15,0.5,15.5,total_evt_weight);
    ((TH1D*) m_HistSvc->GetHisto(Form("CutFlow_%s",m_SysVarName.Data())))->GetXaxis()->SetBinLabel(icut, "pass trackjet overlap cut");
  }

  //if(gbbcand.ind_mj == gbbcand.ind_nmj) {
  //  if(m_Debug) std::cout<<"constructGbbCandidate(): Muon and non-muon jet have same index!"<<std::endl;
  //  return false;
  //}
  if(!gbbcand.hasLeadTrkJets){
    if(m_Debug) std::cout<<"constructGbbCandidate(): Require muon and non-muon jets be leading 2 trackjets"<<std::endl;
    return false;
  }
  icut++;
  m_HistSvc->FastFillTH1D(Form("CutFlow_%s",m_SysVarName.Data()),icut,15,0.5,15.5,total_evt_weight);
  ((TH1D*) m_HistSvc->GetHisto(Form("CutFlow_%s",m_SysVarName.Data())))->GetXaxis()->SetBinLabel(icut, "trackjets are lj/slj");

  if(m_Debug){
    std::cout<<"processgbb(): Finished constructing Gbb candidate!"<<std::endl;
    std::cout<<"processgbb(): gbbcand.ind_fj "<<gbbcand.ind_fj<<std::endl;
    std::cout<<"processgbb(): gbbcand.ind_mj "<<gbbcand.ind_mj<<std::endl;
    std::cout<<"processgbb(): gbbcand.ind_nmj "<<gbbcand.ind_nmj<<std::endl;
  }

  updateFlag(eventFlag, GbbCuts::GbbCandidate, true);

  //define flavour truth labels & pt labels
  int muojet_truth=this->trkjet_truth->at(gbbcand.ind_mj);
  int nonmuojet_truth=this->trkjet_truth->at(gbbcand.ind_nmj);
  TString dijet_flav = this->eve_isMC ? m_config->GetFlavourPair(muojet_truth,nonmuojet_truth) : TString("Data");

  TString ptlabel = m_config->GetDiTrkJetLabel(this->trkjet_pt->at(gbbcand.ind_mj)/1e3,
                                               this->trkjet_pt->at(gbbcand.ind_nmj)/1e3);

  if(m_Debug) std::cout<<"processgbb(): Got labels."<<std::endl;

  float gbbcandpt=this->fat_pt->at(gbbcand.ind_fj);
  //  std::cout<<"trigjet pt is: "<<this->jet_pt->at(i_trigjet)<<" "<<gbbcandpt<<std::endl;

  //cut away region where jet bias is introduced
  if (!cutTriggerBias(gbbcandpt, trigger_passed)) return false;
  if(m_Debug) std::cout<<"processgbb(): Pass pt cuts."<<std::endl;
  icut++;
  m_HistSvc->FastFillTH1D(Form("CutFlow_%s",m_SysVarName.Data()),icut,15,0.5,15.5,total_evt_weight);
  ((TH1D*) m_HistSvc->GetHisto(Form("CutFlow_%s",m_SysVarName.Data())))->GetXaxis()->SetBinLabel(icut, "pass trigger bias cut");


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

  trkjetSd0Info  muojet_sd0Info=this->getTrkjetAssocSd0Info(gbbcand.ind_mj,m_doTrackSmearing,"nominal","nominal",3);
  trkjetSd0Info  nonmuojet_sd0Info=this->getTrkjetAssocSd0Info(gbbcand.ind_nmj,m_doTrackSmearing,"nominal","nominal",3);
  float muojet_maxsd0 = muojet_sd0Info.meanSd0_pt;
  float nonmuojet_maxsd0 = nonmuojet_sd0Info.meanSd0_pt;

  if(TMath::Abs(muojet_maxsd0+99.)<1e-5 || TMath::Abs(nonmuojet_maxsd0+99.)<1e-5) return false; //associated tracks do not fulfil selection cuts

  updateFlag(eventFlag,GbbCuts::GoodSd0Tracks,true);
  if(m_Debug) std::cout<<"processgbb(): Good sd0."<<std::endl;

  icut++;
  m_HistSvc->FastFillTH1D(Form("CutFlow_%s",m_SysVarName.Data()),icut,15,0.5,15.5,total_evt_weight);
  ((TH1D*) m_HistSvc->GetHisto(Form("CutFlow_%s",m_SysVarName.Data())))->GetXaxis()->SetBinLabel(icut, "got Sd0 variables");

  //=========================================
  //5.) Topo cut
  //=========================================

  TLorentzVector mujet,trigjet;

  mujet.SetPtEtaPhiE(this->trkjet_pt->at(gbbcand.ind_mj),this->trkjet_eta->at(gbbcand.ind_mj),this->trkjet_phi->at(gbbcand.ind_mj),this->trkjet_E->at(gbbcand.ind_mj));

  trigjet.SetPtEtaPhiE(this->jet_pt->at(i_trigjet),this->jet_eta->at(i_trigjet),this->jet_phi->at(i_trigjet),this->jet_E->at(i_trigjet));

  if(mujet.DeltaR(trigjet)<1.5) return false;

  updateFlag(eventFlag,GbbCuts::DRTrigJetMuJet,true);

  //DR 2-Track jets
  TLorentzVector ditrkjet,nonmujet, fatjet;
  nonmujet.SetPtEtaPhiE(this->trkjet_pt->at(gbbcand.ind_nmj),this->trkjet_eta->at(gbbcand.ind_nmj),this->trkjet_phi->at(gbbcand.ind_nmj),this->trkjet_E->at(gbbcand.ind_nmj));

  ditrkjet=mujet+nonmujet;
  fatjet.SetPtEtaPhiE(this->fat_pt->at(gbbcand.ind_fj),this->fat_eta->at(gbbcand.ind_fj),this->fat_phi->at(gbbcand.ind_fj),this->fat_E->at(gbbcand.ind_fj));

  float DRditrkjetfatjet=fatjet.DeltaR(ditrkjet);

  if(m_Debug) std::cout<<"processgbb(): Pass topo cuts"<<std::endl;
  icut++;
  m_HistSvc->FastFillTH1D(Form("CutFlow_%s",m_SysVarName.Data()),icut,15,0.5,15.5,total_evt_weight);
  ((TH1D*) m_HistSvc->GetHisto(Form("CutFlow_%s",m_SysVarName.Data())))->GetXaxis()->SetBinLabel(icut, "pass topo cuts");

  icut++;
  if (gbbcand.nRecoMuons > 1) {
    if(m_Debug) std::cout<<"processgbb(): Has two reco muons"<<std::endl;
    m_HistSvc->FastFillTH1D(Form("CutFlow_%s",m_SysVarName.Data()),icut,15,0.5,15.5,total_evt_weight);
    ((TH1D*) m_HistSvc->GetHisto(Form("CutFlow_%s",m_SysVarName.Data())))->GetXaxis()->SetBinLabel(icut, "two muons");
  }

  //=========================================
  //6.) Trigjet reweighting
  //=========================================


  //  //If mode is: FillReweightHists Fill possible new reweighting histogram before event_weight is changed
  //if(m_isNominal && m_RunMode & RunMode::FILL_REWEIGHT) this->FillReweightInfo(i_trigjet,total_evt_weight,trigger_passed);


  /*  if(m_doPostfitPtReweighting && this->eve_isMC){
     double weight=1;
     if((this->fat_pt->at(gbbcand.ind_fj)/1e3 < 250.) || (this->fat_pt->at(gbbcand.ind_fj)/1e3 > 1000.)) weight=1.;
     else weight=m_postfit_reweight_hist->GetBinContent(m_postfit_reweight_hist->FindBin(this->fat_pt->at(gbbcand.ind_fj)/1e3));
     //std::cout<<"postfit weight: "<<weight<<std::endl;

     total_evt_weight*= weight;
     }*/


  //}


  //=========================================
  //7.) B-Tagging
  //=========================================

  /*bTagger ---*/
  // read xbbscore parameters, then use xbbcutter to determine
  // if this gbb candidate is b tagged.
  int isTagged = 0;
  float mjSF(-1.), nmjSF(-1.);
  if(m_Debug) std::cout<<"processgbb(): Getting tags"<<std::endl;
  if(!(m_RunMode & RunMode::FILL_REWEIGHT)){
    isTagged = m_bTagger->accept(gbbcand,mjSF,nmjSF);
  } else {
    std::cout << " not tagging mode. done." << std::endl;
    return false;
  }

  /*bTagger ---*/


  //at least 1 b-tag
  if(isTagged == 1 || isTagged == 2) updateFlag(eventFlag,GbbCuts::MuNonMu1Btag,true);
  //2 b-tags
  if(isTagged == 2) updateFlag(eventFlag,GbbCuts::MuNonMu2Btags,true);
  //0 b-tags
  if(isTagged == 0) updateFlag(eventFlag,GbbCuts::MuNonMuAnti2Btags,true);
  //at most 1 b-tag
  if(isTagged == 0 || isTagged == 1) updateFlag(eventFlag,GbbCuts::MuNonMuUntagged,true);

  icut++;
  if (isTagged == 2) {
    if(m_Debug) std::cout<<"processgbb(): Has two b-tags"<<std::endl;
    m_HistSvc->FastFillTH1D(Form("CutFlow_%s",m_SysVarName.Data()),icut,15,0.5,15.5,total_evt_weight);
    ((TH1D*) m_HistSvc->GetHisto(Form("CutFlow_%s",m_SysVarName.Data())))->GetXaxis()->SetBinLabel(icut, "2 b-tags");
  }

  icut++;
  if (isTagged == 2 && gbbcand.nRecoMuons > 1) {
    if(m_Debug) std::cout<<"processgbb(): Has two b-tags and two muons"<<std::endl;
    m_HistSvc->FastFillTH1D(Form("CutFlow_%s",m_SysVarName.Data()),icut,15,0.5,15.5,total_evt_weight);
    ((TH1D*) m_HistSvc->GetHisto(Form("CutFlow_%s",m_SysVarName.Data())))->GetXaxis()->SetBinLabel(icut, "2 muons and 2 b-tags");
  }

  //=========================================
  //7b.) Bhadron reweighting (temporary)
  //=========================================

  if(m_Debug) std::cout<<"processgbb(): Bhad reweighting"<<std::endl;
  double Bhadupweight=1., Bhaddownweight=1.;
  if(dijet_flav.EqualTo("BB")){
    double BhadPt=this->trkjet_BHad_pt->at(gbbcand.ind_mj)/1e3;

    //up by 10%
    if(BhadPt<250.) Bhadupweight=0.88;
    else Bhadupweight=0.15+0.0024*BhadPt;

    //down by 10%
    if(BhadPt<250.) Bhaddownweight=1.09;
    else Bhaddownweight=1.95-0.0029*BhadPt;

  }

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
  if(m_Debug) std::cout<<"processgbb(): Fill histograms"<<std::endl;
  if(passSpecificCuts(eventFlag, CutsNoBtag)){

    m_HistSvc->FastFillTH1D("Hist_MCStatsUnc",1,1,0.5,1.5,total_evt_weight);

    if (m_isMC && m_isNominal) {
      m_HistSvc->FastFillTH1D( makeDiJetPlotName(&gbbcand,"SF"),";product of track-jet SFs;Events",
        mjSF*nmjSF,50,0.,5.,total_evt_weight);
    }

    if(m_RunMode & RunMode::FILL_TEMPLATES) FillTemplates(&gbbcand,total_evt_weight);
    if(m_RunMode & RunMode::FILL_TRKJET_PROPERTIES) FillTrackJetProperties(&gbbcand,total_evt_weight);
    if(m_RunMode & RunMode::FILL_FATJET_PROPERTIES) FillFatJetProperties(&gbbcand,total_evt_weight);
    if(m_RunMode & RunMode::FILL_ADV_PROPERTIES) FillAdvancedProperties(&gbbcand,i_trigjet,total_evt_weight);
    if(m_RunMode & RunMode::FILL_MC_STATS) FillMCStatsInfo(&gbbcand);

    if (gbbcand.nRecoMuons > 1) {
      if(m_RunMode & RunMode::FILL_TEMPLATES) FillTemplates(&gbbcand,total_evt_weight,"TWOMUON");
      if(m_RunMode & RunMode::FILL_TRKJET_PROPERTIES) FillTrackJetProperties(&gbbcand,total_evt_weight,"TWOMUON");
      if(m_RunMode & RunMode::FILL_FATJET_PROPERTIES) FillFatJetProperties(&gbbcand,total_evt_weight,"TWOMUON");
      if(m_RunMode & RunMode::FILL_ADV_PROPERTIES) FillAdvancedProperties(&gbbcand,i_trigjet,total_evt_weight,"TWOMUON");
      if(m_RunMode & RunMode::FILL_MC_STATS) FillMCStatsInfo(&gbbcand,"TWOMUON");
    }

    //for crosscheck: Fill Even and Odd Templates
    if(this->eve_isMC && m_doEvenOddTemplates && m_RunMode & RunMode::FILL_TEMPLATES){
      if(!(i_evt%2)) this->FillTemplates(&gbbcand,total_evt_weight,"EVEN");
      else this->FillTemplates(&gbbcand,total_evt_weight,"ODD");
    }

    m_HistSvc->FastFillTH1D( makeDiJetPlotName(&gbbcand,"DRditrkjetfatjet"),
     ";#Delta R(fatjet, inv. sum of track-jets);Events/0.002",
     DRditrkjetfatjet,250,0.,0.5,total_evt_weight
    );
    if(m_isNominal) {
      m_HistSvc->FastFillTH2D(
       m_config->GetMCHistName(m_SysVarName,"Incl","Incl","DRditrkjetfatjetVSfjpt"),
       ";#Delta R(fatjet, inv. sum of track-jets);large-R jet p_{T} [GeV]",
       DRditrkjetfatjet,fatjet.Pt()/1e3,25,0.,0.5,50,0.,1000.,total_evt_weight
      );

      //Fill BHad fragmentation information for extrapolation studies
      if (dijet_flav.EqualTo("BB") || dijet_flav.EqualTo("BL")) {
	int ID_transform=0;
	if(TMath::Abs(this->trkjet_BHad_pdgId->at(gbbcand.ind_mj))==511) ID_transform=1; //B0
	else if(TMath::Abs(this->trkjet_BHad_pdgId->at(gbbcand.ind_mj))==521)ID_transform=2; //B+-
	else if(TMath::Abs(this->trkjet_BHad_pdgId->at(gbbcand.ind_mj))==531)ID_transform=3; //B_s
	else if(TMath::Abs(this->trkjet_BHad_pdgId->at(gbbcand.ind_mj))==5122)ID_transform=4; //Lambda_B
	else if(TMath::Abs(this->trkjet_BHad_pdgId->at(gbbcand.ind_mj))==5132)ID_transform=5; //Cascade_B-
	else if(TMath::Abs(this->trkjet_BHad_pdgId->at(gbbcand.ind_mj))==5232)ID_transform=6; //Cascade_B0


	if(ID_transform) {
          m_HistSvc->FastFillTH1D(
           m_config->GetMCHistName(m_SysVarName,"Incl","BX","mjBHadPdgID"),
           ";muon-jet b-hadron ID;Events",
           ID_transform,6,0.5,6.5,total_evt_weight
          );
        }
      }

      //If mode specifies, fill extrapolation-specific histograms
      double Had_weight_incl=1., Had_weight_Mu=1.;
      if(dijet_flav.EqualTo("BB") || dijet_flav.EqualTo("BL")){

        //FIXME: where do these values come from?
	if(TMath::Abs(this->trkjet_BHad_pdgId->at(gbbcand.ind_mj))==511){//B0
	  Had_weight_incl=0.93;
	  Had_weight_Mu=0.97;
	}else if(TMath::Abs(this->trkjet_BHad_pdgId->at(gbbcand.ind_mj))==521){//B+-
	  Had_weight_incl=0.95;
	  Had_weight_Mu=0.95;
	}else if(TMath::Abs(this->trkjet_BHad_pdgId->at(gbbcand.ind_mj))==531){ //B_s
	  Had_weight_incl=1.06;
	  Had_weight_Mu=1.07;
	}else if(TMath::Abs(this->trkjet_BHad_pdgId->at(gbbcand.ind_mj))==5122){//Lambda_B
	  Had_weight_incl=1.81;
	  Had_weight_Mu=2.16;
	}

	/*if(dijet_flav.EqualTo("BB")){
	  FillTemplates(&gbbcand,total_evt_weight*Had_weight_incl);
	 }*/

      }

      if (m_RunMode & RunMode::FILL_EXTRAP_INCL) {
	FillTemplates(&gbbcand,total_evt_weight*Had_weight_incl,"EXTRDATA");
	FillTemplates(&gbbcand,total_evt_weight,"EXTR");
      }
      if (m_RunMode & RunMode::FILL_EXTRAP_MU) {
	FillTemplates(&gbbcand,total_evt_weight*Had_weight_Mu,"EXTRDATA");
	FillTemplates(&gbbcand,total_evt_weight,"EXTR");
      }
    }

    //fill BHadron pt for reweighting
    if(dijet_flav.EqualTo("BB")){
      m_HistSvc->FastFillTH1D(
        m_config->GetMCHistName(m_SysVarName,"Incl",dijet_flav,"BHadPt"),
        ";muon-jet b-hadron p_{T} [GeV];Events/5 GeV;",
        this->trkjet_BHad_pt->at(gbbcand.ind_mj)/1e3,100,0.,500.,total_evt_weight
      );
      //FIXME: don't these just shift the plot left/right slightly?
      m_HistSvc->FastFillTH1D(
        m_config->GetMCHistName(m_SysVarName,"Incl",dijet_flav,"BHadPtScaledUp"),
        ";muon-jet b-hadron p_{T} (scaled up) [GeV];Events/5 GeV;",
        this->trkjet_BHad_pt->at(gbbcand.ind_mj)*1.1/1e3,100,0.,500.,total_evt_weight
      );
      m_HistSvc->FastFillTH1D(
        m_config->GetMCHistName(m_SysVarName,"Incl",dijet_flav,"BHadPtScaledDown"),
        ";muon-jet b-hadron p_{T} (scaled down) [GeV];Events/5 GeV;",
        this->trkjet_BHad_pt->at(gbbcand.ind_mj)*0.9/1e3,100,0.,500.,total_evt_weight
      );

      this->FillTemplates(&gbbcand,total_evt_weight*Bhadupweight,"BHADPTUP");
      this->FillTemplates(&gbbcand,total_evt_weight*Bhaddownweight,"BHADPTDOWN");
    }

    TLorentzVector smallRJet;
    if(i_sublsmallRjet>=0){
      smallRJet.SetPtEtaPhiE(this->jet_pt->at(i_sublsmallRjet),this->jet_eta->at(i_sublsmallRjet),this->jet_phi->at(i_sublsmallRjet),this->jet_E->at(i_sublsmallRjet));
      if(smallRJet.DeltaR(trigjet)>1.5) {
        m_HistSvc->FastFillTH1D( makeDiJetPlotName(&gbbcand,"slR4jpt"),
         ";sub-leading R=0.4 jet p_{T} [GeV];Events/4 GeV",
         trjet_pt/1e3,250,0.,1000.,total_evt_weight);
      }
    }

    m_HistSvc->FastFillTH1D( makeDiJetPlotName(&gbbcand,"srjN"),";No. of R=0.4 jets;Events",
     this->jet_pt->size(),10,0.,10.,total_evt_weight);

    for (unsigned int i=0; i < this->jet_pt->size(); i++) {
      m_HistSvc->FastFillTH1D( makeDiJetPlotName(&gbbcand,"allsrjpt"),";R=0.4 jet p_{T} [GeV];Events/4 GeV",
       this->jet_pt->at(i)/1e3,250,0.,1000.,total_evt_weight);
    }

  }

  //2.) Before Fit and after b-tagging
  float btag_SF_nom=1., btag_SF_up=1., btag_SF_down=1.;
  //FIXME: shouldn't the doApplyBTaggingSF flag turn off BTagging SFs?
  if(this->eve_isMC && (m_doApplyBTaggingSF || !dijet_flav.Contains("BB"))) this->getBtagSFWeights(btag_SF_nom,btag_SF_up,btag_SF_down);

  if (passSpecificCuts(eventFlag, CutsWith2Btags)) {
    //std::cout<<"Filling post-tag plots for"<<m_SysVarName<<"with btag_SF :"<<btag_SF_nom<<std::endl;

    if (m_isMC && m_isNominal) {
      m_HistSvc->FastFillTH1D( makeDiJetPlotName(&gbbcand,"SF_PREFITPOSTTAG"),
        ";product of track-jet SFs;Events",
        mjSF*nmjSF,50,0.,5.,total_evt_weight);
    }

    if(m_RunMode & RunMode::FILL_TEMPLATES && !(m_RunMode & RunMode::FOR_FIT_ONLY)) this->FillTemplates(&gbbcand,total_evt_weight*btag_SF_nom,"PREFITPOSTTAG");
    if(m_RunMode & RunMode::FILL_TRKJET_PROPERTIES) this->FillTrackJetProperties(&gbbcand,total_evt_weight*btag_SF_nom,"PREFITPOSTTAG");
    if(m_RunMode & RunMode::FILL_FATJET_PROPERTIES) this->FillFatJetProperties(&gbbcand,total_evt_weight*btag_SF_nom,"PREFITPOSTTAG");
    if(m_RunMode & RunMode::FILL_ADV_PROPERTIES) this->FillAdvancedProperties(&gbbcand,i_trigjet,total_evt_weight*btag_SF_nom,"PREFITPOSTTAG");
    if(m_RunMode & RunMode::FILL_MC_STATS) FillMCStatsInfo(&gbbcand,"PREFITPOSTTAG");

    if (gbbcand.nRecoMuons > 1) {
      if(m_RunMode & RunMode::FILL_TEMPLATES) FillTemplates(&gbbcand,total_evt_weight,"TWOMUON_PREFITPOSTTAG");
      if(m_RunMode & RunMode::FILL_TRKJET_PROPERTIES) FillTrackJetProperties(&gbbcand,total_evt_weight,"TWOMUON_PREFITPOSTTAG");
      if(m_RunMode & RunMode::FILL_FATJET_PROPERTIES) FillFatJetProperties(&gbbcand,total_evt_weight,"TWOMUON_PREFITPOSTTAG");
      if(m_RunMode & RunMode::FILL_ADV_PROPERTIES) FillAdvancedProperties(&gbbcand,i_trigjet,total_evt_weight,"TWOMUON_PREFITPOSTTAG");
      if(m_RunMode & RunMode::FILL_MC_STATS) FillMCStatsInfo(&gbbcand,"TWOMUON_PREFITPOSTTAG");
    }

    m_HistSvc->FastFillTH1D( makeDiJetPlotName(&gbbcand,"DRditrkjetfatjet_PREFITPOSTTAG"),
     ";#Delta R(fatjet, inv. sum of track-jets);Events/0.002",
     DRditrkjetfatjet,250,0.,0.5,total_evt_weight
    );

    if(m_isNominal){

      if(m_RunMode & RunMode::FILL_TEMPLATES && !(m_RunMode & RunMode::FOR_FIT_ONLY)) this->FillTemplates(&gbbcand,total_evt_weight*btag_SF_up,"PREFITPOSTTAG_BTAGUP");
      if(m_RunMode & RunMode::FILL_TRKJET_PROPERTIES) this->FillTrackJetProperties(&gbbcand,total_evt_weight*btag_SF_up,"PREFITPOSTTAG_BTAGUP");
      if(m_RunMode & RunMode::FILL_FATJET_PROPERTIES) this->FillFatJetProperties(&gbbcand,total_evt_weight*btag_SF_up,"PREFITPOSTTAG_BTAGUP");
    if(m_RunMode & RunMode::FILL_ADV_PROPERTIES) this->FillAdvancedProperties(&gbbcand,i_trigjet,total_evt_weight*btag_SF_up,"PREFITPOSTTAG_BTAGUP");

      if(m_RunMode & RunMode::FILL_TEMPLATES && !(m_RunMode & RunMode::FOR_FIT_ONLY)) this->FillTemplates(&gbbcand,total_evt_weight*btag_SF_down,"PREFITPOSTTAG_BTAGDOWN");
      if(m_RunMode & RunMode::FILL_TRKJET_PROPERTIES) this->FillTrackJetProperties(&gbbcand,total_evt_weight*btag_SF_down,"PREFITPOSTTAG_BTAGDOWN");
      if(m_RunMode & RunMode::FILL_FATJET_PROPERTIES) this->FillFatJetProperties(&gbbcand,total_evt_weight*btag_SF_down,"PREFITPOSTTAG_BTAGDOWN");
      if(m_RunMode & RunMode::FILL_ADV_PROPERTIES) this->FillAdvancedProperties(&gbbcand,i_trigjet,total_evt_weight*btag_SF_down,"PREFITPOSTTAG_BTAGDOWN");

      //If mode specifies, fill extrapolation-specific histograms
      double Had_weight_incl=1., Had_weight_Mu=1.;
      if (dijet_flav.EqualTo("BB") || dijet_flav.EqualTo("BL")) {

        //FIXME: where do these values come from?
	if(TMath::Abs(this->trkjet_BHad_pdgId->at(gbbcand.ind_mj))==511){//B0
	  Had_weight_incl=0.93;
	  Had_weight_Mu=0.97;
	}else if(TMath::Abs(this->trkjet_BHad_pdgId->at(gbbcand.ind_mj))==521){//B+-
	  Had_weight_incl=0.95;
	  Had_weight_Mu=0.95;
	}else if(TMath::Abs(this->trkjet_BHad_pdgId->at(gbbcand.ind_mj))==531){ //B_s
	  Had_weight_incl=1.06;
	  Had_weight_Mu=1.07;
	}else if(TMath::Abs(this->trkjet_BHad_pdgId->at(gbbcand.ind_mj))==5122){//Lambda_B
	  Had_weight_incl=1.81;
	  Had_weight_Mu=2.16;
	}

	/*if(dijet_flav.EqualTo("BB")){
	  FillTemplates(&gbbcand,total_evt_weight*Had_weight_incl);
	 }*/

      }

      if (m_RunMode & RunMode::FILL_EXTRAP_INCL) {
	FillTemplates(&gbbcand,total_evt_weight*Had_weight_incl,"PREFITPOSTTAG_EXTRDATA");
	FillTemplates(&gbbcand,total_evt_weight,"PREFITPOSTTAG_EXTR");
      }
      if (m_RunMode & RunMode::FILL_EXTRAP_MU) {
	FillTemplates(&gbbcand,total_evt_weight*Had_weight_Mu,"PREFITPOSTTAG_EXTRDATA");
	FillTemplates(&gbbcand,total_evt_weight,"PREFITPOSTTAG_EXTR");
      }
    }

    //fill BHadron pt for reweighting
    if (dijet_flav.EqualTo("BB")) {
      this->FillTemplates(&gbbcand,total_evt_weight*Bhadupweight,"PREFITPOSTTAG_BHADPTUP");
      this->FillTemplates(&gbbcand,total_evt_weight*Bhaddownweight,"PREFITPOSTTAG_BHADPTDOWN");
    }
  }


  //3a)Fill fat jet info for AntiBtag
  /*if(passSpecificCuts(eventFlag, CutsWithAnti2Btags)){

    if(m_RunMode & RunMode::FILL_FATJET_PROPERTIES) this->FillFatJetProperties(&gbbcand,total_evt_weight*btag_SF_nom,"PREFITANTITAG");

    }*/

  //3b) Fill untagged info, for MC uncertainty calculation
  if(m_isNominal && passSpecificCuts(eventFlag, CutsWithUntag)){
    if(m_RunMode & RunMode::FILL_TRKJET_PROPERTIES) this->FillTrackJetProperties(&gbbcand,total_evt_weight*btag_SF_nom,"PREFITUNTAG");
    if(m_RunMode & RunMode::FILL_FATJET_PROPERTIES) this->FillFatJetProperties(&gbbcand,total_evt_weight*btag_SF_nom,"PREFITUNTAG");
    // SF calculation needs mjpt_PREFITUNTAG so fill this even if the rest of the track-jet
    // properties aren't asked for
    if(m_RunMode & RunMode::FILL_TEMPLATES && !(m_RunMode & RunMode::FILL_TRKJET_PROPERTIES)) {
      m_HistSvc->FastFillTH1D( makeDiJetPlotName(&gbbcand,"mjpt_PREFITUNTAG"),
       ";muon-jet p_{T} [GeV];Events/2 GeV;",
       this->trkjet_pt->at(gbbcand.ind_mj)/1e3,250,0.,500.,total_evt_weight);
      m_HistSvc->FastFillTH1D( makeInclDiJetPlotName(&gbbcand,"mjpt_PREFITUNTAG"),
       ";muon-jet p_{T} [GeV];Events/2 GeV;",
       this->trkjet_pt->at(gbbcand.ind_mj)/1e3,250,0.,500.,total_evt_weight);
    }

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

    if(m_isNominal) {
      fffit_fact = m_FlavFracCorrector->GetCorrectionFactorNom(dijet_flav,
                    this->trkjet_pt->at(gbbcand.ind_mj)/1e3,
                    this->trkjet_pt->at(gbbcand.ind_nmj)/1e3);
    } else {
      fffit_fact = m_FlavFracCorrector->GetCorrectionFactorSys(dijet_flav,
                    this->trkjet_pt->at(gbbcand.ind_mj)/1e3,
                    this->trkjet_pt->at(gbbcand.ind_nmj)/1e3,
                    m_SysVarName);
    }

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

void GbbTupleAna::GetFilterType(TString url){
  if(m_Debug) std::cout<<"url: " <<url<<std::endl;
  if(url.Contains("3610")) m_FilterType="Inclusive";
  if(url.Contains("42700")||url.Contains("42710")) m_FilterType="MuFiltered";
  else m_FilterType="Unknown";
}

void GbbTupleAna::setReweightHisto(TString filename, TString trigger_passed){

  TH2D *hist;

  TString hist_name="h_reweight_trigjet_pt_eta_"+trigger_passed;
  //TString hist_name="reweight_trigfatjet_pt_eta_"+trigger_passed;

  std::cout<<"Looking for hist: "<<hist_name<<std::endl;

  TFile* file=TFile::Open(PathResolverFindCalibFile(filename.Data()).data(),"READ");
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

std::vector<GbbCandidate> GbbTupleAna::constructGbbCandidates(bool useLeading){

  std::vector<GbbCandidate> cands;

  for (unsigned int i_fj=0; i_fj < this->fat_pt->size(); i_fj++) {

    // fatjet must pass pt-eta cuts and have 2 trackjets
    if (!passR10CaloJetCuts(i_fj)) continue;
    if (this->fat_assocTrkjet_ind->at(i_fj).size() < 2) continue;

    // pairs contain trackjet index and associated muon index (or 999 if no muon)
    std::vector<std::pair<unsigned int,std::vector<unsigned int>>> trkjet_info;

    for (unsigned int i_tj=0; i_tj < this->fat_assocTrkjet_ind->at(i_fj).size(); i_tj++) {
      // find location of associated trackjet in trackjet vector
      int ind_tj = getAssocObjIndex(this->trkjet_ind,this->fat_assocTrkjet_ind->at(i_fj).at(i_tj));

      // trackjet didn't pass selection in CxAODFramework (Tuple Maker)
      if (ind_tj < 0) continue;
      // trackjet pt-eta cuts
      if(!(this->passR2TrackJetCuts(ind_tj))) continue;

      std::vector<unsigned int> ind_mus;
      for (int i_mu=0; i_mu < this->trkjet_assocMuon_n->at(ind_tj); i_mu++) {
        int ind_mu = this->trkjet_assocMuon_index->at(ind_tj).at(i_mu);
        if (passMuonSelection(ind_mu)) {
          ind_mus.push_back(ind_mu);
        }
      }
      trkjet_info.push_back(std::make_pair(ind_tj,ind_mus));
    }

    // need 2 trackjets that pass selection
    if (trkjet_info.size() < 2) continue;

    GbbCandidate gbbcand;
    gbbcand.ind_fj = i_fj;
    gbbcand.ind_mj = 999;
    gbbcand.ind_mj_mu = 999;
    gbbcand.ind_nmj = 999;
    gbbcand.ind_nmj_mu = 999;
    gbbcand.nTruthMuons = 0;
    gbbcand.nRecoMuons = 0;
    gbbcand.hasLeadTrkJets = false;

    if (useLeading) {
      gbbcand.ind_mj = trkjet_info[0].first;
      gbbcand.ind_mj_mu = 999;
      if (trkjet_info[0].second.size() > 0) {
        gbbcand.nRecoMuons++;
        gbbcand.ind_mj_mu = trkjet_info[0].second.at(0);
        for (unsigned int ind_mu : trkjet_info[0].second) {
          if (this->muo_hasTruth->at(ind_mu)) {
            gbbcand.nTruthMuons++;
            break; // only care if there is >1 truth muon per trkjet
          }
        }
      }
      gbbcand.ind_nmj = trkjet_info[1].first;
      gbbcand.ind_nmj_mu = 999;
      if (trkjet_info[1].second.size() > 0) {
        gbbcand.nRecoMuons++;
        gbbcand.ind_nmj_mu = trkjet_info[1].second.at(0);
        for (unsigned int ind_mu : trkjet_info[1].second) {
          if (this->muo_hasTruth->at(ind_mu)) {
            gbbcand.nTruthMuons++;
            break; // only care if there is >1 truth muon per trkjet
          }
        }
      }
    } else {
      for (std::pair<unsigned int,std::vector<unsigned int>> inds : trkjet_info) {
        // if trackjet has muon then try to set muon-jet indices
        if (inds.second.size() != 0) {
          if (gbbcand.ind_mj == 999) {
            gbbcand.ind_mj = inds.first;
            gbbcand.ind_mj_mu = inds.second.at(0);
            gbbcand.nRecoMuons++;
            for (unsigned int ind_mu : inds.second) {
              if (this->muo_hasTruth->at(ind_mu)) {
                gbbcand.nTruthMuons++;
                break; // only care if there is >1 truth muon per trkjet
              }
            }
          // muon-jet already set, so set non-muon-jet instead
          } else if (gbbcand.ind_nmj == 999) {
            gbbcand.ind_nmj = inds.first;
            gbbcand.ind_nmj_mu = inds.second.at(0);
            gbbcand.nRecoMuons++;
            for (unsigned int ind_mu : inds.second) {
              if (this->muo_hasTruth->at(ind_mu)) {
                gbbcand.nTruthMuons++;
                break; // only care if there is >1 truth muon per trkjet
              }
            }
          }
        } else {
          if (gbbcand.ind_nmj == 999) {
            gbbcand.ind_nmj = inds.first;
          }
        }
        // Stop when both muon and non-muon jets are set
        if (gbbcand.ind_mj != 999 && gbbcand.ind_nmj != 999) break;
      }
    }

    // double-check that both muon and non-muon indices are set
    if (gbbcand.ind_mj == 999 || gbbcand.ind_nmj == 999) continue;

    // check if muon-jet is among the first two
    if (gbbcand.ind_mj == trkjet_info[0].first || gbbcand.ind_mj == trkjet_info[1].first) {
      gbbcand.hasLeadTrkJets = true;
    }

    cands.push_back(gbbcand);
  }

  return cands;
}

float GbbTupleAna::getTrigJetWeight(int i_trig_jet,TString trigger_passed){

  return m_reweightHistos[trigger_passed].get()->GetBinContent(m_reweightHistos[trigger_passed].get()->FindBin(this->jet_pt->at(i_trig_jet)/1e3,this->jet_eta->at(i_trig_jet)));

}

float GbbTupleAna::getTrigFatJetWeight(float trigfjpt, float trigfjeta,TString trigger_passed){

  return m_reweightHistos[trigger_passed].get()->GetBinContent(m_reweightHistos[trigger_passed].get()->FindBin(trigfjpt/1e3,trigfjeta));

}

trkjetSd0Info GbbTupleAna::getTrkjetAssocSd0Info(unsigned int i_jet, bool doSmeared, TString d0sys, TString z0sys, int n){

  int tracks_passed=0;
  TLorentzVector jet,trk;
  jet.SetPtEtaPhiM(this->trkjet_pt->at(i_jet),this->trkjet_eta->at(i_jet),this->trkjet_phi->at(i_jet),0);
  std::vector<track> tracks;
  trkjetSd0Info ret = {-99.,-99.,-99.,-99.,-99., -99.,-99.,-99.,-99.,
                       -99.,-99.,-99.,-99., -99.,-99.,-99.,-99.,
                       -99.,-99.,-99.,
                       -99.,-99.,-99., -99.,-99.,-99., n};

  for(unsigned int i_trk=0; i_trk<this->trkjet_assocTrk_pt->at(i_jet).size(); i_trk++){

    if (m_config->GetIsR20p7()) {
      if(!this->passAssocTrkSelection(i_trk,i_jet)) continue;
    } else {
      if(!this->passR21AssocTrkSelection(i_trk,i_jet)) continue;
    }

    tracks_passed++;
    bool duplicate = false;
    for (track tr : tracks) {
      if (fabs(tr.pt - trkjet_assocTrk_pt->at(i_jet).at(i_trk)) < 1e-5) duplicate = true;
    }
    if (duplicate) {
      std::cout<<"Skipping duplicate track! pt: ";
      std::cout<<trkjet_assocTrk_pt->at(i_jet).at(i_trk)<<std::endl;
      continue;
    }

    track tr;

    tr.d0 = getd0(i_trk,i_jet,doSmeared,d0sys);
    tr.d0err = this->trkjet_assocTrk_d0err->at(i_jet).at(i_trk);
    tr.sd0 = getSd0(i_trk,i_jet,doSmeared,d0sys);
    tr.z0sintheta = (getz0(i_trk,i_jet,doSmeared,z0sys)  - this->eve_PVz
                     +this->trkjet_assocTrk_vz->at(i_jet).at(i_trk)) *
                    TMath::Sin(this->trkjet_assocTrk_theta->at(i_jet).at(i_trk));
    tr.pt=this->trkjet_assocTrk_pt->at(i_jet).at(i_trk);

    trk.SetPtEtaPhiM(this->trkjet_assocTrk_pt->at(i_jet).at(i_trk),this->trkjet_assocTrk_eta->at(i_jet).at(i_trk),this->trkjet_assocTrk_phi->at(i_jet).at(i_trk),0);
    tr.dr=jet.DeltaR(trk);

    tracks.push_back(tr);
  } // End loop over tracks

  if (tracks.size() < 3) return ret;
  std::sort(tracks.begin(),tracks.end(),by_abs_sd0());
  ret.maxSd0 = tracks.at(0).sd0;
  ret.maxSd0_dR = tracks.at(0).dr;
  ret.maxd0 = tracks.at(0).d0;
  ret.maxd0err = tracks.at(0).d0err;

  ret.subSd0 = tracks.at(1).sd0;
  ret.subd0 = tracks.at(1).d0;
  ret.subd0err = tracks.at(1).d0err;

  if (tracks.size() > 2) {
    ret.thirdSd0 = tracks.at(2).sd0;
    ret.thirdd0 = tracks.at(2).d0;
    ret.thirdd0err = tracks.at(2).d0err;
  }

  // Calculate averages over min(n, n_tracks) tracks
  int N = TMath::Min(n,(int)tracks.size());
  float sum=0.;
  float sum_d0=0.;
  for(int i=0; i<N; i++){
    //std::cout<<"track "<<i<<": pT"<<tracks.at(i).pt<<std::endl;
    //float d0=tracks.at(i).d0;
    float sd0=tracks.at(i).sd0;
    //sum+=sd0<0 ? -1.*TMath::Abs(d0) : TMath::Abs(d0);
    sum+=sd0;
    sum_d0+=tracks.at(i).d0;
  }
  ret.meanSd0_sd0 = sum/N;
  ret.meand0_sd0 = sum_d0/N;

  std::sort(tracks.begin(),tracks.end(),by_pt());
  sum=0.;
  sum_d0=0.;
  for(int i=0; i<N; i++){
    //std::cout<<"track "<<i<<": pT"<<tracks.at(i).pt<<std::endl;
    //float d0=tracks.at(i).d0;
    float sd0=tracks.at(i).sd0;
    //sum+=sd0<0 ? -1.*TMath::Abs(d0) : TMath::Abs(d0);
    sum+=sd0;
    sum_d0+=tracks.at(i).d0;
  }
  ret.meanSd0_pt = sum/N;
  ret.meand0_pt = sum_d0/N;

  ret.maxSd0_pt = tracks.at(0).sd0;
  ret.maxd0_pt = tracks.at(0).d0;
  ret.max_z0sintheta_pt = tracks.at(0).z0sintheta;
  ret.maxd0err_pt = tracks.at(0).d0err;

  ret.subSd0_pt = tracks.at(1).sd0;
  ret.subd0_pt = tracks.at(1).d0;
  ret.sub_z0sintheta_pt = tracks.at(1).z0sintheta;
  ret.subd0err_pt = tracks.at(1).d0err;

  if (tracks.size() > 2) {
    ret.thirdSd0_pt = tracks.at(2).sd0;
    ret.thirdd0_pt = tracks.at(2).d0;
    ret.third_z0sintheta_pt = tracks.at(2).z0sintheta;
    ret.thirdd0err_pt = tracks.at(2).d0err;
  }

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

float GbbTupleAna::getz0(unsigned int i_trk, unsigned int i_jet, bool doSmeared, TString sys){

  float z0=this->trkjet_assocTrk_z0->at(i_jet).at(i_trk);
  //std::cout<<"d0 is: "<<d0<<std::endl;
  if (doSmeared && this->eve_isMC) {
    if(sys.EqualTo("nominal")) z0=this->trkjet_assocTrk_z0_smear->at(i_jet).at(i_trk);
    else if(sys.EqualTo("up")) z0=this->trkjet_assocTrk_z0_smear_up->at(i_jet).at(i_trk);
    else if(sys.EqualTo("down")) z0=this->trkjet_assocTrk_z0_smear_down->at(i_jet).at(i_trk);
    else std::cout<<"ERROR: You have to specify if you want nominal or sys smeared z0!"<<std::endl;
  }
  //std::cout<<"smeared z0 is: "<<z0<<std::endl;
  return z0;
}

float GbbTupleAna::getSd0(unsigned int i_trk, unsigned int i_jet, bool doSmeared, TString sys){

  TLorentzVector jet;
  jet.SetPtEtaPhiM(this->trkjet_pt->at(i_jet),this->trkjet_eta->at(i_jet),this->trkjet_phi->at(i_jet),0.);

  float d0 = getd0(i_trk,i_jet,doSmeared,sys);
  // Add beamspot uncertainty to d0 error calculation
  // Beamspot slightly wider in MC than data (makes agreement worse)
  //float sin_phi = TMath::Sin(trkjet_assocTrk_phi0->at(i_jet).at(i_trk));
  //float cos_phi = TMath::Cos(trkjet_assocTrk_phi0->at(i_jet).at(i_trk));
  //float beam_unc2 = sin_phi * ( sin_phi * eve_beam_sigma_x*eve_beam_sigma_x
  //                             -cos_phi * eve_beam_sigma_xy)
  //                 +cos_phi * ( cos_phi * eve_beam_sigma_y*eve_beam_sigma_y
  //                             -sin_phi * eve_beam_sigma_xy);
  //float sd0=d0 / TMath::Sqrt(trkjet_assocTrk_d0err->at(i_jet).at(i_trk)*
  //                 trkjet_assocTrk_d0err->at(i_jet).at(i_trk) + beam_unc2);
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

  if(this->trkjet_hasHadMatInt->at(gbbcand->ind_mj) || this->trkjet_hasHadMatInt->at(gbbcand->ind_nmj)) hasHadMatInt=true;

  if(this->trkjet_hasConversion->at(gbbcand->ind_mj) || this->trkjet_hasConversion->at(gbbcand->ind_nmj)) hasConversion=true;

  if(this->trkjet_hasKShort->at(gbbcand->ind_mj) || this->trkjet_hasKShort->at(gbbcand->ind_nmj) || this->trkjet_hasLambda->at(gbbcand->ind_mj) || this->trkjet_hasLambda->at(gbbcand->ind_nmj)) hasLightLongLived=true;

  if(gbbcand->nTruthMuons == 0) hasNoTruthMuon=true;

}


// a function to update the bit flag
void GbbTupleAna::updateFlag(unsigned long int &flag, const unsigned int cutPosition, const bool passCut) {
  // Put bit passCut (true or false) at position cutPosition
  flag = flag | passCut << cutPosition;
}
