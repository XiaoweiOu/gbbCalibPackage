#ifndef GbbAna_cxx
#define GbbAna_cxx

#include "State.h"

char GetFlavour(int truthType) {
  switch (std::abs(truthType)) {
    case 5:
      return 'B';
      break;
    case 4:
      return 'C';
      break;
    case 0:
      return 'L';
      break;
    case 15:
      return 'T';
      break;
    default:
      std::cout<<"GlobalConfig - Unrecognized truth type: "<<truthType<<std::endl;
      return 'O';
      break;
  }
}

TString GetFlavourPair(int muJetTruth, int nonMuJetTruth) {

  int m_doMergeFlavours=1;//BB,BL,CC,CL,LL

  TString output = "";
  output += GetFlavour(muJetTruth);
  output += GetFlavour(nonMuJetTruth);
  if (output.Contains("O") || output.Contains("T")) return "Other";
  if (!m_doMergeFlavours) return output;
  else {
    //FIXME: Hard-coding this is sub-optimal if categories ever change
    if (output == "BC") return "BL";
    else if (output == "CB") return "CL";
    else if (output == "LB") return "LL";
    else if (output == "LC") return "LL";
    else return output;
  }
}

void GbbAna::Loop()
{
//   In a ROOT session, you can do:
//      root> .L GbbAna.C
//      root> GbbAna t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
 

  if (isRead == false) return;
  if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      // if (Cut(ientry) < 0) continue;//event selection

      size_t size=fat_SubjetBScore_Higgs->size();
      for(size_t ii=0;ii<size;ii++)
      {
       if(fat_pt->at(ii)<400)continue;//FIXME:check whether pt's unit is GeV???

       myHist->SubjetBScore_Higgs->Fill(fat_SubjetBScore_Higgs->at(ii));
       myHist->SubjetBScore_Top->Fill(fat_SubjetBScore_Top->at(ii));
       myHist->SubjetBScore_QCD->Fill(fat_SubjetBScore_QCD->at(ii));
    
       double f_top=0.25;
       double combine=log(fat_SubjetBScore_Higgs->at(ii)/(f_top*fat_SubjetBScore_Top->at(ii)+(1-f_top)*fat_SubjetBScore_QCD->at(ii)));
       myHist->SubjetBScore_Combine->Fill(combine);

       //FIXME:you can get more info about jet flavour in /home/ouxiaowei/gbbCalibPackage/source/gbbCalibration/helpers/GlobalConfig.cxx
       //cout<<ii<<": "<<trkjet_truth->at(ii)<<endl;

      }
   }
}
#endif
