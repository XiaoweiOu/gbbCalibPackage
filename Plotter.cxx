/*
 * Plotter.cxx
 *
 *  Created on: Feb 23, 2016
 *      Author: rjacobs
 */

#include "Plotter.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TSystemDirectory.h"
#include "TSystemFile.h"
#include "TList.h"
#include "TLegend.h"
#include "THStack.h"
#include "TColor.h"
#include "TFile.h"
#include "TLatex.h"
#include "TH2D.h"
#include "TMath.h"
#include "TGraphAsymmErrors.h"

Plotter::Plotter() : m_doGrid(false), m_Rebin(false), m_lumi(1e6){
	// TODO Auto-generated constructor stub

}

Plotter::~Plotter() {
	// TODO Auto-generated destructor stub

  //delete m_pad1;
  // delete m_pad2;
  //delete m_canv;


}

Plotter::Plotter(TString in_dir, SampleInfo* info,TString name) : m_doGrid(false), m_Rebin(false), m_lumi(1e3),m_sampinfo(info),m_doSys(false) {
	// TODO Auto-generated constructor stub

        TString canv_name="canv"+name;
	TString pad1_name="pad1"+name;
	TString pad2_name="pad2"+name;

	this->setAtlasStyle();
	m_canv = std::shared_ptr<TCanvas>(new TCanvas(canv_name.Data(),"canv"+name));
	m_canv.get()->cd();
	m_pad1 = std::shared_ptr<TPad>(new TPad(pad1_name.Data(),"pad1",0,0.2,1,1));
	m_pad1.get()->Draw();
	m_canv->cd();
	m_pad2 = std::shared_ptr<TPad>(new TPad(pad2_name.Data(),"pad2",0,0.005,1,0.3));
	m_pad2.get()->Draw();
	this->ListFiles(in_dir);



}

void Plotter::ListFiles(TString in_dir){

	//list samples in file (files within samples must be merged with hadd_hist.sh script
	//indir is directory containing subdirectories for each sample and merged data samples

	TSystemDirectory dir(in_dir, in_dir);
	TList *files = dir.GetListOfFiles();
	if (files) {
		TSystemFile *file;
		TString fname;
		TIter next(files);
		while ((file=(TSystemFile*)next())) {
			fname = file->GetName();
			if (file->IsDirectory() && fname.EndsWith(".root")) {
			  if(!fname.Contains("data") || fname.EqualTo("user.ruth.data15_13TeV.all.root")){
			    m_samples.push_back(in_dir+fname);
			    //std::cout<<"fname is: "<<fname<<std::endl;
			  }

			}
		}
	}
}


float Plotter::getWeightSample(TString sample_name){

	//get EventCount histogram
	TFile* f=TFile::Open(sample_name.Data(),"READ");
	TH1D * h_bkk;
	f->GetObject("Hist_BookKeeping",h_bkk);
	float weight=1./h_bkk->GetBinContent(3);

	std::cout<<"Number of events DAOD: "<<h_bkk->GetBinContent(2)<<std::endl;
	std::cout<<"Number of events AOD hist: "<<h_bkk->GetBinContent(1)<<std::endl;
	std::cout<<"Number of events AOD AMI: "<<m_sampinfo->xAODEventCount(m_sampinfo->getDSID(sample_name))<<std::endl;

	std::cout<<"Number of events input to ntups: "<<m_sampinfo->DxAODEventCount(m_sampinfo->getDSID(sample_name))<<std::endl;
	std::cout<<"Number of events ntups: "<<h_bkk->GetBinContent(3)<<std::endl;

	//weight*=(h_bkk->GetBinContent(2)/m_sampinfo->xAODEventCount(m_sampinfo->getDSID(sample_name))); // n_DAOD/n_AOD
	weight*=(m_sampinfo->DxAODEventCount(m_sampinfo->getDSID(sample_name))/m_sampinfo->xAODEventCount(m_sampinfo->getDSID(sample_name)));


	weight*=m_sampinfo->CrossSection(m_sampinfo->getDSID(sample_name));
	std::cout<<"Cross section "<<m_sampinfo->CrossSection(m_sampinfo->getDSID(sample_name))<<std::endl;

	weight*=m_sampinfo->FilterEfficiency(m_sampinfo->getDSID(sample_name));
	std::cout<<"Filter eff: "<<m_sampinfo->FilterEfficiency(m_sampinfo->getDSID(sample_name))<<std::endl;

	weight*=m_lumi;
	std::cout<<"Lumi: "<<m_lumi<<std::endl;

	//std::cout<<"weight: "<<weight<<std::endl;

	f->Close();
	
	return weight;
}

void Plotter::prepareCanvas(){

	  m_canv.get()->SetLeftMargin(0.12);
	  m_canv.get()->SetBottomMargin(0.13); //0.13
	  m_canv.get()->SetTopMargin(0.03);
	  m_canv.get()->SetRightMargin(0.04);
	  m_canv.get()->SetBorderMode(0);

	  if(m_doGrid){
	    m_canv.get()->SetGridx();
	    m_canv.get()->SetGridy();
	  }

	  m_pad1.get()->SetBottomMargin(0);
	  m_pad1.get()->SetBorderMode(0);

	  //m_pad2.get()->Range(-209.8909,-1.243902,1106.182,2.170732);
	  m_pad2.get()->Range(0,0,1106.182,2.170732);

	  //m_pad2.get()->SetBorderMode(0);
	  m_pad2.get()->SetBorderSize(2);

	  m_pad2.get()->SetLeftMargin(0.1594828);
	  m_pad2.get()->SetRightMargin(0.05028735);
	  m_pad2.get()->SetTopMargin(0.05);
	  m_pad2.get()->SetBottomMargin(0.9);//0.3642857);
	  //m_pad2.get()->SetFrameBorderMode(0);
	  //m_pad2.get()->SetFrameBorderMode(0);

	  m_pad2.get()->Modified();

}


void Plotter::h2_style(TH1 *h){
  //  return;
   h->SetAxisRange(m_xl,m_xh,"X");
   
   if(m_yl>=0 && m_yh>=0 && m_yh>m_yl){
     h->SetAxisRange(m_yl,m_yh,"Y");
   }else h->SetAxisRange(0.,h->GetMaximum()*1.1,"Y");
   h->SetLabelOffset(0.005,"X");  // D=0.005
   h->SetLabelOffset(0.010,"Y");  // D=0.005
   h->SetTitleOffset(1.0,"X");
   h->SetTitle(0);
}


void Plotter::ratio_style(TH1 *h){
	  //  return;
	   h->SetAxisRange(m_xl,m_xh,"X");
	   h->SetAxisRange(0.,2.,"Y");
	   h->SetNdivisions(505,"Y");

	   h->SetLabelOffset(0.005,"X");  // D=0.005
	   h->SetLabelSize(0.14,"X");
	   h->SetTitleSize(0.15,"X");
	   h->SetTitleOffset(0.9,"X");

	   h->SetTickLength(0.1,"X");

	   h->SetLabelOffset(0.010,"Y");  // D=0.005
           h->SetLabelSize(0.13,"Y");

      	   h->SetTitleOffset(0.5,"Y");
           h->SetTitleSize(0.13,"Y");

	   h->SetTitle(0);

}

void Plotter::leg_style(TLegend *h){
  //  h->SetTextFont(mFont);
  h->SetTextSize(0.045);
  h->SetFillColor(0);
  h->SetFillStyle(0);
  h->SetLineColor(0);
  h->SetBorderSize(0);

}

void Plotter::makeDataMCPlot(TString hist_name){

        std::vector<TString> hist_names(1,hist_name);
        TString data_hist_name=hist_name;

	HistogramStack stack=readInHistStack(hist_names,data_hist_name);

	TH1* data_hist=(TH1*)stack.full_data_hist.get()->Clone();

	TH1* mc_hist=(TH1*)stack.full_mc_hist.get()->Clone();

	m_canv.get()->cd();

	if(data_hist){
		//h2_style(data_hist);
		//data_hist->SetXTitle(m_xtitle);
		data_hist->SetYTitle(Form("events/%.1f GeV",(data_hist->GetBinWidth(1))));
		data_hist->SetLabelSize(0., "X");

		data_hist->SetMarkerStyle(20);
		if(m_Rebin) data_hist->Rebin(m_rebin_factor);
                h2_style(data_hist);
		m_pad1.get()->cd();
		data_hist->SetName("full_data");
		data_hist->Draw("EP");
		m_canv.get()->cd();

		if(mc_hist){
			if(m_Rebin)mc_hist->Rebin(m_rebin_factor);
			mc_hist->SetLineColor(4);
			mc_hist->SetLineWidth(2);
			m_pad1.get()->cd();
			mc_hist->SetName("full_mc");
			mc_hist->Draw("HIST SAME");
			data_hist->Draw("EP SAME");
			m_canv.get()->cd();

		}
	}else if(mc_hist){
		        //h2_style(mc_hist);
			//mc_hist->SetXTitle(m_xtitle);
			mc_hist->SetYTitle(Form("events/%.1f GeV",(mc_hist->GetBinWidth(1))));
			mc_hist->SetLabelSize(0.,"X");
			if(m_Rebin)mc_hist->Rebin(m_rebin_factor);
			h2_style(mc_hist);
			mc_hist->SetLineColor(4);
			mc_hist->SetLineWidth(2);
			mc_hist->SetName("full_mc");
			m_pad1.get()->cd();
			mc_hist->Draw();
			m_canv.get()->cd();


	}


	TH1D *h_ratio;

	if(data_hist && mc_hist){

		h_ratio=(TH1D*)data_hist->Clone();
		h_ratio->Divide(mc_hist);
		h_ratio->SetMarkerStyle(20);
		h_ratio->SetXTitle(m_xtitle);
		h_ratio->SetYTitle("data/MC");
		m_pad2.get()->cd();
		m_pad2.get()->SetGrid();
		h_ratio->Draw("EP");
                ratio_style(h_ratio);
		m_pad2.get()->Update();
		m_canv.get()->cd();
	}

	TLegend *leg=new TLegend(0.1,0.7,0.3,0.9);
	leg->SetBorderSize(0);
	if(data_hist) leg->AddEntry(data_hist,"Data","pe");
	if(mc_hist) leg->AddEntry(mc_hist,"MC","l");

	m_pad1.get()->cd();
	leg->Draw();

}

void Plotter::makeDataStackedMCPlot(std::vector<TString>& hist_names,TString data_hist_name){

                HistogramStack stack=readInHistStack(hist_names,data_hist_name);
  
		TH1* data_hist=(TH1*)stack.full_data_hist.get()->Clone();

		TH1* full_mc=(TH1*)stack.full_mc_hist.get()->Clone();
		
		TH1* tmp_stacked_mc=0;

		std::vector<std::shared_ptr<TH1>> mc_stack_hists=stack.mc_stack_hists;

		m_canv.get()->cd();

		//prepare Legend
		TLegend *leg=new TLegend(0.1,0.7,0.2,0.9);
                leg->SetBorderSize(0);
                if(data_hist) leg->AddEntry(data_hist,"Data","pe");

                //TString name_arr[]={"BB","BC","BL","CB","CC","CL","LB","LC","LL"};
		TString name_arr[]={"BB","BL","CC","CL","LL"};

		//make THStack BB,BL,CC,CL,LL

		THStack *mystack=new THStack("myStack","stack");

		for(int i_hist=0; i_hist<mc_stack_hists.size(); i_hist++){

		  //std::cout<<"Color for category "<<name_arr[i_hist]<<" "<<this->getColor(m_stacked_hist_names[i_hist])<<std::endl;

		        if(!(mc_stack_hists[i_hist].get())) continue;
			
			tmp_stacked_mc=(TH1*)(mc_stack_hists[i_hist].get()->Clone());
			
			if(m_Rebin) tmp_stacked_mc->Rebin(m_rebin_factor);

			tmp_stacked_mc->SetFillColor(this->getColor(m_stacked_hist_names[i_hist]));
			mystack->Add(tmp_stacked_mc);
			leg->AddEntry(tmp_stacked_mc,name_arr[i_hist],"f");
		}


		if(data_hist){
		        //h2_style(data_hist);
			//data_hist->SetXTitle(m_xtitle);
			data_hist->SetLabelSize(0,"X");
			data_hist->SetYTitle(Form("events/%.1f",(data_hist->GetBinWidth(1))));
			data_hist->SetMarkerStyle(20);
			if(m_Rebin)data_hist->Rebin(m_rebin_factor);
			h2_style(data_hist);
			m_pad1.get()->cd();
			data_hist->Draw("EP");


			if(mystack){
			        if(m_Rebin) full_mc->Rebin(m_rebin_factor);
				m_pad1.get()->cd();
				mystack->Draw("HIST SAME");
				
				//draw full mc for errors
				full_mc->SetMarkerStyle(20);
				full_mc->SetMarkerColor(kGray+1);
				full_mc->SetFillColor(kGray+1);
				full_mc->SetFillStyle(3001);
				full_mc->Draw("E2 SAME");

				
				data_hist->Draw("EP SAME");

			}
		}else if(mystack){

			        //h2_style(mystack->GetHistogram());
				if(m_Rebin) full_mc->Rebin(m_rebin_factor);
				h2_style(mystack->GetHistogram());
				mystack->GetXaxis()->SetTitle(m_xtitle);
				mystack->GetYaxis()->SetTitle(Form("events/%.1f ",(full_mc->GetBinWidth(1))));

				m_pad1.get()->cd();
				mystack->Draw();

				//draw full mc for errors                                                                                                                                                                                    
				full_mc->SetMarkerStyle(20);
				full_mc->SetMarkerColor(kGray+1);
				full_mc->SetFillColor(kGray+1);
				full_mc->SetFillStyle(3001);
				full_mc->Draw("E3 SAME");

		}


		TH1 *h_ratio;

		TString text="data/MC N/A";
		TString text_ks="KS N/A";
		TString text_Chi2="Chi2/NDF N/A";

		if(data_hist && mystack){

			h_ratio=(TH1*)data_hist->Clone();
			h_ratio->Divide(full_mc);
			h_ratio->SetMarkerStyle(20);
			h_ratio->SetXTitle(m_xtitle);
			h_ratio->SetYTitle("data/MC");
			m_pad2.get()->SetGrid();
			m_pad2.get()->cd();
			h_ratio->Draw("EP");
                        ratio_style(h_ratio);

			double data_integral, data_error, mc_integral, mc_error;
		 
			data_integral=data_hist->IntegralAndError(1,data_hist->GetNbinsX(),data_error);
			mc_integral=full_mc->IntegralAndError(1,full_mc->GetNbinsX(),mc_error);

			double data_mc_ratio=data_integral/mc_integral;
			double data_mc_ratio_err=TMath::Sqrt((data_error/mc_integral)*(data_error/mc_integral)+(data_mc_ratio*mc_error/mc_integral)*(data_mc_ratio*mc_error/mc_integral));

			text=Form("data/mc = %f #pm %f",data_mc_ratio,data_mc_ratio_err);

			double ks=full_mc->KolmogorovTest(data_hist);

			text_ks=Form("KS = %f ",ks);

			double chi2=data_hist->Chi2Test(full_mc,"UW CHI2/NDF");
			text_Chi2=Form("Chi2/NDF = %f",chi2);

		}


		//Systematics shades if needed

		if(m_doSys){

			std::cout<<"!!!!!!!!DO SYST SHADES!!!!!!!!!!"<<std::endl;
			this->addSysShades(full_mc);

		}


		//Draw legend
		m_pad1.get()->cd();
		leg->Draw();


		//Draw data/mc
		TLatex latex;
		latex.SetNDC();
		latex.SetTextAlign(12);
		latex.SetTextSize(0.04);

		latex.DrawLatex(0.6,0.65,text.Data());
		
		//draw ks
		TLatex latex1;
		latex1.SetNDC();
		latex1.SetTextAlign(12);
                latex1.SetTextSize(0.04);

		latex1.DrawLatex(0.6,0.6,text_ks.Data());
		

		//draw chi2

		TLatex latex2;
                latex2.SetNDC();
                latex2.SetTextAlign(12);
                latex2.SetTextSize(0.04);

                latex2.DrawLatex(0.6,0.55,text_Chi2.Data());

}


void Plotter::makeDataMCBtagEffPlot(){

	TString pretag_hist_name="fatjet_BB_pt_POSTFIT";
	TString pretag_data_hist_name="fatjet_other_pt";

	std::vector<TString> pretag_hist_names={"fatjet_BB_pt_POSTFIT","fatjet_BL_pt_POSTFIT","fatjet_CC_pt_POSTFIT","fatjet_CL_pt_POSTFIT","fatjet_LL_pt_POSTFIT"};

	


        HistogramStack pretag_stack=readInHistStack(pretag_hist_names,pretag_data_hist_name);
	
        TH1* pretag_mc_hist=(TH1*)pretag_stack.full_mc_hist.get()->Clone();

	std::vector<TString> posttag_hist_names={"fatjet_BB_pt_POSTFITPOSTTAG","fatjet_BL_pt_POSTFITPOSTTAG","fatjet_CC_pt_POSTFITPOSTTAG","fatjet_CL_pt_POSTFITPOSTTAG","fatjet_LL_pt_POSTFITPOSTTAG"};
        TString posttag_data_hist_name="fatjet_other_pt_POSTTAG";

	HistogramStack posttag_stack=readInHistStack(posttag_hist_names,posttag_data_hist_name);

	std::vector<std::shared_ptr<TH1>> posttag_mc_stack_hists=posttag_stack.mc_stack_hists;

	TH1* posttag_data_hist=(TH1*)posttag_stack.full_data_hist.get()->Clone();
	TH1* pretag_data_hist=(TH1*)pretag_stack.full_data_hist.get()->Clone();


	//Make MC efficiency plot
	
	//TH1* posttag_mc_hist=(TH1*)posttag_mc_stack_hists[0].get()->Clone();
	TH1* posttag_mc_hist=(TH1*)posttag_stack.full_mc_hist.get()->Clone();
	posttag_mc_hist->Divide(pretag_mc_hist);
	

	//make data rate plot: data tagged/data+all

	posttag_data_hist->Divide(pretag_data_hist);

	//set styles and draw
	h2_style(posttag_data_hist);
	posttag_data_hist->SetLabelSize(0,"X");
	posttag_data_hist->SetYTitle("Double b-tagging rate");
	posttag_data_hist->SetMarkerStyle(20);
	m_pad1.get()->cd();
	posttag_data_hist->Draw("EP");

	posttag_mc_hist->SetMarkerColor(2);
	m_pad1.get()->cd();
	posttag_mc_hist->Draw("SAME");

	
	//Plot ratio
	TH1 *h_ratio;

	h_ratio=(TH1*)posttag_data_hist->Clone();
	h_ratio->Divide(posttag_mc_hist);
	h_ratio->SetMarkerStyle(20);
	h_ratio->SetXTitle(m_xtitle);
	h_ratio->SetYTitle("data/MC");
	m_pad2.get()->SetGrid();
	m_pad2.get()->cd();
	h_ratio->Draw("EP");
	ratio_style(h_ratio);

	


}

void Plotter::addSysShades(TH1* nominal_hist){

	std::vector<TString> sys_names_up, sys_names_down;

	int n_bins=nominal_hist->GetNbinsX();
	double errors_up[n_bins],errors_down[n_bins],fiterrors_up[n_bins],fiterrors_down[n_bins],x_values[n_bins],y_values[n_bins],x_error_up[n_bins],x_error_down[n_bins];


	for(int i_sys=0; i_sys<m_SysNames.size(); i_sys++){

		sys_names_up.clear();
		sys_names_down.clear();


		for(int i_hist=0; i_hist<m_stacked_hist_names.size(); i_hist++){
			
			sys_names_up.push_back(m_stacked_hist_names[i_hist]+"_"+m_SysNames[i_sys]+"UP");
			sys_names_down.push_back(m_stacked_hist_names[i_hist]+"_"+m_SysNames[i_sys]+"DOWN");
			
		}
		
		HistogramStack up_stack=readInHistStack(sys_names_up,"");
		HistogramStack down_stack=readInHistStack(sys_names_down,"");

		TH1* h_up=(TH1*)up_stack.full_mc_hist.get()->Clone();
		TH1* h_down=(TH1*)down_stack.full_mc_hist.get()->Clone();
		TH1* h_nom=(TH1*)nominal_hist->Clone();
		
		//rebin up and down histogram if necessary (nominal is already rebinned)
		if(m_Rebin){
			h_up->Rebin(m_rebin_factor);
			h_down->Rebin(m_rebin_factor);
		}

		h_up->Add(nominal_hist,-1);
		h_nom->Add(h_down,-1);
		
		if(m_SysNames[i_sys].Contains("FIT")){
			for(int i=1; i<=nominal_hist->GetNbinsX(); i++){

				if(nominal_hist->GetBinContent(i))fiterrors_up[i-1]=TMath::Abs(h_up->GetBinContent(i)/nominal_hist->GetBinContent(i));
				else fiterrors_up[i-1]=0;

				if(nominal_hist->GetBinContent(i))fiterrors_down[i-1]=TMath::Abs(h_nom->GetBinContent(i)/nominal_hist->GetBinContent(i));
				else fiterrors_down[i-1]=0;
				std::cout<<"Fit errors up: "<<fiterrors_up[i-1]<<std::endl;
				std::cout<<"Fit errors down: "<<fiterrors_down[i-1]<<std::endl;

			}
		}
		
		

		for(int i=1; i<=nominal_hist->GetNbinsX(); i++){
			if(i_sys==0 && i==1){
				errors_up[i-1]=0;
				errors_down[i-1]=0;
			}
			
			errors_up[i-1]=errors_up[i-1]+h_up->GetBinContent(i)*h_up->GetBinContent(i);
			errors_down[i-1]=errors_down[i-1]+h_down->GetBinContent(i)*h_down->GetBinContent(i);
			
		}
		
	}

	for(int i=1; i<=nominal_hist->GetNbinsX(); i++){
		x_values[i-1]=nominal_hist->GetBinCenter(i);
		x_error_up[i-1]=nominal_hist->GetBinWidth(i)/2;
		x_error_down[i-1]=nominal_hist->GetBinWidth(i)/2;
		y_values[i-1]=1.;
	}

	TLegend *leg=new TLegend(0.5,0.75,0.88,0.88);
        leg->SetBorderSize(0);


	if(m_SysNames.size()>1){

		for(int i=1; i<=nominal_hist->GetNbinsX(); i++){
			errors_up[i-1]=TMath::Sqrt(errors_up[i-1])/nominal_hist->GetBinContent(i);
			errors_down[i-1]=TMath::Sqrt(errors_down[i-1])/nominal_hist->GetBinContent(i);
		}

		TGraphAsymmErrors g_errors(n_bins,x_values,y_values,x_error_down,x_error_up,errors_down,errors_up);
		g_errors.SetFillStyle(3005);
		std::cout<<"Graph point 2 error high: "<<g_errors.GetErrorYhigh(2)<<std::endl;

		m_pad2.get()->cd();
		g_errors.Draw("p2");
    	

	}

	TGraphAsymmErrors *g_fiterrors= new TGraphAsymmErrors(n_bins,x_values,y_values,x_error_down,x_error_up,fiterrors_down,fiterrors_up);
	
	g_fiterrors->SetFillColor(kGreen+3);
	g_fiterrors->SetFillStyle(3018);
	std::cout<<"Graph point 2 error high: "<<g_fiterrors->GetErrorYlow(10)<<std::endl;


	m_pad2.get()->cd();

	g_fiterrors->Draw("2");

	leg->AddEntry(g_fiterrors,"Fit Uncertainty","f");
	
	m_pad2.get()->cd();
	leg->Draw();

}

void Plotter::makeComparisonPlot(std::vector<TString>& hist_names, bool doNormalized){

  MultiHistogramStack stack=readInMultiHistStack(hist_names,hist_names);

  TH1* tmp_stacked=0;

  std::vector<std::shared_ptr<TH1>> stack_hists=stack.mc_stack_hists;
  if(std::all_of(stack_hists.begin(),stack_hists.end(),[](std::shared_ptr<TH1> ptr){return !ptr;})) stack_hists=stack.data_stack_hists; //if all entries are zero, try data
  if(std::all_of(stack_hists.begin(),stack_hists.end(),[](std::shared_ptr<TH1> ptr){return !ptr;})) return; //if no histograms exit

  std::cout<<"Comparison plot of: "<<stack_hists.size()<<"Plots!"<<std::endl;

  m_canv.get()->cd();

  //prepare Legend                                                                                                                                                                                                                                               
  TLegend *leg=new TLegend(0.1,0.7,0.2,0.9);
  leg->SetBorderSize(0);

  //TString name_arr[]={"BB","BC","BL","CB","CC","CL","LB","LC","LL"};
  TString name_arr[]={"BB","BL","CC","CL","LL"};

  //make THStack BB,BL,CC,CL,LL                                                                                                                                                                                                                      

  THStack *mystack=new THStack("myStack","stack");

  for(int i_hist=0; i_hist<stack_hists.size(); i_hist++){
    if(!(stack_hists[i_hist].get())) continue;

    tmp_stacked=(TH1*)(stack_hists[i_hist].get()->Clone());

    if(m_Rebin) tmp_stacked->Rebin(m_rebin_factor);
    if(doNormalized) tmp_stacked->Scale(1./tmp_stacked->Integral());
    tmp_stacked->SetMarkerStyle(21);
    tmp_stacked->SetMarkerSize(1);
    tmp_stacked->SetLineColor(this->getColor(hist_names[i_hist]));
    mystack->Add(tmp_stacked,"hist");
    leg->AddEntry(tmp_stacked,name_arr[i_hist],"l");
  }


  if(mystack){

    m_canv.get()->cd();
    mystack->SetTitle("");
    
    m_pad1.get()->SetPad(0.,0.,1.,1.);
    m_pad1.get()->Draw();
    
    m_pad1.get()->cd();

    mystack->Draw("NOSTACK");
    
    h2_style(mystack->GetHistogram());
    
    mystack->GetXaxis()->SetTitle(m_xtitle);
    if(doNormalized) mystack->GetYaxis()->SetTitle("normalized to unity");
    else mystack->GetYaxis()->SetTitle(Form("events/%.1f GeV",(tmp_stacked->GetBinWidth(1))));

  
  }

  m_pad1.get()->cd();
  leg->Draw();

}


void Plotter::makeReweightHisto(TString hist_name,TFile *outfile){
  
  
  m_hist_name=hist_name; //TODO: make this better                                                                                                                                                                                                                        

  TH2D* data_hist=0, *mc_hist=0, *tmp=0;

  TString filename, sample_name;

  TFile *f;

  bool haveDataHist=false, haveMCHist=false;

  float weight=0.;

  for(int i_samp=0; i_samp<m_samples.size(); i_samp++){

    filename=m_samples.at(i_samp);
    sample_name=filename(filename.Last('/')+1,filename.Length());
    filename="/merged_hist_"+sample_name;

    std::cout<<"Open file: "<<m_samples.at(i_samp)+filename<<std::endl;

    f=TFile::Open((m_samples.at(i_samp)+filename).Data(),"READ");

    if(!haveDataHist && m_samples.at(i_samp).Contains("data15")){
      
      data_hist=0;
      f->GetObject(m_hist_name,data_hist);
      if(!data_hist) continue;

      haveDataHist=true;
      data_hist=(TH2D*)data_hist->Clone();
      data_hist->SetDirectory(0);
      data_hist->Sumw2();

    }else if(!haveMCHist && m_samples.at(i_samp).Contains("mc15")){

      mc_hist=0;
      f->GetObject(m_hist_name,mc_hist);
      if(!mc_hist) continue;

      weight=this->getWeightSample(m_samples.at(i_samp)+filename);
      if(weight>10000.) continue;

      haveMCHist=true;
      mc_hist=(TH2D*)mc_hist->Clone();
      mc_hist->SetDirectory(0);
      mc_hist->Sumw2();
      mc_hist->Scale(weight);

    }else{

      tmp=0;
      f->GetObject(m_hist_name,tmp);
      if(!tmp) continue;
      tmp->Sumw2();

      if(m_samples.at(i_samp).Contains("data15")) data_hist->Add(tmp);
      else{
	weight=this->getWeightSample(m_samples.at(i_samp)+filename);
	if(weight>10000.) continue;

	mc_hist->Add(tmp,weight);
      }
    }

    f->Close();
  }


  if(data_hist && mc_hist){
    
    data_hist->Divide(mc_hist);

  }

  outfile->cd();
  
  if(data_hist) data_hist->Write();
  else std::cout<<"!!!!!!!!!!!!!!!!!!!!!!!!! Have no data hist "<< hist_name<<"!!!!!!!!!!!!!!!!!!!!"<<std::endl;

}

void Plotter::printTemplateCorrelations(std::vector<TString>& hist_names, TFile *outfile ){
	MultiHistogramStack stack=readInMultiHistStack(hist_names,hist_names);
	
	TH2D* tmp_stacked=0;
	
	std::vector<std::shared_ptr<TH1>> stack_hists=stack.mc_stack_hists;

	if(std::all_of(stack_hists.begin(),stack_hists.end(),[](std::shared_ptr<TH1> ptr){return !ptr;})) return; //if no histograms exit          
	std::cout<<"Correlation between "<<stack_hists.size()<<"Plots!"<<std::endl;

	std::cout<<"=================================="<<std::endl;
	std::cout<<"|Correlation Factors: "<<std::endl;
	std::cout<<"|"<<std::endl;

	TObjArray* tokens;

	for(int i=0; i<stack_hists.size(); i++){

                tokens=hist_names[i].Tokenize('_');
	       
		if(!(i%5)){

			if(i!=0)std::cout<<"\\end{tabular}"<<std::endl;
			std::cout<<std::endl;
			std::cout<<std::endl;

			std::cout<<"\\begin{tabular}{ |c|c| }"<<std::endl;
			std::cout<<"\\hline"<<std::endl;
			if(tokens->GetLast()==8)std::cout<<"\\multicolumn{2}{|c|}{"<<((TObjString*) (*tokens)[4])->GetString()+" "+((TObjString*) (*tokens)[5])->GetString()+" "+((TObjString*) (*tokens)[6])->GetString()+" "+((TObjString*) (*tokens)[7])->GetString()<<"} \\\\ "<<std::endl;
			else if(tokens->GetLast()<8)std::cout<<"\\multicolumn{2}{|c|}{"<<((TObjString*) (*tokens)[4])->GetString()<<"} \\\\ "<<std::endl;

			std::cout<<"\\hline"<<std::endl;
		}


		tmp_stacked=(TH2D*)(stack_hists[i].get())->Clone();
		tmp_stacked->SetDirectory(0);

		double mean_x, mean_y, rms_x, rms_y, weight, weight_error, sum_corr=0., sum_weight=0., val_x, val_y, corr, corr_err;
		double tmp_corr, sumweight_largecorr=0, denom_x=0, denom_y=0;
				
		//calculate Correlation
		for(int bin_x=1; bin_x<=tmp_stacked->GetNbinsX(); bin_x++ ){
			for(int bin_y=1; bin_y<=tmp_stacked->GetNbinsY(); bin_y++){

				weight=tmp_stacked->GetBinContent(bin_x,bin_y);
				weight_error=tmp_stacked->GetBinError(bin_x,bin_y);
				
								
				mean_x=tmp_stacked->GetMean(1);
				mean_y=tmp_stacked->GetMean(2);
			        rms_x=tmp_stacked->GetRMS(1);
				rms_y=tmp_stacked->GetRMS(2);
				
				val_x=tmp_stacked->GetXaxis()->GetBinCenter(bin_x);
				val_y=tmp_stacked->GetYaxis()->GetBinCenter(bin_y);

				tmp_corr=(val_x-mean_x)*(val_y-mean_y);
                                sum_corr+=weight*tmp_corr;
				denom_x+=weight*(val_x-mean_x)*(val_x-mean_x);
                                denom_y+=weight*(val_y-mean_y)*(val_y-mean_y);
				
				sum_weight+=weight;
				
				if(TMath::Abs(tmp_corr)>1.){
					sumweight_largecorr+=weight;
				}

			}
		}


		corr=sum_corr/TMath::Sqrt(denom_x*denom_y);
		corr_err=TMath::Sqrt((1-corr*corr)/(sum_weight-2));

		//std::cout<<"Fraction of correlations |corr| larger than 1:"<<sumweight_largecorr/sum_weight<<std::endl;

	        std::cout<<((TObjString*) (*tokens)[3])->GetString()<<" & "<<Form("%4.1f",corr*100.)<<"\\pm "<<Form("%4.1f",corr_err*100)<<" \\%  \\\\ "<<std::endl;//<<"frac Underflow:"<<corr_hist.GetBinContent(0)/sum_weight<<" frac Overflow: "<<corr_hist.GetBinContent(corr_hist.GetNbinsX()+1)/sum_weight<<std::endl; 
		std::cout<<"\\hline"<<std::endl;

		if(outfile){
			outfile->cd();
			tmp_stacked->Write();
		}

	}
	
	std::cout<<"=================================="<<std::endl;

}




void Plotter::makeFitTemplates(std::vector<TString>& hist_names,std::vector<TString>& data_hist_names,TFile *outfile,TString sys_ext){

  MultiHistogramStack stack=readInMultiHistStack(hist_names,data_hist_names);

  TH1* tmp_stacked_mc=0, *tmp_stacked_data=0;
  
  TH1F th1f_stacked_mc, th1f_stacked_data;

  std::vector<std::shared_ptr<TH1>> mc_stack_hists=stack.mc_stack_hists;
  std::vector<std::shared_ptr<TH1>> data_stack_hists=stack.data_stack_hists;


  //rename and write to output file
  //TODO: add functionality for systematics histograms
  
  TString hist_name,new_hist_name;
  TObjArray* tokens;

  if(sys_ext.EqualTo("Nom")){ //write data only for nominal
    
    for(int i=0;i<data_stack_hists.size(); i++){

      if(!data_stack_hists[i].get()) continue;
      tmp_stacked_data=(TH1*)data_stack_hists[i].get()->Clone();
      
      hist_name=tmp_stacked_data->GetName();
      
      tokens=hist_name.Tokenize('_');
      
      if(tokens->GetLast()>=6) new_hist_name="hData"+sys_ext+"_"+((TObjString*) (*tokens)[0])->GetString()+"_"+((TObjString*) (*tokens)[2])->GetString()+"_"+((TObjString*) (*tokens)[3])->GetString()+"_"+((TObjString*) (*tokens)[4])->GetString()+"_"+((TObjString*) (*tokens)[5])->GetString()+"_obs_cuts";
      else new_hist_name="hData"+sys_ext+"_"+((TObjString*) (*tokens)[0])->GetString()+"_inclusive_obs_cuts";
      
      tmp_stacked_data->SetName(new_hist_name.Data());
      tmp_stacked_data->Copy(th1f_stacked_data);
      
      outfile->cd();
      th1f_stacked_data.Write();
  

    }

  }

  for(int i=0; i<mc_stack_hists.size(); i++){
    
    if(!mc_stack_hists[i].get()) continue;
    tmp_stacked_mc=(TH1*)mc_stack_hists[i].get()->Clone();
    
    hist_name=tmp_stacked_mc->GetName();
    tokens=hist_name.Tokenize('_');
    if(tokens->GetLast()>=6) new_hist_name="h"+((TObjString*) (*tokens)[1])->GetString()+sys_ext+"_"+((TObjString*) (*tokens)[0])->GetString()+"_"+((TObjString*) (*tokens)[2])->GetString()+"_"+((TObjString*) (*tokens)[3])->GetString()+"_"+((TObjString*) (*tokens)[4])->GetString()+"_"+((TObjString*) (*tokens)[5])->GetString()+"_obs_cuts";
    else new_hist_name="h"+((TObjString*) (*tokens)[1])->GetString()+sys_ext+"_"+((TObjString*) (*tokens)[0])->GetString()+"_inclusive_obs_cuts";

    tmp_stacked_mc->SetName(new_hist_name);

    tmp_stacked_mc->Copy(th1f_stacked_mc);
    
    outfile->cd();
    th1f_stacked_mc.Write();

  }

}


HistogramStack Plotter::readInHistStack(std::vector<TString>& hist_names,TString data_hist_name){

  //Read in stack of multiple MC histograms and 1 full data histogram

  m_stacked_hist_names=hist_names; //TODO: make this better                                                                                                                                                                                                                    

  TH1* data_hist=0, *tmp=0, *full_mc=0, *mc_stack_tmp=0;

  std::vector<std::shared_ptr<TH1>> mc_stack_hists(m_stacked_hist_names.size(),0);

  TString filename, sample_name;

  bool haveDataHist=false, haveFullMCHist=false;

  float weight=0.;

  TFile *f;

  for(int i_samp=0; i_samp<m_samples.size(); i_samp++){

    filename=m_samples.at(i_samp);
    sample_name=filename(filename.Last('/')+1,filename.Length());
    filename="/merged_hist_"+sample_name;

    std::cout<<"Open file: "<<m_samples.at(i_samp)+filename<<std::endl;

    f=TFile::Open((m_samples.at(i_samp)+filename).Data(),"READ");

    if(!haveDataHist && m_samples.at(i_samp).Contains("data15")){
      f->GetObject(data_hist_name,data_hist);

      if(!data_hist) continue;
      haveDataHist=true;

      data_hist=(TH1*)data_hist->Clone();
      data_hist->SetDirectory(0);
      data_hist->Sumw2();

    }else{

      if(m_samples.at(i_samp).Contains("data15")){


	f->GetObject(data_hist_name,tmp);
        if(!tmp) continue;
        tmp=(TH1D*)tmp->Clone();

        data_hist->Add(tmp);
      }else{

        weight=this->getWeightSample(m_samples.at(i_samp)+filename);
        if(weight>10000.) continue;

        for(int i_hist=0; i_hist<m_stacked_hist_names.size(); i_hist++){

          f->GetObject(m_stacked_hist_names.at(i_hist),tmp);
          if(!tmp) continue;


          if(!haveFullMCHist){ //first overall MC histogram                                                                                                                                                                                                                    
            full_mc=(TH1*)tmp->Clone();
            full_mc->SetDirectory(0);
            full_mc->Sumw2();
            full_mc->Scale(weight);
            haveFullMCHist=true;
          }else full_mc->Add(tmp,weight);

          if(mc_stack_hists[i_hist]==0){ //first histogram in this category                                                                                                                                                                                                    
	    mc_stack_tmp=(TH1*)tmp->Clone();
	    mc_stack_tmp->SetDirectory(0);
	    mc_stack_hists[i_hist]=std::shared_ptr<TH1>(mc_stack_tmp);
	    mc_stack_hists[i_hist].get()->Sumw2();
            mc_stack_hists[i_hist].get()->Scale(weight);

          }else{
            mc_stack_hists[i_hist].get()->Add(tmp,weight);
          }


        }

      }
    }


    f->Close();
  }

  HistogramStack stack;

  std::shared_ptr<TH1> sp_full_mc(full_mc);
  std::shared_ptr<TH1> sp_data_hist(data_hist);
 
  if(sp_data_hist) stack.full_data_hist=sp_data_hist;
  else stack.full_data_hist=0;

  if(sp_full_mc)stack.full_mc_hist=sp_full_mc;
  else stack.full_mc_hist=0;

  stack.mc_stack_hists=mc_stack_hists;

  return stack;

}


MultiHistogramStack Plotter::readInMultiHistStack(std::vector<TString>& hist_names,std::vector<TString>& data_names){
  
  //Read in Stack of multiple data histograms and multiple MC histograms (for Templates)

  m_stacked_hist_names=hist_names; //TODO: make this better                                                                                                                                                                                                                  
                                                                                                                                                                                                                                                                           

  TH1* data_hist=0, *tmp=0, *full_mc=0, *mc_stack_tmp=0, *data_stack_tmp=0;

  std::vector<std::shared_ptr<TH1>> mc_stack_hists(m_stacked_hist_names.size(),0);
  std::vector<std::shared_ptr<TH1>> data_stack_hists(data_names.size(),0);

  TString filename, sample_name;

  bool haveDataHist=false, haveFullMCHist=false;

  float weight=0.;

  TFile *f;

  for(int i_samp=0; i_samp<m_samples.size(); i_samp++){

    filename=m_samples.at(i_samp);
    sample_name=filename(filename.Last('/')+1,filename.Length());
    filename="/merged_hist_"+sample_name;

    std::cout<<"Open file: "<<m_samples.at(i_samp)+filename<<std::endl;

    f=TFile::Open((m_samples.at(i_samp)+filename).Data(),"READ");
    
      if(m_samples.at(i_samp).Contains("data15")){

	
	for(int i_hist=0; i_hist<data_names.size(); i_hist++){

	  f->GetObject(data_names.at(i_hist),tmp);
          if(!tmp) continue;
	 
	  if(data_stack_hists[i_hist]==0){ //first histogram in this category       
	    data_stack_tmp=(TH1*)tmp->Clone();
	    data_stack_tmp->SetDirectory(0);
	    data_stack_hists[i_hist]=std::shared_ptr<TH1>(data_stack_tmp);
	  }else data_stack_hists[i_hist].get()->Add(tmp);

	}
        
      }else{

        weight=this->getWeightSample(m_samples.at(i_samp)+filename);
	std::cout<<"Weight is "<<weight<<std::endl;
	if(weight>10000.) continue;

        for(int i_hist=0; i_hist<m_stacked_hist_names.size(); i_hist++){

          f->GetObject(m_stacked_hist_names.at(i_hist),tmp);
          if(!tmp) continue;
          
          if(mc_stack_hists[i_hist]==0){ //first histogram in this category  
            
		 
            mc_stack_tmp=(TH1*)tmp->Clone();
	    if((mc_stack_tmp->GetBinContent(mc_stack_tmp->GetMaximumBin())/mc_stack_tmp->GetBinError(mc_stack_tmp->GetMaximumBin()))<3 && mc_stack_tmp->GetBinContent(mc_stack_tmp->GetMaximumBin())*weight>10) std::cout<<"Have large-weight histogram "<<mc_stack_tmp->GetName()<<" in "<<m_samples.at(i_samp)+filename<<" largest bin content is"<<mc_stack_tmp->GetBinContent(mc_stack_tmp->GetMaximumBin())<<" +- "<<mc_stack_tmp->GetBinError(mc_stack_tmp->GetMaximumBin())<<std::endl;
	    mc_stack_tmp->SetDirectory(0);
            mc_stack_hists[i_hist]=std::shared_ptr<TH1>(mc_stack_tmp);
            mc_stack_hists[i_hist].get()->Sumw2();
            mc_stack_hists[i_hist].get()->Scale(weight);
	   

          }else{
		  
		  if((tmp->GetBinContent(tmp->GetMaximumBin())/tmp->GetBinError(tmp->GetMaximumBin()))<3 && tmp->GetBinContent(tmp->GetMaximumBin())*weight>10) std::cout<<"Have large-weight histogram "<<tmp->GetName()<<" in "<<m_samples.at(i_samp)+filename<<" largest bin content is"<<tmp->GetBinContent(tmp->GetMaximumBin())<<" +- "<<tmp->GetBinError(tmp->GetMaximumBin())<<std::endl;

		  
            mc_stack_hists[i_hist].get()->Add(tmp,weight);
          }


        }

      } 
    


    f->Close();
  }

  MultiHistogramStack stack;

  stack.mc_stack_hists=mc_stack_hists;
  stack.data_stack_hists=data_stack_hists;

  return stack;

}








ColorStack Plotter::getColor(TString hist_name){

  if(hist_name.Contains("_BB_") || hist_name.Contains("_B_")) return BB;
  else if(hist_name.Contains("_BC_")) return BC;
  else if(hist_name.Contains("_BL_")) return BL;
  else if(hist_name.Contains("_CB_")) return CB;
  else if(hist_name.Contains("_CC_")) return CC;
  else if(hist_name.Contains("_CL_") || hist_name.Contains("_C_")) return CL;
  else if(hist_name.Contains("_LB_")) return LB;
  else if(hist_name.Contains("_LC_")) return LC;
  else if(hist_name.Contains("_LL_") || hist_name.Contains("_L_")) return LL;

 return def;
}


void Plotter::setAtlasStyle()
{

	TStyle *atlasStyle = new TStyle("ATLAS","Atlas style");

	// use plain black on white colors
	Int_t icol=0; // WHITE
	atlasStyle->SetFrameBorderMode(icol);
	atlasStyle->SetFrameFillColor(icol);
	atlasStyle->SetCanvasBorderMode(icol);
	atlasStyle->SetCanvasColor(icol);
	atlasStyle->SetPadBorderMode(icol);
	atlasStyle->SetPadColor(icol);
	atlasStyle->SetStatColor(icol);
	//atlasStyle->SetFillColor(icol); // don't use: white fill color for *all* objects

	// set the paper & margin sizes
	atlasStyle->SetPaperSize(20,26);

	// set margin sizes
	atlasStyle->SetPadTopMargin(0.05);
	atlasStyle->SetPadRightMargin(0.05);
	atlasStyle->SetPadBottomMargin(0.16);
	atlasStyle->SetPadLeftMargin(0.16);

	// set title offsets (for axis label)
	atlasStyle->SetTitleXOffset(1.4);
	atlasStyle->SetTitleYOffset(1.4);

	// use large fonts
	//Int_t font=72; // Helvetica italics
	Int_t font=42; // Helvetica
	Double_t tsize=0.05;
	atlasStyle->SetTextFont(font);

	atlasStyle->SetTextSize(tsize);
	atlasStyle->SetLabelFont(font,"x");
	atlasStyle->SetTitleFont(font,"x");
	atlasStyle->SetLabelFont(font,"y");
	atlasStyle->SetTitleFont(font,"y");
	atlasStyle->SetLabelFont(font,"z");
	atlasStyle->SetTitleFont(font,"z");

	atlasStyle->SetLabelSize(tsize,"x");
	atlasStyle->SetTitleSize(tsize,"x");
	atlasStyle->SetLabelSize(tsize,"y");
	atlasStyle->SetTitleSize(tsize,"y");
	atlasStyle->SetLabelSize(tsize,"z");
	atlasStyle->SetTitleSize(tsize,"z");

	// use bold lines and markers
	atlasStyle->SetMarkerStyle(20);
	atlasStyle->SetMarkerSize(1.2);
	atlasStyle->SetHistLineWidth(2.);
	atlasStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes

	// get rid of X error bars
	//atlasStyle->SetErrorX(0.001);
	// get rid of error bar caps
	atlasStyle->SetEndErrorSize(0.);

	// do not display any of the standard histogram decorations
	atlasStyle->SetOptTitle(0);
	//atlasStyle->SetOptStat(1111);
	atlasStyle->SetOptStat(0);
	//atlasStyle->SetOptFit(1111);
	atlasStyle->SetOptFit(0);

	// put tick marks on top and RHS of plots
	atlasStyle->SetPadTickX(1);
	atlasStyle->SetPadTickY(1);

	gROOT->SetStyle("ATLAS");
	gROOT->ForceStyle();


}

void Plotter::addLabel(int type){

	 TLatex *tex0 = new TLatex();
	  double lx = 0.65; double ly = 0.85;


	  if(type==0)
	    tex0= new TLatex(lx,ly,"#font[72]{ATLAS} Internal");
	  else if(type==1)
	    tex0= new TLatex(lx,ly,"#font[72]{ATLAS} Preliminary");
	  else if(type==2)
	    tex0= new TLatex(lx,ly,"#font[72]{ATLAS}");
	  else if(type==3)
		tex0= new TLatex(lx,ly,"#font[72]{ATLAS} Work In Progress");

	  tex0->SetNDC();
	  tex0->SetTextSize(0.05);
	  tex0->SetTextColor(1);
	  tex0->SetTextFont(42);


	  TLatex *tex1 = new TLatex();
	  lx=0.55;
	  ly=0.75;
	  tex1= new TLatex(lx,ly,"#sqrt{s}=13 TeV    #int L dt = 3.2 fb^{-1}");

	  tex1->SetNDC();
	  tex1->SetTextSize(0.05);
	  tex1->SetTextColor(1);
	  tex1->SetTextFont(42);




	  m_pad1.get()->cd();
	  tex0->Draw("same");
	  tex1->Draw("same");
}


/*int Plotter::CountNtups(TString in_dir){

  //count number of ntuples in sample directory (necessary for normalization)
  // in dir is sample directory

  int n_files=0;

  TSystemDirectory dir(in_dir, in_dir);
  TList *files = dir.GetListOfFiles();
  if (files) {
    TSystemFile *file;
    TString fname;
    TIter next(files);
    while ((file=(TSystemFile*)next())) {
      fname = file->GetName();
      if (!file->IsDirectory() && fname.EndsWith(".root") && !fname.Contains("merge")) {
	n_files++;
      }
    }
  }

  return n_files;

  }*/
