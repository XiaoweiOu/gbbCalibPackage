/*                                                                                                                                                            
 * CrossCheckPlotter.cxx                                                                                                                                                
 *                                                                                                                                                            
 *  Created on: April 12, 2016                                                                                                                                  
 *      Author: rjacobs                                                                                                                                       
 */

#include "CrossCheckPlotter.h"

CrossCheckPlotter::CrossCheckPlotter(){
        // TODO Auto-generated constructor stub                                                                                                               

}

CrossCheckPlotter::~CrossCheckPlotter() {
        // TODO Auto-generated destructor stub                                                                                                                

	//delete m_pad1;                                                                                                                                            
	// delete m_pad2;                                                                                                                                           
	//delete m_canv;                                                                                                                                            


}

CrossCheckPlotter::CrossCheckPlotter(TString in_dir, SampleInfo* info,TString name) : Plotter(in_dir,info,name){
}



void CrossCheckPlotter::makeTriggerTurnOnPlot(TString hist_name_num, TString hist_name_denom){

	std::vector<TString> hist_names_num={hist_name_num};
	std::vector<TString> hist_names_denom={hist_name_denom};
        TString data_hist_name="";//"trigjet_pt_noHLTreq";

        HistogramStack stack_num=readInHistStack(hist_names_num,data_hist_name);
	HistogramStack stack_denom=readInHistStack(hist_names_denom,data_hist_name);

        TH1* mc_hist_num=(TH1*)stack_num.full_mc_hist.get()->Clone();
	TH1* mc_hist_denom=(TH1*)stack_denom.full_mc_hist.get()->Clone();

	if(!mc_hist_num)std::cout<<"Could not find histogram: "<<hist_name_num<<std::endl;
	if(!mc_hist_denom)std::cout<<"Could not find histogram: "<<hist_name_denom<<std::endl;


	mc_hist_num->Divide(mc_hist_denom);

        this->getCanvas().get()->cd();
	
	this->setPad1Borders(0.,0.,1.,1.);
	this->getPad1()->Draw();

	this->getPad1()->cd();

	mc_hist_num->Draw();

}

void CrossCheckPlotter::makeCrossCheckFitTemplates(std::vector<TString>& hist_names_even, std::vector<TString>& hist_names_odd,TFile *outfile,TString sys_ext,bool doRescale){

	//make even/odd templates.

	std::vector<TString> data_names; //dummy don't need data here
        MultiHistogramStack stack_even=readInMultiHistStack(hist_names_even,data_names);
        MultiHistogramStack stack_odd=readInMultiHistStack(hist_names_odd,data_names);

	TH1* tmp_stacked_mc_even=0, *tmp_stacked_mc_odd=0, *sum_stacked_mc_odd=0;

	TH1F th1f_stacked_mc_even, th1f_stacked_mc_odd;

	std::vector<std::shared_ptr<TH1>> mc_stack_hists_even=stack_even.mc_stack_hists;
	std::vector<std::shared_ptr<TH1>> mc_stack_hists_odd=stack_odd.mc_stack_hists;

	TString hist_name,new_hist_name;
	TObjArray* tokens;

	//for even histograms same procedure as before, rename and write to outfile

	for(int i=0; i<mc_stack_hists_even.size(); i++){

		if(!mc_stack_hists_even[i].get()) continue;
		tmp_stacked_mc_even=(TH1*)mc_stack_hists_even[i].get()->Clone();

		hist_name=tmp_stacked_mc_even->GetName();
		tokens=hist_name.Tokenize('_');
		if(tokens->GetLast()>=7) new_hist_name="h"+((TObjString*) (*tokens)[1])->GetString()+sys_ext+"_"+((TObjString*) (*tokens)[0])->GetString()+"_"+((TObjString*) (*tokens)[2])->GetString()+"_"+((TObjString*) (*tokens)[3])->GetString()+"_"+((TObjString*) (*tokens)[4])->GetString()+"_"+((TObjString*) (*tokens)[5])->GetString()+"_obs_cuts";
		else new_hist_name="h"+((TObjString*) (*tokens)[1])->GetString()+sys_ext+"_"+((TObjString*) (*tokens)[0])->GetString()+"_inclusive_obs_cuts";

		tmp_stacked_mc_even->SetName(new_hist_name);

		tmp_stacked_mc_even->Copy(th1f_stacked_mc_even);

		outfile->cd();
		th1f_stacked_mc_even.Write();

	}


	//for MC closure test: for odd histograms, pretend they are Data, add 5 of them up and write to file
	
	int i_add=0;
	bool writtenHisto=false;

	std::vector<double> total_norms;

	for(int i=0; i<mc_stack_hists_odd.size(); i++){

                if(!mc_stack_hists_odd[i].get()) continue;
                tmp_stacked_mc_odd=(TH1*)mc_stack_hists_odd[i].get()->Clone();

                hist_name=tmp_stacked_mc_odd->GetName();
                tokens=hist_name.Tokenize('_');
		
		if(i==0 || writtenHisto){
			sum_stacked_mc_odd=tmp_stacked_mc_odd;
			writtenHisto=false;
			i_add=0;
		}else if(i_add<4){
                        sum_stacked_mc_odd->Add(tmp_stacked_mc_odd);
                        writtenHisto=false;
                        i_add++;
                }


		if(i_add==4){
	
			if(tokens->GetLast()>=7) new_hist_name="hDataNom_"+((TObjString*) (*tokens)[0])->GetString()+"_"+((TObjString*) (*tokens)[2])->GetString()+"_"+((TObjString*) (*tokens)[3])->GetString()+"_"+((TObjString*) (*tokens)[4])->GetString()+"_"+((TObjString*) (*tokens)[5])->GetString()+"_obs_cuts";
			else new_hist_name="hDataNom_"+((TObjString*) (*tokens)[0])->GetString()+"_inclusive_obs_cuts";

			sum_stacked_mc_odd->SetName(new_hist_name);

			sum_stacked_mc_odd->Copy(th1f_stacked_mc_odd);

			std::cout<<"Write Hist "<<new_hist_name<<" i_add="<<i_add<<" i="<<i<<std::endl;

			outfile->cd();
			th1f_stacked_mc_odd.Write();
			total_norms.push_back(th1f_stacked_mc_odd.Integral());
			writtenHisto=true;

		}

	
	}



	if(!doRescale) return;

	double total_norm=0.;

	double rescale_BB=0., rescale_others=0.;

        //for linearity check: vary b-fraction in odd sample from 1% to 15% change data name
                                                                                    
	std::vector<double> perc={0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.1,0.15};
	std::vector<TH1*> h_temps(perc.size(),0);

	i_add=0;
	writtenHisto=false;

	int i_hist=0;

        for(int i=0; i<mc_stack_hists_odd.size(); i++){

                if(!mc_stack_hists_odd[i].get()) continue;
                tmp_stacked_mc_odd=(TH1*)mc_stack_hists_odd[i].get()->Clone();

                hist_name=tmp_stacked_mc_odd->GetName();
                tokens=hist_name.Tokenize('_');

                if(i==0 || writtenHisto){
			
			rescale_BB=tmp_stacked_mc_odd->Integral()/total_norms[i_hist];
                        for(int i_p=0; i_p<perc.size(); i_p++){
				h_temps[i_p]=(TH1*)tmp_stacked_mc_odd->Clone();
				h_temps[i_p]->Scale(perc[i_p]/rescale_BB);
				std::cout<<"integral: "<<h_temps[i_p]->Integral()/total_norms[i_hist]<<" target"<<perc[i_p]<<std::endl;
			}
                        writtenHisto=false;
                        i_add=0;
                }else if(i_add<4){

			for(int i_p=0; i_p<perc.size(); i_p++){
			        rescale_others=(1-perc[i_p])/(1-rescale_BB);
				h_temps[i_p]->Add(tmp_stacked_mc_odd,rescale_others);
			}
                        writtenHisto=false;
                        i_add++;
                }


                if(i_add==4){

			for(int i_p=0; i_p<perc.size(); i_p++){

				TString help;
				help.Form("%g",perc[i_p]*100);
				if(tokens->GetLast()>=7) new_hist_name="hDataScale"+help+"Nom_"+((TObjString*) (*tokens)[0])->GetString()+"_"+((TObjString*) (*tokens)[2])->GetString()+"_"+((TObjString*) (*tokens)[3])->GetString()+"_"+((TObjString*) (*tokens)[4])->GetString()+"_"+((TObjString*) (*tokens)[5])->GetString()+"_obs_cuts";
				else new_hist_name="hDataScale"+help+"Nom_"+((TObjString*) (*tokens)[0])->GetString()+"_inclusive_obs_cuts";

				h_temps[i_p]->SetName(new_hist_name);

				h_temps[i_p]->Copy(th1f_stacked_mc_odd);

				std::cout<<"Write Hist "<<new_hist_name<<" i_add="<<i_add<<" i="<<i<<std::endl;

				outfile->cd();
				th1f_stacked_mc_odd.Write();


			}
			i_hist++;
                        writtenHisto=true;

                }


        }




}


void CrossCheckPlotter::makeBBEffPlot(TString type){



	std::vector<TString> hist_names={type+"_dijetcat_BB_pt",type+"_dijetcat_BL_pt",type+"_dijetcat_CC_pt",type+"_dijetcat_CL_pt",type+"_dijetcat_LL_pt"};
        TString data_hist_name=type+"_dijetcat_other_pt";

	HistogramStack stack=readInHistStack(hist_names,data_hist_name);

	std::vector<std::shared_ptr<TH1>> mc_stack_hists=stack.mc_stack_hists;
	 TH1* full_mc_hist=(TH1*)stack.full_mc_hist.get()->Clone();

	//Make MC efficiency plot

	TH1* BB_mc_hist=(TH1*)mc_stack_hists[0].get()->Clone();
	BB_mc_hist->Divide(full_mc_hist);

	this->getCanvas().get()->cd();

        this->setPad1Borders(0.,0.,1.,1.);
	this->getPad1()->Draw();

	//set styles and draw
	h2_style(BB_mc_hist);
	BB_mc_hist->SetLabelSize(0,"X");
	BB_mc_hist->SetYTitle("BB efficiency");
	BB_mc_hist->SetMarkerStyle(20);
	this->getPad1()->cd();

	BB_mc_hist->Draw("EP");


	//make post-fit comparison

	std::vector<TString> postfit_hist_names={type+"_dijetcat_BB_pt_POSTFIT",type+"_dijetcat_BL_pt_POSTFIT",type+"_dijetcat_CC_pt_POSTFIT",type+"_dijetcat_CL_pt_POSTFIT",type+"_dijetcat_LL_pt_POSTFIT"};
        TString postfit_data_hist_name=type+"_dijetcat_other_pt";

	HistogramStack postfit_stack=readInHistStack(postfit_hist_names,postfit_data_hist_name);

	std::vector<std::shared_ptr<TH1>> postfit_mc_stack_hists=postfit_stack.mc_stack_hists;
	TH1* postfit_full_mc_hist=(TH1*)postfit_stack.full_mc_hist.get()->Clone();

	//Make MC efficiency plot                                                                                                                                                                                                            

        TH1* postfit_BB_mc_hist=(TH1*)postfit_mc_stack_hists[0].get()->Clone();
        postfit_BB_mc_hist->Divide(postfit_full_mc_hist);
	postfit_BB_mc_hist->SetMarkerColor(2);
	
	//postfit_BB_mc_hist->Draw("EP same");




}


void CrossCheckPlotter::makeBinBBEffPlot(TString bin){



	std::vector<TString> hist_names={"muojet_dijetcat_BB_"+bin+"_pt","muojet_dijetcat_BL_"+bin+"_pt","muojet_dijetcat_CC"+bin+"_pt","muojet_dijetcat_CL_"+bin+"_pt","muojet_dijetcat_LL"+bin+"_pt"};
        TString data_hist_name="muojet_dijetcat_other_"+bin+"_pt";

        HistogramStack stack=readInHistStack(hist_names,data_hist_name);

	std::vector<std::shared_ptr<TH1>> mc_stack_hists=stack.mc_stack_hists;
	TH1* full_mc_hist=(TH1*)stack.full_mc_hist.get()->Clone();

        //Make MC efficiency plot                                                                                                                                                                                                            

        TH1* BB_mc_hist=(TH1*)mc_stack_hists[0].get()->Clone();
        BB_mc_hist->Divide(full_mc_hist);

        this->getCanvas().get()->cd();

        this->setPad1Borders(0.,0.,1.,1.);
        this->getPad1()->Draw();

        //set styles and draw                                                                                                                                                                                                                
        h2_style(BB_mc_hist);
	BB_mc_hist->SetLabelSize(0.04,"X");
	BB_mc_hist->SetYTitle("BB efficiency");
	BB_mc_hist->SetMarkerStyle(20);
	this->getPad1()->cd();

	BB_mc_hist->Draw("EP");


}



void CrossCheckPlotter::makeTemplateEffPlot(TString type){



	std::vector<TString> hist_names={type+"_dijetcat_BB_pt",type+"_dijetcat_BL_pt",type+"_dijetcat_CC_pt",type+"_dijetcat_CL_pt",type+"_dijetcat_LL_pt"};
        TString data_hist_name=type+"_dijetcat_other_pt";

        HistogramStack stack=readInHistStack(hist_names,data_hist_name);

	std::vector<std::shared_ptr<TH1>> mc_stack_hists=stack.mc_stack_hists;
	TH1* full_mc_hist=(TH1*)stack.full_mc_hist.get()->Clone();

        //Make MC efficiency plot                                                                                                                                                                                                           
	TH1* BB_mc_hist=(TH1*)mc_stack_hists[0].get()->Clone();

	BB_mc_hist->Divide(full_mc_hist);

	this->getCanvas().get()->cd();

	//	this->setPad1Borders(0.,0.,1.,1.);
	//this->getPad1()->Draw();

        //set styles and draw                                                                                                                                                                                                                
        h2_style(BB_mc_hist);
	BB_mc_hist->SetLabelSize(0,"X");
        BB_mc_hist->SetYTitle("FF efficiency");
	BB_mc_hist->SetMarkerStyle(20);
	this->getPad1()->cd();
	BB_mc_hist->Draw("P");

	TH1* BL_mc_hist=(TH1*)mc_stack_hists[1].get()->Clone();
        BL_mc_hist->Divide(full_mc_hist);
	BL_mc_hist->SetMarkerColor(2);
	this->getPad1()->cd();
        BL_mc_hist->Draw("P same");

	TH1* CC_mc_hist=(TH1*)mc_stack_hists[2].get()->Clone();
        CC_mc_hist->Divide(full_mc_hist);
        CC_mc_hist->SetMarkerColor(3);
        this->getPad1()->cd();
        CC_mc_hist->Draw("P same");

	TH1* CL_mc_hist=(TH1*)mc_stack_hists[3].get()->Clone();
        CL_mc_hist->Divide(full_mc_hist);
        CL_mc_hist->SetMarkerColor(4);
        this->getPad1()->cd();
        CL_mc_hist->Draw("P same");

	TH1* LL_mc_hist=(TH1*)mc_stack_hists[4].get()->Clone();
	LL_mc_hist->Divide(full_mc_hist);
	LL_mc_hist->SetMarkerColor(6);
	this->getPad1()->cd();
	LL_mc_hist->Draw("P same");

	TH1* BB_for_ratio=(TH1*)mc_stack_hists[0].get()->Clone();

	TH1* ratio[4];
	
	for(int i=1; i<5; i++){
		ratio[i-1] = (TH1*)mc_stack_hists[i].get()->Clone();
		ratio[i-1]->Divide(BB_for_ratio);
		if(i<4)ratio[i-1]->SetMarkerColor(i+1);
		else ratio[i-1]->SetMarkerColor(i+2);

                ratio[i-1]->SetYTitle("ratio to BB");
                this->getPad2()->cd();
                this->getPad2()->SetGrid();
                ratio[i-1]->SetXTitle("track jet p_{T} [GeV]");
	
		if(i==1)ratio[i-1]->Draw("P");
		else ratio[i-1]->Draw("P same");

		ratio_style(ratio[i-1]);


	}



}




void CrossCheckPlotter::makeMCStatsTestFile(std::vector<TString>& hist_names, std::vector<TString>& data_names, TFile* outfile){


	MultiHistogramStack stack=readInMultiHistStack_unweighted(hist_names,data_names);


	std::vector<std::shared_ptr<TH1>> mc_stack_hists=stack.mc_stack_hists;
	
	for(int i=0; i<mc_stack_hists.size(); i++){
    
		if(!mc_stack_hists[i].get()) continue;
		outfile->cd();
		mc_stack_hists[i].get()->Write();


	}
	

}


MultiHistogramStack CrossCheckPlotter::readInMultiHistStack_unweighted(std::vector<TString>& hist_names, std::vector<TString>& data_names){

	//Read in Stack of multiple data histograms and multiple MC histograms (for Templates)

	this->setStackedHists(hist_names);
	
	std::vector<TString> stacked_hist_names=*(this->getStackedHistsPtr());


	TH1* data_hist=0, *tmp=0, *full_mc=0, *mc_stack_tmp=0, *data_stack_tmp=0;

	std::vector<std::shared_ptr<TH1>> mc_stack_hists(stacked_hist_names.size(),0);
	std::vector<std::shared_ptr<TH1>> data_stack_hists(data_names.size(),0);

	TString filename, sample_name;

	bool haveDataHist=false, haveFullMCHist=false;

	TFile *f;

	std::vector<TString> samples=*(this->getSamplesPtr());

	for(int i_samp=0; i_samp<samples.size(); i_samp++){

		filename=samples.at(i_samp);
		sample_name=filename(filename.Last('/')+1,filename.Length());
		filename="/merged_hist_"+sample_name;

		std::cout<<"Open file: "<<samples.at(i_samp)+filename<<std::endl;

		f=TFile::Open((samples.at(i_samp)+filename).Data(),"READ");
    
		if(samples.at(i_samp).Contains("data15")){

			
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


			for(int i_hist=0; i_hist<stacked_hist_names.size(); i_hist++){

				f->GetObject(stacked_hist_names.at(i_hist),tmp);
				if(!tmp) continue;
          
				if(mc_stack_hists[i_hist]==0){ //first histogram in this category  
            
					 
					mc_stack_tmp=(TH1*)tmp->Clone();

					mc_stack_tmp->SetDirectory(0);
					mc_stack_hists[i_hist]=std::shared_ptr<TH1>(mc_stack_tmp);
					
					   

				}else{
					  
							  
					mc_stack_hists[i_hist].get()->Add(tmp);
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



void CrossCheckPlotter::makePileupComparisonPlot(std::vector<TString>& mc_histo_names){
	//histo names are stubs, without the PUmu_l
	//PUmu bins: l10, g10l15, g15l20, g20


	std::vector<TString> bin= {"l10","g10l15", "g15l20", "g20" };
	
	
	std::vector<TString> hist_names;
	std::vector<std::vector<TString>> all_names;

	for(int ibin=0; ibin<bin.size(); ibin++){

		hist_names.clear();

		for(int ihist=0; ihist<mc_histo_names.size(); ihist++){

			hist_names.push_back(mc_histo_names[ihist]+"_PUmu_"+bin[ibin]);
			

		}

		all_names.push_back(hist_names);

	}

	TString data_hist_name="fatjet_other_pt"; //dummy


	for(int i_plot=0; i_plot<all_names.size(); i_plot++){

		HistogramStack stack=readInHistStack(all_names[i_plot],data_hist_name);
		
		std::vector<std::shared_ptr<TH1>> mc_stack_hists=stack.mc_stack_hists;

		TH1* full_mc_hist=(TH1*)stack.full_mc_hist.get()->Clone();
		
		full_mc_hist->Scale(1./full_mc_hist->Integral());
		
		this->getCanvas().get()->cd();
		
		this->setPad1Borders(0.,0.,1.,1.);
		this->getPad1()->Draw();
		
		//set styles and draw                                                                                                                                                                                                               
		
		h2_style(full_mc_hist);
		full_mc_hist->SetLabelSize(0.04,"X");
		full_mc_hist->SetYTitle("#events");
		full_mc_hist->SetLineColor(i_plot+1);

		this->getPad1()->cd();
		
		if(i_plot==0) full_mc_hist->Draw("HIST");
		else full_mc_hist->Draw("HIST same");
	}


}
