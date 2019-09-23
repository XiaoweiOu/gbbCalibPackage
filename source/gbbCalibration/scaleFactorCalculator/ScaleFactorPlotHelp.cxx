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
#include "helpers/atlasstyle/AtlasLabels.h"

std::vector<TString> ScaleFactorCalculator::makeTrkJetLabels(std::vector<float> bins, TString jetName) {
  std::vector<TString> labels;
  labels.push_back(Form("p_{T}(%s) < %iGeV",jetName.Data(),(int)bins.front()));
  for (unsigned int i=0; i < bins.size()-1; i++) {
    labels.push_back(Form("p_{T}(%s) #splitline{ >%iGeV}{< %iGeV}",
                       jetName.Data(),(int)bins[i],(int)bins[i+1]));
  }
  labels.push_back(Form("p_{T}(%s) > %iGeV",jetName.Data(),(int)bins.back()));

  return labels;
}

void ScaleFactorCalculator::MakeCalibrationPlots(CalibResult cl_result,TString plot_type){
  std::cout<<"INFO: ScaleFactorCalculator::MakeCalibrationPlots(): Making Calibration Plots of type: "<<plot_type<<std::endl;

  std::vector<TString> mj_labels = makeTrkJetLabels(m_config->GetMuonJetPtBins(),"#mu-jet");
  std::vector<TString> nmj_labels = makeTrkJetLabels(m_config->GetNonMuJetPtBins(),"non-#mu-jet");
  std::vector<float> mj_bins, nmj_bins;
  for (unsigned int i=0; i < mj_labels.size()+1; i++) mj_bins.push_back(i);
  for (unsigned int i=0; i < nmj_labels.size()+1; i++) nmj_bins.push_back(i);
  // NB: adding extra bin to provide blank space for header
  nmj_bins.push_back(1.1*(nmj_labels.size()+1));

  std::vector<float> fj_bins = m_config->GetFatJetPtBins();
  std::vector<float> bin_xerr, bin_x;

  // FIXME: will need to change this if/when we calibrate lower pT
  bin_xerr.push_back(0.5*(fj_bins[0]-500));
  bin_x.push_back(500+bin_xerr[0]);
  for(unsigned int i_bin = 0; i_bin<fj_bins.size()-1; i_bin++){
    bin_xerr.push_back(0.5*(fj_bins[i_bin+1]-fj_bins[i_bin]));
    bin_x.push_back(fj_bins[i_bin]+bin_xerr[i_bin+1]);
  }

  int N = bin_xerr.size();

  std::vector<float> val_y,tot_y_err_up,tot_y_err_down,stat_y_err,val_ymc,stat_ymc_err;

  if (plot_type.Contains("SF")) {

    val_y = cl_result.fnominal_sf;
    tot_y_err_up = cl_result.ftot_err_up;
    tot_y_err_down = cl_result.ftot_err_down;
    stat_y_err = cl_result.fstat_err;

  } else if (plot_type.Contains("Eff")) {

    val_y = cl_result.fnominal_data_eff;
    tot_y_err_up = cl_result.ftot_err_data_eff_up;
    tot_y_err_down = cl_result.ftot_err_data_eff_down;
    stat_y_err = cl_result.fstat_err_data_eff;

    val_ymc = cl_result.fnominal_mc_eff;
    stat_ymc_err = cl_result.fstat_err_mc_eff;

  } else {
    std::cout << "ERROR: unrecognized plot type: " << plot_type.Data() << std::endl;
    return;
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

  if (!plot_type.Contains("2D")) {

    //make TGraph with bands
    SF_band_sys=new TGraphAsymmErrors(N,&(bin_x[0]),&(val_y[0]),&(bin_xerr[0]),&(bin_xerr[0]),&(tot_y_err_down[0]),&(tot_y_err_up[0]));
    SF_band_sys->SetMarkerSize(0);
    SF_band_sys->SetFillColor(kGreen+3);
    SF_band_sys->SetFillStyle(3001);
    SF_band_sys->SetTitle("");

    SF_band_sys->GetXaxis()->SetTitle("Large-R Jet p_{T} [GeV]");
    SF_band_sys->GetYaxis()->SetTitle("Scale Factor");
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

  } else if(plot_type.Contains("2D")) {

    hist=new TH2D("hist","",mj_bins.size()-1,&mj_bins[0],nmj_bins.size()-1,&nmj_bins[0]);
    hist_err_up=new TH2D("hist_err_up","",mj_bins.size()-1,&mj_bins[0],nmj_bins.size()-1,&nmj_bins[0]);
    hist_err_down=new TH2D("hist_err_down","",mj_bins.size()-1,&mj_bins[0],nmj_bins.size()-1,&nmj_bins[0]);

    //std::cout<<"After histogram creation"<<std::endl;

    /*for(auto &el : val_y) std::cout<<"val_y: "<<el<<std::endl;
    for(auto &el : tot_y_err_up) std::cout<<"tot_y_err_up: "<<el<<std::endl;
    for(auto &el : tot_y_err_down) std::cout<<"tot_y_err_down: "<<el<<std::endl;
    */

    //for(auto &el : val_ymc) std::cout<<"val_ymc: "<<el<<std::endl;

    for(unsigned int i=0; i<mj_labels.size(); ++i){ // loop over mu jet pt bins
      for(unsigned int j=0; j<nmj_labels.size(); ++j){ // loop over non-mu jet pt bins
        int n_region = j+i*nmj_labels.size();
	      if(plot_type.EqualTo("2DSF") || plot_type.EqualTo("2DEffData")){
	        hist->SetBinContent(i+1,j+1,val_y[n_region]);
	        hist_err_up->SetBinContent(i+1,j+1,tot_y_err_up[n_region]);
	        hist_err_down->SetBinContent(i+1,j+1,tot_y_err_down[n_region]);
	      }else if(plot_type.EqualTo("2DEffMC")){
	        hist->SetBinContent(i+1,j+1,val_ymc[n_region]);
	        hist_err_up->SetBinContent(i+1,j+1,stat_ymc_err[n_region]);
          hist_err_down->SetBinContent(i+1,j+1,stat_ymc_err[n_region]);
	      }
      }
    }

    for(unsigned int i=1; i<=mj_labels.size(); i++) hist->GetXaxis()->SetBinLabel(i,mj_labels[i-1]);
    for(unsigned int i=1; i<=nmj_labels.size(); i++) hist->GetYaxis()->SetBinLabel(i,nmj_labels[i-1]);
    hist->LabelsOption("u","Y");

    hist->SetLabelSize(0.05,"X");
    hist->SetLabelSize(0.05,"Y");

    hist->SetMinimum(0);
    if(plot_type.Contains("SF")) hist->SetMaximum(2.);
    if(plot_type.Contains("Data")) hist->SetMaximum(1.1);
    if(plot_type.Contains("MC")){
      hist->SetMinimum(0.3);
      hist->SetMaximum(0.7);
    }
    canv->SetTopMargin(0.05);
    canv->SetLeftMargin(0.28);
    canv->SetRightMargin(0.05);
    canv->cd();

    hist->SetMarkerSize(2);
    gStyle->SetPaintTextFormat("4.2f");
    hist->Draw("COL TEXT");
    //hist->SetAxisRange(nmj_bins[0], nmj_bins.back()*1.4,"Y");
    hist_err_up->SetBarOffset(0.23);
    hist_err_up->SetMarkerSize(1.3);
    //hist_err_up->SetMarkerSize(1.8);
    //hist_err_up->SetMarkerColor(kGreen+2);
    //gStyle->SetPaintTextFormat("(+) %4.2f ");
    hist_err_up->Draw("TEXT SAME");
    //hist_err_down->SetMarkerSize(1.8);
    //hist_err_down->SetMarkerColor(kRed);
    hist_err_down->SetBarOffset(-0.23);
    hist_err_down->SetMarkerSize(1.3);
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
  ATLASLabel2(0.675,0.86,m_plot_label.Data());
  myText(0.675, 0.815, 1, Form("#scale[0.8]{%s}",m_sub_label.Data()));
  myText(0.675, 0.775, 1, Form("#scale[0.8]{%s}",m_subsub_label.Data()));

  TLine *line = new TLine((m_config->GetFatJetPtBins())[0],1.,(m_config->GetFatJetPtBins()).back(),1.);
  line->SetLineStyle(2);
  line->SetLineWidth(1.5);

  if(plot_type.EqualTo("SF")) line->Draw("same");

  TDatime today;

  TString name;
  if(plot_type.EqualTo("SF")) name = m_outdir + "/SF/ScaleFactors" + m_pext;
  if(plot_type.EqualTo("Eff")) name = m_outdir + "/SF/Efficiencies" + m_pext;
  if(plot_type.EqualTo("2DSF")) name = m_outdir + "/SF/2DScaleFactors" + m_pext;
  if(plot_type.EqualTo("2DEffData")) name = m_outdir + "/SF/2DEfficienciesData" + m_pext;
  if(plot_type.EqualTo("2DEffMC")) name = m_outdir + "/SF/2DEfficienciesMC" + m_pext;

  canv->SaveAs(name.Data());

  if(EFF_mc_stat) delete EFF_mc_stat;
  if(SF_band_sys) delete SF_band_sys;
  if(SF_data_stat) delete SF_data_stat;
  if(hist) delete hist;
  if(hist_err_up) delete hist_err_up;
  if(hist_err_down) delete hist_err_down;
  delete canv;
  delete leg;

}

void ScaleFactorCalculator::MakeTemplateControlPlots(bool applyFitCorrection, std::shared_ptr<TH1D> dataHist, std::vector<std::shared_ptr<TH1D>> templateHists, TString& channel, TString& region, TString &sys,  int rebin, bool isPosttag){

  //FIXME: can some of this setup be moved to another function?
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

  //prepare legend
  TLegend *leg=new TLegend(0.68,0.4,0.88,0.75);

  leg->SetBorderSize(0);
  leg->SetFillStyle(0);

  TH1D* data_hist= (TH1D*)dataHist->Clone();

  if(rebin>1) data_hist->Rebin(rebin);

  THStack *mystack=new THStack("myStack","");

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
      tmp_stacked_mc->SetBinError(i_bin, tmp_stacked_mc->GetBinError(i_bin)/tmp_stacked_mc->GetBinWidth(i_bin));
    }

    if(i_p==0) full_mc=(TH1D*)tmp_stacked_mc->Clone();
    else full_mc->Add(tmp_stacked_mc);

    tmp_stacked_mc->SetFillColor(color[i_p]);
    tmp_stacked_mc->SetLineColor(kBlack);

    mystack->Add(tmp_stacked_mc);

    //leg->AddEntry(tmp_stacked_mc,m_config->GetFlavourPairs()[i_p],"f");
  }

  //add Entry to legend in reverse order
  TList* histKeys=mystack->GetHists();
  TIter next(histKeys,kIterBackward);
  TObject* object=0;
  int i_pairs=m_config->GetFlavourPairs().size()-1;

  while((object = next())){
    leg->AddEntry((TH1*)object,m_config->GetFlavourPairs()[i_pairs],"f");
    i_pairs--;
  }

  //divide by bin width
  for(int i_bin=1; i_bin<=data_hist->GetNbinsX(); i_bin++){
    data_hist->SetBinContent(i_bin, data_hist->GetBinContent(i_bin)/data_hist->GetBinWidth(i_bin));
    data_hist->SetBinError(i_bin, data_hist->GetBinError(i_bin)/data_hist->GetBinWidth(i_bin));
  }

  data_hist->SetTitle("");
  data_hist->SetLabelSize(0,"X");
  data_hist->SetTitleSize(0,"X");
  //data_hist->SetYTitle(Form("events/%.1f",(data_hist->GetBinWidth(1))));
  data_hist->SetYTitle("dN/dS_{d0}");
  data_hist->SetTitleOffset(1.5,"Y");
  data_hist->SetMarkerStyle(20);
  data_hist->SetTitleSize(0.05,"Y");
  data_hist->SetLabelSize(0.05,"Y");

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
  //TODO: for a function that ostensibly takes arbitrary hists, this shouldnt be hard-coded
  h_ratio->SetXTitle("transverse IP significance #LT s_{d0} #GT");
  h_ratio->SetLabelSize(0.1,"X");
  h_ratio->SetLabelSize(0.1,"Y");
  h_ratio->SetTitleSize(0.12,"X");
  h_ratio->SetTitleSize(0.13,"Y");
  h_ratio->SetTitleOffset(0.9,"X");
  h_ratio->SetTitleOffset(0.6,"Y");
  h_ratio->GetYaxis()->SetRangeUser(0.4,1.6);

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
    leg->AddEntry(fitsys,"fit uncert.","f");
    h_ratio->Draw("EPsame");
    h_ratio->GetYaxis()->SetRangeUser(0.4,1.6);
  }

  //double chi2=data_hist->Chi2Test(full_mc,"WW CHI2/NDF");
  //TString text_Chi2=Form("Chi2/NDF = %f",chi2);

  pad1->cd();
  leg->Draw();

  //Add ATLAS label
  pad1->cd();
  ATLASLabel3(0.50,0.825,m_plot_label.Data());
  myText(0.5, 0.78, 1, Form("#scale[0.8]{%s}",m_sub_label.Data()));
  pad1->RedrawAxis();

  TString name = TString(m_outdir+"/ctrl_plots/Template_"+sys+"_"+channel+"_");
  name += applyFitCorrection ? "postfit_" : "prefit_";
  name += isPosttag ? "posttag_" : "pretag_";
  name += region;

  canv->SaveAs((name+m_pext).Data());

  data_hist->SetMinimum(1);
  data_hist->SetMaximum(data_hist->GetBinContent(data_hist->GetMaximumBin())*1e2);
  pad1->SetLogy();
  pad1->RedrawAxis();

  canv->SaveAs((name+"_log"+m_pext).Data());

  delete mystack;
  delete leg;

}

void ScaleFactorCalculator::MakeCorrelationPlots(const TString region) {
  std::cout<<"INFO: ScaleFactorCalculator::MakeCorrelationPlots(): Making plots of covariance matrix for region "<<region.Data()<<std::endl;

  std::vector<double> params = m_fit_params[ (region+"_Nom") ];
  std::vector<double> errs = m_fit_errs[ (region+"_Nom") ];
  std::vector<double> cov_mat = m_nom_cov_mats[region];
  int npar = params.size();

  TH2D* hpar = new TH2D("h_par","",1,0,1,npar+1,0,npar+1);
  TH2D* herr = new TH2D("h_err","",1,0,1,npar+1,0,npar+1);
  TH2D* hist = new TH2D("h_cov_mat","",npar,0,npar,npar+1,0,npar+1);
  for (int i_par=0; i_par < npar; i_par++) {
    hpar->SetBinContent(1,npar-i_par, params[i_par]);
    herr->SetBinContent(1,npar-i_par, errs[i_par]);
    for (int j_par=0; j_par < npar; j_par++) {
      hist->SetBinContent(j_par+1,npar-i_par, cov_mat[i_par*npar + j_par] / (errs[j_par]*errs[i_par]) );
    }
  }

  TCanvas *canv=new TCanvas("canv","",1000,600);
  TPad *pad1=new TPad("pad1","",0,0,0.3,1);
  pad1->SetLeftMargin(0.3);
  pad1->SetRightMargin(0);
  pad1->SetTopMargin(0.05);
  pad1->SetBottomMargin(0.05);
  pad1->Draw();
  canv->cd();
  TPad *pad2=new TPad("pad2","",0.3,0,1,1);
  pad2->SetLeftMargin(0.01);
  pad2->SetRightMargin(0.1);
  pad2->SetTopMargin(0.05);
  pad2->SetBottomMargin(0.05);
  pad2->Draw();
  canv->cd();
  const char* txtForm = gStyle->GetPaintTextFormat();
  gStyle->SetPaintTextFormat(".3g");

  pad1->cd();
  hpar->Draw("text");
  hpar->SetMarkerSize(4.2);
  for (int i=0; i < npar; i++) hpar->GetYaxis()->SetBinLabel(npar-i,m_fitpar_names[i].Data());
  hpar->GetXaxis()->SetLabelSize(0);
  hpar->GetXaxis()->SetNdivisions(0);
  hpar->GetYaxis()->SetLabelSize(0.08);
  herr->SetBarOffset(-0.25);
  herr->SetMarkerSize(3.0);
  herr->Draw("TEXT SAME");

  pad2->cd();
  hist->Draw("col text");
  hist->SetMarkerSize(1.8);
  hist->SetMinimum(hist->GetMinimum());
  for (int i=1; i < npar+1; i++) {
    hist->GetXaxis()->SetBinLabel(i,m_fitpar_names[i-1].Data());
    hist->SetBinContent(i,npar+1, -999.);
  }
  hist->GetYaxis()->SetLabelSize(0);
  hist->GetXaxis()->SetLabelSize(0.04);

  //Add ATLAS label
  ATLASLabel3(0.6,0.88,m_plot_label.Data());
  myText(0.6, 0.84, 1, Form("#scale[0.8]{%s}",m_sub_label.Data()));
  myText(0.6, 0.81, 1, Form("#scale[0.6]{%s}",m_subsub_label.Data()));

  TString name = Form("%s/ctrl_plots/CorrMat_%s" + m_pext,m_outdir.Data(),region.Data());
  canv->SaveAs(name.Data());

  gStyle->SetPaintTextFormat(txtForm);
  delete canv;

}


void ScaleFactorCalculator::MakeFatJetControlPlots(TString &var, bool applyFitCorrection, std::vector<TString>& sys, std::vector<TString>& model_sys, bool doPrintByRegion, TString region){

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
  //prepare Legend
  TLegend *leg=new TLegend(0.68,0.4,0.88,0.75);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);

  std::vector<int> color={kBlue+1,kAzure-4,kCyan+3,kGreen-9,kOrange};

  //if plots in eta/phi bins, take binning for fat pt
  TString var_fjpt="fjpt";
  //TODO: is this needed? if so, can it be added to GetRebinHists functions?
  std::vector<double> fj_bins= (var.Contains("fjphi") && var.Contains("fjeta"))  ? m_config->GetBinning(var_fjpt) : m_config->GetBinning(var);

  bool isPosttag = var.Contains("PREFITPOSTTAG");

  std::cout << " ***** Integrals **** "<< std::endl;
  std::cout << "Variable: " << var;
  if (applyFitCorrection) std::cout << " (postfit)"<< std::endl;
  else std::cout << " (prefit)"<< std::endl;

  std::vector<TH1D*> hist_mc;
  TH1D* hist_data(nullptr);
  if (doPrintByRegion) {
    hist_mc = GetRebinHistsByRegionMC(var, "Nom", region, (int)applyFitCorrection);
    hist_data = GetRebinHistByRegionData(var, region);
  } else {
    hist_mc = GetRebinHistsMC(var, "Nom", (int)applyFitCorrection);
    hist_data = GetRebinHistData(var);
  }

  std::cout << "Data: " << hist_data->Integral() << std::endl;

  double tot_mcint = 0;

  THStack *mystack = new THStack("myStack","stack");
  TH1D* full_mc(nullptr);
  for (unsigned int i=0; i < hist_mc.size(); i++) {
    if (i==0) full_mc = (TH1D*)hist_mc[0]->Clone();
    else full_mc->Add(hist_mc[i]);

    hist_mc[i]->SetFillColor(color[i]);
    hist_mc[i]->SetLineColor(kBlack);

    std::cout << m_config->GetFlavourPairs()[i] <<": " << hist_mc[i]->Integral()*100/hist_data->Integral() << "%" << std::endl;

    tot_mcint += hist_mc[i]->Integral();

    //leg->AddEntry(hist_mc[i],m_config->GetFlavourPairs()[i],"f");
    mystack->Add(hist_mc[i]);
  }

  //add Entry to legend in reverse order
  TList* histKeys=mystack->GetHists();
  TIter next(histKeys,kIterBackward);
  TObject* object=0;
  int i_pairs=m_config->GetFlavourPairs().size()-1;

  while((object = next())){
    leg->AddEntry((TH1*)object,m_config->GetFlavourPairs()[i_pairs],"f");
    i_pairs--;
  }

  std::cout << "Total MC: " << tot_mcint << std::endl;
  std::cout << "Total MC norm: " << tot_mcint/hist_data->Integral() << std::endl;
  std::cout << std::endl << "MC fractions: " << tot_mcint << std::endl;
  for (unsigned int i=0; i < hist_mc.size(); i++) {
    std::cout << m_config->GetFlavourPairs()[i] <<": " << hist_mc[i]->Integral()*100/tot_mcint << "%" << std::endl;
  }
  std::cout << " ***** end var int **** "<< std::endl;

  hist_data->SetTitle("");
  hist_data->SetLabelSize(0,"X");
  if(var.Contains("fjpt")||var.Contains("fjm")) hist_data->SetYTitle(Form("events/%.0f GeV",(hist_data->GetBinWidth(1))));
  else hist_data->SetYTitle(Form("events/%.1f",(hist_data->GetBinWidth(1))));
  hist_data->SetMarkerStyle(20);
  hist_data->SetTitleOffset(1.1,"Y");
  hist_data->SetTitleSize(0.05,"Y");
  hist_data->SetLabelSize(0.05,"Y");

  leg->AddEntry(hist_data,"data","epl");

  hist_data->SetMinimum(0);

  pad1->cd();
  hist_data->Draw("EP");
  if(var.Contains("PREFITPOSTTAG") || var.Contains("nmjpt")) hist_data->GetYaxis()->SetRangeUser(0.,hist_data->GetMaximum()*1.4);
  else if(var.Contains("fjm") || var.Contains("mjpt")) hist_data->GetYaxis()->SetRangeUser(0.,hist_data->GetMaximum()*2.);
  else hist_data->GetYaxis()->SetRangeUser(0.,hist_data->GetMaximum()*1.2);
  mystack->Draw("HIST SAME");
  hist_data->Draw("EP SAME");

  canv->cd();

  //ratio
  TH1D *h_ratio=(TH1D*)hist_data->Clone();
  h_ratio->Divide(full_mc);
  h_ratio->SetMinimum();

  h_ratio->SetTitle("");
  h_ratio->SetMarkerStyle(20);
  h_ratio->SetYTitle("Data/MC");
  //TODO: can this be taken from the histograms directly?
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

      fitsys=this->getFitUncertToys(var);
      fitsys->SetFillColor(kGreen+1);
      fitsys->SetFillStyle(3001);

      pad2->cd();
      fitsys->Draw("2");
      leg->AddEntry(fitsys,"fit uncert.","f");
      //h_ratio->Draw("EPsame");
  }

  if(isPosttag){

    btagsys=this->getBTagUncert(var,applyFitCorrection);
if (btagsys->GetErrorYhigh(10) == 0) std::cout<<"zero btag uncert!"<<std::endl;
    btagsys->SetLineColor(kRed);
    btagsys->SetFillStyle(0);
    //pad2->cd();
    btagsys->Draw("5same");
    //h_ratio->Draw("EPsame");
    leg->AddEntry(btagsys,"b-tagging uncert.","l");

  }

  if(sys.size()>0)expsys=this->getExperimentalUncert(var,sys,applyFitCorrection);
  mcstat=this->getMCStat(full_mc);
  mcstat->SetLineColor(kBlack);
  mcstat->SetFillStyle(0);
std::cout<<"finished getting exp unc"<<std::endl;
  //if(model_sys.size()>0)modsys=this->getModellingUncert(var,model_sys,applyFitCorrection,isPosttag);
  //modsys->SetFillStyle(3001);
  //modsys->SetFillColor(kBlue);
  ////mcstat->Draw("5");

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
    leg->AddEntry(totsys,"total uncert.","f");
    pad2->cd();
    totsys->Draw("5same");
  }
std::cout<<"finished expsys plotting"<<std::endl;

  /*std::vector<TGraphAsymmErrors*> expsys_separate;

  if(sys.size()>0) expsys_separate=this->getExperimentalUncertSeparate(var,sys,applyFitCorrection, isPosttag);
  for(auto &el : expsys_separate) el->Draw("EPsame");
  */
  //if(fitsys) fitsys->Draw("2");
  //if(modsys) modsys->Draw("2");
  //if(btagsys) btagsys->Draw("5");

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
    tot_err->Draw("5same");
    //hist_data->Draw("EP SAME");
  }

  //double chi2=hist_data->Chi2Test(full_mc,"UW CHI2/NDF");
  //TString text_Chi2=Form("Chi2/NDF = %f",chi2);

  pad1->cd();
  leg->Draw();

  //TLatex latex2;
  //latex2.SetNDC();
  //latex2.SetTextAlign(12);
  //latex2.SetTextSize(0.04);
  //latex2.DrawLatex(0.5,0.8,text_Chi2.Data());

  //Add ATLAS label
  pad1->cd();
  ATLASLabel3(0.55,0.825,m_plot_label.Data());
  myText(0.55, 0.78, 1, Form("#scale[0.8]{%s}",m_sub_label.Data()));
  pad1->RedrawAxis();

  TString name = TString(m_outdir+"/ctrl_plots/FatJet_");
  name += applyFitCorrection ? "postfit_" : "prefit_";
  name += isPosttag ? "posttag_" : "pretag_";
  if (doPrintByRegion) name += TString(region+"_");
  name += isPosttag ? TString(var(0,var.Index("_PREFITPOSTTAG"))) : var;
  name += m_pext;

  canv->SaveAs(name.Data());

  delete mystack;
  delete leg;
  delete hist_data;
  delete full_mc;
  for (unsigned int i=0; i < hist_mc.size(); i++) delete hist_mc[i];

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
  //prepare legend
  TLegend *leg=new TLegend(0.68,0.4,0.88,0.75);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);

  std::vector<int> color={kBlue+1,kAzure-4,kCyan+3,kGreen-9,kOrange};

  std::vector<TH1D*> hist_pretag_mc = GetRebinHistsMC(var, "Nom", 1);
  std::vector<TH1D*> hist_posttag_mc = GetRebinHistsMC(var+"_PREFITPOSTTAG", "Nom", 1);
  TH1D* hist_pretag_data = GetRebinHistData(var);
  TH1D* hist_posttag_data = GetRebinHistData(var+"_PREFITPOSTTAG");

  TH1D* full_mc_pretag = (TH1D*)hist_pretag_mc[0]->Clone();
  for (unsigned int i=1; i < hist_pretag_mc.size(); i++) full_mc_pretag->Add(hist_pretag_mc[i]);
  TH1D* full_mc_posttag = (TH1D*)hist_posttag_mc[0]->Clone();
  for (unsigned int i=1; i < hist_posttag_mc.size(); i++) full_mc_posttag->Add(hist_posttag_mc[i]);

  //calculate data rate
  hist_posttag_data->Divide(hist_pretag_data);
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
  //hist_posttag_data->GetYaxis()->SetRangeUser(0.,hist_posttag_data->GetMaximum()*2);
  hist_posttag_data->GetYaxis()->SetRangeUser(0.,0.06);

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
  leg->AddEntry(fitsys,"fit uncert.","f");

  //Btagging uncertainties
  TString fjpt="fjpt",fjpt_posttag="fjpt_PREFITPOSTTAG";
  double x,y,eyl,eyh;
  TGraphAsymmErrors *btagsys=getBTagUncert(fjpt_posttag,true);
  for(int i=0; i<btagsys->GetN(); i++){
    btagsys->GetPoint(i,x,y);
    eyl = btagsys->GetErrorYlow(i);
    eyh = btagsys->GetErrorYhigh(i);
    btagsys->SetPoint(i,x,1);
    btagsys->SetPointEYlow(i,eyl);
    btagsys->SetPointEYhigh(i,eyh);
  }

  btagsys->SetLineColor(kRed);
  btagsys->SetFillStyle(0);
  //pad2->cd();
  btagsys->Draw("5");
  h_ratio->Draw("EPsame");
  leg->AddEntry(btagsys,"b-tagging uncert.","l");

  TGraphAsymmErrors *expsys=this->getExperimentalUncert(fjpt,sys,true,true);

  TGraphAsymmErrors *modsys = nullptr;
  //TGraphAsymmErrors *modsys=this->getModellingUncert(fjpt,model_sys,true,true,true);
  //modsys->SetFillColor(kBlue);
  //modsys->SetFillStyle(3001);

  TGraphAsymmErrors *mcstat=this->getMCStat(full_mc_posttag);

  TGraphAsymmErrors *totsys=this->getTotalSys(fitsys, btagsys, expsys, mcstat,modsys);
  //TGraphAsymmErrors *totsys=mcstat;

  totsys->SetFillColor(kBlack);
  //totsys->SetFillStMyle(3005);
  totsys->SetFillStyle(3245);
  gStyle->SetHatchesLineWidth(1.5);
  totsys->SetLineColor(kBlack);
  pad2->cd();
  leg->AddEntry(totsys,"total uncert.","f");
  totsys->Draw("5");
  fitsys->Draw("2");
  btagsys->Draw("5");
  //if(modsys) modsys->Draw("2");
  h_ratio->Draw("EPsame");


  //Add ATLAS label
  pad1->cd();
  ATLASLabel2(0.23,0.825,m_plot_label.Data());
  myText(0.23, 0.78, 1, Form("#scale[0.8]{%s}",m_sub_label.Data()));
  leg->Draw();

  TString name;
  name= TString(m_outdir+"/ctrl_plots/BTaggingRate" + m_pext);

  canv->SaveAs(name.Data());

  delete leg;
  delete full_mc_pretag;
  delete full_mc_posttag;
  for (unsigned int i=0; i < hist_pretag_mc.size(); i++) delete hist_pretag_mc[i];
  for (unsigned int i=0; i < hist_posttag_mc.size(); i++) delete hist_posttag_mc[i];

}

void ScaleFactorCalculator::SaveReweightHists(TString &var, TString &outfilename){

  std::vector<TH1D*> hist_prefit_mc = GetRebinHistsMC(var, "Nom", 0);
  std::vector<TH1D*> hist_postfit_mc = GetRebinHistsMC(var, "Nom", 1);

  TH1D* full_mc = (TH1D*)hist_prefit_mc[0]->Clone();
  for (unsigned int i=1; i < hist_prefit_mc.size(); i++) full_mc->Add(hist_prefit_mc[i]);
  TH1D* full_mc_postfit = (TH1D*)hist_postfit_mc[0]->Clone();
  for (unsigned int i=1; i < hist_postfit_mc.size(); i++) full_mc->Add(hist_postfit_mc[i]);

  TH1D* scale_factors = (TH1D*) full_mc_postfit->Clone();
  scale_factors->Divide(full_mc);

  full_mc->SetName("full_mc");
  full_mc_postfit->SetName("full_mc_postfit");
  scale_factors->SetName("scale_factors");

  TFile *outfile = TFile::Open(outfilename.Data(),"RECREATE");
  outfile->cd();
  full_mc->Write();
  full_mc_postfit->Write();
  scale_factors->Write();
  outfile->Close();

  delete full_mc;
  delete full_mc_postfit;
  delete scale_factors;
  for (unsigned int i=0; i < hist_prefit_mc.size(); i++) delete hist_prefit_mc[i];
  for (unsigned int i=0; i < hist_postfit_mc.size(); i++) delete hist_postfit_mc[i];
}


void ScaleFactorCalculator::MakeNFPlots(){

  std::vector<TString> flavs = m_config->GetFlavourPairs();
  std::vector<TString> systematics;
  if(m_doSystematics) systematics = m_config->GetSystematics();
  std::vector<TString> regions = m_doFitInFatJetPtBins ? m_config->GetFatJetRegions() : m_config->GetTrkJetRegions();

  systematics.push_back("Nom"); //we want to get nominal plots too

  std::vector<TString> mj_labels = makeTrkJetLabels(m_config->GetMuonJetPtBins(),"#mu-jet");
  std::vector<TString> nmj_labels = makeTrkJetLabels(m_config->GetNonMuJetPtBins(),"non-#mu-jet");
  std::vector<float> mj_bins, nmj_bins;
  for (unsigned int i=0; i < mj_labels.size()+1; i++) mj_bins.push_back(i);
  for (unsigned int i=0; i < nmj_labels.size()+1; i++) nmj_bins.push_back(i);
  // NB: adding extra bin to provide blank space for header
  nmj_bins.push_back(1.1*(nmj_labels.size()+1));

  TCanvas * canv = 0;
  TH2D * hist = 0;
  TH2D * hist_err_up = 0;
  TH2D * hist_err_down = 0;

  for(TString flav : flavs){ // loop over flavour fractions
    for(TString sys : systematics){ // loop over systematics + nominal
      canv = new TCanvas("canv","",800,600);
      canv->cd();
      canv->SetTickx();
      canv->SetTicky();

      hist = new TH2D("hist","",mj_bins.size()-1,&mj_bins[0],nmj_bins.size()-1,&nmj_bins[0]);
      hist_err_up = new TH2D("hist_err_up","",mj_bins.size()-1,&mj_bins[0],nmj_bins.size()-1,&nmj_bins[0]);
      hist_err_down = new TH2D("hist_err_down","",mj_bins.size()-1,&mj_bins[0],nmj_bins.size()-1,&nmj_bins[0]);

      for(int i=0; i<(int)mj_labels.size(); ++i){ // loop over mu jet pt bins
        for(int j=0; j<(int)nmj_labels.size(); ++j){ // loop over non-mu jet pt bins
          double par = GetFitScale(sys,regions.at(j+i*nmj_labels.size()),flav);
          double err = GetFitError(sys,regions.at(j+i*nmj_labels.size()),flav);
          hist->SetBinContent(i+1,j+1,par);
          hist_err_up->SetBinContent(i+1,j+1,err);   // errors are symmetric
          hist_err_down->SetBinContent(i+1,j+1,err); //
        }
      }

      for(unsigned int i=1; i<=3; i++) hist->GetXaxis()->SetBinLabel(i,mj_labels[i-1]);
      for(unsigned int i=1; i<=4; i++) hist->GetYaxis()->SetBinLabel(i,nmj_labels[i-1]);
      hist->LabelsOption("u","Y");
      canv->SetLeftMargin(0.25);
      canv->SetRightMargin(0.05);
      canv->cd();

      hist->SetTitle(flav + " " + sys + " NF");
      hist->SetMarkerSize(1.8);
      hist->SetMaximum(1.5);
      hist->SetMinimum(0);
      gStyle->SetPaintTextFormat("4.2f");
      hist->Draw("COL TEXT");
      hist_err_up->SetBarOffset(0.2);
      hist_err_up->Draw("TEXT SAME");
      hist_err_down->SetBarOffset(-0.2);
      hist_err_down->Draw("TEXT SAME");

      //Add ATLAS label
      ATLASLabel2(0.6,0.825,m_plot_label.Data());
      myText(0.6, 0.78, 1, Form("#scale[0.8]{%s}",m_sub_label.Data()));
      myText(0.6, 0.74, 1, Form("#scale[0.8]{%s}",m_subsub_label.Data()));

      TLine *line = new TLine((m_config->GetFatJetPtBins())[0],1.,(m_config->GetFatJetPtBins()).back(),1.);
      line -> SetLineStyle(2);
      line -> SetLineWidth(1.5);
      line -> Draw("SAME");

      TString name = m_outdir + "/NF/2DNF_"+sys+"_"+flav+".pdf";
      canv -> SaveAs(name.Data());

      delete canv;
      delete hist;
      delete hist_err_up;
      delete hist_err_down;
    }
  }
}

