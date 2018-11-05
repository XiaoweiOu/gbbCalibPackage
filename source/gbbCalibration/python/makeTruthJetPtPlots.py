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

pathJZ3W = basepath + 'Output_Reweight/Reweight_user.ruth.mc15_13TeV.427003.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root/merged_hist_Reweight_user.ruth.mc15_13TeV.427003.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root' 
pathJZ4W = basepath + 'Output_Reweight/Reweight_user.ruth.mc15_13TeV.427004.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root/merged_hist_Reweight_user.ruth.mc15_13TeV.427004.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root' 
pathJZ5W = basepath + 'Output_Reweight/Reweight_user.ruth.mc15_13TeV.427005.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root/merged_hist_Reweight_user.ruth.mc15_13TeV.427005.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root' 
pathJZ6W = basepath + 'Output_Reweight/Reweight_user.ruth.mc15_13TeV.427106.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5839_s2726_r7773_r7676_p2949_tuple.root/merged_hist_Reweight_user.ruth.mc15_13TeV.427106.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5839_s2726_r7773_r7676_p2949_tuple.root' 
pathJZ7W = basepath + 'Output_Reweight/Reweight_user.ruth.mc15_13TeV.427107.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5839_s2726_r7773_r7676_p2949_tuple.root/merged_hist_Reweight_user.ruth.mc15_13TeV.427107.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5839_s2726_r7773_r7676_p2949_tuple.root' 

ListOfMCPaths = [ pathJZ3W, pathJZ4W, pathJZ5W, pathJZ6W, pathJZ7W ]

#make list of histograms
histname = 'truthjet_pt'

#make Canvas
canvas=ROOT.TCanvas("c_truthjet_pt","")

colors = [ROOT.kBlack,ROOT.kBlue,ROOT.kGreen+2, ROOT.kRed,ROOT.kAzure+5]
#SetColors(canvas,colors)

#loop over MC histograms
#index=0
hists = []
for path in ListOfMCPaths :        
    index = ListOfMCPaths.index(path)
    file_curr=ROOT.TFile(path,"READ")
    print("Open file "+path)
    
    bookkeep_hist=file_curr.Get("Hist_BookKeeping") #Events in AOD is in Bin 3
    weight=ListOfCrossSections[index]*ListOfFilterEfficiencies[index]/bookkeep_hist.GetBinContent(3)*Lumi
    print("weight is: "+str(weight))

    if not file_curr.GetListOfKeys().Contains(histname) :
        print("Cannot find first hist "+histname)
    else :
        hist_0=file_curr.Get(histname)
        hist_0.SetDirectory(0)
        hist_0.Scale(weight)
        #hist_0.SetLineColor(colors[index])
        hist_0.SetLineWidth(3);
        #hist_0.SetAxisRange(1.,1e9,'Y')
        hist_0.SetAxisRange(0,2500,'X')
        hist_0.SetName(histname+str(index))
        #AddHistogram(canvas,hist_0,"hist")
        print(hist_0)
        hists.append(hist_0)
        #canvas.cd()
        #if index is 0:
            #hist_0.Draw('hist')
         #   AddHistogram(canvas,hist_0,"hist")
        #else :
            #canvas.cd()
            #hist_0.Draw('histsame')
           # hist_0.SetName(histname+str(index))
            #AddHistogram(canvas,hist_0,"hist")
            #canvas.Update()

for hist in hists:
    AddHistogram(canvas, hist,"hist")

SetColors(canvas,colors)
SetYaxisRanges(canvas,1,1e9)
canvas.SetLogy()
#SetXaxisRanges(canvas,0,2500)
SetAxisLabels(canvas,'Truth Jet p_{T} [GeV]','Events')
FullFormatCanvasDefault(canvas)
SetYaxisRanges(canvas,1,1e9)
ROOT.gPad.RedrawAxis()
canvas.Update()
canvas.Print(outfilename)
        
    
