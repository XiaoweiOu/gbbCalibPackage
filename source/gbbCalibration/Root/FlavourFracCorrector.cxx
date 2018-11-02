/*
 * FlavourFracCorrector.cxx
 *
 *  Created on: Apr 4, 2016
 *      Author: rjacobs
 */

#include "gbbCalibration/FlavourFracCorrector.h"
#include "TCollection.h"
#include "TKey.h"
#include "TClass.h"
#include "TROOT.h"
#include "TMath.h"
#include <iostream>

FlavourFracCorrector::FlavourFracCorrector() {
	// TODO Auto-generated constructor stub

}

FlavourFracCorrector::~FlavourFracCorrector() {
	// TODO Auto-generated destructor stub
}

FlavourFracCorrector::FlavourFracCorrector(TString infilename){
	//m_muojet_pt_bins={70.,150.};
	//m_nonmuojet_pt_bins={20.,50.,80.};

	m_muojet_pt_bins={100.,200.};                                                                                                                                                                                                               m_nonmuojet_pt_bins={20.,50.,80.};
	
	this->ReadInFitResults(infilename);

}


void FlavourFracCorrector::ReadInFitResults(TString infilename){

	TFile *file=TFile::Open(infilename.Data(),"READ");
	TIter next(file->GetListOfKeys());
	TKey *key;
	while ((key = (TKey*)next())) {
		TClass *cl = gROOT->GetClass(key->GetClassName());
		if (!cl->InheritsFrom("TH1")) continue;
		TH1D *h = (TH1D*)key->ReadObj();
		TString name(h->GetName());
		std::cout<<"Reading in "<<name<<std::endl;
		m_FitResults[name]=std::shared_ptr<TH1D>(new TH1D(*h));
		m_FitResults[name].get()->SetDirectory(0);
	}

	file->Close();
}

float FlavourFracCorrector::GetCorrectionFactorNom(TString flav_category,float mu_pt, float nonmu_pt){
	
	TString pt_label=this->getPtLabel(mu_pt,nonmu_pt);

	TString name="hValue_mu_"+flav_category+"_"+pt_label+"_Nom";

	TH1D *tmp;
	tmp=m_FitResults[name].get();

	if(!tmp) std::cout<<"Cannot find histogram "<<name<<std::endl;

	return 	tmp->GetBinContent(1);

}

float FlavourFracCorrector::GetCorrectionFactorSys(TString flav_category,float mu_pt, float nonmu_pt,TString sys_name_variation){

        TString pt_label=this->getPtLabel(mu_pt,nonmu_pt);

        TString name="hValue_mu_"+flav_category+"_"+pt_label+"_"+sys_name_variation;

	TH1D *tmp;
        tmp=m_FitResults[name].get();
        
	return tmp->GetBinContent(1);
}

float FlavourFracCorrector::GetCorrectionFactorStatUp(TString flav_category,float mu_pt, float nonmu_pt){

        TString pt_label=this->getPtLabel(mu_pt,nonmu_pt);

	TString name="hValue_mu_"+flav_category+"_"+pt_label+"_Nom";
	TString name_Err="hErr_mu_"+flav_category+"_"+pt_label+"_Nom";

	TH1D *tmp, *tmp1;
	tmp=m_FitResults[name].get();
        tmp1=m_FitResults[name_Err].get();

	return 	tmp->GetBinContent(1)+tmp1->GetBinContent(1);

}

float FlavourFracCorrector::GetCorrectionFactorStatDown(TString flav_category,float mu_pt, float nonmu_pt){

        TString pt_label=this->getPtLabel(mu_pt,nonmu_pt);

	TString name="hValue_mu_"+flav_category+"_"+pt_label+"_Nom";
	TString name_Err="hErr_mu_"+flav_category+"_"+pt_label+"_Nom";

	TH1D *tmp, *tmp1;
        tmp=m_FitResults[name].get();
        tmp1=m_FitResults[name_Err].get();

	return 	tmp->GetBinContent(1)-tmp1->GetBinContent(1);

}


float FlavourFracCorrector::GetCorrectionFactorTotalError(float mu_pt, float nonmu_pt){

  TString pt_label=this->getPtLabel(mu_pt,nonmu_pt);
  TString name_Err="hTotErr_"+pt_label+"_Nom";

  TH1D *tmp;
  tmp=m_FitResults[name_Err].get();

  return tmp->GetBinContent(1);

}

float FlavourFracCorrector::GetCovarianceMatrixElement(int i, int j, TString ptlabel){
  TString name_CovMat="hCovMat_"+ptlabel+"_Nom";
  
  TH1D *tmp=m_FitResults[name_CovMat].get();
  
  int nparam_sq=tmp->GetNbinsX();
  int nparam=(int)TMath::Sqrt((float)nparam_sq);

  return tmp->GetBinContent(j+nparam*i+1);

}

std::vector<float> FlavourFracCorrector::GetCovarianceMatrix(TString ptlabel){
  
  TString name_CovMat="hCovMat_"+ptlabel+"_Nom";

  TH1D *tmp=m_FitResults[name_CovMat].get();

  std::vector<float> mat;

  for(int i=1; i<=tmp->GetNbinsX(); i++){
    
    mat.push_back(tmp->GetBinContent(i));

  }

  return mat;

}


TString FlavourFracCorrector::getPtLabel(float muojet_pt,float nonmuojet_pt){

	TString label="";


	if(muojet_pt<m_muojet_pt_bins[0]) label+=Form("mjpt_l%.0f_",m_muojet_pt_bins[0]);
	else if(muojet_pt>=m_muojet_pt_bins[m_muojet_pt_bins.size()-1]) label+=Form("mjpt_g%.0f_",m_muojet_pt_bins[m_muojet_pt_bins.size()-1]);
	else{

		for(int i_m=0; i_m<m_muojet_pt_bins.size()-1; i_m++){

			if(muojet_pt>=m_muojet_pt_bins[i_m] && muojet_pt<m_muojet_pt_bins[i_m+1]){
				label+=Form("mjpt_g%.0fl%.0f_",m_muojet_pt_bins[i_m],m_muojet_pt_bins[i_m+1]);
			}

		}

	}

	if(nonmuojet_pt<m_nonmuojet_pt_bins[0]) label+=Form("nmjpt_l%.0f",m_nonmuojet_pt_bins[0]);
	else if(nonmuojet_pt>=m_nonmuojet_pt_bins[m_nonmuojet_pt_bins.size()-1]) label+=Form("nmjpt_g%.0f",m_nonmuojet_pt_bins[m_nonmuojet_pt_bins.size()-1]);
	else{

		for(int i_nm=0; i_nm<m_nonmuojet_pt_bins.size()-1; i_nm++){
			if(nonmuojet_pt>=m_nonmuojet_pt_bins[i_nm] && nonmuojet_pt<m_nonmuojet_pt_bins[i_nm+1]){
				label+=Form("nmjpt_g%.0fl%.0f",m_nonmuojet_pt_bins[i_nm],m_nonmuojet_pt_bins[i_nm+1]);
			}
		}

	}
	
	return label;
}
