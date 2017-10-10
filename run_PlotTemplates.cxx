#include "SampleInfo.h"
#include "HistPlotter.h"
#include "TFile.h"
#include "TString.h"


int main(int argc,char *argv[]){

	bool doRebin=false;
	int i_rebin=3;


        float lumi=2959.7;

        TFile *outfile=new TFile("testoutput/Mufilter_Sd0distributions.root","RECREATE");

        SampleInfo info;
        info.ReadConfig("data/SampleInfo_MuonFilterTest.txt");

        TString input_folder="/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output/";

	std::vector<TString> fl_pairs={"BB","BL","CC","CL","LL"};
	std::vector<TString> fl_single={"B","C","L"};


	std::vector<TString> mc_hists;
        TString data_hist;
	
	std::vector<TString> chan={"muojet","nonmuojet"};

	std::vector<TString> mu_bins={"l100","g100l200","g200"};                                                                                                                                                                             
        std::vector<TString> nonmu_bins={"l20","g20l50","g50l80","g80"};

	std::vector<TString> pt_all_regions;

        for(auto mb : mu_bins){

		for(auto nmb :nonmu_bins){

			pt_all_regions.push_back("_mjpt_"+mb+"_nmjpt_"+nmb);

		}

        }

	pt_all_regions.push_back(""); //inclusive Sd0 histogram



	for(int i=0; i<chan.size(); i++){
		
		for(int j=0; j<pt_all_regions.size(); j++){
		
			for(auto name : fl_pairs){
				mc_hists.push_back(chan[i]+"_"+name+pt_all_regions[j]+"_maxSd0");
			}
			

			//data_hist=chan[i]+"_other_"+pt_all_regions[j]+"_maxSd0";
			data_hist="";

			HistPlotter plot0a(input_folder,&info);
			plot0a.setLumi(lumi); //in pb^-1                                                                                                                                                                                                     
			plot0a.setXaxisLimits(-40,99);
			plot0a.setYaxisLimits(-1,-1); //auto range                                                                                                                                                                                           
			//plot0a.setRebinFactor(2);                                                                                                                                                                                                          
			plot0a.setXaxisTitle("S_{d_{0}}");
			if(doRebin) plot0a.setRebinFactor(i_rebin);
			plot0a.makeDataStackedMCPlot(mc_hists,data_hist);
			plot0a.addLabel();
			plot0a.Save(outfile,chan[i]+pt_all_regions[j]+"_maxSd0_pretag");
			
			mc_hists.clear();
			
			for(auto name : fl_pairs){
			  mc_hists.push_back(chan[i]+"_"+name+pt_all_regions[j]+"_maxSd0_PREFITPOSTTAG");
			}
			
			//data_hist=chan[i]+"_other_"+pt_all_regions[j]+"_maxSd0_POSTTAG";
			data_hist="";

			HistPlotter plot0c(input_folder,&info);
			plot0c.setLumi(lumi); //in pb^-1                                                                                                                                                                                                      
			plot0c.setXaxisLimits(-40,99);
			plot0c.setYaxisLimits(-1,-1); //auto range                                                                                                                                                                                            
			if(doRebin) plot0c.setRebinFactor(i_rebin);                                                                                                                                                                                                          
			plot0c.setXaxisTitle("S_{d_{0}}");
			//plot0c.setSysList(sys_list);
			plot0c.makeDataStackedMCPlot(mc_hists,data_hist);
			plot0c.addLabel();
			plot0c.Save(outfile,chan[i]+pt_all_regions[j]+"_maxSd0_posttag");
			mc_hists.clear();



	
		}
	}
}
