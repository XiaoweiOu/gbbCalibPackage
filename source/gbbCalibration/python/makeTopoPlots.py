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

histHelper = config.HistHelper()

basepath = '/data/users/aemerman/gbbCalibPackage/arcond/'

pathData = basepath + 'data16_13TeV.FTAG1.FTNtupCalib.v00-05-02VRa.181126.gbbHist.root'

#pathJZ3W = basepath + 'mc16_13TeV.427003.FTAG1.FTNtupCalib.v00-06-01VRa.181126.gbbHist.root'
pathJZ4W = basepath + 'mc16_13TeV.427004.FTAG1.FTNtupCalib.v00-06-01VRa.181126.gbbHist.root'
pathJZ5W = basepath + 'mc16_13TeV.427005.FTAG1.FTNtupCalib.v00-06-01VRa.181126.gbbHist.root'
pathJZ6W = basepath + 'mc16_13TeV.427106.FTAG1.FTNtupCalib.v00-06-01VRa.181126.gbbHist.root'
pathJZ7W = basepath + 'mc16_13TeV.427107.FTAG1.FTNtupCalib.v00-06-01VRa.181126.gbbHist.root'
#ListOfMCPaths = [ pathJZ3W, pathJZ4W, pathJZ5W, pathJZ6W, pathJZ7W ]
ListOfMCPaths = [ pathJZ4W, pathJZ5W, pathJZ6W, pathJZ7W ]

#make list of histograms
ListOfHists = ['h_Nom_Incl_Incl_trjeta', 'h_Nom_Incl_Incl_trjpt', 'h_Nom_Incl_Incl_trjmjDR' ]

#make list of axis labels
MapOfAxisLabels = {'h_Nom_Incl_Incl_trjeta':'Trigger Jet #eta', 'h_Nom_Incl_Incl_trjpt':'Trigger Jet p_{T} [GeV]', 'h_Nom_Incl_Incl_trjmjDR':'#DeltaR(Trigger Jet, Muon Jet)'}

colors = [ROOT.kBlue, ROOT.kBlack]
#SetColors(canvas,colors)

#loop over MC histograms
for histname in ListOfHists :
  file_curr = ROOT.TFile(pathData,"READ")
  if not file_curr:
    print("Cannot open file "+path)
    exit()
  histData = file_curr.Get(histname)
  if histData:
    histData.SetDirectory(0)
    histData.SetName(histname+'_Data')
  else:
    print("Cannot find hist "+histname+" in file "+pathData)
    continue

  histMC = histHelper.AddMCHists(histname,ListOfMCPaths)
  if histMC:
    histMC.Scale(Lumi)
    histMC.SetLineWidth(3);
    histMC.SetAxisRange(0,2500,'X')
    histMC.SetName("Pythia8 MC")
  else:
    print("Could not find "+histname+" in all input files!")
    continue

  canv = RatioCanvas('c_'+histname,"",800,800,0.3,'pE1')
  AddHistogram(canv,histMC,'hist')
  AddHistogram(canv,histData,'hist')
  AddRatio(canv,histData,histMC)
  SetAxisLabels(canv,MapOfAxisLabels[histname],'Events')
  FullFormatCanvasDefault(canv,36)
  SetColors(canv,colors)
  canv.Print(outfilename+'_'+histname+'.pdf')
