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

//FIXME: this function is unused. do we need it?
TGraphAsymmErrors* ScaleFactorCalculator::getTemplateFitUncert(bool applyFitCorrection, std::vector<std::shared_ptr<TH1D>> templateHists, TString& region, TString &sys, int rebin){

  TH1D* tmp_stacked_mc(nullptr), *hist_total(nullptr);
  std::vector<double> flavour_norms;
 
  for(unsigned int i_p=0; i_p<templateHists.size(); i_p++){
    if(!(templateHists[i_p].get())) continue;

    tmp_stacked_mc=(TH1D*)templateHists[i_p].get()->Clone();

    if(applyFitCorrection) tmp_stacked_mc->Scale(m_fit_params[region+"_"+sys][i_p]);
    if(rebin>1) tmp_stacked_mc->Rebin(rebin);

    if(i_p==0) hist_total=(TH1D*)tmp_stacked_mc->Clone();
    else hist_total->Add(tmp_stacked_mc);

    flavour_norms.push_back(tmp_stacked_mc->Integral());
  }

  TVectorD v(flavour_norms.size(),&flavour_norms[0]);
  TMatrixD mat(flavour_norms.size(),flavour_norms.size(),&m_nom_cov_mats[region][0]);

  double fit_total_error_slice=(mat*v)*v;

  int n_bins=hist_total->GetNbinsX();

  double* fiterrors_up = new double[n_bins];
  double* fiterrors_down = new double[n_bins];
  double* tot_err_sq = new double[n_bins];
  double* x_values = new double[n_bins];
  double* y_values = new double[n_bins];
  double* x_error_up = new double[n_bins];
  double* x_error_down = new double[n_bins];

  for(int ib=0; ib<n_bins; ib++)
    tot_err_sq[ib]=fit_total_error_slice;

  for(int i=1; i<=hist_total->GetNbinsX(); i++){
    x_values[i-1]=hist_total->GetBinCenter(i);
    x_error_up[i-1]=hist_total->GetBinWidth(i)/2;
    x_error_down[i-1]=hist_total->GetBinWidth(i)/2;
    y_values[i-1]=1.;

    //std::cout<<"total error squared"<<(tot_err_sq[i-1])<<std::endl;
    //std::cout<<"evts"<<(hist_total->GetBinContent(i))<<std::endl;

    //FIXME: this is just TMath::Sqrt(tot_err_sq[i-1])/hist_total->Integral(), is that correct?
    if(hist_total->GetBinContent(i)) fiterrors_up[i-1]=TMath::Sqrt(tot_err_sq[i-1])/hist_total->GetBinContent(i)*(hist_total->GetBinContent(i)/hist_total->Integral());
    else fiterrors_up[i-1]=0;

    if(hist_total->GetBinContent(i)) fiterrors_down[i-1]=TMath::Sqrt(tot_err_sq[i-1])/hist_total->GetBinContent(i)*(hist_total->GetBinContent(i)/hist_total->Integral());
    else fiterrors_down[i-1]=0;

    //std::cout<<"FitError is:"<<fiterrors_up[i-1]<<std::endl;
  }

  TGraphAsymmErrors *g_fiterrors= new TGraphAsymmErrors(n_bins,x_values,y_values,x_error_down,x_error_up,fiterrors_down,fiterrors_up);
  return g_fiterrors;

}

TGraphAsymmErrors* ScaleFactorCalculator::getTemplateFitUncertToys(bool applyFitCorrection, std::vector<std::shared_ptr<TH1D>> templateHists, TString& region, TString &sys, int rebin){

  TH1D* tmp_stacked_mc(nullptr), *hist_total(nullptr);
  std::vector<TH1D*> toy_bins;
  TRandom3 r(0);

  TVectorD randomv(m_config->GetFlavourPairs().size()), nomv(m_config->GetFlavourPairs().size()), eigenval(m_config->GetFlavourPairs().size());
  for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++) nomv(i_p)=m_fit_params[region+"_"+sys][i_p];

  TMatrixD eigenvec(m_config->GetFlavourPairs().size(),m_config->GetFlavourPairs().size());

  std::vector<double> nominal;

  TMatrixDSym mat(m_config->GetFlavourPairs().size(),&m_nom_cov_mats[region][0]);
  //mat.UnitMatrix();
  
  TMatrixDSymEigen mat_eigen(mat);
  eigenval=mat_eigen.GetEigenValues();
  eigenvec=mat_eigen.GetEigenVectors();

  int n_toys=2000;
  for( int i_toy=0; i_toy<n_toys; i_toy++){

    if(i_toy==0){ //nominal rate
      randomv=nomv;
    }else{
      for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){
	randomv(i_p)=r.Gaus(0.,TMath::Sqrt(eigenval(i_p)));
      }
      
      randomv=eigenvec*randomv;
      randomv+=nomv;
    }

    for(unsigned int i_p=0; i_p<templateHists.size(); i_p++){
      if(!(templateHists[i_p].get())) continue;
 
      tmp_stacked_mc=(TH1D*)templateHists[i_p].get()->Clone();

      if(applyFitCorrection) tmp_stacked_mc->Scale(randomv(i_p));
      if(rebin>1) tmp_stacked_mc->Rebin(rebin);
 
      if(i_p==0) hist_total=(TH1D*)tmp_stacked_mc->Clone();
      else hist_total->Add(tmp_stacked_mc);
    }

    for(int i_bin=1; i_bin<=hist_total->GetNbinsX(); i_bin++){
      if(i_toy==0){
	nominal.push_back(hist_total->GetBinContent(i_bin));
	toy_bins.push_back(new TH1D("toy_bins","",50,0,2));
      }
      else toy_bins[i_bin-1]->Fill(hist_total->GetBinContent(i_bin)/nominal[i_bin-1]);
    }
  }//toys

  if (!hist_total) {
    std::cout<<"ScaleFactorCalculator::getTemplateFitUncertToys: No histograms found!"<<std::endl;
    return nullptr;
  }
  int n_bins=hist_total->GetNbinsX();
  
  double* x_values = new double[n_bins];
  double* y_values = new double[n_bins];
  double* x_error_up = new double[n_bins];
  double* x_error_down = new double[n_bins];
  double* y_error_up = new double[n_bins];
  double* y_error_down = new double[n_bins];
  
  double param(0.);
  
  TCanvas* can_toys=new TCanvas("can_toys","",700,600);
  for(int i_bin=1; i_bin<=hist_total->GetNbinsX(); i_bin++){
    TString name=m_outdir+"/ctrl_plots/toys/Templates_toy_bin_";
    if(TString(hist_total->GetName()).Contains("posttag")) name+="posttag_";
    name+=i_bin;
    name+=".pdf";
    if(toy_bins[i_bin-1]->Integral() > 0){
      toy_bins[i_bin-1]->Fit("gaus");
      can_toys->cd();
      toy_bins[i_bin-1]->Draw("HIST");
      TF1* fitfunc= toy_bins[i_bin-1]->GetFunction("gaus");
      fitfunc->SetLineColor(kRed);
      fitfunc->SetLineWidth(2);
      fitfunc->Draw("same");
      param=fitfunc->GetParameter(2);
      can_toys->Print(name.Data());
    }else param=0.;

    delete toy_bins[i_bin-1];
    
    x_values[i_bin-1]=hist_total->GetBinCenter(i_bin);
    x_error_up[i_bin-1]=hist_total->GetBinWidth(i_bin)/2;
    x_error_down[i_bin-1]=hist_total->GetBinWidth(i_bin)/2;
    y_values[i_bin-1]=1.;

    y_error_up[i_bin-1]=param/2.;
    y_error_down[i_bin-1]=param/2.;
  }

  delete can_toys;
  delete hist_total;

  TGraphAsymmErrors *g_fiterrors= new TGraphAsymmErrors(n_bins,x_values,y_values,x_error_down,x_error_up,y_error_down,y_error_up);
  return g_fiterrors;

}

//FIXME: this function is unused. do we need it?
TGraphAsymmErrors* ScaleFactorCalculator::getFitUncert(TString& var, bool isPosttag){
  
  //Calculate fit errors from covariance matrix for each flavour pair and each fit region, add errors in fit regions in qudrature.
  
  std::vector<TH1D*> hist_pretag_mc, hist_posttag_mc;
  TString name_pretag, name_posttag;
 
  TString var_fjpt="fjpt";
  std::vector<double> fj_bins= (var.Contains("fjphi") && var.Contains("fjeta")) ? m_config->GetBinning(var_fjpt) : m_config->GetBinning(var);
  std::vector<TString> regions = m_doFitInFatJetPtBins ? m_config->GetFatJetRegions() : m_config->GetTrkJetRegions();
  
  TH1D *hist_total=new TH1D("hist_total","",fj_bins.size()-1,&(fj_bins[0]));
  
  std::map<TString,double> fit_total_error_slice;
  
  std::vector<double> help_norms(fj_bins.size()-1,0.);
  std::map<TString,std::vector<double>> region_norms;
  
  for (TString region : regions) {

    help_norms.assign(fj_bins.size()-1,0.);
    std::vector<double> flavour_norms(m_config->GetFlavourPairs().size(),0.);

    std::vector<TH1D*> help_vec = GetRebinHistsByRegionMC(var,"Nom",region, isPosttag);

    for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){
      flavour_norms[i_p]=help_vec[i_p]->Integral();
      hist_total->Add(help_vec[i_p]);
      
      for(int i_bin=1; i_bin<=help_vec[i_p]->GetNbinsX(); i_bin++){
        help_norms[i_bin-1]+=help_vec[i_p]->GetBinContent(i_bin);
      }
      delete help_vec[i_p];

    }
    TVectorD v(flavour_norms.size(),&flavour_norms[0]);
    TMatrixD mat(flavour_norms.size(),flavour_norms.size(),&m_nom_cov_mats[region][0]);

    fit_total_error_slice[region] = (mat*v)*v;
    //std::cout<<"fit total error slice: "<<fit_total_error_slice[region]<<std::endl;

    region_norms[region]=help_norms;
  }
  
  int n_bins=fj_bins.size()-1;
  
  double* fiterrors_up = new double[n_bins];
  double* fiterrors_down = new double[n_bins];
  double* tot_err_sq = new double[n_bins];
  double* x_values = new double[n_bins];
  double* y_values = new double[n_bins];
  double* x_error_up = new double[n_bins];
  double* x_error_down = new double[n_bins];
  
  //loop over bins, check fraction contributed by fit region in each bin, calculate error
  for(int i_bin=1; i_bin<=n_bins; i_bin++){
    tot_err_sq[i_bin-1]=0;
    for(TString region : regions) {

      if(hist_total->GetBinContent(i_bin)) tot_err_sq[i_bin-1]+=fit_total_error_slice[region]*region_norms[region][i_bin-1]/hist_total->GetBinContent(i_bin);

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

  delete hist_total;

  TGraphAsymmErrors *g_fiterrors= new TGraphAsymmErrors(n_bins,x_values,y_values,x_error_down,x_error_up,fiterrors_down,fiterrors_up);
  return g_fiterrors;

}

TGraphAsymmErrors* ScaleFactorCalculator::getFitUncertToys(TString& var, bool isPosttag){
  
//Calculate fit errors from covariance matrix for each flavour pair and each fit region, add errors in fit regions in qudrature.
  
  std::vector<TH1D*> toy_bins;
 
  std::vector<double> bins = m_config->GetBinning(var);
  if (bins.size() == 0) {
    std::cerr<<"ERROR: couldn't get bins for variable "<<var.Data()<<std::endl;
    return nullptr;
  }
  std::vector<TString> regions = m_doFitInFatJetPtBins ? m_config->GetFatJetRegions() : m_config->GetTrkJetRegions();
  
  for(unsigned int i_b=0; i_b<bins.size(); i_b++){
    toy_bins.push_back(new TH1D("toy_bins","",50,0,2));
  }

  TH1D *hist_total=new TH1D("hist_total","",bins.size()-1,&(bins[0]));
  
  TRandom3 r(0);
  TVectorD randomv(m_config->GetFlavourPairs().size()), nomv(m_config->GetFlavourPairs().size()), eigenval(m_config->GetFlavourPairs().size());
  TMatrixD eigenvec(m_config->GetFlavourPairs().size(),m_config->GetFlavourPairs().size());

  std::vector<double> nominal(bins.size()-1,0.);

  int n_toys=2000;
  for(int i_toy=0; i_toy<n_toys; i_toy++){
    hist_total->Reset();
    if(!(i_toy%50)) std::cout<<"Throw toys"<<i_toy<<" of "<<n_toys<<std::endl;
    
    for (TString region : regions) {

      TMatrixDSym mat(m_config->GetFlavourPairs().size(),&m_nom_cov_mats[region][0]);
      //mat.UnitMatrix();
 
      TMatrixDSymEigen mat_eigen(mat);
      eigenval=mat_eigen.GetEigenValues();
      eigenvec=mat_eigen.GetEigenVectors();

      if(i_toy==0){ //nominal rate
	for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++) nomv(i_p)=m_fit_params[region+"_Nom"][i_p];
	randomv=nomv;
      }else{
	for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){
	  randomv(i_p)=r.Gaus(0.,TMath::Sqrt(eigenval(i_p)));
	  nomv(i_p)=m_fit_params[region+"_Nom"][i_p];
	}

	randomv=eigenvec*randomv;
	randomv+=nomv;
      }

      std::vector<TH1D*> help_vec = GetRebinHistsByRegionMC(var,"Nom",region);
      for(unsigned int i_p=0; i_p < help_vec.size(); i_p++) {
	help_vec[i_p]->Scale(randomv(i_p));
	hist_total->Add(help_vec[i_p]);
        delete help_vec[i_p];
      }//fpairs
    }//regions
    
    for(int i_bin=1; i_bin<=hist_total->GetNbinsX(); i_bin++){
      if(i_toy==0) nominal[i_bin-1]=hist_total->GetBinContent(i_bin);
      else toy_bins[i_bin-1]->Fill(hist_total->GetBinContent(i_bin)/nominal[i_bin-1]);
    }
  
  }//toys

  int n_bins=hist_total->GetNbinsX();
  
  double* x_values = new double[n_bins];
  double* y_values = new double[n_bins];
  double* x_error_up = new double[n_bins];
  double* x_error_down = new double[n_bins];
  double* y_error_up = new double[n_bins];
  double* y_error_down = new double[n_bins];

  double param;

  TCanvas* can_toys=new TCanvas("can_toys","",700,600);
  for(int i_bin=1; i_bin<=hist_total->GetNbinsX(); i_bin++){
    TString name=m_outdir+"/ctrl_plots/toys/"+var+"_toy_bin_";
    name+=i_bin;
    name+=".pdf";
    if(toy_bins[i_bin-1]->Integral()){
      toy_bins[i_bin-1]->Fit("gaus");

      can_toys->cd();
      toy_bins[i_bin-1]->Draw("HIST");
      TF1* fitfunc= toy_bins[i_bin-1]->GetFunction("gaus");
      fitfunc->SetLineColor(kRed);
      fitfunc->SetLineWidth(2);
      fitfunc->Draw("same");
      param=fitfunc->GetParameter(2);
      can_toys->SaveAs(name.Data());
    }else param=0.;

    delete toy_bins[i_bin-1];
    
    x_values[i_bin-1]=hist_total->GetBinCenter(i_bin);
    x_error_up[i_bin-1]=hist_total->GetBinWidth(i_bin)/2;
    x_error_down[i_bin-1]=hist_total->GetBinWidth(i_bin)/2;
    y_values[i_bin-1]=1.;

    y_error_up[i_bin-1]=param/2.;
    y_error_down[i_bin-1]=param/2.;

  }

  delete hist_total;

  TGraphAsymmErrors *g_fiterrors= new TGraphAsymmErrors(n_bins,x_values,y_values,x_error_down,x_error_up,y_error_down,y_error_up);
  return g_fiterrors;
}

//FIXME: this function is unused. do we need it?
TGraphAsymmErrors* ScaleFactorCalculator::getFitUncertBTagRate(){
  
  TString var="fjpt";

  //Calculate fit errors from covariance matrix for each flavour pair and each fit region, add errors in fit regions in qudrature.
  
  std::vector<TH1D*> hist_pretag_mc, hist_posttag_mc;
  TString name_pretag, name_posttag;
 
  std::vector<double> fj_bins=m_config->GetBinning(var);
  std::vector<TString> regions = m_doFitInFatJetPtBins ? m_config->GetFatJetRegions() : m_config->GetTrkJetRegions();
  
  TH1D *hist_total=new TH1D("hist_total","",fj_bins.size()-1,&(fj_bins[0]));
  TH1D *hist_total_posttag=new TH1D("hist_total_posttag","",fj_bins.size()-1,&(fj_bins[0]));

  std::map<TString,double> norm_region_total;
  std::map<TString,double> norm_region_total_posttag;

  std::map<TString,double> fit_total_error_slice;
  std::map<TString,double> fit_total_error_slice_untagged;
  std::map<TString,double> fit_total_error_slice_combined;

  std::vector<double> help_norms(fj_bins.size()-1,0.);
  std::map<TString,std::vector<double>> region_norms;

  std::vector<double> help_norms_posttag(fj_bins.size()-1,0.);
  std::map<TString,std::vector<double>> region_norms_posttag;

  for (TString region : regions) {

    help_norms.assign(fj_bins.size()-1,0.);
    help_norms_posttag.assign(fj_bins.size()-1,0.);
    
    std::vector<double> flavour_norms(m_config->GetFlavourPairs().size(),0.);
    std::vector<double> flavour_norms_posttag(m_config->GetFlavourPairs().size(),0.);
    
    std::vector<TH1D*> hist_pretag_mc = GetRebinHistsByRegionMC(var,"Nom",region,0);
    std::vector<TH1D*> hist_posttag_mc = GetRebinHistsByRegionMC(var,"Nom",region,1);
    for(unsigned int i_p=0; i_p < hist_pretag_mc.size(); i_p++) {
      hist_total->Add(hist_pretag_mc[i_p]);
      flavour_norms[i_p]=hist_pretag_mc[i_p]->Integral();
      norm_region_total[region]+=hist_pretag_mc[i_p]->Integral();
      //std::cout<<"flavour_norm: "<<flavour_norms[i_p]<<std::endl;
        
      for(int i_bin=1; i_bin<=hist_pretag_mc[i_p]->GetNbinsX(); i_bin++){
	help_norms[i_bin-1]+=hist_pretag_mc[i_p]->GetBinContent(i_bin);
      }
      delete hist_pretag_mc[i_p];
    }//fpairs

    for(unsigned int i_p=0; i_p < hist_posttag_mc.size(); i_p++) {
      hist_total_posttag->Add(hist_posttag_mc[i_p]);
      flavour_norms_posttag[i_p]=hist_posttag_mc[i_p]->Integral();
      norm_region_total_posttag[region]+=hist_posttag_mc[i_p]->Integral();
      //std::cout<<"flavour_norm: "<<flavour_norms_posttag[i_p]<<std::endl;
        
      for(int i_bin=1; i_bin<=hist_posttag_mc[i_p]->GetNbinsX(); i_bin++){
	help_norms_posttag[i_bin-1]+=hist_posttag_mc[i_p]->GetBinContent(i_bin);
      }
      delete hist_posttag_mc[i_p];
    }//fpairs

    TVectorD v(flavour_norms.size(),&flavour_norms[0]);
    TVectorD v_posttag(flavour_norms_posttag.size(),&flavour_norms_posttag[0]);
    TVectorD v_untagged=v-v_posttag;

    TMatrixD mat(flavour_norms.size(),flavour_norms.size(),&m_nom_cov_mats[region][0]);
 
    fit_total_error_slice[region]=(mat*v)*v;
    fit_total_error_slice_untagged[region]=(mat*v_untagged)*v_untagged;
    //fit_total_error_slice_combined[region]=(1./norm_region_total[region]-norm_region_posttag[region]/(norm_region_total[region]*norm_region_total[region]))*(1./norm_region_total[region]-norm_region_posttag[region]/(norm_region_total[region]*norm_region_total[region]))* fit_total_error_slice_untagged[region]*fit_total_error_slice_untagged[region]+(-norm_region_posttag[region]/(norm_region_total[region]*norm_region_total[region]))*(-norm_region_posttag[region]/(norm_region_total[region]*norm_region_total[region]))* fit_total_error_slice_untagged[region]*fit_total_error_slice_untagged[region];
    //std::cout<<"fit total error slice: "<<fit_total_error_slice[region]<<std::endl;
      
    region_norms[region]=help_norms;
    region_norms_posttag[region]=help_norms_posttag;
  }

  int n_bins=fj_bins.size()-1;

  double* fiterrors_up = new double[n_bins];
  double* fiterrors_down = new double[n_bins];
  double* tot_err_sq = new double[n_bins];
  double* x_values = new double[n_bins];
  double* y_values = new double[n_bins];
  double* x_error_up = new double[n_bins];
  double* x_error_down = new double[n_bins];
  double* tot_err_sq_untagged = new double[n_bins];
  double* tot_err_sq_combined = new double[n_bins];

  //loop over bins, check fraction contributed by fit region in each bin, calculate error
  for(int i_bin=1; i_bin<=n_bins; i_bin++){
    tot_err_sq[i_bin-1]=0;
    tot_err_sq_untagged[i_bin-1]=0;
    tot_err_sq_combined[i_bin-1]=0;

    for(TString region : regions){

      if(hist_total->GetBinContent(i_bin)) tot_err_sq[i_bin-1]+=fit_total_error_slice[region]*region_norms[region][i_bin-1]/hist_total->GetBinContent(i_bin);
      if((hist_total->GetBinContent(i_bin)-hist_total_posttag->GetBinContent(i_bin))>0.) tot_err_sq_untagged[i_bin-1]+=fit_total_error_slice_untagged[region]*(region_norms[region][i_bin-1]-region_norms[region][i_bin-1])/(hist_total->GetBinContent(i_bin)-hist_total_posttag->GetBinContent(i_bin));
 
      float N_tagged=hist_total_posttag->GetBinContent(i_bin);
      float N_total=hist_total->GetBinContent(i_bin);

      tot_err_sq_combined[i_bin-1]+=(1./N_total-N_tagged/(N_total*N_total))*(1./N_total-N_tagged/(N_total*N_total))*tot_err_sq[i_bin-1]+(-N_tagged/(N_total*N_total))*(-N_tagged/(N_total*N_total))*tot_err_sq_untagged[i_bin-1];

      //std::cout<<"fit_total_error_slice"<<fit_total_error_slice[region]<<std::endl;
      //std::cout<<"region_norms"<<region_norms[region][i_bin-1]<<std::endl;
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

TGraphAsymmErrors* ScaleFactorCalculator::getFitUncertBTagRateToys(){

  TString var="fjpt";

  //Calculate fit errors from covariance matrix for each flavour pair and each fit region, add errors in fit regions in qudrature.
  
  std::vector<TH1D*> hist_pretag_mc, hist_posttag_mc;
  std::vector<TH1D*> toy_bins;
  TString name_pretag, name_posttag;
 
  std::vector<double> fj_bins=m_config->GetBinning(var);
  std::vector<TString> regions = m_doFitInFatJetPtBins ? m_config->GetFatJetRegions() : m_config->GetTrkJetRegions();
  
  for(unsigned int i_b=0; i_b<fj_bins.size(); i_b++){
    toy_bins.push_back(new TH1D("toy_bins","",50,0,-1));
    toy_bins[i_b]->SetDirectory(0);
  }

  TH1D *hist_total=new TH1D("hist_total","",fj_bins.size()-1,&(fj_bins[0]));
  TH1D *hist_total_posttag=new TH1D("hist_total_posttag","",fj_bins.size()-1,&(fj_bins[0]));

  
  std::vector<double> nominal_rate(fj_bins.size()-1,0.);

  TRandom3 r(0);
  TVectorD randomv(m_config->GetFlavourPairs().size()), nomv(m_config->GetFlavourPairs().size()), eigenval(m_config->GetFlavourPairs().size());
  TMatrixDSym mat(m_config->GetFlavourPairs().size());
  TMatrixD eigenvec(m_config->GetFlavourPairs().size(),m_config->GetFlavourPairs().size());

  std::vector<double> flavour_norms(m_config->GetFlavourPairs().size(),0.);
  std::vector<double> flavour_norms_posttag(m_config->GetFlavourPairs().size(),0.);

  int n_toys=2000;
  for( int i_toy=0; i_toy<n_toys; i_toy++){
    hist_total->Reset();
    hist_total_posttag->Reset();
    
    if(!(i_toy%50)) std::cout<<"Throw toys"<<i_toy<<" of "<<n_toys<<std::endl;
    for (TString region : regions) {
      
      mat.SetMatrixArray(&m_nom_cov_mats[region][0]);

      TMatrixDSymEigen mat_eigen(mat);
      eigenval=mat_eigen.GetEigenValues();
      eigenvec=mat_eigen.GetEigenVectors();

      /*std::cout<<std::endl;
      std::cout<<"===================="<<std::endl;
      std::cout<<"Printing matrices for region: "<<i_reg<<std::endl;
      std::cout<<"===================="<<std::endl;

      std::cout<<"===================="<<std::endl;
      std::cout<<"1) Covariance Matrix"<<std::endl;
      std::cout<<"===================="<<std::endl;
      mat.Print();

      std::cout<<"===================="<<std::endl;
      std::cout<<"2) Eigenvalues"<<std::endl;
      std::cout<<"===================="<<std::endl;
      eigenval.Print();
 
      std::cout<<"===================="<<std::endl;
      std::cout<<"3) Eigenvectors"<<std::endl;
      std::cout<<"===================="<<std::endl;
      eigenvec.Print();
      */

      if(i_toy==0){ //nominal rate
	for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++) nomv(i_p)=m_fit_params[region+"_Nom"][i_p];
	randomv=nomv;
      }else{
	for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){
	  randomv(i_p)=r.Gaus(0.,TMath::Sqrt(eigenval(i_p)));
	  nomv(i_p)=m_fit_params[region+"_Nom"][i_p];
	}

	randomv=eigenvec*randomv;
	randomv+=nomv;
      }
 
      //randomv.Print();
 
      std::vector<TH1D*> help_vec = GetRebinHistsByRegionMC(var,"Nom",region);
      for(unsigned int i_p=0; i_p < help_vec.size(); i_p++) {
	help_vec[i_p]->Scale(randomv(i_p));
	hist_total->Add(help_vec[i_p]);
        delete help_vec[i_p];
      }//fpairs

      help_vec = GetRebinHistsByRegionMC(var+"_PREFITPOSTTAG","Nom",region);
      for(unsigned int i_p=0; i_p < help_vec.size(); i_p++) {
	help_vec[i_p]->Scale(randomv(i_p));
	hist_total_posttag->Add(help_vec[i_p]);
        delete help_vec[i_p];
      }//fpairs
    }
    
    for(int i_bin=1; i_bin<=hist_total_posttag->GetNbinsX(); i_bin++){
      if(i_toy==0) nominal_rate[i_bin-1]=hist_total_posttag->GetBinContent(i_bin)/hist_total->GetBinContent(i_bin);
      else toy_bins[i_bin-1]->Fill(hist_total_posttag->GetBinContent(i_bin)/hist_total->GetBinContent(i_bin)/nominal_rate[i_bin-1]);
      //std::cout<<"Rate ratio is:"<<hist_total_posttag->GetBinContent(i_bin)/hist_total->GetBinContent(i_bin)/nominal_rate[i_bin-1]<<std::endl;
    }

  }  

  int n_bins=hist_total->GetNbinsX();
  
  //double* btagerrors_up = new double[n_bins];
  //double* btagerrors_down = new double[n_bins];
  double* x_values = new double[n_bins];
  double* y_values = new double[n_bins];
  double* x_error_up = new double[n_bins];
  double* x_error_down = new double[n_bins];
  double* y_error_up = new double[n_bins];
  double* y_error_down = new double[n_bins];

  double param(0.);

  TCanvas* can_toys=new TCanvas("can_toys","",700,600);
  for(int i_bin=1; i_bin<=n_bins; i_bin++){
    TString name=m_outdir+"/ctrl_plots/toys/rate_toy_bin_";
    name+=i_bin;
    name+=".pdf";
    if(toy_bins[i_bin-1]->Integral()){
      toy_bins[i_bin-1]->Fit("gaus");
      can_toys->cd();
      toy_bins[i_bin-1]->Draw("HIST");
      TF1* fitfunc= toy_bins[i_bin-1]->GetFunction("gaus");
      fitfunc->SetLineColor(kRed);
      fitfunc->SetLineWidth(2);
      fitfunc->Draw("same");
      param=fitfunc->GetParameter(2);
      can_toys->SaveAs(name.Data());
    }
    else param=0;

    delete toy_bins[i_bin-1];

    x_values[i_bin-1]=hist_total->GetBinCenter(i_bin);
    x_error_up[i_bin-1]=hist_total->GetBinWidth(i_bin)/2;
    x_error_down[i_bin-1]=hist_total->GetBinWidth(i_bin)/2;
    y_values[i_bin-1]=1.;

    y_error_up[i_bin-1]=param/2.;
    y_error_down[i_bin-1]=param/2.;
  }

  TGraphAsymmErrors *g_fiterrors= new TGraphAsymmErrors(n_bins,x_values,y_values,x_error_down,x_error_up,y_error_down,y_error_up);
  return g_fiterrors;

}

TGraphAsymmErrors* ScaleFactorCalculator::getBTagUncert(TString& var, bool applyFitCorrection){

  //Get B-tagging systematics (up or down already defined as going in positive/negative direction)
  std::vector<double> fj_bins=m_config->GetBinning(var);

  TH1D *h_nom=new TH1D("h_nom","",fj_bins.size()-1,&(fj_bins[0]));
  TH1D *h_up=new TH1D("h_up","",fj_bins.size()-1,&(fj_bins[0]));
  TH1D *h_down=new TH1D("h_down","",fj_bins.size()-1,&(fj_bins[0]));

  std::vector<TH1D*> help_vec;
  std::vector<std::pair<TH1*,TH1*> > h_systematics;

  help_vec = GetRebinHistsMC(var,"Nom",applyFitCorrection);
  for (unsigned int i_p=0; i_p < help_vec.size(); i_p++) {
    h_nom->Add(help_vec[i_p]);
    delete help_vec[i_p];
  }

  help_vec = GetRebinHistsMC(var+"_BTAGUP","Nom",applyFitCorrection);
  for (unsigned int i_p=0; i_p < help_vec.size(); i_p++) {
    h_up->Add(help_vec[i_p]);
    delete help_vec[i_p];
  }

  help_vec = GetRebinHistsMC(var+"_BTAGDOWN","Nom",applyFitCorrection);
  for (unsigned int i_p=0; i_p < help_vec.size(); i_p++) {
    h_down->Add(help_vec[i_p]);
    delete help_vec[i_p];
  }

  h_systematics.push_back( std::make_pair(h_up,h_down) );
  
  return HistStackToTGraph(h_nom, h_systematics);  

}

TGraphAsymmErrors* ScaleFactorCalculator::getExperimentalUncert(TString &var, std::vector<TString> &systematics, bool applyFitCorrection, bool isEff){

  std::vector<double> fj_bins=m_config->GetBinning(var);
  
  std::vector<TH1D*> help_vec;

  TH1D *h_nom=new TH1D("h_nom","",fj_bins.size()-1,&(fj_bins[0]));
  TH1D *h_posttag=new TH1D("h_posttag","",fj_bins.size()-1,&(fj_bins[0]));
  std::vector<std::pair<TH1*,TH1*> > h_systematics;

  help_vec = GetRebinHistsMC(var,"Nom",applyFitCorrection);
  for (unsigned int i_p=0; i_p < help_vec.size(); i_p++) {
    h_nom->Add(help_vec[i_p]);
    delete help_vec[i_p];
  }
  
  if (isEff) {
    help_vec = GetRebinHistsMC(var+"_PREFITPOSTTAG","Nom",applyFitCorrection);
    for (unsigned int i_p=0; i_p < help_vec.size(); i_p++) {
      h_posttag->Add(help_vec[i_p]);
      delete help_vec[i_p];
    }
    h_nom->Divide(h_posttag,h_nom);
    h_posttag->Reset();
  }

  for (TString sys : systematics){
    bool isOneSided=false;

    TH1D *h_up=new TH1D("h_up","",fj_bins.size()-1,&(fj_bins[0]));
    TH1D *h_down=new TH1D("h_down","",fj_bins.size()-1,&(fj_bins[0]));

    help_vec = GetRebinHistsMC(var,sys+"__1up",applyFitCorrection);
    for (unsigned int i_p=0; i_p < help_vec.size(); i_p++) {
      h_up->Add(help_vec[i_p]);
      delete help_vec[i_p];
    }
  
    if (isEff) {
      help_vec = GetRebinHistsMC(var+"_PREFITPOSTTAG",sys+"__1up",applyFitCorrection);
      for (unsigned int i_p=0; i_p < help_vec.size(); i_p++) {
        h_posttag->Add(help_vec[i_p]);
        delete help_vec[i_p];
      }
      h_up->Divide(h_posttag,h_up);
      h_posttag->Reset();
    }

    help_vec = GetRebinHistsMC(var,sys+"__1down",applyFitCorrection);
    for (unsigned int i_p=0; i_p < help_vec.size(); i_p++) {
      if (help_vec[i_p]->Integral() == 0) {
        isOneSided = true;
        break;
      }
      h_down->Add(help_vec[i_p]);
      delete help_vec[i_p];
    }
    if (isOneSided) {
      delete h_down;
      h_down = nullptr;
    } else if (isEff) {
      help_vec = GetRebinHistsMC(var+"_PREFITPOSTTAG",sys+"__1down",applyFitCorrection);
      for (unsigned int i_p=0; i_p < help_vec.size(); i_p++) {
        h_posttag->Add(help_vec[i_p]);
        delete help_vec[i_p];
      }
      h_down->Divide(h_posttag,h_down);
      h_posttag->Reset();
    }

    h_systematics.push_back(std::make_pair(h_up,h_down));
  }

  delete h_posttag;

  return HistStackToTGraph(h_nom, h_systematics);  
}

TGraphAsymmErrors* ScaleFactorCalculator::HistStackToTGraph(const TH1* h_nom, const std::vector<std::pair<TH1*,TH1*> > systematics) {
  int n_bins = h_nom->GetNbinsX();

  double* x_values = new double[n_bins];
  double* y_values = new double[n_bins];
  double* x_error_up = new double[n_bins];
  double* x_error_down = new double[n_bins];
  double* y_error_up = new double[n_bins];
  double* y_error_down = new double[n_bins];
  
  double* errsquare_up = new double[n_bins];
  double* errsquare_down = new double[n_bins];
  for(int i=1; i<=n_bins; i++){
    errsquare_down[i-1]=0;
    errsquare_up[i-1]=0;
  }

  bool isOneSided = false;
  float delta_up(0.), delta_down(0.);
  // Pairs are __1up and __1down variation for same systematic
  for (std::pair<TH1*, TH1*> sys : systematics) {
    if (sys.second == nullptr) isOneSided = true;

    for (int i=1; i<=n_bins; i++) { 
      delta_up = sys.first->GetBinContent(i) - h_nom->GetBinContent(i);
      if (isOneSided) {
        //Symmetrize Errors
        errsquare_up[i-1]+=delta_up*delta_up;
        errsquare_down[i-1]+=delta_up*delta_up;
      } else {
        delta_down = sys.second->GetBinContent(i) - h_nom->GetBinContent(i);

        if (delta_up*delta_down < 0) {
          if (delta_up > 0) {
            errsquare_up[i-1]+=delta_up*delta_up;
            errsquare_down[i-1]+=delta_down*delta_down;
          } else {
            errsquare_down[i-1]+=delta_up*delta_up;
            errsquare_up[i-1]+=delta_down*delta_down;
          }
        } else {
          if(TMath::Abs(delta_up) > TMath::Abs(delta_down)) errsquare_up[i-1]+=delta_up*delta_up;
          else errsquare_down[i-1]+=delta_down*delta_down;
        }
      }
    } // end loop over bins
  } // end loop over systematics

  for (int i=1; i<=n_bins; i++) { 
    x_values[i-1]=h_nom->GetBinCenter(i);
    x_error_up[i-1]=h_nom->GetBinWidth(i)/2;
    x_error_down[i-1]=h_nom->GetBinWidth(i)/2;
    y_values[i-1]=1.;

    if(h_nom->GetBinContent(i)) y_error_up[i-1]=TMath::Sqrt(errsquare_up[i-1])/h_nom->GetBinContent(i);
    else y_error_up[i-1]=0;
 
    if(h_nom->GetBinContent(i)) y_error_down[i-1]=TMath::Sqrt(errsquare_down[i-1])/h_nom->GetBinContent(i);
    else y_error_down[i-1]=0;
  }

  return new TGraphAsymmErrors(n_bins,x_values,y_values,x_error_down,x_error_up,y_error_down,y_error_up);
}

//TODO: fix
std::vector<TGraphAsymmErrors*> ScaleFactorCalculator::getExperimentalUncertSeparate(TString &var, std::vector<TString> &systematics, bool applyFitCorrection, bool isPosttag, bool isEff){
 
  std::vector<double> fj_bins=m_config->GetBinning(var);
  std::vector<TString> regions = m_doFitInFatJetPtBins ? m_config->GetFatJetRegions() : m_config->GetTrkJetRegions();
  
  std::vector<int> color={kBlue+1, kViolet, kRed, kGreen+2,kCyan+1, kOrange+7, kViolet+2, kRed-6, kAzure+7, kGray+2};

  std::vector<TGraphAsymmErrors*> uncertainty_graphs;

  TH1D *h_nom=new TH1D("h_nom","",fj_bins.size()-1,&(fj_bins[0]));
  
  //for efficiency plot
  TH1D *h_nom_eff_pretag=new TH1D("h_nom_eff_pretag","",fj_bins.size()-1,&(fj_bins[0]));
  
  TH1D* help, *help_rebinned;
  
  double* d_fj_bins = new double[fj_bins.size()];
  
  for(unsigned int i_b=0; i_b<fj_bins.size(); i_b++) d_fj_bins[i_b]=(double)fj_bins[i_b];
  
  
  int n_bins=fj_bins.size()-1;
  
  double* total_errors_up = new double[n_bins];
  double* total_errors_down = new double[n_bins];
  double* total_errsquare_up = new double[n_bins];
  double* total_errsquare_down = new double[n_bins];
  double* x_values = new double[n_bins];
  double* y_values_up = new double[n_bins];
  double* y_values_down = new double[n_bins];
  double* x_error_up = new double[n_bins];
  double* x_error_down = new double[n_bins];
  
  for(int i=1; i<=n_bins; i++){
    total_errsquare_down[i-1]=0;
    total_errsquare_up[i-1]=0;
    total_errors_up[i-1]=0;
    total_errors_down[i-1]=0;
    
  }
  
  for(unsigned int i_sys=0; i_sys<systematics.size(); i_sys++){
    bool isOneSided=false;

    TH1D *h_up=new TH1D("h_up","",fj_bins.size()-1,&(fj_bins[0]));
    TH1D *h_down=new TH1D("h_down","",fj_bins.size()-1,&(fj_bins[0]));

    //for efficiency plot
    TH1D *h_up_eff_pretag=new TH1D("h_up_eff_pretag","",fj_bins.size()-1,&(fj_bins[0]));
    TH1D *h_down_eff_pretag=new TH1D("h_down_eff_pretag","",fj_bins.size()-1,&(fj_bins[0]));

    for(unsigned int i_reg=0; i_reg<regions.size(); i_reg++){
      
      //if(doPrintByRegion && !(regions[i_reg].EqualTo(region))) continue;
      
      TString name_mc=regions[i_reg]+"_"+var+"_"+"Nom";
      TString name_mc_up=regions[i_reg]+"_"+var+"_"+systematics[i_sys]+"__1up";
      TString name_mc_down=regions[i_reg]+"_"+var+"_"+systematics[i_sys]+"__1down";
      //std::cout<<"name_mc: "<<name_mc<<std::endl;
      //std::cout<<"name_mc sys up: "<<name_mc_up<<std::endl;
      //std::cout<<"name_mc sys down: "<<name_mc_down<<std::endl;
      
      
      for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){

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
	    
	      for( int i=1; i<=n_bins; i++){
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
      
      for( int i=1; i<=n_bins; i++){
        
        float delta_up=h_up->GetBinContent(i)-h_nom->GetBinContent(i);
	if(h_nom->GetBinContent(i)) y_values_up[i-1]=1.+delta_up/h_nom->GetBinContent(i);
	
        //get symmetrized down variation
        if(h_nom->GetBinContent(i)) y_values_down[i-1]=1.-delta_up/h_nom->GetBinContent(i);
      }

      uncertainty_graphs.push_back(new TGraphAsymmErrors(n_bins,x_values,y_values_up,x_error_down,x_error_up,total_errors_down,total_errors_up));
      uncertainty_graphs[uncertainty_graphs.size()-1]->SetLineColor(color[i_sys]);
      uncertainty_graphs[uncertainty_graphs.size()-1]->SetMarkerStyle(1);

      //make symmetrized down variation graph
      uncertainty_graphs.push_back(new TGraphAsymmErrors(n_bins,x_values,y_values_down,x_error_down,x_error_up,total_errors_down,total_errors_up));
      uncertainty_graphs[uncertainty_graphs.size()-1]->SetLineColor(color[i_sys]);
      uncertainty_graphs[uncertainty_graphs.size()-1]->SetLineStyle(2);
      uncertainty_graphs[uncertainty_graphs.size()-1]->SetMarkerStyle(1);
    }else{
      
      for( int i=1; i<=n_bins; i++){
        
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

TGraphAsymmErrors* ScaleFactorCalculator::getModellingUncert(TString &var, std::vector<TString> &systematics, bool applyFitCorrection, bool isPosttag, bool isEff){

  std::vector<double> fj_bins = m_config->GetBinning(var);
  std::vector<TString> regions = m_doFitInFatJetPtBins ? m_config->GetFatJetRegions() : m_config->GetTrkJetRegions();
  std::vector<TString> flavours = m_config->GetFlavourPairs();

  isPosttag = var.Contains("PREFITPOSTTAG");

  TH1D *h_nom=new TH1D("h_nom","",fj_bins.size()-1,&(fj_bins[0]));

  //for efficiency plot
  TH1D *h_nom_pretag=new TH1D("h_nom_pretag","",fj_bins.size()-1,&(fj_bins[0]));
  TH1D *h_nom_posttag=new TH1D("h_nom_posttag","",fj_bins.size()-1,&(fj_bins[0]));

  std::vector<TH1D*> help_vec;

  int n_bins=fj_bins.size()-1;

  double* total_errors_up = new double[n_bins];
  double* total_errors_down = new double[n_bins];
  double* total_errsquare_up = new double[n_bins];
  double* total_errsquare_down = new double[n_bins];
  double* x_values = new double[n_bins];
  double* y_values = new double[n_bins];
  double* x_error_up = new double[n_bins];
  double* x_error_down = new double[n_bins];
  double* total_errors_up_smooth = new double[n_bins];
  double* total_errors_down_smooth = new double[n_bins];

  for( int i=1; i<=n_bins; i++){
    total_errsquare_down[i-1]=0;
    total_errsquare_up[i-1]=0;
  }

  if (!isPosttag || isEff) {
    help_vec = GetRebinHistsMC(var,"Nom",applyFitCorrection);
    for (unsigned int i_p=0; i_p < help_vec.size(); i_p++) {
      h_nom_pretag->Add(help_vec[i_p]);
      delete help_vec[i_p];
    }
  }
  if (isPosttag || isEff) {
    help_vec = GetRebinHistsMC(var,"Nom",applyFitCorrection);
    for (unsigned int i_p=0; i_p < help_vec.size(); i_p++) {
      h_nom_posttag->Add(help_vec[i_p]);
      delete help_vec[i_p];
    }
  }

  h_nom = isPosttag ? h_nom_posttag : h_nom_pretag;
  double pythia_nominal_fraction = h_nom->Integral();
  if(isEff) h_nom->Divide(h_nom_posttag,h_nom_pretag);

  for (TString sys : systematics){
    bool isOneSided=false;

    TH1D *h_up=new TH1D("h_up","",fj_bins.size()-1,&(fj_bins[0]));

    //for efficiency plot
    TH1D *h_up_pretag=new TH1D("h_up_pretag","",fj_bins.size()-1,&(fj_bins[0]));
    TH1D *h_up_posttag=new TH1D("h_up_posttag","",fj_bins.size()-1,&(fj_bins[0]));

    for (TString region : regions) {
      if (!isPosttag || isEff) {
        help_vec = GetRebinHistsByRegionMC(var,sys+"__1up",region,applyFitCorrection);
        for (unsigned int i_p=0; i_p < help_vec.size(); i_p++) {
          //std::cout<<"Herwig correction factor"<<pythia_nominal_fraction/help_rebinned->Integral()<<std::endl;
          //if(applyFitCorrection && help_rebinned->Integral()) help_rebinned->Scale(pythia_nominal_fraction/help_rebinned->Integral());
          //scale to Herwig FF
          help_vec[i_p]->Scale(GetFitScale("Herwig__1up",region,flavours[i_p]));
          h_up_pretag->Add(help_vec[i_p]);
          delete help_vec[i_p];
        }
      }

      if (isPosttag || isEff) {
        help_vec = GetRebinHistsByRegionMC(var,sys+"__1up",region,applyFitCorrection);
        for (unsigned int i_p=0; i_p < help_vec.size(); i_p++) {
          //std::cout<<"Herwig correction factor"<<pythia_nominal_fraction/help_rebinned->Integral()<<std::endl;
          //if(applyFitCorrection && help_rebinned->Integral()) help_rebinned->Scale(pythia_nominal_fraction/help_rebinned->Integral());
          //scale to Herwig FF
          help_vec[i_p]->Scale(GetFitScale("Herwig__1up",region,flavours[i_p]));
          h_up_posttag->Add(help_vec[i_p]);
          delete help_vec[i_p];
        }
      }
    }

    h_up = isPosttag ? h_up_posttag : h_up_pretag;

    if(isEff){
      h_up->Divide(h_up_posttag,h_up_pretag);
    }

    for( int i=1; i<=n_bins; i++){
 
      float delta_up=h_up->GetBinContent(i)-h_nom->GetBinContent(i);
      std::cout<<"delta up one-sided: "<< sys<<" : "<<delta_up<<std::endl;
      std::cout<<"Bin content nominal:"<<h_nom->GetBinContent(i)<<std::endl;
      std::cout<<"Bin content up:"<<h_up->GetBinContent(i)<<std::endl;

      //if(delta_up>0) total_errsquare_up[i-1]+=delta_up*delta_up;
      //else total_errsquare_down[i-1]+=delta_up*delta_up;

      //Symmetrize Errors
      total_errsquare_up[i-1]+=delta_up*delta_up;
      total_errsquare_down[i-1]+=delta_up*delta_up;
    }
  }

  for( int i=1; i<=n_bins; i++){
    x_values[i-1]=h_nom->GetBinCenter(i);
    x_error_up[i-1]=h_nom->GetBinWidth(i)/2;
    x_error_down[i-1]=h_nom->GetBinWidth(i)/2;
    y_values[i-1]=1.;

    if(h_nom->GetBinContent(i)) total_errors_up[i-1]=TMath::Sqrt(total_errsquare_up[i-1])/h_nom->GetBinContent(i);
    else total_errors_up[i-1]=0;
    
    if(h_nom->GetBinContent(i)) total_errors_down[i-1]=TMath::Sqrt(total_errsquare_down[i-1])/h_nom->GetBinContent(i);
    else total_errors_down[i-1]=0;
    
    if(isEff || 1){
      std::cout<<"h_nom->GetBinContent(i): "<<h_nom->GetBinContent(i)<<std::endl;
      std::cout<<"Exp error up: "<<total_errors_up[i-1]<<std::endl;
      std::cout<<"Exp error down: "<<total_errors_down[i-1]<<std::endl;
    }
  }

  //nearest neighbor smoothing
  for( int i=1; i<=n_bins; i++){
    if(i==1){
      total_errors_up_smooth[i-1]=(2*total_errors_up[i-1]+total_errors_up[i])/3.;
      total_errors_down_smooth[i-1]=(2*total_errors_down[i-1]+total_errors_down[i])/3.;
    }else if(i==n_bins){
      total_errors_up_smooth[i-1]=(2*total_errors_up[i-1]+total_errors_up[i-2])/3.;
      total_errors_down_smooth[i-1]=(2*total_errors_down[i-1]+total_errors_down[i-2])/3.;

    }else{
      total_errors_up_smooth[i-1]=(2*total_errors_up[i-1]+total_errors_up[i-2]+total_errors_up[i])/4.;
      total_errors_down_smooth[i-1]=(2*total_errors_down[i-1]+total_errors_up[i-2]+total_errors_down[i])/3.;
    }
  }

  TGraphAsymmErrors *g_modelling= new TGraphAsymmErrors(n_bins,x_values,y_values,x_error_down,x_error_up,total_errors_down_smooth,total_errors_up_smooth);
  return g_modelling;

}

TGraphAsymmErrors* ScaleFactorCalculator::getTotalSys(TGraphAsymmErrors* fitsysgraph, TGraphAsymmErrors* btagsysgraph, TGraphAsymmErrors* jetsysgraph, TGraphAsymmErrors* stat, TGraphAsymmErrors* model_sys){

  int n_points=stat->GetN();

  double* err_squared_tot_up = new double[n_points];
  double* err_squared_tot_down = new double[n_points];
  double* err_tot_up = new double[n_points];
  double* err_tot_down = new double[n_points];

  for( int i=0; i<n_points; i++){

    err_squared_tot_up[i]=0.;
    err_squared_tot_down[i]=0.;

    if(fitsysgraph) err_squared_tot_up[i]+=fitsysgraph->GetErrorYhigh(i)*fitsysgraph->GetErrorYhigh(i);
    if(btagsysgraph) err_squared_tot_up[i]+=btagsysgraph->GetErrorYhigh(i)*btagsysgraph->GetErrorYhigh(i);
    if(jetsysgraph) err_squared_tot_up[i]+=jetsysgraph->GetErrorYhigh(i)*jetsysgraph->GetErrorYhigh(i);
    if(stat) err_squared_tot_up[i]+=stat->GetErrorYhigh(i)*stat->GetErrorYhigh(i);
    if(model_sys) err_squared_tot_up[i]+=model_sys->GetErrorYhigh(i)*model_sys->GetErrorYhigh(i);

    if(fitsysgraph) err_squared_tot_down[i]+=fitsysgraph->GetErrorYlow(i)*fitsysgraph->GetErrorYlow(i);
    if(btagsysgraph) err_squared_tot_down[i]+=btagsysgraph->GetErrorYlow(i)*btagsysgraph->GetErrorYlow(i);
    if(jetsysgraph) err_squared_tot_down[i]+=jetsysgraph->GetErrorYlow(i)*jetsysgraph->GetErrorYlow(i);
    if(stat) err_squared_tot_down[i]+=stat->GetErrorYlow(i)*stat->GetErrorYlow(i);
    if(model_sys) err_squared_tot_down[i]+=model_sys->GetErrorYlow(i)*model_sys->GetErrorYlow(i);

    err_tot_up[i]=TMath::Sqrt(err_squared_tot_up[i]);
    err_tot_down[i]=TMath::Sqrt(err_squared_tot_down[i]);

  }

  TGraphAsymmErrors *g_totalerrors= new TGraphAsymmErrors(n_points,jetsysgraph->GetX(),jetsysgraph->GetY(),jetsysgraph->GetEXlow(),jetsysgraph->GetEXhigh(),err_tot_down,err_tot_up);
  return g_totalerrors;

} 

TGraphAsymmErrors* ScaleFactorCalculator::getMCStat(TH1* full_mc){
  
  int nbins=full_mc->GetNbinsX();
  
  double* x_values = new double[nbins];
  double* y_values = new double[nbins];
  double* x_error_up = new double[nbins];
  double* x_error_down = new double[nbins];
  double* y_error_up = new double[nbins];
  double* y_error_down = new double[nbins];
  
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
