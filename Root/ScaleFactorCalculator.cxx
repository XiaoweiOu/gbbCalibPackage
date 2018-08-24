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
#include "TH2D.h"

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

  std::cout<<"==================================================="<<std::endl;
  std::cout<<"| HELLO THIS IS THE GBB SCALE FACTOR CALCULATOR!   "<<std::endl;
  //std::cout<<"| Brumm... "<<std::endl;
  std::cout<<"==================================================="<<std::endl;

  
  m_config=new Config(cfg_file);
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

  std::vector<TString> fitstatus, flav_fractions;



  for(int i_reg=0; i_reg<regions.size(); i_reg++){
    
    tmpl_muo_data=m_config->GetDataHistName(regions[i_reg],mj_maxSd0);
    tmpl_nonmuo_data=m_config->GetDataHistName(regions[i_reg],nmj_maxSd0);
    

    for(int i_sys=0; i_sys<systematics.size(); i_sys++){
      
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

      
      for(int i_par=0; i_par<fitpar_names.size(); i_par++){	

	std::cout<<"Adding: "<<fitpar_names[i_par]<<" "<<fitpar_start[i_par]<<" "<<fitpar_low[i_par]<<" "<<fitpar_high[i_par]<<std::endl;

	m_fitter.AddParameter(fitpar_names[i_par],fitpar_start[i_par],0.001,fitpar_low[i_par],fitpar_high[i_par]);

      } 
      
      m_fitter.Initialize();      

      for(int i_chan=0; i_chan<chans.size(); i_chan++) m_fitdata->AutoRebinHistograms(chans[i_chan],1);
      // if(m_config->GetSmoothNtimes())m_fitdata->KernelSmoothTemplates(m_config->GetSmoothNtimes());
      //m_fitdata->FitTemplateSlopes(-10.,10.);

      m_fitter.PrintParameters("Simple");
      m_fitter.fit();
      m_fitter.PrintParameters("Simple");
      fitstatus.push_back(m_fitter.getFitStatus());
      
      m_fit_params[ (regions[i_reg]+"_"+systematics[i_sys]) ]=m_fitter.FitParameters();
      
      if(systematics[i_sys].EqualTo("Nom")) m_nom_cov_mats[regions[i_reg]]=m_fitter.CovarianceMatrix();
      
      if(systematics[i_sys].EqualTo("Nom")){ //make control plots

 	this->MakeTemplateControlPlots(false,m_fitdata->GetDataHist(chans[0]),m_fitdata->GetMCHists(chans[0]),chans[0],regions[i_reg],systematics[i_sys],1);
	this->MakeTemplateControlPlots(true,m_fitdata->GetDataHist(chans[0]),m_fitdata->GetMCHists(chans[0]),chans[0],regions[i_reg],systematics[i_sys],1);
		
 	this->MakeTemplateControlPlots(false,m_fitdata->GetDataHist(chans[1]),m_fitdata->GetMCHists(chans[1]),chans[1],regions[i_reg],systematics[i_sys],1);
	this->MakeTemplateControlPlots(true,m_fitdata->GetDataHist(chans[1]),m_fitdata->GetMCHists(chans[1]),chans[1],regions[i_reg],systematics[i_sys],1);
	flav_fractions.push_back(this->MakeFlavourFractionTable(true,m_fitdata->GetDataHist(chans[1]),m_fitdata->GetMCHists(chans[1]),chans[1],regions[i_reg]));
      }


      m_fitdata->ResetHists();

    }
  }

  std::cout<<"=========================="<<std::endl;
  std::cout<<"| FIT STATUS SUMMARY: "<<std::endl;
  for(auto &el : fitstatus) std::cout<<"| "<<el<<std::endl;
  std::cout<<"=========================="<<std::endl;

  std::cout<<"=========================="<<std::endl;
  std::cout<<"| FLAVOUR FRACTIONS SUMMARY: "<<std::endl;
  std::cout<<"| BB BL CC CL LL "<<std::endl;
  for(auto &el : flav_fractions) std::cout<<"| "<<el<<std::endl;
  std::cout<<"=========================="<<std::endl;


  


  
    //Run Pseudo-Experiments
  
  std::vector<std::vector<double>> help;

  for(int i_reg=0; i_reg<regions.size(); i_reg++){
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


  //start calibration sequence
  TString pt_name="fjpt", pt_posttag_name="pt_PREFITPOSTTAG";
  
  //  std::vector<TString> variables={"fjpt","fjm"};
  std::vector<TString> variables=m_config->GetPlotVariables();
  std::vector<TString> sys={"Nom","JET_Rtrk_Baseline_Kin__1up","JET_Rtrk_Baseline_Kin__1down","JET_Rtrk_Modelling_Kin__1up", "JET_Rtrk_Modelling_Kin__1down", "JET_Rtrk_Tracking_Kin__1up", "JET_Rtrk_Tracking_Kin__1down", "JET_Rtrk_TotalStat_Kin__1up", "JET_Rtrk_TotalStat_Kin__1down", "JET_Rtrk_Baseline_Sub__1up", "JET_Rtrk_Baseline_Sub__1down","JET_Rtrk_Modelling_Sub__1up", "JET_Rtrk_Modelling_Sub__1down", "JET_Rtrk_Tracking_Sub__1up","JET_Rtrk_Tracking_Sub__1down", "JET_Rtrk_TotalStat_Sub__1up", "JET_Rtrk_TotalStat_Sub__1down", "FATJET_JMR__1up","FATJET_JER__1up"};
  std::vector<TString> sys_only={"JET_Rtrk_Baseline_Kin", "JET_Rtrk_Modelling_Kin", "JET_Rtrk_Tracking_Kin", "JET_Rtrk_TotalStat_Kin", "JET_Rtrk_Baseline_Sub","JET_Rtrk_Modelling_Sub","JET_Rtrk_Tracking_Sub","JET_Rtrk_TotalStat_Sub","FATJET_JMR","FATJET_JER"};
  std::vector<TString> nominal_only={"Nom"};
  std::vector<TString> none={};
  std::vector<TString> variables_posttag, variables_posttag_btagsys;
  
  
  for(auto& el : variables){
    if(el.Contains("ANTITAG") || el.Contains("trjpt") || el.Contains("srj") || el.Contains("evemu") || (el.Contains("fjeta") && el.Contains("fjphi")) || el.Contains("slR4jpt") || el.Contains("trjptfjptratio") || el.Contains("trjptgbbcandratio")) continue;
    variables_posttag.push_back(TString(el)+"_PREFITPOSTTAG");
  }
  for(auto& el : variables_posttag){
    variables_posttag_btagsys.push_back(TString(el)+"_BTAGUP");
    variables_posttag_btagsys.push_back(TString(el)+"_BTAGDOWN");
  }
  
  this->ReadInFatJetHists(variables,sys);
  this->ReadInFatJetHists(variables_posttag,sys);
  this->ReadInFatJetHists(variables_posttag_btagsys,nominal_only);
  
  TString ts_pt="fjpt";
  
  for(int i_var=0; i_var<variables.size(); i_var++){
    //fat jet control plots
    this->MakeFatJetControlPlots(variables[i_var],false,false,sys_only);
    this->MakeFatJetControlPlots(variables[i_var],false,true,sys_only);
  }

  for(int i_var=0; i_var<variables_posttag.size(); i_var++){
    this->MakeFatJetControlPlots(variables_posttag[i_var],true,false,sys_only);
    this->MakeFatJetControlPlots(variables_posttag[i_var],true,true,sys_only);
  }



  //fat jet pt plots in bins of fat jet
  std::vector<TString> bins={"g-2l-1","g-1l0","g0l1","g1l2"};
  TString philabel,etalabel;

  std::vector<TString> philabels,etalabels;
  std::vector<TString> fjpt_etaphibins;


  for(auto &el_phi : bins){

    for(auto &el_eta : bins){
      TString etaphiregion="fjeta_"+el_eta+"_fjphi_"+el_phi+"_fjpt";
      fjpt_etaphibins.push_back(etaphiregion);
    }    
  }

  //this->ReadInFatJetHists(fjpt_etaphibins,nominal_only);

  //for(auto &el : fjpt_etaphibins) this->MakeFatJetControlPlots(el,false,true,none);
  
    
  //fat jet binning control plots
  //for(auto& reg : regions){
    //this->MakeFatJetControlPlots(ts_pt,false,false,nominal,true,reg);
    //this->MakeFatJetControlPlots(ts_pt,false,true,none,true,reg);
    //this->MakeFatJetControlPlots(ts_pt,true,true,nominal,true,reg);
  //}
  

  //BTagging Rate Plots
  this->MakeBTaggingRatePlots(sys_only);

  TString reweight_name="test_reweight_hists.root";
  this->SaveReweightHists(pt_name, reweight_name);
  
  
  CalibResult c_res=this->CalculateScaleFactorsAndErrors(true);
  //this->MakeCalibrationPlots(c_res,"SF");
  //this->MakeCalibrationPlots(c_res,"Eff");
  this->MakeCalibrationPlots(c_res,"2DSF");
  this->MakeCalibrationPlots(c_res,"2DEffMC");
  this->MakeCalibrationPlots(c_res,"2DEffData");
  
    
}


SFCalcResult ScaleFactorCalculator::CalculateScaleFactors(TString &sys, bool doPseudo, int i_pseudo){
  //Correct MC histograms by fit factors, subtract from data, BB_data

  std::vector<float> fj_bins=m_config->GetFatJetBins();
  std::vector<TString> regions=m_config->GetAllRegions();
  
  std::vector<TH1D*> hist_pretag_mc, hist_posttag_mc;
  TString name_pretag, name_posttag;

  for(int i_p=0; i_p<m_config->GetPairs().size(); i_p++){

    name_pretag="hist_pretag_mc_"+(m_config->GetPairs())[i_p];
    name_posttag="hist_posttag_mc_"+(m_config->GetPairs())[i_p];

    hist_pretag_mc.push_back(new TH1D(name_pretag.Data(),"",fj_bins.size()-1,&(fj_bins[0])));
    hist_posttag_mc.push_back(new TH1D(name_posttag.Data(),"",fj_bins.size()-1,&(fj_bins[0])));

  }  

  TH1D *hist_pretag_data=new TH1D("hist_pretag_data","",fj_bins.size()-1,&(fj_bins[0]));
  TH1D *hist_posttag_data=new TH1D("hist_posttag_data","",fj_bins.size()-1,&(fj_bins[0])); 
  
  TH1D* help, *help_rebinned;

  double d_fj_bins[fj_bins.size()];

  for(int i_b=0; i_b<fj_bins.size(); i_b++) d_fj_bins[i_b]=(double)fj_bins[i_b];

  for(int i_reg=0; i_reg<regions.size(); i_reg++){
    
    TString mc_name=regions[i_reg]+"_fjpt_Nom";
    TString mc_name_posttag=regions[i_reg]+"_fjpt_PREFITPOSTTAG_Nom";

    for(int i_p=0; i_p<m_config->GetPairs().size(); i_p++){
      
      help=(TH1D*)m_fatjet_histograms_pretag[mc_name][i_p]->Clone();
      help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
      
      if(doPseudo)help_rebinned->Scale(m_pseudo_fit_params[regions[i_reg]][i_pseudo][i_p]);
      else help_rebinned->Scale(m_fit_params[regions[i_reg]+"_"+sys][i_p]);
      
      hist_pretag_mc[i_p]->Add(help_rebinned);

      help=(TH1D*)m_fatjet_histograms_posttag[mc_name_posttag][i_p]->Clone();
      help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
      
      if(doPseudo)help_rebinned->Scale(m_pseudo_fit_params[regions[i_reg]][i_pseudo][i_p]);
      else help_rebinned->Scale(m_fit_params[regions[i_reg]+"_"+sys][i_p]);

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
  
  float N_BB_pretag_mc, N_BB_posttag_mc, N_BB_pretag_data, N_BB_posttag_data, N_total_pretag_mc, N_total_pretag_data;

  std::vector<float> BB_SF, BB_SF_datastaterr, data_eff, mc_eff, data_eff_datastaterr;

  float err_factor_tagged, err_factor_untagged;

  for(int i_bin=1; i_bin<fj_bins.size(); i_bin++){

    N_BB_pretag_mc=hist_pretag_mc[0]->GetBinContent(i_bin);
    N_BB_posttag_mc=hist_posttag_mc[0]->GetBinContent(i_bin);

    //std::cout<<"N_BB_pretag_mc"<<N_BB_pretag_mc<<std::endl;
    //std::cout<<"N_BB_posttag_mc"<<N_BB_posttag_mc<<std::endl;
    
    N_total_pretag_mc=N_BB_pretag_mc;
    
    N_total_pretag_data=hist_pretag_data->GetBinContent(i_bin);

    N_BB_pretag_data=hist_pretag_data->GetBinContent(i_bin);
    N_BB_posttag_data=hist_posttag_data->GetBinContent(i_bin);

    //std::cout<<"N_BB_pretag_data - before subtraction"<<N_BB_pretag_data<<std::endl;
    //std::cout<<"N_BB_posttag_data - before subtraction"<<N_BB_posttag_data<<std::endl;    


    for(int i_p=1; i_p<m_config->GetPairs().size(); i_p++){

      //std::cout<<"Pretag -- subtracting component"<<i_p<<" "<<hist_pretag_mc[i_p]->GetBinContent(i_bin)<<std::endl;
      //std::cout<<"Posttag -- subtracting component"<<i_p<<" "<<hist_posttag_mc[i_p]->GetBinContent(i_bin)<<std::endl;
      
      N_total_pretag_mc+=hist_pretag_mc[i_p]->GetBinContent(i_bin);

      N_BB_pretag_data-=hist_pretag_mc[i_p]->GetBinContent(i_bin);
      N_BB_posttag_data-=hist_posttag_mc[i_p]->GetBinContent(i_bin);
      
    } 

    //TEST: set N_BB_data=frac_BB_mc*N_data


    //std::cout<<"Ntotal pretag MC"<<N_total_pretag_mc<<std::endl;
    
    N_BB_pretag_data=N_BB_pretag_mc/N_total_pretag_mc*N_total_pretag_data;

    //std::cout<<"N_BB_pretag_data"<<N_BB_pretag_data<<std::endl;
    //std::cout<<"N_BB_posttag_data"<<N_BB_posttag_data<<std::endl;    

    err_factor_tagged=(1/N_BB_pretag_data-N_BB_posttag_data/(N_BB_pretag_data*N_BB_pretag_data));
    err_factor_untagged=-N_BB_posttag_data/(N_BB_pretag_data*N_BB_pretag_data);

    BB_SF_datastaterr.push_back(TMath::Sqrt(err_factor_tagged*err_factor_tagged*N_BB_posttag_data+err_factor_untagged*err_factor_untagged*(N_BB_pretag_data-N_BB_posttag_data))/(N_BB_posttag_mc/N_BB_pretag_mc));

    BB_SF.push_back((N_BB_posttag_data/N_BB_pretag_data)/(N_BB_posttag_mc/N_BB_pretag_mc));
    
    data_eff.push_back(N_BB_posttag_data/N_BB_pretag_data);
    mc_eff.push_back(N_BB_posttag_mc/N_BB_pretag_mc);

    data_eff_datastaterr.push_back(TMath::Sqrt(err_factor_tagged*err_factor_tagged*N_BB_posttag_data+err_factor_untagged*err_factor_untagged*(N_BB_pretag_data-N_BB_posttag_data)));

    //std::cout<<"Scale factor: "<<BB_SF[i_bin-1]<<std::endl;

  }



  //delete heap objects
  for(int i_p=0; i_p<m_config->GetPairs().size(); i_p++){
   
    delete hist_pretag_mc[i_p];
    delete hist_posttag_mc[i_p];

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


SFCalcResult ScaleFactorCalculator::CalculateScaleFactorsByRegion(TString &sys, bool doPseudo, int i_pseudo){
  //Correct MC histograms by fit factors, subtract from data, BB_data

  std::vector<TString> regions=m_config->GetAllRegions();
  
  TH1D* help, *help_rebinned;

  //subtract backgrounds and calculate scale factor (or data stat error)
  
  float N_BB_pretag_mc, N_BB_posttag_mc, N_BB_pretag_data, N_BB_posttag_data, N_total_pretag_mc=0., N_total_posttag_mc=0., N_total_pretag_data;

  std::vector<float> BB_SF, BB_SF_datastaterr, data_eff, mc_eff, data_eff_datastaterr;

  float err_factor_tagged, err_factor_untagged;

  for(int i_reg=0; i_reg<regions.size(); i_reg++){
    
    TString mc_name=regions[i_reg]+"_fjpt_Nom";
    TString mc_name_posttag=regions[i_reg]+"_fjpt_PREFITPOSTTAG_Nom";

    for(int i_p=0; i_p<m_config->GetPairs().size(); i_p++){
      if(i_p==0){
	N_total_pretag_mc=0.;
	N_total_posttag_mc=0.;
      }
       
      help=(TH1D*)m_fatjet_histograms_pretag[mc_name][i_p]->Clone();      
      if(doPseudo)help->Scale(m_pseudo_fit_params[regions[i_reg]][i_pseudo][i_p]);
      else help->Scale(m_fit_params[regions[i_reg]+"_"+sys][i_p]);
      
      //consider also overflow and underflow bin in integral
      N_total_pretag_mc+=help->Integral()+help->GetBinContent(0)+help->GetBinContent(help->GetNbinsX()+1);
      if(i_p==0)N_BB_pretag_mc=help->Integral()+help->GetBinContent(0)+help->GetBinContent(help->GetNbinsX()+1);
      

      help=(TH1D*)m_fatjet_histograms_posttag[mc_name_posttag][i_p]->Clone();
      if(doPseudo)help->Scale(m_pseudo_fit_params[regions[i_reg]][i_pseudo][i_p]);
      else help->Scale(m_fit_params[regions[i_reg]+"_"+sys][i_p]);
            
      N_total_posttag_mc+=help->Integral()+help->GetBinContent(0)+help->GetBinContent(help->GetNbinsX()+1);
      if(i_p==0)N_BB_posttag_mc=help->Integral()+help->GetBinContent(0)+help->GetBinContent(help->GetNbinsX()+1);
      

    }

    TString data_name=regions[i_reg]+"_fjpt";
    TString data_name_posttag=regions[i_reg]+"_fjpt_PREFITPOSTTAG";

    
    help=(TH1D*)m_fatjet_histograms_pretag_data[data_name]->Clone();
    N_BB_pretag_data=help->Integral()+help->GetBinContent(0)+help->GetBinContent(help->GetNbinsX()+1)-(N_total_pretag_mc-N_BB_pretag_mc);
    N_total_pretag_data=help->Integral()+help->GetBinContent(0)+help->GetBinContent(help->GetNbinsX()+1);
    
    help=(TH1D*)m_fatjet_histograms_posttag_data[data_name_posttag]->Clone();
    N_BB_posttag_data=help->Integral()+help->GetBinContent(0)+help->GetBinContent(help->GetNbinsX()+1)-(N_total_posttag_mc-N_BB_posttag_mc);


    //calculate efficiencies and errors
    
    //IS THIS A TEST?
    N_BB_pretag_data=N_BB_pretag_mc/N_total_pretag_mc*N_total_pretag_data;

    //std::cout<<"N_BB_pretag_data"<<N_BB_pretag_data<<std::endl;
    //std::cout<<"N_BB_posttag_data"<<N_BB_posttag_data<<std::endl;    

    err_factor_tagged=(1/N_BB_pretag_data-N_BB_posttag_data/(N_BB_pretag_data*N_BB_pretag_data));
    err_factor_untagged=-N_BB_posttag_data/(N_BB_pretag_data*N_BB_pretag_data);

    BB_SF_datastaterr.push_back(TMath::Sqrt(err_factor_tagged*err_factor_tagged*N_BB_posttag_data+err_factor_untagged*err_factor_untagged*(N_BB_pretag_data-N_BB_posttag_data))/(N_BB_posttag_mc/N_BB_pretag_mc));

    BB_SF.push_back((N_BB_posttag_data/N_BB_pretag_data)/(N_BB_posttag_mc/N_BB_pretag_mc));
    
    data_eff.push_back(N_BB_posttag_data/N_BB_pretag_data);
    mc_eff.push_back(N_BB_posttag_mc/N_BB_pretag_mc);

    data_eff_datastaterr.push_back(TMath::Sqrt(err_factor_tagged*err_factor_tagged*N_BB_posttag_data+err_factor_untagged*err_factor_untagged*(N_BB_pretag_data-N_BB_posttag_data)));

  }

  SFCalcResult result;
  result.fsf=BB_SF;
  result.fsf_datastat_err=BB_SF_datastaterr;
  result.feff_data=data_eff;
  result.feff_mc=mc_eff;
  result.feff_datastat_err=data_eff_datastaterr;
  
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
  }else fj_bins=m_config->GetFatJetBins();
  
std::vector<float> current_SF,current_data_eff;  
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
  for(int i_sys=0; i_sys<systematics.size(); i_sys++){

    if(doByRegion) sf_result=this->CalculateScaleFactorsByRegion(systematics[i_sys]);
    else sf_result=this->CalculateScaleFactors(systematics[i_sys]);

    if(systematics[i_sys].EqualTo("Nom")){
      sf_nom=sf_result.fsf;
      sf_nom_datastaterr=sf_result.fsf_datastat_err;
      data_eff_nom=sf_result.feff_data;
      data_eff_nom_datastaterr=sf_result.feff_datastat_err;
      mc_eff_nom=sf_result.feff_mc;

      current_SF=sf_nom;
      current_data_eff=data_eff_nom;
    }else{
      current_SF=sf_result.fsf;
      current_data_eff=sf_result.feff_data; 
    }
    for(int i_bin=0; i_bin<fj_bins.size()-1; i_bin++){
      
      float diff=current_SF[i_bin]-sf_nom[i_bin];
      
      if(diff>0) err_sq_up[i_bin]+=diff*diff;
      else err_sq_down[i_bin]+=diff*diff;
      
      float diff_data_eff=current_data_eff[i_bin]-data_eff_nom[i_bin]; 

      if(diff_data_eff>0) data_eff_err_sq_up[i_bin]+=diff_data_eff*diff_data_eff;
      else data_eff_err_sq_down[i_bin]+=diff_data_eff*diff_data_eff;


    }    
  }


  SFCalcResult pseudo_result;



  std::vector<float> pseudo_err_sq(fj_bins.size()-1,0.);
  std::vector<float> pseudo_err_sq_eff_data(fj_bins.size()-1,0.);
  std::vector<float> pseudo_err_sq_eff_mc(fj_bins.size()-1,0.);


  
  TString nominal="Nom";

  //Calculate template variance with the online algorithm
  /*  std::vector<float> n(fj_bins.size()-1,0.);
  std::vector<float> M2(fj_bins.size()-1,0.);
  std::vector<float> mean(fj_bins.size()-1,0.);
  std::vector<float> delta(fj_bins.size()-1,0.);
  std::vector<float> delta2(fj_bins.size()-1,0.); */

  std::vector<double> help;

  std::vector<std::vector<double>> PseudoScaleFactors(fj_bins.size()-1,help);
  std::vector<std::vector<double>> PseudoEfficiencies_Data(fj_bins.size()-1,help);
  std::vector<std::vector<double>> PseudoEfficiencies_MC(fj_bins.size()-1,help);

  for(int i=0; i<m_pseudo_fit_params[m_config->GetAllRegions()[0]].size(); i++){

    if(!(i%250)) std::cout<<"Calculate ScaleFactors Fit Uncertainty: Fit "<<i<<std::endl;
  
    if(doByRegion) pseudo_result=this->CalculateScaleFactorsByRegion(nominal,true,i);
    else pseudo_result=this->CalculateScaleFactors(nominal,true,i);

    
    //PseudoScaleFactors.push_back(pseudo_result.fsf);
    //PseudoEfficiencies_Data.push_back((double)feff_data);
    //PseudoEfficiencies_MC.push_back((double)feff_mc);

    for(int i_bin=0; i_bin<(fj_bins.size()-1); i_bin++){
      PseudoScaleFactors[i_bin].push_back((double)pseudo_result.fsf[i_bin]);
      PseudoEfficiencies_Data[i_bin].push_back((double)pseudo_result.feff_data[i_bin]);
      PseudoEfficiencies_MC[i_bin].push_back((double)pseudo_result.feff_mc[i_bin]);



    }
    

  }

    /*current_SF=pseudo_result.fsf;
  
    for(int i_bin=0; i_bin<(fj_bins.size()-1); i_bin++){
      n[i_bin]+=1.;
      delta[i_bin]=current_SF[i_bin]-mean[i_bin];
      mean[i_bin]+=delta[i_bin]/n[i_bin];
      delta2[i_bin]=current_SF[i_bin]-mean[i_bin];
      M2[i_bin]+=delta[i_bin]*delta2[i_bin];

      if(i==(m_pseudo_fit_params[m_config->GetAllRegions()[0]].size()-1)) pseudo_err_sq[i_bin]=M2[i_bin]/(n[i_bin]-1);
      }*/
 
  

  for(int i_bin=0; i_bin<(fj_bins.size()-1); i_bin++){

    pseudo_err_sq[i_bin]=TMath::RMS(PseudoScaleFactors[i_bin].size(),&(PseudoScaleFactors[i_bin][0]));
    pseudo_err_sq[i_bin]*=pseudo_err_sq[i_bin];
    
    pseudo_err_sq_eff_data[i_bin]=TMath::RMS(PseudoEfficiencies_Data[i_bin].size(),&(PseudoEfficiencies_Data[i_bin][0]));
    pseudo_err_sq_eff_data[i_bin]*=pseudo_err_sq_eff_data[i_bin];

    pseudo_err_sq_eff_mc[i_bin]=TMath::RMS(PseudoEfficiencies_MC[i_bin].size(),&(PseudoEfficiencies_MC[i_bin][0]));
    pseudo_err_sq_eff_mc[i_bin]*=pseudo_err_sq_eff_mc[i_bin];
    

  }
  
  for(int i_bin=0; i_bin<fj_bins.size()-1; i_bin++){
    
    err_sq_up[i_bin]+=sf_nom_datastaterr[i_bin]*sf_nom_datastaterr[i_bin];
    err_sq_down[i_bin]+=sf_nom_datastaterr[i_bin]*sf_nom_datastaterr[i_bin];

    err_sq_up[i_bin]+=pseudo_err_sq[i_bin];
    err_sq_down[i_bin]+=pseudo_err_sq[i_bin];
    
    err_up.push_back(TMath::Sqrt(err_sq_up[i_bin]));
    err_down.push_back(TMath::Sqrt(err_sq_down[i_bin]));

    data_eff_err_sq_up[i_bin]+=data_eff_nom_datastaterr[i_bin]*data_eff_nom_datastaterr[i_bin];
    data_eff_err_sq_down[i_bin]+=data_eff_nom_datastaterr[i_bin]*data_eff_nom_datastaterr[i_bin];
    
    data_eff_err_sq_up[i_bin]+=pseudo_err_sq_eff_data[i_bin];
    data_eff_err_sq_down[i_bin]+=pseudo_err_sq_eff_data[i_bin];

    data_eff_err_up.push_back(TMath::Sqrt(data_eff_err_sq_up[i_bin]));
    data_eff_err_down.push_back(TMath::Sqrt(data_eff_err_sq_down[i_bin]));

    mc_eff_stat_err.push_back(TMath::Sqrt(pseudo_err_sq_eff_mc[i_bin]));

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


void ScaleFactorCalculator::MakeCalibrationPlots(CalibResult cl_result,TString plot_type){
  std::cout<<"INFO: ScaleFactorCalculator::MakeCalibrationPlots(): Making Calibration Plots of type: "<<plot_type<<std::endl;

  std::vector<float> mutrackjetbins=m_config->GetMuTrackJetBins();
  std::vector<float> nonmutrackjetbins=m_config->GetNonmuTrackJetBins();
  std::vector<float> fj_bins=m_config->GetFatJetBins();
  std::vector<float> bin_xerr, bin_x;

  for(int i_bin=0; i_bin<(fj_bins.size()-1); i_bin++){
    bin_xerr.push_back(0.5*(fj_bins[i_bin+1]-fj_bins[i_bin]));
    bin_x.push_back(fj_bins[i_bin]+bin_xerr[i_bin]);
  }

  int N=bin_xerr.size();

  std::vector<float> val_y,tot_y_err_up,tot_y_err_down,stat_y_err,val_ymc,stat_ymc_err;

  if(plot_type.EqualTo("SF") || plot_type.EqualTo("2DSF")){

    val_y=cl_result.fnominal_sf;
    tot_y_err_up=cl_result.ftot_err_up;
    tot_y_err_down=cl_result.ftot_err_down;
    stat_y_err=cl_result.fstat_err;

  }else if(plot_type.EqualTo("Eff") || plot_type.EqualTo("2DEffData") || plot_type.EqualTo("2DEffMC")){

    val_y=cl_result.fnominal_data_eff;
    tot_y_err_up=cl_result.ftot_err_data_eff_up;
    tot_y_err_down=cl_result.ftot_err_data_eff_down;
    stat_y_err=cl_result.fstat_err_data_eff;

    val_ymc=cl_result.fnominal_mc_eff;
    stat_ymc_err=cl_result.fstat_err_mc_eff;

  }
  
  TCanvas *canv=new TCanvas("canv","",800,600);
  canv->cd();
  canv->SetTickx();
  canv->SetTicky();

  
  TGraphAsymmErrors *SF_band_sys=0;
  TGraphErrors *SF_data_stat=0;
  TGraphErrors *EFF_mc_stat=0;

  TH2D *hist=0;
  TH2D *hist_err_up=0;
  TH2D *hist_err_down=0;

  if(plot_type.EqualTo("Eff") || plot_type.EqualTo("SF")){

    //make TGraph with bands
    SF_band_sys=new TGraphAsymmErrors(N,&(bin_x[0]),&(val_y[0]),&(bin_xerr[0]),&(bin_xerr[0]),&(tot_y_err_down[0]),&(tot_y_err_up[0]));
    SF_band_sys->SetMarkerSize(0);
    SF_band_sys->SetFillColor(kGreen+3);
    SF_band_sys->SetFillStyle(3001);
    SF_band_sys->SetTitle("");
    
    SF_band_sys->GetXaxis()->SetTitle(m_config->GetXLabel().Data());
    SF_band_sys->GetYaxis()->SetTitle(m_config->GetYLabel().Data());
    if(plot_type.EqualTo("Eff"))  SF_band_sys->GetYaxis()->SetTitle("Double-b-tagging Efficiency");
    
    SF_band_sys->GetXaxis()->SetTitleSize(0.04);
    SF_band_sys->GetYaxis()->SetTitleSize(0.04);
    
    SF_band_sys->GetXaxis()->SetRangeUser((m_config->GetFatJetBins())[0],m_config->GetFatJetBins().back());
    
    if(plot_type.EqualTo("SF")) SF_band_sys->GetYaxis()->SetRangeUser(0.6,1.6);
    if(plot_type.EqualTo("Eff")) SF_band_sys->GetYaxis()->SetRangeUser(0.,1.6);
    
    SF_data_stat=new TGraphErrors(N,&(bin_x[0]),&(val_y[0]),&(bin_xerr[0]),&(stat_y_err[0]));
    SF_data_stat->SetMarkerStyle(20);
    SF_data_stat->SetTitle("");
    
    if(plot_type.EqualTo("Eff")){
      EFF_mc_stat=new TGraphErrors(N,&(bin_x[0]),&(val_ymc[0]),&(bin_xerr[0]),&(stat_ymc_err[0]));
      EFF_mc_stat->SetMarkerStyle(25);
      EFF_mc_stat->SetTitle("");
    }
    

    SF_band_sys->Draw("a2");
    SF_data_stat->Draw("p");
    if(plot_type.EqualTo("Eff")) EFF_mc_stat->Draw("ep");

  }else if(plot_type.Contains("2D")){


    mutrackjetbins.insert(mutrackjetbins.begin(),0.);
    mutrackjetbins.push_back(300.);

    nonmutrackjetbins.insert(nonmutrackjetbins.begin(),0.);
    nonmutrackjetbins.push_back(100.);
    nonmutrackjetbins.push_back(140.);


    for(auto &el : mutrackjetbins) std::cout<<"Muon track jet bins: "<<el<<std::endl;
    for(auto &el : nonmutrackjetbins) std::cout<<"Non Muon track jet bins: "<<el<<std::endl;

    const char* mu_labels[3]={"p_{T}(#mu-jet) < 100GeV","100GeV < p_{T}(#mu-jet) < 200GeV", "p_{T}(#mu-jet) > 200GeV"};
    const char* nonmu_labels[4]={"p_{T}(#mu-jet) < 20GeV","20GeV < p_{T}(#mu-jet) < 50GeV","50GeV < p_{T}(#mu-jet) < 80GeV", "p_{T}(#mu-jet) > 80GeV"};


    hist=new TH2D("hist","",mutrackjetbins.size()-1,&mutrackjetbins[0],nonmutrackjetbins.size()-1,&nonmutrackjetbins[0]);
    hist_err_up=new TH2D("hist_err_up","",mutrackjetbins.size()-1,&mutrackjetbins[0],nonmutrackjetbins.size()-1,&nonmutrackjetbins[0]);
    hist_err_down=new TH2D("hist_err_down","",mutrackjetbins.size()-1,&mutrackjetbins[0],nonmutrackjetbins.size()-1,&nonmutrackjetbins[0]);

    std::cout<<"After histogram creation"<<std::endl;

    for(auto &el : val_y) std::cout<<"val_y: "<<el<<std::endl;
    for(auto &el : val_ymc) std::cout<<"val_ymc: "<<el<<std::endl;

    for(int i=1; i<=hist->GetNbinsX(); i++){
      for(int j=1; j<=hist->GetNbinsY()-1; j++){
	if(plot_type.EqualTo("2DSF") || plot_type.EqualTo("2DEffData")){
	  hist->SetBinContent(i,j,val_y[4*(i-1)+(j-1)]);
	  hist_err_up->SetBinContent(i,j,tot_y_err_up[4*(i-1)+(j-1)]);
	  hist_err_down->SetBinContent(i,j,tot_y_err_down[4*(i-1)+(j-1)]);
	}else if(plot_type.EqualTo("2DEffMC")){
	  hist->SetBinContent(i,j,val_ymc[4*(i-1)+(j-1)]);
	  hist_err_up->SetBinContent(i,j,stat_ymc_err[4*(i-1)+(j-1)]);
          hist_err_down->SetBinContent(i,j,stat_ymc_err[4*(i-1)+(j-1)]);
	}
      }
    }

    for(int i=1; i<=3; i++) hist->GetXaxis()->SetBinLabel(i,mu_labels[i-1]);
    for(int i=1; i<=4; i++) hist->GetYaxis()->SetBinLabel(i,nonmu_labels[i-1]);
    hist->LabelsOption("u","Y");

    canv->SetLeftMargin(0.2);
    canv->SetRightMargin(0.05);
    canv->cd();

    hist->SetMarkerSize(1.8);
    hist->Draw("COL TEXT");
    //hist->SetAxisRange(nonmutrackjetbins[0], nonmutrackjetbins.back()*1.4,"Y");
    hist_err_up->SetBarOffset(0.2);
    //hist_err_up->SetMarkerSize(1.8);
    hist_err_up->SetMarkerColor(kGreen+2);
    hist_err_up->Draw("TEXT SAME");
    //hist_err_down->SetMarkerSize(1.8);
    hist_err_down->SetMarkerColor(kRed);
    hist_err_down->SetBarOffset(-0.2);
    hist_err_down->Draw("TEXT SAME");

  }

  //prepare Legend                                                                                                  
  TLegend *leg=new TLegend(0.5,0.5,0.88,0.65);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  
  if(plot_type.EqualTo("SF")){
    leg->AddEntry(SF_band_sys,"bb-tagging SF (stat.+sys.)","f");
    leg->AddEntry(SF_data_stat,"bb-tagging SF (stat.)","lep");
  }
  if(plot_type.EqualTo("Eff")){
    leg->AddEntry(SF_band_sys,"bb-tagging data eff. (stat.+sys.)","f");
    leg->AddEntry(SF_data_stat,"bb-tagging data eff. (stat.)","lep");
    leg->AddEntry(EFF_mc_stat,"bb-tagging mc eff. (stat.)","lep");
  }

  leg->SetEntrySeparation(0.15);

  leg->Draw();

  
  //Add ATLAS label
  TLatex *tex0 = new TLatex();
  double lx = 0.6; double ly = 0.825;
  TString text_0="#font[72]{ATLAS} "+m_config->GetPlotLabel();
  tex0= new TLatex(lx,ly,text_0.Data());
  tex0->SetNDC();
  tex0->SetTextSize(0.05);
  tex0->SetTextColor(1);
  tex0->SetTextFont(42);

  TLatex *tex1 = new TLatex();
  lx=0.5;
  ly=0.75;
  tex1= new TLatex(lx,ly,m_config->GetSubLabel());
  tex1->SetNDC();
  tex1->SetTextSize(0.04);
  tex1->SetTextColor(1);
  tex1->SetTextFont(42);
  
  TLatex *tex2 = new TLatex();
  lx=0.6;
  ly=0.68;
  tex2= new TLatex(lx,ly,m_config->GetSubSubLabel());
  tex2->SetNDC();
  tex2->SetTextSize(0.03);
  tex2->SetTextColor(1);
  tex2->SetTextFont(42);
  

  tex0->Draw("same");
  tex1->Draw("same");
  tex2->Draw("same");

  TLine *line = new TLine((m_config->GetFatJetBins())[0],1.,(m_config->GetFatJetBins()).back(),1.);
  line->SetLineStyle(2);
  line->SetLineWidth(1.5);

  if(plot_type.EqualTo("SF")) line->Draw("same");

  TDatime today;
  
  TString name;
  if(plot_type.EqualTo("SF")) name.Form("%d_ScaleFactors.pdf",today.GetDate());
  if(plot_type.EqualTo("Eff")) name.Form("%d_Efficiencies.pdf",today.GetDate());
  if(plot_type.EqualTo("2DSF")) name.Form("%d_2DScaleFactors.pdf",today.GetDate());
  if(plot_type.EqualTo("2DEffData")) name.Form("%d_2DEfficienciesData.pdf",today.GetDate());
  if(plot_type.EqualTo("2DEffMC")) name.Form("%d_2DEfficienciesMC.pdf",today.GetDate());

  canv->SaveAs(name.Data());

  if(EFF_mc_stat) delete EFF_mc_stat; 
  if(SF_band_sys) delete SF_band_sys;
  if(SF_data_stat) delete SF_data_stat;
  if(hist) delete hist;
  if(hist_err_up) delete hist_err_up;
  if(hist_err_down) delete hist_err_down;
  delete tex0;
  delete tex1;
  delete tex2;
  delete canv;
  delete leg;


}


void ScaleFactorCalculator::ReadInFatJetHists(std::vector<TString>& var, std::vector<TString>& sys){
  
  std::vector<TString> hist_mc;
  TString hist_data;

  std::vector<TString> regions=m_config->GetAllRegions();

  TFile *infile=TFile::Open(m_config->GetInfileName(),"READ");

  std::cout<<"infile name"<<m_config->GetInfileName()<<std::endl;

  TH1D* tmp=0, *clone_tmp=0;

  
  for(int i_var=0; i_var<var.size(); i_var++){
  
    TString jettype="fatjet";
    
    for(int i_reg=0; i_reg<regions.size(); i_reg++){
      
      //Get Data Hist for region i_reg
      if( ! (var[i_var].Contains("BTAGUP") || var[i_var].Contains("BTAGDOWN") ) ){
	hist_data=m_config->GetDataHistName(regions[i_reg], var[i_var]);
	
	infile->GetObject(hist_data.Data(),tmp);
	
	//std::cout<<"Read in"<<hist_data<<"!!"<<std::endl;
	
	//if(tmp) std::cout<<"Read in "<<hist_data<<std::endl;
	if(!tmp) std::cout<<"Error in ScaleFactorCalculator::ReadInFatJetHists(): Can't find histogram "<<hist_data<<std::endl;
	
	clone_tmp=(TH1D*)tmp->Clone();
	clone_tmp->SetDirectory(0);
	
	TString name_data=regions[i_reg]+"_"+var[i_var];
      
	if(var[i_var].Contains("POSTTAG")) m_fatjet_histograms_posttag_data[name_data]=std::shared_ptr<TH1D>((TH1D*)clone_tmp);
	else m_fatjet_histograms_pretag_data[name_data]=std::shared_ptr<TH1D>((TH1D*)clone_tmp);
	
      }

      for(int i_sys=0; i_sys<sys.size(); i_sys++){
        
        
        
        //Get MC Hists for region
        hist_mc=m_config->GetMCHistNames(regions[i_reg],sys[i_sys],var[i_var]);
        
        for(int i_h=0; i_h<hist_mc.size(); i_h++){
          
          infile->GetObject(hist_mc[i_h],tmp);
          
          //if(tmp) std::cout<<"Read in "<<hist_mc[i_h]<<std::endl;
          if(!tmp) std::cout<<"Error in ScaleFactorCalculator::ReadInFatJetHists(): Can't find histogram "<<hist_mc[i_h]<<std::endl;
          
          clone_tmp=(TH1D*)tmp->Clone();
          clone_tmp->SetDirectory(0);
          
          TString name_mc=regions[i_reg]+"_"+var[i_var]+"_"+sys[i_sys];
	  //std::cout<<"name_mc read-in: "<<name_mc<<std::endl;
          
          if(var[i_var].Contains("POSTTAG")) m_fatjet_histograms_posttag[name_mc].push_back(std::shared_ptr<TH1D>((TH1D*)clone_tmp));
          else  m_fatjet_histograms_pretag[name_mc].push_back(std::shared_ptr<TH1D>((TH1D*)clone_tmp));
          
          
        }
        
      }
    }
  }
  
  infile->Close();

}


void ScaleFactorCalculator::MakeTemplateControlPlots(bool applyFitCorrection, std::shared_ptr<TH1D> dataHist, std::vector<std::shared_ptr<TH1D>> templateHists, TString& channel, TString& region, TString &sys,  int rebin){
  
  std::shared_ptr<TCanvas> canv(new TCanvas("canv","",600,800));
  canv.get()->cd();
  std::shared_ptr<TPad> pad1(new TPad("pad1","pad1",0,0.25,1,1));
  pad1.get()->SetLeftMargin(0.15);
  pad1.get()->Draw();
  canv->cd();
  std::shared_ptr<TPad> pad2 = std::shared_ptr<TPad>(new TPad("pad2","pad2",0,0,1,0.3));
  pad2->SetLeftMargin(0.15);
  pad2->SetTopMargin(0.05);
  pad2->SetBottomMargin(0.3);
  pad2.get()->Draw();

  std::vector<int> color={kBlue+1,kAzure-4,kCyan+3,kGreen-9,kOrange};

  //prepare Legend                                                                                                                                                                                                             
  TLegend *leg=new TLegend(0.55,0.5,0.88,0.825);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);

  TH1D* data_hist= (TH1D*)dataHist->Clone();

  if(rebin>1) data_hist->Rebin(rebin);

  THStack *mystack=new THStack("myStack","stack");

  TH1D* tmp_stacked_mc;
  
  TH1D *full_mc;

  for(int i_p=0; i_p<templateHists.size(); i_p++){

    if(!(templateHists[i_p].get())) continue;
    
    tmp_stacked_mc=(TH1D*)templateHists[i_p].get()->Clone();

    if(applyFitCorrection) tmp_stacked_mc->Scale(m_fit_params[region+"_"+sys][i_p]);

    if(rebin>1) tmp_stacked_mc->Rebin(rebin);

    //divide bin content by bin width
    for(int i_bin=1; i_bin<=tmp_stacked_mc->GetNbinsX(); i_bin++){
      tmp_stacked_mc->SetBinContent(i_bin, tmp_stacked_mc->GetBinContent(i_bin)/tmp_stacked_mc->GetBinWidth(i_bin));
    }

    if(i_p==0) full_mc=(TH1D*)tmp_stacked_mc->Clone();
    else full_mc->Add(tmp_stacked_mc);

    tmp_stacked_mc->SetFillColor(color[i_p]);
    tmp_stacked_mc->SetLineColor(kBlack);

    mystack->Add(tmp_stacked_mc);

    leg->AddEntry(tmp_stacked_mc,m_config->GetPairs()[i_p],"f");
  }

  //divide by bin width
  for(int i_bin=1; i_bin<=data_hist->GetNbinsX(); i_bin++){
    data_hist->SetBinContent(i_bin, data_hist->GetBinContent(i_bin)/data_hist->GetBinWidth(i_bin));
  }

  
  data_hist->SetTitle("");
  data_hist->SetLabelSize(0,"X");
  //data_hist->SetYTitle(Form("events/%.1f",(data_hist->GetBinWidth(1))));
  data_hist->SetYTitle("dN/dS_{d0}");
  data_hist->SetTitleOffset(1.5,"Y");
  data_hist->SetMarkerStyle(20);
  data_hist->SetTitleSize(0.04,"Y");
  data_hist->SetLabelSize(0.04,"Y");

  leg->AddEntry(data_hist,"Data","epl");
  
  data_hist->SetMinimum(0);

  pad1->cd();
  data_hist->Draw("EP");

  mystack->Draw("HIST SAME");
  data_hist->Draw("EP SAME");
  
  canv->cd();

  //ratio
  TH1D *h_ratio=(TH1D*)data_hist->Clone();
  h_ratio->Divide(full_mc);
  //h_ratio->SetMinimum();

  if(applyFitCorrection) std::cout<<"Normalization ratio in region "<<region<<" : "<<data_hist->Integral()/full_mc->Integral()<<std::endl;


  h_ratio->SetTitle("");
  h_ratio->SetMarkerStyle(20);
  h_ratio->SetYTitle("data/MC");
  h_ratio->SetXTitle("transverse IP significance S_{d0}"); 
  h_ratio->SetLabelSize(0.08,"X");
  h_ratio->SetLabelSize(0.08,"Y");
  h_ratio->SetTitleSize(0.08,"X");
  h_ratio->SetTitleSize(0.08,"Y");
  h_ratio->SetTitleOffset(0.9,"X");
  h_ratio->SetTitleOffset(0.6,"Y");

  pad2->SetGrid();
  pad2->cd();
  h_ratio->Draw("EP");

  //get Fit systematics band
  TGraphAsymmErrors* fitsys=0;
  
  if(applyFitCorrection){
    
    fitsys=this->getTemplateFitUncert(applyFitCorrection,templateHists,region,sys,rebin);
    fitsys->SetFillColor(kGreen+1);
    fitsys->SetFillStyle(3001);
    
    pad2->cd();
    fitsys->Draw("2");
    leg->AddEntry(fitsys,"Fit Uncertainty","f");
    h_ratio->Draw("EPsame");
    h_ratio->GetYaxis()->SetRangeUser(0.,2);
  }




  double chi2=data_hist->Chi2Test(full_mc,"WW CHI2/NDF");
  TString text_Chi2=Form("Chi2/NDF = %f",chi2);
  
  pad1->cd();
  leg->Draw();

  TLatex latex2;
  latex2.SetNDC();
  latex2.SetTextAlign(12);
  latex2.SetTextSize(0.04);
  //latex2.DrawLatex(0.5,0.8,text_Chi2.Data());

  //Add ATLAS label
  TLatex *tex0 = new TLatex();
  double lx = 0.25; double ly = 0.825;
  TString text_0="#font[72]{ATLAS} "+m_config->GetPlotLabel();
  tex0= new TLatex(lx,ly,text_0.Data());
  tex0->SetNDC();
  tex0->SetTextSize(0.04);
  tex0->SetTextColor(1);
  tex0->SetTextFont(42);

  TLatex *tex1 = new TLatex();
  lx=0.25;
  ly=0.78;
  tex1= new TLatex(lx,ly,m_config->GetSubLabel());
  tex1->SetNDC();
  tex1->SetTextSize(0.03);
  tex1->SetTextColor(1);
  tex1->SetTextFont(42);
 
  pad1->cd();
  tex0->Draw("same");
  tex1->Draw("same");
  pad1->RedrawAxis();
  

  TString name= applyFitCorrection ? TString("./ctrl_plots/Template_"+channel+"_postfit_"+region+"_sd0.pdf") : TString("./ctrl_plots/Template_"+channel+"_prefit_"+region+"_sd0.pdf"); 

  canv->SaveAs(name.Data());

  data_hist->SetMinimum(10);
  data_hist->SetMaximum(data_hist->GetBinContent(data_hist->GetMaximumBin())*1e2);
  pad1->SetLogy();
  pad1->RedrawAxis();
  
  TString namelog= applyFitCorrection ? TString("./ctrl_plots/Template_"+channel+"_postfit_"+region+"_sd0_log.pdf") : TString("./ctrl_plots/Template_"+channel+"_prefit_"+region+"_sd0_log.pdf"); 

  canv->SaveAs(namelog.Data());

  delete mystack;
  delete leg;

}

TString ScaleFactorCalculator::MakeFlavourFractionTable(bool applyFitCorrection, std::shared_ptr<TH1D> dataHist,std::vector<std::shared_ptr<TH1D>> templateHists, TString& channel, TString& region){

  std::cout<<"Flavour fraction region: "<<region<<std::endl;
  
  TH1D* tmp_stacked_mc;
  
  TH1D *full_mc;

  std::vector<float> flavour_fractions;

  for(int i_p=0; i_p<templateHists.size(); i_p++){

    if(!(templateHists[i_p].get())) continue;
    
    tmp_stacked_mc=(TH1D*)templateHists[i_p].get()->Clone();

    if(applyFitCorrection) tmp_stacked_mc->Scale(m_fit_params[region+"_Nom"][i_p]);

    if(i_p==0) full_mc=(TH1D*)tmp_stacked_mc->Clone();
    else full_mc->Add(tmp_stacked_mc);

    flavour_fractions.push_back(tmp_stacked_mc->Integral());

  }

  std::vector<float> matrix;

  for(int i_entry=0; i_entry<m_nom_cov_mats[region].size(); i_entry++){
    matrix.push_back((float)(m_nom_cov_mats[region][i_entry])); //get region covariance matrix
  }

  TMatrixF mat(flavour_fractions.size(),flavour_fractions.size(),&matrix[0]);
  std::vector<double> flavour_fraction_err(flavour_fractions.size(),0.);

  TVectorF v(flavour_fractions.size(),&flavour_fractions[0]);
  TVectorF v2=v;

  float N_total=full_mc->Integral();

  //calculate error for each flavour fraction according to propagation formula
  for(int i=0; i<flavour_fractions.size(); i++){
    std::vector<int> keepRowCol;
    float err_flavour=TMath::Sqrt(mat[i][i])*flavour_fractions[i];
    std::cout<<"err_flavour"<<err_flavour<<std::endl;
    float N_flavour=flavour_fractions[i];
    std::vector<float> reduced_flavour_fractions;

    for(int j=0; j<flavour_fractions.size(); j++){
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
    for(int k=0; k<keepRowCol.size(); k++){
      TMatrixFColumn(reduced_mat,k) = TMatrixFColumn(reduced_mat,keepRowCol[k]);
    }
    reduced_mat.ResizeTo(reduced_mat.GetNrows(),keepRowCol.size());
    std::cout<<"Nrows: "<<reduced_mat.GetNrows()<<" Ncolumns: "<<reduced_mat.GetNcols()<<std::endl;

    //delete column
    for(int l=0; l<keepRowCol.size(); l++){
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
  TString name=region+": ";
  TString help;

  for(int i=0; i<flavour_fractions.size(); i++){
    //std::cout<<" "<<flavour_fractions[i]/full_mc->Integral()<<"+-"<<TMath::Sqrt(mat[i][i])/full_mc->Integral();
    //std::cout<<std::endl;
    //help.Form("  %.3f+-%.3f  ",flavour_fractions[i]/full_mc->Integral(),flavour_fraction_err[i]);
    help.Form("  %.3f+-%.3f  ",flavour_fractions[i]/full_mc->Integral(),TMath::Sqrt(mat[i][i])*flavour_fractions[i]/full_mc->Integral());
    name.Append(help);
  }

  return name;
}


void ScaleFactorCalculator::MakeFatJetControlPlots(TString &var,bool isPosttag, bool applyFitCorrection, std::vector<TString>& sys, bool doPrintByRegion, TString region){
  
  std::shared_ptr<TCanvas> canv(new TCanvas("canv","",700,800));
  canv.get()->cd();
  std::shared_ptr<TPad> pad1(new TPad("pad1","pad1",0,0.25,1,1));
  pad1.get()->SetLeftMargin(0.15);
  pad1.get()->Draw();
  pad1.get()->SetTicks();
  canv->cd();
  std::shared_ptr<TPad> pad2 = std::shared_ptr<TPad>(new TPad("pad2","pad2",0,0.0,1,0.3));
  pad2.get()->SetLeftMargin(0.15);
  pad2->SetTopMargin(0.05);
  pad2->SetBottomMargin(0.3);
  pad2.get()->Draw();
  pad2.get()->SetTicks();

  std::vector<int> color={kBlue+1,kAzure-4,kCyan+3,kGreen-9,kOrange};

  //if plots in eta/phi bins, take binning for fat pt
  TString var_fjpt="fjpt";
  std::vector<float> fj_bins= (var.Contains("fjphi") && var.Contains("fjeta"))  ? m_config->GetBins(var_fjpt) : m_config->GetBins(var);

  //std::cout<<"Bins for variable:"<<var<<std::endl;
  //for(auto &el : fj_bins) std::cout<<el<<std::endl;

  std::vector<TString> regions=m_config->GetAllRegions();
  
  std::vector<TH1D*> hist_pretag_mc, hist_posttag_mc;
  TString name_pretag, name_posttag;

  for(int i_p=0; i_p<m_config->GetPairs().size(); i_p++){

    name_pretag="hist_pretag_mc_"+(m_config->GetPairs())[i_p];
    name_posttag="hist_posttag_mc_"+(m_config->GetPairs())[i_p];

    hist_pretag_mc.push_back(new TH1D(name_pretag.Data(),"",fj_bins.size()-1,&(fj_bins[0])));
    hist_posttag_mc.push_back(new TH1D(name_posttag.Data(),"",fj_bins.size()-1,&(fj_bins[0])));

  }  

  TH1D *hist_pretag_data=new TH1D("hist_pretag_data","",fj_bins.size()-1,&(fj_bins[0]));
  TH1D *hist_posttag_data=new TH1D("hist_posttag_data","",fj_bins.size()-1,&(fj_bins[0])); 
  
  TH1D* help, *help_rebinned;

  double d_fj_bins[fj_bins.size()];

  for(int i_b=0; i_b<fj_bins.size(); i_b++) d_fj_bins[i_b]=(double)fj_bins[i_b];

  for(int i_reg=0; i_reg<regions.size(); i_reg++){

    if(doPrintByRegion && !(regions[i_reg].EqualTo(region))) continue;
    
    TString name_mc=regions[i_reg]+"_"+var+"_"+"Nom";
    //std::cout<<"name_mc: "<<name_mc<<std::endl;

    for(int i_p=0; i_p<m_config->GetPairs().size(); i_p++){
      
      if(!isPosttag){
	help=(TH1D*)m_fatjet_histograms_pretag[name_mc][i_p]->Clone();
	help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
	if(applyFitCorrection) help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
	//std::cout<<"Correction factor "<<i_p<<" : for hist: "<<m_fatjet_histograms_pretag[name_mc][i_p]->GetName()<<" : "<<m_fit_params[regions[i_reg]+"_Nom"][i_p]<<std::endl;
	hist_pretag_mc[i_p]->Add(help_rebinned);
      }else{
	help=(TH1D*)m_fatjet_histograms_posttag[name_mc][i_p]->Clone();
	help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
	if(applyFitCorrection){
	  help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
	  //std::cout<<"Correction factor "<<i_p<<" : for hist: "<<m_fatjet_histograms_posttag[name_mc][i_p]->GetName()<<" : "<<m_fit_params[regions[i_reg]+"_Nom"][i_p]<<std::endl;
	}
	hist_posttag_mc[i_p]->Add(help_rebinned);
      }
    }

    //TODO take out, to check scaling only!!!!
    TString name_data=regions[i_reg]+"_"+var;
    
    if(!isPosttag){
      help=(TH1D*)m_fatjet_histograms_pretag_data[name_data]->Clone();
      help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
      hist_pretag_data->Add(help_rebinned);
    }else{
      help=(TH1D*)m_fatjet_histograms_posttag_data[name_data]->Clone();
      help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
      hist_posttag_data->Add(help_rebinned);
    }
      
  }

  //prepare Legend                                                                                                                                                                                                             
  TLegend *leg=new TLegend(0.55,0.4,0.88,0.75);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);

  TH1D* data_hist= isPosttag ? hist_posttag_data : hist_pretag_data;

  THStack *mystack=new THStack("myStack","stack");

  TH1D* tmp_stacked_mc;
  
  TH1D *full_mc;

  for(int i_p=0; i_p<m_config->GetPairs().size(); i_p++){

    tmp_stacked_mc=isPosttag ?  (TH1D*)hist_posttag_mc[i_p]->Clone() : (TH1D*)hist_pretag_mc[i_p]->Clone();

    if(i_p==0) full_mc=(TH1D*)tmp_stacked_mc->Clone();
    else full_mc->Add(tmp_stacked_mc);


    tmp_stacked_mc->SetFillColor(color[i_p]);
    tmp_stacked_mc->SetLineColor(kBlack);

    mystack->Add(tmp_stacked_mc);

    leg->AddEntry(tmp_stacked_mc,m_config->GetPairs()[i_p],"f");
  }

  
  data_hist->SetTitle("");
  data_hist->SetLabelSize(0,"X");
  if(var.Contains("fjpt")||var.Contains("fjm")) data_hist->SetYTitle(Form("events/%.0f GeV",(data_hist->GetBinWidth(1))));
  else data_hist->SetYTitle(Form("events/%.1f",(data_hist->GetBinWidth(1))));
  data_hist->SetMarkerStyle(20);
  data_hist->SetTitleOffset(1.1,"Y");
  data_hist->SetTitleSize(0.05,"Y");
  data_hist->SetLabelSize(0.04, "Y");

  leg->AddEntry(data_hist,"data","epl");
  
  data_hist->SetMinimum(0);
  
  pad1->cd();
  data_hist->Draw("EP");
  data_hist->GetYaxis()->SetRangeUser(0.,data_hist->GetMaximum()*1.2);
  mystack->Draw("HIST SAME");
  data_hist->Draw("EP SAME");
  
  canv->cd();

  //ratio
  TH1D *h_ratio=(TH1D*)data_hist->Clone();
  h_ratio->Divide(full_mc);
  h_ratio->SetMinimum();

  h_ratio->SetTitle("");
  h_ratio->SetMarkerStyle(20);
  h_ratio->SetYTitle("Data/MC");
  if(var.Contains("fjpt")) h_ratio->SetXTitle("large-R jet p_{T} [GeV]");
  else if (var.Contains("fjm"))h_ratio->SetXTitle("large-R jet mass [GeV]");
  else if (var.Contains("fjD2"))h_ratio->SetXTitle("large-R jet D2");
  else if (var.Contains("nmjpt"))h_ratio->SetXTitle("non-muon jet p_{T} [GeV]");
  else if (var.Contains("mjpt"))h_ratio->SetXTitle("muon jet p_{T} [GeV]");
  else if (var.Contains("nmjeta"))h_ratio->SetXTitle("non-muon jet #eta");
  else if (var.Contains("mjeta"))h_ratio->SetXTitle("muon jet #eta");
  h_ratio->SetLabelSize(0.1,"X");
  h_ratio->SetTickLength(0.05,"X");
  h_ratio->SetLabelSize(0.1,"Y");
  h_ratio->SetLabelOffset(0.02,"X");
  h_ratio->SetTitleSize(0.12,"X");
  h_ratio->SetTitleSize(0.12,"Y");
  h_ratio->SetTitleOffset(0.4,"Y");
  h_ratio->SetTitleOffset(1.0,"X");
  h_ratio->GetYaxis()->SetNdivisions(505);


  pad2->SetGrid();
  pad2->cd();
  h_ratio->Draw("EP");
  //h_ratio->GetYaxis()->SetRangeUser(0.9,1.1);
  h_ratio->GetYaxis()->SetRangeUser(0.4,1.6);

  
  //get Fit systematics band
  TGraphAsymmErrors* fitsys=0, *btagsys=0, *expsys=0, *mcstat=0, *totsys=0;
  
  if(applyFitCorrection){
    
      fitsys=this->getFitUncert(var,isPosttag);
      fitsys->SetFillColor(kGreen+1);
      fitsys->SetFillStyle(3001);
      
      pad2->cd();
      fitsys->Draw("2");
      leg->AddEntry(fitsys,"fit uncertainty","f");
      h_ratio->Draw("EPsame");
  }

  if(isPosttag){
    
    btagsys=this->getBTagUncert(var,applyFitCorrection);
    btagsys->SetLineColor(kRed);
    btagsys->SetFillStyle(0);
    //pad2->cd();
    btagsys->Draw("5");
    h_ratio->Draw("EPsame");
    leg->AddEntry(btagsys,"b-tagging uncertainty","l");
    
  }

  if(sys.size()>0)expsys=this->getExperimentalUncert(var,sys,applyFitCorrection, isPosttag);
  mcstat=this->getMCStat(full_mc);
  mcstat->SetLineColor(kBlack);
  mcstat->SetFillStyle(0);
  mcstat->Draw("5");
 
  if(expsys){
    totsys=getTotalSys(fitsys,btagsys,expsys,mcstat);
    totsys->SetFillColor(kBlack);
    //totsys->SetFillStMyle(3005);
    totsys->SetFillStyle(3245);
    gStyle->SetHatchesLineWidth(1.5);
    totsys->SetLineColor(kBlack);
    pad2->cd();
    leg->AddEntry(totsys,"total uncertainty","f");
    totsys->Draw("5");
  }
  
  std::vector<TGraphAsymmErrors*> expsys_separate;

  //if(sys.size()>0) ecanvxpsys_separate=this->getExperimentalUncertSeparate(var,sys,applyFitCorrection, isPosttag);
  //for(auto &el : expsys_separate) el->Draw("EPsame");

  if(fitsys) fitsys->Draw("2");
  if(btagsys) btagsys->Draw("5");
  h_ratio->Draw("EPsame");

  
  //Draw errors in main pad
  TGraphAsymmErrors* tot_err=0;
  if(totsys){
    tot_err=(TGraphAsymmErrors*)totsys->Clone();
    for(int point=0; point<tot_err->GetN(); point++){
      double x,y;
      tot_err->GetPoint(point,x,y);
      
      tot_err->SetPoint(point,x,full_mc->GetBinContent(point+1));
      tot_err->SetPointEYhigh(point,tot_err->GetErrorYhigh(point)*full_mc->GetBinContent(point+1));
      tot_err->SetPointEYlow(point,tot_err->GetErrorYlow(point)*full_mc->GetBinContent(point+1));
      tot_err->SetFillColor(kBlack);
      tot_err->SetLineColor(kBlack);
      tot_err->SetFillStyle(3245);
    }
  
    pad1->cd();
    tot_err->Draw("5");
    data_hist->Draw("EP SAME");
  }

  double chi2=data_hist->Chi2Test(full_mc,"UW CHI2/NDF");
  TString text_Chi2=Form("Chi2/NDF = %f",chi2);
  
  pad1->cd();
  leg->Draw();

  TLatex latex2;
  latex2.SetNDC();
  latex2.SetTextAlign(12);
  latex2.SetTextSize(0.04);
  //latex2.DrawLatex(0.5,0.8,text_Chi2.Data());

  //Add ATLAS label
  TLatex *tex0 = new TLatex();
  double lx = 0.55; double ly = 0.825;
  TString text_0="#font[72]{ATLAS} "+m_config->GetPlotLabel();
  tex0= new TLatex(lx,ly,text_0.Data());
  tex0->SetNDC();
  tex0->SetTextSize(0.04);
  tex0->SetTextColor(1);
  tex0->SetTextFont(42);

  TLatex *tex1 = new TLatex();
  lx=0.55;
  ly=0.78;
  tex1= new TLatex(lx,ly,m_config->GetSubLabel());
  tex1->SetNDC();
  tex1->SetTextSize(0.03);
  tex1->SetTextColor(1);
  tex1->SetTextFont(42);
 
  pad1->cd();
  tex0->Draw("same");
  tex1->Draw("same");
  pad1->RedrawAxis();

  TString name;

  if(doPrintByRegion){

    if(isPosttag){
      name= applyFitCorrection ? TString("./ctrl_plots/FatJet_postfit_posttag_")+region+TString("_")+var+TString(".pdf") : TString("./ctrl_plots/FatJet_prefit_posttag_")+region+TString("_")+var+TString(".pdf");
    }else name= applyFitCorrection ? TString("./ctrl_plots/FatJet_postfit_pretag_")+region+TString("_")+var+TString(".pdf") : TString("./ctrl_plots/FatJet_prefit_pretag_")+region+TString("_")+var+TString(".pdf");
    
  }else{
    if(isPosttag){
      name= applyFitCorrection ? TString("./ctrl_plots/FatJet_postfit_posttag_")+var+TString(".pdf") : TString("./ctrl_plots/FatJet_prefit_posttag_")+var+TString(".pdf");
    }else name= applyFitCorrection ? TString("./ctrl_plots/FatJet_postfit_pretag_")+var+TString(".pdf") : TString("./ctrl_plots/FatJet_prefit_pretag")+var+TString(".pdf");
  }
  canv->SaveAs(name.Data());


  delete mystack;
  delete leg;

}


void ScaleFactorCalculator::MakeBTaggingRatePlots(std::vector<TString> &sys){

  TString var="fjpt";

  std::shared_ptr<TCanvas> canv(new TCanvas("canv","",700,800));
  canv.get()->cd();
  std::shared_ptr<TPad> pad1(new TPad("pad1","pad1",0,0.25,1,1));
  pad1.get()->SetLeftMargin(0.15);
  pad1.get()->Draw();
  pad1.get()->SetTicks();
  canv->cd();
  std::shared_ptr<TPad> pad2 = std::shared_ptr<TPad>(new TPad("pad2","pad2",0,0.0,1,0.3));
  pad2.get()->SetLeftMargin(0.15);
  pad2->SetTopMargin(0.05);
  pad2->SetBottomMargin(0.3);
  pad2.get()->Draw();
  pad2.get()->SetTicks();

  std::vector<int> color={kBlue+1,kAzure-4,kCyan+3,kGreen-9,kOrange};

  std::vector<float> fj_bins=m_config->GetBins(var);

  //std::cout<<"Bins for variable:"<<var<<std::endl;
  //for(auto &el : fj_bins) std::cout<<el<<std::endl;

  std::vector<TString> regions=m_config->GetAllRegions();
  
  std::vector<TH1D*> hist_pretag_mc, hist_posttag_mc;
  TString name_pretag, name_posttag;

  for(int i_p=0; i_p<m_config->GetPairs().size(); i_p++){

    name_pretag="hist_pretag_mc_"+(m_config->GetPairs())[i_p];
    name_posttag="hist_posttag_mc_"+(m_config->GetPairs())[i_p];

    hist_pretag_mc.push_back(new TH1D(name_pretag.Data(),"",fj_bins.size()-1,&(fj_bins[0])));
    hist_posttag_mc.push_back(new TH1D(name_posttag.Data(),"",fj_bins.size()-1,&(fj_bins[0])));

  }  

  TH1D *hist_pretag_data=new TH1D("hist_pretag_data","",fj_bins.size()-1,&(fj_bins[0]));
  TH1D *hist_posttag_data=new TH1D("hist_posttag_data","",fj_bins.size()-1,&(fj_bins[0])); 
  
  TH1D* help, *help_rebinned;

  double d_fj_bins[fj_bins.size()];

  for(int i_b=0; i_b<fj_bins.size(); i_b++) d_fj_bins[i_b]=(double)fj_bins[i_b];

  for(int i_reg=0; i_reg<regions.size(); i_reg++){

    TString name_mc_pretag=regions[i_reg]+"_"+var+"_"+"Nom";
    TString name_mc_posttag=regions[i_reg]+"_"+var+"_PREFITPOSTTAG"+"_"+"Nom";
    //std::cout<<"name_mc: "<<name_mc<<std::endl;

    for(int i_p=0; i_p<m_config->GetPairs().size(); i_p++){
      
    	help=(TH1D*)m_fatjet_histograms_pretag[name_mc_pretag][i_p]->Clone();
	help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
	help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
	//std::cout<<"Correction factor "<<i_p<<" : for hist: "<<m_fatjet_histograms_pretag[name_mc][i_p]->GetName()<<" : "<<m_fit_params[regions[i_reg]+"_Nom"][i_p]<<std::endl;
	hist_pretag_mc[i_p]->Add(help_rebinned);
  
	help=(TH1D*)m_fatjet_histograms_posttag[name_mc_posttag][i_p]->Clone();
	help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
	help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
	//std::cout<<"Correction factor "<<i_p<<" : for hist: "<<m_fatjet_histograms_posttag[name_mc][i_p]->GetName()<<" : "<<m_fit_params[regions[i_reg]+"_Nom"][i_p]<<std::endl;
	hist_posttag_mc[i_p]->Add(help_rebinned);
      
    }

    TString name_data=regions[i_reg]+"_"+var;
    TString name_data_posttag=regions[i_reg]+"_"+var+"_PREFITPOSTTAG";
    
    help=(TH1D*)m_fatjet_histograms_pretag_data[name_data]->Clone();
    help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
    hist_pretag_data->Add(help_rebinned);
    help=(TH1D*)m_fatjet_histograms_posttag_data[name_data_posttag]->Clone();
    help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
    hist_posttag_data->Add(help_rebinned);
      
  }

//prepare Legend                                                                                                                                                                                                             
  TLegend *leg=new TLegend(0.55,0.4,0.88,0.75);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  
  //calculate data rate
  hist_posttag_data->Divide(hist_pretag_data);

  TH1D *full_mc_pretag, *full_mc_posttag;

  for(int i_p=0; i_p<m_config->GetPairs().size(); i_p++){

    if(i_p==0){
      full_mc_pretag=(TH1D*)hist_pretag_mc[i_p]->Clone();
      full_mc_posttag=(TH1D*)hist_posttag_mc[i_p]->Clone();      
    }else{
      full_mc_pretag->Add(hist_pretag_mc[i_p]);
      full_mc_posttag->Add(hist_posttag_mc[i_p]);
    }

  }
  
  //calculate MC rate
  full_mc_posttag->Divide(full_mc_pretag);

  hist_posttag_data->SetTitle("");
  hist_posttag_data->SetLabelSize(0,"X");
  hist_posttag_data->SetYTitle("double-b-tagging rate");
  hist_posttag_data->SetMarkerStyle(20);
  hist_posttag_data->SetLineColor(kBlack);
  hist_posttag_data->SetTitleOffset(1.1,"Y");
  hist_posttag_data->SetTitleSize(0.05,"Y");
  hist_posttag_data->SetLabelSize(0.04, "Y");

  leg->AddEntry(hist_posttag_data,"data","epl");
  
  hist_posttag_data->SetMinimum(0);
  
  pad1->cd();
  hist_posttag_data->Draw("EP");
  hist_posttag_data->GetYaxis()->SetRangeUser(0.,hist_posttag_data->GetMaximum()*1.2);

  hist_posttag_data->Draw("EP SAME");

  full_mc_posttag->SetMarkerStyle(20);
  full_mc_posttag->SetMarkerColor(2);
  full_mc_posttag->SetLineColor(kBlack);
  full_mc_posttag->Draw("EP SAME");

  leg->AddEntry(full_mc_posttag,"Pythia8 MC","epl");

  canv->cd();

  //ratio
  TH1D *h_ratio=(TH1D*)hist_posttag_data->Clone();
  h_ratio->Divide(full_mc_posttag);
  h_ratio->SetMinimum(0);

  h_ratio->SetTitle("");
  h_ratio->SetMarkerStyle(20);
  h_ratio->SetYTitle("Data/MC");
  if(var.Contains("fjpt")) h_ratio->SetXTitle("large-R jet p_{T} [GeV]");
  else if (var.Contains("fjm"))h_ratio->SetXTitle("large-R jet mass [GeV]");
  else if (var.Contains("fjD2"))h_ratio->SetXTitle("large-R jet D2");
  else if (var.Contains("nmjpt"))h_ratio->SetXTitle("non-muon jet p_{T} [GeV]");
  else if (var.Contains("mjpt"))h_ratio->SetXTitle("muon jet p_{T} [GeV]");
  else if (var.Contains("nmjeta"))h_ratio->SetXTitle("non-muon jet #eta");
  else if (var.Contains("mjeta"))h_ratio->SetXTitle("muon jet #eta");
  h_ratio->SetLabelSize(0.1,"X");
  h_ratio->SetTickLength(0.05,"X");
  h_ratio->SetLabelSize(0.1,"Y");
  h_ratio->SetLabelOffset(0.02,"X");
  h_ratio->SetTitleSize(0.12,"X");
  h_ratio->SetTitleSize(0.12,"Y");
  h_ratio->SetTitleOffset(0.4,"Y");
  h_ratio->SetTitleOffset(1.0,"X");
  h_ratio->GetYaxis()->SetNdivisions(505);
  h_ratio->GetYaxis()->SetRangeUser(0.,2.);

  pad2->SetGrid();
  pad2->cd();
  h_ratio->Draw("EP");

  //Systematic uncertainties
  TGraphAsymmErrors *fitsys=getFitUncertBTagRate();
  fitsys->SetFillColor(kGreen+1);
  fitsys->SetFillStyle(3001);

  pad2->cd();
  fitsys->Draw("2");
  leg->AddEntry(fitsys,"fit uncertainty","f");

  //Btagging uncertainties
  TString fjpt="fjpt",fjpt_posttag="fjpt_PREFITPOSTTAG";
  double x,y,eyl,eyh;
  TGraphAsymmErrors *btagsys=getBTagUncert(fjpt_posttag,true);
  for(int i=0; i<btagsys->GetN(); i++){
    btagsys->GetPoint(i,x,y);
    btagsys->SetPoint(i,x,1);
    btagsys->SetPointEYlow(i,btagsys->GetErrorYlow(i));
    btagsys->SetPointEYhigh(i,btagsys->GetErrorYhigh(i));
    

  }

  btagsys->SetLineColor(kRed);
  btagsys->SetFillStyle(0);
  //pad2->cd();                                                                                                                                                                        
  btagsys->Draw("5");
  h_ratio->Draw("EPsame");
  leg->AddEntry(btagsys,"b-tagging uncertainty","l");

  TGraphAsymmErrors *expsys=this->getExperimentalUncert(fjpt,sys,true,true,true);
  
  TGraphAsymmErrors *mcstat=this->getMCStat(full_mc_posttag);
  
  TGraphAsymmErrors *totsys=this->getTotalSys(fitsys, btagsys, expsys, mcstat);
  //TGraphAsymmErrors *totsys=mcstat;

  totsys->SetFillColor(kBlack);
  //totsys->SetFillStMyle(3005);                                                                                                                                                       
  totsys->SetFillStyle(3245);
  gStyle->SetHatchesLineWidth(1.5);
  totsys->SetLineColor(kBlack);
  pad2->cd();
  leg->AddEntry(totsys,"total uncertainty","f");
  totsys->Draw("5");
  fitsys->Draw("2");
  btagsys->Draw("5");
  h_ratio->Draw("EPsame");


    //Add ATLAS label
  TLatex *tex0 = new TLatex();
  double lx = 0.55; double ly = 0.825;
  TString text_0="#font[72]{ATLAS} "+m_config->GetPlotLabel();
  tex0= new TLatex(lx,ly,text_0.Data());
  tex0->SetNDC();
  tex0->SetTextSize(0.04);
  tex0->SetTextColor(1);
  tex0->SetTextFont(42);

  TLatex *tex1 = new TLatex();
  lx=0.55;
  ly=0.78;
  tex1= new TLatex(lx,ly,m_config->GetSubLabel());
  tex1->SetNDC();
  tex1->SetTextSize(0.03);
  tex1->SetTextColor(1);
  tex1->SetTextFont(42);
 
  pad1->cd();
  tex0->Draw("same");
  tex1->Draw("same");
  
  leg->Draw();

  TString name;
  name= TString("./ctrl_plots/BTaggingRate.pdf");
  
  canv->SaveAs(name.Data());

  delete leg;


}



TGraphAsymmErrors* ScaleFactorCalculator::getTemplateFitUncert(bool applyFitCorrection, std::vector<std::shared_ptr<TH1D>> templateHists, TString& region, TString &sys, int rebin){

  TH1D* tmp_stacked_mc, *hist_total;
  std::vector<float> flavour_norms;
 
  for(int i_p=0; i_p<templateHists.size(); i_p++){

    if(!(templateHists[i_p].get())) continue;
    
    tmp_stacked_mc=(TH1D*)templateHists[i_p].get()->Clone();

    if(applyFitCorrection) tmp_stacked_mc->Scale(m_fit_params[region+"_"+sys][i_p]);

    if(rebin>1) tmp_stacked_mc->Rebin(rebin);

    if(i_p==0) hist_total=(TH1D*)tmp_stacked_mc->Clone();
    else hist_total->Add(tmp_stacked_mc);

    flavour_norms.push_back(tmp_stacked_mc->Integral());
 }

  TVector v(flavour_norms.size(),&flavour_norms[0]);
      
  TVector v2=v;
  
  std::vector<float> matrix;
  for(int i_entry=0; i_entry<m_nom_cov_mats[region].size(); i_entry++){
    
    //std::cout<<"size of matrix"<<m_nom_cov_mats[regions[i_reg]].size()<<std::endl;
    matrix.push_back((float)(m_nom_cov_mats[region][i_entry])); //get region covariance matrix
    //std::cout<<"matrix element: "<<m_nom_cov_mats[regions[i_reg]][i_entry]<<std::endl;
    
  }
  //for(int j=0; j<matrix.size(); j++) std::cout<<"Matrix "<<j<<" "<<matrix[j]<<std::endl;
  
  TMatrix mat(flavour_norms.size(),flavour_norms.size(),&matrix[0]);
  
  v2*=mat;
  
  float fit_total_error_slice=v2*v;

  
  int n_bins=hist_total->GetNbinsX();
  
  double fiterrors_up[n_bins],fiterrors_down[n_bins],tot_err_sq[n_bins],x_values[n_bins],y_values[n_bins],x_error_up[n_bins],x_error_down[n_bins];
  
  for(int ib=0; ib<n_bins; ib++){
    
    tot_err_sq[ib]=fit_total_error_slice;

  }

  for(int i=1; i<=hist_total->GetNbinsX(); i++){
    x_values[i-1]=hist_total->GetBinCenter(i);
    x_error_up[i-1]=hist_total->GetBinWidth(i)/2;
    x_error_down[i-1]=hist_total->GetBinWidth(i)/2;
    y_values[i-1]=1.;
    
    //std::cout<<"total error squared"<<(tot_err_sq[i-1])<<std::endl;
    
    //std::cout<<"evts"<<(hist_total->GetBinContent(i))<<std::endl;
    
    
    if(hist_total->GetBinContent(i)) fiterrors_up[i-1]=TMath::Sqrt(tot_err_sq[i-1])/hist_total->GetBinContent(i);
    else fiterrors_up[i-1]=0;
    
    if(hist_total->GetBinContent(i)) fiterrors_down[i-1]=TMath::Sqrt(tot_err_sq[i-1])/hist_total->GetBinContent(i);
    else fiterrors_down[i-1]=0;
    
    //std::cout<<"FitError is:"<<fiterrors_up[i-1]<<std::endl;
    
  }
  
  TGraphAsymmErrors *g_fiterrors= new TGraphAsymmErrors(n_bins,x_values,y_values,x_error_down,x_error_up,fiterrors_down,fiterrors_up);
  
  return g_fiterrors;


}




TGraphAsymmErrors* ScaleFactorCalculator::getFitUncert(TString& var, bool isPosttag){
  
  //Calculate fit errors from covariance matrix for each flavour pair and each fit region, add errors in fit regions in qudrature.
  
  std::vector<TH1D*> hist_pretag_mc, hist_posttag_mc;
  TString name_pretag, name_posttag;
 
  TString var_fjpt="fjpt";
  std::vector<float> fj_bins= (var.Contains("fjphi") && var.Contains("fjeta")) ? m_config->GetBins(var_fjpt) : m_config->GetBins(var);
  std::vector<TString> regions=m_config->GetAllRegions();
  
  TH1D* help, *help_rebinned;
  
  double d_fj_bins[fj_bins.size()];
  
  for(int i_b=0; i_b<fj_bins.size(); i_b++) d_fj_bins[i_b]=(double)fj_bins[i_b];
  
  TH1D *hist_total=new TH1D("hist_total","",fj_bins.size()-1,&(fj_bins[0]));
  
  std::vector<float> fit_total_error_slice(regions.size(),0.);
  
  std::vector<double> help_norms(fj_bins.size()-1,0.);
  std::vector<std::vector<double>> region_norms(regions.size(), help_norms);
  
  for(int i_reg=0; i_reg<regions.size(); i_reg++){

    help_norms.assign(fj_bins.size()-1,0.);
    
    TString name_mc=regions[i_reg]+"_"+var+"_"+"Nom";
    //std::cout<<"name_mc: "<<name_mc<<std::endl;
    
    std::vector<float> flavour_norms(m_config->GetPairs().size(),0.);
    
    for(int i_p=0; i_p<m_config->GetPairs().size(); i_p++){
      
      if(!isPosttag){
        help=(TH1D*)m_fatjet_histograms_pretag[name_mc][i_p]->Clone();
        help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
        help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
        hist_total->Add(help_rebinned);
        flavour_norms[i_p]=help_rebinned->Integral();
        //std::cout<<"flavour_norm: "<<flavour_norms[i_p]<<std::endl;
        
        for(int i_bin=1; i_bin<=help_rebinned->GetNbinsX(); i_bin++){
          help_norms[i_bin-1]+=help_rebinned->GetBinContent(i_bin);
        }
        
      }else{
        help=(TH1D*)m_fatjet_histograms_posttag[name_mc][i_p]->Clone();
        help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
        help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
        //std::cout<<"Correction factor "<<i_p<<" : "<< m_fit_params[regions[i_reg]+"_"+sys][i_p]<<std::endl;
        hist_total->Add(help_rebinned);
        flavour_norms[i_p]=help_rebinned->Integral();
        //std::cout<<"flavour_norm: "<<flavour_norms[i_p]<<std::endl;
        for(int i_bin=1; i_bin<=help_rebinned->GetNbinsX(); i_bin++){
          help_norms[i_bin-1]+=help_rebinned->GetBinContent(i_bin);
        }
      }
      
      
      
      TVector v(flavour_norms.size(),&flavour_norms[0]);
      
      TVector v2=v;
      
      std::vector<float> matrix;
      for(int i_entry=0; i_entry<m_nom_cov_mats[regions[i_reg]].size(); i_entry++){
      
        //std::cout<<"size of matrix"<<m_nom_cov_mats[regions[i_reg]].size()<<std::endl;
        matrix.push_back((float)(m_nom_cov_mats[regions[i_reg]][i_entry])); //get region covariance matrix
        //std::cout<<"matrix element: "<<m_nom_cov_mats[regions[i_reg]][i_entry]<<std::endl;

      }
      //for(int j=0; j<matrix.size(); j++) std::cout<<"Matrix "<<j<<" "<<matrix[j]<<std::endl;
      
      TMatrix mat(flavour_norms.size(),flavour_norms.size(),&matrix[0]);
      
      v2*=mat;
      
      fit_total_error_slice[i_reg]=v2*v;
      //std::cout<<"fit total error slice: "<<fit_total_error_slice[i_reg]<<std::endl;
      
      
    }

    
    region_norms[i_reg]=help_norms;
    
    
    
  }
  
  int n_bins=fj_bins.size()-1;
  
  double fiterrors_up[n_bins],fiterrors_down[n_bins],tot_err_sq[n_bins],x_values[n_bins],y_values[n_bins],x_error_up[n_bins],x_error_down[n_bins];
  
  for(int ib=0; ib<n_bins; ib++){
    
    tot_err_sq[ib]=0;

  }

  
  //loop over bins, check fraction contributed by fit region in each bin, calculate error
  for(int i_bin=1; i_bin<=n_bins; i_bin++){

    for(int i_reg=0; i_reg<regions.size(); i_reg++){

      if(hist_total->GetBinContent(i_bin)) tot_err_sq[i_bin-1]+=fit_total_error_slice[i_reg]*region_norms[i_reg][i_bin-1]/hist_total->GetBinContent(i_bin);
      
      //std::cout<<"fit_total_error_slice"<<fit_total_error_slice[i_reg]<<std::endl;
      //std::cout<<"region_norms"<<region_norms[i_reg][i_bin-1]<<std::endl;
      
    }
    
  }
  
  
  for(int i=1; i<=hist_total->GetNbinsX(); i++){
    x_values[i-1]=hist_total->GetBinCenter(i);
    x_error_up[i-1]=hist_total->GetBinWidth(i)/2;
    x_error_down[i-1]=hist_total->GetBinWidth(i)/2;
    y_values[i-1]=1.;
    
    //std::cout<<"total error squared"<<(tot_err_sq[i-1])<<std::endl;
    
    //std::cout<<"evts"<<(hist_total->GetBinContent(i))<<std::endl;
    
    
    if(hist_total->GetBinContent(i)) fiterrors_up[i-1]=TMath::Sqrt(tot_err_sq[i-1])/hist_total->GetBinContent(i);
    else fiterrors_up[i-1]=0;
    
    if(hist_total->GetBinContent(i)) fiterrors_down[i-1]=TMath::Sqrt(tot_err_sq[i-1])/hist_total->GetBinContent(i);
    else fiterrors_down[i-1]=0;
    
    //std::cout<<"FitError is:"<<fiterrors_up[i-1]<<std::endl;
    
  }
  
  
  
  TGraphAsymmErrors *g_fiterrors= new TGraphAsymmErrors(n_bins,x_values,y_values,x_error_down,x_error_up,fiterrors_down,fiterrors_up);
  return g_fiterrors;

}


TGraphAsymmErrors* ScaleFactorCalculator::getFitUncertBTagRate(){
  
  TString var="fjpt";

  //Calculate fit errors from covariance matrix for each flavour pair and each fit region, add errors in fit regions in qudrature.
  
  std::vector<TH1D*> hist_pretag_mc, hist_posttag_mc;
  TString name_pretag, name_posttag;
 
  std::vector<float> fj_bins=m_config->GetBins(var);
  std::vector<TString> regions=m_config->GetAllRegions();
  
  TH1D* help, *help_rebinned;
  
  double d_fj_bins[fj_bins.size()];
  
  for(int i_b=0; i_b<fj_bins.size(); i_b++) d_fj_bins[i_b]=(double)fj_bins[i_b];
  
  TH1D *hist_total=new TH1D("hist_total","",fj_bins.size()-1,&(fj_bins[0]));
  TH1D *hist_total_posttag=new TH1D("hist_total_posttag","",fj_bins.size()-1,&(fj_bins[0]));

  
  std::vector<float> norm_region_total(regions.size(),0.);
  std::vector<float> norm_region_total_posttag(regions.size(),0.);

  std::vector<float> fit_total_error_slice(regions.size(),0.);
  std::vector<float> fit_total_error_slice_untagged(regions.size(),0.);
  std::vector<float> fit_total_error_slice_combined(regions.size(),0.);

  
  std::vector<double> help_norms(fj_bins.size()-1,0.);
  std::vector<std::vector<double>> region_norms(regions.size(), help_norms);

  std::vector<double> help_norms_posttag(fj_bins.size()-1,0.);
  std::vector<std::vector<double>> region_norms_posttag(regions.size(), help_norms_posttag);

  
  for(int i_reg=0; i_reg<regions.size(); i_reg++){

    help_norms.assign(fj_bins.size()-1,0.);
    help_norms_posttag.assign(fj_bins.size()-1,0.);
    
    TString name_mc=regions[i_reg]+"_"+var+"_"+"Nom";
    TString name_mc_posttag=regions[i_reg]+"_"+var+"_PREFITPOSTTAG_"+"Nom";

    //std::cout<<"name_mc: "<<name_mc<<std::endl;
    
    std::vector<float> flavour_norms(m_config->GetPairs().size(),0.);
    std::vector<float> flavour_norms_posttag(m_config->GetPairs().size(),0.);
    
    for(int i_p=0; i_p<m_config->GetPairs().size(); i_p++){
      
      help=(TH1D*)m_fatjet_histograms_pretag[name_mc][i_p]->Clone();
      help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
      help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
      hist_total->Add(help_rebinned);
      flavour_norms[i_p]=help_rebinned->Integral();
      norm_region_total[i_reg]+=help_rebinned->Integral();
      //std::cout<<"flavour_norm: "<<flavour_norms[i_p]<<std::endl;
        
      for(int i_bin=1; i_bin<=help_rebinned->GetNbinsX(); i_bin++){
	help_norms[i_bin-1]+=help_rebinned->GetBinContent(i_bin);
      }
        
      help=(TH1D*)m_fatjet_histograms_posttag[name_mc_posttag][i_p]->Clone();
      help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
      help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
      //std::cout<<"Correction factor "<<i_p<<" : "<< m_fit_params[regions[i_reg]+"_"+sys][i_p]<<std::endl;
      hist_total_posttag->Add(help_rebinned);
      flavour_norms_posttag[i_p]=help_rebinned->Integral();
      norm_region_total_posttag[i_reg]+=help_rebinned->Integral();
      //std::cout<<"flavour_norm: "<<flavour_norms[i_p]<<std::endl;
      for(int i_bin=1; i_bin<=help_rebinned->GetNbinsX(); i_bin++){
	help_norms_posttag[i_bin-1]+=help_rebinned->GetBinContent(i_bin);
      }
    
    }  
      
      
    TVector v(flavour_norms.size(),&flavour_norms[0]);
    TVector v_posttag(flavour_norms.size(),&flavour_norms[0]);
    TVector v_untagged=v-v_posttag;

    TVector v2=v;
    TVector v2_untagged=v_untagged;
    
    std::vector<float> matrix;
    for(int i_entry=0; i_entry<m_nom_cov_mats[regions[i_reg]].size(); i_entry++){
      
      //std::cout<<"size of matrix"<<m_nom_cov_mats[regions[i_reg]].size()<<std::endl;
      matrix.push_back((float)(m_nom_cov_mats[regions[i_reg]][i_entry])); //get region covariance matrix
      //std::cout<<"matrix element: "<<m_nom_cov_mats[regions[i_reg]][i_entry]<<std::endl;
      
    }
    //for(int j=0; j<matrix.size(); j++) std::cout<<"Matrix "<<j<<" "<<matrix[j]<<std::endl;
    
    TMatrix mat(flavour_norms.size(),flavour_norms.size(),&matrix[0]);
    
    v2*=mat;
    v2_untagged*=mat;
    
    fit_total_error_slice[i_reg]=v2*v;
    fit_total_error_slice_untagged[i_reg]=v2_untagged*v_untagged;
    //fit_total_error_slice_combined[i_reg]=(1./norm_region_total[i_reg]-norm_region_posttag[i_reg]/(norm_region_total[i_reg]*norm_region_total[i_reg]))*(1./norm_region_total[i_reg]-norm_region_posttag[i_reg]/(norm_region_total[i_reg]*norm_region_total[i_reg]))* fit_total_error_slice_untagged[i_reg]*fit_total_error_slice_untagged[i_reg]+(-norm_region_posttag[i_reg]/(norm_region_total[i_reg]*norm_region_total[i_reg]))*(-norm_region_posttag[i_reg]/(norm_region_total[i_reg]*norm_region_total[i_reg]))* fit_total_error_slice_untagged[i_reg]*fit_total_error_slice_untagged[i_reg];
    //std::cout<<"fit total error slice: "<<fit_total_error_slice[i_reg]<<std::endl;
      
    region_norms[i_reg]=help_norms;
    region_norms_posttag[i_reg]=help_norms_posttag;
  
    
    
  }
  
  int n_bins=fj_bins.size()-1;
  
  double fiterrors_up[n_bins],fiterrors_down[n_bins],tot_err_sq[n_bins],x_values[n_bins],y_values[n_bins],x_error_up[n_bins],x_error_down[n_bins],tot_err_sq_untagged[n_bins],tot_err_sq_combined[n_bins];
  
  for(int ib=0; ib<n_bins; ib++){
    
    tot_err_sq[ib]=0;
    tot_err_sq_untagged[ib]=0;
    tot_err_sq_combined[ib]=0;
    
  }

  
  //loop over bins, check fraction contributed by fit region in each bin, calculate error
  for(int i_bin=1; i_bin<=n_bins; i_bin++){

    for(int i_reg=0; i_reg<regions.size(); i_reg++){

      if(hist_total->GetBinContent(i_bin)) tot_err_sq[i_bin-1]+=fit_total_error_slice[i_reg]*region_norms[i_reg][i_bin-1]/hist_total->GetBinContent(i_bin);
      if((hist_total->GetBinContent(i_bin)-hist_total_posttag->GetBinContent(i_bin))>0.) tot_err_sq_untagged[i_bin-1]+=fit_total_error_slice_untagged[i_reg]*(region_norms[i_reg][i_bin-1]-region_norms[i_reg][i_bin-1])/(hist_total->GetBinContent(i_bin)-hist_total_posttag->GetBinContent(i_bin));
      
      float N_tagged=hist_total_posttag->GetBinContent(i_bin);
      float N_total=hist_total->GetBinContent(i_bin);

      tot_err_sq_combined[i_bin-1]+=(1./N_total-N_tagged/(N_total*N_total))*(1./N_total-N_tagged/(N_total*N_total))*tot_err_sq[i_bin-1]+(-N_tagged/(N_total*N_total))*(-N_tagged/(N_total*N_total))*tot_err_sq_untagged[i_bin-1];


      //std::cout<<"fit_total_error_slice"<<fit_total_error_slice[i_reg]<<std::endl;
      //std::cout<<"region_norms"<<region_norms[i_reg][i_bin-1]<<std::endl;
      
    }
    
  }
  
  
  for(int i=1; i<=hist_total->GetNbinsX(); i++){
    x_values[i-1]=hist_total->GetBinCenter(i);
    x_error_up[i-1]=hist_total->GetBinWidth(i)/2;
    x_error_down[i-1]=hist_total->GetBinWidth(i)/2;
    y_values[i-1]=1.;
    
    std::cout<<"total error squared"<<(tot_err_sq_combined[i-1])<<std::endl;
    
    std::cout<<"evts"<<(hist_total->GetBinContent(i))<<std::endl;
    
    
    if(hist_total->GetBinContent(i)) fiterrors_up[i-1]=TMath::Sqrt(tot_err_sq_combined[i-1])/(hist_total_posttag->GetBinContent(i)/hist_total->GetBinContent(i));
    else fiterrors_up[i-1]=0;
    
    if(hist_total->GetBinContent(i)) fiterrors_down[i-1]=TMath::Sqrt(tot_err_sq_combined[i-1])/(hist_total_posttag->GetBinContent(i)/hist_total->GetBinContent(i));
    else fiterrors_down[i-1]=0;
    
    std::cout<<"FitError is:"<<fiterrors_up[i-1]<<std::endl;
    
  }
  
  
  
  TGraphAsymmErrors *g_fiterrors= new TGraphAsymmErrors(n_bins,x_values,y_values,x_error_down,x_error_up,fiterrors_down,fiterrors_up);
  return g_fiterrors;

}



TGraphAsymmErrors* ScaleFactorCalculator::getBTagUncert(TString& var, bool applyFitCorrection){
  
  //Get B-tagging systematics (up or down already defined as going in positive/negative direction)
  std::vector<float> fj_bins=m_config->GetBins(var);
  std::vector<TString> regions=m_config->GetAllRegions();
  

  
  TH1D *h_nom=new TH1D("h_nom","",fj_bins.size()-1,&(fj_bins[0]));
  TH1D *h_up=new TH1D("h_up","",fj_bins.size()-1,&(fj_bins[0]));
  TH1D *h_down=new TH1D("h_down","",fj_bins.size()-1,&(fj_bins[0]));
  
  
  TH1D* help, *help_rebinned;
  
  double d_fj_bins[fj_bins.size()];
  
  for(int i_b=0; i_b<fj_bins.size(); i_b++) d_fj_bins[i_b]=(double)fj_bins[i_b];
  
  for(int i_reg=0; i_reg<regions.size(); i_reg++){
    
    //if(doPrintByRegion && !(regions[i_reg].EqualTo(region))) continue;
    
    TString name_mc=regions[i_reg]+"_"+var+"_"+"Nom";
    TString name_mc_up=regions[i_reg]+"_"+var+"_"+"BTAGUP_Nom";
    TString name_mc_down=regions[i_reg]+"_"+var+"_"+"BTAGDOWN_Nom";
    //std::cout<<"name_mc: "<<name_mc<<std::endl;
    //std::cout<<"name_mc btag up: "<<name_mc_up<<std::endl;
    //std::cout<<"name_mc btag down: "<<name_mc_down<<std::endl;

    
    for(int i_p=0; i_p<m_config->GetPairs().size(); i_p++){
        help=(TH1D*)m_fatjet_histograms_posttag[name_mc][i_p]->Clone();
        help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
        if(applyFitCorrection) help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
        //std::cout<<"Correction factor "<<i_p<<" : "<< m_fit_params[regions[i_reg]+"_"+sys][i_p]<<std::endl;
        h_nom->Add(help_rebinned);
      
        help=(TH1D*)m_fatjet_histograms_posttag[name_mc_up][i_p]->Clone();
        help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
        if(applyFitCorrection) help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
        //std::cout<<"Correction factor "<<i_p<<" : "<< m_fit_params[regions[i_reg]+"_"+sys][i_p]<<std::endl;
        h_up->Add(help_rebinned);
      
        help=(TH1D*)m_fatjet_histograms_posttag[name_mc_down][i_p]->Clone();
        help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
        if(applyFitCorrection) help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
        //std::cout<<"Correction factor "<<i_p<<" : "<< m_fit_params[regions[i_reg]+"_"+sys][i_p]<<std::endl;
        h_down->Add(help_rebinned);
      
    }
  }
  
  
  //normalized option
  
  /*if(m_doNormalized){
    
    h_up->Scale(1./h_up->Integral());
    h_down->Scale(1./h_down->Integral());
    h_nom->Scale(1./h_nom->Integral());
    
  }*/
  
  //rebin up and down histogram if necessary
  /*if(m_Rebin){
    h_nom->Rebin(m_rebin_factor);
    h_up->Rebin(m_rebin_factor);
    h_down->Rebin(m_rebin_factor);
  }*/
  
  int n_bins=h_nom->GetNbinsX();
  
  double btagerrors_up[n_bins],btagerrors_down[n_bins],x_values[n_bins],y_values[n_bins],x_error_up[n_bins],x_error_down[n_bins];
  
  TH1* h_diff_up=(TH1*)h_nom->Clone();
  TH1* h_diff_down=(TH1*)h_nom->Clone();
  
  h_diff_up->Add(h_up,h_nom,1,-1);
  h_diff_down->Add(h_nom,h_down,1,-1);
  
  
  for(int i=1; i<=h_nom->GetNbinsX(); i++){
    x_values[i-1]=h_nom->GetBinCenter(i);
    x_error_up[i-1]=h_nom->GetBinWidth(i)/2;
    x_error_down[i-1]=h_nom->GetBinWidth(i)/2;
    y_values[i-1]=1.;
    
    if(h_nom->GetBinContent(i)) btagerrors_up[i-1]=h_diff_up->GetBinContent(i)/h_nom->GetBinContent(i);
    else btagerrors_up[i-1]=0;
    
    if(h_nom->GetBinContent(i)) btagerrors_down[i-1]=h_diff_down->GetBinContent(i)/h_nom->GetBinContent(i);
    else btagerrors_down[i-1]=0;
    
  }
  
  
  TGraphAsymmErrors *g_btagerrors= new TGraphAsymmErrors(n_bins,x_values,y_values,x_error_down,x_error_up,btagerrors_down,btagerrors_up);
  
  return g_btagerrors;
  
  
}

TGraphAsymmErrors* ScaleFactorCalculator::getExperimentalUncert(TString &var, std::vector<TString> &sys, bool applyFitCorrection, bool isPosttag, bool isEff){
 
  std::vector<float> fj_bins=m_config->GetBins(var);
  std::vector<TString> regions=m_config->GetAllRegions();
  
  TH1D *h_nom=new TH1D("h_nom","",fj_bins.size()-1,&(fj_bins[0]));
  
  //for efficiency plot
  TH1D *h_nom_eff_pretag=new TH1D("h_nom_eff_pretag","",fj_bins.size()-1,&(fj_bins[0]));
  
  TH1D* help, *help_rebinned;
  
  double d_fj_bins[fj_bins.size()];
  
  for(int i_b=0; i_b<fj_bins.size(); i_b++) d_fj_bins[i_b]=(double)fj_bins[i_b];
  
  
  int n_bins=fj_bins.size()-1;
  
  double total_errors_up[n_bins],total_errors_down[n_bins], total_errsquare_up[n_bins],total_errsquare_down[n_bins],x_values[n_bins],y_values[n_bins],x_error_up[n_bins],x_error_down[n_bins];
  
  for(int i=1; i<=n_bins; i++){
    total_errsquare_down[i-1]=0;
    total_errsquare_up[i-1]=0;
    
  }
  
  for(int i_sys=0; i_sys<sys.size(); i_sys++){
    bool isOneSided=false;

    TH1D *h_up=new TH1D("h_up","",fj_bins.size()-1,&(fj_bins[0]));
    TH1D *h_down=new TH1D("h_down","",fj_bins.size()-1,&(fj_bins[0]));

    //for efficiency plot
    TH1D *h_up_eff_pretag=new TH1D("h_up_eff_pretag","",fj_bins.size()-1,&(fj_bins[0]));
    TH1D *h_down_eff_pretag=new TH1D("h_down_eff_pretag","",fj_bins.size()-1,&(fj_bins[0]));

    for(int i_reg=0; i_reg<regions.size(); i_reg++){
      
      //if(doPrintByRegion && !(regions[i_reg].EqualTo(region))) continue;
      
      TString name_mc=regions[i_reg]+"_"+var+"_"+"Nom";
      TString name_mc_up=regions[i_reg]+"_"+var+"_"+sys[i_sys]+"__1up";
      TString name_mc_down=regions[i_reg]+"_"+var+"_"+sys[i_sys]+"__1down";

      TString name_mc_posttag=regions[i_reg]+"_"+var+"_PREFITPOSTTAG_"+"Nom";
      TString name_mc_up_posttag=regions[i_reg]+"_"+var+"_PREFITPOSTTAG_"+sys[i_sys]+"__1up";
      TString name_mc_down_posttag=regions[i_reg]+"_"+var+"_PREFITPOSTTAG_"+sys[i_sys]+"__1down";

      //std::cout<<"name_mc: "<<name_mc<<std::endl;
      //std::cout<<"name_mc sys up: "<<name_mc_up<<std::endl;
      //std::cout<<"name_mc sys down: "<<name_mc_down<<std::endl;
      
      
      for(int i_p=0; i_p<m_config->GetPairs().size(); i_p++){

	if(isPosttag || isEff){
	  if(!isEff){
	    name_mc_posttag=name_mc;
	    name_mc_up_posttag=name_mc_up;
	    name_mc_down_posttag=name_mc_down;
	  } 

	  if(i_sys==0){
	    //only need to get nominal once
	    help=(TH1D*)m_fatjet_histograms_posttag[name_mc_posttag][i_p]->Clone();
	    help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
	    if(applyFitCorrection) help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
	    //std::cout<<"Correction factor "<<i_p<<" : "<< m_fit_params[regions[i_reg]+"_"+sys][i_p]<<std::endl;
	    h_nom->Add(help_rebinned);
	    
	  }
	  
	  help=(TH1D*)m_fatjet_histograms_posttag[name_mc_up_posttag][i_p]->Clone();
	  help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
	  if(applyFitCorrection) help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
	  //std::cout<<"Correction factor "<<i_p<<" : "<< m_fit_params[regions[i_reg]+"_"+sys][i_p]<<std::endl;
	  h_up->Add(help_rebinned);
	  
	  if(m_fatjet_histograms_posttag.find(name_mc_down_posttag)==m_fatjet_histograms_posttag.end()){
	    //systematic is one-sided
	    isOneSided=true;
	    if(!isEff) continue;
	  }else{
	    help=(TH1D*)m_fatjet_histograms_posttag[name_mc_down_posttag][i_p]->Clone();
	    help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
	    if(applyFitCorrection) help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
	    //std::cout<<"Correction factor "<<i_p<<" : "<< m_fit_params[regions[i_reg]+"_"+sys][i_p]<<std::endl;
	    h_down->Add(help_rebinned);
	  }
        }
	if(!isPosttag || isEff){
	  if(i_sys==0){
	    //only need to get nominal once
	    help=(TH1D*)m_fatjet_histograms_pretag[name_mc][i_p]->Clone();
	    help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
	    if(applyFitCorrection) help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
	    //std::cout<<"Correction factor "<<i_p<<" : "<< m_fit_params[regions[i_reg]+"_"+sys][i_p]<<std::endl;
	    if(!isEff) h_nom->Add(help_rebinned);
	    else h_nom_eff_pretag->Add(help_rebinned);
	  }
	  
	  help=(TH1D*)m_fatjet_histograms_pretag[name_mc_up][i_p]->Clone();
	  help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
	  if(applyFitCorrection) help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
	  //std::cout<<"Correction factor "<<i_p<<" : "<< m_fit_params[regions[i_reg]+"_"+sys][i_p]<<std::endl;
	  if(!isEff) h_up->Add(help_rebinned);
	  else h_up_eff_pretag->Add(help_rebinned);
	  if(m_fatjet_histograms_pretag.find(name_mc_down)==m_fatjet_histograms_pretag.end()){
	    //systematic is one-sided
	    isOneSided=true;
	    if(!isEff) continue;
	  }else{	 
	    help=(TH1D*)m_fatjet_histograms_pretag[name_mc_down][i_p]->Clone();
	    help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
	    if(applyFitCorrection) help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
	    //std::cout<<"Correction factor "<<i_p<<" : "<< m_fit_params[regions[i_reg]+"_"+sys][i_p]<<std::endl;
	    if(!isEff) h_down->Add(help_rebinned);
	    else h_down_eff_pretag->Add(help_rebinned);
          }
	}


      }
    }

    if(isEff){
      if(i_sys==0) h_nom->Divide(h_nom_eff_pretag);
      h_up->Divide(h_up_eff_pretag);
      if(!isOneSided) h_down->Divide(h_down_eff_pretag);
      
    }
    
    if(isOneSided){
      
      for(int i=1; i<=n_bins; i++){
        
        float delta_up=h_up->GetBinContent(i)-h_nom->GetBinContent(i);
	if(isEff) std::cout<<"delta up one-sided: "<< sys[i_sys]<<" : "<<delta_up<<std::endl;

        if(delta_up>0) total_errsquare_up[i-1]+=delta_up*delta_up;
        else total_errsquare_down[i-1]+=delta_up*delta_up;
        
      }
      
    }else{
      
      for(int i=1; i<=n_bins; i++){
	if(isEff) std::cout<<"Nominal bin "<<i<<": "<<h_nom->GetBinContent(i)<<std::endl;

        float delta_up=h_up->GetBinContent(i)-h_nom->GetBinContent(i);
        float delta_down=h_down->GetBinContent(i)-h_nom->GetBinContent(i);
	if(isEff){
	  std::cout<<"delta up: "<< sys[i_sys]<<" : "<<delta_up<<std::endl;
	  std::cout<<"delta down: "<< sys[i_sys]<<" : "<<delta_down<<std::endl;
	}
        
        if(delta_up*delta_down<0){
          
          if(delta_up>0){
            total_errsquare_up[i-1]+=delta_up*delta_up;
            total_errsquare_down[i-1]+=delta_down*delta_down;
          }else{
            total_errsquare_down[i-1]+=delta_up*delta_up;
            total_errsquare_up[i-1]+=delta_down*delta_down;
          }
          
        }else{
	  if(isEff) std::cout<<"same-sign variation" << std::endl;
	  
          float bigger_delta= std::max(TMath::Abs(delta_up),TMath::Abs(delta_down));
          if(bigger_delta>0) total_errsquare_up[i-1]+=bigger_delta*bigger_delta;
          else total_errsquare_down[i-1]+=bigger_delta*bigger_delta;
          
        }
        
      }
      
      
    }
  }
  
  for(int i=1; i<=n_bins; i++){
    
    x_values[i-1]=h_nom->GetBinCenter(i);
    x_error_up[i-1]=h_nom->GetBinWidth(i)/2;
    x_error_down[i-1]=h_nom->GetBinWidth(i)/2;
    y_values[i-1]=1.;
    
    if(h_nom->GetBinContent(i)) total_errors_up[i-1]=TMath::Sqrt(total_errsquare_up[i-1])/h_nom->GetBinContent(i);
    else total_errors_up[i-1]=0;
    
    if(h_nom->GetBinContent(i)) total_errors_down[i-1]=TMath::Sqrt(total_errsquare_down[i-1])/h_nom->GetBinContent(i);
    else total_errors_down[i-1]=0;

    if(isEff){
      std::cout<<"h_nom->GetBinContent(i): "<<h_nom->GetBinContent(i)<<std::endl;
      std::cout<<"Exp error up: "<<total_errors_up[i-1]<<std::endl;
      std::cout<<"Exp error down: "<<total_errors_down[i-1]<<std::endl;
    }

  }

  
  TGraphAsymmErrors *g_totaljeterrors= new TGraphAsymmErrors(n_bins,x_values,y_values,x_error_down,x_error_up,total_errors_down,total_errors_up);
  
  return g_totaljeterrors;


}


std::vector<TGraphAsymmErrors*> ScaleFactorCalculator::getExperimentalUncertSeparate(TString &var, std::vector<TString> &sys, bool applyFitCorrection, bool isPosttag, bool isEff){
 
  std::vector<float> fj_bins=m_config->GetBins(var);
  std::vector<TString> regions=m_config->GetAllRegions();
  
  std::vector<int> color={kBlue+1, kViolet, kRed, kGreen+2,kCyan+1, kOrange+7, kViolet+2, kRed-6, kAzure+7, kGray+2};

  std::vector<TGraphAsymmErrors*> uncertainty_graphs;

  TH1D *h_nom=new TH1D("h_nom","",fj_bins.size()-1,&(fj_bins[0]));
  
  //for efficiency plot
  TH1D *h_nom_eff_pretag=new TH1D("h_nom_eff_pretag","",fj_bins.size()-1,&(fj_bins[0]));
  
  TH1D* help, *help_rebinned;
  
  double d_fj_bins[fj_bins.size()];
  
  for(int i_b=0; i_b<fj_bins.size(); i_b++) d_fj_bins[i_b]=(double)fj_bins[i_b];
  
  
  int n_bins=fj_bins.size()-1;
  
  double total_errors_up[n_bins],total_errors_down[n_bins], total_errsquare_up[n_bins],total_errsquare_down[n_bins],x_values[n_bins],y_values_up[n_bins],y_values_down[n_bins],x_error_up[n_bins],x_error_down[n_bins];
  
  for(int i=1; i<=n_bins; i++){
    total_errsquare_down[i-1]=0;
    total_errsquare_up[i-1]=0;
    
  }
  
  for(int i_sys=0; i_sys<sys.size(); i_sys++){
    bool isOneSided=false;

    TH1D *h_up=new TH1D("h_up","",fj_bins.size()-1,&(fj_bins[0]));
    TH1D *h_down=new TH1D("h_down","",fj_bins.size()-1,&(fj_bins[0]));

    //for efficiency plot
    TH1D *h_up_eff_pretag=new TH1D("h_up_eff_pretag","",fj_bins.size()-1,&(fj_bins[0]));
    TH1D *h_down_eff_pretag=new TH1D("h_down_eff_pretag","",fj_bins.size()-1,&(fj_bins[0]));

    for(int i_reg=0; i_reg<regions.size(); i_reg++){
      
      //if(doPrintByRegion && !(regions[i_reg].EqualTo(region))) continue;
      
      TString name_mc=regions[i_reg]+"_"+var+"_"+"Nom";
      TString name_mc_up=regions[i_reg]+"_"+var+"_"+sys[i_sys]+"__1up";
      TString name_mc_down=regions[i_reg]+"_"+var+"_"+sys[i_sys]+"__1down";
      //std::cout<<"name_mc: "<<name_mc<<std::endl;
      //std::cout<<"name_mc sys up: "<<name_mc_up<<std::endl;
      //std::cout<<"name_mc sys down: "<<name_mc_down<<std::endl;
      
      
      for(int i_p=0; i_p<m_config->GetPairs().size(); i_p++){

	if(isPosttag || isEff){
	  if(i_sys==0){
	    //only need to get nominal once
	    help=(TH1D*)m_fatjet_histograms_posttag[name_mc][i_p]->Clone();
	    help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
	    if(applyFitCorrection) help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
	    //std::cout<<"Correction factor "<<i_p<<" : "<< m_fit_params[regions[i_reg]+"_"+sys][i_p]<<std::endl;
	    h_nom->Add(help_rebinned);

	    for(int i=1; i<=n_bins; i++){
	      x_values[i-1]=h_nom->GetBinCenter(i);
	      x_error_up[i-1]=h_nom->GetBinWidth(i)/2;
	      x_error_down[i-1]=h_nom->GetBinWidth(i)/2;
	    }

	  }
	  
	  help=(TH1D*)m_fatjet_histograms_posttag[name_mc_up][i_p]->Clone();
	  help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
	  if(applyFitCorrection) help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
	  //std::cout<<"Correction factor "<<i_p<<" : "<< m_fit_params[regions[i_reg]+"_"+sys][i_p]<<std::endl;
	  h_up->Add(help_rebinned);
	  
	  if(m_fatjet_histograms_posttag.find(name_mc_down)==m_fatjet_histograms_posttag.end()){
	    //systematic is one-sided
	    isOneSided=true;
	    continue;
	  }
	 
	  help=(TH1D*)m_fatjet_histograms_posttag[name_mc_down][i_p]->Clone();
	  help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
	  if(applyFitCorrection) help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
	  //std::cout<<"Correction factor "<<i_p<<" : "<< m_fit_params[regions[i_reg]+"_"+sys][i_p]<<std::endl;
	  h_down->Add(help_rebinned);
        }
	if(!isPosttag || isEff){
	  if(i_sys==0){
	    //only need to get nominal once
	    help=(TH1D*)m_fatjet_histograms_pretag[name_mc][i_p]->Clone();
	    help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
	    if(applyFitCorrection) help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
	    //std::cout<<"Correction factor "<<i_p<<" : "<< m_fit_params[regions[i_reg]+"_"+sys][i_p]<<std::endl;
	    if(!isEff) h_nom->Add(help_rebinned);
	    else h_nom_eff_pretag->Add(help_rebinned);
	    
	      for(int i=1; i<=n_bins; i++){
		x_values[i-1]=h_nom->GetBinCenter(i);
		x_error_up[i-1]=h_nom->GetBinWidth(i)/2;
		x_error_down[i-1]=h_nom->GetBinWidth(i)/2;
	      }

	  }
	  
	  help=(TH1D*)m_fatjet_histograms_pretag[name_mc_up][i_p]->Clone();
	  help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
	  if(applyFitCorrection) help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
	  //std::cout<<"Correction factor "<<i_p<<" : "<< m_fit_params[regions[i_reg]+"_"+sys][i_p]<<std::endl;
	  if(!isEff) h_up->Add(help_rebinned);
	  else h_up_eff_pretag->Add(help_rebinned);
	  if(m_fatjet_histograms_pretag.find(name_mc_down)==m_fatjet_histograms_pretag.end()){
	    //systematic is one-sided
	    isOneSided=true;
	    continue;
	  }
	 
	  help=(TH1D*)m_fatjet_histograms_pretag[name_mc_down][i_p]->Clone();
	  help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
	  if(applyFitCorrection) help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
	  //std::cout<<"Correction factor "<<i_p<<" : "<< m_fit_params[regions[i_reg]+"_"+sys][i_p]<<std::endl;
	  if(!isEff) h_down->Add(help_rebinned);
	  else h_down_eff_pretag->Add(help_rebinned);
          
	}


      }
    }

    if(isEff){
      h_nom->Divide(h_nom_eff_pretag);
      h_up->Divide(h_up_eff_pretag);
      if(!isOneSided) h_down->Divide(h_down_eff_pretag);
      
    }
    



    if(isOneSided){
      
      for(int i=1; i<=n_bins; i++){
        
        float delta_up=h_up->GetBinContent(i)-h_nom->GetBinContent(i);
	if(h_nom->GetBinContent(i)) y_values_up[i-1]=1.+delta_up/h_nom->GetBinContent(i);
	
      }

      uncertainty_graphs.push_back(new TGraphAsymmErrors(n_bins,x_values,y_values_up,x_error_down,x_error_up,total_errors_down,total_errors_up));
      uncertainty_graphs[uncertainty_graphs.size()-1]->SetLineColor(color[i_sys]);
      uncertainty_graphs[uncertainty_graphs.size()-1]->SetMarkerStyle(1);
      
    }else{
      
      for(int i=1; i<=n_bins; i++){
        
        float delta_up=h_up->GetBinContent(i)-h_nom->GetBinContent(i);
        float delta_down=h_down->GetBinContent(i)-h_nom->GetBinContent(i);

	if(h_nom->GetBinContent(i))y_values_up[i-1]=1+delta_up/h_nom->GetBinContent(i);
	else y_values_up[i-1]=1.;
	
	if(h_nom->GetBinContent(i))y_values_down[i-1]=1+delta_down/h_nom->GetBinContent(i);
	else y_values_down[i-1]=1.;

      }
      uncertainty_graphs.push_back(new TGraphAsymmErrors(n_bins,x_values,y_values_up,x_error_down,x_error_up,total_errors_down,total_errors_up));
      uncertainty_graphs[uncertainty_graphs.size()-1]->SetLineColor(color[i_sys]);
      uncertainty_graphs[uncertainty_graphs.size()-1]->SetMarkerStyle(1);


      uncertainty_graphs.push_back(new TGraphAsymmErrors(n_bins,x_values,y_values_down,x_error_down,x_error_up,total_errors_down,total_errors_up));
      uncertainty_graphs[uncertainty_graphs.size()-1]->SetLineColor(color[i_sys]);
      uncertainty_graphs[uncertainty_graphs.size()-1]->SetLineStyle(2);
      uncertainty_graphs[uncertainty_graphs.size()-1]->SetMarkerStyle(1);

           
    }
  }
  
  return uncertainty_graphs;


}



TGraphAsymmErrors* ScaleFactorCalculator::getTotalSys(TGraphAsymmErrors* fitsysgraph, TGraphAsymmErrors* btagsysgraph, TGraphAsymmErrors* jetsysgraph, TGraphAsymmErrors* stat){
  
  int n_points=stat->GetN();
  
  double err_squared_tot_up[n_points],err_squared_tot_down[n_points],err_tot_up[n_points],err_tot_down[n_points];
  
  
  for(int i=0; i<n_points; i++){
    
    err_squared_tot_up[i]=0.;
    err_squared_tot_down[i]=0.;
    
    if(fitsysgraph) err_squared_tot_up[i]+=fitsysgraph->GetErrorYhigh(i)*fitsysgraph->GetErrorYhigh(i);
    if(btagsysgraph) err_squared_tot_up[i]+=btagsysgraph->GetErrorYhigh(i)*btagsysgraph->GetErrorYhigh(i);
    if(jetsysgraph) err_squared_tot_up[i]+=jetsysgraph->GetErrorYhigh(i)*jetsysgraph->GetErrorYhigh(i);
    if(stat) err_squared_tot_up[i]+=stat->GetErrorYhigh(i)*stat->GetErrorYhigh(i);
    
    if(fitsysgraph) err_squared_tot_down[i]+=fitsysgraph->GetErrorYlow(i)*fitsysgraph->GetErrorYlow(i);
    if(btagsysgraph) err_squared_tot_down[i]+=btagsysgraph->GetErrorYlow(i)*btagsysgraph->GetErrorYlow(i);
    if(jetsysgraph) err_squared_tot_down[i]+=jetsysgraph->GetErrorYlow(i)*jetsysgraph->GetErrorYlow(i);
    if(stat) err_squared_tot_down[i]+=stat->GetErrorYlow(i)*stat->GetErrorYlow(i);
    
    
    err_tot_up[i]=TMath::Sqrt(err_squared_tot_up[i]);
    err_tot_down[i]=TMath::Sqrt(err_squared_tot_down[i]);
    
  }
  
  TGraphAsymmErrors *g_totalerrors= new TGraphAsymmErrors(n_points,jetsysgraph->GetX(),jetsysgraph->GetY(),jetsysgraph->GetEXlow(),jetsysgraph->GetEXhigh(),err_tot_down,err_tot_up);
  
  return g_totalerrors;
  
} 

TGraphAsymmErrors* ScaleFactorCalculator::getMCStat(TH1* full_mc){
  
  int nbins=full_mc->GetNbinsX();
  
  double x_values[nbins], y_values[nbins], x_error_up[nbins],x_error_down[nbins],y_error_up[nbins],y_error_down[nbins];
  
  for(int i=1; i<=full_mc->GetNbinsX(); i++){
    
    x_values[i-1]=full_mc->GetBinCenter(i);
    x_error_up[i-1]=full_mc->GetBinWidth(i)/2;
    x_error_down[i-1]=full_mc->GetBinWidth(i)/2;
    y_values[i-1]=1.;
    
    if(full_mc->GetBinContent(i)) y_error_up[i-1]=full_mc->GetBinError(i)/full_mc->GetBinContent(i);
    else y_error_up[i-1]=0;
    
    if(full_mc->GetBinContent(i)) y_error_down[i-1]=full_mc->GetBinError(i)/full_mc->GetBinContent(i);
    else y_error_down[i-1]=0;
    
  }
  
  TGraphAsymmErrors *g_mc_stat_errors= new TGraphAsymmErrors(nbins,x_values,y_values,x_error_down,x_error_up,y_error_down,y_error_up);
  
  return g_mc_stat_errors;
  
  
}


void ScaleFactorCalculator::SaveReweightHists(TString &var, TString &outfilename){

   std::vector<float> fj_bins=m_config->GetBins(var);
  std::vector<TString> regions=m_config->GetAllRegions();
  
  std::vector<TH1D*> hist_prefit_mc, hist_postfit_mc;
  TString name_pretag;

  for(int i_p=0; i_p<m_config->GetPairs().size(); i_p++){

    name_pretag="hist_prefit_mc_"+(m_config->GetPairs())[i_p];

    hist_prefit_mc.push_back(new TH1D(name_pretag.Data(),"",fj_bins.size()-1,&(fj_bins[0])));

    name_pretag="hist_postfit_mc_"+(m_config->GetPairs())[i_p];

    hist_postfit_mc.push_back(new TH1D(name_pretag.Data(),"",fj_bins.size()-1,&(fj_bins[0])));
    

  }  

  TH1D* help, *help_rebinned;

  double d_fj_bins[fj_bins.size()];

  for(int i_b=0; i_b<fj_bins.size(); i_b++) d_fj_bins[i_b]=(double)fj_bins[i_b];

  for(int i_reg=0; i_reg<regions.size(); i_reg++){

    TString name_mc=regions[i_reg]+"_"+var+"_"+"Nom";
    //std::cout<<"name_mc: "<<name_mc<<std::endl;

    for(int i_p=0; i_p<m_config->GetPairs().size(); i_p++){
      help=(TH1D*)m_fatjet_histograms_pretag[name_mc][i_p]->Clone();
      help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
      hist_prefit_mc[i_p]->Add(help_rebinned);
      help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
      //std::cout<<"Correction factor "<<i_p<<" : for hist: "<<m_fatjet_histograms_pretag[name_mc][i_p]->GetName()<<" : "<<m_fit_params[regions[i_reg]+"_Nom"][i_p]<<std::endl;
      hist_postfit_mc[i_p]->Add(help_rebinned);
    }
  }

  TH1D* tmp_stacked_mc;
  
  TH1D *full_mc, *full_mc_postfit, *scale_factors;

  scale_factors=new TH1D("scale_factors","",fj_bins.size()-1,&(fj_bins[0]));

  for(int i_p=0; i_p<m_config->GetPairs().size(); i_p++){

    tmp_stacked_mc=(TH1D*)hist_prefit_mc[i_p]->Clone();

    if(i_p==0) full_mc=(TH1D*)tmp_stacked_mc->Clone();
    else full_mc->Add(tmp_stacked_mc);

    tmp_stacked_mc=(TH1D*)hist_postfit_mc[i_p]->Clone();

    if(i_p==0) full_mc_postfit=(TH1D*)tmp_stacked_mc->Clone();
    else full_mc_postfit->Add(tmp_stacked_mc);
    
  }

  full_mc->SetName("full_mc");
  full_mc_postfit->SetName("full_mc_postfit");

  scale_factors->Divide(full_mc_postfit,full_mc);
  scale_factors->SetName("scale_factors");

  TFile *outfile=TFile::Open(outfilename.Data(),"RECREATE");
  outfile->cd();
  full_mc->Write();
  full_mc_postfit->Write();
  scale_factors->Write();
  outfile->Close();


}


//  LocalWords:  ReadInHistograms
