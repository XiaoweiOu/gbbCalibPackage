// 
// created October 2006, Eckhard von Toerne

#include <string>
#include "TVector.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TPDF.h"
#include "TCanvas.h"
#include "TObject.h"
#include "TList.h"
#include "TRandom.h"
#include "TF1.h"
#include "Riostream.h"
#include "TObjString.h"
#include "TStyle.h"
#include "TBenchmark.h"
#include "TDirectory.h"
#include "GbbCalibration/HistogramService.h"

using namespace std;

HistogramService::HistogramService() : fCurrentDir("")
{ // constructor
   cout << "creating HistogramService" << endl;
}

HistogramService::~HistogramService()
{ 
   map<string, TObject* >::iterator it;
   for (it = fMegaList.begin();it != fMegaList.end(); it++){ 
      delete it->second;
   }
   fMegaList.clear(); 
}

void HistogramService::FastFillTH1D(TString tkey, double x, int nbin, double xmin, double xmax, double weight)
{
   string key = tkey.Data();
   key = fCurrentDir + "/" + key;
   if (fMegaList.find(key)==fMegaList.end()){  // book histogram
      fMegaList[key] = new TH1D(tkey, tkey, nbin, xmin, xmax);
      fDirMap[key]   = fCurrentDir;
      ((TH1D*)fMegaList[key])->Sumw2();
   }
   ((TH1D*) fMegaList[key])->Fill(x,weight);
}

void HistogramService::FastFillTH2D(TString tkey, double x, double y, int nbinx, double xmin, double xmax, int nbiny, double ymin, double ymax, double weight )
{
   string key = tkey.Data();
   key = fCurrentDir + "/" + key;
   if (fMegaList[key]==0){  // book histogram
      fMegaList[key] = new TH2D(tkey, tkey, 
                                nbinx, xmin, xmax, 
                                nbiny, ymin, ymax);
      fDirMap[key]   = fCurrentDir;
      ((TH2D*)fMegaList[key])->Sumw2();
   }
   ((TH2D*) fMegaList[key])->Fill(x,y,weight);
}

void HistogramService::WriteRootFile(TString fileName)
{  
   std::map< string , TDirectory* > mapOfCreatedDirs;    
   TFile *f = new TFile(fileName,"RECREATE");  
   f->cd();
   // using a map::iterator
   map<string, TObject* >::iterator it;
   map<string, TDirectory* >::iterator dirit;
   for (it = fMegaList.begin();it != fMegaList.end(); it++){ 
      string key = it->first;
      string dirName = fDirMap[key];
      dirit = mapOfCreatedDirs.find(dirName);

      if (dirName=="") f->cd();
      else if ( dirit == mapOfCreatedDirs.end()){
         //  create dir
         TDirectory *lstDir = f->mkdir(dirName.data(), dirName.data());
         lstDir->cd();
         mapOfCreatedDirs[dirName] = lstDir;
      }
      else dirit->second->cd(); // cd to dir 
      cout << "writing histo "<<it->first<< " to dir "<<dirName<<endl;
      it->second->Write();
   }
   cout << "wrote histo file=" << fileName << endl;

   f->Close();
   mapOfCreatedDirs.clear();
}

bool HistogramService::BookRootObject(TObject* hist){
   string key = hist->GetName();
   if (fMegaList.find(key)==fMegaList.end()){
      //hist->SetDirectory(fDir);  ToDo is this ok???
      fMegaList[key]=hist;
      return true;
   } 
   return false;
}

TObject* HistogramService::GetHisto(TString tkey){
	string key = tkey.Data();
	key = fCurrentDir + "/" + key;
	if (fMegaList.find(key)!=fMegaList.end()){
		//hist->SetDirectory(fDir);  ToDo is this ok???

		return fMegaList[key];

	}else std::cout<<"Could not get Histo: "<<key<<std::endl;

	return 0;
}
