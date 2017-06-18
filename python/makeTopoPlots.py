import ROOT
import sys
import math
import string
from PlotFunctions import *
from TAxisFunctions import *
import os

ROOT.gROOT.SetBatch(True)
from ROOT import TCanvas,TPad,TH2,TColor

#ROOT.gStyle.SetOptStat(0)
SetupStyle()

outfilename = sys.argv[1]

Lumi = 36000.0 #in pb^-1

#Pythia
ListOfCrossSections = [2.6454e7,254620.,4553.5,257.54,16.215] #in pb
ListOfFilterEfficiencies=[1.6117e-5,3.5545e-5,7.1955e-5,7.6347e-5,3.1782e-5]

#Herwig
#ListOfCrossSections = [1.8831e7,173300.,2965.4,162.76,10.113] #in pb
#ListOfFilterEfficiencies = [1.9120e-05,4.2602e-5,9.0537e-5,0.00010259,4.7654e-5] #UPDATE for JZ3W 

basepath = '/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/'

pathJZ3W = basepath + 'Output_Topo/Topo_user.ruth.mc15_13TeV.427003.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root/merged_hist_Topo_user.ruth.mc15_13TeV.427003.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root' 
pathJZ4W = basepath + 'Output_Topo/Topo_user.ruth.mc15_13TeV.427004.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root/merged_hist_Topo_user.ruth.mc15_13TeV.427004.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root' 
pathJZ5W = basepath + 'Output_Topo/Topo_user.ruth.mc15_13TeV.427005.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root/merged_hist_Topo_user.ruth.mc15_13TeV.427005.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root' 
pathJZ6W = basepath + 'Output_Topo/Topo_user.ruth.mc15_13TeV.427106.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5839_s2726_r7773_r7676_p2949_tuple.root/merged_hist_Topo_user.ruth.mc15_13TeV.427106.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5839_s2726_r7773_r7676_p2949_tuple.root' 
pathJZ7W = basepath + 'Output_Topo/Topo_user.ruth.mc15_13TeV.427107.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5839_s2726_r7773_r7676_p2949_tuple.root/merged_hist_Topo_user.ruth.mc15_13TeV.427107.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5839_s2726_r7773_r7676_p2949_tuple.root' 

ListOfMCPaths = [ pathJZ3W, pathJZ4W, pathJZ5W, pathJZ6W, pathJZ7W ]
pathData = basepath + 'Output_Topo/user.ruth.data1516_13TeV.all.root/merged_hist_user.ruth.data1516_13TeV.all.root '

#make list of histograms
ListOfHists = ['h_trjeta', 'h_trjpt', 'h_trjmjDR' ]

#make list of axis labels
ListOfAxisLabels = ['Trigger Jet #eta', 'Trigger Jet p_{T} [GeV]', '#DeltaR(Trigger Jet, Muon Jet)' ]

#make Canvas
ListOfCanvasses = []

ListOfCanvasses.append(RatioCanvas('c_trjeta','c_trjeta',800,800, 0.3, 'pE1') )
ListOfCanvasses.append(RatioCanvas('c_trjpt','c_trjpt',800,800, 0.3, 'pE1') )
ListOfCanvasses.append(RatioCanvas('c_trjmjDR','c_trjmjDR',800,800, 0.3, 'pE1') )

#ListOfCanvasses.append(ROOT.TCanvas("c_trjeta",""))
#ListOfCanvasses.append(ROOT.TCanvas("c_trjpt",""))
#ListOfCanvasses.append(ROOT.TCanvas("c_trjmjDR",""))


colors = [ROOT.kBlue, ROOT.kBlack]
#SetColors(canvas,colors)

#loop over MC histograms
#index=0
hists = []
hists.append([])
hists.append([])
hists.append([])


for histname in ListOfHists :
    hist_index = ListOfHists.index(histname)

    for path in ListOfMCPaths :        
        index = ListOfMCPaths.index(path)
        file_curr=ROOT.TFile(path,"READ")
        print("Open file "+path)
    
        bookkeep_hist=file_curr.Get("Hist_BookKeeping") #Events in AOD is in Bin 3
        weight=ListOfCrossSections[index]*ListOfFilterEfficiencies[index]/bookkeep_hist.GetBinContent(3)*Lumi
        print("weight is: "+str(weight))

        if not file_curr.GetListOfKeys().Contains(histname) :
            print("Cannot find first hist "+histname)
        elif index is 0 :
            hist_0=file_curr.Get(histname)
            hist_0.SetDirectory(0)
            hist_0.Scale(weight)
            hist_0.SetLineWidth(3);
            hist_0.SetAxisRange(0,2500,'X')
            hist_0.SetName(histname+str(index))
        elif index is not 0 :
            hist_tmp=file_curr.Get(histname)
            hist_0.Add(hist_tmp,weight)

            print(hist_0)
    hist_0.SetName("Pythia8 MC")
    hists[hist_index].append(hist_0)
        
    #get data histogram
    file_data=ROOT.TFile(pathData,"READ")
    hist_data=file_data.Get(histname)
    hist_data.SetDirectory(0)
    hist_data.SetName("Data")
    hists[hist_index].insert(0,hist_data)


for i in range(0,len(ListOfCanvasses)) :
    for j  in range(0,len(hists[i])) :
        if j is not 0:
            AddHistogram(ListOfCanvasses[i],hists[i][j],'hist')
            AddRatio(ListOfCanvasses[i],hists[i][0],hists[i][j])

    SetAxisLabels(ListOfCanvasses[i],ListOfAxisLabels[i],'Events')
    FullFormatCanvasDefault(ListOfCanvasses[i],36)
    SetColors(ListOfCanvasses[i],colors)
    ListOfCanvasses[i].Print(outfilename+'_'+ListOfHists[i]+'.pdf')

        
    
