// created November 2006, Eckhard von Toerne

#ifndef EVT_HISTOGRAMSERVICE_H
#define EVT_HISTOGRAMSERVICE_H

// #include "TObject.h"
// #include "TH1D.h"
// #include "TH2D.h"
// #include "TDirectory.h"
// #include "TFile.h"
#include "Riostream.h"
#include <map>
#include <string>
using std::map;
using std::string;

class TObject;
class TH1D;
class TH2D;
class TDirectory;
class TFile;


class HistogramService
{
   friend class GbbTupleAna;
private:
   string fCurrentDir;
   map< string, string>   fDirMap;
   map< string, TObject*> fMegaList;
   HistogramService();  //! private constructor to be invoked by freind classes
   ~HistogramService(); // destructor

public:

   void FastFillTH1D(TString key, double x, int nbin=100, double xmin=0., double xmax=0., double weight=1.) {
     FastFillTH1D(key, key, x, nbin, xmin, xmax, weight);
   }
   void FastFillTH1D(TString key, TString title, double x, int nbin=100, double xmin=0., double xmax=0., double weight=1.);
   
   void FastFillTH2D(TString key, double x, double y, int nbinx, double xmin, double xmax, int nbiny, double ymin, double ymax, double weight=1.) {
     FastFillTH2D(key, key, x, y, nbinx, xmin, xmax, nbiny, ymin, ymax, weight);
   }
   void FastFillTH2D(TString key, TString title, double x, double y, int nbinx, double xmin, double xmax, int nbiny, double ymin, double ymax, double weight=1.);
   
   void WriteRootFile(TString fileName);

   void SetCurrentDir(TString dirname){ fCurrentDir=dirname; }
   string GetCurrentDir(){ return fCurrentDir; }
   bool BookRootObject(TObject* hist);
   TObject* GetHisto(TString key);

};

#endif
