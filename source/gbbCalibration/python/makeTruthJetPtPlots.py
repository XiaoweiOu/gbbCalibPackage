import ROOT
import sys
import math
import string
from PlotFunctions import *
from TAxisFunctions import *
import ConfigFunctions as config
import os

ROOT.gROOT.SetBatch(True)
from ROOT import TCanvas,TPad,TH2,TColor

#ROOT.gStyle.SetOptStat(0)
SetupStyle()

outfilename = sys.argv[1]

Lumi = 36000.0 #in pb^-1

MapOfChannelWeights = config.GetChannelWeights()

basepath = '/data/users/aemerman/gbbCalibPackage/arcond/'

#pathJZ3W = basepath + 'mc16_13TeV.427003.FTAG1.FTNtupCalib.v00-06-01VRa.181126.gbbHist.root'
pathJZ4W = basepath + 'mc16_13TeV.427004.FTAG1.FTNtupCalib.v00-06-01VRa.181126.gbbHist.root'
pathJZ5W = basepath + 'mc16_13TeV.427005.FTAG1.FTNtupCalib.v00-06-01VRa.181126.gbbHist.root'
pathJZ6W = basepath + 'mc16_13TeV.427106.FTAG1.FTNtupCalib.v00-06-01VRa.181126.gbbHist.root'
pathJZ7W = basepath + 'mc16_13TeV.427107.FTAG1.FTNtupCalib.v00-06-01VRa.181126.gbbHist.root'
#ListOfMCPaths = [ pathJZ3W, pathJZ4W, pathJZ5W, pathJZ6W, pathJZ7W ]
ListOfMCPaths = [ pathJZ4W, pathJZ5W, pathJZ6W, pathJZ7W ]

#make list of histograms
histname = 'truthjet_pt'

#make Canvas
canvas=ROOT.TCanvas("c_truthjet_pt","")

colors = [ROOT.kBlack,ROOT.kBlue,ROOT.kGreen+2, ROOT.kRed,ROOT.kAzure+5]
#SetColors(canvas,colors)

#loop over MC histograms
hists = []
for path in ListOfMCPaths :
  file_curr = ROOT.TFile(path,"READ")
  if not file_curr:
    print("Cannot open file "+path)
    exit()

  channel = config.GetChannelNumber(path)
  bookkeep_hist=file_curr.Get("Hist_BookKeeping") #Events in AOD is in Bin 3
  weight = MapOfChannelWeights[channel]/bookkeep_hist.GetBinContent(3)*Lumi
  
  histMC = file_curr.Get(histname)
  if histMC:
    histMC.SetDirectory(0)
    histMC.Scale(weight)
    #histMC.SetLineColor(colors[index])
    histMC.SetLineWidth(3);
    #histMC.SetAxisRange(1.,1e9,'Y')
    histMC.SetAxisRange(0,2500,'X')
    histMC.SetName(histname+"_"+str(channel))
    hists.append(histMC)
  else:
    print("Cannot find hist "+histname+" in file "+path)

for hist in hists:
    AddHistogram(canvas, hist, "hist")

SetColors(canvas,colors)
canvas.SetLogy()
SetAxisLabels(canvas,'Truth Jet p_{T} [GeV]','Events')
FullFormatCanvasDefault(canvas)
#SetXaxisRanges(canvas,0,2500)
SetYaxisRanges(canvas,1,1e9)
ROOT.gPad.RedrawAxis()
canvas.Update()
canvas.Print(outfilename)    
