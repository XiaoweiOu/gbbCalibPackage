/*
 * HistogramHandler.cxx
 *
 *  Created on: Feb 23, 2016
 *      Author: rjacobs
 */

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

HistogramHandler::HistogramHandler() :  m_lumi(1e6){
	// TODO Auto-generated constructor stub

}

HistogramHandler::~HistogramHandler() {
	// TODO Auto-generated destructor stub


}

HistogramHandler::HistogramHandler(TString in_dir, SampleInfo* info, float lumi, TString name) : m_lumi(lumi),m_sampinfo(info) {
	// TODO Auto-generated constructor stub

	this->ListFiles(in_dir);


}

void HistogramHandler::ListFiles(TString in_dir){

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
			    std::cout<<"fname is: "<<fname<<std::endl;
			  }

			}
		}
	}
}


float HistogramHandler::getWeightSample(TString sample_name){
 
  std::cout<<"Getting weight"<<std::endl;
	//get EventCount histogram
	TFile* f=TFile::Open(sample_name.Data(),"READ");
	TH1D * h_bkk;
	f->GetObject("Hist_BookKeeping",h_bkk);
	float weight=1./h_bkk->GetBinContent(3);
	
	std::cout<<"Getting Sample Info: "<<std::endl;
	std::cout<<"DSID: "<<m_sampinfo->getDSID(sample_name)<<std::endl;

	//std::cout<<"Number of events DAOD: "<<h_bkk->GetBinContent(2)<<std::endl;
	//std::cout<<"Number of events AOD hist: "<<h_bkk->GetBinContent(1)<<std::endl;
	//std::cout<<"Number of events AOD AMI: "<<m_sampinfo->xAODEventCount(m_sampinfo->getDSID(sample_name))<<std::endl;
	//std::cout<<"Number of events ntups: "<<h_bkk->GetBinContent(3)<<std::endl;

	//weight*=(h_bkk->GetBinContent(2)/m_sampinfo->xAODEventCount(m_sampinfo->getDSID(sample_name))); // n_DAOD/n_AOD
	weight*=(m_sampinfo->DxAODEventCount(m_sampinfo->getDSID(sample_name))/m_sampinfo->xAODEventCount(m_sampinfo->getDSID(sample_name))); // n_DAOD/n_AOD 

	weight*=m_sampinfo->CrossSection(m_sampinfo->getDSID(sample_name));
	//std::cout<<"Cross section "<<m_sampinfo->CrossSection(m_sampinfo->getDSID(sample_name))<<std::endl;

	weight*=m_sampinfo->FilterEfficiency(m_sampinfo->getDSID(sample_name));
	//std::cout<<"Filter eff: "<<m_sampinfo->FilterEfficiency(m_sampinfo->getDSID(sample_name))<<std::endl;

	weight*=m_lumi;
	//std::cout<<"Lumi: "<<m_lumi<<std::endl;

	std::cout<<"weight: "<<weight<<std::endl;

	f->Close();
	
	return weight;
}


HistogramStack HistogramHandler::readInHistStack(std::vector<TString>& hist_names,TString data_hist_name){

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


MultiHistogramStack HistogramHandler::readInMultiHistStack(std::vector<TString>& hist_names,std::vector<TString>& data_names){
  
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
