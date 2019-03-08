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
#include "helpers/GlobalConfig.h"
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
  GlobalConfig* m_config;

  TString m_outdir;
  
  bool m_Debug;
  bool m_doMCStatsNP;
  bool m_doFitInFatJetPtBins;
  bool m_doControlPlots;
  bool m_doNFPlots;
  bool m_doCalibSequence;
  int m_nSmoothingPasses;
  std::vector<TString> m_chans;
  std::vector<TString> m_fitpar_names;
  std::vector<float> m_fitpar_start;
  std::vector<float> m_fitpar_low;
  std::vector<float> m_fitpar_high;
  int m_nPseudoExps;
  TString m_pext;
  TString m_xlabel;
  TString m_ylabel;
  TString m_plot_label;
  TString m_sub_label;
  TString m_subsub_label;
  float m_rebinStatThr;

  std::map<TString, TH1D* > m_HistMap;
  std::map<TString, std::vector<double> > m_FitParamsMap;
  std::map<TString, std::vector<double> > m_CovMatMap;
  std::map<TString, std::vector<double> > m_PseudoFitParamsMap;
  std::map<TString, std::vector<double> > m_PseudoFitParamsMapData;

  TH1D* GetRebinHistData(const TString var);
  TH1D* GetRebinHistMC(const TString var, const TString sys, const TString region, const TString flav, const unsigned int scaleType = 0, const unsigned int i_pseudo = 0);
  std::vector<TH1D*> GetRebinHistsMC(const TString var, const TString sys, const unsigned int scaleType = 0, const unsigned int i_pseudo = 0);
  TH1D* GetRebinHistByRegionData(const TString var, const TString region);
  std::vector<TH1D*> GetRebinHistsByRegionMC(const TString var, const TString sys, const TString region, const unsigned int scaleType = 0, const unsigned int i_pseudo = 0);
  float GetFitScale(const TString sys, const TString region, const TString flav);
  float GetFitError(const TString sys, const TString region, const TString flav);
  float GetPseudoFitScale(const TString region, const TString flav, const unsigned int i_pseudo);
  float GetPseudoDataFitScale(const TString region, const TString flav, const unsigned int i_pseudo);

  std::map<TString,std::shared_ptr<TH1D>> m_fatjet_histograms_pretag_data;
  std::map<TString,std::shared_ptr<TH1D>> m_fatjet_histograms_posttag_data;
 
  std::map<TString,std::vector<std::shared_ptr<TH1D>>> m_fatjet_histograms_pretag; //key: <ptregion>_<sys>, element: hists all pairs
  std::map<TString,std::vector<std::shared_ptr<TH1D>>> m_fatjet_histograms_posttag;
  std::map<TString,std::vector<double>> m_fit_params;
  std::map<TString,std::vector<double>> m_fit_errs;
  std::map<TString,std::vector<double>> m_nom_cov_mats; //key: <ptregion>, element: nominal covariance matrix
  
  std::map<TString,std::vector<std::vector<double>>> m_pseudo_fit_params;
  std::map<TString,std::vector<std::vector<double>>> m_pseudo_fit_params_Data;

 public:
	ScaleFactorCalculator();
	ScaleFactorCalculator(TString &cfg_file, TString &output_dir);
	virtual ~ScaleFactorCalculator();

	void ReadConfig(const TString config_path);
	
	SFCalcResult CalculateScaleFactors(TString &sys, bool doPseudo=false, unsigned int i_pseudo=0,bool doPseudoData=false);
	SFCalcResult CalculateScaleFactorsByRegion(TString &sys, bool doPseudo=false, unsigned int i_pseudo=0,bool doPseudoData=false);
	CalibResult CalculateScaleFactorsAndErrors(bool doByRegion=false);
	void MakeCalibrationPlots(CalibResult cl_result, TString plot_type);
	void ReadInFatJetHists(const std::vector<TString> vars, const std::vector<TString> systematics);

        void MakeCorrelationPlots(const TString region);
	void MakeTemplateControlPlots(bool applyFitCorrection, std::shared_ptr<TH1D> dataHist,std::vector<std::shared_ptr<TH1D>> templateHists, TString& channel, TString& region, TString &sys, int rebin, bool isPosttag=false);

	void MakeFatJetControlPlots(TString& var, bool applyFitCorrection, std::vector<TString>& systematics, std::vector<TString>& model_sys, bool doPrintByRegion=false, TString region="DEFAULT");

	void MakeBTaggingRatePlots(std::vector<TString> &systematics, std::vector<TString> &model_sys);

	void MakeNFPlots();

	TString MakeFlavourFractionTable(bool applyFitCorrection, std::vector<std::shared_ptr<TH1D>> templateHists, TString& region);
	TString PrintMuAndError(TString region,std::vector<std::shared_ptr<TH1D>> templateHists);
  
	TGraphAsymmErrors* getFitUncert(TString& var);
	TGraphAsymmErrors* getFitUncertToys(TString& var);

	TGraphAsymmErrors* getTemplateFitUncert(bool applyFitCorrection,std::vector<std::shared_ptr<TH1D>> templateHists, TString& region, TString &sys, int rebin);
	TGraphAsymmErrors* getTemplateFitUncertToys(bool applyFitCorrection,std::vector<std::shared_ptr<TH1D>> templateHists, TString& region, TString &sys, int rebin);

	TGraphAsymmErrors* getFitUncertBTagRate();
	TGraphAsymmErrors* getFitUncertBTagRateToys();

	TGraphAsymmErrors* getModellingUncert(TString &var, std::vector<TString> &systematics, bool applyFitCorrection, bool isPosttag, bool isEff=false);
	
	TGraphAsymmErrors* getBTagUncert(TString &var, bool applyFitCorrection);
        TGraphAsymmErrors* getExperimentalUncert(TString &var, std::vector<TString> &systematics, bool applyFitCorrection, bool isEff=false);
        TGraphAsymmErrors* HistStackToTGraph(const TH1* h_nom, const std::vector<std::pair<TH1*,TH1*> > systematics);

	std::vector<TGraphAsymmErrors*> getExperimentalUncertSeparate(TString &var, std::vector<TString> &systematics, bool applyFitCorrection, bool isPosttag, bool isEff=false);
	TGraphAsymmErrors* getTotalSys(TGraphAsymmErrors* fitsysgraph, TGraphAsymmErrors* btagsysgraph, TGraphAsymmErrors* jetsysgraph, TGraphAsymmErrors* stat, TGraphAsymmErrors* model_sys=0);

	TGraphAsymmErrors* getMCStat(TH1* full_mc);
  
	void SaveReweightHists(TString &var, TString &outfilename);
	void SaveFitCorrectionFactorsSys();

	std::vector<TString> SplitString(TString str, char delim);
	std::vector<float> SplitStringD(TString str, char delim);
};

#endif /* SCALEFACTORCALCULATOR_H_ */
