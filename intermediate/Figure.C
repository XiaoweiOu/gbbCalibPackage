#include"/home/ouxiaowei/include/Figure.h"

int main()
{
 TString type_BScore[4]={"Higgs","Top","QCD","f0p25"};
 TString mjpt_PtRange[3]={"l200","g200l300","g300"};
 TString nmjpt_PtRange[3]={"l100","g100l200","g200"};

 for(int i=0;i<4;i++)
 {
  Figure *result = new Figure("./figure/SubjetBScore_prefit_"+type_BScore[i]+".eps", "SubjetBScore_"+type_BScore[i], "Number of Events");
  result->SetMode("HSTACK");

  //combine all pt region
 /* TH1D *p_Data=new TH1D("Data","Data",100,0,1);
  TH1D *p_BB=new TH1D("BB","BB",100,0,1);
  TH1D *p_BL=new TH1D("BL","BL",100,0,1);
  TH1D *p_CC=new TH1D("CC","CC",100,0,1);
  TH1D *p_CL=new TH1D("CL","CL",100,0,1);
  TH1D *p_LL=new TH1D("LL","LL",100,0,1);*/
  TH1D *p_Data,*p_BB,*p_BL,*p_CC,*p_CL,*p_LL;

  TFile *file=new TFile("ConsolidatedHist.root");

  for(int i_mjpt=0;i_mjpt<3;i_mjpt++)
   for(int i_nmjpt=0;i_nmjpt<3;i_nmjpt++)
   {
    TH1D *help_Data = (TH1D *)file->Get("h_mjpt_"+mjpt_PtRange[i_mjpt]+"_nmjpt_"+nmjpt_PtRange[i_nmjpt]+"_SubjetBScore_"+type_BScore[i]+"_1MUON_Data_Nom");
    if(i_mjpt==0 && i_nmjpt==0)p_Data = (TH1D *)help_Data->Clone();
    else p_Data->Add(help_Data);
 
    cout<<"h_mjpt_"+mjpt_PtRange[i_mjpt]+"_nmjpt_"+nmjpt_PtRange[i_nmjpt]+"_SubjetBScore_"+type_BScore[i]+"_1MUON_Data_Nom"<<endl;
 
    TH1D *help_BB = (TH1D *)file->Get("h_mjpt_"+mjpt_PtRange[i_mjpt]+"_nmjpt_"+nmjpt_PtRange[i_nmjpt]+"_SubjetBScore_"+type_BScore[i]+"_1MUON_BB_Nom");
    for(int i_bin=1;i_bin<=help_BB->GetNbinsX();i_bin++)help_BB->SetBinError(i_bin,0);
    if(i_mjpt==0 && i_nmjpt==0)p_BB = (TH1D *)help_BB->Clone();
    else p_BB->Add(help_BB);

    TH1D *help_BL = (TH1D *)file->Get("h_mjpt_"+mjpt_PtRange[i_mjpt]+"_nmjpt_"+nmjpt_PtRange[i_nmjpt]+"_SubjetBScore_"+type_BScore[i]+"_1MUON_BL_Nom");
    for(int i_bin=1;i_bin<=help_BL->GetNbinsX();i_bin++)help_BL->SetBinError(i_bin,0);
    if(i_mjpt==0 && i_nmjpt==0)p_BL = (TH1D *)help_BL->Clone();
    else p_BL->Add(help_BL);

    TH1D *help_CC = (TH1D *)file->Get("h_mjpt_"+mjpt_PtRange[i_mjpt]+"_nmjpt_"+nmjpt_PtRange[i_nmjpt]+"_SubjetBScore_"+type_BScore[i]+"_1MUON_CC_Nom");
    for(int i_bin=1;i_bin<=help_CC->GetNbinsX();i_bin++)help_CC->SetBinError(i_bin,0);
    if(i_mjpt==0 && i_nmjpt==0)p_CC = (TH1D *)help_CC->Clone();
    else p_CC->Add(help_CC);

    TH1D *help_CL = (TH1D *)file->Get("h_mjpt_"+mjpt_PtRange[i_mjpt]+"_nmjpt_"+nmjpt_PtRange[i_nmjpt]+"_SubjetBScore_"+type_BScore[i]+"_1MUON_CL_Nom");
    for(int i_bin=1;i_bin<=help_CL->GetNbinsX();i_bin++)help_CL->SetBinError(i_bin,0);
    if(i_mjpt==0 && i_nmjpt==0)p_CL = (TH1D *)help_CL->Clone();
    else p_CL->Add(help_CL);

    TH1D *help_LL = (TH1D *)file->Get("h_mjpt_"+mjpt_PtRange[i_mjpt]+"_nmjpt_"+nmjpt_PtRange[i_nmjpt]+"_SubjetBScore_"+type_BScore[i]+"_1MUON_LL_Nom");
    for(int i_bin=1;i_bin<=help_LL->GetNbinsX();i_bin++)help_LL->SetBinError(i_bin,0);
    if(i_mjpt==0 && i_nmjpt==0)p_LL = (TH1D *)help_LL->Clone();
    else p_LL->Add(help_LL);
   }

  result->SetInputFigure(1,p_Data, "Data");
  result->SetInputFigure(2,p_BB, "BB");
  result->SetInputFigure(3,p_BL, "BL");
  result->SetInputFigure(4,p_CC, "CC");
  result->SetInputFigure(5,p_CL, "CL");
  result->SetInputFigure(6,p_LL, "LL");

  result->SetRatiorange(0.4, 1.6);
  result->SetHeaderName("ATLAS Internal, 139.0 fb^{-1}");
  result->DrawFigure();
  delete result;
 }
}
