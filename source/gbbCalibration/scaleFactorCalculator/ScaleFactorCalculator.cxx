/*
 * ScaleFactorCalculator.cxx
 *
 *  Created on: Dec 12, 2016
 *      Author: rjacobs
 */

#include "ScaleFactorCalculator.h"
#include "TGraphAsymmErrors.h"
#include "TGraphErrors.h"
#include "TLegend.h"
#include "TColor.h"
#include "TDatime.h"
#include "TLatex.h"
#include "TCanvas.h"
#include "TLine.h"
#include "THStack.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TGaxis.h"
#include "TVector.h"
#include "TMatrix.h"
#include "TMatrixFUtils.h"
#include <TMatrixDSymEigen.h>
#include <TMatrixDSym.h>
#include <TVectorD.h>
#include "TH2.h"

ScaleFactorCalculator::ScaleFactorCalculator() {
  // TODO Auto-generated constructor stub

}

ScaleFactorCalculator::~ScaleFactorCalculator() {
  // TODO Auto-generated destructor stub
  if(m_config) delete m_config;
}


ScaleFactorCalculator::ScaleFactorCalculator(TString &cfg_file){

  gStyle->SetOptStat(0);
  TGaxis::SetMaxDigits(4);
  TH1::AddDirectory(kFALSE); 

  std::cout<<"==================================================="<<std::endl;
  std::cout<<"| HELLO THIS IS THE GBB SCALE FACTOR CALCULATOR!   "<<std::endl;
  //std::cout<<"| Brumm... "<<std::endl;
  std::cout<<"==================================================="<<std::endl;

  
  m_config=new SFConfig(cfg_file);
  m_fitdata=new FitData(m_config->GetInfileName(),m_config->GetChans());

  std::vector<TString> systematics=m_config->GetSystematics();
  std::vector<TString> regions=m_config->GetAllRegions();
  std::vector<TString> chans=m_config->GetChans();
  std::vector<TString> fitpar_names=m_config->GetParamNames();
  std::vector<float> fitpar_start=m_config->GetParamStartValues();
  std::vector<float> fitpar_low=m_config->GetParamLowValues();
  std::vector<float> fitpar_high=m_config->GetParamHighValues(); 

  TString tmpl_muo_data, tmpl_nonmuo_data;
  std::vector<TString> tmpl_muo_mc, tmpl_nonmuo_mc;

  TString muo_name="muojet";
  TString nonmuo_name="nonmuojet";
  TString mj_maxSd0="mjmaxSd0";
  TString nmj_maxSd0="nmjmaxSd0";


  TString nominal="Nom";

  std::vector<TString> fitstatus, flav_fractions, mu_factors;

  std::vector<std::vector<double>> binning;

  for(unsigned int i_reg=0; i_reg<regions.size(); i_reg++){
    
    tmpl_muo_data=m_config->GetDataHistName(regions[i_reg],mj_maxSd0);
    tmpl_nonmuo_data=m_config->GetDataHistName(regions[i_reg],nmj_maxSd0);


    for(unsigned int i_sys=0; i_sys<systematics.size(); i_sys++){
      
      tmpl_muo_mc=m_config->GetMCHistNames(regions[i_reg],systematics[i_sys],mj_maxSd0);
      tmpl_nonmuo_mc=m_config->GetMCHistNames(regions[i_reg],systematics[i_sys],nmj_maxSd0);
  
      m_fitdata->SetHistogramNames(chans[0],tmpl_muo_data,tmpl_muo_mc);
      m_fitdata->SetHistogramNames(chans[1],tmpl_nonmuo_data,tmpl_nonmuo_mc);

      m_fitdata->ReadInHistograms();
      //m_fitdata->FitTemplateSlopes(-5.,10.);
      m_fitter=Fitter(m_fitdata, fitpar_names.size() );

      std::cout<<fitpar_names.size()<<std::endl;
      std::cout<<fitpar_start.size()<<std::endl;
      std::cout<<fitpar_low.size()<<std::endl;
      std::cout<<fitpar_high.size()<<std::endl;

      
      for(unsigned int i_par=0; i_par<fitpar_names.size(); i_par++){	

	std::cout<<"Adding: "<<fitpar_names[i_par]<<" "<<fitpar_start[i_par]<<" "<<fitpar_low[i_par]<<" "<<fitpar_high[i_par]<<std::endl;

	m_fitter.AddParameter(fitpar_names[i_par],fitpar_start[i_par],0.001,fitpar_low[i_par],fitpar_high[i_par]);

      } 
      
      m_fitter.Initialize();      

      if(m_config->GetSmoothNtimes())m_fitdata->KernelSmoothTemplates(m_config->GetSmoothNtimes());

      for(unsigned int i_chan=0; i_chan<chans.size(); i_chan++){ //Auto rebin on nominal, use same bins for systematics
	if(systematics[i_sys].EqualTo("Nom")) binning.push_back(m_fitdata->AutoRebinHistograms(chans[i_chan],m_config->GetRebinStatThr(),1));
	else{
	  m_fitdata->FixHistogramBins(chans[i_chan],binning[2*i_reg+i_chan]);
	}
      }
      
      //m_fitdata->FitTemplateSlopes(-10.,10.);

      m_fitter.PrintParameters("Simple");
      m_fitter.fit();
      m_fitter.PrintParameters("Simple");
      fitstatus.push_back(m_fitter.getFitStatus()+"_"+systematics[i_sys]+"_"+regions[i_reg]);
      
      m_fit_params[ (regions[i_reg]+"_"+systematics[i_sys]) ]=m_fitter.FitParameters();
      
      if(systematics[i_sys].EqualTo("Nom")) m_nom_cov_mats[regions[i_reg]]=m_fitter.CovarianceMatrix();
      
      if(systematics[i_sys].EqualTo("Nom") || systematics[i_sys].EqualTo("MUON_ID__1up") || regions[i_reg].Contains("mjpt_g200_nmjpt_g300")){ //make control plots

	if(m_config->DoControlPlots() || 1){
	  this->MakeTemplateControlPlots(false,m_fitdata->GetDataHist(chans[0]),m_fitdata->GetMCHists(chans[0]),chans[0],regions[i_reg],systematics[i_sys],1);
	  this->MakeTemplateControlPlots(true,m_fitdata->GetDataHist(chans[0]),m_fitdata->GetMCHists(chans[0]),chans[0],regions[i_reg],systematics[i_sys],1);
	  
	  this->MakeTemplateControlPlots(false,m_fitdata->GetDataHist(chans[1]),m_fitdata->GetMCHists(chans[1]),chans[1],regions[i_reg],systematics[i_sys],1);
	  this->MakeTemplateControlPlots(true,m_fitdata->GetDataHist(chans[1]),m_fitdata->GetMCHists(chans[1]),chans[1],regions[i_reg],systematics[i_sys],1);
	}

	flav_fractions.push_back(this->MakeFlavourFractionTable(true,m_fitdata->GetDataHist(chans[1]),m_fitdata->GetMCHists(chans[1]),chans[1],regions[i_reg]));
	mu_factors.push_back(this->PrintMuAndError(regions[i_reg],m_fitdata->GetMCHists(chans[1])));
      }


      m_fitdata->ResetHists();

    }
  }

  //make Posttag Template Plots
  TString mj_maxSd0_posttag="mjmaxSd0_PREFITPOSTTAG";
  TString nmj_maxSd0_posttag="nmjmaxSd0_PREFITPOSTTAG";
  
  for(unsigned int i_reg=0; i_reg<regions.size(); i_reg++){
    
    tmpl_muo_data=m_config->GetDataHistName(regions[i_reg],mj_maxSd0_posttag);
    tmpl_nonmuo_data=m_config->GetDataHistName(regions[i_reg],nmj_maxSd0_posttag);


    for(unsigned int i_sys=0; i_sys<systematics.size(); i_sys++){
      
      tmpl_muo_mc=m_config->GetMCHistNames(regions[i_reg],systematics[i_sys],mj_maxSd0_posttag);
      tmpl_nonmuo_mc=m_config->GetMCHistNames(regions[i_reg],systematics[i_sys],nmj_maxSd0_posttag);
  
      m_fitdata->SetHistogramNames(chans[0],tmpl_muo_data,tmpl_muo_mc);
      m_fitdata->SetHistogramNames(chans[1],tmpl_nonmuo_data,tmpl_nonmuo_mc);

      m_fitdata->ReadInHistograms();

      if(systematics[i_sys].EqualTo("Nom") && m_config->DoControlPlots() ){
	this->MakeTemplateControlPlots(true,m_fitdata->GetDataHist(chans[0]),m_fitdata->GetMCHists(chans[0]),chans[0],regions[i_reg],systematics[i_sys],4,true);
	this->MakeTemplateControlPlots(true,m_fitdata->GetDataHist(chans[1]),m_fitdata->GetMCHists(chans[1]),chans[1],regions[i_reg],systematics[i_sys],4,true);
	this->MakeTemplateControlPlots(false,m_fitdata->GetDataHist(chans[0]),m_fitdata->GetMCHists(chans[0]),chans[0],regions[i_reg],systematics[i_sys],4,true);
	this->MakeTemplateControlPlots(false,m_fitdata->GetDataHist(chans[1]),m_fitdata->GetMCHists(chans[1]),chans[1],regions[i_reg],systematics[i_sys],4,true);
      
      }


    }

    m_fitdata->ResetHists();
  }
  


  std::cout<<"=========================="<<std::endl;
  std::cout<<"| FIT STATUS SUMMARY: "<<std::endl;
  for(auto &el : fitstatus) std::cout<<"| "<<el<<std::endl;
  std::cout<<"=========================="<<std::endl;

  /*  std::cout<<"=========================="<<std::endl;
  std::cout<<"| FLAVOUR FRACTIONS SUMMARY: "<<std::endl;
  std::cout<<"| BB BL CC CL LL "<<std::endl;
  for(auto &el : flav_fractions) std::cout<<"| "<<el<<std::endl;
  std::cout<<"=========================="<<std::endl;
  */

  std::cout<<"=========================="<<std::endl;
  std::cout<<"| FLAVOUR FRACTIONS LATEX SUMMARY: "<<std::endl;
  std::cout<<"\\begin{tabular}{ | c | c | c | c | c | c | }"<<std::endl;
  std::cout<<"\\hline"<<std::endl;
  std::cout<<"\\textbf{fit category} & \\textbf{BB} & \\textbf{BL} & \\textbf{CC} & \\textbf{CL} & \\textbf{LL} \\\\ \\hline\\hline"<<std::endl;
  for(auto &el : flav_fractions) std::cout<<el<<"\\\\ \\hline"<<std::endl;
  std::cout<<"\\hline"<<std::endl;
  std::cout<<"\\end{tabular}"<<std::endl;
  std::cout<<"=========================="<<std::endl;

  std::cout<<"=========================="<<std::endl;
  std::cout<<"| CORRECTION FACTORS (norm corrected) LATEX SUMMARY: "<<std::endl;
  std::cout<<"\\begin{tabular}{ | c | c | c | c | c | c | }"<<std::endl;
  std::cout<<"\\hline"<<std::endl;
  std::cout<<"\\textbf{fit category} & \\textbf{BB} & \\textbf{BL} & \\textbf{CC} & \\textbf{CL} & \\textbf{LL} \\\\ \\hline\\hline"<<std::endl;
  for(auto &mu : mu_factors) std::cout<<mu<<"\\\\ \\hline"<<std::endl;
  std::cout<<"\\hline"<<std::endl;
  std::cout<<"\\end{tabular}"<<std::endl;
  std::cout<<"=========================="<<std::endl;
  


  if(m_config->DoCalibSequence()){
    //Run Pseudo-Experiments for template stat. uncertainty
  
    std::vector<std::vector<double>> help;
    
    for(unsigned int i_reg=0; i_reg<regions.size(); i_reg++){
      tmpl_muo_data=m_config->GetDataHistName(regions[i_reg],mj_maxSd0);
      tmpl_nonmuo_data=m_config->GetDataHistName(regions[i_reg],nmj_maxSd0);
    
      tmpl_muo_mc=m_config->GetMCHistNames(regions[i_reg],nominal,mj_maxSd0);
      tmpl_nonmuo_mc=m_config->GetMCHistNames(regions[i_reg],nominal,nmj_maxSd0);
      
      m_fitdata->SetHistogramNames(chans[0],tmpl_muo_data,tmpl_muo_mc);
      m_fitdata->SetHistogramNames(chans[1],tmpl_nonmuo_data,tmpl_nonmuo_mc);
      
      m_fitdata->ReadInHistograms();
      
      help.clear();
      
      m_fitter.GetPseudoTemplateFitResult(help, m_config->GetNPseudoExp());
      m_pseudo_fit_params[regions[i_reg]]=help;
      
      m_fitdata->ResetHists();
      
    }
    
    
    //Run Pseudo-Experiments for data stat uncertainty
    
    
    for(unsigned int i_reg=0; i_reg<regions.size(); i_reg++){
      tmpl_muo_data=m_config->GetDataHistName(regions[i_reg],mj_maxSd0);
      tmpl_nonmuo_data=m_config->GetDataHistName(regions[i_reg],nmj_maxSd0);
      
      tmpl_muo_mc=m_config->GetMCHistNames(regions[i_reg],nominal,mj_maxSd0);
      tmpl_nonmuo_mc=m_config->GetMCHistNames(regions[i_reg],nominal,nmj_maxSd0);
      
      m_fitdata->SetHistogramNames(chans[0],tmpl_muo_data,tmpl_muo_mc);
      m_fitdata->SetHistogramNames(chans[1],tmpl_nonmuo_data,tmpl_nonmuo_mc);
      
      m_fitdata->ReadInHistograms();
      
      help.clear();
      
      m_fitter.GetPseudoDataFitResult(help, m_config->GetNPseudoExp());
      m_pseudo_fit_params_Data[regions[i_reg]]=help;
      
      m_fitdata->ResetHists();
      
    }
    
  }


  //start calibration sequence
  TString pt_name="fjpt", pt_posttag_name="pt_PREFITPOSTTAG";
  
  //  std::vector<TString> variables={"fjpt","fjm"};
  std::vector<TString> variables = {"fjpt","mjmaxSd0"}; //m_config->GetPlotVariables();
  std::vector<TString> sys = m_config->GetFatJetSystematics(); //{"Nom","JET_Rtrk_Baseline_Kin__1up","JET_Rtrk_Baseline_Kin__1down","JET_Rtrk_Modelling_Kin__1up", "JET_Rtrk_Modelling_Kin__1down", "JET_Rtrk_Tracking_Kin__1up", "JET_Rtrk_Tracking_Kin__1down", "JET_Rtrk_TotalStat_Kin__1up", "JET_Rtrk_TotalStat_Kin__1down", "JET_Rtrk_Baseline_Sub__1up", "JET_Rtrk_Baseline_Sub__1down","JET_Rtrk_Modelling_Sub__1up", "JET_Rtrk_Modelling_Sub__1down", "JET_Rtrk_Tracking_Sub__1up","JET_Rtrk_Tracking_Sub__1down", "JET_Rtrk_TotalStat_Sub__1up", "JET_Rtrk_TotalStat_Sub__1down", "FATJET_JMR__1up","FATJET_JER__1up","Herwig__1up"};
  sys.insert(0,"Nom");
  std::vector<TString> sys_only={"JET_Rtrk_Baseline_Kin", "JET_Rtrk_Modelling_Kin", "JET_Rtrk_Tracking_Kin", "JET_Rtrk_TotalStat_Kin", "JET_Rtrk_Baseline_Sub","JET_Rtrk_Modelling_Sub","JET_Rtrk_Tracking_Sub","JET_Rtrk_TotalStat_Sub","FATJET_JMR","FATJET_JER"};
  std::vector<TString> model_sys={"Herwig"};
  std::vector<TString> nominal_only={"Nom"};
  std::vector<TString> none={};
  std::vector<TString> variables_posttag, variables_posttag_btagsys;
  
  std::vector<TString> calib_var={"mjmaxSd0", "mjmaxSd0_PREFITPOSTTAG"};
  std::vector<TString> calib_sys={"Conversion__1up","Conversion__1down","HadMatInt__1up","HadMatInt__1down","LightLongLived__1up","LightLongLived__1down","SD0Smear__1up","SD0SMEAR__1down"};
//,"MUON_ID__1up","MUON_ID__1down","MUON_MS__1up","MUON_MS__1down","MUON_SCALE__1up","MUON_SCALE__1down","MUON_SAGITTA_RESBIAS__1up","MUON_SAGITTA_RESBIAS__1down","MUON_SAGITTA_RHO__1up","MUON_SAGITTA_RHO__1down","MUON_EFF_STAT__1up","MUON_EFF_STAT__1down","MUON_EFF_SYS__1up","MUON_EFF_SYS__1down","MUON_EFF_STAT_LOWPT__1up","MUON_EFF_STAT_LOWPT__1down","MUON_EFF_SYS_LOWPT__1up","MUON_EFF_SYS_LOWPT__1down","MUON_TTVA_STAT__1up","MUON_TTVA_STAT__1down","MUON_TTVA_SYS__1up","MUON_TTVA_SYS__1down"};
  
  for(auto& el : variables){
    if(el.Contains("ANTITAG") || el.Contains("trjpt") || el.Contains("srj") || el.Contains("evemu") || (el.Contains("fjeta") && el.Contains("fjphi")) || el.Contains("slR4jpt") || el.Contains("trjptfjptratio") || el.Contains("trjptgbbcandratio")) continue;
    variables_posttag.push_back(TString(el)+"_PREFITPOSTTAG");
  }
  for(auto& el : variables_posttag){
    if(el.Contains("mjmaxSd0")) continue;
    variables_posttag_btagsys.push_back(TString(el)+"_BTAGUP");
    variables_posttag_btagsys.push_back(TString(el)+"_BTAGDOWN");
  }
  variables_posttag_btagsys.push_back("mjpt_PREFITUNTAG");
  
  this->ReadInFatJetHists(variables,sys);
  this->ReadInFatJetHists(variables_posttag,sys);
  this->ReadInFatJetHists(variables_posttag_btagsys,nominal_only);
  this->ReadInFatJetHists(calib_var,calib_sys);
  
  TString ts_pt="fjpt";
  
  if(m_config->DoControlPlots() && !m_config->DoCalibSequence()){
    for(unsigned int i_var=0; i_var<variables.size(); i_var++){
      //fat jet control plots
      //this->MakeFatJetControlPlots(variables[i_var],false,false,sys_only,model_sys);
      this->MakeFatJetControlPlots(variables[i_var],false,true,sys_only,model_sys);
    }
    
    //posttag plots
    for(unsigned int i_var=0; i_var<variables_posttag.size(); i_var++){
      //this->MakeFatJetControlPlots(variables_posttag[i_var],true,false,sys_only,model_sys);
      this->MakeFatJetControlPlots(variables_posttag[i_var],true,true,sys_only,model_sys);
    }
  
  }

  //fat jet binning control plots
  //for(auto& reg : regions){
    //this->MakeFatJetControlPlots(ts_pt,false,false,nominal,true,reg);
    //this->MakeFatJetControlPlots(ts_pt,false,true,none,true,reg);
    //this->MakeFatJetControlPlots(ts_pt,true,true,nominal,true,reg);
  //}
  

  //BTagging Rate Plots
  if(m_config->DoControlPlots() && !m_config->DoCalibSequence()) this->MakeBTaggingRatePlots(sys_only,model_sys);

  //TString reweight_name="test_reweight_hists.root";
  //this->SaveReweightHists(pt_name, reweight_name);
  
  if(m_config->DoCalibSequence()){
  
    CalibResult c_res=this->CalculateScaleFactorsAndErrors(true);
    this->MakeCalibrationPlots(c_res,"2DSF");
    this->MakeCalibrationPlots(c_res,"2DEffMC");
    this->MakeCalibrationPlots(c_res,"2DEffData");

    this->SaveFitCorrectionFactorsSys();
    /*CalibResult c_res_1d=this->CalculateScaleFactorsAndErrors(false);
    this->MakeCalibrationPlots(c_res_1d,"SF");
    this->MakeCalibrationPlots(c_res_1d,"Eff");
    */
  }

  /*  for(unsigned int i_reg=0; i_reg<regions.size(); i_reg++){
    for(unsigned int i_sys=0; i_sys<systematics.size(); i_sys++){
      std::cout<<"Fit parameters in region "<<regions[i_reg]<<"for sys "<<systematics[i_sys]<<" :"<<std::endl;
      for(unsigned int i_p=0; i_p<m_fit_params[ (regions[i_reg]+"_"+systematics[i_sys]) ].size(); i_p++) std::cout<<m_fit_params[ (regions[i_reg]+"_"+systematics[i_sys]) ][i_p]<<std::endl;
    }
    }*/

    
}


SFCalcResult ScaleFactorCalculator::CalculateScaleFactors(TString &sys, bool doPseudo, unsigned int i_pseudo, bool doPseudoData){
  //Correct MC histograms by fit factors, subtract from data, BB_data

  std::vector<float> fj_bins=m_config->GetFatJetPtBins();
  std::vector<TString> regions=m_config->GetAllRegions();
  
  std::vector<TH1D*> hist_pretag_mc, hist_posttag_mc, hist_pretag_mc_unscaled, hist_posttag_mc_unscaled;
  TString name_pretag, name_posttag;

  for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){

    name_pretag="hist_pretag_mc_"+(m_config->GetFlavourPairs())[i_p];
    name_posttag="hist_posttag_mc_"+(m_config->GetFlavourPairs())[i_p];

    hist_pretag_mc.push_back(new TH1D(name_pretag.Data(),"",fj_bins.size()-1,&(fj_bins[0])));
    hist_posttag_mc.push_back(new TH1D(name_posttag.Data(),"",fj_bins.size()-1,&(fj_bins[0])));

    name_pretag="hist_pretag_mc_unscaled_"+(m_config->GetFlavourPairs())[i_p];
    name_posttag="hist_posttag_mc_unscaled_"+(m_config->GetFlavourPairs())[i_p];

    hist_pretag_mc_unscaled.push_back(new TH1D(name_pretag.Data(),"",fj_bins.size()-1,&(fj_bins[0])));
    hist_posttag_mc_unscaled.push_back(new TH1D(name_posttag.Data(),"",fj_bins.size()-1,&(fj_bins[0])));


  }  

  TH1D *hist_pretag_data=new TH1D("hist_pretag_data","",fj_bins.size()-1,&(fj_bins[0]));
  TH1D *hist_posttag_data=new TH1D("hist_posttag_data","",fj_bins.size()-1,&(fj_bins[0])); 

  
  TH1D* help, *help_rebinned;

  double* d_fj_bins = new double[fj_bins.size()];

  for(unsigned int i_b=0; i_b<fj_bins.size(); i_b++) d_fj_bins[i_b]=(double)fj_bins[i_b];

  for(unsigned int i_reg=0; i_reg<regions.size(); i_reg++){
    
    TString mc_name=regions[i_reg]+"_fjpt_Nom";
    TString mc_name_posttag=regions[i_reg]+"_fjpt_PREFITPOSTTAG_Nom";

    for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){
      
      help=(TH1D*)m_fatjet_histograms_pretag[mc_name][i_p]->Clone();
      help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
      
      //get unscaled histogram for MC efficiency
      hist_pretag_mc_unscaled[i_p]->Add(help_rebinned);

      if(doPseudo)help_rebinned->Scale(m_pseudo_fit_params[regions[i_reg]][i_pseudo][i_p]);
      else if(doPseudoData)help_rebinned->Scale(m_pseudo_fit_params_Data[regions[i_reg]][i_pseudo][i_p]);
      else help_rebinned->Scale(m_fit_params[regions[i_reg]+"_"+sys][i_p]);
      
      hist_pretag_mc[i_p]->Add(help_rebinned);

      help=(TH1D*)m_fatjet_histograms_posttag[mc_name_posttag][i_p]->Clone();
      help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
      
      if(doPseudo)help_rebinned->Scale(m_pseudo_fit_params[regions[i_reg]][i_pseudo][i_p]);
      else if(doPseudoData)help_rebinned->Scale(m_pseudo_fit_params[regions[i_reg]][i_pseudo][i_p]);
      else help_rebinned->Scale(m_fit_params[regions[i_reg]+"_"+sys][i_p]);

      //get unscaled histogram for MC efficiency
      hist_posttag_mc_unscaled[i_p]->Add(help_rebinned);


      //std::cout<<"Correction factor "<<i_p<<" : "<< m_fit_params[regions[i_reg]+"_"+sys][i_p]<<std::endl;
      hist_posttag_mc[i_p]->Add(help_rebinned);

    }

    TString data_name=regions[i_reg]+"_fjpt";
    TString data_name_posttag=regions[i_reg]+"_fjpt_PREFITPOSTTAG";

    
    help=(TH1D*)m_fatjet_histograms_pretag_data[data_name]->Clone();
    help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
    hist_pretag_data->Add(help_rebinned);
    
    help=(TH1D*)m_fatjet_histograms_posttag_data[data_name_posttag]->Clone();
    help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
    hist_posttag_data->Add(help_rebinned);

  }


  //subtract backgrounds and calculate scale factor (or data stat error)
  
  float N_BB_pretag_mc, N_BB_posttag_mc, N_BB_pretag_data, N_BB_posttag_data, N_total_pretag_mc, N_total_pretag_data, N_total_posttag_data;

  std::vector<float> BB_SF, BB_SF_datastaterr, data_eff, mc_eff, data_eff_datastaterr;

  float err_factor_tagged, err_factor_untagged;

  for(unsigned int i_bin=1; i_bin<fj_bins.size(); i_bin++){

    N_BB_pretag_mc=hist_pretag_mc_unscaled[0]->GetBinContent(i_bin);
    N_BB_posttag_mc=hist_posttag_mc_unscaled[0]->GetBinContent(i_bin);

    //std::cout<<"N_BB_pretag_mc"<<N_BB_pretag_mc<<std::endl;
    //std::cout<<"N_BB_posttag_mc"<<N_BB_posttag_mc<<std::endl;
    
    N_total_pretag_mc=N_BB_pretag_mc;
    
    N_total_pretag_data=hist_pretag_data->GetBinContent(i_bin);
    N_total_posttag_data=hist_posttag_data->GetBinContent(i_bin);

    N_BB_pretag_data=hist_pretag_data->GetBinContent(i_bin);
    N_BB_posttag_data=hist_posttag_data->GetBinContent(i_bin);

 

    //std::cout<<"N_BB_pretag_data - before subtraction"<<N_BB_pretag_data<<std::endl;
    //std::cout<<"N_BB_posttag_data - before subtraction"<<N_BB_posttag_data<<std::endl;    


    for(unsigned int i_p=1; i_p<m_config->GetFlavourPairs().size(); i_p++){

      //std::cout<<"Pretag -- subtracting component"<<i_p<<" "<<hist_pretag_mc[i_p]->GetBinContent(i_bin)<<std::endl;
      //std::cout<<"Posttag -- subtracting component"<<i_p<<" "<<hist_posttag_mc[i_p]->GetBinContent(i_bin)<<std::endl;
      
      N_total_pretag_mc+=hist_pretag_mc_unscaled[i_p]->GetBinContent(i_bin);

      N_BB_pretag_data-=hist_pretag_mc[i_p]->GetBinContent(i_bin);
      N_BB_posttag_data-=hist_posttag_mc[i_p]->GetBinContent(i_bin);
      
    } 

    //TEST: set N_BB_data=frac_BB_mc*N_data
    //N_BB_pretag_data=N_BB_pretag_mc/N_total_pretag_mc*N_total_pretag_data;

    //std::cout<<"N_BB_pretag_data"<<N_BB_pretag_data<<std::endl;
    //std::cout<<"N_BB_posttag_data"<<N_BB_posttag_data<<std::endl;    

    err_factor_tagged=(1/N_BB_pretag_data-N_BB_posttag_data/(N_BB_pretag_data*N_BB_pretag_data));
    err_factor_untagged=-N_BB_posttag_data/(N_BB_pretag_data*N_BB_pretag_data);

    BB_SF_datastaterr.push_back(TMath::Sqrt(err_factor_tagged*err_factor_tagged*N_total_posttag_data+err_factor_untagged*err_factor_untagged*(N_total_pretag_data-N_total_posttag_data))/(N_BB_posttag_mc/N_BB_pretag_mc));

    BB_SF.push_back((N_BB_posttag_data/N_BB_pretag_data)/(N_BB_posttag_mc/N_BB_pretag_mc));
    
    data_eff.push_back(N_BB_posttag_data/N_BB_pretag_data);
    mc_eff.push_back(N_BB_posttag_mc/N_BB_pretag_mc);

    data_eff_datastaterr.push_back(TMath::Sqrt(err_factor_tagged*err_factor_tagged*N_total_posttag_data+err_factor_untagged*err_factor_untagged*(N_total_pretag_data-N_total_posttag_data)));

    //std::cout<<"Scale factor: "<<BB_SF[i_bin-1]<<std::endl;

   //Binomial errors
   //data_eff_datastaterr.push_back(TMath::Sqrt(N_BB_posttag_data*(1-N_BB_posttag_data/N_BB_pretag_data))/N_BB_pretag_data);
   //BB_SF_datastaterr.push_back(TMath::Sqrt(N_BB_posttag_data*(1-N_BB_posttag_data/N_BB_pretag_data))/N_BB_pretag_data/(N_BB_posttag_mc/N_BB_pretag_mc));
   


  }



  //delete heap objects
  for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){
   
    delete hist_pretag_mc[i_p];
    delete hist_posttag_mc[i_p];
    delete hist_pretag_mc_unscaled[i_p];
    delete hist_posttag_mc_unscaled[i_p];


  }

  delete hist_pretag_data;
  delete hist_posttag_data;

  SFCalcResult result;
  result.fsf=BB_SF;
  result.fsf_datastat_err=BB_SF_datastaterr;
  result.feff_data=data_eff;
  result.feff_mc=mc_eff;
  result.feff_datastat_err=data_eff_datastaterr;
  
  return result;

} 


SFCalcResult ScaleFactorCalculator::CalculateScaleFactorsByRegion(TString &sys, bool doPseudo, unsigned int i_pseudo, bool doPseudoData){
  //Correct MC histograms by fit factors, subtract from data, BB_data

  std::vector<TString> regions=m_config->GetAllRegions();
  
  TH1D* help(nullptr), *help_rebinned(nullptr);

  //subtract backgrounds and calculate scale factor (or data stat error)
  
  float N_BB_pretag_mc, N_BB_posttag_mc, N_BB_pretag_data, N_BB_posttag_data, N_total_pretag_mc=0., N_total_posttag_mc=0., N_total_pretag_data, N_total_posttag_data;

  float N_BB_pretag_mc_unscaled=0. , N_BB_posttag_mc_unscaled=0., N_total_pretag_mc_unscaled=0., N_total_posttag_mc_unscaled=0., N_BB_untag_mc_unscaled=0., N_total_untag_mc_unscaled=0.;

  double Err_BB_posttag_mc_unscaled=0., Err_BB_untag_mc_unscaled=0.;

  std::vector<float> BB_SF, BB_SF_datastaterr, data_eff, mc_eff, data_eff_datastaterr, mc_eff_staterr;

  float err_factor_tagged, err_factor_untagged;

  for(unsigned int i_reg=0; i_reg<regions.size(); i_reg++){
    
    /* TString mc_name=regions[i_reg]+"_fjpt_Nom";
    TString mc_name_posttag=regions[i_reg]+"_fjpt_PREFITPOSTTAG_Nom";

    if(sys.Contains("Rtrk") || sys.Contains("JER") || sys.Contains("JMR")){
      mc_name=regions[i_reg]+"_fjpt_"+sys;
      mc_name_posttag=regions[i_reg]+"_fjpt_PREFITPOSTTAG_"+sys;
    }*/
    
    TString mc_name=regions[i_reg]+"_mjmaxSd0_"+sys;
    TString mc_name_posttag=regions[i_reg]+"_mjmaxSd0_PREFITPOSTTAG_"+sys;
    TString mc_name_untag=regions[i_reg]+"_mjpt_PREFITUNTAG_"+sys;
    

    for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){
      if(i_p==0){
	N_total_pretag_mc=0.;
	N_total_posttag_mc=0.;
	N_total_pretag_mc_unscaled=0.;
	N_total_posttag_mc_unscaled=0.;
	N_total_untag_mc_unscaled=0.;

      }
       
      help=(TH1D*)m_fatjet_histograms_pretag[mc_name][i_p]->Clone();      
      
      N_total_pretag_mc_unscaled+=help->Integral()+help->GetBinContent(0)+help->GetBinContent(help->GetNbinsX()+1);
      if(i_p==0)N_BB_pretag_mc_unscaled=help->Integral()+help->GetBinContent(0)+help->GetBinContent(help->GetNbinsX()+1);
      
      
      if(doPseudo)help->Scale(m_pseudo_fit_params[regions[i_reg]][i_pseudo][i_p]);
      else if(doPseudoData) help->Scale(m_pseudo_fit_params_Data[regions[i_reg]][i_pseudo][i_p]);
      else{
	if(sys.Contains("JET")) help->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
	else help->Scale(m_fit_params[regions[i_reg]+"_"+sys][i_p]);
      }
      //consider also overflow and underflow bin in integral
      N_total_pretag_mc+=help->Integral()+help->GetBinContent(0)+help->GetBinContent(help->GetNbinsX()+1);
      if(i_p==0)N_BB_pretag_mc=help->Integral()+help->GetBinContent(0)+help->GetBinContent(help->GetNbinsX()+1);
      
      if(!doPseudo && !doPseudoData && sys.Contains("Nom")){
	
	help=(TH1D*)m_fatjet_histograms_pretag[mc_name_untag][i_p]->Clone();
	N_total_untag_mc_unscaled+=help->Integral()+help->GetBinContent(0)+help->GetBinContent(help->GetNbinsX()+1);
	if(i_p==0){
	  N_BB_untag_mc_unscaled=help->Integral()+help->GetBinContent(0)+help->GetBinContent(help->GetNbinsX()+1);
	  help->IntegralAndError(0,help->GetNbinsX()+1,Err_BB_untag_mc_unscaled);
	}

      }

      help=(TH1D*)m_fatjet_histograms_posttag[mc_name_posttag][i_p]->Clone();

      N_total_posttag_mc_unscaled+=help->Integral()+help->GetBinContent(0)+help->GetBinContent(help->GetNbinsX()+1);
      if(i_p==0){
	N_BB_posttag_mc_unscaled=help->Integral()+help->GetBinContent(0)+help->GetBinContent(help->GetNbinsX()+1);
	help->IntegralAndError(0,help->GetNbinsX()+1,Err_BB_posttag_mc_unscaled);
      }

      if(doPseudo)help->Scale(m_pseudo_fit_params[regions[i_reg]][i_pseudo][i_p]);
      else if(doPseudoData) help->Scale(m_pseudo_fit_params_Data[regions[i_reg]][i_pseudo][i_p]);
      else{
	if(sys.Contains("JET")) help->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
	else help->Scale(m_fit_params[regions[i_reg]+"_"+sys][i_p]);
      }    

      N_total_posttag_mc+=help->Integral()+help->GetBinContent(0)+help->GetBinContent(help->GetNbinsX()+1);
      if(i_p==0)N_BB_posttag_mc=help->Integral()+help->GetBinContent(0)+help->GetBinContent(help->GetNbinsX()+1);
      
    }

    /*TString data_name=regions[i_reg]+"_fjpt";
      TString data_name_posttag=regions[i_reg]+"_fjpt_PREFITPOSTTAG";*/
    
    TString data_name=regions[i_reg]+"_mjmaxSd0";
    TString data_name_posttag=regions[i_reg]+"_mjmaxSd0_PREFITPOSTTAG";
    
    
    help=(TH1D*)m_fatjet_histograms_pretag_data[data_name]->Clone();
    //N_BB_pretag_data=help->Integral()+help->GetBinContent(0)+help->GetBinContent(help->GetNbinsX()+1)-(N_total_pretag_mc-N_BB_pretag_mc);
    N_total_pretag_data=help->Integral()+help->GetBinContent(0)+help->GetBinContent(help->GetNbinsX()+1);
    
    help=(TH1D*)m_fatjet_histograms_posttag_data[data_name_posttag]->Clone();
    //N_BB_posttag_data=help->Integral()+help->GetBinContent(0)+help->GetBinContent(help->GetNbinsX()+1);//-(N_total_posttag_mc-N_BB_posttag_mc);
    N_total_posttag_data=help->Integral()+help->GetBinContent(0)+help->GetBinContent(help->GetNbinsX()+1);

    //calculate efficiencies and errors
    
    //pretag: post-Sd0 fit MC, posttag: option a) after background subtraction b)BB after scaling to posttag data norm
    N_BB_pretag_data=N_BB_pretag_mc;//N_total_pretag_mc*N_total_pretag_data;

    //option a)
    N_BB_posttag_data=N_total_posttag_data-(N_total_posttag_mc-N_BB_posttag_mc);

    //option b)
    //N_BB_posttag_data=N_BB_posttag_mc/N_total_posttag_mc*N_total_posttag_data;

    if(sys.Contains("JET")){
      N_BB_pretag_data=N_BB_pretag_mc/N_total_pretag_mc*N_total_pretag_data; //scale to match pretag norm
      
      //option a)
      N_BB_posttag_data=N_total_posttag_data-(N_total_posttag_mc-N_BB_posttag_mc)/N_total_pretag_mc*N_total_pretag_data; //scale MC to match pretag norm

      //option b)
      //N_BB_posttag_data=N_BB_posttag_mc/N_total_pretag_mc*N_total_pretag_data/N_total_posttag_mc*N_total_posttag_data; //scale to match pretag norm
    }




    //std::cout<<"N_BB_pretag_data"<<N_BB_pretag_data<<std::endl;
    //std::cout<<"N_BB_posttag_data"<<N_BB_posttag_data<<std::endl;    

    //err_factor_tagged=(1/N_BB_pretag_data-N_BB_posttag_data/(N_BB_pretag_data*N_BB_pretag_data));
    //err_factor_untagged=-N_BB_posttag_data/(N_BB_pretag_data*N_BB_pretag_data);

    //BB_SF_datastaterr.push_back(TMath::Sqrt(err_factor_tagged*err_factor_tagged*N_total_posttag_data+err_factor_untagged*err_factor_untagged*(N_total_pretag_data-N_total_posttag_data))/(N_BB_posttag_mc_unscaled/N_BB_pretag_mc_unscaled));

    BB_SF.push_back((N_BB_posttag_data/N_BB_pretag_data)/(N_BB_posttag_mc_unscaled/N_BB_pretag_mc_unscaled));
    
    if(i_reg==11 && !doPseudo && !doPseudoData){
      std::cout<<"Sys is:"<<sys<<std::endl;
      std::cout<<"Diff BB is:"<<(m_fit_params[regions[i_reg]+"_Nom"][0]-m_fit_params[regions[i_reg]+"_"+sys][0])/m_fit_params[regions[i_reg]+"_Nom"][0]<<std::endl;
      std::cout<<"Diff BL is:"<<(m_fit_params[regions[i_reg]+"_Nom"][1]-m_fit_params[regions[i_reg]+"_"+sys][1])/m_fit_params[regions[i_reg]+"_Nom"][1]<<std::endl;
      std::cout<<"Diff CC is:"<<(m_fit_params[regions[i_reg]+"_Nom"][2]-m_fit_params[regions[i_reg]+"_"+sys][2])/m_fit_params[regions[i_reg]+"_Nom"][2]<<std::endl;
      std::cout<<"Diff CL is:"<<(m_fit_params[regions[i_reg]+"_Nom"][3]-m_fit_params[regions[i_reg]+"_"+sys][3])/m_fit_params[regions[i_reg]+"_Nom"][3]<<std::endl;
      std::cout<<"Diff LL is:"<<(m_fit_params[regions[i_reg]+"_Nom"][4]-m_fit_params[regions[i_reg]+"_"+sys][4])/m_fit_params[regions[i_reg]+"_Nom"][4]<<std::endl;

    }

    data_eff.push_back(N_BB_posttag_data/N_BB_pretag_data);
    mc_eff.push_back(N_BB_posttag_mc_unscaled/N_BB_pretag_mc_unscaled);

    //data_eff_datastaterr.push_back(TMath::Sqrt(err_factor_tagged*err_factor_tagged*N_total_posttag_data+err_factor_untagged*err_factor_untagged*(N_total_pretag_data-N_total_posttag_data)));
    
    //Binomial errors
    //data_eff_datastaterr.push_back(TMath::Sqrt(N_BB_posttag_data*(1-N_BB_posttag_data/N_BB_pretag_data))/N_BB_pretag_data);
    //BB_SF_datastaterr.push_back(TMath::Sqrt(N_BB_posttag_data*(1-N_BB_posttag_data/N_BB_pretag_data))/N_BB_pretag_data/(N_BB_posttag_mc_unscaled/N_BB_pretag_mc_unscaled));
   

    data_eff_datastaterr.push_back(0.);
    BB_SF_datastaterr.push_back(0.);


    //Calculate MC stat uncertainty on MC efficiency
    float delta_eff_mc_term1=(1./N_BB_pretag_mc_unscaled-N_BB_posttag_mc_unscaled/(N_BB_untag_mc_unscaled*N_BB_untag_mc_unscaled))*(1./N_BB_pretag_mc_unscaled-N_BB_posttag_mc_unscaled/(N_BB_untag_mc_unscaled*N_BB_untag_mc_unscaled))*Err_BB_posttag_mc_unscaled*Err_BB_posttag_mc_unscaled;
    float delta_eff_mc_term2=(N_BB_posttag_mc_unscaled/(N_BB_untag_mc_unscaled*N_BB_untag_mc_unscaled))*(N_BB_posttag_mc_unscaled/(N_BB_untag_mc_unscaled*N_BB_untag_mc_unscaled))*Err_BB_untag_mc_unscaled*Err_BB_untag_mc_unscaled;
    mc_eff_staterr.push_back(TMath::Sqrt(delta_eff_mc_term1+delta_eff_mc_term2));
    

  }

  SFCalcResult result;
  result.fsf=BB_SF;
  result.fsf_datastat_err=BB_SF_datastaterr;
  result.feff_data=data_eff;
  result.feff_mc=mc_eff;
  result.feff_datastat_err=data_eff_datastaterr;
  result.feff_mc_stat_err=mc_eff_staterr;

  return result;

} 




CalibResult ScaleFactorCalculator::CalculateScaleFactorsAndErrors(bool doByRegion){

  std::vector<float> sf_nom, err_up, err_down, sf_nom_datastaterr;

  std::vector<float> data_eff_nom, data_eff_err_up, data_eff_err_down, data_eff_nom_datastaterr;
  std::vector<float> mc_eff_nom, mc_eff_stat_err;

  std::vector<float> fj_bins;
  if(doByRegion){
    int size=m_config->GetAllRegions().size();
    for(int i=0; i<size; i++) fj_bins.push_back((float)i);
    fj_bins.push_back(fj_bins.back()+1.);
  }else fj_bins=m_config->GetFatJetPtBins();
  
  std::vector<float> current_SF,current_data_eff, current_mc_eff;  
  std::vector<float> err_sq_up(fj_bins.size()-1,0.);
  std::vector<float> err_sq_down(fj_bins.size()-1,0.);
  std::vector<float> data_eff_err_sq_up(fj_bins.size()-1,0.);
  std::vector<float> data_eff_err_sq_down(fj_bins.size()-1,0.);

  std::vector<TString> systematics=m_config->GetSystematics();
 
  sf_nom.clear();
  err_up.clear();
  err_down.clear();

  SFCalcResult sf_result;
  
  //calculate SF and sys errors
  std::map<TString,std::vector<double>> error_map_up;
  std::map<TString,std::vector<double>> error_map_down;


   for(unsigned int i_sys=0; i_sys<systematics.size(); i_sys++){
    //FOR NOW: Don't consider Herwig systematics
    if(systematics[i_sys].Contains("Herwig")) continue;

    if(doByRegion) sf_result=this->CalculateScaleFactorsByRegion(systematics[i_sys]);
    else sf_result=this->CalculateScaleFactors(systematics[i_sys]);

    if(systematics[i_sys].EqualTo("Nom")){
      sf_nom=sf_result.fsf;
      //sf_nom_datastaterr=sf_result.fsf_datastat_err;
      data_eff_nom=sf_result.feff_data;
      //data_eff_nom_datastaterr=sf_result.feff_datastat_err;
      mc_eff_nom=sf_result.feff_mc;
      mc_eff_stat_err=sf_result.feff_mc_stat_err;

      current_SF=sf_nom;
      current_data_eff=data_eff_nom;
      current_mc_eff=sf_result.feff_mc;
      

    }else{
      current_SF=sf_result.fsf;
      current_data_eff=sf_result.feff_data; 
      current_mc_eff=sf_result.feff_mc;
    }
    for(unsigned int i_bin=0; i_bin<fj_bins.size()-1; i_bin++){

      //if(!systematics[i_sys].EqualTo("Nom")) current_SF[i_bin]=current_SF[i_bin]*current_mc_eff[i_bin]/mc_eff_nom[i_bin];
      
      if(i_bin==11)std::cout<<"Region: "<<m_config->GetAllRegions()[i_bin]<<std::endl;

      float diff=current_SF[i_bin]-sf_nom[i_bin];
      if(i_bin==11) std::cout<<"SF diff: "<<diff<<std::endl;
      if(i_bin==11) std::cout<<"data eff diff: "<<current_data_eff[i_bin]-data_eff_nom[i_bin]<<std::endl;
      if(i_bin==11) std::cout<<"MC eff diff: "<<current_mc_eff[i_bin]-mc_eff_nom[i_bin]<<std::endl;

      /*if(TMath::Abs(diff)>0.058){
	std::cout<<"Region: "<<m_config->GetAllRegions()[i_bin]<<std::endl;
	std::cout<<"Big Diff SF for sys "<<systematics[i_sys]<< " :"<<diff<<std::endl;
	}*/
      if(diff>0) err_sq_up[i_bin]+=diff*diff;
      else err_sq_down[i_bin]+=diff*diff;
      
      float diff_data_eff=current_data_eff[i_bin]-data_eff_nom[i_bin]; 
      
      //std::cout<<"Diff data eff for sys "<<systematics[i_sys]<< " :"<<diff<<std::endl;


      /*if(TMath::Abs(diff)>0.1 || diff_data_eff> 0.1){
	std::cout<<"Region: "<<m_config->GetAllRegions()[i_bin]<<std::endl;
	std::cout<<"Big Diff SF for sys "<<systematics[i_sys]<< " :"<<diff<<std::endl;
	std::cout<<"Big Diff SF for sys "<<systematics[i_sys]<< " :"<<diff_data_eff<<std::endl;
	}*/


      if(diff_data_eff>0) data_eff_err_sq_up[i_bin]+=diff_data_eff*diff_data_eff;
      else data_eff_err_sq_down[i_bin]+=diff_data_eff*diff_data_eff;

      error_map_up[m_config->GetAllRegions()[i_bin]].push_back(TMath::Sqrt(err_sq_up[i_bin]));
      error_map_down[m_config->GetAllRegions()[i_bin]].push_back(TMath::Sqrt(err_sq_down[i_bin]));


      /*std::cout<<"After: "<<systematics[i_sys]<<std::endl;
      std::cout<<"Up is now "<<TMath::Sqrt(data_eff_err_sq_up[i_bin])<<std::endl;
      std::cout<<"Down is now "<<TMath::Sqrt(data_eff_err_sq_down[i_bin])<<std::endl;
      */

    }    
  }

   std::cout<<"Total: "<<std::endl;
   std::cout<<"Up is now "<<error_map_up[m_config->GetAllRegions()[11]].back()<<std::endl;
   std::cout<<"Down is now "<<error_map_down[m_config->GetAllRegions()[11]].back()<<std::endl;
   

   /*for(unsigned int i_bin=0; i_bin<fj_bins.size()-1; i_bin++){

    std::cout<<"Region: "<<m_config->GetAllRegions()[i_bin]<<std::endl;
    std::cout<<"Nominal is: "<<sf_nom[i_bin]<<std::endl;
      std::cout<<"Up is: "<<TMath::Sqrt(err_sq_up[i_bin])<<std::endl;
      std::cout<<"Down is: "<<TMath::Sqrt(err_sq_down[i_bin])<<std::endl;
    */ 
    /*for(unsigned int i_sys=0; i_sys<systematics.size(); i_sys++){
      
       std::cout<<"After: "<<systematics[i_sys]<<std::endl;
       std::cout<<"Up is now "<<error_map_up[m_config->GetAllRegions()[i_bin]].at(i_sys)<<std::endl;
       std::cout<<"Down is now "<<error_map_down[m_config->GetAllRegions()[i_bin]].at(i_sys)<<std::endl;

       }

  }*/


  SFCalcResult pseudo_templates_result, pseudo_data_result;
  



  std::vector<float> pseudo_err_sq(fj_bins.size()-1,0.);
  std::vector<float> pseudo_err_sq_eff_data(fj_bins.size()-1,0.);
  //std::vector<float> pseudo_err_sq_eff_mc(fj_bins.size()-1,0.);

  std::vector<float> pseudodata_err_sq(fj_bins.size()-1,0.);
  std::vector<float> pseudodata_err_sq_eff_data(fj_bins.size()-1,0.);


  
  TString nominal="Nom";

  //Calculate template variance with the online algorithm
  /*  std::vector<float> n(fj_bins.size()-1,0.);
  std::vector<float> M2(fj_bins.size()-1,0.);
  std::vector<float> mean(fj_bins.size()-1,0.);
  std::vector<float> delta(fj_bins.size()-1,0.);
  std::vector<float> delta2(fj_bins.size()-1,0.); */

  std::vector<double> help;

  std::vector<std::vector<double>> PseudoTemplateScaleFactors(fj_bins.size()-1,help);
  std::vector<std::vector<double>> PseudoTemplateEfficiencies_Data(fj_bins.size()-1,help);
  
  std::vector<std::vector<double>> PseudoDataScaleFactors(fj_bins.size()-1,help);
  std::vector<std::vector<double>> PseudoDataEfficiencies_Data(fj_bins.size()-1,help);
  
  
  //std::vector<std::vector<double>> PseudoEfficiencies_MC(fj_bins.size()-1,help);

  //loop over all fits with pseudo-templates
 
  for(unsigned int i=0; i<m_pseudo_fit_params[m_config->GetAllRegions()[0]].size(); i++){

    //std::cout<<"Templates Pseudo Experiment"<<i<<std::endl;

    if(!(i%100)) std::cout<<"Calculate ScaleFactors Fit Uncertainty: Fit "<<i<<std::endl;
  
    if(doByRegion) pseudo_templates_result=this->CalculateScaleFactorsByRegion(nominal,true,i);
    else pseudo_templates_result=this->CalculateScaleFactors(nominal,true,i);

    
    //PseudoTemplateScaleFactors.push_back(pseudo_templates_result.fsf);
    //PseudoTemplateEfficiencies_Data.push_back((double)feff_data);
    //PseudoEfficiencies_MC.push_back((double)feff_mc);

    for(unsigned int i_bin=0; i_bin<(fj_bins.size()-1); i_bin++){
      PseudoTemplateScaleFactors[i_bin].push_back((double)pseudo_templates_result.fsf[i_bin]);
      PseudoTemplateEfficiencies_Data[i_bin].push_back((double)pseudo_templates_result.feff_data[i_bin]);
      //PseudoEfficiencies_MC[i_bin].push_back((double)pseudo_templates_result.feff_mc[i_bin]);
      //std::cout<<"Pseudo-Efficiency MC"<<pseudo_templates_result.feff_mc[i_bin]<<std::endl;


    }
    

  }

  //loop over all fits with pseudo-data
  for(unsigned int i=0; i<m_pseudo_fit_params_Data[m_config->GetAllRegions()[0]].size(); i++){

    if(!(i%100)) std::cout<<"Calculate ScaleFactors Data Fit Uncertainty: Fit "<<i<<std::endl;
    
    if(doByRegion) pseudo_data_result=this->CalculateScaleFactorsByRegion(nominal,false,i,true);
    else pseudo_data_result=this->CalculateScaleFactors(nominal,false,i,true);

    for(unsigned int i_bin=0; i_bin<(fj_bins.size()-1); i_bin++){
      PseudoDataScaleFactors[i_bin].push_back((double)pseudo_data_result.fsf[i_bin]);
      PseudoDataEfficiencies_Data[i_bin].push_back((double)pseudo_data_result.feff_data[i_bin]);

    }
    

  }





    /*current_SF=pseudo_templates_result.fsf;
  
    for(unsigned int i_bin=0; i_bin<(fj_bins.size()-1); i_bin++){
      n[i_bin]+=1.;
      delta[i_bin]=current_SF[i_bin]-mean[i_bin];
      mean[i_bin]+=delta[i_bin]/n[i_bin];
      delta2[i_bin]=current_SF[i_bin]-mean[i_bin];
      M2[i_bin]+=delta[i_bin]*delta2[i_bin];

      if(i==(m_pseudo_fit_params[m_config->GetAllRegions()[0]].size()-1)) pseudo_err_sq[i_bin]=M2[i_bin]/(n[i_bin]-1);
      }*/
 
  

  for(unsigned int i_bin=0; i_bin<(fj_bins.size()-1); i_bin++){

    TH1D help_pseudo("help_pseudo","",10,0.,-1.);
    TH1D help_pseudodata("help_pseudodata","",10,0.,-1.);
    TH1D help_pseudo_eff("help_pseudo_eff","",10,0.,-1.);
    TH1D help_pseudodata_eff("help_pseudodata_eff","",10,0.,-1.);

    for(unsigned int i_p=0; i_p<PseudoTemplateScaleFactors[i_bin].size(); i_p++) help_pseudo.Fill(PseudoTemplateScaleFactors[i_bin][i_p]);
    for(unsigned int i_p=0; i_p<PseudoTemplateEfficiencies_Data[i_bin].size(); i_p++) help_pseudo_eff.Fill(PseudoTemplateEfficiencies_Data[i_bin][i_p]);
    for(unsigned int i_p=0; i_p<PseudoDataScaleFactors[i_bin].size(); i_p++) help_pseudodata.Fill(PseudoDataScaleFactors[i_bin][i_p]);
    for(unsigned int i_p=0; i_p<PseudoDataEfficiencies_Data[i_bin].size(); i_p++) help_pseudodata_eff.Fill(PseudoDataEfficiencies_Data[i_bin][i_p]);
    
    help_pseudo.Fit("gaus");
    help_pseudo_eff.Fit("gaus");
    help_pseudodata.Fit("gaus");
    help_pseudodata_eff.Fit("gaus");

    //pseudo_err_sq[i_bin]=TMath::RMS(PseudoTemplateScaleFactors[i_bin].size(),&(PseudoTemplateScaleFactors[i_bin][0]))/2.;
    pseudo_err_sq[i_bin]=help_pseudo.GetFunction("gaus")->GetParameter(2);
    pseudo_err_sq[i_bin]*=pseudo_err_sq[i_bin];
    
    //pseudo_err_sq_eff_data[i_bin]=TMath::RMS(PseudoTemplateEfficiencies_Data[i_bin].size(),&(PseudoTemplateEfficiencies_Data[i_bin][0]))/2.;
    pseudo_err_sq_eff_data[i_bin]=help_pseudo_eff.GetFunction("gaus")->GetParameter(2);
    pseudo_err_sq_eff_data[i_bin]*=pseudo_err_sq_eff_data[i_bin];

    //pseudodata_err_sq[i_bin]=TMath::RMS(PseudoDataScaleFactors[i_bin].size(),&(PseudoDataScaleFactors[i_bin][0]))/2.;
    pseudodata_err_sq[i_bin]=help_pseudodata.GetFunction("gaus")->GetParameter(2);
    pseudodata_err_sq[i_bin]*=pseudodata_err_sq[i_bin];
    
    //pseudodata_err_sq_eff_data[i_bin]=TMath::RMS(PseudoDataEfficiencies_Data[i_bin].size(),&(PseudoDataEfficiencies_Data[i_bin][0]))/2.;
    pseudodata_err_sq_eff_data[i_bin]=help_pseudodata_eff.GetFunction("gaus")->GetParameter(2);
    pseudodata_err_sq_eff_data[i_bin]*=pseudodata_err_sq_eff_data[i_bin];


    //pseudo_err_sq_eff_mc[i_bin]=TMath::RMS(PseudoEfficiencies_MC[i_bin].size(),&(PseudoEfficiencies_MC[i_bin][0]));
    //pseudo_err_sq_eff_mc[i_bin]*=pseudo_err_sq_eff_mc[i_bin];
    

  }
  
  for(unsigned int i_bin=0; i_bin<fj_bins.size()-1; i_bin++){

    //Only Data Stat error
    sf_nom_datastaterr.push_back(TMath::Sqrt(pseudodata_err_sq[i_bin]));
    data_eff_nom_datastaterr.push_back(TMath::Sqrt(pseudodata_err_sq_eff_data[i_bin]));
    
    //Scale Factor total error
    err_sq_up[i_bin]+=sf_nom_datastaterr[i_bin]*sf_nom_datastaterr[i_bin];
    err_sq_down[i_bin]+=sf_nom_datastaterr[i_bin]*sf_nom_datastaterr[i_bin];

    err_sq_up[i_bin]+=pseudo_err_sq[i_bin];
    err_sq_down[i_bin]+=pseudo_err_sq[i_bin];

    //add MC efficiency error
    err_sq_up[i_bin]+=(sf_nom[i_bin]/mc_eff_nom[i_bin]*mc_eff_stat_err[i_bin])*(sf_nom[i_bin]/mc_eff_nom[i_bin]*mc_eff_stat_err[i_bin]);
    err_sq_down[i_bin]+=(sf_nom[i_bin]/mc_eff_nom[i_bin]*mc_eff_stat_err[i_bin])*(sf_nom[i_bin]/mc_eff_nom[i_bin]*mc_eff_stat_err[i_bin]);
   
    
    err_up.push_back(TMath::Sqrt(err_sq_up[i_bin]));
    err_down.push_back(TMath::Sqrt(err_sq_down[i_bin]));

    if(i_bin==11){
      std::cout<<"err_up final"<<err_up.back()<<std::endl;
      std::cout<<"err_down final"<<err_down.back()<<std::endl;

    }

    //Data efficiency total error
    data_eff_err_sq_up[i_bin]+=data_eff_nom_datastaterr[i_bin]*data_eff_nom_datastaterr[i_bin];
    data_eff_err_sq_down[i_bin]+=data_eff_nom_datastaterr[i_bin]*data_eff_nom_datastaterr[i_bin];
    
    data_eff_err_sq_up[i_bin]+=pseudo_err_sq_eff_data[i_bin];
    data_eff_err_sq_down[i_bin]+=pseudo_err_sq_eff_data[i_bin];


    data_eff_err_up.push_back(TMath::Sqrt(data_eff_err_sq_up[i_bin]));
    data_eff_err_down.push_back(TMath::Sqrt(data_eff_err_sq_down[i_bin]));

    

    //mc_eff_stat_err.push_back(TMath::Sqrt(pseudo_err_sq_eff_mc[i_bin]));
    mc_eff_stat_err.push_back(0.);



  }


  CalibResult cal_result;
  cal_result.fnominal_sf=sf_nom;
  cal_result.ftot_err_up=err_up;
  cal_result.ftot_err_down=err_down;
  cal_result.fstat_err=sf_nom_datastaterr;

  cal_result.fnominal_data_eff=data_eff_nom;
  cal_result.ftot_err_data_eff_up=data_eff_err_up;
  cal_result.ftot_err_data_eff_down=data_eff_err_down;
  cal_result.fstat_err_data_eff=data_eff_nom_datastaterr;
  cal_result.fnominal_mc_eff=mc_eff_nom;
  //todo: mc eff stat errors
  cal_result.fstat_err_mc_eff=mc_eff_stat_err;

  std::cout<<"INFO: CalculateScaleFactorsAndErrors(): Finished calculating Scale Factors! "<<std::endl;


  return cal_result;


}

void ScaleFactorCalculator::ReadInFatJetHists(std::vector<TString>& var, std::vector<TString>& sys){
  
  std::vector<TString> hist_mc;
  TString hist_data;

  std::vector<TString> regions=m_config->GetAllRegions();

  TFile *infile=TFile::Open(m_config->GetInfileName(),"READ");

  std::cout<<"infile name"<<m_config->GetInfileName()<<std::endl;

  TH1D* tmp=0, *clone_tmp=0;

  
  for(unsigned int i_var=0; i_var<var.size(); i_var++){
  
    TString jettype="fatjet";
    
    for(unsigned int i_reg=0; i_reg<regions.size(); i_reg++){
      
      //Get Data Hist for region i_reg
      if( ! (var[i_var].Contains("BTAGUP") || var[i_var].Contains("BTAGDOWN") ) ){
	hist_data=m_config->GetDataHistName(regions[i_reg], var[i_var]);
	
	infile->GetObject(hist_data.Data(),tmp);
	
	//std::cout<<"Read in"<<hist_data<<"!!"<<std::endl;
	
	//if(tmp) std::cout<<"Read in "<<hist_data<<std::endl;

	if(tmp){
	  clone_tmp=(TH1D*)tmp->Clone();
	  clone_tmp->SetDirectory(0);
	  
	  TString name_data=regions[i_reg]+"_"+var[i_var];
	  
	  if(var[i_var].Contains("POSTTAG")) m_fatjet_histograms_posttag_data[name_data]=std::shared_ptr<TH1D>((TH1D*)clone_tmp);
	  else m_fatjet_histograms_pretag_data[name_data]=std::shared_ptr<TH1D>((TH1D*)clone_tmp);
	}else std::cout<<"Error in ScaleFactorCalculator::ReadInFatJetHists(): Can't find histogram"<<hist_data<<"ddd"<<std::endl;

      }

      for(unsigned int i_sys=0; i_sys<sys.size(); i_sys++){
        
        
        
        //Get MC Hists for region
        hist_mc=m_config->GetMCHistNames(regions[i_reg],sys[i_sys],var[i_var]);
        
        for(unsigned int i_h=0; i_h<hist_mc.size(); i_h++){
          
          infile->GetObject(hist_mc[i_h],tmp);
          
          //if(tmp) std::cout<<"Read in "<<hist_mc[i_h]<<std::endl;
          if(!tmp) std::cout<<"Error in ScaleFactorCalculator::ReadInFatJetHists(): Can't find histogram "<<hist_mc[i_h]<<"ddd"<<std::endl;
          if(hist_mc[i_h].Contains("PREFITUNTAG")) std::cout<<"reading in untag hist: "<< hist_mc[i_h]<<std::endl;

          clone_tmp=(TH1D*)tmp->Clone();
          clone_tmp->SetDirectory(0);
          
          TString name_mc=regions[i_reg]+"_"+var[i_var]+"_"+sys[i_sys];
	  //std::cout<<"name_mc read-in: "<<name_mc<<std::endl;
	  if(name_mc.Contains("UNTAG")) std::cout<<"name_mc read-in: "<<name_mc<<std::endl;      

          if(var[i_var].Contains("POSTTAG")) m_fatjet_histograms_posttag[name_mc].push_back(std::shared_ptr<TH1D>((TH1D*)clone_tmp));
          else  m_fatjet_histograms_pretag[name_mc].push_back(std::shared_ptr<TH1D>((TH1D*)clone_tmp));
          
          
        }
        
      }
    }
  }
  
  infile->Close();

}

TString ScaleFactorCalculator::MakeFlavourFractionTable(bool applyFitCorrection, std::shared_ptr<TH1D> dataHist,std::vector<std::shared_ptr<TH1D>> templateHists, TString& channel, TString& region){

  std::cout<<"Flavour fraction region: "<<region<<std::endl;
  
  TH1D* tmp_stacked_mc(nullptr);
  
  TH1D *full_mc(nullptr);

  std::vector<float> flavour_fractions;

  for(unsigned int i_p=0; i_p<templateHists.size(); i_p++){

    if(!(templateHists[i_p].get())) continue;
    
    tmp_stacked_mc=(TH1D*)templateHists[i_p].get()->Clone();

    if(applyFitCorrection) tmp_stacked_mc->Scale(m_fit_params[region+"_Nom"][i_p]);

    if(i_p==0) full_mc=(TH1D*)tmp_stacked_mc->Clone();
    else full_mc->Add(tmp_stacked_mc);

    flavour_fractions.push_back(tmp_stacked_mc->Integral());

  }

  std::vector<float> matrix;

  for(unsigned int i_entry=0; i_entry<m_nom_cov_mats[region].size(); i_entry++){
    matrix.push_back((float)(m_nom_cov_mats[region][i_entry])); //get region covariance matrix
  }

  TMatrixF mat(flavour_fractions.size(),flavour_fractions.size(),&matrix[0]);
  std::vector<double> flavour_fraction_err(flavour_fractions.size(),0.);

  TVectorF v(flavour_fractions.size(),&flavour_fractions[0]);
  TVectorF v2=v;

  float N_total=full_mc->Integral();

  //calculate error for each flavour fraction according to propagation formula
  for(unsigned int i=0; i<flavour_fractions.size(); i++){
    std::vector<int> keepRowCol;
    float err_flavour=TMath::Sqrt(mat[i][i])*flavour_fractions[i];
    std::cout<<"err_flavour"<<err_flavour<<std::endl;
    float N_flavour=flavour_fractions[i];
    std::vector<float> reduced_flavour_fractions;

    for(unsigned int j=0; j<flavour_fractions.size(); j++){
      if(i==j) continue;
      else{
	keepRowCol.push_back(j);
	reduced_flavour_fractions.push_back(flavour_fractions[j]);
      }
    }

    std::cout<<"Reduced flavour fractions size: "<<reduced_flavour_fractions.size()<<std::endl;
    TVectorF v_red(reduced_flavour_fractions.size(),&reduced_flavour_fractions[0]);
    TVectorF v2_red=v_red;
    
    //delete row
    TMatrixF reduced_mat(mat);
    for(unsigned int k=0; k<keepRowCol.size(); k++){
      TMatrixFColumn(reduced_mat,k) = TMatrixFColumn(reduced_mat,keepRowCol[k]);
    }
    reduced_mat.ResizeTo(reduced_mat.GetNrows(),keepRowCol.size());
    std::cout<<"Nrows: "<<reduced_mat.GetNrows()<<" Ncolumns: "<<reduced_mat.GetNcols()<<std::endl;

    //delete column
    for(unsigned int l=0; l<keepRowCol.size(); l++){
      TMatrixFRow(reduced_mat,l) = TMatrixFRow(reduced_mat,keepRowCol[l]);
    }
    reduced_mat.ResizeTo(keepRowCol.size(),reduced_mat.GetNcols());

    std::cout<<"Nrows: "<<reduced_mat.GetNrows()<<" Ncolumns: "<<reduced_mat.GetNcols()<<std::endl;
    
    v2_red*=reduced_mat;

    float err_non_flavour=v2_red*v_red;
    std::cout<<"err_non_flavour "<<err_non_flavour<<std::endl;

    float err_total=(1./N_total-N_flavour/(N_total*N_total))*(1./N_total-N_flavour/(N_total*N_total))*err_flavour*err_flavour+(-N_flavour/(N_total*N_total))*(-N_flavour/(N_total*N_total))*err_non_flavour*err_non_flavour;

    flavour_fraction_err[i]=TMath::Sqrt(err_total);
    std::cout<<"flavour_fraction_err "<<flavour_fraction_err[i]<<std::endl;  
  }

  //std::cout<<"  BB  "<<"  BL  "<<"  CC  "<<"  CL  "<<"  LL  "<<std::endl;
  TString name=region+" ";
  TString help;

  for(unsigned int i=0; i<flavour_fractions.size(); i++){
    //std::cout<<" "<<flavour_fractions[i]/full_mc->Integral()<<"+-"<<TMath::Sqrt(mat[i][i])/full_mc->Integral();
    //std::cout<<std::endl;
    //help.Form("  %.3f+-%.3f  ",flavour_fractions[i]/full_mc->Integral(),flavour_fraction_err[i]);
    help.Form("& $ %.3f \\pm %.3f  $",flavour_fractions[i]/full_mc->Integral(),TMath::Sqrt(mat[i][i])*flavour_fractions[i]/full_mc->Integral());
    name.Append(help);
  }

  return name;
}


TString ScaleFactorCalculator::PrintMuAndError(TString region,std::vector<std::shared_ptr<TH1D>> templateHists){


  TH1D* tmp_stacked_mc(nullptr);
  
  TH1D *full_mc(nullptr), *full_mc_prefit(nullptr);

  std::vector<float> flavour_fractions;

  for(unsigned int i_p=0; i_p<templateHists.size(); i_p++){

    if(!(templateHists[i_p].get())) continue;
    
    tmp_stacked_mc=(TH1D*)templateHists[i_p].get()->Clone();

    if(i_p==0) full_mc_prefit=(TH1D*)tmp_stacked_mc->Clone();
    else full_mc_prefit->Add(tmp_stacked_mc);

    tmp_stacked_mc->Scale(m_fit_params[region+"_Nom"][i_p]);

    if(i_p==0) full_mc=(TH1D*)tmp_stacked_mc->Clone();
    else full_mc->Add(tmp_stacked_mc);


  }

  double total_norm_correction=full_mc->Integral()/full_mc_prefit->Integral();

  std::vector<double> matrix;

  for(unsigned int i_entry=0; i_entry<m_nom_cov_mats[region].size(); i_entry++){
    matrix.push_back(m_nom_cov_mats[region][i_entry]); //get region covariance matrix
  }

  

  std::vector<double> mus=m_fit_params[region+"_Nom"];

  TMatrixD mat(mus.size(),mus.size(),&matrix[0]);

  TString name=region+" ";
  TString help;

  for(unsigned int i=0; i<mus.size(); i++){
    //std::cout<<" "<<flavour_fractions[i]/full_mc->Integral()<<"+-"<<TMath::Sqrt(mat[i][i])/full_mc->Integral();
    //std::cout<<std::endl;
    //help.Form("  %.3f+-%.3f  ",flavour_fractions[i]/full_mc->Integral(),flavour_fraction_err[i]);
    help.Form("& $ %.3f \\pm %.3f  $",mus[i]/total_norm_correction,TMath::Sqrt(mat[i][i])/total_norm_correction);
    name.Append(help);
  }

  return name;

}

void ScaleFactorCalculator::SaveFitCorrectionFactorsSys(){

  std::vector<TString> names={"BB_corrfact","BL_corrfact","CC_corrfact","CL_corrfact","LL_corrfact"};
 
  std::vector<TString> regions=m_config->GetAllRegions();
  std::vector<TString> systematics=m_config->GetSystematics();

  TString name;
  TCanvas *canv=new TCanvas("canv","",400,300);

  for(unsigned int i_reg=0; i_reg<m_config->GetAllRegions().size(); i_reg++){
  std::vector<TH1D*> hists;
    for(auto &el : names){
      name=el+"_"+m_config->GetAllRegions().at(i_reg);
      hists.push_back(new TH1D(name.Data(),"",25,0.,-1.));
    }

    for(unsigned int i_sys=0; i_sys<systematics.size(); i_sys++){
      if(i_sys==0) std::cout<<"Sys is "<<systematics[i_sys]<<std::endl;
      for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++) hists[i_p]->Fill(m_fit_params[regions[i_reg]+"_"+systematics[i_sys]][i_p]/m_fit_params[regions[i_reg]+"_Nom"][i_p]);
    
    }
    
    for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){
      name=TString(hists[i_p]->GetName())+".pdf";
      canv->cd();
      hists[i_p]->Draw();
      canv->SaveAs(name.Data());
    }
  }
}


//  LocalWords:  ReadInHistograms
