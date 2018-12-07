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
#include "HistogramService.h"

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

void HistogramService::FastFillTH1D(TString tkey, TString title, double x, int nbin, double xmin, double xmax, double weight)
{
   string key = tkey.Data();
   string hist = key.substr(key.find_last_of("/")+1);
   string dir = key.substr(0,key.find_last_of("/")+1);
   key = fCurrentDir + "/" + key;
   if (fMegaList.find(key)==fMegaList.end()){  // book histogram
      fMegaList[key] = new TH1D(hist.c_str(), title, nbin, xmin, xmax);
      fDirMap[key] = fCurrentDir + "/" + dir;

      ((TH1D*)fMegaList[key])->Sumw2();
   }
   ((TH1D*) fMegaList[key])->Fill(x,weight);
}

void HistogramService::FastFillTH2D(TString tkey, TString title, double x, double y, int nbinx, double xmin, double xmax, int nbiny, double ymin, double ymax, double weight )
{
   string key = tkey.Data();
   string hist = key.substr(key.find_last_of("/")+1);
   string dir = key.substr(0,key.find_last_of("/")+1);
   key = fCurrentDir + "/" + key;
   if (fMegaList[key]==0){  // book histogram
      fMegaList[key] = new TH2D(hist.c_str(), title, 
                                nbinx, xmin, xmax, 
                                nbiny, ymin, ymax);
      fDirMap[key] = fCurrentDir + "/" + dir;
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
   map<string, TDirectory* >::iterator dirIt;
   map<string, TDirectory* >::iterator subdirIt;
   for (it = fMegaList.begin();it != fMegaList.end(); it++){ 
      string key = it->first;
      string dirName = fDirMap[key];
      while (dirName[0] == '/') dirName.erase(0,1); //Strip leading "/"
      dirIt = mapOfCreatedDirs.find(dirName);

      if (dirName=="") f->cd();
      else if ( dirIt == mapOfCreatedDirs.end()) {
         //  create directories recursively
         size_t pos(0), prev_pos(0);
         while ( (pos=dirName.find("/",prev_pos)) != std::string::npos) {
           string subdirName = dirName.substr(prev_pos, pos-prev_pos);
           string subdirPath = dirName.substr(0, pos);
           subdirIt = mapOfCreatedDirs.find(subdirPath);
           if (subdirName=="") { prev_pos++; continue; } // Skip past "//" sequences
           else if ( subdirIt == mapOfCreatedDirs.end()) {
             cout << "creating dir "<<subdirPath<<" for key "<<key<<endl;
             f->mkdir(subdirPath.data(), subdirName.data());
             f->cd(subdirPath.data());
             mapOfCreatedDirs[subdirPath] = gDirectory->CurrentDirectory();
           } else subdirIt->second->cd();
           prev_pos = pos+1; // Move to the next subdir
         }
         string subdirName = dirName.substr(prev_pos);
         if (subdirName!="") {
           //cout << "creating dir "<<subdirName<<" for key "<<key<<endl;
           f->mkdir(dirName.data(), subdirName.data());
           f->cd(dirName.data());
           mapOfCreatedDirs[dirName] = gDirectory->CurrentDirectory();
         }
      } else dirIt->second->cd(); // cd to dir 
      //cout << "writing histo "<<it->first<< " to dir "<<dirName<<std::endl;
      it->second->Write();
      f->cd();
   }
   cout << "wrote histo file=" << fileName << endl;

   f->Close();
   mapOfCreatedDirs.clear();
}

bool HistogramService::BookRootObject(TObject* hist){
   string key = hist->GetName();
   if (fMegaList.find(key)==fMegaList.end()) {
      //hist->SetDirectory(fDir);  ToDo is this ok???
      fMegaList[key]=hist;
      return true;
   } 
   return false;
}

TObject* HistogramService::GetHisto(TString tkey){
  string key = tkey.Data();
  key = fCurrentDir + "/" + key;
  if (fMegaList.find(key)!=fMegaList.end()) {
    //hist->SetDirectory(fDir);  ToDo is this ok???
    return fMegaList[key];
  } else std::cout<<"Could not get Histo: "<<key<<std::endl;

  return 0;
}
