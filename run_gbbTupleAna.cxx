#include "GbbTupleAna.h"



int main(int argc,char *argv[]){

  TString infilename;

  //TString basedir("/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Inputs/FTagNTuples_Calib/v_00-00-21TESTMUONFILTER/");
  //TString basedir("/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Inputs/FTagNTuples_Calib/v00-01-02/");

  TString basedir("/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Inputs/FTagNTuples_Calib/v00-01-06/");
  //TString basedir("/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Code/FTAGxAOD/submitDir_testfatjettrigger/data-tuple/");



  //TString sampledir("user.ruth.mc15_13TeV.427034.DAOD_FTAG1.FTNtupCalib.gbb_v00-00-21bTESTMUONFILTER.e3968_s2608_s2183_r7725_r7676_p2839_tuple.root/");
  //TString sampledir("user.ruth.mc15_13TeV.427004.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root/");
  //TString sampledir("user.ruth.mc15_13TeV.427004.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e5660_s2726_r7773_r7676_p2949_tuple.root/");  
  //TString sampledir("user.ruth.mc15_13TeV.427004.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04c.e5660_s2726_r7773_r7676_p2949_tuple.root/");
  //data
  //TString sampledir("user.ruth.data16_13TeV.00311481.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.f758_m1710_p2950_tuple.root/");
  
  //TString sampledir("user.ruth:user.ruth.data15_13TeV.00276262.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.r7562_p2521_p2950_tuple.root/");
  //TString sampledir="";
  TString sampledir=("user.ruth.mc15_13TeV.361025.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-06b.e3668_s2576_s2132_r7725_r7676_p3323_tuple.root/");


  //TString tuple("user.ruth.9913914._000001.tuple.root");
  //TString tuple("user.ruth.11445391._000120.tuple.root");
  //TString tuple("user.ruth.12228436._000011.tuple.root");
  //TString tuple("user.ruth.12329754._000221.tuple.root");


  //data
  //TString tuple("user.ruth.11443807._000001.tuple.root");
  
  //TString tuple("user.ruth.12234275._000001.tuple.root");
  //TString tuple("mc15_13TeV.361025.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ5W.merge.DAOD_FTAG1.e3668_s2576_s2132_r7725_r7676_p2949.root");
  TString tuple("user.ruth.12994888._000828.tuple.root");


  TString treename("FlavourTagging_Nominal");

  TString outfilename("./testoutput/test.root");
  
  //TString configname("./configs/run_GbbTupleAna_MuonFilterCheck.cfg");

  TString configname("./configs/run_GbbTupleAna_Calib.cfg");
  
  std::vector<TString> inlist;

  if(argc==1){
	  infilename=basedir+sampledir+tuple;
          std:cout<<"No arguments, run on test file!"<<std::endl;

  }else{
	  infilename=TString(argv[1]);
	  outfilename=TString(argv[2]);
	  treename=TString(argv[3]);
	  configname=TString(argv[4]);
	  if(infilename.EqualTo("list")){
	    for(int i=5; i<argc; i++){ 
	      inlist.push_back(TString(argv[i]));
	      std::cout<<"pushing back:"<<argv[i]<<std::endl;
	    }

	  }
  }


  GbbTupleAna ana(infilename,treename,outfilename,configname,inlist);

  ana.Loop();

  ana.Finalize();

}
