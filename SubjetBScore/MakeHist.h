#ifndef MAKEHIST_HEADER
#define MAKEHIST_HEADER

#include "State.h"

class MakeHist
{
public:
 //file for histogram saving
 TFile *result;

 //histograms
 TH1D *SubjetBScore_Higgs;
 TH1D *SubjetBScore_Top;
 TH1D *SubjetBScore_QCD;
 TH1D *SubjetBScore_Combine;

 //function
 void BookHist(const char *fName);
 void SaveHist();
};

#endif
