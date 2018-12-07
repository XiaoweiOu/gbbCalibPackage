/*
 * FitData.cxx
 *
 *  Created on: Mar 29, 2016
 *      Author: rjacobs
 */

#include "FitData.h"

FitData::FitData() {
  // TODO Auto-generated constructor stub

}

FitData::~FitData() {
  // TODO Auto-generated destructor stub	m_infile.get()->Close();
}

FitData::FitData(TString filename, std::vector<TString> chans){
  m_infile=std::shared_ptr<TFile>(new TFile(filename.Data(),"READ"));

  if(m_infile->IsOpen()) std::cout<<"Info in FitData: Open file "<<filename.Data()<<std::endl;
  else std::cout<<"Error in FitData: Could not open file "<<filename.Data()<<std::endl;

  m_chans=chans;

  //Random generator for fit error calculation, set seed 0
  m_rndm=std::shared_ptr<TRandom3>(new TRandom3(0));
       
}


double FitData::NegLogSingleBinPoisson(int i,TString& channel, double* par, bool doRandom, bool doRandomMC){

  double y_i=0;
	

  if(doRandom){
    y_i=m_BootStrapDataMap[channel][i-1];
  }else y_i=m_dataHistMap[channel].get()->GetBinContent(i);

  double sum=0;

  for(unsigned int j=0; j<m_templateHistsMap[channel].size(); j++){
	
    if(doRandomMC)sum+=(par[j]*m_BootStrapTemplateMap[channel][j][i-1]);
    else sum+=(par[j]*m_templateHistsMap[channel][j].get()->GetBinContent(i));

  }

  double value;

  if(!(sum>0.)) std::cout<<"MC prediction is zero in bin" <<i<<"!!! Should be caught by rebinning. Data is:"<<y_i<<std::endl;

  if(y_i<0) value=0.;
  else if(!(sum>0.)) value=0;
  else value=y_i*TMath::Log(sum)-sum;

  return value;
}

double FitData::NegLogSingleBinPoissonWithNP(int i,TString& channel, double* par, bool doRandom){

  double y_i=0;

  if(doRandom){
    y_i=m_BootStrapDataMap[channel][i-1];
  }else y_i=m_dataHistMap[channel].get()->GetBinContent(i);

  double sumerr2=0., sum_nopar=0.;
  for(unsigned int j=0; j<m_templateHistsMap[channel].size(); j++){
    sumerr2+=m_templateHistsMap[channel][j].get()->GetBinError(i)*m_templateHistsMap[channel][j].get()->GetBinError(i);
    sum_nopar+=m_templateHistsMap[channel][j].get()->GetBinContent(i);
  }

  //add nuisance parameter for MC stats;
  int index= channel.EqualTo("mu") ? i+m_templateHistsMap[channel].size()-1 : i+m_templateHistsMap[channel].size()-1+m_templateHistsMap["mu"][0].get()->GetNbinsX();
  double np_sum=(par[index]-1.)*(par[index]-1.)/(2*sumerr2/(sum_nopar*sum_nopar));

  // std::cout<<"NP penalty is "<<np_sum<<std::endl;

  // std::cout<<"Index is"<<index<<"Bin is"<< i <<std::endl;

  double sum=0;
  for(unsigned int j=0; j<m_templateHistsMap[channel].size(); j++){
    sum+=(par[j]*par[index]*m_templateHistsMap[channel][j].get()->GetBinContent(i));
    
  }

  double value=0.;

  
  if(!(sum>0.)) std::cout<<"MC prediction is zero in bin" <<i<<"!!! Should be caught by rebinning. Data is:"<< y_i<<std::endl;

  if(y_i<0) value=0.;
  else if(!(sum>0.)) value=0;
  else value=y_i*TMath::Log(sum)-sum - np_sum;
  
  return value;

}

double FitData::NegLogLikelihood(double* par){

  TString channel;

  double LH=0.;

	

  for(unsigned int i_chan=0; i_chan<m_chans.size(); i_chan++){

    channel=m_chans[i_chan];

    //std::cout<<"Data histogram has"<<m_dataHistMap[channel].get()->GetNbinsX()<<" bins"<<std::endl;

    int bin_low=(m_dataHistMap[channel].get())->FindBin(-10);
    int bin_high=(m_dataHistMap[channel].get())->FindBin(10);
    //int bin_low_0=(m_dataHistMap[channel].get())->FindBin(0.);


    for(int i_bin=1; i_bin<=(m_dataHistMap[channel].get()->GetNbinsX()); i_bin++){
		
      //for(int i_bin=bin_low; i_bin<=bin_high; i_bin++){
		  
      //if(i_bin==18 || i_bin==19) continue;
      LH+=this->NegLogSingleBinPoisson(i_bin,channel,par);
    }
  }

  //std::cout<<"Likelihood is "<<-LH<<std::endl;

  return -LH;
}

double FitData::NegLogLikelihoodRndmData(double* par){

  TString channel;

  double LH=0.;		

  for(unsigned int i_chan=0; i_chan<m_chans.size(); i_chan++){

    channel=m_chans[i_chan];

    //std::cout<<"Data histogram has"<<m_dataHistMap[channel].get()->GetNbinsX()<<" bins"<<std::endl;


    for(int i_bin=1; i_bin<=(m_dataHistMap[channel].get()->GetNbinsX()); i_bin++){		     
      LH+=this->NegLogSingleBinPoisson(i_bin,channel,par,true);
      //std::cout<<"Likelihood constribution is "<<this->NegLogSingleBinPoisson(i_bin,channel,par,true)<<std::endl;
      //std::cout<<"Likelihood is "<<-LH<<std::endl;
    }
  }

  //std::cout<<"Likelihood is "<<-LH<<std::endl;                                                                                        
  return -LH;
}


double FitData::NegLogLikelihoodRndmDataWithNP(double* par){

  TString channel;

  double LH=0.;

  for(unsigned int i_chan=0; i_chan<m_chans.size(); i_chan++){

    channel=m_chans[i_chan];

    for(int i_bin=1; i_bin<=(m_dataHistMap[channel].get()->GetNbinsX()); i_bin++){
      LH+=this->NegLogSingleBinPoissonWithNP(i_bin,channel,par,true);
    }
  }

  //std::cout<<"Likelihood is "<<-LH<<std::endl;                                                                                                                                                                                       
  return -LH;
}




double FitData::NegLogLikelihoodWithNP(double* par){

  TString channel;

  double LH=0.;

  for(unsigned int i_chan=0; i_chan<m_chans.size(); i_chan++){

    channel=m_chans[i_chan];

    for(int i_bin=1; i_bin<=(m_dataHistMap[channel].get()->GetNbinsX()); i_bin++){
      LH+=this->NegLogSingleBinPoissonWithNP(i_bin,channel,par);
    }
  }

  //std::cout<<"Likelihood is "<<-LH<<std::endl;                                                                                                                                                                                             
  
  return -LH;
}


double FitData::NegLogLikelihoodRndmTemplate(double* par){

  TString channel;

  double LH=0.;		

  for(unsigned int i_chan=0; i_chan<m_chans.size(); i_chan++){

    channel=m_chans[i_chan];

    for(int i_bin=1; i_bin<=(m_dataHistMap[channel].get()->GetNbinsX()); i_bin++){		     
      LH+=this->NegLogSingleBinPoisson(i_bin,channel,par,false,true);

    }
  }

  return -LH;
}

void FitData::MakeBootStrapData(){

 TString channel;

  for(unsigned int i_chan=0; i_chan<m_chans.size(); i_chan++){

    channel=m_chans[i_chan];
		
    if(!(m_BootStrapDataMap.find(channel)==m_BootStrapDataMap.end())) m_BootStrapDataMap[channel].clear();


    for(int i_bin=1; i_bin<=(m_dataHistMap[channel].get()->GetNbinsX()); i_bin++){
      m_BootStrapDataMap[channel].push_back(m_rndm->PoissonD(m_dataHistMap[channel].get()->GetBinContent(i_bin)));
    }
  }

}



void FitData::MakeBootStrapTemplates(){

  TString channel;
  std::vector<double> new_bin_contents;

  for(unsigned int i_chan=0; i_chan<m_chans.size(); i_chan++){

    channel=m_chans[i_chan];
		
    if(!(m_BootStrapTemplateMap.find(channel)==m_BootStrapTemplateMap.end())) m_BootStrapTemplateMap[channel].clear();
    for(unsigned int i_t=0; i_t<m_templateHistsMap[channel].size(); i_t++){
      new_bin_contents.clear();
      
      for(int i_bin=1; i_bin<=(m_templateHistsMap[channel][i_t].get()->GetNbinsX()); i_bin++){
	new_bin_contents.push_back(m_rndm->Gaus(m_templateHistsMap[channel][i_t].get()->GetBinContent(i_bin),m_templateHistsMap[channel][i_t].get()->GetBinError(i_bin)));
	if(new_bin_contents[i_bin-1]<=0.)new_bin_contents[i_bin-1]=1e-5;

	//std::cout<<"Old: "<<m_templateHistsMap[channel][i_t].get()->GetBinContent(i_bin)<<" New  "<<new_bin_contents[i_bin-1]<<std::endl;
	if(new_bin_contents[i_bin-1]==0) std::cout<<"Throw 0 value!!!"<<std::endl;
      }
      
      m_BootStrapTemplateMap[channel].push_back(new_bin_contents);
    }
  }	
}

void FitData::CheckHistograms(TString &channel){

  int nbins_data_tmp=0, nbins_mc_tmp=0;
  float xlow_data=0., xhigh_data=0.;
  float xlow_mc=0., xhigh_mc=0.;

  TH1D* data_hist;

  for(unsigned int i_chan=0; i_chan<m_chans.size(); i_chan++){
    data_hist=m_dataHistMap[channel].get();
    nbins_data_tmp=data_hist->GetSize()-2;
    xlow_data=data_hist->GetXaxis()->GetBinLowEdge(1);
    xhigh_data=data_hist->GetXaxis()->GetBinUpEdge(nbins_data_tmp);


    for(auto mc_hist :  m_templateHistsMap[channel]){

      nbins_mc_tmp=mc_hist.get()->GetSize()-2;
      xlow_mc=mc_hist.get()->GetXaxis()->GetBinLowEdge(1);
      xhigh_mc=mc_hist.get()->GetXaxis()->GetBinUpEdge(nbins_data_tmp);

      if(!(nbins_mc_tmp==nbins_data_tmp) || !(TMath::Abs(xlow_data-xlow_mc)<1e-5) || !(TMath::Abs(xhigh_data-xhigh_mc)<1e-5)){
	std::cerr<<"Error in FitData::checkHistograms(): For channel: "<<channel<<" Expected (Nbins,xlow,xhigh) ";
	std::cerr<<"("<<nbins_data_tmp<<","<<xlow_data<<","<<xhigh_data<<"), but got";
	std::cerr<<"("<<nbins_mc_tmp<<","<<xlow_mc<<","<<xhigh_mc<<")"<<std::endl;

      }

    }

  }

}

std::vector<double> FitData::AutoRebinHistograms(TString& channel, double statThr, int N_force){

  float n_mc_total=0.;
  float err_mc_total=0.;

  std::vector<float> n_mc_template(m_templateHistsMap[channel].size(),0.);
  std::vector<float> err_mc_template(m_templateHistsMap[channel].size(),0.);


  std::vector<double> xbins;
	
  bool createBin=true;
  bool binningChanged=false;
  int curr_bin=0;
	
  std::cout<<"=================================="<<std::endl;
  std::cout<<"| AutoMerger channel "<<channel<<std::endl;
  std::cout<<"|"<<std::endl;



  xbins.push_back(m_templateHistsMap[channel][0]->GetXaxis()->GetBinLowEdge(1));
  curr_bin=0;
	
  for(int i_bin=1; i_bin<=m_templateHistsMap[channel][0]->GetNbinsX(); i_bin++){

    if(createBin){ 
      n_mc_total=0.;
      err_mc_total=0;


      n_mc_template.assign(m_templateHistsMap[channel].size(),0.);
      err_mc_template.assign(m_templateHistsMap[channel].size(),0.);

    }


    for(auto& mc_hist :  m_templateHistsMap[channel]){
		  
      int pos = &mc_hist-&(m_templateHistsMap[channel][0]);
      //std::cout<<"pos: "<<pos<<std::endl;

      n_mc_template[pos]+=mc_hist.get()->GetBinContent(i_bin);
      err_mc_template[pos]+=mc_hist.get()->GetBinError(i_bin)*mc_hist.get()->GetBinError(i_bin);

      n_mc_total+=mc_hist.get()->GetBinContent(i_bin);
      err_mc_total+=mc_hist.get()->GetBinError(i_bin)*mc_hist.get()->GetBinError(i_bin);
    }

    //std::cout<<"difference is "<<i_bin+1-curr_bin<<std::endl;
    //std::cout<<"bin center is "<<m_templateHistsMap[channel][0]->GetBinCenter(i_bin)<<std::endl;
		
		
    bool hasStatsTemplates=true;
    for(unsigned int i=0; i<n_mc_template.size(); i++){
		  
      //if((TMath::Sqrt(err_mc_template[i])/n_mc_template[i])>0.25) hasStatsTemplates=false;
		  
      if(TMath::Abs(n_mc_template[i])<1e-10 || (TMath::Sqrt(err_mc_template[i])/n_mc_template[i])>statThr) hasStatsTemplates=false;

    }


		
    //if(n_mc_total>0./*(TMath::Sqrt(err_mc_total)/n_mc_total)<0.1 ){*/ //&& (((i_bin+1-curr_bin)>=N_force+1 && TMath::Abs(m_templateHistsMap[channel][0]->GetBinCenter(i_bin))>20.) || TMath::Abs(m_templateHistsMap[channel][0]->GetBinCenter(i_bin))<20.)){
		
    if(hasStatsTemplates /*n_mc_total>0. && (TMath::Sqrt(err_mc_total)/n_mc_total)<0.1*/  && (i_bin+1-curr_bin)>=N_force+1){
      //if(hasStatsTemplates){
      curr_bin=i_bin;
      createBin=true;
      xbins.push_back(m_templateHistsMap[channel][0]->GetXaxis()->GetBinLowEdge(i_bin+1));
    }else{
      binningChanged=true;
      createBin=false;
      std::cout<<"| Info: Merge bin "<<i_bin+1<<" into "<<curr_bin+1<<std::endl;
      if(i_bin==m_templateHistsMap[channel][0]->GetNbinsX()) xbins[xbins.size()-1]=m_templateHistsMap[channel][0]->GetXaxis()->GetBinLowEdge(i_bin+1);
										 
    } 
  }

  //xbins.push_back(m_templateHistsMap[channel][0]->GetXaxis()->GetBinUpEdge(m_templateHistsMap[channel][0]->GetNbinsX()));

  if(!binningChanged){

    std::cout<<"|"<<std::endl;
    std::cout<<"| Binning in channel "<<channel<<" is ok! Proceed to fit... "<<std::endl;
	

    return xbins;
  }

  int nbins=xbins.size()-1;
	
  std::cout<<"|"<<std::endl;
  std::cout<<"| New Binning in channel "<<channel<<" :"<<std::endl;
  std::cout<<"| NBins = "<<nbins<<std::endl;
  std::cout<<"| "<<std::endl;


  for(unsigned int i=0; i<xbins.size(); i++){

    std::cout<<"| [bin edge "<<i<<"] "<<xbins[i]<<std::endl;
  }

  std::cout<<"=================================="<<std::endl;


  //Rebin Data and MC histograms
  TH1D* temp_data=(TH1D*)m_dataHistMap[channel].get()->Rebin(nbins,m_dataHistMap[channel].get()->GetName(),&xbins[0]);
	
  m_dataHistMap[channel]=std::shared_ptr<TH1D>(new TH1D(*temp_data));

  std::cout<<"Data histogram has "<<m_dataHistMap[channel].get()->GetNbinsX()<<" bins"<<std::endl;                                                                                                                            


  for(unsigned int i_mc=0; i_mc<m_templateHistsMap[channel].size(); i_mc++){

    TH1D* temp_mc=(TH1D*) m_templateHistsMap[channel][i_mc].get()->Rebin(nbins,m_templateHistsMap[channel][i_mc].get()->GetName(),&xbins[0]);
    m_templateHistsMap[channel][i_mc]=std::shared_ptr<TH1D>(new TH1D(*temp_mc));
	  


  }

  return xbins;
}

void FitData::FixHistogramBins(TString& channel, std::vector<double>& xbins){
  int nbins=xbins.size()-1;

  std::cout<<"Rebin using fixed bins."<<std::endl;
  std::cout<<"Number of bins is "<<nbins<<std::endl;
  for(auto &el : xbins) std::cout<<"bin: "<<el<<std::endl;
  

  for(unsigned int i_mc=0; i_mc<m_templateHistsMap[channel].size(); i_mc++){
    TH1D* temp_mc=(TH1D*) m_templateHistsMap[channel][i_mc].get()->Rebin(nbins,m_templateHistsMap[channel][i_mc].get()->GetName(),&xbins[0]);
    m_templateHistsMap[channel][i_mc]=std::shared_ptr<TH1D>(new TH1D(*temp_mc));
	 
  }

  TH1D* temp_data=(TH1D*)m_dataHistMap[channel].get()->Rebin(nbins,m_dataHistMap[channel].get()->GetName(),&xbins[0]);
  m_dataHistMap[channel]=std::shared_ptr<TH1D>(new TH1D(*temp_data));


}

void FitData::MergeTemplates(int tmp1, int tmp2){

  std::cout<<"=================================="<<std::endl;
  std::cout<<"| Merging templates "<<tmp1<<" and "<<tmp2<<std::endl;
  std::cout<<"=================================="<<std::endl;


  for(unsigned int i=0; i<m_chans.size(); i++){
	
    m_templateHistsMap[m_chans[i]][tmp1].get()->Add(m_templateHistsMap[m_chans[i]][tmp2].get());

    std::cout<<"Merged, now erase:"<<m_templateHistsMap[m_chans[i]][tmp2].get()->GetName()<<std::endl;

    m_templateHistsMap[m_chans[i]][tmp2].reset();
    m_templateHistsMap[m_chans[i]].erase(m_templateHistsMap[m_chans[i]].begin()+tmp2);

    m_MC_HistNamesMap[m_chans[i]].erase(m_MC_HistNamesMap[m_chans[i]].begin()+tmp2);

  }

}

void FitData::KernelSmoothTemplates(float scale){


  std::cout<<"=================================="<<std::endl;
  std::cout<<"| Smooth templates using Gaussian kernel"<<std::endl;
  std::cout<<"=================================="<<std::endl;

  float entry, entry_numerator;

      float scale_previous=scale;  

  for(unsigned int i=0; i<m_chans.size(); i++){

    for(unsigned int j=0; j<m_templateHistsMap[m_chans[i]].size(); j++){
  
      TH1D* hist_temp=(TH1D*)m_templateHistsMap[m_chans[i]][j].get()->Clone();

      // Integral of the hist (ignoring underflow)
      float area=m_templateHistsMap[m_chans[i]][j].get()->Integral(1,m_templateHistsMap[m_chans[i]][j]->GetNbinsX());
  
      int Nbinsx = m_templateHistsMap[m_chans[i]][j].get()->GetNbinsX();

      std::vector<float> new_entries(Nbinsx,0.);


      std::cout<<"Scale: "<<scale<<std::endl;
      float lowbin,highbin;
      float gauss_scale=1;

      for(int i_bin=1; i_bin<=Nbinsx; i_bin++){
        // Shifts the bins 1 to the left
	new_entries[i_bin-1]=m_templateHistsMap[m_chans[i]][j].get()->GetBinContent(i_bin);
	if(TMath::Abs(m_templateHistsMap[m_chans[i]][j].get()->GetBinLowEdge(i_bin))<10) continue;
        // why is -10 the highbin and 10 the lowbin?
	if(m_templateHistsMap[m_chans[i]][j].get()->GetBinLowEdge(i_bin)<=-10){
	  lowbin=1;
	  highbin=m_templateHistsMap[m_chans[i]][j].get()->FindBin(-10);
	  if(highbin>Nbinsx)highbin=Nbinsx;
	}else if(m_templateHistsMap[m_chans[i]][j].get()->GetBinLowEdge(i_bin)>=10){
	  lowbin=m_templateHistsMap[m_chans[i]][j].get()->FindBin(10);
	  if(lowbin<1) lowbin=1;
	  highbin=Nbinsx;
	}


	entry=0;
	entry_numerator=0;
	gauss_scale=scale;

	
	for(int j_bin=lowbin; j_bin<=highbin; j_bin++){
	  //if(i_bin==j_bin) continue;
	  //if(TMath::Abs(m_templateHistsMap[m_chans[i]][j].get()->GetBinLowEdge(j_bin))<10) continue;

	  float x_i_bin=m_templateHistsMap[m_chans[i]][j].get()->GetBinCenter(i_bin);
	  float x_j_bin=m_templateHistsMap[m_chans[i]][j].get()->GetBinCenter(j_bin);
	  
	  std::cout<<"difference: "<<x_i_bin-x_j_bin<<std::endl;
	  std::cout<<"gauss_scale: "<<gauss_scale<<std::endl;

	  entry+=TMath::Exp(-((x_i_bin-x_j_bin)*(x_i_bin-x_j_bin)/(2*gauss_scale*gauss_scale)))*m_templateHistsMap[m_chans[i]][j].get()->GetBinContent(j_bin);///m_templateHistsMap[m_chans[i]][j].get()->GetBinWidth(j_bin);
	  entry_numerator+=TMath::Exp(-((x_i_bin-x_j_bin)*(x_i_bin-x_j_bin)/(2*gauss_scale*gauss_scale)));
	}

	std::cout<<"entry: "<<entry<<std::endl;
	std::cout<<"entry_numerator: "<<entry_numerator<<std::endl;

        // replaces new_entries from earlier (bin content) with sum of distances btwn i and j weighted 
        // by j_bin content divided by sum of distances
        // distances are measured in # of sigma of a gaussians, with sigma = scale
	new_entries[i_bin-1]=entry/entry_numerator;//*m_templateHistsMap[m_chans[i]][j].get()->GetBinWidth(i_bin);
	std::cout<<"New entries for bin"<<i_bin<<": "<<new_entries[i_bin-1]<<std::endl;

	
      }

      
      for(int i_bin=1; i_bin<=Nbinsx; i_bin++){
	
	hist_temp->SetBinContent(i_bin,new_entries[i_bin-1]);
	hist_temp->SetBinError(i_bin,m_templateHistsMap[m_chans[i]][j].get()->GetBinError(i_bin));
	
      }

      //scale to match old norm
      hist_temp->Scale(area/hist_temp->Integral(1,m_templateHistsMap[m_chans[i]][j]->GetNbinsX()));

      std::shared_ptr<TH1D> hist_new(hist_temp);
      
      m_templateHistsMap[m_chans[i]][j]=hist_new;

    }

  }



}


void FitData::FitTemplateSlopes(float lowgap, float highgap){


  std::cout<<"=================================="<<std::endl;
  std::cout<<"| Smooth templates using functions fit to sides"<<std::endl;
  std::cout<<"=================================="<<std::endl;


  for(unsigned int i=0; i<m_chans.size(); i++){

    for(unsigned int j=0; j<m_templateHistsMap[m_chans[i]].size(); j++){
  
      TH1D* hist_temp=(TH1D*)m_templateHistsMap[m_chans[i]][j].get()->Clone();

      float area=m_templateHistsMap[m_chans[i]][j].get()->Integral(1,m_templateHistsMap[m_chans[i]][j]->GetNbinsX());
    

      //left hand side: fit gaussian
    
      TF1 *g1= new TF1("g1","gaus",-40.,lowgap);
      g1->FixParameter(1,0.);

      TF1 *g2= new TF1("g2","expo",highgap,80.);
      
      TF1 *dg= new TF1("dg","gaus(0)+gaus(3)",-40.,lowgap);

      
      TCanvas *c = new TCanvas("c","",800,700);
      c->cd();
      
      TH1D *hist_temp0=(TH1D*)hist_temp->Clone();
      
      hist_temp0->SetLineColor(1);
      //hist_temp0->Draw("HIST");
      
      for(int i_bin=1; i_bin<=hist_temp->GetNbinsX(); i_bin++){
	
	//hist_temp->SetBinContent(i_bin,hist_temp->GetBinContent(i_bin)/hist_temp->GetBinWidth(i_bin));
	
      }
    
      hist_temp->Fit(g1,"","",-40.,lowgap);
      hist_temp->Fit(g2,"+","",highgap,80.);


      double par[6];
      g1->GetParameters(&par[0]);

      g1->GetParameters(&par[3]);

      par[3]*=1e-2;
      par[6]*=10;

      dg->SetParameters(par);

      dg->FixParameter(1,0.);
      dg->FixParameter(4,0.);

      hist_temp->Fit(dg,"+","",-40.,lowgap);
      


      TH1D* hist_temp2=(TH1D*)hist_temp->Clone();

      hist_temp2->SetLineColor(2);
      hist_temp2->Draw("HIST");
      hist_temp->GetFunction("dg")->SetLineColor(2);
      hist_temp->GetFunction("dg")->Draw("lsame");
      hist_temp->GetFunction("g2")->SetLineColor(4);
      hist_temp->GetFunction("g2")->Draw("lsame");
    

      for(int i_bin=1; i_bin<=hist_temp->GetNbinsX(); i_bin++){
	float xbin=hist_temp->GetBinCenter(i_bin);
	//float xbinwidth=hist_temp->GetBinWidth(i_bin);
	if(xbin<=lowgap) hist_temp->SetBinContent(i_bin,hist_temp->GetFunction("dg")->Eval(xbin));
	else if(xbin>=highgap) hist_temp->SetBinContent(i_bin,hist_temp->GetFunction("g2")->Eval(xbin));
	else hist_temp->SetBinContent(i_bin,hist_temp->GetBinContent(i_bin));
      }

      TH1D* hist_temp3=(TH1D*)hist_temp->Clone();
      hist_temp3->SetLineColor(4);
      hist_temp3->Draw("histsame");
      c->SetLogy();

      TString name=TString("./ctrl_plots/fitslopes_ctrl_")+TString(hist_temp->GetName())+TString(".pdf");
      c->SaveAs(name.Data());
      delete c;
    
      
      //scale to match old norm                                                                                                                                                        
      //hist_temp->Scale(area/hist_temp->Integral(1,m_templateHistsMap[m_chans[i]][j]->GetNbinsX()));
      std::shared_ptr<TH1D> hist_new(hist_temp);
      m_templateHistsMap[m_chans[i]][j]=hist_new;

    }
    
  }

}


void FitData::RescaleTemplate(int tmp1, double factor, bool doKeepNorm){

  std::cout<<"=================================="<<std::endl;
  std::cout<<"| Rescale template "<<tmp1<<" by factor "<<factor<<std::endl;
  if(doKeepNorm) std::cout<<"| Keep norm constant "<<std::endl;
  else std::cout<<"| Allow norm to vary "<<std::endl;
  std::cout<<"=================================="<<std::endl;

  double flav_frac_tmp1;
  double flav_frac_others=0;
  double factor_others;

  for(unsigned int i=0; i<m_chans.size(); i++){
		
    m_templateHistsMap[m_chans[i]][tmp1].get()->Scale(factor);
	       

    if(doKeepNorm){
			
      flav_frac_others=0;

      for(unsigned int j=0; j<m_templateHistsMap[m_chans[i]].size(); j++){
	if(j==tmp1) flav_frac_tmp1=this->GetFlavourFraction(m_chans[j],j);
	else flav_frac_others+=this->GetFlavourFraction(m_chans[j],j);
      }
			
      factor_others=(1.-factor)/(flav_frac_others);
			
      int i_hist=0;
      for(auto mc_hist :  m_templateHistsMap[m_chans[i]]){
	if(i_hist==tmp1) continue;
	else mc_hist.get()->Scale(factor_others);

	i_hist++;
      }
    }

  }
}

double FitData::GetFlavourFraction(TString &channel, int i_flav_pair){

  double integral_all=0., integral_hist=0.;

  for(unsigned int i_hist=0; i_hist<m_templateHistsMap[channel].size(); i_hist++){

    integral_all+=m_templateHistsMap[channel][i_hist]->Integral(1,m_templateHistsMap[channel][i_hist]->GetNbinsX());

    if(i_hist==i_flav_pair){
      integral_hist+=m_templateHistsMap[channel][i_hist]->Integral(1,m_templateHistsMap[channel][i_hist]->GetNbinsX());
    }

  }

  return integral_hist/integral_all;
}

double FitData::GetFlavourNorm(TString &channel, int i_flav_pair){

  return m_templateHistsMap[channel][i_flav_pair]->Integral();
 
}

double FitData::GetDataNorm(TString &channel){

  return m_dataHistMap[channel]->Integral();

}


void FitData::ReadInHistograms(TString &channel, int smooth_Ntimes){

  TH1D* tmp, *clone_tmp;

  m_infile.get()->GetObject(m_Data_HistNamesMap[channel].Data(),tmp);

  if(tmp) std::cout<<"Read in "<<m_Data_HistNamesMap[channel]<<std::endl;
  else std::cout<<"Error in FitData::ReadInHistograms(): Can't find histogram "<<m_Data_HistNamesMap[channel]<<"!"<<std::endl;

  clone_tmp=(TH1D*)tmp->Clone();
  clone_tmp->SetDirectory(0);

  m_dataHistMap[channel]=std::shared_ptr<TH1D>(clone_tmp);

  for(unsigned int i=0; i<m_MC_HistNamesMap[channel].size(); i++){

    m_infile.get()->GetObject(m_MC_HistNamesMap[channel][i],tmp);

    if(tmp) std::cout<<"Read in "<<m_MC_HistNamesMap[channel][i]<<std::endl;
    else std::cout<<"Error in FitData::ReadInHistograms(): Can't find histogram "<<m_MC_HistNamesMap[channel][i]<<std::endl;

    clone_tmp=(TH1D*)tmp->Clone();
    clone_tmp->SetDirectory(0);
    if(smooth_Ntimes) clone_tmp->Smooth(smooth_Ntimes);

    m_templateHistsMap[channel].push_back(std::shared_ptr<TH1D>(clone_tmp));
  }


}


void FitData::PrintHistograms(TFile* file){



  std::cout<<"=================================="<<std::endl;
  std::cout<<"| Print modified templates to file"<<std::endl;
  std::cout<<"=================================="<<std::endl;

  float entry;



  for(unsigned int i=0; i<m_chans.size(); i++){

    for(unsigned int j=0; j<m_templateHistsMap[m_chans[i]].size(); j++){

      file->cd();
      m_templateHistsMap[m_chans[i]][j].get()->Write();

    }
  }


}


float FitData::GetParamStartValue(){

  TH1D* data_tmp, *mc_tmp;

  for(unsigned int i_chan=0; i_chan<m_chans.size(); i_chan++){

    for(unsigned int j=0; j<m_templateHistsMap[m_chans[i_chan]].size(); j++){
     
      if(i_chan==0 && j==0) mc_tmp=(TH1D*)m_templateHistsMap[m_chans[i_chan]][j].get()->Clone();
      else mc_tmp->Add(m_templateHistsMap[m_chans[i_chan]][j].get());

    }

    if(i_chan==0)data_tmp=(TH1D*)m_dataHistMap[m_chans[i_chan]].get()->Clone();
    else data_tmp->Add(m_dataHistMap[m_chans[i_chan]].get());

  }


  float norm_data=data_tmp->Integral();
  float norm_mc=mc_tmp->Integral();

  return norm_data/norm_mc;

}

void FitData::GetMCStatsNPInitVal(TString &channel, std::vector<double>& par, std::vector<double>& par_err ){

  TH1D* mc_tmp;

  for(unsigned int j=0; j<m_templateHistsMap[channel].size(); j++){

    if(j==0) mc_tmp=(TH1D*)m_templateHistsMap[channel][j].get()->Clone();
    else mc_tmp->Add(m_templateHistsMap[channel][j].get());

  }


  for(int i=1; i<=mc_tmp->GetNbinsX(); i++){
    std::cout<<"Bin"<<i<<": "<<mc_tmp->GetBinContent(i)<<" +- "<<mc_tmp->GetBinError(i)<<std::endl;

    par.push_back(1.);
    par_err.push_back(mc_tmp->GetBinError(i)/mc_tmp->GetBinContent(i));

  }

}
