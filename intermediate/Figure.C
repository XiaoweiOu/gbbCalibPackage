#include"/home/ouxiaowei/include/Figure.h"
#include<iomanip>

int main()
{
 TFile *file = new TFile("ConsolidatedHist.root");

 TString str[2]={"BL","BB"};

 for(int i=0;i<2;i++)
 {
 TH1D *mjmeanSd0_B0 = (TH1D *)file->Get("h_mjpt_g200l300_nmjpt_g100l200_mjmeanSd0_B0_"+str[i]+"_Nom");
 TH1D *mjmeanSd0_Bpm = (TH1D *)file->Get("h_mjpt_g200l300_nmjpt_g100l200_mjmeanSd0_Bpm_"+str[i]+"_Nom");
 TH1D *mjmeanSd0_B_s = (TH1D *)file->Get("h_mjpt_g200l300_nmjpt_g100l200_mjmeanSd0_B_s_"+str[i]+"_Nom");
 TH1D *mjmeanSd0_Lambda_B = (TH1D *)file->Get("h_mjpt_g200l300_nmjpt_g100l200_mjmeanSd0_Lambda_B_"+str[i]+"_Nom");
 TH1D *mjmeanSd0_Cascade_Bm = (TH1D *)file->Get("h_mjpt_g200l300_nmjpt_g100l200_mjmeanSd0_Cascade_Bm_"+str[i]+"_Nom");
 TH1D *mjmeanSd0_Cascade_B0 = (TH1D *)file->Get("h_mjpt_g200l300_nmjpt_g100l200_mjmeanSd0_Cascade_B0_"+str[i]+"_Nom");

 //print the fraction
 double total=mjmeanSd0_B0->Integral()+mjmeanSd0_Bpm->Integral()
             +mjmeanSd0_B_s->Integral()+mjmeanSd0_Lambda_B->Integral()
             +mjmeanSd0_Cascade_Bm->Integral()+mjmeanSd0_Cascade_B0->Integral();
 cout<<str[i]<<endl;
 cout<<"B0: "<<fixed<<setprecision(3)<<mjmeanSd0_B0->Integral()/total<<endl
     <<"Bpm: "<<mjmeanSd0_Bpm->Integral()/total<<endl
     <<"B_s: "<<mjmeanSd0_B_s->Integral()/total<<endl
     <<"Lambda_B: "<<mjmeanSd0_Lambda_B->Integral()/total<<endl
     <<"Cascade_Bm: "<<mjmeanSd0_Cascade_Bm->Integral()/total<<endl
     <<"Cascade_B0: "<<mjmeanSd0_Cascade_B0->Integral()/total<<endl;

 /*Figure *result = new Figure("./figure/h_mjpt_g200l300_nmjpt_g100l200_mjmeanSd0_"+str[i]+".eps","mjmeanSd0","Number of Events");
 result->SetMode("SIMPLE");
 result->SetRatiorange(0.8,1.2);
 result->SetInputFigure(1,mjmeanSd0_B0,"B0");
 result->SetInputFigure(2,mjmeanSd0_Bpm,"Bpm");
 result->SetInputFigure(3,mjmeanSd0_B_s,"B_s");
 result->SetInputFigure(4,mjmeanSd0_Lambda_B,"Lambda_B");
 result->SetInputFigure(5,mjmeanSd0_Cascade_Bm,"Cascade_Bm");
 result->SetInputFigure(6,mjmeanSd0_Cascade_B0,"Cascade_B0");

 result->SetHeaderName("ATLAS Internal, 139.0 fb^{-1}, "+str[i]);
 result->DrawFigure();
 delete result;*/
 }
}
