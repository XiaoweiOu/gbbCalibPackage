#include "SampleInfo.h"
#include "HistPlotter.h"
#include "TFile.h"
#include "TString.h"


int main(int argc,char *argv[]){

	bool doRebin=false;
	int i_rebin=3;


        float lumi=3193.68;

        TFile *outfile=new TFile("161206_MUFilterTest_Sd0_WITHFILTER.root","RECREATE");

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
	pt_all_regions.push_back("");//inclusive in pt


	for(int i=0; i<chan.size(); i++){
		
		for(int j=0; j<pt_all_regions.size(); j++){
		
			for(auto name : fl_pairs){
				mc_hists.push_back(chan[i]+"_"+name+pt_all_regions[j]+"_maxSd0");
			}
			
			data_hist="";//chan[i]+"_other_"+pt_all_regions[j]+"_maxSd0";
			
			HistPlotter plot0a(input_folder,&info);
			plot0a.setLumi(lumi); //in pb^-1                                                                                                                                                                                                     
			plot0a.setXaxisLimits(-40,99);
			plot0a.setYaxisLimits(-1,-1); //auto range                                                                                                                                                                                           
			//plot0a.setRebinFactor(2);                                                                                                                                                                                                          
			plot0a.setXaxisTitle("S_{d_{0}}");
			if(doRebin) plot0a.setRebinFactor(i_rebin);
			plot0a.makeDataStackedMCPlot(mc_hists,data_hist);
			plot0a.addLabel();
			plot0a.Save(outfile,chan[i]+"_"+pt_all_regions[j]+"_maxSd0_pretag");
			
			mc_hists.clear();
			
			
			for(auto name : fl_pairs){
				mc_hists.push_back(chan[i]+"_"+name+pt_all_regions[j]+"_maxSd0_PREFITPOSTTAG");
			}
			
			data_hist=""; //chan[i]+"_other_"+pt_all_regions[j]+"_maxSd0";
			
			HistPlotter plot0b(input_folder,&info);
			plot0b.setLumi(lumi); //in pb^-1                                                                                                                                                                                                     
			
			plot0b.setXaxisLimits(-40,99);
			plot0b.setYaxisLimits(-1,-1); //auto range                                                                                                                                                                                          
			
			// plot0b.setRebinFactor(2);                                                                                                                                                                                                          
			plot0b.setXaxisTitle("S_{d_{0}}");
			if(doRebin) plot0b.setRebinFactor(i_rebin);
			
			//plot0b.setSysConfig(1,0,0, fitfile, &pt_all_regions); //show fit uncertainties band
			//plot0b.setSysLists(sys_list_up,sys_list_down,sys_list_1side);
			
			plot0b.makeDataStackedMCPlot(mc_hists,data_hist);
			plot0b.addLabel();
			plot0b.Save(outfile,chan[i]+"_"+pt_all_regions[j]+"_maxSd0_posttag");
			
			mc_hists.clear();
	
			
		}
	}
}
