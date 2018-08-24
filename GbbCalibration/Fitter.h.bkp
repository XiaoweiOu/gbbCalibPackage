/*
 * Fitter.h
 *
 *  Created on: Mar 23, 2016
 *      Author: rjacobs
 */

#ifndef FITTER_H_
#define FITTER_H_

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



class Fitter {

 private:

  std::vector<TString> m_params_names;
  std::vector<double> m_params_in;
  std::vector<double> m_params_low;
  std::vector<double> m_params_high;
  std::vector<double> m_params_stepsize;
  std::vector<double> m_params_fixed;
  std::vector<double> m_params_sampling_errors;
  std::vector<double> m_params_sampling_values;


  static std::shared_ptr<TMinuit> m_Minuit;
  bool m_doMCStatsNP;


 public:
  Fitter();
  Fitter(FitData *fitdata, int n_params, bool m_doMCStatsNP=false);
  virtual ~Fitter();

  void AddParameter(TString name,double val, double err, double low, double high, int doFixed=0);
  void AddMCStatsNuisanceParameters(TString channel,double step_factor,double low_factor, double high_factor);

  void ClearParameters();

  void ClearOutParamsSampling(){
    m_params_sampling_errors.clear();
    m_params_sampling_values.clear();
  }

  void Initialize(bool doRandom=false, bool doRandomTemplate=false);

  void fit();
  void fitMinos();


  static void fcn(int &npar, double* gin, double &f, double *par, int iflag);
  static void fcn_withNP(int &npar, double* gin, double &f, double *par, int iflag);
  static void fcn_rndm(int &npar, double* gin, double &f, double *par, int iflag);
  static void fcn_rndm_withNP(int &npar, double* gin, double &f, double *par, int iflag);
  static void fcn_rndm_template(int &npar, double* gin, double &f, double *par, int iflag);

  void PrintParameters(TString err_mode="Sampling"); //other modes: "Simple","Minos"
  void PrintFlavourFractions(TString& chan, TString err_mode="Sampling"); 
  void GetParameterSimple(int i_par,double &val,double& err){
    m_Minuit->GetParameter(i_par,val,err);
  }
  void GetParMinosErrors(int i_par, double &err_up, double &err_down, double &err, double &gcc){
    m_Minuit->mnerrs(i_par, err_up, err_down, err, gcc);
  }
  void CalculateResultSampling(int N);
  void GetPseudoTemplateFitResult(std::vector<std::vector<double>>& pseudo_params, int N_fits);


  double GetParErrorSampling(int i_par,double &par_val, double& par_err, int N=1000);

  void MakeOutputHistFile(TFile* outfile,TString histname_stub, TString err_mode="Sampling");

  std::vector<double> FitParameters();
  std::vector<double> CovarianceMatrix();

  double getTotalFitError();

  TString getFitStatus(){
    
    return TString(m_Minuit->fCstatu.Data());
  }

};

#endif /* FITTER_H_ */
