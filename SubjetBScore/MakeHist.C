#ifndef MAKEHIST_SOURCE
#define MAKEHIST_SOURCE

#include "State.h"

void MakeHist::BookHist(const char *fName)
{
 result = new TFile(fName,"RECREATE");

 //Initial plots
 SubjetBScore_Higgs = new TH1D("SubjetBScore_Higgs","SubjetBScore_Higgs",50,0,1);
 SubjetBScore_Top = new TH1D("SubjetBScore_Top","SubjetBScore_Top",50,0,1);
 SubjetBScore_QCD = new TH1D("SubjetBScore_QCD","SubjetBScore_QCD",50,0,1);
 SubjetBScore_Combine = new TH1D("SubjetBScore_Combine","SubjetBScore_Combine",40,-10,10);
}

void MakeHist::SaveHist(void)
{
 result->Write();
 result->Save();
}

#endif
