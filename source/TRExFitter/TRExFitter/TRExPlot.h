#ifndef TRExPLOT_H
#define TRExPLOT_H

/// Framework includes
#include "TRExFitter/TRExFit.h"

/// c++ includes
#include <string>
#include <vector>

/// Forwards class declaration
class TCanvas;
class TGraphAsymmErrors;
class TH1;
class TH1D;
class THStack;
class TLegend;
class TPad;

const int MAXbins = 1000;

class TRExPlot {
  public:
    TRExPlot(std::string name="c",int canvasWidth=600,int canvasHeight=700,bool hideRatioPad=false);
    ~TRExPlot();

    void SetChannel(const std::string& name);
    void AddLabel(const std::string& name);
    void SetLumi(const std::string& name);
    void SetLumiScale(double scale);
    void SetCME(const std::string& name);
    void SetXaxis(const std::string& name,bool isNjet=false);
    void SetYaxis(const std::string& name);
    void SetYmaxScale(double scale);
    void SetBinLabel(int bin, const std::string& name);
    void SetBinWidth(double width);

    void SetData(TH1* h,std::string name="Data");
    void AddSignal(TH1* h,std::string name="Signal");
    void AddNormSignal(TH1* h,std::string name="Signal");
    void AddOverSignal(TH1* h,std::string name="Signal");
    void AddBackground(TH1* h,std::string name="MC");
    void SetTotBkgAsym(TGraphAsymmErrors* g);
    void SetTotBkg(TH1* h);

    void SetChi2KS(double chi2prob,double ksprob=-1.,double chi2val=-1.,int ndf=-1);
    void BlindData();

    void SetXaxisRange(const std::vector<double>& vec) {fXaxisRange = vec;}

    void Draw(std::string options="");
    void SaveAs(const std::string& name) const;
    void WriteToFile(const std::string& name) const;

    TCanvas* GetCanvas() const;

    void SetBinBlinding(bool on,double threshold=0.02,TRExFit::BlindingType=TRExFit::SOVERB);
    void SetBinBlinding(bool on,TH1D*h_blind,TRExFit::BlindingType=TRExFit::SOVERB);

    std::string fName;
    TH1* h_data;
    TGraphAsymmErrors* g_data;
    TH1* h_mc;
    std::vector<TH1*> h_bkg;
    std::vector<TH1*> h_signal;
    std::vector<TH1*> h_normsig;
    std::vector<TH1*> h_oversig;
    THStack* h_stack;
    TH1* h_tot;
    TGraphAsymmErrors* g_tot;
    TH1D* h_blinding;
    TH1* h_tot_bkg_prefit;
    TH1* h_dummy;

    TCanvas* c;
    TLegend* leg;
    TLegend* leg1;
    TPad* pad0;
    TPad* pad1;

    std::string xtitle;
    std::string ytitle;
    std::string fDataName;
    std::vector< std::string > fBkgNames;
    std::vector< std::string > fSigNames;
    std::vector< std::string > fNormSigNames;
    std::vector< std::string > fOverSigNames;
    std::vector< std::string > fLabels;
    std::string fLumi;
    std::string fCME;
    std::string fATLASlabel;
    double yMaxScale;
    int NDF;
    double Chi2val;
    double Chi2prob;
    double KSprob;

    double fYmax;
    double fYmin;
    double fRatioYmax;
    double fRatioYmin;
    double fBinWidth;
    bool fIsNjet;
    bool fShowYields;
    std::string fBinLabel[MAXbins];
    double fLumiScale;
    double fBlindingThreshold;
    TRExFit::BlindingType fBlindingType;
    int fLegendNColumns;
    std::vector<double> fXaxisRange;
    std::string fRatioYtitle;

    std::string fRatioType;
    
    double fLabelX;
    double fLabelY;
    double fLegendX1;
    double fLegendX2;
    double fLegendY;

public:
    const TH1* GetTotal() const { return h_tot; };
    TH1* GetTotBkg() const;

};

// function to get asymmetric error bars for hists
double GC_up(double data);
double GC_down(double data);
TGraphAsymmErrors* poissonize(TH1 *h);
TGraphAsymmErrors* histToGraph(TH1* h);
void SetHistBinWidth(TH1* h,double width);
void SetGraphBinWidth(TGraphAsymmErrors* g,double width);

#endif
