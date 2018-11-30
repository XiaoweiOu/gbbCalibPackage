import ROOT
import sys
import math
import string
from PlotFunctions import *
from TAxisFunctions import *
import ConfigFunctions as config
import os
import argparse

ROOT.gROOT.SetBatch(True)
from ROOT import TCanvas,TPad,TH2,TColor

#ROOT.gStyle.SetOptStat(0)
SetupStyle()

parser = argparse.ArgumentParser(description='Make reweight histograms.')
parser.add_argument('outfile', help="Prefix of output files")
parser.add_argument('infiles', help="JSON file with paths for data and MC files. See README for format")
args = parser.parse_args()

outfilename = args.outfile
pathData, ListOfMCPaths, ListOfInclusiveMCPaths = config.GetPathsFromJSON(args.infiles)

Lumi = 36000.0 #in pb^-1

MapOfChannelWeights = config.GetChannelWeights()

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
