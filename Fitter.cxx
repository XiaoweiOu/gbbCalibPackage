/*
 * Fitter.cxx
 *
 *  Created on: Mar 23, 2016
 *      Author: rjacobs
 */

#include "Fitter.h"
#include "TMath.h"
#include "TMatrixD.h"
#include "TMatrixDEigen.h"


//initialize static member variables
std::shared_ptr<TMinuit> Fitter::m_Minuit=0;

Fitter::Fitter() {
	// TODO Auto-generated constructor stub

}

Fitter::~Fitter() {
	// TODO Auto-generated destructor stub
}


Fitter::Fitter(FitData* fitdata, int n_params, bool doMCStatsNP){

	m_Minuit=std::shared_ptr<TMinuit>(new TMinuit(n_params));
	m_Minuit->SetObjectFit((TObject*)fitdata);
	m_doMCStatsNP=doMCStatsNP;

}


void Fitter::AddParameter(TString name,double val, double err, double low, double high, int doFixed)
{
	m_params_names.push_back(name);
	m_params_in.push_back(val);
	m_params_stepsize.push_back(err);
	m_params_low.push_back(low);
	m_params_high.push_back(high);
	m_params_fixed.push_back(doFixed);

}

void Fitter::AddMCStatsNuisanceParameters(TString channel,double step_factor,double low_factor, double high_factor){
  
  std::vector<double> par, par_err;
  FitData *fitdata=(FitData*)m_Minuit->GetObjectFit();

  fitdata->GetMCStatsNPInitVal(channel,par,par_err);

  for(int i=0; i<par.size(); i++){

    TString NP_name="NP_"+channel+"_"+Form("%i",i);
    this->AddParameter(NP_name,par[i],step_factor*par_err[i],0., par[i]+high_factor*par_err[i],false);
  }

}

void Fitter::ClearParameters(){
  
  m_params_names.clear();
  m_params_in.clear();
  m_params_stepsize.clear();
  m_params_low.clear();
  m_params_high.clear();
  m_params_fixed.clear();


}

void Fitter::Initialize(bool doRandom, bool doRandomTemplate){
  //m_Minuit->SetPrintLevel(-1); //quiet mode
  m_Minuit->SetErrorDef(0.5); //1-sigma for log-likelihood
  
  m_Minuit->Command("CLEAR");
  
  if(!m_doMCStatsNP){
    if(doRandom)m_Minuit->SetFCN(&fcn_rndm); //use likelihood with sampled data for error estimates
    else if(doRandomTemplate) m_Minuit->SetFCN(&fcn_rndm_template);
    else m_Minuit->SetFCN(&fcn);
  }else{
    if(doRandom) m_Minuit->SetFCN(&fcn_rndm_withNP);
    else m_Minuit->SetFCN(&fcn_withNP);
  }
  
  
  //set parameters minuit
  for(int i=0; i<m_params_names.size(); i++){
    //if(i==0) std::cout<<m_params_names[0]<<" is "<<m_params_in[0]<<std::endl;
    m_Minuit->DefineParameter(i,m_params_names[i].Data(),m_params_in[i],m_params_stepsize[i],m_params_low[i],m_params_high[i]);
    if(m_params_fixed[i])  m_Minuit->FixParameter(i);
    
  }
}

void Fitter::fit(){

	m_Minuit->Migrad();
	m_Minuit->Command("HESSE");

}


void Fitter::fitMinos(){

  m_Minuit->Migrad();
  m_Minuit->Command("MINOS");  
}

void Fitter::fcn(int &npar, double* gin, double &f, double *par, int iflag){
  FitData *fitdata=(FitData*)m_Minuit->GetObjectFit();
  f=fitdata->NegLogLikelihood(par);
}

void Fitter::fcn_rndm(int &npar, double* gin, double &f, double *par, int iflag){
        FitData *fitdata=(FitData*)m_Minuit->GetObjectFit();
        f=fitdata->NegLogLikelihoodRndmData(par);
}

void Fitter::fcn_rndm_withNP(int &npar, double* gin, double &f, double *par, int iflag){
  
  FitData *fitdata=(FitData*)m_Minuit->GetObjectFit();
  f=fitdata->NegLogLikelihoodRndmDataWithNP(par);

}

void Fitter::fcn_withNP(int &npar, double* gin, double &f, double *par, int iflag){
  FitData *fitdata=(FitData*)m_Minuit->GetObjectFit();
  f=fitdata->NegLogLikelihoodWithNP(par);
}

void Fitter::fcn_rndm_template(int &npar, double* gin, double &f, double *par, int iflag){
        FitData *fitdata=(FitData*)m_Minuit->GetObjectFit();
        f=fitdata->NegLogLikelihoodRndmTemplate(par);
}

void Fitter::PrintParameters(TString err_mode){

  double par_val, par_err, par_err_up, par_err_down, gcc;
  
  std::cout<<"=============================="<<std::endl;
  std::cout<<"| Parameters:"<<std::endl;
  std::cout<<"|"<<std::endl;
  
  for(int i=0; i<m_params_names.size(); i++){
    
    if(m_params_names[i].Contains("NP")) continue; //Don't print for NP                                                                                                                                                               
    

    this->GetParameterSimple(i,par_val,par_err);
    
    if(err_mode.EqualTo("Sampling")){
      this->GetParErrorSampling(i,par_val,par_err);
      std::cout<<"| ["<<i<<"]: "<<m_params_names[i]<<" "<<par_val<<"+-"<<par_err<<std::endl;
    }else if(err_mode.EqualTo("Minos")){
      this->GetParMinosErrors(i,par_err_up,par_err_down,par_err,gcc);
      std::cout<<"| ["<<i<<"]: "<<m_params_names[i]<<" "<<par_val<<" + "<<par_err_up<<" - "<<par_err_down<<" err: "<<par_err<<std::endl;
      
    }else std::cout<<"| ["<<i<<"]: "<<m_params_names[i]<<" "<<par_val<<"+-"<<par_err<<std::endl;
  }
  
  std::cout<<"=============================="<<std::endl;
  
  
}

void Fitter::PrintFlavourFractions(TString& chan, TString err_mode){
  double par_val, par_err, par_err_up, par_err_down, gcc;
  
  FitData *fitdata=(FitData*)m_Minuit->GetObjectFit();
  
  double flav_frac=0., sum_flav_frac=0., flav_norm, total_mc_norm=0;
  
  std::cout<<"=============================="<<std::endl;
  std::cout<<"| Flavour Fractions:"<<std::endl;
  std::cout<<"|"<<std::endl;
  
  std::cout<<"| before fit :"<<std::endl;
  
  for(int i=0; i<m_params_names.size(); i++){

    if(m_params_names[i].Contains("NP")) continue; //Don't print for NP

    if(!err_mode.EqualTo("Sampling")) this->GetParameterSimple(i,par_val,par_err);
    else this->GetParErrorSampling(i,par_val,par_err);

    flav_frac=fitdata->GetFlavourFraction(chan,i);	      
    flav_norm=fitdata->GetFlavourNorm(chan,i);

    std::cout<<"| [frac "<<i<<"]: "<<m_params_names[i]<<" "<<flav_frac<<std::endl;
    std::cout<<"| [norm "<<i<<"]: "<<m_params_names[i]<<" "<<flav_norm<<std::endl;

    sum_flav_frac+=par_val*flav_frac;
    total_mc_norm+=flav_norm;
  }

  std::cout<<"| [total mc norm]: "<<total_mc_norm<<std::endl;

  std::cout<<"|"<<std::endl;
  std::cout<<"| after fit :"<<std::endl;

  total_mc_norm=0;

  for(int i=0; i<m_params_names.size(); i++){

    if(m_params_names[i].Contains("NP")) continue; //Don't print for NP                                                                                                                                                               

    flav_frac=fitdata->GetFlavourFraction(chan,i);
    flav_norm=fitdata->GetFlavourNorm(chan,i);

    flav_frac*=1./sum_flav_frac;

    if(err_mode.EqualTo("Sampling")){
      this->GetParErrorSampling(i,par_val,par_err);
      std::cout<<"| [frac "<<i<<"]: "<<m_params_names[i]<<" "<<par_val*flav_frac<<"+-"<<par_err*flav_frac<<std::endl;
      std::cout<<"| [norm "<<i<<"]: "<<m_params_names[i]<<" "<<par_val*flav_norm<<"+-"<<par_err*flav_norm<<std::endl;
    }else if(err_mode.EqualTo("Minos")){
      this->GetParameterSimple(i,par_val,par_err);
      this->GetParMinosErrors(i,par_err_up,par_err_down,par_err,gcc);
      std::cout<<"| [ frac "<<i<<"]: "<<m_params_names[i]<<" "<<par_val*flav_frac<<" + "<<par_err_up*flav_frac<<" - "<<par_err_down*flav_frac<<" err: "<<par_err*flav_frac<<std::endl;

    }else if(err_mode.EqualTo("Simple")){ 
      this->GetParameterSimple(i,par_val,par_err);
      std::cout<<"| [frac "<<i<<"]: "<<m_params_names[i]<<" "<<par_val*flav_frac<<"+-"<<par_err*flav_frac<<std::endl;
    }

    total_mc_norm+=par_val*flav_norm;
  }

  std::cout<<"| [total mc norm]: "<<total_mc_norm<<std::endl;

  flav_norm=fitdata->GetDataNorm(chan);
  std::cout<<"| [norm data]: "<<flav_norm<<std::endl;


  std::cout<<"=============================="<<std::endl;

}

void Fitter::CalculateResultSampling(int N){

  m_params_sampling_errors.clear();
  m_params_sampling_values.clear();


  std::vector<std::shared_ptr<TH1D>> histos;
	
  for(int i_par=0; i_par<m_params_names.size(); i_par++) histos.push_back(std::shared_ptr<TH1D>(new TH1D(m_params_names[i_par].Data(),"",1000,0.,10.)));
	
  double par_val=0, par_err=0;
	
  for(int i=0; i<N; i++){
    //std::cout<<"Fit "<<i<<std::endl;

    FitData *fitdata=(FitData*)m_Minuit->GetObjectFit();
    fitdata->MakeBootStrapData();

    this->Initialize(true);
    this->fit();

    for(int i_par=0; i_par<m_params_names.size(); i_par++){
      this->GetParameterSimple(i_par, par_val, par_err);
      //if(i_par==0) std::cout<<"===========================================================after fit value par[0] is "<<par_val<<"================================================================="<<std::endl;
      histos[i_par].get()->Fill(par_val);
    }
  }

  for(int i_par=0; i_par<m_params_names.size(); i_par++){ 
		
    TString name="BootStrapHisto_"+m_params_names[i_par]+".root";
    histos[i_par].get()->SaveAs(name.Data());

    m_params_sampling_errors.push_back(histos[i_par].get()->GetRMS());
    m_params_sampling_values.push_back(histos[i_par].get()->GetMean());
  }
}


void Fitter::GetPseudoTemplateFitResult(std::vector<std::vector<double>>& pseudo_params, int N_fits){
  m_Minuit->SetPrintLevel(-1); //quiet mode 

  for(int i=0; i<N_fits; i++){
    if(!(i%250)) std::cout<<"Fit "<<i<<std::endl;

    FitData *fitdata=(FitData*)m_Minuit->GetObjectFit();
    fitdata->MakeBootStrapTemplates();   
    this->Initialize(false,true);
    this->fit();

    pseudo_params.push_back(this->FitParameters());
  }


}

double Fitter::GetParErrorSampling(int i_par,double &par_val,double &par_err , int N){

  if(m_params_sampling_errors.size()==0) this->CalculateResultSampling(N);
  par_err=m_params_sampling_errors[i_par];
  par_val=m_params_sampling_values[i_par];

}


void Fitter::MakeOutputHistFile(TFile *outfile, TString histname_stub, TString err_mode){
  double par_val, par_err, par_err_up, par_err_down, gcc;

  if(!err_mode.EqualTo("Minos")){
    par_err_up=0.;
    par_err_down=0.;
    gcc=0.;
  }

  TFile *out=outfile;
  if(!out->IsOpen()){
    std::cout<<"Error in MakeOutputHistFile: File not open!"<<std::endl;
    return;
  }

  for(int i=0; i<m_params_names.size(); i++){

    if(m_params_names[i].Contains("NP")) continue; //Don't print for NP                                                                                                                                                               

    if(err_mode.EqualTo("Sampling")){
      this->GetParErrorSampling(i,par_val,par_err);

    }else if(err_mode.EqualTo("Minos")){
      this->GetParameterSimple(i,par_val,par_err);
      this->GetParMinosErrors(i,par_err_up,par_err_down,par_err,gcc);
    }else if(err_mode.EqualTo("Simple")){
      this->GetParameterSimple(i,par_val,par_err);
    }

    TString name="hValue_"+m_params_names[i]+"_"+histname_stub;
    TH1D *hist_value=new TH1D(name.Data(),"",1,0.,1.);
    hist_value->Fill(0.,par_val);

    name="hErr_"+m_params_names[i]+"_"+histname_stub;
    TH1D *hist_err=new TH1D(name.Data(),err_mode.Data(),1,0.,1.);
    hist_err->Fill(0.,par_err);

    name="hErr_up_"+m_params_names[i]+"_"+histname_stub;
    TH1D *hist_err_up=new TH1D(name.Data(),"",1,0.,1.);
    hist_err_up->Fill(0.,par_err_up);


    name="hErr_down_"+m_params_names[i]+"_"+histname_stub;
    TH1D *hist_err_down=new TH1D(name.Data(),"",1,0.,1.);
    hist_err_down->Fill(0.,TMath::Abs(par_err_down));

    out->cd();

    hist_value->Write();
    hist_err->Write();

    if(err_mode.EqualTo("Minos")){
      hist_err_up->Write();
      hist_err_down->Write();
    }

    delete hist_value;
    delete hist_err;
    delete hist_err_up;
    delete hist_err_down;

  }

  //write Covariance Matrix
  int nparams=m_params_names.size();

  TString cov_mat_name="hCovMat_"+histname_stub;
  TH1D *hist_cov_mat=new TH1D(cov_mat_name.Data(),err_mode.Data(),nparams*nparams,0.,(double)(nparams*nparams));

  double *cov_mat=new double[nparams*nparams];
  m_Minuit.get()->mnemat(cov_mat,nparams);

  for(int i=0; i<nparams*nparams; i++){

    hist_cov_mat->Fill(i,cov_mat[i]);

  }
	
  out->cd();
  hist_cov_mat->Write();

  delete hist_cov_mat;

}



double Fitter::getTotalFitError(){

  int n_par=m_Minuit->GetNumPars();

  std::cout<<"npar is"<<n_par<<std::endl;

  double *cov_mat=new double[n_par*n_par];

  m_Minuit.get()->mnemat(cov_mat,n_par);

  for(int i=0; i<n_par; i++){

    std::cout<<"Before diag error "<<i<<i<<"is"<<TMath::Sqrt(cov_mat[i+n_par*i])<<std::endl;

  }



  TMatrixD matrix(n_par,n_par,cov_mat);
  
  TMatrixDEigen eigen_matrix(matrix);

  TMatrixD diag_matrix=eigen_matrix.GetEigenValues();

  double *diag_array=diag_matrix.GetMatrixArray();

  double total_fit_err=0.;

  for(int i=0; i<n_par; i++){

    std::cout<<"Diag error "<<i<<i<<"is"<<TMath::Sqrt(diag_array[i+n_par*i])<<std::endl;

    total_fit_err+=diag_array[i+n_par*i];

  }

  delete cov_mat;

  return TMath::Sqrt(total_fit_err);

}


std::vector<double> Fitter::FitParameters(){

  int n_par=m_Minuit->GetNumPars();
  double val, err;
  std::vector<double> params;

  for(int i_par=0; i_par<n_par; i_par++){

    if(m_params_names[i_par].Contains("NP")) continue; //Don't print for NP
    m_Minuit->GetParameter(i_par,val,err);
    params.push_back(val);
    
    
  }
  
  return params;

}

std::vector<double> Fitter::CovarianceMatrix(){

  int n_par=m_Minuit->GetNumPars();

  double *cov_mat=new double[n_par*n_par];

  m_Minuit.get()->mnemat(cov_mat,n_par);

  //Todo: Treat Nuisance Parameters

  std::vector<double> v_cov_mat(cov_mat,cov_mat+sizeof cov_mat/ sizeof cov_mat[0]);

  return v_cov_mat;


}
