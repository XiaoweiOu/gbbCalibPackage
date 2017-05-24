#include "SampleInfo.h"
#include "Plotter.h"
#include "TFile.h"
#include "TString.h"
#include "TDatime.h"

int main(int argc,char *argv[]){

  //float lumi=2959.7;
  float lumi=3193.68;

  TDatime today;
  int date=today.GetDate();

  TString name;
  name.Form("./data/%d_reweightHist.root",date);


  TFile *outfile=new TFile(name.Data(),"RECREATE");

  SampleInfo info;
  info.ReadConfig("data/SampleInfo_r20p7.txt");

  Plotter plot("/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/Output_Calib/",&info);
  plot.setLumi(lumi); //in pb^-1
  plot.makeReweightHisto("reweight_trigjet_pt_eta_HLT_j85",outfile);
  plot.makeReweightHisto("reweight_trigjet_pt_eta_HLT_j110",outfile);
  plot.makeReweightHisto("reweight_trigjet_pt_eta_HLT_j150",outfile);
  plot.makeReweightHisto("reweight_trigjet_pt_eta_HLT_j260",outfile);
  plot.makeReweightHisto("reweight_trigjet_pt_eta_HLT_j360",outfile);
  plot.makeReweightHisto("reweight_trigjet_pt_eta_HLT_j100",outfile);
  plot.makeReweightHisto("reweight_trigjet_pt_eta_HLT_j175",outfile);
  plot.makeReweightHisto("reweight_trigjet_pt_eta_HLT_j200",outfile);
  plot.makeReweightHisto("reweight_trigjet_pt_eta_HLT_j300",outfile);
  plot.makeReweightHisto("reweight_trigjet_pt_eta_HLT_j320",outfile);

  outfile->Close();
  delete outfile;

}
