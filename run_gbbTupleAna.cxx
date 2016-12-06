#include "GbbTupleAna.h"



int main(int argc,char *argv[]){

  TString infilename;

  TString basedir("/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Inputs/FTagNTuples_Calib/v_00-00-21TESTMUONFILTER/");

  TString sampledir("user.ruth.mc15_13TeV.427034.DAOD_FTAG1.FTNtupCalib.gbb_v00-00-21bTESTMUONFILTER.e3968_s2608_s2183_r7725_r7676_p2839_tuple.root/");
  
  TString tuple("user.ruth.9913914._000001.tuple.root");

  TString treename("FlavourTagging_Nominal");

  TString outfilename("./testoutput/test.root");
  
  TString configname("./configs/run_GbbTupleAna_MuonFilterCheck.cfg");
  
  if(argc==1){
	  infilename=basedir+sampledir+tuple;
          std:cout<<"No arguments, run on test file!"<<std::endl;

  }else{
	  infilename=TString(argv[1]);
	  outfilename=TString(argv[2]);
	  treename=TString(argv[3]);
	  configname=TString(argv[4]);
  }


  GbbTupleAna ana(infilename,treename,outfilename,configname);

  ana.Loop();

  ana.Finalize();

}
