/*
 * HistPlotter.cxx
 *
 *  Created on: Feb 23, 2016
 *      Author: rjacobs
 */

#include "HistPlotter.h"
#include "HistogramHandler.h"
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
#include "TMatrix.h"
#include "TVector.h"


HistPlotter::HistPlotter() : m_doGrid(false), m_Rebin(false), m_lumi(1e6){
	// TODO Auto-generated constructor stub

}

HistPlotter::~HistPlotter() {
	// TODO Auto-generated destructor stub

  //delete m_pad1;
  // delete m_pad2;
  //delete m_canv;

  if(m_corrector) delete m_corrector;


}

HistPlotter::HistPlotter(TString in_dir, SampleInfo* info,TString name) : m_base_dir(in_dir),m_doGrid(false), m_Rebin(false), m_lumi(1e3),m_sample_info(info),m_showFitSys(false), m_showBtagSys(false),m_showTotalSys(false), m_corrector(0), m_doNormalized(false) {
	// TODO Auto-generated constructor stub

         TH1::AddDirectory(0);


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

}

void HistPlotter::prepareCanvas(){

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


void HistPlotter::h2_style(TH1 *h){
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


void HistPlotter::ratio_style(TH1 *h){
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

void HistPlotter::leg_style(TLegend *h){
  //  h->SetTextFont(mFont);
  h->SetTextSize(0.045);
  h->SetFillColor(0);
  h->SetFillStyle(0);
  h->SetLineColor(0);
  h->SetBorderSize(0);

}

void HistPlotter::makeDataMCPlot(TString hist_name){

        std::vector<TString> hist_names(1,hist_name);
       TString data_hist_name=hist_name;

	HistogramHandler hist_handler(m_base_dir+"Output_gbbTupleAna/",m_sample_info,m_lumi);

	HistogramStack stack=hist_handler.readInHistStack(hist_names,data_hist_name);

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

	TLegend *leg=new TLegend(0.6,0.3,0.8,0.5);
	leg->SetBorderSize(0);
	if(data_hist) leg->AddEntry(data_hist,"Data","pe");
	if(mc_hist) leg->AddEntry(mc_hist,"MC","l");

	m_pad1.get()->cd();
	leg->Draw();

}

void HistPlotter::makeDataStackedMCPlot(std::vector<TString>& hist_names,TString data_hist_name){
  
                HistogramHandler hist_handler(m_base_dir+"Output_gbbTupleAna/",m_sample_info,m_lumi);  

                HistogramStack stack=hist_handler.readInHistStack(hist_names,data_hist_name);
  
		TH1* data_hist=(TH1*)stack.full_data_hist.get()->Clone();

		TH1* full_mc=(TH1*)stack.full_mc_hist.get()->Clone();
		
		TH1* tmp_stacked_mc=0;

		std::vector<std::shared_ptr<TH1>> mc_stack_hists=stack.mc_stack_hists;

		m_canv.get()->cd();

		//prepare Legend
		TLegend *leg=new TLegend(0.68,0.3,0.88,0.7);
                leg->SetBorderSize(0);
		leg->SetFillStyle(0);
                if(data_hist) leg->AddEntry(data_hist,"Data","pe");

                //TString name_arr[]={"BB","BC","BL","CB","CC","CL","LB","LC","LL"};
		TString name_arr[]={"BB","BL","CC","CL","LL"};

		//make THStack BB,BL,CC,CL,LL

		THStack *mystack=new THStack("myStack","stack");

		for(int i_hist=0; i_hist<mc_stack_hists.size(); i_hist++){

		        if(!(mc_stack_hists[i_hist].get())) continue;
			
			tmp_stacked_mc=(TH1*)(mc_stack_hists[i_hist].get()->Clone());
			
			if(m_Rebin) tmp_stacked_mc->Rebin(m_rebin_factor);

			tmp_stacked_mc->SetFillColor(this->getColor(hist_names[i_hist]));
			
			if(m_doNormalized) tmp_stacked_mc->Scale(1./full_mc->Integral());

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

			if(m_doNormalized) data_hist->Scale(1./data_hist->Integral());
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
				//full_mc->Draw("E2 SAME");
				//leg->AddEntry(full_mc,"total MC","pe");
				
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
				//full_mc->Draw("E3 SAME");
				//leg->AddEntry(full_mc,"total MC","pf");

		}


		TH1 *h_ratio;

		TString text="data/MC N/A";
		TString text_ks="KS N/A";
		TString text_Chi2="Chi2/NDF N/A";

		if(data_hist && mystack){

		  if(m_doNormalized) full_mc->Scale(1./full_mc->Integral());

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
		
		TLegend *leg_sys_1=new TLegend(0.25,0.75,0.5,0.85);                                                                                                                                                                       \
		leg_sys_1->SetBorderSize(0);
		leg_sys_1->SetFillStyle(0);
		TLegend *leg_sys_2=new TLegend(0.25,0.65,0.5,0.75);                                                                                                                                                                       \
                leg_sys_2->SetBorderSize(0);
		leg_sys_2->SetFillStyle(0);
                TLegend *leg_sys_3=new TLegend(0.25,0.55,0.5,0.65);                                                                                                                                                                       \
                leg_sys_3->SetBorderSize(0);
		leg_sys_3->SetFillStyle(0);

		TGraphAsymmErrors* fitsys=0, *btagsys=0;

		if(m_showFitSys){
		  
		  fitsys=this->getFitSys(hist_names);
		  fitsys->SetFillColor(kGreen+1);
		  fitsys->SetFillStyle(3001);

		  m_pad2.get()->cd();
		  //fitsys->Draw("2");
		  leg_sys_1->AddEntry(fitsys,"Fit Uncertainty","f");

		}
		
		if(m_showBtagSys){
		  btagsys=this->getBTagSys(hist_names);
		  btagsys->SetLineColor(kRed);
		  btagsys->SetFillStyle(0);
		  m_pad2.get()->cd();
		  // btagsys->Draw("5");
		  
		  leg_sys_2->AddEntry(btagsys,"B-tagging Uncertainty","l");

		}

		if(m_showTotalSys){
		  
		  TGraphAsymmErrors* jetsys=this->getTotalJetSys(hist_names);
		  TGraphAsymmErrors* mcstat=this->getMCStat(full_mc);

		  TGraphAsymmErrors* totsys=getTotalSys(fitsys,btagsys,jetsys,mcstat);
		  totsys->SetFillColor(kBlack);
		  totsys->SetFillStyle(3005);
		  totsys->SetLineColor(kBlack);
		  m_pad2.get()->cd();
		  totsys->Draw("5");

		  leg_sys_3->AddEntry(totsys,"Total Uncertainty","f");

		}

		m_pad2.get()->cd();
		if(m_showFitSys) fitsys->Draw("2");
                if(m_showBtagSys) btagsys->Draw("5");
		
		m_pad2.get()->cd();
		h_ratio->Draw("same");
		if(m_showFitSys) leg_sys_1->Draw();
                if(m_showBtagSys) leg_sys_2->Draw();
                if(m_showTotalSys) leg_sys_3->Draw();

		//Draw legend
		m_pad1.get()->cd();
		leg->Draw();


		//Draw data/mc
		TLatex latex;
		latex.SetNDC();
		latex.SetTextAlign(12);
		latex.SetTextSize(0.04);

		if(data_hist_name.Contains("maxSd0")) latex.DrawLatex(0.6,0.65,text.Data());
		
		//draw ks
		TLatex latex1;
		latex1.SetNDC();
		latex1.SetTextAlign(12);
                latex1.SetTextSize(0.04);

		if(data_hist_name.Contains("maxSd0")) latex1.DrawLatex(0.6,0.6,text_ks.Data());
		

		//draw chi2

		TLatex latex2;
                latex2.SetNDC();
                latex2.SetTextAlign(12);
                latex2.SetTextSize(0.04);

                if(data_hist_name.Contains("maxSd0")) latex2.DrawLatex(0.6,0.55,text_Chi2.Data());

}


void HistPlotter::makeDataMCBtagEffPlot(TString var){

	TString pretag_hist_name="fatjet_BB_"+var+"_POSTFIT";
	TString pretag_data_hist_name="fatjet_other_"+var;

	std::vector<TString> pretag_hist_names={"fatjet_BB_"+var+"_POSTFIT","fatjet_BL_"+var+"_POSTFIT","fatjet_CC_"+var+"_POSTFIT","fatjet_CL_"+var+"_POSTFIT","fatjet_LL_"+var+"_POSTFIT"};

	
        HistogramHandler hist_handler(m_base_dir+"Output_gbbTupleAna/",m_sample_info,m_lumi); 

        HistogramStack pretag_stack=hist_handler.readInHistStack(pretag_hist_names,pretag_data_hist_name);
	
        TH1* pretag_mc_hist=(TH1*)pretag_stack.full_mc_hist.get()->Clone();

	std::vector<TString> posttag_hist_names={"fatjet_BB_"+var+"_POSTFITPOSTTAG","fatjet_BL_"+var+"_POSTFITPOSTTAG","fatjet_CC_"+var+"_POSTFITPOSTTAG","fatjet_CL_"+var+"_POSTFITPOSTTAG","fatjet_LL_"+var+"_POSTFITPOSTTAG"};
        TString posttag_data_hist_name="fatjet_other_"+var+"_POSTTAG";

	HistogramStack posttag_stack=hist_handler.readInHistStack(posttag_hist_names,posttag_data_hist_name);

	std::vector<std::shared_ptr<TH1>> posttag_mc_stack_hists=posttag_stack.mc_stack_hists;

	TH1* posttag_data_hist=(TH1*)posttag_stack.full_data_hist.get()->Clone();
	TH1* pretag_data_hist=(TH1*)pretag_stack.full_data_hist.get()->Clone();

	//rebinning
	
	/*
	std::vector<double> newbins;
	
	for(int i=1; i<posttag_data_hist->GetNbinsX()-3; i++){
	  newbins.push_back(posttag_data_hist->GetBinLowEdge(i));
	  
	}
	
	double *arr_newbins=&newbins[0];

	newbins.push_back(posttag_data_hist->GetBinLowEdge(posttag_data_hist->GetNbinsX()+1));
			  
	
	posttag_data_hist=posttag_data_hist->Rebin(newbins.size()-1,"",arr_newbins);
	pretag_data_hist=pretag_data_hist->Rebin(newbins.size()-1,"",arr_newbins);

	pretag_mc_hist=pretag_mc_hist->Rebin(newbins.size()-1,"",arr_newbins);

	*/
	TLegend *leg=new TLegend(0.1,0.7,0.2,0.9);
	leg->SetBorderSize(0);



	//Make MC efficiency plot
	TH1* posttag_mc_hist_orig=(TH1*)posttag_stack.full_mc_hist.get()->Clone();
	TH1* posttag_mc_hist=(TH1*)posttag_stack.full_mc_hist.get()->Clone();

	//posttag_mc_hist_orig=posttag_mc_hist_orig->Rebin(newbins.size()-1,"",arr_newbins);
	//posttag_mc_hist=posttag_mc_hist->Rebin(newbins.size()-1,"",arr_newbins);

	posttag_mc_hist->Divide(pretag_mc_hist);
	

	//make data rate plot: data tagged/data+all

	posttag_data_hist->Divide(pretag_data_hist);

	posttag_data_hist->SetDrawOption("EP");

	TH1* posttag_data_hist_noerr=(TH1*)posttag_data_hist->Clone();
        TH1* posttag_mc_hist_noerr=(TH1*)posttag_mc_hist->Clone();

	

	for(int i=1; i<=posttag_data_hist_noerr->GetNbinsX(); i++){
          posttag_data_hist_noerr->SetBinError(i,0.0001);
          posttag_mc_hist_noerr->SetBinError(i,0.0001);

        }


	//set styles and draw
	h2_style(posttag_mc_hist_noerr);
	posttag_mc_hist_noerr->SetLabelSize(0,"X");
	posttag_mc_hist_noerr->SetYTitle("Double b-tagging rate");
	posttag_data_hist_noerr->SetMarkerStyle(20);
	posttag_mc_hist_noerr->SetMarkerStyle(20);
	posttag_mc_hist_noerr->SetMarkerColor(2);

	m_pad1.get()->cd();

	posttag_mc_hist_noerr->Draw("EP");
	posttag_data_hist_noerr->Draw("same ep");

	leg->AddEntry(posttag_data_hist_noerr,"Data","pe");
	leg->AddEntry(posttag_mc_hist_noerr,"Pythia8 MC","pe");

	leg->Draw();
	
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


	//calculate systematics and show  bands
	TGraphAsymmErrors* fitsys_pretag=0, *jetsys_pretag=0;
	TGraphAsymmErrors* fitsys_posttag=0, *btagsys=0, *jetsys_posttag=0;
	TGraphAsymmErrors* mcstat_pretag=0, *mcstat_posttag=0;
	
	
	//explanation: sys gives relative error to respective histogram, posttag_mc=posttag_mc_orig/pretag_mc contains efficiency at this point


	TLegend *leg_sys_1=new TLegend(0.25,0.75,0.5,0.85);                                                                                                                                                                       \
	leg_sys_1->SetBorderSize(0);
	leg_sys_1->SetFillStyle(0);
        TLegend *leg_sys_2=new TLegend(0.25,0.65,0.5,0.75);                                                                                                                                                                       \
        leg_sys_2->SetBorderSize(0);
	leg_sys_2->SetFillStyle(0);
        TLegend *leg_sys_3=new TLegend(0.25,0.55,0.5,0.65);                                                                                                                                                                       \
        leg_sys_3->SetBorderSize(0);
	leg_sys_3->SetFillStyle(0);

	if(m_showFitSys){
  
	  fitsys_pretag=this->getFitSys(pretag_hist_names);
	  fitsys_posttag=this->getFitSys(posttag_hist_names);
	  
	  for(int i=0; i<fitsys_posttag->GetN(); i++){
	    float exp1_up=fitsys_posttag->GetErrorYhigh(i)*posttag_mc_hist->GetBinContent(i+1);
	    float exp2_up=posttag_mc_hist->GetBinContent(i+1)*fitsys_pretag->GetErrorYhigh(i);

	    float exp1_down=fitsys_posttag->GetErrorYlow(i)*posttag_mc_hist->GetBinContent(i+1);
	    float exp2_down=posttag_mc_hist->GetBinContent(i+1)*fitsys_pretag->GetErrorYlow(i);

	    double err_up=TMath::Sqrt((exp1_up*exp1_up)+(exp2_up*exp2_up));
	    double err_down=TMath::Sqrt((exp1_down*exp1_down)+(exp2_down*exp2_down));
	    
	    fitsys_pretag->SetPointEYhigh(i,err_up/posttag_mc_hist->GetBinContent(i+1));
	    fitsys_pretag->SetPointEYlow(i,err_down/posttag_mc_hist->GetBinContent(i+1));

	  }

	  
	  fitsys_pretag->SetFillColor(kGreen+1);
	  fitsys_pretag->SetFillStyle(3001);
	  
	  //m_pad2.get()->cd();
	  //fitsys_pretag->Draw("2");
	  
	  leg_sys_1->AddEntry(fitsys_pretag,"Fit Uncertainty","f");


	}


	if(m_showBtagSys){
	  
	  btagsys=this->getBTagSys(posttag_hist_names);
	  
	  for(int i=0; i<fitsys_posttag->GetN(); i++){

	    btagsys->SetPointEYhigh(i,btagsys->GetErrorYhigh(i));
	    btagsys->SetPointEYlow(i,btagsys->GetErrorYlow(i));

	  }
	  
	  btagsys->SetLineColor(kRed);
	  btagsys->SetFillStyle(0);
	  //m_pad2.get()->cd();
	  //btagsys->Draw("5");
	  leg_sys_2->AddEntry(btagsys,"B-tagging Uncertainty","l");


	}

	TGraphAsymmErrors* totsys;
	
	if(m_showTotalSys){
	  
	  jetsys_pretag=this->getTotalJetSys(pretag_hist_names);
	  jetsys_posttag=this->getTotalJetSys(posttag_hist_names);
	  
	  for(int i=0; i<jetsys_posttag->GetN(); i++){
            float exp1_up=jetsys_posttag->GetErrorYhigh(i)*posttag_mc_hist->GetBinContent(i+1);
            float exp2_up=posttag_mc_hist->GetBinContent(i+1)*jetsys_pretag->GetErrorYhigh(i);

            float exp1_down=jetsys_posttag->GetErrorYlow(i)*posttag_mc_hist->GetBinContent(i+1);
            float exp2_down=posttag_mc_hist->GetBinContent(i+1)*jetsys_pretag->GetErrorYlow(i);

            double err_up=TMath::Sqrt((exp1_up*exp1_up)+(exp2_up*exp2_up));
            double err_down=TMath::Sqrt((exp1_down*exp1_down)+(exp2_down*exp2_down));

            jetsys_pretag->SetPointEYhigh(i,err_up/posttag_mc_hist->GetBinContent(i+1));
            jetsys_pretag->SetPointEYlow(i,err_down/posttag_mc_hist->GetBinContent(i+1));
	    
          }

	  mcstat_pretag=this->getMCStat(pretag_mc_hist);
	  mcstat_posttag=this->getMCStat(posttag_mc_hist_orig);

	  for(int i=0; i<mcstat_posttag->GetN(); i++){
            float exp1_up=mcstat_posttag->GetErrorYhigh(i)*posttag_mc_hist->GetBinContent(i+1);
            float exp2_up=posttag_mc_hist->GetBinContent(i+1)*mcstat_pretag->GetErrorYhigh(i);

            float exp1_down=mcstat_posttag->GetErrorYlow(i)*posttag_mc_hist->GetBinContent(i+1);
            float exp2_down=posttag_mc_hist->GetBinContent(i+1)*mcstat_pretag->GetErrorYlow(i);

            double err_up=TMath::Sqrt((exp1_up*exp1_up)+(exp2_up*exp2_up));
            double err_down=TMath::Sqrt((exp1_down*exp1_down)+(exp2_down*exp2_down));

            mcstat_pretag->SetPointEYhigh(i,err_up/posttag_mc_hist->GetBinContent(i+1));
            mcstat_pretag->SetPointEYlow(i,err_down/posttag_mc_hist->GetBinContent(i+1));

          }

	  
	  totsys=getTotalSys(fitsys_pretag,btagsys,jetsys_pretag,mcstat_pretag);
	  totsys->SetFillColor(kBlack);
	  totsys->SetFillStyle(3005);
	  totsys->SetLineColor(kBlack);
	  //m_pad2.get()->cd();
	  //totsys->Draw("5");
	  
	  leg_sys_3->AddEntry(totsys,"Total Uncertainty","f");

	}
	
	m_pad2.get()->cd();
	
	if(m_showTotalSys) totsys->Draw("5");
	if(m_showFitSys) fitsys_pretag->Draw("2");
	if(m_showBtagSys) btagsys->Draw("5");


	h_ratio->Draw("same");
	leg_sys_1->Draw();
	leg_sys_2->Draw();
        leg_sys_3->Draw();

}


void HistPlotter::setSysConfig(bool showFitSys, bool showBtagSys, bool showTotSys,TString fit_file, std::vector<TString> *fit_ptbins){

  m_showFitSys=showFitSys;
  m_showBtagSys=showBtagSys;
  m_showTotalSys=showTotSys;

  if(showFitSys && (fit_file.EqualTo("") || fit_ptbins==0)){
    std::cout<<"No Fit file specified, run without Fir uncertainty!"<<std::endl;
    m_showFitSys=false;
  }else{    
    if(!m_corrector) m_corrector=new FlavourFracCorrector(fit_file);
    m_fit_ptbins=*(fit_ptbins);
  }

}


TGraphAsymmErrors* HistPlotter::getFitSys(std::vector<TString> &base_hist_names){

  std::vector<TString> sys_hist_names;

  HistogramHandler hist_handler(m_base_dir+"Output_gbbTupleAna/",m_sample_info,m_lumi);

  HistogramStack nom_stack=hist_handler.readInHistStack(base_hist_names,"");
  TH1* h_nom=(TH1*)nom_stack.full_mc_hist.get()->Clone();
  
  //rebin histogram if necessary                                                                                                                                                                                                           
  if(m_Rebin){
    h_nom->Rebin(m_rebin_factor);
  }

  int n_bins=h_nom->GetNbinsX();
  int n_fitslices=m_fit_ptbins.size();

  double fiterrors_up[n_bins],fiterrors_down[n_bins],tot_err_sq[n_bins],x_values[n_bins],y_values[n_bins],x_error_up[n_bins],x_error_down[n_bins], total_bin_content[n_bins];
  double full_norm[n_bins], norm_slices_varbin[n_fitslices][n_bins];

  double fraction_total_norm_varbin[n_fitslices][n_bins];

  double fit_total_error_slice[n_fitslices];
  
  double norm_fitbin[n_fitslices];
  double full_var_norm=0;

  

  for(int ib=0; ib<n_bins; ib++){

    tot_err_sq[ib]=0;
    total_bin_content[ib]=0;
    full_norm[ib]=0;
  }



  if(base_hist_names[0].Contains("maxSd0")){
    TObjArray* tokens;
    TString name=base_hist_names[0](0,base_hist_names[0].Last('_'));
    tokens=name.Tokenize('_');
    TString ptbin=((TObjString*)(*tokens)[2])->GetString()+"_"+((TObjString*)(*tokens)[3])->GetString()+"_"+((TObjString*)(*tokens)[4])->GetString()+"_"+((TObjString*)(*tokens)[5])->GetString();

    m_fit_ptbins.assign(1,ptbin);
    
  }


  
  
  for(int i_fbin=0; i_fbin<m_fit_ptbins.size(); i_fbin++){
    
    sys_hist_names.clear();
    

    if(base_hist_names[0].Contains("maxSd0_POSTFIT")){
      //sd0 histograms are already in correct bins

      for(int i_hist=0; i_hist<base_hist_names.size(); i_hist++){
	
	sys_hist_names.push_back(base_hist_names[i_hist](0,base_hist_names[i_hist].Last('_')));
	std::cout<<"looking for"<< sys_hist_names.back()<<std::endl;

      }
      
    }else if(base_hist_names[0].Contains("_B_") || base_hist_names[0].Contains("_C_") || base_hist_names[0].Contains("_L_") ){
      //for muonjet pt and nonmuon jet pt need all flavour categories
      TString name=base_hist_names[0](0,base_hist_names[0].Last('_'));

      if(base_hist_names[0].Contains("POSTFITPOSTTAG")){

	name=base_hist_names[0](0,base_hist_names[0].Last('_')) +"_PREFITPOSTTAG";
      
      }else if(base_hist_names[0].Contains("POSTFIT")){

	name=base_hist_names[0](0,base_hist_names[0].Last('_'));
      }


      std::vector<TString> flavours={"BB","BL","CC","CL","LL"};
      
      TObjArray* tokens;
      tokens=name.Tokenize('_');
      TString sys_tmp;

      for(int i_fl=0; i_fl<flavours.size(); i_fl++){
	sys_tmp=((TObjString*)(*tokens)[0])->GetString()+"_dijetcat_"+flavours[i_fl]+"_"+((TObjString*)(*tokens)[2])->GetString()+"_"+m_fit_ptbins[i_fbin]+"_"+"FITERR";
	sys_hist_names.push_back(sys_tmp);
	std::cout<<"looking for"<< sys_hist_names.back()<<std::endl;
      }
      
    }else{

      for(int i_hist=0; i_hist<base_hist_names.size(); i_hist++){
	
	TString name=base_hist_names[i_hist];
	
	if(base_hist_names[i_hist].Contains("POSTFITPOSTTAG")){
	  
	  name=base_hist_names[i_hist](0,base_hist_names[i_hist].Last('_')) +"_PREFITPOSTTAG" ;
	  
	}else if(base_hist_names[i_hist].Contains("POSTFIT")){
	  
	  name=base_hist_names[i_hist](0,base_hist_names[i_hist].Last('_'));
	  
	}
	
	sys_hist_names.push_back(name+"_"+m_fit_ptbins[i_fbin]+"_"+"FITERR");
	
	std::cout<<"looking for"<< sys_hist_names.back()<<std::endl;
      }
    }

    
    HistogramStack hist_stack=hist_handler.readInHistStack(sys_hist_names,"");
    
    std::vector<std::shared_ptr<TH1>> mc_stack_hists=hist_stack.mc_stack_hists;
    
  
    TH1* tmp_global=(TH1*)hist_stack.full_mc_hist.get();

    if(m_Rebin){
      
      tmp_global->Rebin(m_rebin_factor);
    }
    
    std::vector<float> N_template;

    
    N_template.clear();

    for(int i_stack=0; i_stack<mc_stack_hists.size(); i_stack++){
      TH1* temp=(TH1*)mc_stack_hists[i_stack].get();
      
      //rebin histogram if necessary                                                                                                                                                     
      if(m_Rebin){                             
	temp->Rebin(m_rebin_factor); 
      }
      
      
      N_template.push_back(temp->Integral());

      
    }
    
    for(int i=0; i<N_template.size(); i++) std::cout<<"Ntemplate "<<i<<" "<<N_template[i]<<std::endl; 
    
    
    TVector v(N_template.size(),&N_template[0]);
    
    TVector v2=v;
    
    
    std::vector<float> matrix=m_corrector->GetCovarianceMatrix(m_fit_ptbins[i_fbin]);
    
    //for(int j=0; j<matrix.size(); j++) std::cout<<"Matrix "<<j<<" "<<matrix[j]<<std::endl;
    
    TMatrix mat(N_template.size(),N_template.size(),&matrix[0]);
    
    v2*=mat;
    
    fit_total_error_slice[i_fbin]=v2*v;

    std::cout<<"Total norm uncertainty"<<TMath::Sqrt(fit_total_error_slice[i_fbin])<<std::endl;
    std::cout<<"sqrt total evt in this bin: "<<TMath::Sqrt(tmp_global->Integral())<<std::endl;
    
    
    for(int i=1; i<=tmp_global->GetNbinsX(); i++){
      
      fraction_total_norm_varbin[i_fbin][i-1]=tmp_global->GetBinContent(i)/tmp_global->Integral();

      norm_slices_varbin[i_fbin][i-1]=tmp_global->GetBinContent(i);
      full_norm[i-1]+=tmp_global->GetBinContent(i);
    }
    

    norm_fitbin[i_fbin]=tmp_global->Integral();
    full_var_norm+=tmp_global->Integral();
    
    
  }


  //calculate fit contributions in each bin
  for(int i_fbin=0; i_fbin<m_fit_ptbins.size(); i_fbin++){
    

    for(int i=0; i<n_bins; i++){

      tot_err_sq[i]+=fit_total_error_slice[i_fbin]*fraction_total_norm_varbin[i_fbin][i];

    }

  }
  
  for(int i=1; i<=h_nom->GetNbinsX(); i++){                                                                                                                                                                                     
    x_values[i-1]=h_nom->GetBinCenter(i);                                                                                                                                                                                 
    x_error_up[i-1]=h_nom->GetBinWidth(i)/2;                                                                                                                                                                              
    x_error_down[i-1]=h_nom->GetBinWidth(i)/2;                                                                                                                                                                            
    y_values[i-1]=1.;
    
    std::cout<<"total error squared"<<(tot_err_sq[i-1])<<std::endl;

    std::cout<<"evts"<<(h_nom->GetBinContent(i))<<std::endl;
    

    if(h_nom->GetBinContent(i)) fiterrors_up[i-1]=TMath::Sqrt(tot_err_sq[i-1])/h_nom->GetBinContent(i);
    else fiterrors_up[i-1]=0;
    
    if(h_nom->GetBinContent(i)) fiterrors_down[i-1]=TMath::Sqrt(tot_err_sq[i-1])/h_nom->GetBinContent(i);
    else fiterrors_down[i-1]=0;

    std::cout<<"FitError is:"<<fiterrors_up[i-1]<<std::endl;

  }   

  

  TGraphAsymmErrors *g_fiterrors= new TGraphAsymmErrors(n_bins,x_values,y_values,x_error_down,x_error_up,fiterrors_down,fiterrors_up);

  return g_fiterrors;

}


TGraphAsymmErrors* HistPlotter::getBTagSys(std::vector<TString> &base_hist_names){

  HistogramHandler hist_handler(m_base_dir+"Output_gbbTupleAna/",m_sample_info,m_lumi);

  std::vector<TString> sys_names_up,  sys_names_down;                                                                                                                                                                                                      
  for(int i_hist=0; i_hist<base_hist_names.size(); i_hist++){                                                                                                                                                             
    sys_names_up.push_back(base_hist_names[i_hist]+"_"+"BTAGUP");                                                                                        
    sys_names_down.push_back(base_hist_names[i_hist]+"_"+"BTAGDOWN");

  } 

  HistogramStack nom_stack=hist_handler.readInHistStack(base_hist_names,"");
  HistogramStack up_stack=hist_handler.readInHistStack(sys_names_up,"");
  HistogramStack down_stack=hist_handler.readInHistStack(sys_names_down,"");
  
  TH1* h_up=(TH1*)up_stack.full_mc_hist.get()->Clone();                                                                                                                                                                        
  TH1* h_down=(TH1*)down_stack.full_mc_hist.get()->Clone();                                                                                                                                                                    
  TH1* h_nom=(TH1*)nom_stack.full_mc_hist.get()->Clone();

  //normalized option

  if(m_doNormalized){

    h_up->Scale(1./h_up->Integral());
    h_down->Scale(1./h_down->Integral());
    h_nom->Scale(1./h_nom->Integral());

  }

  //rebin up and down histogram if necessary                                                                                                                                                                    
  if(m_Rebin){
    h_nom->Rebin(m_rebin_factor);
    h_up->Rebin(m_rebin_factor);
    h_down->Rebin(m_rebin_factor);
  }

  int n_bins=h_nom->GetNbinsX();

  double btagerrors_up[n_bins],btagerrors_down[n_bins],x_values[n_bins],y_values[n_bins],x_error_up[n_bins],x_error_down[n_bins];
  
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

TGraphAsymmErrors* HistPlotter::getTotalJetSys(std::vector<TString>& base_hist_names){

  HistogramHandler hist_handler(m_base_dir+"Output_gbbTupleAna/",m_sample_info,m_lumi);
  HistogramStack nom_stack=hist_handler.readInHistStack(base_hist_names,"");


  TH1* h_nom=(TH1*)nom_stack.full_mc_hist.get()->Clone();
 
  if(m_doNormalized) h_nom->Scale(1./h_nom->Integral());

  if(m_Rebin){
    h_nom->Rebin(m_rebin_factor);
  }

  int n_bins=h_nom->GetNbinsX();

  double total_errors_up[n_bins],total_errors_down[n_bins], total_errsquare_up[n_bins],total_errsquare_down[n_bins],x_values[n_bins],y_values[n_bins],x_error_up[n_bins],x_error_down[n_bins];

  for(int i=1; i<=h_nom->GetNbinsX(); i++){
    total_errsquare_down[i-1]=0;
    total_errsquare_up[i-1]=0;

  }


  for(int i_sys=0; i_sys<m_syslist_up.size(); i_sys++){
    
    HistogramHandler hist_handler_up(m_base_dir+"Output_gbbTupleAna_"+m_syslist_up[i_sys]+"/",m_sample_info,m_lumi);
    HistogramHandler hist_handler_down(m_base_dir+"Output_gbbTupleAna_"+m_syslist_down[i_sys]+"/",m_sample_info,m_lumi);

    HistogramStack up_stack=hist_handler_up.readInHistStack(base_hist_names,"");
    HistogramStack down_stack=hist_handler_down.readInHistStack(base_hist_names,"");

    TH1* h_up=(TH1*)up_stack.full_mc_hist.get()->Clone();
    TH1* h_down=(TH1*)down_stack.full_mc_hist.get()->Clone();

    //normalized option
    if(m_doNormalized){
      
      h_up->Scale(1./h_up->Integral());
      h_down->Scale(1./h_down->Integral());
      
    }

    //rebin up and down histogram if necessary (nominal is already rebinned)                                                                                                                                                            
    if(m_Rebin){
      h_up->Rebin(m_rebin_factor);
      h_down->Rebin(m_rebin_factor);
    }

    for(int i=1; i<=h_nom->GetNbinsX(); i++){
      
      float delta_up=h_up->GetBinContent(i)-h_nom->GetBinContent(i);
      float delta_down=h_down->GetBinContent(i)-h_nom->GetBinContent(i);

      if(delta_up*delta_down<0){
	
	if(delta_up>0){
	  total_errsquare_up[i-1]+=delta_up*delta_up;
	  total_errsquare_down[i-1]+=delta_down*delta_down;
	}else{
	  total_errsquare_down[i-1]+=delta_up*delta_up;
          total_errsquare_up[i-1]+=delta_down*delta_down;
	}

      }else{
	
	float bigger_delta= std::max(TMath::Abs(delta_up),TMath::Abs(delta_down));
	if(delta_up>0) total_errsquare_up[i-1]+=bigger_delta*bigger_delta;
	else total_errsquare_down[i-1]+=bigger_delta*bigger_delta;

      }

      

    }


  }

  for(int i_sys=0; i_sys<m_syslist_1side.size(); i_sys++){

    HistogramHandler hist_handler_1side(m_base_dir+"Output_gbbTupleAna_"+m_syslist_1side[i_sys]+"/",m_sample_info,m_lumi);

    HistogramStack side_stack=hist_handler_1side.readInHistStack(base_hist_names,"");

    TH1* h_1side=(TH1*)side_stack.full_mc_hist.get()->Clone();

    //normalized option
    if(m_doNormalized) h_1side->Scale(1./h_1side->Integral());

    //rebin up and down histogram if necessary (nominal is already rebinned)                                                                                                                                                                 
    if(m_Rebin){
      h_1side->Rebin(m_rebin_factor);
    }

    for(int i=1; i<=h_nom->GetNbinsX(); i++){

      float delta_up=h_1side->GetBinContent(i)-h_nom->GetBinContent(i);

      if(delta_up>0) total_errsquare_up[i-1]+=delta_up*delta_up;
      else total_errsquare_down[i-1]+=delta_up*delta_up;
	
    }


  }


  for(int i=1; i<=h_nom->GetNbinsX(); i++){

    x_values[i-1]=h_nom->GetBinCenter(i);
    x_error_up[i-1]=h_nom->GetBinWidth(i)/2;
    x_error_down[i-1]=h_nom->GetBinWidth(i)/2;
    y_values[i-1]=1.;

    if(h_nom->GetBinContent(i)) total_errors_up[i-1]=TMath::Sqrt(total_errsquare_up[i-1])/h_nom->GetBinContent(i);
    else total_errors_up[i-1]=0;

    if(h_nom->GetBinContent(i)) total_errors_down[i-1]=TMath::Sqrt(total_errsquare_down[i-1])/h_nom->GetBinContent(i);
    else total_errors_down[i-1]=0;

  }
  

  

  TGraphAsymmErrors *g_totaljeterrors= new TGraphAsymmErrors(n_bins,x_values,y_values,x_error_down,x_error_up,total_errors_down,total_errors_up);

  return g_totaljeterrors;


}


TGraphAsymmErrors* HistPlotter::getMCStat(TH1* full_mc){

  int nbins=full_mc->GetNbinsX();
  
  double x_values[nbins], y_values[nbins], x_error_up[nbins],x_error_down[nbins],y_error_up[nbins],y_error_down[nbins];

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



TGraphAsymmErrors* HistPlotter::getTotalSys(TGraphAsymmErrors* fitsysgraph, TGraphAsymmErrors* btagsysgraph, TGraphAsymmErrors* jetsysgraph, TGraphAsymmErrors* stat){

  int n_points=fitsysgraph->GetN();

  double err_squared_tot_up[n_points],err_squared_tot_down[n_points],err_tot_up[n_points],err_tot_down[n_points];
  

  for(int i=0; i<n_points; i++){
  
    err_squared_tot_up[i]=0.;
    err_squared_tot_down[i]=0.;
  
    err_squared_tot_up[i]+=fitsysgraph->GetErrorYhigh(i)*fitsysgraph->GetErrorYhigh(i);
    if(btagsysgraph) err_squared_tot_up[i]+=btagsysgraph->GetErrorYhigh(i)*btagsysgraph->GetErrorYhigh(i);
    err_squared_tot_up[i]+=jetsysgraph->GetErrorYhigh(i)*jetsysgraph->GetErrorYhigh(i);
    if(stat) err_squared_tot_up[i]+=stat->GetErrorYhigh(i)*stat->GetErrorYhigh(i);

    err_squared_tot_down[i]+=fitsysgraph->GetErrorYlow(i)*fitsysgraph->GetErrorYlow(i);
    if(btagsysgraph) err_squared_tot_down[i]+=btagsysgraph->GetErrorYlow(i)*btagsysgraph->GetErrorYlow(i);
    err_squared_tot_down[i]+=jetsysgraph->GetErrorYlow(i)*jetsysgraph->GetErrorYlow(i);
    if(stat) err_squared_tot_down[i]+=stat->GetErrorYlow(i)*stat->GetErrorYlow(i);


    err_tot_up[i]=TMath::Sqrt(err_squared_tot_up[i]);
    err_tot_down[i]=TMath::Sqrt(err_squared_tot_down[i]);

  }

  TGraphAsymmErrors *g_totalerrors= new TGraphAsymmErrors(n_points,fitsysgraph->GetX(),fitsysgraph->GetY(),fitsysgraph->GetEXlow(),fitsysgraph->GetEXhigh(),err_tot_down,err_tot_up);

  return g_totalerrors;

} 


/*
void HistPlotter::addSysShades(TH1* nominal_hist){

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

*/

void HistPlotter::makeComparisonPlot(std::vector<TString>& hist_names, bool doNormalized){

  HistogramHandler hist_handler(m_base_dir+"Output_gbbTupleAna/",m_sample_info,m_lumi); 

  MultiHistogramStack stack=hist_handler.readInMultiHistStack(hist_names,hist_names);

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


ColorStack HistPlotter::getColor(TString hist_name){

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


void HistPlotter::setAtlasStyle()
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

void HistPlotter::addLabel(int type){

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


