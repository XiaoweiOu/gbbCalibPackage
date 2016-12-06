#include "TFile.h"
#include "TTree.h"
#include "TSystemDirectory.h"
#include "TSystemFile.h"
#include "TList.h"


void GetSampleN(const char* in_dir){


  long total_sample_entries=0;

  TSystemDirectory dir(in_dir, in_dir);
  TList *files = dir.GetListOfFiles();
  if (files) {

    TSystemFile *file;
    TString fname;
    TIter next(files);
    while ((file=(TSystemFile*)next())) {
      fname = file->GetName();

      if (!(file->IsDirectory())) {
	
	TString filename=TString(in_dir)+"/"+fname;

	TFile *infile=TFile::Open(filename.Data(),"READ");

	TTree* tree=(TTree*) infile->Get("FlavourTagging_Nominal");

	  long nentries=tree->GetEntriesFast();

	  total_sample_entries+=nentries;

	  //	  std::cout<<"Tree has "<<nentries<<" entries "<<std::endl;

	  infile->Close();
	

      }
    }
  }


  std::cout<<"Sample has "<<total_sample_entries<<" entries "<<std::endl;

}
