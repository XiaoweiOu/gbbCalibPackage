/*
 * ScaleFactorCalculator.h
 *
 *  Created on: Dec 12, 2016
 *      Author: rjacobs
 */

#ifndef SCALEFACTORCALCULATOR_H_
#define SCALEFACTORCALCULATOR_H_

#include "TFile.h"
#include "TString.h"
#include "TH1D.h"
#include <vector>
#include <map>
#include <iostream>
#include "TMinuit.h"
#include <utility>
#include <memory>
#include "FitData.h"
#include "Fitter.h"
#include "Config.h"
#include "TGraphAsymmErrors.h"

struct CalibResult{

  //for ScaleFactor
  std::vector<float> fnominal_sf;
  std::vector<float> ftot_err_up;
  std::vector<float> ftot_err_down;
  std::vector<float> fstat_err;

  //for efficiencies
  std::vector<float> fnominal_data_eff;
  std::vector<float> fnominal_mc_eff;
  std::vector<float> fstat_err_data_eff;
  std::vector<float> ftot_err_data_eff_up;
  std::vector<float> ftot_err_data_eff_down;
  std::vector<float> fstat_err_mc_eff;

};

struct SFCalcResult{
  std::vector<float> fsf;
  std::vector<float> fsf_datastat_err;
  std::vector<float> feff_data;
  std::vector<float> feff_datastat_err;
  std::vector<float> feff_mc;
  std::vector<float> feff_mc_stat_err;
  
};

class ScaleFactorCalculator {

private:

  TString m_inputfile;
  Fitter m_fitter;
  FitData* m_fitdata;
  Config* m_config;


  std::map<TString,std::shared_ptr<TH1D>> m_fatjet_histograms_pretag_data;
  std::map<TString,std::shared_ptr<TH1D>> m_fatjet_histograms_posttag_data;
 
  std::map<TString,std::vector<std::shared_ptr<TH1D>>> m_fatjet_histograms_pretag; //key: <ptregion>_<sys>, element: hists all pairs
  std::map<TString,std::vector<std::shared_ptr<TH1D>>> m_fatjet_histograms_posttag;
  std::map<TString,std::vector<double>> m_fit_params;
  std::map<TString,std::vector<double>> m_nom_cov_mats; //key: <ptregion>, element: nominal covariance matrix
  
  std::map<TString,std::vector<std::vector<double>>> m_pseudo_fit_params;
  std::map<TString,std::vector<std::vector<double>>> m_pseudo_fit_params_Data;

 public:
	ScaleFactorCalculator();
	ScaleFactorCalculator(TString &cfg_file);
	virtual ~ScaleFactorCalculator();
	
	SFCalcResult CalculateScaleFactors(TString &sys, bool doPseudo=false, unsigned int i_pseudo=0,bool doPseudoData=false);
	SFCalcResult CalculateScaleFactorsByRegion(TString &sys, bool doPseudo=false, unsigned int i_pseudo=0,bool doPseudoData=false);
	CalibResult CalculateScaleFactorsAndErrors(bool doByRegion=false);
	void MakeCalibrationPlots(CalibResult cl_result, TString plot_type);
	void ReadInFatJetHists(std::vector<TString>& var, std::vector<TString>& sys);

	void MakeTemplateControlPlots(bool applyFitCorrection, std::shared_ptr<TH1D> dataHist,std::vector<std::shared_ptr<TH1D>> templateHists, TString& channel, TString& region, TString &sys, int rebin, bool isPosttag=false);

	void MakeFatJetControlPlots(TString& var, bool isPosttag, bool applyFitCorrection, std::vector<TString>& sys, std::vector<TString>& model_sys, bool doPrintByRegion=false, TString region="DEFAULT");

	void MakeBTaggingRatePlots(std::vector<TString> &sys, std::vector<TString> &model_sys);

	TString MakeFlavourFractionTable(bool applyFitCorrection, std::shared_ptr<TH1D> dataHist,std::vector<std::shared_ptr<TH1D>> templateHists, TString& channel, TString& region);
	TString PrintMuAndError(TString region,std::vector<std::shared_ptr<TH1D>> templateHists);
  
	TGraphAsymmErrors* getFitUncert(TString& var, bool isPosttag);
	TGraphAsymmErrors* getFitUncertToys(TString& var, bool isPosttag);

	TGraphAsymmErrors* getTemplateFitUncert(bool applyFitCorrection,std::vector<std::shared_ptr<TH1D>> templateHists, TString& region, TString &sys, int rebin);
	TGraphAsymmErrors* getTemplateFitUncertToys(bool applyFitCorrection,std::vector<std::shared_ptr<TH1D>> templateHists, TString& region, TString &sys, int rebin);

	TGraphAsymmErrors* getFitUncertBTagRate();
	TGraphAsymmErrors* getFitUncertBTagRateToys();

	TGraphAsymmErrors* getModellingUncert(TString &var, std::vector<TString> &sys, bool applyFitCorrection, bool isPosttag, bool isEff=false);
	
	TGraphAsymmErrors* getBTagUncert(TString &var, bool applyFitCorrection);
	TGraphAsymmErrors* getExperimentalUncert(TString &name, std::vector<TString> &sys, bool applyFitCorrection, bool isPosttag, bool isEff=false);

	std::vector<TGraphAsymmErrors*> getExperimentalUncertSeparate(TString &var, std::vector<TString> &sys, bool applyFitCorrection, bool isPosttag, bool isEff=false);
	TGraphAsymmErrors* getTotalSys(TGraphAsymmErrors* fitsysgraph, TGraphAsymmErrors* btagsysgraph, TGraphAsymmErrors* jetsysgraph, TGraphAsymmErrors* stat, TGraphAsymmErrors* model_sys=0);

	TGraphAsymmErrors* getMCStat(TH1* full_mc);
  
	void SaveReweightHists(TString &var, TString &outfilename);
	void SaveFitCorrectionFactorsSys();
};

#endif /* SCALEFACTORCALCULATOR_H_ */
