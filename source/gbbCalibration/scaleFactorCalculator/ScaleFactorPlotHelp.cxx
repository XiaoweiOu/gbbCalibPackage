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

void ScaleFactorCalculator::MakeCalibrationPlots(CalibResult cl_result,TString plot_type){
  std::cout<<"INFO: ScaleFactorCalculator::MakeCalibrationPlots(): Making Calibration Plots of type: "<<plot_type<<std::endl;

  std::vector<float> mutrackjetbins=m_config->GetMuonJetPtBins();
  std::vector<float> nonmutrackjetbins=m_config->GetNonMuJetPtBins();
  std::vector<float> fj_bins=m_config->GetFatJetPtBins();
  std::vector<float> bin_xerr, bin_x;

  for(unsigned int i_bin=0; i_bin<(fj_bins.size()-1); i_bin++){
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
    
    SF_band_sys->GetXaxis()->SetRangeUser((m_config->GetFatJetPtBins())[0],m_config->GetFatJetPtBins().back());
    
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
    mutrackjetbins.push_back(2*mutrackjetbins[mutrackjetbins.size()-1]-mutrackjetbins[mutrackjetbins.size()-2]);

    nonmutrackjetbins.insert(nonmutrackjetbins.begin(),0.);
    nonmutrackjetbins.push_back(2*nonmutrackjetbins[nonmutrackjetbins.size()-1]-nonmutrackjetbins[nonmutrackjetbins.size()-2]);
    nonmutrackjetbins.push_back(nonmutrackjetbins[nonmutrackjetbins.size()-1]+140.);


    for(auto &el : mutrackjetbins) std::cout<<"Muon track jet bins: "<<el<<std::endl;
    for(auto &el : nonmutrackjetbins) std::cout<<"Non Muon track jet bins: "<<el<<std::endl;

    const char* mu_labels[3]={"p_{T}(#mu-jet) < 100GeV","100GeV < p_{T}(#mu-jet) < 200GeV", "p_{T}(#mu-jet) > 200GeV"};
    const char* nonmu_labels[4]={"p_{T}(non-#mu-jet) < 100GeV","100GeV < p_{T}(non-#mu-jet) < 200GeV","200GeV < p_{T}(non-#mu-jet) < 300GeV", "p_{T}(non-#mu-jet) > 300GeV"};


    hist=new TH2D("hist","",mutrackjetbins.size()-1,&mutrackjetbins[0],nonmutrackjetbins.size()-1,&nonmutrackjetbins[0]);
    hist_err_up=new TH2D("hist_err_up","",mutrackjetbins.size()-1,&mutrackjetbins[0],nonmutrackjetbins.size()-1,&nonmutrackjetbins[0]);
    hist_err_down=new TH2D("hist_err_down","",mutrackjetbins.size()-1,&mutrackjetbins[0],nonmutrackjetbins.size()-1,&nonmutrackjetbins[0]);

    //std::cout<<"After histogram creation"<<std::endl;

    /*for(auto &el : val_y) std::cout<<"val_y: "<<el<<std::endl;
    for(auto &el : tot_y_err_up) std::cout<<"tot_y_err_up: "<<el<<std::endl;
    for(auto &el : tot_y_err_down) std::cout<<"tot_y_err_down: "<<el<<std::endl;
    */

    //for(auto &el : val_ymc) std::cout<<"val_ymc: "<<el<<std::endl;

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

    for(unsigned int i=1; i<=3; i++) hist->GetXaxis()->SetBinLabel(i,mu_labels[i-1]);
    for(unsigned int i=1; i<=4; i++) hist->GetYaxis()->SetBinLabel(i,nonmu_labels[i-1]);
    hist->LabelsOption("u","Y");

    canv->SetLeftMargin(0.25);
    canv->SetRightMargin(0.05);
    canv->cd();

    hist->SetMarkerSize(1.8);
    gStyle->SetPaintTextFormat("4.2f");
    hist->Draw("COL TEXT");
    //hist->SetAxisRange(nonmutrackjetbins[0], nonmutrackjetbins.back()*1.4,"Y");
    hist_err_up->SetBarOffset(0.2);
    //hist_err_up->SetMarkerSize(1.8);
    //hist_err_up->SetMarkerColor(kGreen+2);
    //gStyle->SetPaintTextFormat("(+) %4.2f ");
    hist_err_up->Draw("TEXT SAME");
    //hist_err_down->SetMarkerSize(1.8);
    //hist_err_down->SetMarkerColor(kRed);
    hist_err_down->SetBarOffset(-0.2);
    //gStyle->SetPaintTextFormat("8-) %4.2f ");
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

  TLine *line = new TLine((m_config->GetFatJetPtBins())[0],1.,(m_config->GetFatJetPtBins()).back(),1.);
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

void ScaleFactorCalculator::MakeTemplateControlPlots(bool applyFitCorrection, std::shared_ptr<TH1D> dataHist, std::vector<std::shared_ptr<TH1D>> templateHists, TString& channel, TString& region, TString &sys,  int rebin, bool isPosttag){
  
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

  TH1D* tmp_stacked_mc(nullptr);
  
  TH1D *full_mc(nullptr);

  for(unsigned int i_p=0; i_p<templateHists.size(); i_p++){

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

    leg->AddEntry(tmp_stacked_mc,m_config->GetFlavourPairs()[i_p],"f");
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
  h_ratio->SetXTitle("transverse IP significance #LT s_{d0} #GT"); 
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
    
    fitsys=this->getTemplateFitUncertToys(applyFitCorrection,templateHists,region,sys,rebin);
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
  

  TString name= applyFitCorrection ? TString("./ctrl_plots/Template_"+sys+"_"+channel+"_postfit_"+region+"_sd0.pdf") : TString("./ctrl_plots/Template_"+sys+"_"+channel+"_prefit_"+region+"_sd0.pdf"); 
  
  if(isPosttag) name=applyFitCorrection ? TString("./ctrl_plots/Template_"+sys+"_"+channel+"_postfit_"+region+"_sd0_posttag.pdf") : TString("./ctrl_plots/Template_"+sys+"_"+channel+"_prefit_"+region+"_sd0_posttag.pdf"); 
  

  canv->SaveAs(name.Data());

  data_hist->SetMinimum(1);
  data_hist->SetMaximum(data_hist->GetBinContent(data_hist->GetMaximumBin())*1e2);
  pad1->SetLogy();
  pad1->RedrawAxis();
  
  TString namelog= applyFitCorrection ? TString("./ctrl_plots/Template_"+sys+"_"+channel+"_postfit_"+region+"_sd0_log.pdf") : TString("./ctrl_plots/Template_"+sys+"_"+channel+"_prefit_"+region+"_sd0_log.pdf"); 

  if(isPosttag) namelog=applyFitCorrection ? TString("./ctrl_plots/Template_"+sys+"_"+channel+"_postfit_"+region+"_sd0_posttag_log.pdf") : TString("./ctrl_plots/Template_"+sys+"_"+channel+"_prefit_"+region+"_sd0_posttag_log.pdf");


  canv->SaveAs(namelog.Data());

  delete mystack;
  delete leg;

}


void ScaleFactorCalculator::MakeFatJetControlPlots(TString &var,bool isPosttag, bool applyFitCorrection, std::vector<TString>& sys, std::vector<TString>& model_sys, bool doPrintByRegion, TString region){
  
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

  for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){

    name_pretag="hist_pretag_mc_"+(m_config->GetFlavourPairs())[i_p];
    name_posttag="hist_posttag_mc_"+(m_config->GetFlavourPairs())[i_p];

    hist_pretag_mc.push_back(new TH1D(name_pretag.Data(),"",fj_bins.size()-1,&(fj_bins[0])));
    hist_posttag_mc.push_back(new TH1D(name_posttag.Data(),"",fj_bins.size()-1,&(fj_bins[0])));

  }  

  TH1D *hist_pretag_data=new TH1D("hist_pretag_data","",fj_bins.size()-1,&(fj_bins[0]));
  TH1D *hist_posttag_data=new TH1D("hist_posttag_data","",fj_bins.size()-1,&(fj_bins[0])); 
  
  TH1D* help(nullptr), *help_rebinned(nullptr);

  double* d_fj_bins = new double[fj_bins.size()];

  for(unsigned int i_b=0; i_b<fj_bins.size(); i_b++) d_fj_bins[i_b]=(double)fj_bins[i_b];

  for(unsigned int i_reg=0; i_reg<regions.size(); i_reg++){

    if(doPrintByRegion && !(regions[i_reg].EqualTo(region))) continue;
    
    TString name_mc=regions[i_reg]+"_"+var+"_"+"Nom";
    //std::cout<<"name_mc: "<<name_mc<<std::endl;

    for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){
      
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

  TH1D* tmp_stacked_mc(nullptr);
  
  TH1D *full_mc(nullptr);

  for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){

    tmp_stacked_mc=isPosttag ?  (TH1D*)hist_posttag_mc[i_p]->Clone() : (TH1D*)hist_pretag_mc[i_p]->Clone();

    if(i_p==0) full_mc=(TH1D*)tmp_stacked_mc->Clone();
    else full_mc->Add(tmp_stacked_mc);


    tmp_stacked_mc->SetFillColor(color[i_p]);
    tmp_stacked_mc->SetLineColor(kBlack);

    mystack->Add(tmp_stacked_mc);

    leg->AddEntry(tmp_stacked_mc,m_config->GetFlavourPairs()[i_p],"f");
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
  if(var.Contains("PREFITPOSTTAG") || var.Contains("nmjpt")) data_hist->GetYaxis()->SetRangeUser(0.,data_hist->GetMaximum()*1.4);
  else if(var.Contains("fjm") || var.Contains("mjpt")) data_hist->GetYaxis()->SetRangeUser(0.,data_hist->GetMaximum()*2.);
  else data_hist->GetYaxis()->SetRangeUser(0.,data_hist->GetMaximum()*1.2);
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
  else if (var.Contains("fjtau21"))h_ratio->SetXTitle("large-R jet #tau_{21}");

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
  TGraphAsymmErrors* fitsys=0, *btagsys=0, *expsys=0, *mcstat=0, *totsys=0, *modsys=0;
  
  if(applyFitCorrection){
    
      fitsys=this->getFitUncertToys(var,isPosttag);
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

  if(model_sys.size()>0)modsys=this->getModellingUncert(var,model_sys,applyFitCorrection,isPosttag);
  modsys->SetFillStyle(3001);
  modsys->SetFillColor(kBlue);  
  //mcstat->Draw("5");

  /*expsys->SetFillColor(kBlue);
  //totsys->SetFillStyle(3005);                                                                                                                                                             
  expsys->SetFillStyle(3245);
  gStyle->SetHatchesLineWidth(1.5);
  expsys->SetLineColor(kBlue);
  expsys->Draw("5");*/

  if(expsys){
    totsys=getTotalSys(fitsys,btagsys,expsys,mcstat,modsys);
    totsys->SetFillColor(kBlack);
    //totsys->SetFillStMyle(3005);
    totsys->SetFillStyle(3245);
    gStyle->SetHatchesLineWidth(1.5);
    totsys->SetLineColor(kBlack);
    pad2->cd();
    leg->AddEntry(totsys,"total uncertainty","f");
    totsys->Draw("5");
  }
  
  /*std::vector<TGraphAsymmErrors*> expsys_separate;

  if(sys.size()>0) expsys_separate=this->getExperimentalUncertSeparate(var,sys,applyFitCorrection, isPosttag);
  for(auto &el : expsys_separate) el->Draw("EPsame");
  */
  if(fitsys) fitsys->Draw("2");
  //if(modsys) modsys->Draw("2");
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

void ScaleFactorCalculator::MakeBTaggingRatePlots(std::vector<TString> &sys, std::vector<TString>& model_sys){

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

  for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){

    name_pretag="hist_pretag_mc_"+(m_config->GetFlavourPairs())[i_p];
    name_posttag="hist_posttag_mc_"+(m_config->GetFlavourPairs())[i_p];

    hist_pretag_mc.push_back(new TH1D(name_pretag.Data(),"",fj_bins.size()-1,&(fj_bins[0])));
    hist_posttag_mc.push_back(new TH1D(name_posttag.Data(),"",fj_bins.size()-1,&(fj_bins[0])));

  }  

  TH1D *hist_pretag_data=new TH1D("hist_pretag_data","",fj_bins.size()-1,&(fj_bins[0]));
  TH1D *hist_posttag_data=new TH1D("hist_posttag_data","",fj_bins.size()-1,&(fj_bins[0])); 
  
  TH1D* help, *help_rebinned;

  double* d_fj_bins = new double[fj_bins.size()];

  for(unsigned int i_b=0; i_b<fj_bins.size(); i_b++) d_fj_bins[i_b]=(double)fj_bins[i_b];

  for(unsigned int i_reg=0; i_reg<regions.size(); i_reg++){

    TString name_mc_pretag=regions[i_reg]+"_"+var+"_"+"Nom";
    TString name_mc_posttag=regions[i_reg]+"_"+var+"_PREFITPOSTTAG"+"_"+"Nom";
    //std::cout<<"name_mc: "<<name_mc<<std::endl;

    for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){
      
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

  TH1D *full_mc_pretag(nullptr), *full_mc_posttag(nullptr);

  for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){

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
  hist_posttag_data->GetYaxis()->SetRangeUser(0.,hist_posttag_data->GetMaximum()*2);

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
  TGraphAsymmErrors *fitsys=getFitUncertBTagRateToys();
  fitsys->SetFillColor(kGreen+1);
  fitsys->SetFillStyle(3001);

  //TEST
  //TGraphAsymmErrors *fitsys_toys=getFitUncertBTagRateToys();

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
  
  TGraphAsymmErrors *modsys=this->getModellingUncert(fjpt,model_sys,true,true,true);
  modsys->SetFillColor(kBlue);
  modsys->SetFillStyle(3001);


  TGraphAsymmErrors *mcstat=this->getMCStat(full_mc_posttag);
  
  TGraphAsymmErrors *totsys=this->getTotalSys(fitsys, btagsys, expsys, mcstat,modsys);
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
  //if(modsys) modsys->Draw("2");
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

void ScaleFactorCalculator::SaveReweightHists(TString &var, TString &outfilename){

  std::vector<float> fj_bins=m_config->GetBins(var);
  std::vector<TString> regions=m_config->GetAllRegions();
  
  std::vector<TH1D*> hist_prefit_mc, hist_postfit_mc;
  TString name_pretag;

  for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){

    name_pretag="hist_prefit_mc_"+(m_config->GetFlavourPairs())[i_p];

    hist_prefit_mc.push_back(new TH1D(name_pretag.Data(),"",fj_bins.size()-1,&(fj_bins[0])));

    name_pretag="hist_postfit_mc_"+(m_config->GetFlavourPairs())[i_p];

    hist_postfit_mc.push_back(new TH1D(name_pretag.Data(),"",fj_bins.size()-1,&(fj_bins[0])));
    

  }  

  TH1D* help, *help_rebinned;

  double* d_fj_bins = new double[fj_bins.size()];

  for(unsigned int i_b=0; i_b<fj_bins.size(); i_b++) d_fj_bins[i_b]=(double)fj_bins[i_b];

  for(unsigned int i_reg=0; i_reg<regions.size(); i_reg++){

    TString name_mc=regions[i_reg]+"_"+var+"_"+"Nom";
    //std::cout<<"name_mc: "<<name_mc<<std::endl;

    for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){
      help=(TH1D*)m_fatjet_histograms_pretag[name_mc][i_p]->Clone();
      help_rebinned=(TH1D*)help->Rebin((int)fj_bins.size()-1,"help_rebinned",d_fj_bins);
      hist_prefit_mc[i_p]->Add(help_rebinned);
      help_rebinned->Scale(m_fit_params[regions[i_reg]+"_Nom"][i_p]);
      //std::cout<<"Correction factor "<<i_p<<" : for hist: "<<m_fatjet_histograms_pretag[name_mc][i_p]->GetName()<<" : "<<m_fit_params[regions[i_reg]+"_Nom"][i_p]<<std::endl;
      hist_postfit_mc[i_p]->Add(help_rebinned);
    }
  }

  TH1D* tmp_stacked_mc(nullptr);
  
  TH1D *full_mc(nullptr), *full_mc_postfit(nullptr), *scale_factors(nullptr);

  scale_factors=new TH1D("scale_factors","",fj_bins.size()-1,&(fj_bins[0]));

  for(unsigned int i_p=0; i_p<m_config->GetFlavourPairs().size(); i_p++){

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
