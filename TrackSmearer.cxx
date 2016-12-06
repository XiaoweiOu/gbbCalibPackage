#include "TrackSmearer.h"


TrackSmearer::TrackSmearer(TString hist_filename, TString func_filename, bool doDeadModules){

	m_do_Dead=doDeadModules;
	m_hist_infile=std::shared_ptr<TFile>(new TFile(hist_filename.Data(),"READ"));
	m_func_infile=std::shared_ptr<TFile>(new TFile(func_filename.Data(),"READ"));

	//Random generator for correction factor calculation, set seed 0                                                    
	m_rndm=std::shared_ptr<TRandom3>(new TRandom3(0));


	TH2 *tmp_2dhist;
	tmp_2dhist=(TH2*)(m_func_infile.get()->Get("quad_diff/d0quaddiff_comb_Pt_Eta_Loose"));
	       	
	TH1* tmp_hist;
	tmp_hist=(TH1*)(m_hist_infile.get()->Get("res_pt_d0_0"));
	tmp_hist->SetDirectory(0);

	m_quaddiff=std::shared_ptr<TH2>((TH2*)tmp_2dhist->Clone());
	m_dead_hist=std::shared_ptr<TH1>((TH1*)tmp_hist->Clone());
	m_dead_hist.get()->SetDirectory(0);

	m_hist_infile.get()->Close();
        m_func_infile.get()->Close();

        
}



float TrackSmearer::getD0SmearSigma(float pt, float eta){
	pt*=1.e-3;
	
	//std::cout<<"pt: "<<pt<<" eta: "<<eta<<std::endl;

	float sigma_d0=0.;

	if(m_do_Dead){
	  
	  float d0Smear=m_dead_hist->GetBinContent(m_dead_hist.get()->FindBin(pt));
	  sigma_d0+=d0Smear*d0Smear;
	}
		
	float d0Smear=readHistogram(m_quaddiff.get(),pt,eta);
	sigma_d0+=d0Smear*d0Smear;
			
	return TMath::Sqrt(sigma_d0); 
	
}

float TrackSmearer::getSmearedD0(float pt, float eta, float d0){

	//only call if widths are larger than 0

  //std::cout<<"d0 before"<<d0<<std::endl;

        float sigma_d0=this->getD0SmearSigma(pt, eta);
	
	float smeared_d0= sigma_d0>0. ? m_rndm->Gaus(d0,sigma_d0) : d0;
	
	//std::cout<<"d0 after"<<smeared_d0<<std::endl;

	return smeared_d0;

}

float TrackSmearer::readHistogram(TH2* hist, float pt, float eta){

  if(eta>2.499) eta=2.499;
  if(eta<-2.499) eta =2.499;
  if(pt >=hist->GetXaxis()->GetXmax()){
    pt=hist->GetXaxis()->GetXmax()-0.01;

  }

  int bin=hist->FindFixBin(pt,eta);
  float val = hist->GetBinContent(bin);

  return val;

}
