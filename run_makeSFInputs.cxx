#include "SampleInfo.h"
#include "Plotter.h"
#include "TFile.h"
#include "TString.h"
#include "TDatime.h"


int main(int argc,char *argv[]){


  TDatime today;
  int date=today.GetDate();

  TString name;
  name.Form("./testoutput/%d_CalibrationInputs_Nominal.root",date);

  float lumi=2959.7;

  TFile *outfile=new TFile(name.Data(),"RECREATE");

  SampleInfo info;
  info.ReadConfig("data/SampleInfo_r20p7.txt");

  std::vector<TString> dijet_cat={"BB","BL","CC","CL","LL"}; //merged LB, CB, BC, LC
  std::vector<TString> ptnonmuojet={"l20","g20l50","g50l80","g80"};
  std::vector<TString> ptmuojet={"l100","g100l200","g200"};


  std::vector<float> fatptbins={250,300,350,400,450,500,550,600,750,1000};
  std::vector<TString> ptfatjet;
  TString help;

  //help=Form("l%.0f",fatptbins[0]);
  //ptfatjet.push_back(help);

  for(int i=0; i<fatptbins.size()-1; i++){

    help=Form("g%.0fl%.0f",fatptbins[i],fatptbins[i+1]);
    ptfatjet.push_back(help);

  }

  help=Form("g%.0f",fatptbins[fatptbins.size()-1]);
  ptfatjet.push_back(help);


  std::vector<TString> names, data_names;

  std::vector<TString> fatnames, fatdata_names;

  for(int i=0; i<dijet_cat.size(); i++){

    for(int j=0; j<ptmuojet.size(); j++){

      for(int k=0; k<ptnonmuojet.size(); k++){
	
	if(k==0 && j==0){
	  names.push_back("muojet_"+dijet_cat[i]+"_maxSd0");
	  names.push_back("nonmuojet_"+dijet_cat[i]+"_maxSd0");
	  //fatnames.push_back("fatjet_"+dijet_cat[i]+"_maxSd0");

	  if(i==0){
	    data_names.push_back("muojet_other_maxSd0");
	    data_names.push_back("nonmuojet_other_maxSd0"); 

	  }

        }

	names.push_back("muojet_"+dijet_cat[i]+"_mjpt_"+ptmuojet[j]+"_nmjpt_"+ptnonmuojet[k]+"_maxSd0");
	names.push_back("nonmuojet_"+dijet_cat[i]+"_mjpt_"+ptmuojet[j]+"_nmjpt_"+ptnonmuojet[k]+"_maxSd0");
	fatnames.push_back("fatjet_"+dijet_cat[i]+"_mjpt_"+ptmuojet[j]+"_nmjpt_"+ptnonmuojet[k]+"_pt");
	fatnames.push_back("fatjet_"+dijet_cat[i]+"_mjpt_"+ptmuojet[j]+"_nmjpt_"+ptnonmuojet[k]+"_pt_PREFITPOSTTAG");

	if(i==0){
	  data_names.push_back("muojet_other_mjpt_"+ptmuojet[j]+"_nmjpt_"+ptnonmuojet[k]+"_maxSd0");
          data_names.push_back("nonmuojet_other_mjpt_"+ptmuojet[j]+"_nmjpt_"+ptnonmuojet[k]+"_maxSd0");
	  fatdata_names.push_back("fatjet_other_mjpt_"+ptmuojet[j]+"_nmjpt_"+ptnonmuojet[k]+"_pt");
	  fatdata_names.push_back("fatjet_other_mjpt_"+ptmuojet[j]+"_nmjpt_"+ptnonmuojet[k]+"_pt_PREFITPOSTTAG");

	}
      }

    }

    for(int l=0; l<ptfatjet.size(); l++){
      
      std::cout<<"pushed back"<<TString("muojet_"+dijet_cat[i]+"_fjpt_"+ptfatjet[l]+"_maxSd0")<<std::endl;
      

      names.push_back("muojet_"+dijet_cat[i]+"_fjpt_"+ptfatjet[l]+"_maxSd0");
      names.push_back("nonmuojet_"+dijet_cat[i]+"_fjpt_"+ptfatjet[l]+"_maxSd0");
      fatnames.push_back("fatjet_"+dijet_cat[i]+"_fjpt_"+ptfatjet[l]+"_pt");
      fatnames.push_back("fatjet_"+dijet_cat[i]+"_fjpt_"+ptfatjet[l]+"_pt_PREFITPOSTTAG");
      
      if(i==0){
	
	std::cout<<"pushed back"<<TString("muojet_other_fjpt_"+ptfatjet[l]+"_maxSd0")<<std::endl;

	data_names.push_back("muojet_other_fjpt_"+ptfatjet[l]+"_maxSd0");
	data_names.push_back("nonmuojet_other_fjpt_"+ptfatjet[l]+"_maxSd0");
	fatdata_names.push_back("fatjet_other_fjpt_"+ptfatjet[l]+"_pt");
	fatdata_names.push_back("fatjet_other_fjpt_"+ptfatjet[l]+"_pt_PREFITPOSTTAG");
	
      }
      

      

    }


  }


  
  Plotter plotter("/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/Output_Calib/",&info);
  plotter.setLumi(lumi); //in pb^-1                                                                                                                                                                                                                                       
  plotter.makeFitTemplates(names,data_names,outfile);

  plotter.makeFitTemplates(fatnames,fatdata_names,outfile);



  /* std::cout<<"Plot down systematics"<<std::endl;

  Plotter plotter_sysdown("/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output/Output_gbbTupleAna_JET_Hbb_Run1_pT__1down/",&info,"_sys_down");
  plotter_sysdown.setLumi(lumi); //in pb^-1                                                                                                                                                                                                                                   
  plotter_sysdown.makeFitTemplates(names,data_names,outfile,"JET_Hbb_Run1_pTLow");

  std::cout<<"Plot up systematics"<<std::endl;

  Plotter plotter_sysup("/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output/Output_gbbTupleAna_JET_Hbb_Run1_pT__1up/",&info,"_sys_up");
  plotter_sysup.setLumi(lumi); //in pb^-1                                                                                                                                                                                                                                   
  plotter_sysup.makeFitTemplates(names,data_names,outfile,"JET_Hbb_Run1_pTHigh");

  */

  outfile->Close();
  delete outfile;
  
  return 0;

}
