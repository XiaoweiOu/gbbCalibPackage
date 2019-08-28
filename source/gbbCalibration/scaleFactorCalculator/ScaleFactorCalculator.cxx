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
#include "TObjString.h"
#include "PathResolver/PathResolver.h"
#include "TSystem.h"
#include "TEnv.h"

ScaleFactorCalculator::ScaleFactorCalculator() {
  // TODO Auto-generated constructor stub

}

ScaleFactorCalculator::~ScaleFactorCalculator() {
  // TODO Auto-generated destructor stub
  if(m_config) delete m_config;
}


void ScaleFactorCalculator::ReadConfig(const TString config_path){

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

  m_Debug = config->GetValue("doDebug",false);
  std::cout<<"doDebug: "<<m_Debug<<std::endl;

  m_config = new GlobalConfig(PathResolverFindCalibFile("gbbCalibration/configs/GlobalConfig.cfg"));
  std::cout<<"Loaded GlobalConfig"<<std::endl;

  m_doMCStatsNP = config->GetValue("doMCStatsNP",false);
  std::cout<<"doNPStatsMC: "<<m_doMCStatsNP<<std::endl;

  m_doFitInFatJetPtBins = config->GetValue("doFitInFatJetPtBins",false);
  std::cout<<"doFitInFatJetPtBins: "<<m_doFitInFatJetPtBins<<std::endl;

  m_doControlPlots = config->GetValue("doControlPlots",false);
  std::cout<<"doControlPlots: "<<m_doControlPlots<<std::endl;

  m_doNFPlots = config->GetValue("doNFPlots",false);
  std::cout<<"doNFPlots: "<<m_doNFPlots<<std::endl;

  m_doCalibSequence = config->GetValue("doCalibSequence",false);
  std::cout<<"doCalibSequence: "<<m_doCalibSequence<<std::endl;

  m_chans=SplitString(config->GetValue("Channels","muo,nonmuo"),',');
  std::cout<<"Channels: "<<config->GetValue("Channels","")<<std::endl;

  m_nSmoothingPasses=config->GetValue("smoothTemplatesNtimes",0);
  std::cout<<"smoothTemplatesNtimes: "<<m_nSmoothingPasses<<std::endl;

  m_fitpar_names=SplitString(config->GetValue("ParameterNames",""),',');
  std::cout<<"ParameterNames: "<<config->GetValue("ParameterNames","")<<std::endl;

  m_fitpar_start=SplitStringD(config->GetValue("ParameterStart",""),',');
  std::cout<<"ParameterStart: "<<config->GetValue("ParameterStart","")<<std::endl;

  m_fitpar_low=SplitStringD(config->GetValue("ParameterLow",""),',');
  std::cout<<"ParameterLow: "<<config->GetValue("ParameterLow","")<<std::endl;

  m_fitpar_high=SplitStringD(config->GetValue("ParameterHigh",""),',');
  std::cout<<"ParameterHigh: "<<config->GetValue("ParameterHigh","")<<std::endl;

  m_nPseudoExps=config->GetValue("NPseudoExperiments",1000);
  std::cout<<"NPseudoExperiments: "<<m_nPseudoExps<<std::endl;

  m_pext = config->GetValue("PlotExtention",".pdf");
  std::cout<<"PlotExtention: "<<m_pext<<std::endl;

  m_plot_label = config->GetValue("PlotLabel","Internal");
  std::cout<<"PlotLabel: "<<m_plot_label<<std::endl;

  //TODO: make lumi label easily configurable
  m_sub_label = config->GetValue("SubLabel","#sqrt{s} = 13 TeV");//, 36.1 fb^{-1}");
  std::cout<<"SubLabel: "<<m_sub_label<<std::endl;

  m_subsub_label = config->GetValue("SubSubLabel","#bf{g #rightarrow bb calibration}");
  std::cout<<"SubSubLabel: "<<m_subsub_label<<std::endl;

  m_rebinStatThr = config->GetValue("RebinStatThreshold",0.5);
  std::cout<<"RebinStatThreshold: "<<m_rebinStatThr<<std::endl;

  delete config;
  std::cout<<"=============================================="<<std::endl;
}

ScaleFactorCalculator::ScaleFactorCalculator(TString &input_file, TString &cfg_file, TString &output_dir){

  gStyle->SetOptStat(0);
  TGaxis::SetMaxDigits(4);
  TH1::AddDirectory(kFALSE);

  std::cout<<"==================================================="<<std::endl;
  std::cout<<"| HELLO THIS IS THE GBB SCALE FACTOR CALCULATOR!   "<<std::endl;
  //std::cout<<"| Brumm... "<<std::endl;
  std::cout<<"==================================================="<<std::endl;

  m_inputfile = input_file;
  m_outdir = output_dir;

  ReadConfig(cfg_file);

  std::vector<TString> systematics = m_config->GetSystematics();
  systematics.emplace(systematics.begin(),TString("Nom")); //Run over nominal first
  //std::vector<TString> systematics = { "Nom" };
  std::vector<TString> calib_sys = m_config->GetSystematics_Sd0();
  std::vector<TString> btag_sys = m_config->GetSystematics_WeightVar();
  //std::vector<TString> model_sys={"Herwig"}; //if using herwig in R20.7
  std::vector<TString> model_sys={};
  // Store sys names (without up/down label)
  std::vector<TString> sys_only;
  for (TString sys : systematics) {
    if (sys.Contains("Nom")) continue;
    if (sys.Contains("__1up")) sys_only.push_back(TString(sys(0,sys.Length()-5)));
  }

  std::vector<TString> regions = m_doFitInFatJetPtBins ? m_config->GetFatJetRegions() : m_config->GetTrkJetRegions();
  std::vector<TString> tmpl_vars = m_config->GetTemplateVariables();
  std::vector<TString> fitpar_names=m_fitpar_names;
  std::vector<float> fitpar_start=m_fitpar_start;
  std::vector<float> fitpar_low=m_fitpar_low;
  std::vector<float> fitpar_high=m_fitpar_high;

  m_fitdata=new FitData(m_inputfile,tmpl_vars);

  TString tmpl_data;
  std::vector<TString> tmpl_mc;

  TString nominal="Nom";

  std::vector<TString> fitstatus, flav_fractions, mu_factors;

  //std::vector<std::vector<double>> binning;
  std::map<TString, std::vector<double> > binning;

  for (TString region : regions) {
    for (TString sys : systematics) {
      for (TString var : tmpl_vars) {
        tmpl_data = m_config->GetDataHistName(region,var); //FIXME?: less calls to GetDataHistName if this could be out of the systematics loop
        std::cout<<"found hists for "<<region.Data()<<" "<<var.Data()<<std::endl;
        tmpl_mc = m_config->GetMCHistNamesBySys(sys,region,var);
        std::cout<<"found hists for "<<region.Data()<<" "<<sys.Data()<<" "<<var.Data()<<std::endl;
        m_fitdata->SetHistogramNames(var,tmpl_data,tmpl_mc);
      }

      m_fitdata->ReadInHistograms();
      m_fitter = Fitter(m_fitdata, fitpar_names.size()); //FIXME: should this be re-constructed every loop?

      std::cout<<fitpar_names.size()<<std::endl;
      std::cout<<fitpar_start.size()<<std::endl;
      std::cout<<fitpar_low.size()<<std::endl;
      std::cout<<fitpar_high.size()<<std::endl;

      for (unsigned int i_par=0; i_par<fitpar_names.size(); i_par++) {
        std::cout<<"Adding: "<<fitpar_names[i_par]<<" "<<fitpar_start[i_par]<<" "<<fitpar_low[i_par]<<" "<<fitpar_high[i_par]<<std::endl;
        m_fitter.AddParameter(fitpar_names[i_par],fitpar_start[i_par],0.001,fitpar_low[i_par],fitpar_high[i_par]);
      }
      m_fitter.Initialize();

      if (m_nSmoothingPasses > 0) m_fitdata->KernelSmoothTemplates(m_nSmoothingPasses);

      for (TString var : tmpl_vars) {
        if(sys.EqualTo("Nom")) binning[var] = m_fitdata->AutoRebinHistograms(var,m_rebinStatThr,1);
        else m_fitdata->FixHistogramBins(var, binning[var]); //binning[2*i_reg+i_chan]);//why 2*i_reg?
      }

      m_fitter.PrintParameters("Simple");
      m_fitter.fit();
      m_fitter.PrintParameters("Simple");

      fitstatus.push_back(m_fitter.getFitStatus()+"_"+sys+"_"+region);

      m_fit_params[ (region+"_"+sys) ] = m_fitter.FitParameters();
      m_fit_errs[ (region+"_"+sys) ] = m_fitter.FitErrors();

      if(sys.EqualTo("Nom")) m_nom_cov_mats[region] = m_fitter.CovarianceMatrix();

      if (m_doControlPlots) MakeCorrelationPlots(region);

      //TODO: why muon_id__1up? region shouldn't be hard-coded
      //if(sys.EqualTo("Nom") || sys.EqualTo("MUON_ID__1up") || region.Contains("mjpt_g200_nmjpt_g300")){ //make control plots
      if(sys.EqualTo("Nom")) {

        if(m_doControlPlots){
          for (TString var : tmpl_vars) {
            MakeTemplateControlPlots(false,m_fitdata->GetDataHist(var),m_fitdata->GetMCHists(var),var,region,sys,1);
            MakeTemplateControlPlots(true,m_fitdata->GetDataHist(var),m_fitdata->GetMCHists(var),var,region,sys,1);
          }
        }

        //TODO: should this loop over all tmpl_vars?
        flav_fractions.push_back(this->MakeFlavourFractionTableFullCovariance(true,m_fitdata->GetMCHists(tmpl_vars[1]),region));
        mu_factors.push_back(this->PrintMuAndError(region,m_fitdata->GetMCHists(tmpl_vars[1])));
      }

      m_fitdata->ResetHists();
    } // End loop over systematics
  } // End loop over regions

  //FIXME: why even use fitdata here? no fit is being done so we can read the histograms directly instead
  for (TString region : regions) {
    for (TString sys : systematics) {
      for (TString var : tmpl_vars) {
        tmpl_data = m_config->GetDataHistName(region,var+"_PREFITPOSTTAG"); //FIXME?: less calls to GetDataHistName if this could be out of the systematics loop
        tmpl_mc = m_config->GetMCHistNamesBySys(sys,region,var+"_PREFITPOSTTAG");
        m_fitdata->SetHistogramNames(var,tmpl_data,tmpl_mc);
      }

      m_fitdata->ReadInHistograms();

      if(sys.EqualTo("Nom") && m_doControlPlots ){
        for (TString var : tmpl_vars) {
          MakeTemplateControlPlots(false,m_fitdata->GetDataHist(var),m_fitdata->GetMCHists(var),var,region,sys,4,true);
          MakeTemplateControlPlots(true,m_fitdata->GetDataHist(var),m_fitdata->GetMCHists(var),var,region,sys,4,true);
        }
      }
    //FIXME: was only being reset in region loop. bug?
    m_fitdata->ResetHists();
    } // End loop over systematics
  } // End loop over regions


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


  if(m_doCalibSequence){
    std::vector<std::vector<double>> pseudo_exp_result;
    //Run Pseudo-Experiments for template stat. uncertainty
    for (TString region : regions) {
      pseudo_exp_result.clear();

      for (TString var : tmpl_vars) {
        tmpl_data = m_config->GetDataHistName(region,var);
        tmpl_mc = m_config->GetMCHistNamesBySys("Nom",region,var);
        m_fitdata->SetHistogramNames(var,tmpl_data,tmpl_mc);
      }

      m_fitdata->ReadInHistograms();

      m_fitter.GetPseudoTemplateFitResult(pseudo_exp_result, m_nPseudoExps);
      m_pseudo_fit_params[region] = pseudo_exp_result;

      m_fitdata->ResetHists();
    }


    //Run Pseudo-Experiments for data stat uncertainty
    for (TString region : regions) {
      pseudo_exp_result.clear();

      for (TString var : tmpl_vars) {
        tmpl_data = m_config->GetDataHistName(region,var);
        tmpl_mc = m_config->GetMCHistNamesBySys("Nom",region,var);
        m_fitdata->SetHistogramNames(var,tmpl_data,tmpl_mc);
      }

      m_fitdata->ReadInHistograms();

      m_fitter.GetPseudoDataFitResult(pseudo_exp_result, m_nPseudoExps);
      m_pseudo_fit_params_Data[region] = pseudo_exp_result;

      m_fitdata->ResetHists();
    }
  }
  std::cout << "Finished pseudo-experiments" << std::endl;

  //start calibration sequence
  std::vector<TString> none={};
  std::vector<TString> variables = m_config->GetPlotVariables();
  std::vector<TString> variables_posttag, variables_posttag_btagsys;
  //std::vector<TString> calib_var={"mjmaxSd0", "mjmaxSd0_PREFITPOSTTAG"};

  for(auto& el : variables){
  //TODO: can this be mre generic?
    if(el.Contains("ANTITAG") || el.Contains("trjpt") || el.Contains("srj") || el.Contains("evemu") || (el.Contains("fjeta") && el.Contains("fjphi")) || el.Contains("slR4jpt") || el.Contains("trjptfjptratio") || el.Contains("trjptgbbcandratio")) continue;
    variables_posttag.push_back(TString(el)+"_PREFITPOSTTAG");
  }
  for(auto& el : variables_posttag){
    //TODO: not really sure why we skip mjmaxSd0 here
    //bool isTemplate = false;
    //for (TString var : tmpl_vars) {
    //  if(el.Contains(var)) { isTemplate = true; break; }
    //}
    //if (isTemplate) continue;
    for (TString sys : btag_sys) variables_posttag_btagsys.push_back(TString(el)+"_"+sys);
  }
  variables_posttag_btagsys.push_back("mjpt_PREFITUNTAG");

  this->ReadInFatJetHists(variables,systematics);
  this->ReadInFatJetHists(variables_posttag,systematics);
  this->ReadInFatJetHists(variables_posttag_btagsys,{"Nom"});
  if(m_doCalibSequence) this->ReadInFatJetHists(tmpl_vars,calib_sys);

  TString ts_pt="fjpt";

  if(m_doControlPlots){
    for (TString var : variables) {
      //fat jet control plots
      this->MakeFatJetControlPlots(var,false,sys_only,model_sys);
      this->MakeFatJetControlPlots(var,true,sys_only,model_sys);
    }

    //posttag plots
    for (TString var : variables_posttag) {
      this->MakeFatJetControlPlots(var,false,sys_only,model_sys);
      this->MakeFatJetControlPlots(var,true,sys_only,model_sys);
    }

    //BTagging Rate Plots
    this->MakeBTaggingRatePlots(sys_only,model_sys);
  }

  //TString reweight_name="test_reweight_hists.root";
  //this->SaveReweightHists(pt_name, reweight_name);

  if(m_doCalibSequence){

    this->SaveFitCorrectionFactorsSys();
    CalibResult c_res=this->CalculateScaleFactorsAndErrors(true);
    if (m_doFitInFatJetPtBins) {
      MakeCalibrationPlots(c_res,"SF");
      MakeCalibrationPlots(c_res,"Eff");
    } else {
      MakeCalibrationPlots(c_res,"2DSF");
      MakeCalibrationPlots(c_res,"2DEffMC");
      MakeCalibrationPlots(c_res,"2DEffData");
    }
  }

  /*  for(unsigned int i_reg=0; i_reg<regions.size(); i_reg++){
    for(unsigned int i_sys=0; i_sys<systematics.size(); i_sys++){
      std::cout<<"Fit parameters in region "<<regions[i_reg]<<"for sys "<<systematics[i_sys]<<" :"<<std::endl;
      for(unsigned int i_p=0; i_p<m_fit_params[ (regions[i_reg]+"_"+systematics[i_sys]) ].size(); i_p++) std::cout<<m_fit_params[ (regions[i_reg]+"_"+systematics[i_sys]) ][i_p]<<std::endl;
    }
    }*/

	if(m_doNFPlots){
		MakeNFPlots();
	}

}


SFCalcResult ScaleFactorCalculator::CalculateScaleFactors(TString &sys, bool doPseudo, unsigned int i_pseudo, bool doPseudoData){
  //Correct MC histograms by fit factors, subtract from data, BB_data

  std::vector<float> fj_bins = m_config->GetFatJetPtBins();
  //std::vector<TString> regions = m_config->GetAllRegions();

//FIXME: why only Nominal?
  std::vector<TH1D*> hist_pretag_mc_unscaled = GetRebinHistsMC("fjpt", sys, 0);
//NOTE: posttag_unscaled was scaled before
  std::vector<TH1D*> hist_posttag_mc_unscaled = GetRebinHistsMC("fjpt_PREFITPOSTTAG", sys, 0);
  std::vector<TH1D*> hist_pretag_mc;
  std::vector<TH1D*> hist_posttag_mc;
  if (doPseudo) {
    hist_pretag_mc = GetRebinHistsMC("fjpt", sys, 2, i_pseudo);
    hist_posttag_mc = GetRebinHistsMC("fjpt_PREFITPOSTTAG", sys, 2, i_pseudo);
  } else if (doPseudoData) {
    hist_pretag_mc = GetRebinHistsMC("fjpt", sys, 3, i_pseudo);
    hist_posttag_mc = GetRebinHistsMC("fjpt_PREFITPOSTTAG", sys, 3, i_pseudo);
  } else {
    hist_pretag_mc = GetRebinHistsMC("fjpt", sys, 1);
    hist_posttag_mc = GetRebinHistsMC("fjpt_PREFITPOSTTAG", sys, 1);
  }

  TH1D* hist_pretag_data = GetRebinHistData("fjpt");
  TH1D* hist_posttag_data = GetRebinHistData("fjpt_PREFITPOSTTAG");

  //subtract backgrounds and calculate scale factor (or data stat error)
  float N_BB_pretag_mc, N_BB_posttag_mc, N_BB_pretag_data, N_BB_posttag_data, N_total_pretag_mc, N_total_pretag_data, N_total_posttag_data;

  std::vector<float> BB_SF, BB_SF_datastaterr, data_eff, mc_eff, data_eff_datastaterr;

  float err_factor_tagged, err_factor_untagged;

  for (unsigned int i_bin=1; i_bin < fj_bins.size(); i_bin++) {

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
std::cout<<"In ScaleFactorCalculator::CalculateScaleFactorsByRegion"<<std::endl;
  //Correct MC histograms by fit factors, subtract from data, BB_data

  std::vector<TString> regions = m_doFitInFatJetPtBins ? m_config->GetFatJetRegions() : m_config->GetTrkJetRegions();
  // The variable used here doesn't matter since we only care about nEvents in the histograms
  TString var = m_config->GetTemplateVariables()[0];

  //subtract backgrounds and calculate scale factor (or data stat error)

  float N_BB_pretag_mc=0, N_BB_posttag_mc=0, N_BB_pretag_data, N_BB_posttag_data, N_total_pretag_mc=0., N_total_posttag_mc=0., N_total_pretag_data, N_total_posttag_data;

  float N_BB_pretag_mc_unscaled=0. , N_BB_posttag_mc_unscaled=0., N_total_pretag_mc_unscaled=0., N_total_posttag_mc_unscaled=0., N_BB_untag_mc_unscaled=0., N_total_untag_mc_unscaled=0.;

  double Err_BB_posttag_mc_unscaled=0., Err_BB_untag_mc_unscaled=0.;
  double Err_flavour=0., Int_flavour=0.;

  std::vector<float> BB_SF, BB_SF_datastaterr, data_eff, mc_eff, data_eff_datastaterr, mc_eff_staterr;

  //float err_factor_tagged, err_factor_untagged;

  for (TString region : regions) {

    /* TString mc_name=regions[i_reg]+"_fjpt_Nom";
    TString mc_name_posttag=regions[i_reg]+"_fjpt_PREFITPOSTTAG_Nom";

    if(sys.Contains("Rtrk") || sys.Contains("JER") || sys.Contains("JMR")){
      mc_name=regions[i_reg]+"_fjpt_"+sys;
      mc_name_posttag=regions[i_reg]+"_fjpt_PREFITPOSTTAG_"+sys;
    }*/

    //TString mc_name=regions[i_reg]+"_mjmaxSd0_"+sys;
    //TString mc_name_posttag=regions[i_reg]+"_mjmaxSd0_PREFITPOSTTAG_"+sys;
    //TString mc_name_untag=regions[i_reg]+"_mjpt_PREFITUNTAG_"+sys;

    std::vector<TH1D*> hist_pretag_mc_unscaled = GetRebinHistsByRegionMC(var, sys, region, 0);
    std::vector<TH1D*> hist_posttag_mc_unscaled = GetRebinHistsByRegionMC(var+"_PREFITPOSTTAG", sys, region, 0);
    std::vector<TH1D*> hist_pretag_mc;
    std::vector<TH1D*> hist_posttag_mc;
    std::vector<TH1D*> hist_untag_mc_unscaled;
    if (doPseudo) {
      hist_pretag_mc = GetRebinHistsByRegionMC(var, sys, region, 2, i_pseudo);
      hist_posttag_mc = GetRebinHistsByRegionMC(var+"_PREFITPOSTTAG", sys, region, 2, i_pseudo);
    } else if (doPseudoData) {
      hist_pretag_mc = GetRebinHistsByRegionMC(var, sys, region, 3, i_pseudo);
      hist_posttag_mc = GetRebinHistsByRegionMC(var+"_PREFITPOSTTAG", sys, region, 3, i_pseudo);
    } else {
      hist_pretag_mc = GetRebinHistsByRegionMC(var, sys, region, 1);
      hist_posttag_mc = GetRebinHistsByRegionMC(var+"_PREFITPOSTTAG", sys, region, 1);
    }
    if (sys.Contains("Nom")) hist_untag_mc_unscaled = GetRebinHistsByRegionMC("mjpt_PREFITUNTAG", sys, region, 0);
    unsigned int nBinsX = hist_pretag_mc_unscaled[0]->GetNbinsX(); // All hists have same binning

    TH1D* hist_pretag_data = GetRebinHistByRegionData(var, region);
    TH1D* hist_posttag_data = GetRebinHistByRegionData(var+"_PREFITPOSTTAG", region);

    N_total_pretag_mc=0.;
    N_total_posttag_mc=0.;
    N_total_pretag_mc_unscaled=0.;
    N_total_posttag_mc_unscaled=0.;
    N_total_untag_mc_unscaled=0.;
    for (unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++) {

      //consider also overflow and underflow bin in integral
      N_total_pretag_mc_unscaled += hist_pretag_mc_unscaled[i_p]->Integral(0,nBinsX+1);
      N_total_pretag_mc += hist_pretag_mc[i_p]->Integral(0,nBinsX+1);
      if (hist_untag_mc_unscaled.size() > 0) {
        N_total_untag_mc_unscaled += hist_untag_mc_unscaled[i_p]->Integral(0,hist_untag_mc_unscaled[i_p]->GetNbinsX()+1);
      }
      N_total_posttag_mc_unscaled += hist_posttag_mc_unscaled[i_p]->Integral(0,nBinsX+1);
      N_total_posttag_mc += hist_posttag_mc[i_p]->Integral(0,nBinsX+1);

      if (m_config->GetFlavourPairs()[i_p] == "BB") {
        N_BB_pretag_mc_unscaled = hist_pretag_mc_unscaled[i_p]->Integral(0,nBinsX+1);
        N_BB_pretag_mc = hist_pretag_mc[i_p]->Integral(0,nBinsX+1);
        if (hist_untag_mc_unscaled.size() > 0) {
          N_BB_untag_mc_unscaled = hist_untag_mc_unscaled[i_p]->IntegralAndError(0,nBinsX+1,Err_BB_untag_mc_unscaled);
        }
        N_BB_posttag_mc_unscaled = hist_posttag_mc_unscaled[i_p]->Integral(0,nBinsX+1);
        N_BB_posttag_mc = hist_posttag_mc[i_p]->Integral(0,nBinsX+1);

        if(sys.EqualTo("Nom") && region.Contains("l100") && region.Contains("g300") && !doPseudo && !doPseudoData){
          Int_flavour=hist_posttag_mc_unscaled[i_p]->IntegralAndError(0,nBinsX+1,Err_flavour);
          if(Err_flavour/Int_flavour>0.2) std::cout<<"Have bigger than 20% error on flavour fraction"<<i_p<<std::endl;
          if(Err_flavour/Int_flavour>0.3) std::cout<<"Have bigger than 30% error on flavour fraction"<<i_p<<std::endl;
          if(Err_flavour/Int_flavour>0.4) std::cout<<"Have bigger than 40% error on flavour fraction"<<i_p<<std::endl;
          if(Err_flavour/Int_flavour>0.5) std::cout<<"Have bigger than 50% error on flavour fraction"<<i_p<<std::endl;
        }
      }

      //if(doPseudo)help->Scale(m_pseudo_fit_params[regions[i_reg]][i_pseudo][i_p]);
      //else if(doPseudoData) help->Scale(m_pseudo_fit_params_Data[regions[i_reg]][i_pseudo][i_p]);
      //else{
      //FIXME: why use nominal scale for jet systematics?
      //  if(sys.Contains("JET")) help->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
      //  else help->Scale(m_fit_params[regions[i_reg]+"_"+sys][i_p]);
      //}
    }

    N_total_pretag_data = hist_pretag_data->Integral(0,nBinsX+1);
    N_total_posttag_data = hist_posttag_data->Integral(0,nBinsX+1);

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

    //FIXME: which is region 11?
    //if(i_reg==11 && !doPseudo && !doPseudoData){
    //  std::cout<<"Sys is:"<<sys<<std::endl;
    //  std::cout<<"Diff BB is:"<<(m_fit_params[regions[i_reg]+"_Nom"][0]-m_fit_params[regions[i_reg]+"_"+sys][0])/m_fit_params[regions[i_reg]+"_Nom"][0]<<std::endl;
    //  std::cout<<"Diff BL is:"<<(m_fit_params[regions[i_reg]+"_Nom"][1]-m_fit_params[regions[i_reg]+"_"+sys][1])/m_fit_params[regions[i_reg]+"_Nom"][1]<<std::endl;
    //  std::cout<<"Diff CC is:"<<(m_fit_params[regions[i_reg]+"_Nom"][2]-m_fit_params[regions[i_reg]+"_"+sys][2])/m_fit_params[regions[i_reg]+"_Nom"][2]<<std::endl;
    //  std::cout<<"Diff CL is:"<<(m_fit_params[regions[i_reg]+"_Nom"][3]-m_fit_params[regions[i_reg]+"_"+sys][3])/m_fit_params[regions[i_reg]+"_Nom"][3]<<std::endl;
    //  std::cout<<"Diff LL is:"<<(m_fit_params[regions[i_reg]+"_Nom"][4]-m_fit_params[regions[i_reg]+"_"+sys][4])/m_fit_params[regions[i_reg]+"_Nom"][4]<<std::endl;

    //}

    data_eff.push_back(N_BB_posttag_data/N_BB_pretag_data);
    mc_eff.push_back(N_BB_posttag_mc_unscaled/N_BB_pretag_mc_unscaled);

    //data_eff_datastaterr.push_back(TMath::Sqrt(err_factor_tagged*err_factor_tagged*N_total_posttag_data+err_factor_untagged*err_factor_untagged*(N_total_pretag_data-N_total_posttag_data)));

    //Binomial errors
    //data_eff_datastaterr.push_back(TMath::Sqrt(N_BB_posttag_data*(1-N_BB_posttag_data/N_BB_pretag_data))/N_BB_pretag_data);
    //BB_SF_datastaterr.push_back(TMath::Sqrt(N_BB_posttag_data*(1-N_BB_posttag_data/N_BB_pretag_data))/N_BB_pretag_data/(N_BB_posttag_mc_unscaled/N_BB_pretag_mc_unscaled));


    data_eff_datastaterr.push_back(0.);
    BB_SF_datastaterr.push_back(0.);


    //Calculate MC stat uncertainty on MC efficiency
    float delta_eff_mc_term1=(1./N_BB_pretag_mc_unscaled-N_BB_posttag_mc_unscaled/(N_BB_pretag_mc_unscaled*N_BB_pretag_mc_unscaled))*(1./N_BB_pretag_mc_unscaled-N_BB_posttag_mc_unscaled/(N_BB_pretag_mc_unscaled*N_BB_pretag_mc_unscaled))*Err_BB_posttag_mc_unscaled*Err_BB_posttag_mc_unscaled;
    float delta_eff_mc_term2=(N_BB_posttag_mc_unscaled/(N_BB_pretag_mc_unscaled*N_BB_pretag_mc_unscaled))*(N_BB_posttag_mc_unscaled/(N_BB_pretag_mc_unscaled*N_BB_pretag_mc_unscaled))*Err_BB_untag_mc_unscaled*Err_BB_untag_mc_unscaled;
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
std::cout<<"In ScaleFactorCalculator::CalculateScaleFactorsAndErrors"<<std::endl;
  std::vector<float> sf_nom, err_up, err_down, sf_nom_datastaterr;

  std::vector<float> data_eff_nom, data_eff_err_up, data_eff_err_down, data_eff_nom_datastaterr;
  std::vector<float> mc_eff_nom, mc_eff_stat_err;

  std::vector<float> fj_bins;
  std::vector<TString> regions = m_doFitInFatJetPtBins ? m_config->GetFatJetRegions() : m_config->GetTrkJetRegions();
  if(doByRegion){
    int size = regions.size();
    for(int i=0; i<size; i++) fj_bins.push_back((float)i);
    fj_bins.push_back(fj_bins.back()+1.);
  }else fj_bins=m_config->GetFatJetPtBins();

  std::vector<float> current_SF,current_data_eff, current_mc_eff;
  std::vector<float> err_sq_up(fj_bins.size()-1,0.);
  std::vector<float> err_sq_down(fj_bins.size()-1,0.);
  std::vector<float> data_eff_err_sq_up(fj_bins.size()-1,0.);
  std::vector<float> data_eff_err_sq_down(fj_bins.size()-1,0.);

  std::vector<TString> systematics=m_config->GetSystematics();
  systematics.emplace(systematics.begin(),TString("Nom")); //Run over nominal first

  sf_nom.clear();
  err_up.clear();
  err_down.clear();

  SFCalcResult sf_result;

  //calculate SF and sys errors
  std::map<TString,std::vector<double>> error_map_up;
  std::map<TString,std::vector<double>> error_map_down;


   for (TString sys : systematics) {
    //FOR NOW: Don't consider Herwig systematics
    if(sys.Contains("Herwig")) continue;

    if(doByRegion) sf_result=this->CalculateScaleFactorsByRegion(sys);
    else sf_result=this->CalculateScaleFactors(sys);

    if(sys.EqualTo("Nom")){
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

      //if(!sys.EqualTo("Nom")) current_SF[i_bin]=current_SF[i_bin]*current_mc_eff[i_bin]/mc_eff_nom[i_bin];

      //FIXME: what is bin 11?
      //if(i_bin==11)std::cout<<"Region: "<<m_config->GetTrkJetRegions()[i_bin]<<std::endl;

      float diff=current_SF[i_bin]-sf_nom[i_bin];
      //if(i_bin==11) std::cout<<"SF diff: "<<diff<<std::endl;
      //if(i_bin==11) std::cout<<"data eff diff: "<<current_data_eff[i_bin]-data_eff_nom[i_bin]<<std::endl;
      //if(i_bin==11) std::cout<<"MC eff diff: "<<current_mc_eff[i_bin]-mc_eff_nom[i_bin]<<std::endl;

      /*if(TMath::Abs(diff)>0.058){
	std::cout<<"Region: "<<m_config->GetAllRegions()[i_bin]<<std::endl;
	std::cout<<"Big Diff SF for sys "<<sys<< " :"<<diff<<std::endl;
	}*/
      //symmetrize for one-sided systematics (for now hardcoded JER and JMR)
      if (sys.Contains("JER") || sys.Contains("JMR") || sys.Contains("MassRes")) {
        err_sq_up[i_bin]+=diff*diff;
        err_sq_down[i_bin]+=diff*diff;
      } else {
        if(diff>0) err_sq_up[i_bin]+=diff*diff;
        else err_sq_down[i_bin]+=diff*diff;
      }


      float diff_data_eff=current_data_eff[i_bin]-data_eff_nom[i_bin];

      //std::cout<<"Diff data eff for sys "<<sys<< " :"<<diff<<std::endl;

      /*if(TMath::Abs(diff)>0.1 || diff_data_eff> 0.1){
	std::cout<<"Region: "<<m_config->GetAllRegions()[i_bin]<<std::endl;
	std::cout<<"Big Diff SF for sys "<<sys<< " :"<<diff<<std::endl;
	std::cout<<"Big Diff SF for sys "<<sys<< " :"<<diff_data_eff<<std::endl;
	}*/

      //symmetrize for one-sided systematics (for now hardcoded JER and JMR)
      if (sys.Contains("JER") || sys.Contains("JMR") || sys.Contains("MassRes")) {
        data_eff_err_sq_up[i_bin]+=diff_data_eff*diff_data_eff;
        data_eff_err_sq_down[i_bin]+=diff_data_eff*diff_data_eff;
      } else {
        if(diff_data_eff>0) data_eff_err_sq_up[i_bin]+=diff_data_eff*diff_data_eff;
        else data_eff_err_sq_down[i_bin]+=diff_data_eff*diff_data_eff;
      }

//FIXME
      error_map_up[regions[i_bin]].push_back(TMath::Sqrt(err_sq_up[i_bin]));
      error_map_down[regions[i_bin]].push_back(TMath::Sqrt(err_sq_down[i_bin]));

      /*std::cout<<"After: "<<systematics[i_sys]<<std::endl;
      std::cout<<"Up is now "<<TMath::Sqrt(data_eff_err_sq_up[i_bin])<<std::endl;
      std::cout<<"Down is now "<<TMath::Sqrt(data_eff_err_sq_down[i_bin])<<std::endl;
      */
    }
  }

   //FIXME: what is region 11?
   //std::cout<<"Total: "<<std::endl;
   //std::cout<<"Up is now "<<error_map_up[m_config->GetTrkJetRegions()[11]].back()<<std::endl;
   //std::cout<<"Down is now "<<error_map_down[m_config->GetTrkJetRegions()[11]].back()<<std::endl;


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

//FIXME
  for(unsigned int i=0; i<m_pseudo_fit_params[regions[0]].size(); i++){

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
  for(unsigned int i=0; i<m_pseudo_fit_params_Data[regions[0]].size(); i++){

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

    if (m_Debug) {
      std::cout << "PRINTING PSEUDODATA FIT HISTS" << std::endl;
      for (auto bin=0; bin <= help_pseudo.GetNbinsX(); bin++) {
        std::cout << "bin " << bin << " " << help_pseudo.GetBinContent(bin) << std::endl;
      }
    }

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

void ScaleFactorCalculator::ReadInFatJetHists(const std::vector<TString> vars, const std::vector<TString> systematics){
  std::vector<TString> regions = m_doFitInFatJetPtBins ? m_config->GetFatJetRegions() : m_config->GetTrkJetRegions();
  //std::vector<TString> regions = m_config->GetFatJetRegions();

  TFile *infile = TFile::Open(m_inputfile.Data(),"READ");
  if (infile->IsZombie()) {
    std::cerr<<"Failed to open file "<<m_inputfile.Data()<<std::endl;
    return;
  }
  TString histName;
  TH1D *temp(nullptr), *clone(nullptr);

  for (TString region : regions) {
    for (TString var : vars) {
      // Read in data hist
      if ( !var.Contains("BTAG") && !var.Contains("UNTAG") ) {
        histName = m_config->GetDataHistName(region,var).Data();
        infile->GetObject(histName.Data(), temp);
        if (!temp) {
          std::cerr<<"Failed to get histogram "<<histName.Data()<<std::endl;
          return;
        }
        clone = (TH1D*)temp->Clone();
        clone->SetDirectory(0);
        m_HistMap.emplace(histName,clone);
        if (m_Debug) std::cout<<"Added "<<histName.Data()<<" to HistMap"<<std::endl;
        temp = nullptr;
      }
      for (TString sys : systematics) {
        // Read in MC hists
        for (TString histName : m_config->GetMCHistNamesBySys(sys,region,var)) {
          infile->GetObject(histName.Data(), temp);
          if (!temp) {
            std::cerr<<"Failed to get histogram "<<histName.Data()<<std::endl;
            return;
          }
          clone = (TH1D*)temp->Clone();
          clone->SetDirectory(0);
          m_HistMap.emplace(histName,clone);
          if (m_Debug) std::cout<<"Added "<<histName.Data()<<" to HistMap"<<std::endl;
          temp = nullptr;
        } // End loop over flavour pairs
      } // End loop over systematics
    } // End loop over variables
  } // End loop over regions
  infile->Close();
  return;
}

TString ScaleFactorCalculator::MakeFlavourFractionTable(bool applyFitCorrection, std::vector<std::shared_ptr<TH1D>> templateHists, TString& region){

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

    //float err_total=(1./N_total-N_flavour/(N_total*N_total))*(1./N_total-N_flavour/(N_total*N_total))*err_flavour*err_flavour+(-N_flavour/(N_total*N_total))*(-N_flavour/(N_total*N_total))*err_non_flavour*err_non_flavour;
    float err_total=(1./N_total-N_flavour/(N_total*N_total))*(1./N_total-N_flavour/(N_total*N_total))*err_flavour*err_flavour+(-N_flavour/(N_total*N_total))*(-N_flavour/(N_total*N_total))*err_non_flavour;

    flavour_fraction_err[i]=TMath::Sqrt(err_total);
    std::cout<<"flavour_fraction_err "<<flavour_fraction_err[i]<<std::endl;
  }

  //std::cout<<"  BB  "<<"  BL  "<<"  CC  "<<"  CL  "<<"  LL  "<<std::endl;
  TString name=region+" ";
  TString help;

  for(unsigned int i=0; i<flavour_fractions.size(); i++){
    //std::cout<<" "<<flavour_fractions[i]/full_mc->Integral()<<"+-"<<TMath::Sqrt(mat[i][i])/full_mc->Integral();
    //std::cout<<std::endl;
    //help.Form("& $ %.3f \\pm %.3f  $",flavour_fractions[i]/full_mc->Integral(),TMath::Sqrt(mat[i][i])*flavour_fractions[i]/full_mc->Integral());
    help.Form("& $ %.3f \\pm %.3f  $",flavour_fractions[i]/full_mc->Integral(),flavour_fraction_err[i]);
    name.Append(help);
  }

  return name;
}
TString ScaleFactorCalculator::MakeFlavourFractionTableFullCovariance(bool applyFitCorrection, std::vector<std::shared_ptr<TH1D>> templateHists, TString& region){

  std::cout<<"Flavour fraction region: "<<region<<std::endl;

  TH1D* tmp_stacked_mc(nullptr), *full_mc(nullptr);

  std::vector<double> flavour_fractions, flavour_fractions_prefit;

  TVectorD randomv(m_config->GetFlavourPairs().size()), nomv(m_config->GetFlavourPairs().size()), eigenval(m_config->GetFlavourPairs().size());

  TMatrixD eigenvec(m_config->GetFlavourPairs().size(),m_config->GetFlavourPairs().size());


  for(unsigned int i_p=0; i_p<templateHists.size(); i_p++){

    if(!(templateHists[i_p].get())) continue;

    tmp_stacked_mc=(TH1D*)templateHists[i_p].get()->Clone();
    flavour_fractions_prefit.push_back(tmp_stacked_mc->Integral());

    if(applyFitCorrection) tmp_stacked_mc->Scale(m_fit_params[region+"_Nom"][i_p]);


    if(i_p==0) full_mc=(TH1D*)tmp_stacked_mc->Clone();
    else full_mc->Add(tmp_stacked_mc);

    flavour_fractions.push_back(tmp_stacked_mc->Integral());

  }

  std::vector<double> matrix;

  for(unsigned int i_entry=0; i_entry<m_nom_cov_mats[region].size(); i_entry++){
    matrix.push_back((double)(m_nom_cov_mats[region][i_entry])); //get region covariance matrix
  }


  std::vector<double> flavour_fraction_err(flavour_fractions.size(),0.);

  TVectorD v(flavour_fractions.size(),&flavour_fractions[0]);
  TVectorD v2=v;

  TMatrixDSym mat(m_config->GetFlavourPairs().size(),&matrix[0]);
  //mat.UnitMatrix();

  for(unsigned int i=0; i<flavour_fractions.size(); i++){
    for(unsigned int j=0; j<flavour_fractions.size(); j++){
      mat[i][j]*=flavour_fractions_prefit[i]*flavour_fractions_prefit[j]; //make into covariance matrix of flavour norms
    }
  }

  //TMatrixDSymEigen mat_eigen(mat);
  //eigenval=mat_eigen.GetEigenValues();
  //eigenvec=mat_eigen.GetEigenVectors();

  TString name=region+" ";
  TString help;

  float N_total=full_mc->Integral();
  float N_flavour, N_non_flavour, err_flavour, err_non_flavour;

  float err_total;

  for(unsigned int i=0; i<flavour_fractions.size(); i++){

    N_flavour=0;
    N_non_flavour=0;
    err_non_flavour=0;
    err_total=0;

    for(unsigned int j=0; j<flavour_fractions.size(); j++){
      if(i==j){
        N_flavour=flavour_fractions[j];
      }else{
        N_non_flavour+=flavour_fractions[j];
      }
    }

    for(unsigned int j=0; j<flavour_fractions.size(); j++){
      for(unsigned int k=0; k<flavour_fractions.size(); k++){
        if(i==j && i==k){
          err_total+=(N_non_flavour/(N_total*N_total))*(N_non_flavour/(N_total*N_total))*mat[j][k];
        }else if(i==j || i==k){
          err_total+=(N_non_flavour/(N_total*N_total))*(-N_flavour/(N_total*N_total))*mat[j][k];
        }else{
          err_total+=(-N_flavour/(N_total*N_total))*(-N_flavour/(N_total*N_total))*mat[j][k];
        }
      }
    }
    help.Form("& $ %.3f \\pm %.3f  $",flavour_fractions[i]/full_mc->Integral(),TMath::Sqrt(err_total));
    //help.Form("& $ %.3f \\pm %.3f  $",flavour_fractions[i]/full_mc->Integral(),TMath::Sqrt(eigenval[i])*flavour_fractions_prefit[i]/full_mc->Integral());
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

  std::vector<TString> regions = m_doFitInFatJetPtBins ? m_config->GetFatJetRegions() : m_config->GetTrkJetRegions();
  std::vector<TString> systematics=m_config->GetSystematics();

  TString name;
  TCanvas *canv=new TCanvas("canv","",400,300);

  for(unsigned int i_reg=0; i_reg<regions.size(); i_reg++){
  std::vector<TH1D*> hists;
    for(auto &el : names){
      name=el+"_"+regions.at(i_reg);
      hists.push_back(new TH1D(name.Data(),"",25,0.,-1.));
    }

    for(unsigned int i_sys=0; i_sys<systematics.size(); i_sys++){
      if(i_sys==0) std::cout<<"Sys is "<<systematics[i_sys]<<std::endl;
      for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++) hists[i_p]->Fill(m_fit_params[regions[i_reg]+"_"+systematics[i_sys]][i_p]/m_fit_params[regions[i_reg]+"_Nom"][i_p]);

    }

    for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){
      name=m_outdir+"/SF/"+TString(hists[i_p]->GetName())+".pdf";
      canv->cd();
      hists[i_p]->Draw();
      canv->SaveAs(name.Data());
    }
  }
}

TH1D* ScaleFactorCalculator::GetRebinHistData(const TString var) {
  std::vector<TString> regions = m_doFitInFatJetPtBins ? m_config->GetFatJetRegions() : m_config->GetTrkJetRegions();
  std::vector<double> bins = m_config->GetBinning(var);
  if (bins.size() == 0) {
    std::cerr<<"ERROR: couldn't get bins for variable "<<var.Data()<<std::endl;
    return nullptr;
  }
  //float* bins = new float[vec_bins.size()]; //Rebin doesn't take vectors, only pointers
  //for (unsigned int i=0; i < vec_bins.size(); i++) bins[i] = vec_bins[i];

  TH1D *help(nullptr), *sum(nullptr);

  sum = new TH1D(("data_"+var).Data(),"",(int)bins.size()-1,&(bins[0]));
  for (TString region : regions) {
    TString name = m_config->GetDataHistName(region,var);
    help = m_HistMap[name];
    if (!help) {
      std::cerr<<"ERROR: couldn't find histogram "<<name.Data()<<std::endl;
      continue;
    }
    help = (TH1D*) help->Clone(); //Clone to avoid changing the original hist
    //help = (TH1D*) help->Rebin((int)vec_bins.size()-1,(name+"_rebin").Data(),bins);
    help = (TH1D*) help->Rebin((int)bins.size()-1,(name+"_rebin").Data(),&(bins[0]));
    sum->Add(help);
  }
  delete help;
  return sum;
}

TH1D* ScaleFactorCalculator::GetRebinHistByRegionData(const TString var, const TString region) {
  std::vector<double> bins = m_config->GetBinning(var);
  if (bins.size() == 0) {
    std::cerr<<"ERROR: couldn't get bins for variable "<<var.Data()<<std::endl;
    return nullptr;
  }
  //float* bins = new float[vec_bins.size()]; //Rebin doesn't take vectors, only pointers
  //for (unsigned int i=0; i < vec_bins.size(); i++) bins[i] = vec_bins[i];

  TH1D *help(nullptr);

  TString name = m_config->GetDataHistName(region,var);
  help = m_HistMap[name];
  if (!help) {
    std::cerr<<"ERROR: couldn't find histogram "<<name.Data()<<std::endl;
    return nullptr;
  }
  help = (TH1D*) help->Clone(); //Clone to avoid changing the original hist
  //help = (TH1D*) help->Rebin((int)vec_bins.size()-1,(name+"_rebin").Data(),bins);
  help = (TH1D*) help->Rebin((int)bins.size()-1,(name+"_rebin").Data(),&(bins[0]));
  return help;
}

std::vector<TH1D*> ScaleFactorCalculator::GetRebinHistsMC(const TString var, const TString sys, const unsigned int scaleType, const unsigned int i_pseudo) {
  std::vector<TH1D*> output = std::vector<TH1D*>();
  std::vector<TString> regions = m_doFitInFatJetPtBins ? m_config->GetFatJetRegions() : m_config->GetTrkJetRegions();
  std::vector<double> bins = m_config->GetBinning(var);
  if (bins.size() == 0) {
    std::cerr<<"ERROR: couldn't get bins for variable "<<var.Data()<<std::endl;
    return output;
  }

  TH1D *help(nullptr), *sum(nullptr);

  for (TString flav : m_config->GetFlavourPairs()) {
    sum = new TH1D((sys+"_"+flav+"_"+var).Data(),"",(int)bins.size()-1,&(bins[0]));
    for (TString region : regions) {
      help = GetRebinHistMC(var,sys,region,flav,scaleType,i_pseudo);
      if (help) sum->Add(help);
      delete help;
    }
    output.push_back(sum);
  }
  return output;
}

std::vector<TH1D*> ScaleFactorCalculator::GetRebinHistsByRegionMC(const TString var, const TString sys, const TString region, const unsigned int scaleType, const unsigned int i_pseudo) {
  std::vector<TH1D*> output = std::vector<TH1D*>();

  for (TString flav : m_config->GetFlavourPairs()) {
    output.push_back(GetRebinHistMC(var,sys,region,flav,scaleType,i_pseudo));
  }
  return output;
}

TH1D* ScaleFactorCalculator::GetRebinHistMC(const TString var, const TString sys, const TString region, const TString flav, const unsigned int scaleType, const unsigned int i_pseudo) {
  std::vector<double> bins = m_config->GetBinning(var);
  if (bins.size() == 0) {
    std::cerr<<"ERROR: couldn't get bins for variable "<<var.Data()<<std::endl;
    return nullptr;
  }

  TH1D *result(nullptr);

  TString name = m_config->GetMCHistName(sys,region,flav,var);
  result = m_HistMap[name];
  if (!result) {
    std::cerr<<"ERROR: couldn't find histogram "<<name.Data()<<std::endl;
    return nullptr;
  }
  result = (TH1D*) result->Clone(); //Clone to avoid changing the original hist
  result = (TH1D*) result->Rebin((int)bins.size()-1,(name+"_rebin").Data(),&(bins[0]));
  if (scaleType == 1) result->Scale(GetFitScale(sys,region,flav));
  else if (scaleType == 2) result->Scale(GetPseudoFitScale(region,flav,i_pseudo));
  else if (scaleType == 3) result->Scale(GetPseudoDataFitScale(region,flav,i_pseudo));

  return result;
}

float ScaleFactorCalculator::GetFitScale(const TString sys, const TString region, const TString flav) {
  unsigned int i_p=0;
  bool found=false;
  for (; i_p < m_config->GetFlavourPairs().size(); i_p++) {
    if (m_config->GetFlavourPairs()[i_p] == flav) {
      found = true;
      break;
    }
  }
  if (found) return m_fit_params[region+"_"+sys][i_p];
  else return 0;
}

float ScaleFactorCalculator::GetFitError(const TString sys, const TString region, const TString flav) {
  unsigned int i_p=0;
  bool found=false;
  for (; i_p < m_config->GetFlavourPairs().size(); i_p++) {
    if (m_config->GetFlavourPairs()[i_p] == flav) {
      found = true;
      break;
    }
  }
  if (found) return m_fit_errs[region+"_"+sys][i_p];
  else return 0;
}

float ScaleFactorCalculator::GetPseudoFitScale(const TString region, const TString flav, const unsigned int i_pseudo) {
  unsigned int i_p=0;
  bool found=false;
  for (; i_p < m_config->GetFlavourPairs().size(); i_p++) {
    if (m_config->GetFlavourPairs()[i_p] == flav) {
      found = true;
      break;
    }
  }
  if (found) return m_pseudo_fit_params[region][i_pseudo][i_p];
  else return 0;
}

float ScaleFactorCalculator::GetPseudoDataFitScale(const TString region, const TString flav, const unsigned int i_pseudo) {
  unsigned int i_p=0;
  bool found=false;
  for (; i_p < m_config->GetFlavourPairs().size(); i_p++) {
    if (m_config->GetFlavourPairs()[i_p] == flav) {
      found = true;
      break;
    }
  }
  if (found) return m_pseudo_fit_params_Data[region][i_pseudo][i_p];
  else return 0;
}

std::vector<TString> ScaleFactorCalculator::SplitString(TString str, char delim){

  std::vector<TString> tokens;
  TObjArray *Strings=str.Tokenize(delim);

  for(int i=0; i<Strings->GetEntriesFast(); i++){
    tokens.push_back(((TObjString*) (*Strings)[i])->GetString());
  }

  return tokens;
}

std::vector<float> ScaleFactorCalculator::SplitStringD(TString str, char delim){

  std::vector<float> tokens;
  TObjArray *Strings=str.Tokenize(delim);

  for(int i=0; i<Strings->GetEntriesFast(); i++){
    tokens.push_back((((TObjString*) (*Strings)[i])->GetString()).Atof());
    std::cout<<"Token: "<<((TObjString*) (*Strings)[i])->GetString()<<" vs "<<tokens[i]<<std::endl;
  }

  return tokens;
}

//  LocalWords:  ReadInHistograms
