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

	m_HistogramService=new HistogramService();
}

GbbTupleAna::~GbbTupleAna() {
	// TODO Auto-generated destructor stub

	if(m_FlavFracCorrector) delete m_FlavFracCorrector;
	delete m_HistogramService;
}


void GbbTupleAna::ReadConfig(TString &config_path){
 
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

  m_RunMode   = config->GetValue("RunMode",         "FillTemplates");
  std::cout<<"RunMode: "<<m_RunMode<<std::endl;
  
  m_Debug = config->GetValue("doDebug",false);
  std::cout<<"doDebug: "<<m_Debug<<std::endl;
  
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
  
  m_ditrkjet_cat=this->SplitString(config->GetValue("DiTrkjetCategories","BB,BC,BL,CC,CL,LL,other"),',');
  std::cout<<"m_ditrkjet_cat: "<<config->GetValue("DiTrkjetCategories","BB,BC,BL,CC,CL,LL,other")<<std::endl;

  m_doMergeDiTrkjetCat=config->GetValue("doMergeDiTrkjetCat",true);
  std::cout<<"m_doMergeDiTrkjetCat: "<< m_doMergeDiTrkjetCat<<std::endl;

  m_doEvenOddTemplates=config->GetValue("doEvenOddTemplates",false);
  std::cout<<"m_doEvemOddTemplates: "<< m_doEvenOddTemplates<<std::endl;

  m_trkjet_cat=this->SplitString(config->GetValue("TrkjetCategories","B,C,L,other"),',');
  std::cout<<"m_trkjet_cat: "<<config->GetValue("TrkjetCategories","B,C,L,other")<<std::endl;
  
  m_muojet_pt_bins=this->SplitStringD(config->GetValue("MuoJetPtBins","100.,200."),',');
  std::cout<<"m_muojet_pt_bins: "<<config->GetValue("MuoJetPtBins","100.,200.")<<std::endl;
  
  m_nonmuojet_pt_bins=this->SplitStringD(config->GetValue("NonMuoJetPtBins","20.,50.,80."),',');
  std::cout<<"m_nonmuojet_pt_bins: "<<config->GetValue("NonMuoJetPtBins","20.,50.,80.")<<std::endl;

  m_doRandomSplitting=config->GetValue("doRandomSplitting",false);
 std::cout<<"m_doRandomSplitting: "<< m_doRandomSplitting<<std::endl;

 m_fatjet_pt_bins=this->SplitStringD(config->GetValue("FatJetPtBins",""),',');
 std::cout<<"m_fatjet_pt_bins: "<<config->GetValue("FatJetPtBins","")<<std::endl;

 m_doPostfitPtReweighting=config->GetValue("doPostfitPtReweighting",false);
 std::cout<<"m_doPostfitPtReweighting: "<<m_doPostfitPtReweighting<<std::endl;

 m_PostfitPtReweightingFile=config->GetValue("PostfitPtReweightingFile","DEFAULT.root");
 std::cout<<"m_PostfitPtReweightingFile: "<<m_PostfitPtReweightingFile<<std::endl;

}



GbbTupleAna::GbbTupleAna(TString& infilename, TString& treename, TString& outfilename, TString &configname, std::vector<TString>& infilelist) : TupleAna(),m_Debug(false),m_SumWeightTuple(0),m_nevtTuple(0)
{

  TH1::AddDirectory(0);
  //=========================================
  // Read Config
  //=========================================
  
  this->ReadConfig(configname);

  //=========================================
  // Initialize 
  //=========================================
  
  TTree *tree=0;
  TTree *ftree=0;
  TFile *f = 0;
  TChain *tc=0;
  TChain *ftc=0;
  if(infilename.EqualTo("list")){
    tc=new TChain("tc");
    if(!treename.EqualTo("FlavourTagging_Nominal")) ftc=new TChain("FlavourTagging_Nominal");
    for(auto &el : infilelist){
      TString url=el+"/"+treename;

      if(url.Contains("3610") || url.Contains("42700") || url.Contains("42710")) m_GeneratorName="Pythia";
      else if (url.Contains("4260") || url.Contains("42711") ) m_GeneratorName="Herwig";
      else m_GeneratorName="Unknown";

      tc->Add(url.Data());
      if(!treename.EqualTo("FlavourTagging_Nominal")){ //if systematics tree: add nominal as friend to retrieve the truth label (temp patch)
	TString furl=el+"/FlavourTagging_Nominal";
	ftc->Add(furl.Data());
      }
    }
    if(ftc) tc->AddFriend(ftc);
    tree=tc;
    

  }else{
    
    if(infilename.Contains("3610") || infilename.Contains("42700") || infilename.Contains("42710")) m_GeneratorName="Pythia";
    else if (infilename.Contains("4260") || infilename.Contains("42711") ) m_GeneratorName="Herwig";
    else m_GeneratorName="Unknown";

    f=(TFile*)gROOT->GetListOfFiles()->FindObject(infilename.Data());
    if (!f || !f->IsOpen()) {
      f = new TFile(infilename.Data(),"READ");
    }
    f->GetObject(treename.Data(),tree);
    if(!treename.EqualTo("FlavourTagging_Nominal")){//if systematics tree: add nominal as friend to retrieve the truth label (temp patch)
      f->GetObject("FlavourTagging_Nominal",ftree);
      tree->AddFriend(ftree);
      std::cout<<"Adding friend tree: "<<ftree->GetName()<<std::endl;
    }
  }
  
  std::cout<<"Generator is:"<<m_GeneratorName<<std::endl;

  m_Outputname=outfilename;
  
  //running on the nominal sample?
  if(treename.Contains("Nominal")){
    m_isNominal=true;
  }else m_isNominal=false;
  

  //if not, extract systematic variation from treename
  m_SysVarName = !m_isNominal ? treename(15,treename.Length()) : TString("Nom");
  std::cout<<"SysVarName:"<<m_SysVarName<<"!"<<std::endl;
  
  
  if(m_isNominal) std::cout<<"Running on Nominal sample!"<<std::endl;
  else std::cout<<"Running on Systematics sample!"<<std::endl;
  

  m_HistogramService=new HistogramService();
  

  //=========================================
  //Initialize Correction and Smearing
  //=========================================
   
  //Flavour Fraction correction
  if(m_doFlavFracCorrection) m_FlavFracCorrector=new FlavourFracCorrector(m_FlavFracCorrectorFile); 
  else m_FlavFracCorrector=0;
  

  //=========================================
  //Initialize BookKeeping Histograms
  //=========================================
  
  TH1D* metahist=0, *metahist_tmp=0;
  std::cout<<"infilename: "<<infilename<<std::endl;
  if(infilename.EqualTo("list")){
    std::cout<<"list! "<<std::endl;
    for(auto &el : infilelist){
      std::cout<<"looking for file"<<el<<std::endl;
      f=(TFile*)gROOT->GetListOfFiles()->FindObject(el.Data());
      if (!f || !f->IsOpen()) {
	f = new TFile(el.Data(),"READ");	
      }

      if(!metahist){
	f->GetObject("MetaData_EventCount",metahist);
      }	else {
	f->GetObject("MetaData_EventCount",metahist_tmp);
	metahist->Add(metahist_tmp);
      }  
    }

  }else f->GetObject("MetaData_EventCount",metahist);
  if (!metahist) std::cout<<"FATAL: no metadata found!"<<std::endl; 
  
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
   
  Init(tree);
}


void GbbTupleAna::Finalize(){
  
  m_HistogramService->FastFillTH1D("Hist_BookKeeping",6,6,0.5,6.5,m_SumWeightTuple);
  m_HistogramService->FastFillTH1D("Hist_BookKeeping",3,6,0.5,6.5,m_nevtTuple);  
  m_HistogramService->WriteRootFile(m_Outputname);
  
}


void GbbTupleAna::Loop()
{
  if (fChain == 0) return;

  Long64_t nentries = fChain->GetEntriesFast();
  int real_entries = 0;

  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;

    if(!(jentry%1000)) std::cout<<"Processing event: "<<jentry<<" of "<<nentries<<std::endl;
    // if (Cut(ientry) < 0) continue;
    m_nevtTuple++;

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
  m_HistogramService->FastFillTH1D("CutFlow",icut,15,0.5,15.5,total_evt_weight);

  //m_SumWeightTuple+=total_evt_weight;
  m_SumWeightTuple+=this->eve_pu_w;

  double mc_jet_ratio=1.;

  if(!(this->eve_isMC)){

    if(!(this->eve_isCleanEvent && this->eve_passGRL)) return false;
    if(this->jet_pt->size()<2) return false;
    
  }else{ //MC: throw out event with large weights out of Pt range

	  m_HistogramService->FastFillTH1D("truthjet_pt",this->truthjet_pt->at(0)/1e3,100,0.,5000.,total_evt_weight);
#if 0 //FIXME: ask Ruth why this cut exists. Tuples only contain one (R=0.4) jet, is that new?
	  if(this->jet_pt->size()>=2 && this->truthjet_pt->size()){
		  
		  mc_jet_ratio=0.5*(this->jet_pt->at(0)+this->jet_pt->at(1))/this->truthjet_pt->at(0);

		  if(mc_jet_ratio>1.4){ 
		    std::cout<<"Have event with funny pt(reco)/pt(truth) ratio!"<<std::endl;
		    return false;
		  }
		  
	  }else{
	    std::cout<<"Have less than 2 jets"<<std::endl;
	    return false;
	  }
#endif
  }

  if(this->hasBadJet()) return false;

  updateFlag(eventFlag,GbbCuts::EventCleaning,true);

  icut++;
  m_HistogramService->FastFillTH1D("CutFlow",icut,15,0.5,15.5,total_evt_weight);

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

  //TRIGGER MATCHING (leading reco jet must be the leading triger jet);
  /*TLorentzVector trigjet_trlvl,trigjet_reco;
  trigjet_reco.SetPtEtaPhiM(this->jet_pt->at(i_trigjet),this->jet_eta->at(i_trigjet),this->jet_phi->at(i_trigjet),0.);
  trigjet_trlvl.SetPtEtaPhiM(this->trigjet_pt->at(0),this->trigjet_eta->at(0),this->trigjet_phi->at(0),0.);
  if(trigjet_reco.DeltaR(trigjet_trlvl)>0.4) return false;*/

  //Trigger matching (trigger jet == offline jet matched to leading trigger-level jet
  TLorentzVector trigjet_trlvl,jet_reco;
  double DRmin_trigmatch=999.;
  int i_trigjet=-1;
  trigjet_trlvl.SetPtEtaPhiM(this->trigjet_pt->at(0),this->trigjet_eta->at(0),this->trigjet_phi->at(0),0.);
  for(unsigned int i=0; i<this->jet_pt->size(); i++){
    jet_reco.SetPtEtaPhiM(this->jet_pt->at(i),this->jet_eta->at(i),this->jet_phi->at(i),0.);
    if(jet_reco.DeltaR(trigjet_trlvl)<0.4 && jet_reco.DeltaR(trigjet_trlvl)<DRmin_trigmatch && this->passR4CaloJetCuts(i)){
      DRmin_trigmatch=jet_reco.DeltaR(trigjet_trlvl);
      i_trigjet=i;
    }
  }
  if(i_trigjet<0) return false; //no successful trigger match

  //i_trigjet=0;

  //Try to match leading R=0.4 and R=1.0 jets to find what causes the trigger bias
  /*  TLorentzVector largeRjet;
  double ptratio=-1;
  double DR_min=999.;
  double largeRtrigpt=-1, largeRtrigeta=-1;

  for(unsigned int i=0; i<this->fat_pt->size(); i++){
    largeRjet.SetPtEtaPhiM(this->fat_pt->at(i),this->fat_eta->at(i),this->fat_phi->at(i),0.);
    if(trigjet_reco.DeltaR(largeRjet)<1.0 && trigjet_reco.DeltaR(largeRjet)<DR_min){
      ptratio=trigjet_reco.Pt()/largeRjet.Pt();
      largeRtrigpt=largeRjet.Pt();
      largeRtrigeta=largeRjet.Eta();
      DR_min=trigjet_reco.DeltaR(largeRjet);
    }
    }*/

  


  updateFlag(eventFlag,GbbCuts::TriggerJet,true);
  if(m_Debug) std::cout<<"processgbb(): Got Trigger Jet!"<<std::endl;
 
  icut++;
  m_HistogramService->FastFillTH1D("CutFlow",icut,15,0.5,15.5,total_evt_weight);

  //FILL TRIGGER TURNONHISTS
  if(m_RunMode.Contains("TriggerTurnOn")) FillTriggerTurnOnHistograms(i_trigjet,total_evt_weight);


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
  
  double trjet_pt=this->jet_pt->at(i_trigjet);
  double prescale=1.;
  TString trigger_passed="none";
 
  
  /*if(m_RunMode.Contains("TriggerTurnOn")){
    prescale=1.;
  }else if(trjet_pt>500e3 && this->eve_HLT_j380) trigger_passed="HLT_j380";
  else return false;*/


  /*  if(m_RunMode.Contains("TriggerTurnOn")){
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

  /*  if(m_RunMode.Contains("TriggerTurnOn")){
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
  

  if(this->jet_pt->at(i_trigjet)>500e3 && this->eve_HLT_j380){ //demand that full efficiency of turnon curve has been reached
    trigger_passed="HLT_j380";
  }else{
    return false;
  }



  //total_evt_weight*=prescale;

  if(m_Debug) std::cout<<"processgbb(): Passed trigger jet requirement"<<std::endl;

  icut++;
  m_HistogramService->FastFillTH1D("CutFlow",icut,15,0.5,15.5,total_evt_weight);

  //FILL REWEIGHT HISTOGRAMS
  if(m_isNominal && m_RunMode.Contains("FillReweightHists")){
    //FIXME: do we want to trigger on large-R jet?
    this->FillReweightInfo(i_trigjet,total_evt_weight,trigger_passed);
    //this->FillFatReweightInfo(largeRtrigpt,largeRtrigeta,total_evt_weight,trigger_passed);
  }

  //Fill Pileup reweighting histograms
  if(m_isNominal && m_RunMode.Contains("FillPRWHists")) m_HistogramService->FastFillTH1D("h"+m_SysVarName+"_evemu",this->eve_mu,12,0.,60.,total_evt_weight);

  // TRIGGER JET REWEIGHTING
  float trig_weight;
  if(m_doJetPtReweighting && !m_RunMode.Contains("FillReweightHists")  && m_reweightHistos[trigger_passed].get() && this->eve_isMC){
    trig_weight=this->getTrigJetWeight(i_trigjet,trigger_passed);
    //std::cout<<"Trigger weight is:"<<trig_weight<<std::endl;
    //float trig_weight=this->getTrigFatJetWeight(largeRtrigpt,largeRtrigeta,trigger_passed);

    if(TMath::Abs(trig_weight)<1e-10) trig_weight=1.; //avoid weights of zero //FIXME:shouldn't this set trig_weight to 1e-10?
    total_evt_weight*=trig_weight; //Temporary!!! put back in

  }

  // std::cout<<"prescale: "<<prescale<<" trig weight: "<<trig_weight<<std::endl;

   icut++;
   m_HistogramService->FastFillTH1D("CutFlow",icut,15,0.5,15.5,total_evt_weight);

  //=========================================                                                                                               
  //3.) Gbb candidate selection & definition                                                                                                              
  //=========================================

  if(m_Debug) std::cout<<"processgbb(): Constructing Gbb candidate..."<<std::endl;

  GbbCandidate gbbcand=constructGbbCandidate();

  if(gbbcand.fat_index==999){
    if(m_Debug) std::cout<<"processgbb(): No gbb candidate found"<<std::endl;
    return false;
  }

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
  
  TString ptlabel=getPtLabel(muojet_pt/1e3,nonmuojet_pt/1e3);

  if(m_Debug) std::cout<<"processgbb(): Got labels."<<std::endl;

  icut++;
  m_HistogramService->FastFillTH1D("CutFlow",icut,15,0.5,15.5,total_evt_weight);

  double gbbcandpt=this->fat_pt->at(gbbcand.fat_index);
  //  std::cout<<"trigjet pt is: "<<this->jet_pt->at(i_trigjet)<<" "<<gbbcandpt<<std::endl;

  //cut away region where jet bias is introduced
  if(gbbcandpt<500e3) return false;

  /*  if(gbbcandpt>250e3 && gbbcandpt<=280e3 && trigger_passed.EqualTo("HLT_j150")){
  }else if(gbbcandpt>280e3 && gbbcandpt<=380e3 && trigger_passed.EqualTo("HLT_j175")){
  }else if(gbbcandpt>380e3 && gbbcandpt<=500e3 && trigger_passed.EqualTo("HLT_j260")){
  }else if(gbbcandpt>500e3 && trigger_passed.EqualTo("HLT_j380")){
  }else{
    std::cout<<"Event discarded: pt(R=0.4): "<<trjet_pt<<" pt(R=1.0): "<<gbbcandpt<<std::endl;  
    return false;
    }*/

  //FILL REWEIGHT HISTOGRAMS
  //if(m_isNominal && m_RunMode.Contains("FillReweightHists")) this->FillReweightInfo(i_trigjet,total_evt_weight,trigger_passed);


  //=========================================                                                                                               
  //4.) Tracks passing requirements for templates                                                                                                                
  //=========================================    

  //float muojet_maxsd0=this->getTrkjetAssocTrkMaxSd0(gbbcand.muojet_index,m_doTrackSmearing,"nominal");
  //float nonmuojet_maxsd0=this->getTrkjetAssocTrkMaxSd0(gbbcand.nonmuojet_index,m_doTrackSmearing,"nominal"); 

  //calculate Mean sd0 for leading three tracks
//  float muojet_maxsd0=this->getTrkjetAssocTrkMeand0(gbbcand.muojet_index,m_doTrackSmearing,"nominal",3);
//  float nonmuojet_maxsd0=this->getTrkjetAssocTrkMeand0(gbbcand.nonmuojet_index,m_doTrackSmearing,"nominal",3); 

  trkjetSd0Info  muojet_sd0Info=this->getTrkjetAssocSd0Info(gbbcand.muojet_index,m_doTrackSmearing,"nominal",3);
  trkjetSd0Info  nonmuojet_sd0Info=this->getTrkjetAssocSd0Info(gbbcand.nonmuojet_index,m_doTrackSmearing,"nominal",3);
  float muojet_maxsd0 = muojet_sd0Info.meanSd0_ptSort;
  float nonmuojet_maxsd0 = nonmuojet_sd0Info.meanSd0_ptSort;

  if(TMath::Abs(muojet_maxsd0+99.)<1e-5 || TMath::Abs(nonmuojet_maxsd0+99.)<1e-5) return false; //associated tracks do not fulfil selection cuts  

  updateFlag(eventFlag,GbbCuts::GoodSd0Tracks,true);

  icut++;
  m_HistogramService->FastFillTH1D("CutFlow",icut,15,0.5,15.5,total_evt_weight);


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

  icut++;
  m_HistogramService->FastFillTH1D("CutFlow",icut,15,0.5,15.5,total_evt_weight);

  //=========================================
  //6.) Trigjet reweighting
  //=========================================  


  //  //If mode is: FillReweightHists Fill possible new reweighting histogram before event_weight is changed
  //if(m_isNominal && m_RunMode.Contains("FillReweightHists")) this->FillReweightInfo(i_trigjet,total_evt_weight,trigger_passed);


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
    
    if(m_RunMode.Contains("FillTemplates")) FillTemplates(&gbbcand,total_evt_weight);
    if(m_RunMode.Contains("FillTrackJetProperties")) FillTrackJetProperties(&gbbcand,total_evt_weight);
    if(m_RunMode.Contains("FillFatJetProperties")) FillFatJetProperties(&gbbcand,total_evt_weight);
    if(m_RunMode.Contains("FillAdvancedProperties")) FillAdvancedProperties(&gbbcand,i_trigjet,total_evt_weight);
    if(m_RunMode.Contains("FillMCStatsInfo")) FillMCStatsInfo(&gbbcand);


    //for crosscheck: Fill Even and Odd Templates
    if(this->eve_isMC && m_doEvenOddTemplates && m_RunMode.Contains("FillTemplates")){
      if(!(i_evt%2)) this->FillTemplates(&gbbcand,total_evt_weight,"EVEN");
      if(!(i_evt%2)) this->FillTemplates(&gbbcand,total_evt_weight,"ODD");		  
    }
    
    m_HistogramService->FastFillTH1D("h"+dijet_name+m_SysVarName+"_"+ptlabel+"_trjpt",trjet_pt/1e3,250,0.,1000.,total_evt_weight);
    m_HistogramService->FastFillTH1D("h"+dijet_name+m_SysVarName+"_"+ptlabel+"_DRditrkjetfatjet",DRditrkjetfatjet,250,0.,0.5,total_evt_weight);
    if(m_isNominal) m_HistogramService->FastFillTH2D("h_DRditrkjetfatjetVSfjpt",DRditrkjetfatjet,fatjet.Pt()/1e3,25,0.,0.5,50,0.,1000.,total_evt_weight);

    m_HistogramService->FastFillTH1D("h"+dijet_name+m_SysVarName+"_"+ptlabel+"_trjptgbbcandratio",trjet_pt/fatjet.Pt(),50,0.,10.,total_evt_weight);

    //m_HistogramService->FastFillTH1D("h"+dijet_name+m_SysVarName+"_"+ptlabel+"_trjptfjptratio",ptratio,50,-1.,1.,total_evt_weight);

    if(m_isNominal){
      m_HistogramService->FastFillTH2D("h_2DtrjptVsfjpt",trjet_pt/1e3,fatjet.Pt()/1e3,100,0.,1000.,100,0.,1000.,total_evt_weight);
      m_HistogramService->FastFillTH2D("h"+dijet_name+"_2DmjptVsnmjpt",muojet_pt/1e3,nonmuojet_pt/1e3,100,0.,1000.,100,0.,1000.,total_evt_weight);
    }    


    TLorentzVector smallRJet;
    if(i_sublsmallRjet>=0){
      smallRJet.SetPtEtaPhiE(this->jet_pt->at(i_sublsmallRjet),this->jet_eta->at(i_sublsmallRjet),this->jet_phi->at(i_sublsmallRjet),this->jet_E->at(i_sublsmallRjet));
      if(smallRJet.DeltaR(trigjet)>1.5) m_HistogramService->FastFillTH1D("h"+dijet_name+m_SysVarName+"_"+ptlabel+"_slR4jpt",trjet_pt/1e3,250,0.,1000.,total_evt_weight);
    }

    m_HistogramService->FastFillTH1D("h"+dijet_name+m_SysVarName+"_"+ptlabel+"_evemu",this->eve_mu,60,0.,60.,total_evt_weight);
    
    m_HistogramService->FastFillTH1D("h"+dijet_name+m_SysVarName+"_"+ptlabel+"_srjN",this->jet_pt->size(),10,0.,10.,total_evt_weight);

    for(int i=0; i<this->jet_pt->size(); i++){
      m_HistogramService->FastFillTH1D("h"+dijet_name+m_SysVarName+"_"+ptlabel+"_allsrjpt",this->jet_pt->at(i)/1e3,250,0.,1000.,total_evt_weight);
    }

  }
  
  //2.) Before Fit and after b-tagging  
  float btag_SF_nom=1., btag_SF_up=1., btag_SF_down=1.; 
  if(this->eve_isMC && m_doApplyBTaggingSF) this->getBtagSFWeights(btag_SF_nom,btag_SF_up,btag_SF_down);

  if(passSpecificCuts(eventFlag, CutsWith2Btags)){  
    //std::cout<<"Filling post-tag plots for"<<m_SysVarName<<"with btag_SF :"<<btag_SF_nom<<std::endl;
    
    if(m_RunMode.Contains("FillTemplates") && !m_RunMode.Contains("ForFitOnly")) this->FillTemplates(&gbbcand,total_evt_weight*btag_SF_nom,"PREFITPOSTTAG");
    if(m_RunMode.Contains("FillTrackJetProperties")) this->FillTrackJetProperties(&gbbcand,total_evt_weight*btag_SF_nom,"PREFITPOSTTAG");
    if(m_RunMode.Contains("FillFatJetProperties")) this->FillFatJetProperties(&gbbcand,total_evt_weight*btag_SF_nom,"PREFITPOSTTAG");
    if(m_RunMode.Contains("FillMCStatsInfo")) FillMCStatsInfo(&gbbcand,"PREFITPOSTTAG");

    if(m_isNominal){
      
      if(m_RunMode.Contains("FillTemplates") && !m_RunMode.Contains("ForFitOnly")) this->FillTemplates(&gbbcand,total_evt_weight*btag_SF_up,"PREFITPOSTTAG_BTAGUP");
      if(m_RunMode.Contains("FillTrackJetProperties")) this->FillTrackJetProperties(&gbbcand,total_evt_weight*btag_SF_up,"PREFITPOSTTAG_BTAGUP");
      if(m_RunMode.Contains("FillFatJetProperties")) this->FillFatJetProperties(&gbbcand,total_evt_weight*btag_SF_up,"PREFITPOSTTAG_BTAGUP");

      if(m_RunMode.Contains("FillTemplates") && !m_RunMode.Contains("ForFitOnly")) this->FillTemplates(&gbbcand,total_evt_weight*btag_SF_down,"PREFITPOSTTAG_BTAGDOWN");
      if(m_RunMode.Contains("FillTrackJetProperties")) this->FillTrackJetProperties(&gbbcand,total_evt_weight*btag_SF_down,"PREFITPOSTTAG_BTAGDOWN");
      if(m_RunMode.Contains("FillFatJetProperties")) this->FillFatJetProperties(&gbbcand,total_evt_weight*btag_SF_down,"PREFITPOSTTAG_BTAGDOWN");

      
    }

    m_HistogramService->FastFillTH1D("h"+dijet_name+m_SysVarName+"_"+ptlabel+"_trjpt_PREFITPOSTTAG",trjet_pt/1e3,250,0.,1000.,total_evt_weight);
    m_HistogramService->FastFillTH1D("h"+dijet_name+m_SysVarName+"_"+ptlabel+"_DRditrkjetfatjet_PREFITPOSTTAG",DRditrkjetfatjet,250,0.,0.5,total_evt_weight);
    


  }


  //3a)Fill fat jet info for AntiBtag
  /*if(passSpecificCuts(eventFlag, CutsWithAnti2Btags)){  
  
    if(m_RunMode.Contains("FillFatJetProperties")) this->FillFatJetProperties(&gbbcand,total_evt_weight*btag_SF_nom,"PREFITANTITAG");

    }*/

  //3b) Fill untagged info, for MC uncertainty calculation
  if(m_isNominal && passSpecificCuts(eventFlag, CutsWithUntag)){
    if(m_RunMode.Contains("FillTrackJetProperties")) this->FillTrackJetProperties(&gbbcand,total_evt_weight*btag_SF_nom,"PREFITUNTAG");
     if(m_RunMode.Contains("FillFatJetProperties")) this->FillFatJetProperties(&gbbcand,total_evt_weight*btag_SF_nom,"PREFITUNTAG");

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

bool GbbTupleAna::hasBadJet(){

  for(unsigned int i=0; i<this->jet_pt->size(); i++){
    
    if(!(this->jet_clean->at(i))) return true;

  }


  return false;
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
    
    if(this->fat_trkjet_assocTrkjet_ind->at(i_jet).size()<2){
	    //if(m_Debug) std::cout<<"constructGbbCandidate(): Fat Jet has less than 2 associated track jets"<<std::endl;
	    continue; 
    }

    int assocTJ_ind=-1;
    
    std::vector<unsigned int> nonmuon_cand_index;
    
    muonjet_index=999;
    nonmuonjet_index=999;
  
    for(unsigned int i=0; i<this->fat_trkjet_assocTrkjet_ind->at(i_jet).size(); i++){
      
      assocTJ_ind=this->getAssocObjIndex(this->trkjet_ind,fat_trkjet_assocTrkjet_ind->at(i_jet).at(i)); //find position of associated track jet in ntup vector (add. selection applied after association)

      if(assocTJ_ind<0){
	if(m_Debug) std::cout<<"constructGbbCandidate(): Track Jet did not pass cuts!"<<std::endl;
	continue; //track jet didn't pass selection in CxAODFramework (Tuple Maker)
      }
     
    
      //Track jet selection cuts already applied in CxAODFramework (Tuple Maker)
      if(!(this->passR2TrackJetCuts(assocTJ_ind))) continue;
      // else continue;

      //if(muonjet_index != 999){ //next leading pt track jet is non-muon jet
      //nonmuonjet_index=assocTJ_ind;
      //	break;
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

GbbCandidate GbbTupleAna::constructGbbCandidateAlternative(){

  GbbCandidate gbbcand;
  gbbcand.fat_pt=0.;
  gbbcand.muojet_index=999;
  gbbcand.nonmuojet_index=999;
  gbbcand.fat_index=999;


  unsigned int muonjet_index=999, nonmuonjet_index=999;

  for(unsigned int i_jet=0; i_jet<this->fat_pt->size(); i_jet++){
    
    if(!passR10CaloJetCuts(i_jet)) continue;
    
    if(this->fat_trkjet_assocTrkjet_ind->at(i_jet).size()<2){
	    //if(m_Debug) std::cout<<"constructGbbCandidate(): Fat Jet has less than 2 associated track jets"<<std::endl;
	    continue; 
    }
    int assocTJ_ind=-1;
    
    std::vector<unsigned int> nonmuon_cand_index;
    
    muonjet_index=999;
    nonmuonjet_index=999;
  
    for(unsigned int i=0; i<this->fat_trkjet_assocTrkjet_ind->at(i_jet).size(); i++){
      
      assocTJ_ind=this->getAssocObjIndex(this->trkjet_ind,fat_trkjet_assocTrkjet_ind->at(i_jet).at(i)); //find position of associated track jet in ntup vector (add. selection applied after association)

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



float GbbTupleAna::getTrigJetWeight(int i_trig_jet,TString trigger_passed){

	return m_reweightHistos[trigger_passed].get()->GetBinContent(m_reweightHistos[trigger_passed].get()->FindBin(this->jet_pt->at(i_trig_jet)/1e3,this->jet_eta->at(i_trig_jet)));

}

float GbbTupleAna::getTrigFatJetWeight(float trigfjpt, float trigfjeta,TString trigger_passed){

	return m_reweightHistos[trigger_passed].get()->GetBinContent(m_reweightHistos[trigger_passed].get()->FindBin(trigfjpt/1e3,trigfjeta));

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

  m_HistogramService->FastFillTH1D("h"+dijet_hist_name+m_SysVarName+"_"+ptlabel+"_mjpt"+nametag,this->trkjet_pt->at(gbbcand->muojet_index)/1e3,250,0.,500.,event_weight);
  m_HistogramService->FastFillTH1D("h"+dijet_hist_name+m_SysVarName+"_"+ptlabel+"_nmjpt"+nametag,this->trkjet_pt->at(gbbcand->nonmuojet_index)/1e3,250,0.,500.,event_weight);
  
  m_HistogramService->FastFillTH1D("h"+dijet_hist_name+m_SysVarName+"_"+ptlabel+"_mjeta"+nametag,this->trkjet_eta->at(gbbcand->muojet_index)/1e3,100,-2.5,2.5,event_weight);
  m_HistogramService->FastFillTH1D("h"+dijet_hist_name+m_SysVarName+"_"+ptlabel+"_nmjeta"+nametag,this->trkjet_eta->at(gbbcand->nonmuojet_index)/1e3,100,-2.5,2.5,event_weight);

 
}



void GbbTupleAna::FillTemplates(GbbCandidate* gbbcand, float event_weight,TString nametag){
  
  if(!nametag.IsNull())nametag.Prepend("_");

  int muojet_truth=this->trkjet_truth->at(gbbcand->muojet_index);
  int nonmuojet_truth=this->trkjet_truth->at(gbbcand->nonmuojet_index);
  
  //inclusive in Pt
  TString hist_name=this->eve_isMC ? m_ditrkjet_cat.at(this->getCategoryNumber(muojet_truth,nonmuojet_truth,m_doMergeDiTrkjetCat)) : TString("Data");

  //float muojet_maxsd0=this->getTrkjetAssocTrkMaxSd0(gbbcand->muojet_index, m_doTrackSmearing,"nominal");
  //float nonmuojet_maxsd0=this->getTrkjetAssocTrkMaxSd0(gbbcand->nonmuojet_index,m_doTrackSmearing,"nominal"); 

  //calculate mean sd0 for leading three tracks
  float muojet_maxsd0=this->getTrkjetAssocTrkMeand0(gbbcand->muojet_index, m_doTrackSmearing,"nominal",3);
  float nonmuojet_maxsd0=this->getTrkjetAssocTrkMeand0(gbbcand->nonmuojet_index,m_doTrackSmearing,"nominal",3);

  trkjetSd0Info  muojet_sd0Info=this->getTrkjetAssocSd0Info(gbbcand->muojet_index,m_doTrackSmearing,"nominal",3);
  trkjetSd0Info  nonmuojet_sd0Info=this->getTrkjetAssocSd0Info(gbbcand->nonmuojet_index,m_doTrackSmearing,"nominal",3);
  //float muojet_maxsd0 = muojet_sd0Info.meanSd0_ptSort;
  //float nonmuojet_maxsd0 = nonmuojet_sd0Info.meanSd0_ptSort;

  //float muojet_maxsd0DR=this->getTrkjetAssocTrkMaxSd0DR(gbbcand->muojet_index, m_doTrackSmearing,"nominal");
  //float nonmuojet_maxsd0DR=this->getTrkjetAssocTrkMaxSd0DR(gbbcand->nonmuojet_index,m_doTrackSmearing,"nominal");
  float muojet_maxsd0DR = muojet_sd0Info.maxSd0_dR;
  float nonmuojet_maxsd0DR = nonmuojet_sd0Info.maxSd0_dR;

  if(TMath::Abs(muojet_maxsd0+99)<1e-5 || TMath::Abs(nonmuojet_maxsd0+99)<1e-5) return; //associated tracks do not fulfil selection cuts


  m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_mjoldSd0"+nametag,muojet_maxsd0,50,-40,80,event_weight);
  m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_nmjoldSd0"+nametag,nonmuojet_maxsd0,50,-40,80,event_weight);
  m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_mjmaxSd0"+nametag,muojet_sd0Info.maxSd0,50,-40,80,event_weight);
  m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_nmjmaxSd0"+nametag,nonmuojet_sd0Info.maxSd0,50,-40,80,event_weight);
  m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_mjmeanSd0"+nametag,muojet_sd0Info.meanSd0_ptSort,50,-40,80,event_weight);
  m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_nmjmeanSd0"+nametag,nonmuojet_sd0Info.meanSd0_ptSort,50,-40,80,event_weight);
  m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_mjsubSd0"+nametag,muojet_sd0Info.subSd0,50,-40,80,event_weight);
  m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_nmjsubSd0"+nametag,nonmuojet_sd0Info.subSd0,50,-40,80,event_weight);
  m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_mjthirdSd0"+nametag,muojet_sd0Info.thirdSd0,50,-40,80,event_weight);
  m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_nmjthirdSd0"+nametag,nonmuojet_sd0Info.thirdSd0,50,-40,80,event_weight);


  //In Pt Bins
  TString ptlabel=this->getPtLabel(this->trkjet_pt->at(gbbcand->muojet_index)/1e3,this->trkjet_pt->at(gbbcand->nonmuojet_index)/1e3);
  
  if(m_doRandomSplitting){
    
    if(m_doFillMujet) m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_"+ptlabel+"_mjmaxSd0"+nametag,muojet_maxsd0,80,-40,80,event_weight);
    else m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_"+ptlabel+"_nmjmaxSd0"+nametag,nonmuojet_maxsd0,80,-40,80,event_weight);
    
  }else{

    m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_"+ptlabel+"_mjmaxSd0"+nametag,muojet_maxsd0,80,-40,80,event_weight);
    m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_"+ptlabel+"_nmjmaxSd0"+nametag,nonmuojet_maxsd0,80,-40,80,event_weight);

    m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_mjmaxSd0DR"+nametag,muojet_maxsd0DR,25,0.,0.5,event_weight);
    m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_nmjmaxSd0DR"+nametag,nonmuojet_maxsd0DR,25,0.,0.5,event_weight);


  }

  //in Fat jet pt bins
  /*TString fatptlabel=this->getFatjetPtLabel(this->fat_pt->at(gbbcand->fat_index)/1e3);
  m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_"+fatptlabel+"_mjmaxSd0"+nametag,muojet_maxsd0,80,-10,10,event_weight);
  m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_"+fatptlabel+"_nmjmaxSd0"+nametag,nonmuojet_maxsd0,80,-10,10,event_weight);
  */
  


  //Fill fat jet histograms for SF calculation
  
  if(m_isNominal){
    
    m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_"+ptlabel+"_fjpt"+nametag,this->fat_pt->at(gbbcand->fat_index)/1e3,250,0.,1000.,event_weight);
    //m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_"+fatptlabel+"_fjpt"+nametag,this->fat_pt->at(gbbcand->fat_index)/1e3,250,0.,1000.,event_weight);
    
    if(this->eve_isMC && this->getScaledFatPt(this->fat_pt->at(gbbcand->fat_index)/1e3)>250.) m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_"+ptlabel+"_fjptsc"+nametag,this->getScaledFatPt(this->fat_pt->at(gbbcand->fat_index)/1e3),250,0.,1000.,event_weight);
    else if(!this->eve_isMC) m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_"+ptlabel+"_fjptsc"+nametag,this->fat_pt->at(gbbcand->fat_index)/1e3,250,0.,1000.,event_weight);
       

    
    if(this->eve_isMC && m_isNominal){
      
      //float muojet_maxsd0_up=this->getTrkjetAssocTrkMaxSd0(gbbcand->muojet_index, m_doTrackSmearing,"up");
      //float nonmuojet_maxsd0_up=this->getTrkjetAssocTrkMaxSd0(gbbcand->nonmuojet_index,m_doTrackSmearing,"up"); //test Ruth 23.3.
      
      //calculate mean sd0 for leading three tracks
      float muojet_maxsd0_up=this->getTrkjetAssocTrkMeand0(gbbcand->muojet_index, m_doTrackSmearing,"up",3);
      float nonmuojet_maxsd0_up=this->getTrkjetAssocTrkMeand0(gbbcand->nonmuojet_index,m_doTrackSmearing,"up",3);
  
  trkjetSd0Info  muojet_sd0Info_up=this->getTrkjetAssocSd0Info(gbbcand->muojet_index,m_doTrackSmearing,"up",3);
  trkjetSd0Info  nonmuojet_sd0Info_up=this->getTrkjetAssocSd0Info(gbbcand->nonmuojet_index,m_doTrackSmearing,"up",3);
 
      m_HistogramService->FastFillTH1D("h"+hist_name+"SD0Smear__1up_"+ptlabel+"_mjmaxSd0"+nametag,muojet_maxsd0,80,-40,80,event_weight);
      m_HistogramService->FastFillTH1D("h"+hist_name+"SD0Smear__1up_"+ptlabel+"_nmjmaxSd0"+nametag,nonmuojet_maxsd0,80,-40,80,event_weight);
      
      //float muojet_maxsd0_down=this->getTrkjetAssocTrkMaxSd0(gbbcand->muojet_index, m_doTrackSmearing,"down");
      //float nonmuojet_maxsd0_down=this->getTrkjetAssocTrkMaxSd0(gbbcand->nonmuojet_index,m_doTrackSmearing,"down"); //test Ruth 23.3.

      //calculate mean sd0 for leading three tracks
      float muojet_maxsd0_down=this->getTrkjetAssocTrkMeand0(gbbcand->muojet_index, m_doTrackSmearing,"down",3);
      float nonmuojet_maxsd0_down=this->getTrkjetAssocTrkMeand0(gbbcand->nonmuojet_index,m_doTrackSmearing,"down",3);
  
  trkjetSd0Info  muojet_sd0Info_down=this->getTrkjetAssocSd0Info(gbbcand->muojet_index,m_doTrackSmearing,"down",3);
  trkjetSd0Info  nonmuojet_sd0Info_down=this->getTrkjetAssocSd0Info(gbbcand->nonmuojet_index,m_doTrackSmearing,"down",3);

      m_HistogramService->FastFillTH1D("h"+hist_name+"SD0SMEAR__1down_"+ptlabel+"_mjmaxSd0"+nametag,muojet_maxsd0,80,-40,80,event_weight);
      m_HistogramService->FastFillTH1D("h"+hist_name+"SD0SMEAR__1down_"+ptlabel+"_nmjmaxSd0"+nametag,nonmuojet_maxsd0,80,-40,80,event_weight);
      
      bool hasConversion=false, hasHadMatInt=false, hasLightLongLived=false, hasNoTruthMu=false;
      this->getSystematicsFlags(gbbcand, hasConversion, hasHadMatInt, hasLightLongLived, hasNoTruthMu);
      if(hasConversion){
	m_HistogramService->FastFillTH1D("h"+hist_name+"Conversion__1up_"+ptlabel+"_mjmaxSd0"+nametag,muojet_maxsd0,80,-40,80,event_weight*1.1);
	m_HistogramService->FastFillTH1D("h"+hist_name+"Conversion__1up_"+ptlabel+"_nmjmaxSd0"+nametag,nonmuojet_maxsd0,80,-40,80,event_weight*1.1);
	m_HistogramService->FastFillTH1D("h"+hist_name+"Conversion__1down_"+ptlabel+"_mjmaxSd0"+nametag,muojet_maxsd0,80,-40,80,event_weight*0.9);
	m_HistogramService->FastFillTH1D("h"+hist_name+"Conversion__1down_"+ptlabel+"_nmjmaxSd0"+nametag,nonmuojet_maxsd0,80,-40,80,event_weight*0.9);	
      }else{
	m_HistogramService->FastFillTH1D("h"+hist_name+"Conversion__1up_"+ptlabel+"_mjmaxSd0"+nametag,muojet_maxsd0,80,-40,80,event_weight);
        m_HistogramService->FastFillTH1D("h"+hist_name+"Conversion__1up_"+ptlabel+"_nmjmaxSd0"+nametag,nonmuojet_maxsd0,80,-40,80,event_weight);
        m_HistogramService->FastFillTH1D("h"+hist_name+"Conversion__1down_"+ptlabel+"_mjmaxSd0"+nametag,muojet_maxsd0,80,-40,80,event_weight);
        m_HistogramService->FastFillTH1D("h"+hist_name+"Conversion__1down_"+ptlabel+"_nmjmaxSd0"+nametag,nonmuojet_maxsd0,80,-40,80,event_weight);
      }

      if(hasHadMatInt){
	m_HistogramService->FastFillTH1D("h"+hist_name+"HadMatInt__1up_"+ptlabel+"_mjmaxSd0"+nametag,muojet_maxsd0,80,-40,80,event_weight*1.1);
	m_HistogramService->FastFillTH1D("h"+hist_name+"HadMatInt__1up_"+ptlabel+"_nmjmaxSd0"+nametag,nonmuojet_maxsd0,80,-40,80,event_weight*1.1);
	m_HistogramService->FastFillTH1D("h"+hist_name+"HadMatInt__1down_"+ptlabel+"_mjmaxSd0"+nametag,muojet_maxsd0,80,-40,80,event_weight*0.9);
	m_HistogramService->FastFillTH1D("h"+hist_name+"HadMatInt__1down_"+ptlabel+"_nmjmaxSd0"+nametag,nonmuojet_maxsd0,80,-40,80,event_weight*0.9);	
      }else{
	m_HistogramService->FastFillTH1D("h"+hist_name+"HadMatInt__1up_"+ptlabel+"_mjmaxSd0"+nametag,muojet_maxsd0,80,-40,80,event_weight);
        m_HistogramService->FastFillTH1D("h"+hist_name+"HadMatInt__1up_"+ptlabel+"_nmjmaxSd0"+nametag,nonmuojet_maxsd0,80,-40,80,event_weight);
        m_HistogramService->FastFillTH1D("h"+hist_name+"HadMatInt__1down_"+ptlabel+"_mjmaxSd0"+nametag,muojet_maxsd0,80,-40,80,event_weight);
        m_HistogramService->FastFillTH1D("h"+hist_name+"HadMatInt__1down_"+ptlabel+"_nmjmaxSd0"+nametag,nonmuojet_maxsd0,80,-40,80,event_weight);
      }

      if(hasLightLongLived){
	m_HistogramService->FastFillTH1D("h"+hist_name+"LightLongLived__1up_"+ptlabel+"_mjmaxSd0"+nametag,muojet_maxsd0,80,-40,80,event_weight*1.1);
	m_HistogramService->FastFillTH1D("h"+hist_name+"LightLongLived__1up_"+ptlabel+"_nmjmaxSd0"+nametag,nonmuojet_maxsd0,80,-40,80,event_weight*1.1);
	m_HistogramService->FastFillTH1D("h"+hist_name+"LightLongLived__1down_"+ptlabel+"_mjmaxSd0"+nametag,muojet_maxsd0,80,-40,80,event_weight*0.9);
	m_HistogramService->FastFillTH1D("h"+hist_name+"LightLongLived__1down_"+ptlabel+"_nmjmaxSd0"+nametag,nonmuojet_maxsd0,80,-40,80,event_weight*0.9);	
      }else{
	m_HistogramService->FastFillTH1D("h"+hist_name+"LightLongLived__1up_"+ptlabel+"_mjmaxSd0"+nametag,muojet_maxsd0,80,-40,80,event_weight);
        m_HistogramService->FastFillTH1D("h"+hist_name+"LightLongLived__1up_"+ptlabel+"_nmjmaxSd0"+nametag,nonmuojet_maxsd0,80,-40,80,event_weight);
        m_HistogramService->FastFillTH1D("h"+hist_name+"LightLongLived__1down_"+ptlabel+"_mjmaxSd0"+nametag,muojet_maxsd0,80,-40,80,event_weight);
        m_HistogramService->FastFillTH1D("h"+hist_name+"LightLongLived__1down_"+ptlabel+"_nmjmaxSd0"+nametag,nonmuojet_maxsd0,80,-40,80,event_weight);
      }

      if(hasNoTruthMu){
	m_HistogramService->FastFillTH1D("h"+hist_name+"FakeMuons__1up_"+ptlabel+"_mjmaxSd0"+nametag,muojet_maxsd0,80,-40,80,event_weight*3.);
	m_HistogramService->FastFillTH1D("h"+hist_name+"FakeMuons__1up_"+ptlabel+"_nmjmaxSd0"+nametag,nonmuojet_maxsd0,80,-40,80,event_weight*3.);
      }else{
	m_HistogramService->FastFillTH1D("h"+hist_name+"FakeMuons__1up_"+ptlabel+"_mjmaxSd0"+nametag,muojet_maxsd0,80,-40,80,event_weight*3.);
        m_HistogramService->FastFillTH1D("h"+hist_name+"FakeMuons__1up_"+ptlabel+"_nmjmaxSd0"+nametag,nonmuojet_maxsd0,80,-40,80,event_weight*3.);
      }


    }

  }
}


void GbbTupleAna::FillMCStatsInfo(GbbCandidate* gbbcand, TString nametag){
  
  if(!nametag.IsNull())nametag.Prepend("_");

  int muojet_truth=this->trkjet_truth->at(gbbcand->muojet_index);
  int nonmuojet_truth=this->trkjet_truth->at(gbbcand->nonmuojet_index);
  
  TString dijet_hist_name=this->eve_isMC ? m_ditrkjet_cat.at(this->getCategoryNumber(muojet_truth,nonmuojet_truth,m_doMergeDiTrkjetCat)) : TString("Data");
  
  m_HistogramService->FastFillTH2D("h_dijetcat_"+dijet_hist_name+"_mjpt_vs_nmjpt_unweighted"+nametag,this->trkjet_pt->at(gbbcand->muojet_index)/1e3,this->trkjet_pt->at(gbbcand->nonmuojet_index)/1e3,50,0.,200.,50,0.,200.,1.);

}



void GbbTupleAna::FillFatJetProperties(GbbCandidate* gbbcand, float event_weight, TString nametag){

  if(!nametag.IsNull())nametag.Prepend("_");
  
  int muojet_truth=this->trkjet_truth->at(gbbcand->muojet_index);
  int nonmuojet_truth=this->trkjet_truth->at(gbbcand->nonmuojet_index);
  
  TString hist_name=this->eve_isMC ? m_ditrkjet_cat.at(this->getCategoryNumber(muojet_truth,nonmuojet_truth,m_doMergeDiTrkjetCat)) : TString("Data");

  //In Pt Bins
  TString ptlabel=this->getPtLabel(this->trkjet_pt->at(gbbcand->muojet_index)/1e3,this->trkjet_pt->at(gbbcand->nonmuojet_index)/1e3);
  
  //inclusive only for pt and eta
  if(m_isNominal) m_HistogramService->FastFillTH1D("hIncl_fjpt"+nametag,this->fat_pt->at(gbbcand->fat_index)/1e3,250,0.,1000.,event_weight);
  if(m_isNominal) m_HistogramService->FastFillTH1D("hIncl_fjeta"+nametag,this->fat_eta->at(gbbcand->fat_index),10,-2.5,2.5,event_weight);

  if(m_isNominal) m_HistogramService->FastFillTH1D("hNom"+hist_name+"_fjpt"+nametag,this->fat_pt->at(gbbcand->fat_index)/1e3,250,0.,1000.,event_weight);
  //if(m_isNominal) m_HistogramService->FastFillTH1D("hNom"+hist_name+"_fjptsc"+nametag,this->getScaledFatPt(this->fat_pt->at(gbbcand->fat_index)/1e3),250,0.,1000.,event_weight);
  if(m_isNominal) m_HistogramService->FastFillTH1D("hNom"+hist_name+"_fjeta"+nametag,this->fat_eta->at(gbbcand->fat_index),10,-2.5,2.5,event_weight);


  if(!m_isNominal){
    m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_"+ptlabel+"_fjpt"+nametag,this->fat_pt->at(gbbcand->fat_index)/1e3,250,0.,1000.,event_weight);

    if(this->eve_isMC && this->getScaledFatPt(this->fat_pt->at(gbbcand->fat_index)/1e3)>250.) m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_"+ptlabel+"_fjptsc"+nametag,this->getScaledFatPt(this->fat_pt->at(gbbcand->fat_index)/1e3),250,0.,1000.,event_weight);
    else if(!this->eve_isMC) m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_"+ptlabel+"_fjptsc"+nametag,this->fat_pt->at(gbbcand->fat_index)/1e3,250,0.,1000.,event_weight);
  
  }else if(nametag.Contains("ANTITAG")) m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_"+ptlabel+"_fjpt"+nametag,this->fat_pt->at(gbbcand->fat_index)/1e3,250,0.,1000.,event_weight);
  m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_"+ptlabel+"_fjD2"+nametag,this->fat_D2->at(gbbcand->fat_index),250,0.,4.5,event_weight);
  m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_"+ptlabel+"_fjtau21"+nametag,this->fat_tau21->at(gbbcand->fat_index),250,0.,0.9,event_weight);
  
  TLorentzVector fatjet;
  fatjet.SetPtEtaPhiE(this->fat_pt->at(gbbcand->fat_index),this->fat_eta->at(gbbcand->fat_index),this->fat_phi->at(gbbcand->fat_index),this->fat_E->at(gbbcand->fat_index));
  
  m_HistogramService->FastFillTH1D("h"+hist_name+m_SysVarName+"_"+ptlabel+"_fjm"+nametag,fatjet.M()/1e3,25,0.,500.,event_weight);

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

float GbbTupleAna::getTrkjetAssocTrkMaxSd0(unsigned int i_jet, bool doSmeared, TString sys){

  //give Sd0 of maximum significant among 3 leading track jets

  float max_sd0=0.,max2_sd0=0.,max3_sd0=0., tmp_sd0=0., ret=0.;
  float tmp_pt=0.,max_pt=0., max2_pt=0., max3_pt=0.;

    int tracks_passed=0;

    TLorentzVector jet,trk;

    float px, py,pz;

  for(unsigned int i_trk=0; i_trk<this->trkjet_assocTrk_pt->at(i_jet).size(); i_trk++){

    if(!this->passAssocTrkSelection(i_trk,i_jet)) continue;

    tracks_passed++;

    if(sys.EqualTo("nominal") && doSmeared)tmp_sd0=getSd0_smeared(i_trk,i_jet);
    else if(sys.EqualTo("up") && doSmeared)tmp_sd0=getSd0_smeared_sys_up(i_trk,i_jet);
    else if(sys.EqualTo("down") && doSmeared)tmp_sd0=getSd0_smeared_sys_up(i_trk,i_jet);
    else if(!doSmeared) tmp_sd0=getSd0(i_trk,i_jet);
    else std::cout<<"ERROR: You have to specify if you want smeared, nominal or sys Sd0!"<<std::endl;

    jet.SetPtEtaPhiM(this->trkjet_pt->at(i_jet),this->trkjet_eta->at(i_jet),this->trkjet_phi->at(i_jet),0);
    
    
    trk.SetPtEtaPhiM(this->trkjet_assocTrk_pt->at(i_jet).at(i_trk),this->trkjet_assocTrk_eta->at(i_jet).at(i_trk),this->trkjet_assocTrk_phi->at(i_jet).at(i_trk),0);

    //std::cout<<"DR(trk,jet) is "<<jet.DeltaR(trk)<<std::endl; 
    //tmp_DR=jet.DeltaR(trk);


    tmp_pt=this->trkjet_assocTrk_pt->at(i_jet).at(i_trk);

    if(tmp_pt>max_pt){
	    max3_pt=max2_pt;
	    max3_sd0=max2_sd0;

	    max2_pt=max_pt;
            max2_sd0=max_sd0;

            max_pt=tmp_pt;
            max_sd0=tmp_sd0;

    }else if(tmp_pt>max2_pt){

	    max3_pt=max2_pt;
            max3_sd0=max2_sd0;

	    max2_pt=tmp_pt;
	    max2_sd0=tmp_sd0;

    }else if(tmp_pt>max3_pt){

	    max3_pt=tmp_pt;
	    max3_sd0=tmp_sd0;
	    
    }

    
  }

  if(tracks_passed<3) return -99;
  
  float val_DR;

  if(TMath::Abs(max_sd0)>TMath::Abs(max2_sd0) && TMath::Abs(max_sd0)>TMath::Abs(max3_sd0)){
    ret=max_sd0;
  }
  else if(TMath::Abs(max2_sd0)>TMath::Abs(max3_sd0)){
    ret=max2_sd0;
  }
  else{
    ret=max3_sd0;
  } 

  return ret;

}

float GbbTupleAna::getTrkjetAssocTrkMeand0(unsigned int i_jet, bool doSmeared, TString sys, int n){

  //give average d0 of maximum significant of n leading track jets

    int tracks_passed=0;

    TLorentzVector jet,trk;

    float px, py,pz;

    std::vector<track> tracks;

  for(unsigned int i_trk=0; i_trk<this->trkjet_assocTrk_pt->at(i_jet).size(); i_trk++){

    if(!this->passAssocTrkSelection(i_trk,i_jet)) continue;

    tracks_passed++;

    track tr;

    if(sys.EqualTo("nominal") && doSmeared)tr.sd0=getSd0_smeared(i_trk,i_jet);
    else if(sys.EqualTo("up") && doSmeared)tr.sd0=getSd0_smeared_sys_up(i_trk,i_jet);
    else if(sys.EqualTo("down") && doSmeared)tr.sd0=getSd0_smeared_sys_up(i_trk,i_jet);
    else if(!doSmeared) tr.sd0=getSd0(i_trk,i_jet);
    else std::cout<<"ERROR: You have to specify if you want smeared, nominal or sys Sd0!"<<std::endl;


    tr.pt=this->trkjet_assocTrk_pt->at(i_jet).at(i_trk);

    

    if(sys.EqualTo("nominal") && doSmeared)tr.d0=this->trkjet_assocTrk_d0_smear->at(i_jet).at(i_trk);
    else if(sys.EqualTo("up") && doSmeared)tr.d0=this->trkjet_assocTrk_d0_smear_up->at(i_jet).at(i_trk);
    else if(sys.EqualTo("down") && doSmeared)tr.d0=this->trkjet_assocTrk_d0_smear_down->at(i_jet).at(i_trk);
    else if(!doSmeared) tr.d0=this->trkjet_assocTrk_d0->at(i_jet).at(i_trk);
    else std::cout<<"ERROR: You have to specify if you want smeared, nominal or sys Sd0!"<<std::endl;

    tracks.push_back(tr);
    
  }

  float sum=0.;

  //std::sort(tracks.begin(),tracks.end(),by_abs_sd0());
  std::sort(tracks.begin(),tracks.end(),by_pt());

  
  if(tracks.size()<n) return -99.;

  for(int i=0; i<n; i++){
    //std::cout<<"track "<<i<<": pT"<<tracks.at(i).pt<<std::endl;
    float d0=tracks.at(i).d0;
    float sd0=tracks.at(i).sd0;
    //sum+=sd0<0 ? -1.*TMath::Abs(d0) : TMath::Abs(d0);
    sum+=sd0;
  }

  return sum/n;

}


float GbbTupleAna::getTrkjetAssocTrkMaxSd0DR(unsigned int i_jet, bool doSmeared, TString sys){

  //give Sd0 of maximum significant among 3 leading track jets

  float max_sd0=0.,max2_sd0=0.,max3_sd0=0., tmp_sd0=0., ret=0.;
  float tmp_pt=0.,max_pt=0., max2_pt=0., max3_pt=0.;
  float tmp_DR=0.,DR=0.,DR_2=0., DR_3=0.;

    int tracks_passed=0;

    TLorentzVector jet,trk;

    float px, py,pz;

  for(unsigned int i_trk=0; i_trk<this->trkjet_assocTrk_pt->at(i_jet).size(); i_trk++){

    if(!this->passAssocTrkSelection(i_trk,i_jet)) continue;

    tracks_passed++;

    if(sys.EqualTo("nominal") && doSmeared)tmp_sd0=getSd0_smeared(i_trk,i_jet);
    else if(sys.EqualTo("up") && doSmeared)tmp_sd0=getSd0_smeared_sys_up(i_trk,i_jet);
    else if(sys.EqualTo("down") && doSmeared)tmp_sd0=getSd0_smeared_sys_up(i_trk,i_jet);
    else if(!doSmeared) tmp_sd0=getSd0(i_trk,i_jet);
    else std::cout<<"ERROR: You have to specify if you want smeared, nominal or sys Sd0!"<<std::endl;

    jet.SetPtEtaPhiM(this->trkjet_pt->at(i_jet),this->trkjet_eta->at(i_jet),this->trkjet_phi->at(i_jet),0);
    
    
    trk.SetPtEtaPhiM(this->trkjet_assocTrk_pt->at(i_jet).at(i_trk),this->trkjet_assocTrk_eta->at(i_jet).at(i_trk),this->trkjet_assocTrk_phi->at(i_jet).at(i_trk),0);

    //std::cout<<"DR(trk,jet) is "<<jet.DeltaR(trk)<<std::endl; 
    tmp_DR=jet.DeltaR(trk);


    tmp_pt=this->trkjet_assocTrk_pt->at(i_jet).at(i_trk);

    if(tmp_pt>max_pt){
	    max3_pt=max2_pt;
	    max3_sd0=max2_sd0;
	    DR_3=DR_2;

	    max2_pt=max_pt;
            max2_sd0=max_sd0;
	    DR_2=DR;

            max_pt=tmp_pt;
            max_sd0=tmp_sd0;
	    DR=tmp_DR;

    }else if(tmp_pt>max2_pt){

	    max3_pt=max2_pt;
            max3_sd0=max2_sd0;
	    DR_3=DR_2;

	    max2_pt=tmp_pt;
	    max2_sd0=tmp_sd0;
	    DR_2=tmp_DR;

    }else if(tmp_pt>max3_pt){

	    max3_pt=tmp_pt;
	    max3_sd0=tmp_sd0;
	    DR_3=tmp_DR;
	    
    }

    
  }

  if(tracks_passed<3) return -99;
  
  float val_DR;

  if(TMath::Abs(max_sd0)>TMath::Abs(max2_sd0) && TMath::Abs(max_sd0)>TMath::Abs(max3_sd0)){
    ret=max_sd0;
    val_DR=DR_3;
  }
  else if(TMath::Abs(max2_sd0)>TMath::Abs(max3_sd0)){
    ret=max2_sd0;
    val_DR=DR_2;
  }
  else{
    ret=max3_sd0;
    val_DR=DR;
  } 
  //if(val_DR>0.2) std::cout<<"TemplateTrack DR larger than 0.2, value: "<<val_DR<<std::endl;

  return val_DR;

}

trkjetSd0Info GbbTupleAna::getTrkjetAssocSd0Info(unsigned int i_jet, bool doSmeared, TString sys, int n){

  int tracks_passed=0;
  TLorentzVector jet,trk;
  jet.SetPtEtaPhiM(this->trkjet_pt->at(i_jet),this->trkjet_eta->at(i_jet),this->trkjet_phi->at(i_jet),0);
  float px, py,pz;
  std::vector<track> tracks;
  trkjetSd0Info ret = {-99.,-99.,-99.,-99.,-99.,-99.,-99.,-99.,n};

  for(unsigned int i_trk=0; i_trk<this->trkjet_assocTrk_pt->at(i_jet).size(); i_trk++){
    if(!this->passAssocTrkSelection(i_trk,i_jet)) continue;

    tracks_passed++;
    track tr;

    if(sys.EqualTo("nominal") && doSmeared)tr.sd0=getSd0_smeared(i_trk,i_jet);
    else if(sys.EqualTo("up") && doSmeared)tr.sd0=getSd0_smeared_sys_up(i_trk,i_jet);
    else if(sys.EqualTo("down") && doSmeared)tr.sd0=getSd0_smeared_sys_up(i_trk,i_jet);
    else if(!doSmeared) tr.sd0=getSd0(i_trk,i_jet);
    else std::cout<<"ERROR: You have to specify if you want smeared, nominal or sys Sd0!"<<std::endl;

    tr.pt=this->trkjet_assocTrk_pt->at(i_jet).at(i_trk);

    if(sys.EqualTo("nominal") && doSmeared)tr.d0=this->trkjet_assocTrk_d0_smear->at(i_jet).at(i_trk);
    else if(sys.EqualTo("up") && doSmeared)tr.d0=this->trkjet_assocTrk_d0_smear_up->at(i_jet).at(i_trk);
    else if(sys.EqualTo("down") && doSmeared)tr.d0=this->trkjet_assocTrk_d0_smear_down->at(i_jet).at(i_trk);
    else if(!doSmeared) tr.d0=this->trkjet_assocTrk_d0->at(i_jet).at(i_trk);
    else std::cout<<"ERROR: You have to specify if you want smeared, nominal or sys Sd0!"<<std::endl;

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

float GbbTupleAna::getSd0(unsigned int i_trk, unsigned int i_jet){
  
  TLorentzVector jet;
  jet.SetPtEtaPhiM(this->trkjet_pt->at(i_jet),this->trkjet_eta->at(i_jet),this->trkjet_phi->at(i_jet),0.);

  float d0=this->trkjet_assocTrk_d0->at(i_jet).at(i_trk);
  
  //std::cout<<"d0 is: "<<d0<<std::endl;

  float trk_pt=this->trkjet_assocTrk_pt->at(i_jet).at(i_trk);
  float trk_eta=this->trkjet_assocTrk_eta->at(i_jet).at(i_trk);

  //std::cout<<"smeared d0 is: "<<d0<<std::endl;

  float sd0=d0/this->trkjet_assocTrk_d0err->at(i_jet).at(i_trk);

  float det_sign=TMath::Sin(jet.Phi()-this->trkjet_assocTrk_phi->at(i_jet).at(i_trk))*d0;

  //std::cout<<"phi difference"<<jet.Phi()-this->trkjet_assocTrk_phi->at(i_jet).at(i_trk)<<std::endl;

  //std::cout<<"sign variables"<<det_sign<<"  "<< det_sign_other<<std::endl;

  float val = det_sign>=0 ? 1. : -1.;

  
  return val*TMath::Abs(sd0);


}


float GbbTupleAna::getSd0_smeared(unsigned int i_trk, unsigned int i_jet){
  
  TLorentzVector jet;
  jet.SetPtEtaPhiM(this->trkjet_pt->at(i_jet),this->trkjet_eta->at(i_jet),this->trkjet_phi->at(i_jet),0.);
  
  float d0=this->trkjet_assocTrk_d0->at(i_jet).at(i_trk);
  
  //std::cout<<"d0 is: "<<d0<<std::endl;
  
  float trk_pt=this->trkjet_assocTrk_pt->at(i_jet).at(i_trk);
  float trk_eta=this->trkjet_assocTrk_eta->at(i_jet).at(i_trk);
  
  if(this->eve_isMC) d0=this->trkjet_assocTrk_d0_smear->at(i_jet).at(i_trk);
  
  //std::cout<<"smeared d0 is: "<<d0<<std::endl;
  
  float sd0=d0/this->trkjet_assocTrk_d0err->at(i_jet).at(i_trk);
  
  float det_sign=TMath::Sin(jet.Phi()-this->trkjet_assocTrk_phi->at(i_jet).at(i_trk))*d0;
  
  //std::cout<<"phi difference"<<jet.Phi()-this->trkjet_assocTrk_phi->at(i_jet).at(i_trk)<<std::endl;
  
  //std::cout<<"sign variables"<<det_sign<<"  "<< det_sign_other<<std::endl;
  
  float val = det_sign>=0 ? 1. : -1.;
  
  
  return val*TMath::Abs(sd0);
  
  
}

float GbbTupleAna::getSd0_smeared_sys_up(unsigned int i_trk, unsigned int i_jet){
  
  TLorentzVector jet;
  jet.SetPtEtaPhiM(this->trkjet_pt->at(i_jet),this->trkjet_eta->at(i_jet),this->trkjet_phi->at(i_jet),0.);
  
  float d0=this->trkjet_assocTrk_d0->at(i_jet).at(i_trk);
  
  float trk_pt=this->trkjet_assocTrk_pt->at(i_jet).at(i_trk);
  float trk_eta=this->trkjet_assocTrk_eta->at(i_jet).at(i_trk);
  
  if(this->eve_isMC) d0=this->trkjet_assocTrk_d0_smear_up->at(i_jet).at(i_trk);
  
  float sd0=d0/this->trkjet_assocTrk_d0err->at(i_jet).at(i_trk);
  
  float det_sign=TMath::Sin(jet.Phi()-this->trkjet_assocTrk_phi->at(i_jet).at(i_trk))*d0;
  
  float val = det_sign>=0 ? 1. : -1.;
  
  
  return val*TMath::Abs(sd0);
  
  
}

float GbbTupleAna::getSd0_smeared_sys_down(unsigned int i_trk, unsigned int i_jet){
  
  TLorentzVector jet;
  jet.SetPtEtaPhiM(this->trkjet_pt->at(i_jet),this->trkjet_eta->at(i_jet),this->trkjet_phi->at(i_jet),0.);
  
  float d0=this->trkjet_assocTrk_d0->at(i_jet).at(i_trk);
  
  float trk_pt=this->trkjet_assocTrk_pt->at(i_jet).at(i_trk);
  float trk_eta=this->trkjet_assocTrk_eta->at(i_jet).at(i_trk);
  
  if(this->eve_isMC) d0=this->trkjet_assocTrk_d0_smear_down->at(i_jet).at(i_trk);
  
  float sd0=d0/this->trkjet_assocTrk_d0err->at(i_jet).at(i_trk);
  
  float det_sign=TMath::Sin(jet.Phi()-this->trkjet_assocTrk_phi->at(i_jet).at(i_trk))*d0;
  
  float val = det_sign>=0 ? 1. : -1.;
  
  
  return val*TMath::Abs(sd0);
  
  
}

float GbbTupleAna::getCMd0Difference(unsigned int i_trk, unsigned int j_trk, GbbCandidate* gbbcand){
  //THIS PART NEEDS EDITING

  float diff=0;
  /*TLorentzVector fatjet;
  fatjet.SetPtEtaPhiM(this->fat_pt->at(gbbcand->fat_index),this->fat_pt->eta(gbbcand->fat_index),this->fat_phi->at(gbbcand->fat_index), this->fat_m->at(gbbcand->fat_index));
  
  int mj_index=gbbcand->muojet_index;
  int nmj_index=gbbcand->nonmuojet_index;

  int mj_d0=0;

  if(this->eve_isMC) d0=this->trkjet_assocTrk_d0_smear->at(i_jet).at(i_trk);
  else d0=this->trkjet_assocTrk_d0_smear->at(i_jet).at(i_trk);
  */


  return diff;
}


float GbbTupleAna::getAssocTrackOldSelectiond0AndErr(unsigned int i_jet,float & d0, float& err_d0){

	float max_sd0=0., tmp_sd0=0.;
	
	float max_d0=0., max_errd0=0.;
	int tracks_passed=0;
	
	for(unsigned int i_trk=0; i_trk<this->trkjet_assocTrk_pt->at(i_jet).size(); i_trk++){
		
		if(!this->passAssocTrkSelection(i_trk,i_jet)) continue;
		
		tracks_passed++;
		
		tmp_sd0=getSd0(i_trk,i_jet);
	

		if(TMath::Abs(tmp_sd0)>TMath::Abs(max_sd0)){
			
			max_sd0=tmp_sd0;
			max_errd0=this->trkjet_assocTrk_d0err->at(i_jet).at(i_trk);
			max_d0=this->trkjet_assocTrk_d0->at(i_jet).at(i_trk);

		}
	
	}
	

	if(tracks_passed==0){

		max_d0=-999;
		max_errd0=-999;
		return -99;
	}else{

		d0=max_d0;
		err_d0=max_errd0;
	}


	return max_sd0;
}



float GbbTupleAna::getAssocTrackNewSelectiond0AndErr(unsigned int i_jet,float & d0, float& err_d0){

	float max_sd0=0.,max2_sd0=0.,max3_sd0=0., tmp_sd0=0., ret=0.;
	float tmp_pt=0.,max_pt=0., max2_pt=0., max3_pt=0.;
	float max_d0=0., max_errd0=0., max2_d0=0., max2_errd0=0., max3_d0=0., max3_errd0=0., tmp_d0=0., tmp_errd0=0.;

	int tracks_passed=0;

	for(unsigned int i_trk=0; i_trk<this->trkjet_assocTrk_pt->at(i_jet).size(); i_trk++){

	  TLorentzVector v_jet, v_trk;
	  v_jet.SetPtEtaPhiM(this->trkjet_pt->at(i_jet),this->trkjet_eta->at(i_jet),this->trkjet_phi->at(i_jet),0.);
	  v_trk.SetPtEtaPhiM(this->trkjet_assocTrk_pt->at(i_jet).at(i_trk),this->trkjet_assocTrk_eta->at(i_jet).at(i_trk),this->trkjet_assocTrk_phi->at(i_jet).at(i_trk),0.);
	  std::cout<<"DR(trk,jet) is "<<v_jet.DeltaR(v_trk)<<std::endl; 

		if(!this->passAssocTrkSelection(i_trk,i_jet)) continue;
		

		tracks_passed++;

		tmp_sd0=getSd0(i_trk,i_jet);

		tmp_pt=this->trkjet_assocTrk_pt->at(i_jet).at(i_trk);
		tmp_d0=this->trkjet_assocTrk_d0->at(i_jet).at(i_trk);
		tmp_errd0=this->trkjet_assocTrk_d0err->at(i_jet).at(i_trk);


		if(tmp_pt>max_pt){
			max3_pt=max2_pt;
			max3_sd0=max2_sd0;
			max3_d0=max2_d0;
			max3_errd0=max2_errd0;


			max2_pt=max_pt;
			max2_sd0=max_sd0;
			max2_d0=max_d0;
			max2_errd0=max_errd0;
			

			max_pt=tmp_pt;
			max_sd0=tmp_sd0;
			max_d0=tmp_d0;
			max_errd0=tmp_errd0;


		}else if(tmp_pt>max2_pt){

			max3_pt=max2_pt;
			max3_sd0=max2_sd0;
			max3_d0=max2_d0;
			max3_errd0=max2_errd0;

			max2_pt=tmp_pt;
			max2_sd0=tmp_sd0;
			max2_d0=tmp_d0;
			max2_errd0=tmp_errd0;

		}else if(tmp_pt>max3_pt){

			max3_pt=tmp_pt;
			max3_sd0=tmp_sd0;
			max3_d0=tmp_d0;
			max3_errd0=tmp_errd0;

		}



	}

	if(tracks_passed<3) return -99;
	
	if(TMath::Abs(max_sd0)>TMath::Abs(max2_sd0) && TMath::Abs(max_sd0)>TMath::Abs(max3_sd0)){

		d0=max_d0;
		err_d0=max_errd0;
		ret=max_sd0;
	
	}else if(TMath::Abs(max2_sd0)>TMath::Abs(max3_sd0)){
		
		d0=max2_d0;
		err_d0=max2_errd0;
		ret=max2_sd0;

	}else{
		d0=max3_d0;
		err_d0=max3_errd0;
		ret=max3_sd0;
	}

	
	
       	return ret;


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

void GbbTupleAna::getSystematicsFlags(GbbCandidate *gbbcand, bool &hasConversion, bool &hasHadMatInt, bool &hasLightLongLived, bool &hasNoTruthMuon){

  if(this->trkjet_hasHadMatInt->at(gbbcand->muojet_index) || this->trkjet_hasHadMatInt->at(gbbcand->nonmuojet_index)) hasHadMatInt=true;

  if(this->trkjet_hasConversion->at(gbbcand->muojet_index) || this->trkjet_hasConversion->at(gbbcand->nonmuojet_index)) hasConversion=true;
  
  if(this->trkjet_hasKShort->at(gbbcand->muojet_index) || this->trkjet_hasKShort->at(gbbcand->nonmuojet_index) || this->trkjet_hasLambda->at(gbbcand->muojet_index) || this->trkjet_hasLambda->at(gbbcand->nonmuojet_index)) hasLightLongLived=true;

  if(!gbbcand->hasTruthMuon) hasNoTruthMuon=true;

}


std::vector<float> GbbTupleAna::SplitStringD(TString str, char delim){

  std::vector<float> tokens;
  TObjArray *Strings=str.Tokenize(delim);
  
  for(int i=0; i<Strings->GetEntriesFast(); i++){

    tokens.push_back((((TObjString*) (*Strings)[i])->GetString()).Atof());
    //tokens.push_back((*((TString*)(*Strings)[i])).Atof());
    std::cout<<"Token: "<<((TObjString*) (*Strings)[i])->GetString()<<" vs "<<tokens[i]<<std::endl;

  }

  return tokens;

}

// function to create the bit mask
unsigned int GbbTupleAna::bitmask(const unsigned int cut, const std::vector<unsigned int> &excludeCuts) {

  unsigned int mask = 0;
  unsigned int bit  = 0;

  for (unsigned int i=0; i < cut+1; ++i) {
    if   ( std::find(excludeCuts.begin(), excludeCuts.end(), i) != excludeCuts.end() ) {
      // if a cut should be excluded set the corresponding bit to 0                                                                                                                                                                          
      mask = mask | 0 << bit++;
    } else  {
      // otherwise set the bit to 1                                                                                                                                                                                                          
      mask = mask | 1 << bit++;
    }
  }

  return mask;
}


bool GbbTupleAna::passAllCutsUpTo(const unsigned long int flag, const unsigned int cut, const std::vector<unsigned int> &excludeCuts) {

  // Get the bitmask: we want to check all cuts up to "cut" excluding the cuts listed in excludeCuts                                                                                                                                         
  unsigned int mask = bitmask(cut, excludeCuts);
  // Check if the flag matches the bit mask                                                                                                                                                                                                  
  return (flag & mask) == mask;

}

// a function to check specifig cuts                                                                                                                                                                                                         
bool GbbTupleAna::passSpecificCuts(const unsigned long int flag, const std::vector<unsigned int> &cuts) {
  unsigned int mask = 0;
  // Make the bit mask                                                                                                                                                                                                                       
  for (auto cut : cuts) mask = mask | 1 << cut;
  // Check if the flag matches the bit mask                                                                                                                                                                                                  
  return (flag & mask) == mask;
}


// a function to update the bit flag                                                                                                                                                                                                         
void GbbTupleAna::updateFlag(unsigned long int &flag, const unsigned int cutPosition, const bool passCut) {
  // Put bit passCut (true or false) at position cutPosition                                                                                                                                                                                
  flag = flag | passCut << cutPosition;
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


