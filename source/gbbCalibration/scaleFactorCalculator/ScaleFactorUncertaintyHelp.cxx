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

TGraphAsymmErrors* ScaleFactorCalculator::getTemplateFitUncert(bool applyFitCorrection, std::vector<std::shared_ptr<TH1D>> templateHists, TString& region, TString &sys, int rebin){

  TH1D* tmp_stacked_mc(nullptr), *hist_total(nullptr);
  std::vector<float> flavour_norms;
 
  for(unsigned int i_p=0; i_p<templateHists.size(); i_p++){

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
  for(unsigned int i_entry=0; i_entry<m_nom_cov_mats[region].size(); i_entry++){
    
    //std::cout<<"size of matrix"<<m_nom_cov_mats[regions[i_reg]].size()<<std::endl;
    matrix.push_back((float)(m_nom_cov_mats[region][i_entry])); //get region covariance matrix
    //std::cout<<"matrix element: "<<m_nom_cov_mats[regions[i_reg]][i_entry]<<std::endl;
    
  }
  //for(int j=0; j<matrix.size(); j++) std::cout<<"Matrix "<<j<<" "<<matrix[j]<<std::endl;
  
  TMatrix mat(flavour_norms.size(),flavour_norms.size(),&matrix[0]);
  
  v2*=mat;
  
  float fit_total_error_slice=v2*v;

  
  int n_bins=hist_total->GetNbinsX();
  
  double* fiterrors_up = new double[n_bins];
  double* fiterrors_down = new double[n_bins];
  double* tot_err_sq = new double[n_bins];
  double* x_values = new double[n_bins];
  double* y_values = new double[n_bins];
  double* x_error_up = new double[n_bins];
  double* x_error_down = new double[n_bins];
  
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

  TH1D* tmp_stacked_mc, *hist_total;

  std::vector<TH1D*> toy_bins;

  TRandom3 r(0);

  TVectorD randomv(m_config->GetFlavourPairs().size()), nomv(m_config->GetFlavourPairs().size()), eigenval(m_config->GetFlavourPairs().size());

  TMatrixD eigenvec(m_config->GetFlavourPairs().size(),m_config->GetFlavourPairs().size());

  std::vector<double> matrix;

  std::vector<double> nominal;


  for(unsigned int i_entry=0; i_entry<m_nom_cov_mats[region].size(); i_entry++){
    
    matrix.push_back((double)(m_nom_cov_mats[region][i_entry])); //get region covariance matrix
    
  }

  TMatrixDSym mat(m_config->GetFlavourPairs().size(),&matrix[0]);
  //mat.UnitMatrix();
  
  TMatrixDSymEigen mat_eigen(mat);
  eigenval=mat_eigen.GetEigenValues();
  eigenvec=mat_eigen.GetEigenVectors();

  int n_toys=2000;
  
  for( int i_toy=0; i_toy<n_toys; i_toy++){

    if(i_toy==0){ //nominal rate
      for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++) nomv(i_p)=m_fit_params[region+"_"+sys][i_p];
      randomv=nomv;
    }else{
      for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){
	randomv(i_p)=r.Gaus(0.,TMath::Sqrt(eigenval(i_p)));
	nomv(i_p)=m_fit_params[region+"_"+sys][i_p];
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
	toy_bins.push_back(new TH1D("toy_bins","",50,0,-1));
      }
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
    TString name="./ctrl_plots/toys/Templates_toy_bin_";
    if(TString(hist_total->GetName()).Contains("posttag")) name+="posttag_";
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

TGraphAsymmErrors* ScaleFactorCalculator::getFitUncert(TString& var, bool isPosttag){
  
  //Calculate fit errors from covariance matrix for each flavour pair and each fit region, add errors in fit regions in qudrature.
  
  std::vector<TH1D*> hist_pretag_mc, hist_posttag_mc;
  TString name_pretag, name_posttag;
 
  TString var_fjpt="fjpt";
  std::vector<double> fj_bins= (var.Contains("fjphi") && var.Contains("fjeta")) ? m_config->GetBinning(var_fjpt) : m_config->GetBinning(var);
  std::vector<TString> regions = m_doFitInFatJetPtBins ? m_config->GetFatJetRegions() : m_config->GetTrkJetRegions();
  
  TH1D* help, *help_rebinned;
  
  double* d_fj_bins = new double[fj_bins.size()];
  
  for(unsigned int i_b=0; i_b<fj_bins.size(); i_b++) d_fj_bins[i_b]=(double)fj_bins[i_b];
  
  TH1D *hist_total=new TH1D("hist_total","",fj_bins.size()-1,&(fj_bins[0]));
  
  std::vector<float> fit_total_error_slice(regions.size(),0.);
  
  std::vector<double> help_norms(fj_bins.size()-1,0.);
  std::vector<std::vector<double>> region_norms(regions.size(), help_norms);
  
  for(unsigned int i_reg=0; i_reg<regions.size(); i_reg++){

    help_norms.assign(fj_bins.size()-1,0.);
    
    TString name_mc=regions[i_reg]+"_"+var+"_"+"Nom";
    //std::cout<<"name_mc: "<<name_mc<<std::endl;
    
    std::vector<float> flavour_norms(m_config->GetFlavourPairs().size(),0.);
    
    for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){
      
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
      for(unsigned int i_entry=0; i_entry<m_nom_cov_mats[regions[i_reg]].size(); i_entry++){
      
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
  
  double* fiterrors_up = new double[n_bins];
  double* fiterrors_down = new double[n_bins];
  double* tot_err_sq = new double[n_bins];
  double* x_values = new double[n_bins];
  double* y_values = new double[n_bins];
  double* x_error_up = new double[n_bins];
  double* x_error_down = new double[n_bins];
  
  for(int ib=0; ib<n_bins; ib++){
    
    tot_err_sq[ib]=0;

  }

  
  //loop over bins, check fraction contributed by fit region in each bin, calculate error
  for(int i_bin=1; i_bin<=n_bins; i_bin++){

    for(unsigned int i_reg=0; i_reg<regions.size(); i_reg++){

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

TGraphAsymmErrors* ScaleFactorCalculator::getFitUncertToys(TString& var, bool isPosttag){
  
//Calculate fit errors from covariance matrix for each flavour pair and each fit region, add errors in fit regions in qudrature.
  
  std::vector<TH1D*> hist_pretag_mc, hist_posttag_mc, toy_bins;
  TString name_pretag, name_posttag;
 
  TString var_fjpt="fjpt";
  std::vector<double> fj_bins= (var.Contains("fjphi") && var.Contains("fjeta")) ? m_config->GetBinning(var_fjpt) : m_config->GetBinning(var);
  std::vector<TString> regions = m_doFitInFatJetPtBins ? m_config->GetFatJetRegions() : m_config->GetTrkJetRegions();
  
  TH1D* help, *help_rebinned;
  
  double* d_fj_bins = new double[fj_bins.size()];
  
  for(unsigned int i_b=0; i_b<fj_bins.size(); i_b++){
    d_fj_bins[i_b]=(double)fj_bins[i_b];
    toy_bins.push_back(new TH1D("toy_bins","",50,0,-1));
    toy_bins[i_b]->SetDirectory(0);
  }

  TH1D *hist_total=new TH1D("hist_total","",fj_bins.size()-1,&(fj_bins[0]));
  
  TRandom3 r(0);

  TVectorD randomv(m_config->GetFlavourPairs().size()), nomv(m_config->GetFlavourPairs().size()), eigenval(m_config->GetFlavourPairs().size());

  TMatrixD eigenvec(m_config->GetFlavourPairs().size(),m_config->GetFlavourPairs().size());

  std::vector<double> matrix;

  std::vector<double> nominal(fj_bins.size()-1,0.);

  int n_toys=2000;

  for(int i_toy=0; i_toy<n_toys; i_toy++){
    hist_total->Reset();
    if(!(i_toy%50)) std::cout<<"Throw toys"<<i_toy<<" of "<<n_toys<<std::endl;
    
    for(unsigned int i_reg=0; i_reg<regions.size(); i_reg++){
      
      
      TString name_mc=regions[i_reg]+"_"+var+"_"+"Nom";
      //std::cout<<"name_mc: "<<name_mc<<std::endl;
      
      matrix.clear();

      for(unsigned int i_entry=0; i_entry<m_nom_cov_mats[regions[i_reg]].size(); i_entry++){
	matrix.push_back((double)(m_nom_cov_mats[regions[i_reg]][i_entry])); //get region covariance matrix
	
      }
      
      TMatrixDSym mat(m_config->GetFlavourPairs().size(),&matrix[0]);
      //mat.UnitMatrix();
      
      TMatrixDSymEigen mat_eigen(mat);
      eigenval=mat_eigen.GetEigenValues();
      eigenvec=mat_eigen.GetEigenVectors();
      
      if(i_toy==0){ //nominal rate
	for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++) nomv(i_p)=m_fit_params[regions[i_reg]+"_Nom"][i_p];
	randomv=nomv;
      }else{
	for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){
	  randomv(i_p)=r.Gaus(0.,TMath::Sqrt(eigenval(i_p)));
	  nomv(i_p)=m_fit_params[regions[i_reg]+"_Nom"][i_p];
	}
	
	randomv=eigenvec*randomv;
	randomv+=nomv;
      }
      
      
      for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){
	
	if(!isPosttag){
	  help=(TH1D*)m_fatjet_histograms_pretag[name_mc][i_p]->Clone();
	  help->SetDirectory(0);
	  help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
	  help_rebinned->SetDirectory(0);
	  help_rebinned->Scale(randomv(i_p));
	  hist_total->Add(help_rebinned);
	  
	  delete help;
	  
	}else{
	  help=(TH1D*)m_fatjet_histograms_posttag[name_mc][i_p]->Clone();
	  help->SetDirectory(0);
	  help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
	  help_rebinned->SetDirectory(0);
	  help_rebinned->Scale(randomv(i_p));
	  hist_total->Add(help_rebinned);
	  
	  delete help;
	  
	}
	
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
    TString name="./ctrl_plots/toys/"+var+"_toy_bin_";
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


TGraphAsymmErrors* ScaleFactorCalculator::getFitUncertBTagRate(){
  
  TString var="fjpt";

  //Calculate fit errors from covariance matrix for each flavour pair and each fit region, add errors in fit regions in qudrature.
  
  std::vector<TH1D*> hist_pretag_mc, hist_posttag_mc;
  TString name_pretag, name_posttag;
 
  std::vector<double> fj_bins=m_config->GetBinning(var);
  std::vector<TString> regions = m_doFitInFatJetPtBins ? m_config->GetFatJetRegions() : m_config->GetTrkJetRegions();
  
  TH1D* help, *help_rebinned;
  
  double* d_fj_bins = new double[fj_bins.size()];
  
  for(unsigned int i_b=0; i_b<fj_bins.size(); i_b++) d_fj_bins[i_b]=(double)fj_bins[i_b];
  
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

  
  for(unsigned int i_reg=0; i_reg<regions.size(); i_reg++){

    help_norms.assign(fj_bins.size()-1,0.);
    help_norms_posttag.assign(fj_bins.size()-1,0.);
    
    TString name_mc=regions[i_reg]+"_"+var+"_"+"Nom";
    TString name_mc_posttag=regions[i_reg]+"_"+var+"_PREFITPOSTTAG_"+"Nom";

    //std::cout<<"name_mc: "<<name_mc<<std::endl;
    
    std::vector<float> flavour_norms(m_config->GetFlavourPairs().size(),0.);
    std::vector<float> flavour_norms_posttag(m_config->GetFlavourPairs().size(),0.);
    
    for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){
      
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
    for(unsigned int i_entry=0; i_entry<m_nom_cov_mats[regions[i_reg]].size(); i_entry++){
      
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
  
  double* fiterrors_up = new double[n_bins];
  double* fiterrors_down = new double[n_bins];
  double* tot_err_sq = new double[n_bins];
  double* x_values = new double[n_bins];
  double* y_values = new double[n_bins];
  double* x_error_up = new double[n_bins];
  double* x_error_down = new double[n_bins];
  double* tot_err_sq_untagged = new double[n_bins];
  double* tot_err_sq_combined = new double[n_bins];
  
  for(int ib=0; ib<n_bins; ib++){
    
    tot_err_sq[ib]=0;
    tot_err_sq_untagged[ib]=0;
    tot_err_sq_combined[ib]=0;
    
  }

  
  //loop over bins, check fraction contributed by fit region in each bin, calculate error
  for(int i_bin=1; i_bin<=n_bins; i_bin++){

    for(unsigned int i_reg=0; i_reg<regions.size(); i_reg++){

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

TGraphAsymmErrors* ScaleFactorCalculator::getFitUncertBTagRateToys(){
 

  TString var="fjpt";

  //Calculate fit errors from covariance matrix for each flavour pair and each fit region, add errors in fit regions in qudrature.
  
  std::vector<TH1D*> hist_pretag_mc, hist_posttag_mc;
  std::vector<TH1D*> toy_bins;
  TString name_pretag, name_posttag;
 
  std::vector<double> fj_bins=m_config->GetBinning(var);
  std::vector<TString> regions = m_doFitInFatJetPtBins ? m_config->GetFatJetRegions() : m_config->GetTrkJetRegions();
  
  TH1D* help(nullptr), *help_rebinned(nullptr);
  
  double* d_fj_bins = new double[fj_bins.size()];
  
  for(unsigned int i_b=0; i_b<fj_bins.size(); i_b++){
    d_fj_bins[i_b]=(double)fj_bins[i_b];
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

  std::vector<double> matrix;
  std::vector<float> flavour_norms, flavour_norms_posttag;

  TString name_mc, name_mc_posttag;

  int n_toys=2000;
  
  for( int i_toy=0; i_toy<n_toys; i_toy++){
    hist_total->Reset();
    hist_total_posttag->Reset();
    
    if(!(i_toy%50)) std::cout<<"Throw toys"<<i_toy<<" of "<<n_toys<<std::endl;
    for(unsigned int i_reg=0; i_reg<regions.size(); i_reg++){
      
      name_mc=regions[i_reg]+"_"+var+"_"+"Nom";
      name_mc_posttag=regions[i_reg]+"_"+var+"_PREFITPOSTTAG_"+"Nom";
      
      //std::cout<<"name_mc: "<<name_mc<<std::endl;

      matrix.clear();

      for(unsigned int i_entry=0; i_entry<m_nom_cov_mats[regions[i_reg]].size(); i_entry++){
	
	//std::cout<<"size of matrix"<<m_nom_cov_mats[regions[i_reg]].size()<<std::endl;
	matrix.push_back((double)(m_nom_cov_mats[regions[i_reg]][i_entry])); //get region covariance matrix
	//std::cout<<"matrix element: "<<m_nom_cov_mats[regions[i_reg]][i_entry]<<std::endl;
	
      }
      //for(int j=0; j<matrix.size(); j++) std::cout<<"Matrix "<<j<<" "<<matrix[j]<<std::endl;
    
      mat.SetMatrixArray(&matrix[0]);
      
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
	for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++) nomv(i_p)=m_fit_params[regions[i_reg]+"_Nom"][i_p];
	randomv=nomv;
      }else{
	for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){
	  randomv(i_p)=r.Gaus(0.,TMath::Sqrt(eigenval(i_p)));
	  nomv(i_p)=m_fit_params[regions[i_reg]+"_Nom"][i_p];
	}
	
	randomv=eigenvec*randomv;
	randomv+=nomv;
      }
      
      //randomv.Print();
      
      for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){
	
	help=(TH1D*)m_fatjet_histograms_pretag[name_mc][i_p]->Clone();
	help->SetDirectory(0);
	help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
	help_rebinned->SetDirectory(0);
	help_rebinned->Scale(randomv[i_p]);
	hist_total->Add(help_rebinned);
  
	delete help;

	help=(TH1D*)m_fatjet_histograms_posttag[name_mc_posttag][i_p]->Clone();
	help->SetDirectory(0);
	help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned_posttag",d_fj_bins);
	help_rebinned->SetDirectory(0);
	help_rebinned->Scale(randomv[i_p]);
	hist_total_posttag->Add(help_rebinned);
	delete help;

      }  
      
      
      
    }
    
    for(int i_bin=1; i_bin<=hist_total_posttag->GetNbinsX(); i_bin++){
      if(i_toy==0) nominal_rate[i_bin-1]=hist_total_posttag->GetBinContent(i_bin)/hist_total->GetBinContent(i_bin);
      else toy_bins[i_bin-1]->Fill(hist_total_posttag->GetBinContent(i_bin)/hist_total->GetBinContent(i_bin)/nominal_rate[i_bin-1]);
      //std::cout<<"Rate ratio is:"<<hist_total_posttag->GetBinContent(i_bin)/hist_total->GetBinContent(i_bin)/nominal_rate[i_bin-1]<<std::endl;
    }
    

  }  

  int n_bins=hist_total->GetNbinsX();
  
  double* btagerrors_up = new double[n_bins];
  double* btagerrors_down = new double[n_bins];
  double* x_values = new double[n_bins];
  double* y_values = new double[n_bins];
  double* x_error_up = new double[n_bins];
  double* x_error_down = new double[n_bins];
  double* y_error_up = new double[n_bins];
  double* y_error_down = new double[n_bins];

  double param;

  TCanvas* can_toys=new TCanvas("can_toys","",700,600);
  for(int i_bin=1; i_bin<=help_rebinned->GetNbinsX(); i_bin++){
    TString name="./ctrl_plots/toys/rate_toy_bin_";
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
  std::vector<TString> regions = m_doFitInFatJetPtBins ? m_config->GetFatJetRegions() : m_config->GetTrkJetRegions();
  

  
  TH1D *h_nom=new TH1D("h_nom","",fj_bins.size()-1,&(fj_bins[0]));
  TH1D *h_up=new TH1D("h_up","",fj_bins.size()-1,&(fj_bins[0]));
  TH1D *h_down=new TH1D("h_down","",fj_bins.size()-1,&(fj_bins[0]));
  
  
  TH1D* help, *help_rebinned;
  
  double* d_fj_bins = new double[fj_bins.size()];
  
  for(unsigned int i_b=0; i_b<fj_bins.size(); i_b++) d_fj_bins[i_b]=(double)fj_bins[i_b];
  
  for(unsigned int i_reg=0; i_reg<regions.size(); i_reg++){
    
    //if(doPrintByRegion && !(regions[i_reg].EqualTo(region))) continue;
    
    TString name_mc=regions[i_reg]+"_"+var+"_"+"Nom";
    TString name_mc_up=regions[i_reg]+"_"+var+"_"+"BTAGUP_Nom";
    TString name_mc_down=regions[i_reg]+"_"+var+"_"+"BTAGDOWN_Nom";
    //std::cout<<"name_mc: "<<name_mc<<std::endl;
    //std::cout<<"name_mc btag up: "<<name_mc_up<<std::endl;
    //std::cout<<"name_mc btag down: "<<name_mc_down<<std::endl;

    
    for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){
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
  
  double* btagerrors_up = new double[n_bins];
  double* btagerrors_down = new double[n_bins];
  double* x_values = new double[n_bins];
  double* y_values = new double[n_bins];
  double* x_error_up = new double[n_bins];
  double* x_error_down = new double[n_bins];
  
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
 
  std::vector<double> fj_bins=m_config->GetBinning(var);
  std::vector<TString> regions = m_doFitInFatJetPtBins ? m_config->GetFatJetRegions() : m_config->GetTrkJetRegions();
  
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
  double* y_values = new double[n_bins];
  double* x_error_up = new double[n_bins];
  double* x_error_down = new double[n_bins];
  
  for(int i=1; i<=n_bins; i++){
    total_errsquare_down[i-1]=0;
    total_errsquare_up[i-1]=0;
    
  }
  
  for(unsigned int i_sys=0; i_sys<sys.size(); i_sys++){
    bool isOneSided=false;

    TH1D *h_up=new TH1D("h_up","",fj_bins.size()-1,&(fj_bins[0]));
    TH1D *h_down=new TH1D("h_down","",fj_bins.size()-1,&(fj_bins[0]));

    //for efficiency plot
    TH1D *h_up_eff_pretag=new TH1D("h_up_eff_pretag","",fj_bins.size()-1,&(fj_bins[0]));
    TH1D *h_down_eff_pretag=new TH1D("h_down_eff_pretag","",fj_bins.size()-1,&(fj_bins[0]));

    for(unsigned int i_reg=0; i_reg<regions.size(); i_reg++){
      
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
      
      
      for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){

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
	if(isEff || 1) std::cout<<"delta up one-sided: "<< sys[i_sys]<<" : "<<delta_up<<std::endl;

        //if(delta_up>0) total_errsquare_up[i-1]+=delta_up*delta_up;
        //else total_errsquare_down[i-1]+=delta_up*delta_up;
        
        //Symmetrize Errors
        total_errsquare_up[i-1]+=delta_up*delta_up;
        total_errsquare_down[i-1]+=delta_up*delta_up;
        
      }
      
    }else{
      
      for(int i=1; i<=n_bins; i++){
	if(isEff) std::cout<<"Nominal bin "<<i<<": "<<h_nom->GetBinContent(i)<<std::endl;

        float delta_up=h_up->GetBinContent(i)-h_nom->GetBinContent(i);
        float delta_down=h_down->GetBinContent(i)-h_nom->GetBinContent(i);
	if(isEff || 1){
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

    if(isEff || 1){
      std::cout<<"h_nom->GetBinContent(i): "<<h_nom->GetBinContent(i)<<std::endl;
      std::cout<<"Exp error up: "<<total_errors_up[i-1]<<std::endl;
      std::cout<<"Exp error down: "<<total_errors_down[i-1]<<std::endl;
    }

  }

  
  TGraphAsymmErrors *g_totaljeterrors= new TGraphAsymmErrors(n_bins,x_values,y_values,x_error_down,x_error_up,total_errors_down,total_errors_up);
  
  return g_totaljeterrors;


}


std::vector<TGraphAsymmErrors*> ScaleFactorCalculator::getExperimentalUncertSeparate(TString &var, std::vector<TString> &sys, bool applyFitCorrection, bool isPosttag, bool isEff){
 
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
  
  for(unsigned int i_sys=0; i_sys<sys.size(); i_sys++){
    bool isOneSided=false;

    TH1D *h_up=new TH1D("h_up","",fj_bins.size()-1,&(fj_bins[0]));
    TH1D *h_down=new TH1D("h_down","",fj_bins.size()-1,&(fj_bins[0]));

    //for efficiency plot
    TH1D *h_up_eff_pretag=new TH1D("h_up_eff_pretag","",fj_bins.size()-1,&(fj_bins[0]));
    TH1D *h_down_eff_pretag=new TH1D("h_down_eff_pretag","",fj_bins.size()-1,&(fj_bins[0]));

    for(unsigned int i_reg=0; i_reg<regions.size(); i_reg++){
      
      //if(doPrintByRegion && !(regions[i_reg].EqualTo(region))) continue;
      
      TString name_mc=regions[i_reg]+"_"+var+"_"+"Nom";
      TString name_mc_up=regions[i_reg]+"_"+var+"_"+sys[i_sys]+"__1up";
      TString name_mc_down=regions[i_reg]+"_"+var+"_"+sys[i_sys]+"__1down";
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



TGraphAsymmErrors* ScaleFactorCalculator::getModellingUncert(TString &var, std::vector<TString> &sys, bool applyFitCorrection, bool isPosttag, bool isEff){

  std::vector<double> fj_bins=m_config->GetBinning(var);
  std::vector<TString> regions = m_doFitInFatJetPtBins ? m_config->GetFatJetRegions() : m_config->GetTrkJetRegions();
  
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
  double* y_values = new double[n_bins];
  double* x_error_up = new double[n_bins];
  double* x_error_down = new double[n_bins];
  double* total_errors_up_smooth = new double[n_bins];
  double* total_errors_down_smooth = new double[n_bins];


  for( int i=1; i<=n_bins; i++){
    total_errsquare_down[i-1]=0;
    total_errsquare_up[i-1]=0;
    
  }
  
  for(unsigned int i_sys=0; i_sys<sys.size(); i_sys++){
    bool isOneSided=false;

    TH1D *h_up=new TH1D("h_up","",fj_bins.size()-1,&(fj_bins[0]));

    //for efficiency plot
    TH1D *h_up_eff_pretag=new TH1D("h_up_eff_pretag","",fj_bins.size()-1,&(fj_bins[0]));

    for(unsigned int i_reg=0; i_reg<regions.size(); i_reg++){
      
      //if(doPrintByRegion && !(regions[i_reg].EqualTo(region))) continue;
      
      TString name_mc=regions[i_reg]+"_"+var+"_"+"Nom";
      TString name_mc_up=regions[i_reg]+"_"+var+"_"+sys[i_sys]+"__1up";

      TString name_mc_posttag=regions[i_reg]+"_"+var+"_PREFITPOSTTAG_"+"Nom";
      TString name_mc_up_posttag=regions[i_reg]+"_"+var+"_PREFITPOSTTAG_"+sys[i_sys]+"__1up";

      //std::cout<<"name_mc: "<<name_mc<<std::endl;
      //std::cout<<"name_mc sys up: "<<name_mc_up<<std::endl;
      //std::cout<<"name_mc sys down: "<<name_mc_down<<std::endl;
      
      double pythia_nominal_fraction=0.;
      for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){

	if(isPosttag || isEff){
	  if(!isEff){
	    name_mc_posttag=name_mc;
	    name_mc_up_posttag=name_mc_up;
	  } 
	  

	  //get nominal
	  help=(TH1D*)m_fatjet_histograms_posttag[name_mc_posttag][i_p]->Clone();
	  help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
	  if(applyFitCorrection) help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
	  pythia_nominal_fraction=help_rebinned->Integral();
	  h_nom->Add(help_rebinned);
	  
	  //get up variation
	  help=(TH1D*)m_fatjet_histograms_posttag[name_mc_up_posttag][i_p]->Clone();
	  help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
	  //scale flavour fractions to Pythia corrected fractions
	  //std::cout<<"Herwig correction factor"<<pythia_nominal_fraction/help_rebinned->Integral()<<std::endl;
	  //if(applyFitCorrection && help_rebinned->Integral()) help_rebinned->Scale(pythia_nominal_fraction/help_rebinned->Integral());

	  //scale to Herwig FF
	  help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Herwig__1up"][i_p]);
	  h_up->Add(help_rebinned);

        }
	if(!isPosttag || isEff){
	   
	  //get nominal
	  help=(TH1D*)m_fatjet_histograms_pretag[name_mc][i_p]->Clone();
	  help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
	  if(applyFitCorrection) help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
	  pythia_nominal_fraction=help_rebinned->Integral();
	  if(!isEff) h_nom->Add(help_rebinned);
	  else h_nom_eff_pretag->Add(help_rebinned);
	  
	  //get up variation (Modelling is always one-sided)
	  help=(TH1D*)m_fatjet_histograms_pretag[name_mc_up][i_p]->Clone();
	  help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
	  //scale flavour fractions to Pythia corrected fractions  
	  //std::cout<<"Herwig correction factor"<<pythia_nominal_fraction/help_rebinned->Integral()<<std::endl;
	  //if(applyFitCorrection && help_rebinned->Integral()) help_rebinned->Scale(pythia_nominal_fraction/help_rebinned->Integral());

	  //scale to Herwig FF
	  help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Herwig__1up"][i_p]);

	  if(!isEff) h_up->Add(help_rebinned);
	  else h_up_eff_pretag->Add(help_rebinned);
	  
	}


      }
    }

    if(isEff){
      if(i_sys==0) h_nom->Divide(h_nom_eff_pretag);
      h_up->Divide(h_up_eff_pretag);
      
    }
    
    
      
    for( int i=1; i<=n_bins; i++){
        
      float delta_up=h_up->GetBinContent(i)-h_nom->GetBinContent(i);
      std::cout<<"delta up one-sided: "<< sys[i_sys]<<" : "<<delta_up<<std::endl;
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
    if(stat) err_squared_tot_down[i]+=model_sys->GetErrorYlow(i)*model_sys->GetErrorYlow(i);
    
    
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
