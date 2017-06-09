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

ScaleFactorCalculator::ScaleFactorCalculator() {
  // TODO Auto-generated constructor stub

}

ScaleFactorCalculator::~ScaleFactorCalculator() {
  // TODO Auto-generated destructor stub
  if(m_config) delete m_config;
}


ScaleFactorCalculator::ScaleFactorCalculator(TString &cfg_file){

  gStyle->SetOptStat(0);
  TGaxis::SetMaxDigits(3);

  std::cout<<"==================================================="<<std::endl;
  std::cout<<"| HELLO THIS IS THE GBB SCALE FACTOR CALCULATOR!   "<<std::endl;
  std::cout<<"| Brumm... "<<std::endl;
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

  for(int i_reg=0; i_reg<regions.size(); i_reg++){
    
    tmpl_muo_data=m_config->GetDataHistName(regions[i_reg],mj_maxSd0);
    tmpl_nonmuo_data=m_config->GetDataHistName(regions[i_reg],nmj_maxSd0);
    

    for(int i_sys=0; i_sys<systematics.size(); i_sys++){
      
      tmpl_muo_mc=m_config->GetMCHistNames(regions[i_reg],systematics[i_sys],mj_maxSd0);
      tmpl_nonmuo_mc=m_config->GetMCHistNames(regions[i_reg],systematics[i_sys],nmj_maxSd0);
  
      m_fitdata->SetHistogramNames(chans[0],tmpl_muo_data,tmpl_muo_mc);
      m_fitdata->SetHistogramNames(chans[1],tmpl_nonmuo_data,tmpl_nonmuo_mc);

      m_fitdata->ReadInHistograms();
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

      m_fitter.PrintParameters("Simple");
      m_fitter.fit();
      m_fitter.PrintParameters("Simple");
      
      m_fit_params[ (regions[i_reg]+"_"+systematics[i_sys]) ]=m_fitter.FitParameters();
      
      if(systematics[i_sys].EqualTo("Nom")){ //make control plots

 	this->MakeTemplateControlPlots(false,m_fitdata->GetDataHist(chans[0]),m_fitdata->GetMCHists(chans[0]),chans[0],regions[i_reg],systematics[i_sys],1);
	this->MakeTemplateControlPlots(true,m_fitdata->GetDataHist(chans[0]),m_fitdata->GetMCHists(chans[0]),chans[0],regions[i_reg],systematics[i_sys],1);
		
 	this->MakeTemplateControlPlots(false,m_fitdata->GetDataHist(chans[1]),m_fitdata->GetMCHists(chans[1]),chans[1],regions[i_reg],systematics[i_sys],1);
	this->MakeTemplateControlPlots(true,m_fitdata->GetDataHist(chans[1]),m_fitdata->GetMCHists(chans[1]),chans[1],regions[i_reg],systematics[i_sys],1);
	
      }

      if(systematics[i_sys].EqualTo("Nom")) m_nom_cov_mats[regions[i_reg]]=m_fitter.CovarianceMatrix();

      m_fitdata->ResetHists();

    }
  }

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
  TString pt_name="pt", pt_posttag_name="pt_PREFITPOSTTAG";
  
  std::vector<TString> variables={"pt"};
  std::vector<TString> sys={"Nom"};
  std::vector<TString> variables_posttag;
  for(auto& el : variables)variables_posttag.push_back(TString(el)+"_PREFITPOSTTAG");
  
  this->ReadInFatJetHists(variables,sys);
  this->ReadInFatJetHists(variables_posttag,sys);

  TString ts_pt="pt";
  
  //fat jet control plots
  this->MakeFatJetControlPlots(ts_pt,false,false,nominal);
  this->MakeFatJetControlPlots(ts_pt,false,true,nominal);
  this->MakeFatJetControlPlots(ts_pt,true,false,nominal);
  this->MakeFatJetControlPlots(ts_pt,true,true,nominal);

  //fat jet binning control plots
  for(auto& reg : regions){
    this->MakeFatJetControlPlots(ts_pt,false,false,nominal,true,reg);
    this->MakeFatJetControlPlots(ts_pt,false,true,nominal,true,reg);
    this->MakeFatJetControlPlots(ts_pt,true,true,nominal,true,reg);
  }
  
  CalibResult c_res=this->CalculateScaleFactorsAndErrors();
  this->MakeCalibrationPlots(c_res,"SF");
  this->MakeCalibrationPlots(c_res,"Eff");
  
    
}


SFCalcResult ScaleFactorCalculator::CalculateScaleFactors(TString &sys, bool doPseudo, int i_pseudo){
  //Correct MC histograms by fit factors, subtract from data, BB_data

  std::vector<float> fj_bins=m_config->GetFatJetBins();
  std::vector<TString> regions=m_config->GetAllRegions();
  
  std::vector<TH1F*> hist_pretag_mc, hist_posttag_mc;
  TString name_pretag, name_posttag;

  for(int i_p=0; i_p<m_config->GetPairs().size(); i_p++){

    name_pretag="hist_pretag_mc_"+(m_config->GetPairs())[i_p];
    name_posttag="hist_posttag_mc_"+(m_config->GetPairs())[i_p];

    hist_pretag_mc.push_back(new TH1F(name_pretag.Data(),"",fj_bins.size()-1,&(fj_bins[0])));
    hist_posttag_mc.push_back(new TH1F(name_posttag.Data(),"",fj_bins.size()-1,&(fj_bins[0])));

  }  

  TH1F *hist_pretag_data=new TH1F("hist_pretag_data","",fj_bins.size()-1,&(fj_bins[0]));
  TH1F *hist_posttag_data=new TH1F("hist_posttag_data","",fj_bins.size()-1,&(fj_bins[0])); 
  
  TH1F* help, *help_rebinned;

  double d_fj_bins[fj_bins.size()];

  for(int i_b=0; i_b<fj_bins.size(); i_b++) d_fj_bins[i_b]=(double)fj_bins[i_b];

  for(int i_reg=0; i_reg<regions.size(); i_reg++){
    
    TString mc_name=regions[i_reg]+"_pt_Nom";

    for(int i_p=0; i_p<m_config->GetPairs().size(); i_p++){
      
      help=(TH1F*)m_fatjet_histograms_pretag[mc_name][i_p]->Clone();
      help_rebinned=(TH1F*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
      
      if(doPseudo)help_rebinned->Scale(m_pseudo_fit_params[regions[i_reg]][i_pseudo][i_p]);
      else help_rebinned->Scale(m_fit_params[regions[i_reg]+"_"+sys][i_p]);
      
      hist_pretag_mc[i_p]->Add(help_rebinned);

      help=(TH1F*)m_fatjet_histograms_posttag[mc_name][i_p]->Clone();
      help_rebinned=(TH1F*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
      
      if(doPseudo)help_rebinned->Scale(m_pseudo_fit_params[regions[i_reg]][i_pseudo][i_p]);
      else help_rebinned->Scale(m_fit_params[regions[i_reg]+"_"+sys][i_p]);

      //std::cout<<"Correction factor "<<i_p<<" : "<< m_fit_params[regions[i_reg]+"_"+sys][i_p]<<std::endl;
      hist_posttag_mc[i_p]->Add(help_rebinned);

    }

    TString data_name=regions[i_reg]+"_pt";
    
    help=(TH1F*)m_fatjet_histograms_pretag_data[data_name]->Clone();
    help_rebinned=(TH1F*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
    hist_pretag_data->Add(help_rebinned);
    
    help=(TH1F*)m_fatjet_histograms_posttag_data[data_name]->Clone();
    help_rebinned=(TH1F*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
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



CalibResult ScaleFactorCalculator::CalculateScaleFactorsAndErrors(){

  std::vector<float> sf_nom, err_up, err_down, sf_nom_datastaterr;

  std::vector<float> data_eff_nom, data_eff_err_up, data_eff_err_down, data_eff_nom_datastaterr;
  std::vector<float> mc_eff_nom, mc_eff_stat_err;

  std::vector<float> fj_bins=m_config->GetFatJetBins();

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

    sf_result=this->CalculateScaleFactors(systematics[i_sys]);

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
  
    pseudo_result=this->CalculateScaleFactors(nominal,true,i);
    
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

  std::vector<float> fj_bins=m_config->GetFatJetBins();
  std::vector<float> bin_xerr, bin_x;

  for(int i_bin=0; i_bin<(fj_bins.size()-1); i_bin++){
    bin_xerr.push_back(0.5*(fj_bins[i_bin+1]-fj_bins[i_bin]));
    bin_x.push_back(fj_bins[i_bin]+bin_xerr[i_bin]);
  }

  int N=bin_xerr.size();

  std::vector<float> val_y,tot_y_err_up,tot_y_err_down,stat_y_err,val_ymc,stat_ymc_err;

  if(plot_type.EqualTo("SF")){

    val_y=cl_result.fnominal_sf;
    tot_y_err_up=cl_result.ftot_err_up;
    tot_y_err_down=cl_result.ftot_err_down;
    stat_y_err=cl_result.fstat_err;

  }else if(plot_type.EqualTo("Eff")){

    val_y=cl_result.fnominal_data_eff;
    tot_y_err_up=cl_result.ftot_err_data_eff_up;
    tot_y_err_down=cl_result.ftot_err_data_eff_down;
    stat_y_err=cl_result.fstat_err_data_eff;

    val_ymc=cl_result.fnominal_mc_eff;
    stat_ymc_err=cl_result.fstat_err_mc_eff;

  }
  

  //make TGraph with bands
  TGraphAsymmErrors *SF_band_sys=new TGraphAsymmErrors(N,&(bin_x[0]),&(val_y[0]),&(bin_xerr[0]),&(bin_xerr[0]),&(tot_y_err_down[0]),&(tot_y_err_up[0]));
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

  TGraphErrors *SF_data_stat=new TGraphErrors(N,&(bin_x[0]),&(val_y[0]),&(bin_xerr[0]),&(stat_y_err[0]));
  SF_data_stat->SetMarkerStyle(20);
  SF_data_stat->SetTitle("");

  TGraphErrors *EFF_mc_stat=0;
  
  if(plot_type.EqualTo("Eff")){
    EFF_mc_stat=new TGraphErrors(N,&(bin_x[0]),&(val_ymc[0]),&(bin_xerr[0]),&(stat_ymc_err[0]));
    EFF_mc_stat->SetMarkerStyle(25);
    EFF_mc_stat->SetTitle("");
  }


  TCanvas *canv=new TCanvas("canv","",800,600);
  canv->cd();
  canv->SetTickx();
  canv->SetTicky();
  SF_band_sys->Draw("a2");
  SF_data_stat->Draw("p");
  if(plot_type.EqualTo("Eff")) EFF_mc_stat->Draw("ep");

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

  canv->SaveAs(name.Data());

  if(EFF_mc_stat) delete EFF_mc_stat; 
  delete SF_band_sys;
  delete SF_data_stat;
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

  TH1F* tmp=0, *clone_tmp=0;

  
  for(int i_var=0; i_var<var.size(); i_var++){
  
    TString jettype="fatjet";
    
    for(int i_reg=0; i_reg<regions.size(); i_reg++){
      
      //Get Data Hist for region i_reg
      hist_data=m_config->GetDataHistName(regions[i_reg], var[i_var]);
      
      infile->GetObject(hist_data.Data(),tmp);
      
      std::cout<<"Read in"<<hist_data<<"!!"<<std::endl;
      
      if(tmp) std::cout<<"Read in "<<hist_data<<std::endl;
      else std::cout<<"Error in ScaleFactorCalculator::ReadInFatJetHists(): Can't find histogram "<<hist_data<<std::endl;
      
      clone_tmp=(TH1F*)tmp->Clone();
      clone_tmp->SetDirectory(0);
      
      TString name_data=regions[i_reg]+"_"+var[i_var];
      
      if(var[i_var].Contains("POSTTAG")) m_fatjet_histograms_posttag_data[name_data]=std::shared_ptr<TH1F>((TH1F*)clone_tmp);
      else m_fatjet_histograms_pretag_data[name_data]=std::shared_ptr<TH1F>((TH1F*)clone_tmp);
      
      for(int i_sys=0; i_sys<sys.size(); i_sys++){
        
        
        
        //Get MC Hists for region
        hist_mc=m_config->GetMCHistNames(regions[i_reg],sys[i_sys],var[i_var]);
        
        for(int i_h=0; i_h<hist_mc.size(); i_h++){
          
          infile->GetObject(hist_mc[i_h],tmp);
          
          if(tmp) std::cout<<"Read in "<<hist_mc[i_h]<<std::endl;
          else std::cout<<"Error in ScaleFactorCalculator::ReadInFatJetHists(): Can't find histogram "<<hist_mc[i_h]<<std::endl;
          
          clone_tmp=(TH1F*)tmp->Clone();
          clone_tmp->SetDirectory(0);
          
          TString name_mc=regions[i_reg]+"_"+var[i_var]+"_"+sys[i_sys];
          
          if(var[i_var].Contains("POSTTAG")) m_fatjet_histograms_posttag[name_mc].push_back(std::shared_ptr<TH1F>((TH1F*)clone_tmp));
          else  m_fatjet_histograms_pretag[name_mc].push_back(std::shared_ptr<TH1F>((TH1F*)clone_tmp));
          
          
        }
        
      }
    }
  }
  
  infile->Close();

}


void ScaleFactorCalculator::MakeTemplateControlPlots(bool applyFitCorrection, std::shared_ptr<TH1F> dataHist, std::vector<std::shared_ptr<TH1F>> templateHists, TString& channel, TString& region, TString &sys,  int rebin){
  
  std::shared_ptr<TCanvas> canv(new TCanvas("canv","",600,800));
  canv.get()->cd();
  std::shared_ptr<TPad> pad1(new TPad("pad1","pad1",0,0.25,1,1));
  pad1.get()->SetLeftMargin(0.15);
  pad1.get()->Draw();
  canv->cd();
  std::shared_ptr<TPad> pad2 = std::shared_ptr<TPad>(new TPad("pad2","pad2",0,0,1,0.3));
  pad2->SetLeftMargin(0.15);
  pad2->SetTopMargin(0);
  pad2->SetBottomMargin(0.2);
  pad2.get()->Draw();

  std::vector<int> color={kBlue+1,kAzure-4,kCyan+3,kGreen-9,kOrange};

  //prepare Legend                                                                                                                                                                                                             
  TLegend *leg=new TLegend(0.68,0.3,0.88,0.7);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);

  TH1F* data_hist= (TH1F*)dataHist->Clone();

  if(rebin>1) data_hist->Rebin(rebin);

  THStack *mystack=new THStack("myStack","stack");

  TH1F* tmp_stacked_mc;
  
  TH1F *full_mc;

  for(int i_p=0; i_p<templateHists.size(); i_p++){

    if(!(templateHists[i_p].get())) continue;
    
    tmp_stacked_mc=(TH1F*)templateHists[i_p].get()->Clone();

    if(applyFitCorrection) tmp_stacked_mc->Scale(m_fit_params[region+"_"+sys][i_p]);

    if(rebin>1) tmp_stacked_mc->Rebin(rebin);

    if(i_p==0) full_mc=(TH1F*)tmp_stacked_mc->Clone();
    else full_mc->Add(tmp_stacked_mc);


    tmp_stacked_mc->SetFillColor(color[i_p]);

    mystack->Add(tmp_stacked_mc);

    leg->AddEntry(tmp_stacked_mc,m_config->GetPairs()[i_p],"f");
  }

  
  data_hist->SetTitle("");
  data_hist->SetLabelSize(0,"X");
  data_hist->SetYTitle(Form("events/%.1f",(data_hist->GetBinWidth(1))));
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
  TH1F *h_ratio=(TH1F*)data_hist->Clone();
  h_ratio->Divide(full_mc);
  h_ratio->SetMinimum();

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

  double chi2=data_hist->Chi2Test(full_mc,"UW CHI2/NDF");
  TString text_Chi2=Form("Chi2/NDF = %f",chi2);
  
  pad1->cd();
  leg->Draw();

  TLatex latex2;
  latex2.SetNDC();
  latex2.SetTextAlign(12);
  latex2.SetTextSize(0.04);
  latex2.DrawLatex(0.5,0.8,text_Chi2.Data());

  TString name= applyFitCorrection ? TString("./ctrl_plots/Template_"+channel+"_postfit_"+region+"_sd0.pdf") : TString("./ctrl_plots/Template_"+channel+"_prefit_"+region+"_sd0.pdf"); 

  canv->SaveAs(name.Data());


  delete mystack;
  delete leg;

}


void ScaleFactorCalculator::MakeFatJetControlPlots(TString &var,bool isPosttag, bool applyFitCorrection, TString& sys, bool doPrintByRegion, TString region){
  
  std::shared_ptr<TCanvas> canv(new TCanvas("canv","",600,800));
  canv.get()->cd();
  std::shared_ptr<TPad> pad1(new TPad("pad1","pad1",0,0.25,1,1));
  pad1.get()->SetLeftMargin(0.15);
  pad1.get()->Draw();
  canv->cd();
  std::shared_ptr<TPad> pad2 = std::shared_ptr<TPad>(new TPad("pad2","pad2",0,0.0,1,0.3));
  pad2.get()->SetLeftMargin(0.15);
  pad2->SetTopMargin(0);
  pad2->SetBottomMargin(0.2);
  pad2.get()->Draw();

  std::vector<int> color={kBlue+1,kAzure-4,kCyan+3,kGreen-9,kOrange};

  std::vector<float> fj_bins=m_config->GetFatJetBins();
  std::vector<TString> regions=m_config->GetAllRegions();
  
  std::vector<TH1F*> hist_pretag_mc, hist_posttag_mc;
  TString name_pretag, name_posttag;

  for(int i_p=0; i_p<m_config->GetPairs().size(); i_p++){

    name_pretag="hist_pretag_mc_"+(m_config->GetPairs())[i_p];
    name_posttag="hist_posttag_mc_"+(m_config->GetPairs())[i_p];

    hist_pretag_mc.push_back(new TH1F(name_pretag.Data(),"",fj_bins.size()-1,&(fj_bins[0])));
    hist_posttag_mc.push_back(new TH1F(name_posttag.Data(),"",fj_bins.size()-1,&(fj_bins[0])));

  }  

  TH1F *hist_pretag_data=new TH1F("hist_pretag_data","",fj_bins.size()-1,&(fj_bins[0]));
  TH1F *hist_posttag_data=new TH1F("hist_posttag_data","",fj_bins.size()-1,&(fj_bins[0])); 
  
  TH1F* help, *help_rebinned;

  double d_fj_bins[fj_bins.size()];

  for(int i_b=0; i_b<fj_bins.size(); i_b++) d_fj_bins[i_b]=(double)fj_bins[i_b];

  for(int i_reg=0; i_reg<regions.size(); i_reg++){

    if(doPrintByRegion && !(regions[i_reg].EqualTo(region))) continue;
    
    TString name_mc=regions[i_reg]+"_"+var+"_"+"Nom";

    for(int i_p=0; i_p<m_config->GetPairs().size(); i_p++){
      
      help=(TH1F*)m_fatjet_histograms_pretag[name_mc][i_p]->Clone();
      help_rebinned=(TH1F*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
      if(applyFitCorrection) help_rebinned->Scale(m_fit_params[regions[i_reg]+"_"+sys][i_p]);
      hist_pretag_mc[i_p]->Add(help_rebinned);

      help=(TH1F*)m_fatjet_histograms_posttag[name_mc][i_p]->Clone();
      help_rebinned=(TH1F*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
      if(applyFitCorrection) help_rebinned->Scale(m_fit_params[regions[i_reg]+"_"+sys][i_p]);
      //std::cout<<"Correction factor "<<i_p<<" : "<< m_fit_params[regions[i_reg]+"_"+sys][i_p]<<std::endl;
      hist_posttag_mc[i_p]->Add(help_rebinned);

    }

    TString name_data=regions[i_reg]+"_"+var;
    
    help=(TH1F*)m_fatjet_histograms_pretag_data[name_data]->Clone();
    help_rebinned=(TH1F*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
    hist_pretag_data->Add(help_rebinned);
    
    help=(TH1F*)m_fatjet_histograms_posttag_data[name_data]->Clone();
    help_rebinned=(TH1F*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
    hist_posttag_data->Add(help_rebinned);

  }

  //prepare Legend                                                                                                                                                                                                             
  TLegend *leg=new TLegend(0.68,0.5,0.88,0.85);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);

  TH1F* data_hist= isPosttag ? hist_posttag_data : hist_pretag_data;

  THStack *mystack=new THStack("myStack","stack");

  TH1F* tmp_stacked_mc;
  
  TH1F *full_mc;

  for(int i_p=0; i_p<m_config->GetPairs().size(); i_p++){

    tmp_stacked_mc=isPosttag ?  (TH1F*)hist_posttag_mc[i_p]->Clone() : (TH1F*)hist_pretag_mc[i_p]->Clone();

    if(i_p==0) full_mc=(TH1F*)tmp_stacked_mc->Clone();
    else full_mc->Add(tmp_stacked_mc);


    tmp_stacked_mc->SetFillColor(color[i_p]);

    mystack->Add(tmp_stacked_mc);

    leg->AddEntry(tmp_stacked_mc,m_config->GetPairs()[i_p],"f");
  }

  
  data_hist->SetTitle("");
  data_hist->SetLabelSize(0,"X");
  data_hist->SetYTitle(Form("events/%.1f",(data_hist->GetBinWidth(1))));
  data_hist->SetMarkerStyle(20);
  data_hist->SetTitleOffset(1.5,"Y");

  leg->AddEntry(data_hist,"Data","epl");
  
  data_hist->SetMinimum(0);
  
  pad1->cd();
  data_hist->Draw("EP");
  mystack->Draw("HIST SAME");
  data_hist->Draw("EP SAME");
  
  canv->cd();

  //ratio
  TH1F *h_ratio=(TH1F*)data_hist->Clone();
  h_ratio->Divide(full_mc);
  h_ratio->SetMinimum();

  h_ratio->SetTitle("");
  h_ratio->SetMarkerStyle(20);
  h_ratio->SetYTitle("Data/MC");
  if(var=="pt") h_ratio->SetXTitle("Large-R Jet p_{T} [GeV]");
  else if (var=="m")h_ratio->SetXTitle("Large-R Jet Mass [GeV]");
  else if (var=="D2")h_ratio->SetXTitle("Large-R Jet D2");
  h_ratio->SetLabelSize(0.08,"X");
  h_ratio->SetLabelSize(0.08,"Y");
  h_ratio->SetTitleSize(0.08,"X");
  h_ratio->SetTitleSize(0.08,"Y");
  h_ratio->SetTitleOffset(0.6,"Y");


  pad2->SetGrid();
  pad2->cd();
  h_ratio->Draw("EP");

  double chi2=data_hist->Chi2Test(full_mc,"UW CHI2/NDF");
  TString text_Chi2=Form("Chi2/NDF = %f",chi2);
  
  pad1->cd();
  leg->Draw();

  TLatex latex2;
  latex2.SetNDC();
  latex2.SetTextAlign(12);
  latex2.SetTextSize(0.04);
  //latex2.DrawLatex(0.5,0.8,text_Chi2.Data());

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


//  LocalWords:  ReadInHistograms
