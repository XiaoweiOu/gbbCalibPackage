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

MyConfig = config.LoadGlobalConfig()
histHelper = config.HistHelper()

ListOfSystematics = [ ROOT.TString("Nom") ] #MyConfig.GetSystematics() 
ListOfFlavourPairs = MyConfig.GetFlavourPairs()
ListOfInclusiveFlavourPairs = [ 'LL' ]

basepath = '/data/users/aemerman/gbbCalibPackage/arcond/'

pathData = basepath + 'data16_13TeV.FTAG1.FTNtupCalib.v00-05-02VRa.181126.gbbHist.root'

#pathJZ3W = basepath + 'mc16_13TeV.427003.FTAG1.FTNtupCalib.v00-06-01VRa.181126.gbbHist.root'
pathJZ4W = basepath + 'mc16_13TeV.427004.FTAG1.FTNtupCalib.v00-06-01VRa.181126.gbbHist.root'
pathJZ5W = basepath + 'mc16_13TeV.427005.FTAG1.FTNtupCalib.v00-06-01VRa.181126.gbbHist.root'
pathJZ6W = basepath + 'mc16_13TeV.427106.FTAG1.FTNtupCalib.v00-06-01VRa.181126.gbbHist.root'
pathJZ7W = basepath + 'mc16_13TeV.427107.FTAG1.FTNtupCalib.v00-06-01VRa.181126.gbbHist.root'
#ListOfMCPaths = [ pathJZ3W, pathJZ4W, pathJZ5W, pathJZ6W, pathJZ7W ]
ListOfMCPaths = [ pathJZ4W, pathJZ5W, pathJZ6W, pathJZ7W ]

#pathJZ3W_incl = basepath + 'mc16_13TeV.361023.FTAG1.FTNtupCalib.v00-06-01VRa.181126.gbbHist.root'
pathJZ4W_incl = basepath + 'mc16_13TeV.361024.FTAG1.FTNtupCalib.v00-06-01VRa.181126.gbbHist.root'
pathJZ5W_incl = basepath + 'mc16_13TeV.361025.FTAG1.FTNtupCalib.v00-06-01VRa.181126.gbbHist.root'
pathJZ6W_incl = basepath + 'mc16_13TeV.361026.FTAG1.FTNtupCalib.v00-06-01VRa.181126.gbbHist.root'
pathJZ7W_incl = basepath + 'mc16_13TeV.361027.FTAG1.FTNtupCalib.v00-06-01VRa.181126.gbbHist.root'
#ListOfInclusiveMCPaths = [ pathJZ3W_incl, pathJZ4W_incl, pathJZ5W_incl, pathJZ6W_incl, pathJZ7W_incl ]
ListOfInclusiveMCPaths = [ pathJZ4W_incl, pathJZ5W_incl, pathJZ6W_incl, pathJZ7W_incl ]

ListOfVariables = [ 'maxSd0' ]

MapOfAxisLabels = { 'maxSd0':'#LT s_{d0} #GT' }

#make Canvas
canvMJ = ROOT.TCanvas("c_mujetSd0","",800,800);
canvNMJ = ROOT.TCanvas("c_nonmujetSd0","",800,800);

colors=[ROOT.kBlue+1,ROOT.kAzure-4,ROOT.kCyan+3,ROOT.kGreen-9,ROOT.kOrange]
#colors = [ROOT.kBlue, ROOT.kBlack]
#SetColors(canvas,colors)

#loop over MC histograms
for var in ListOfVariables :
  histsMJ = []
  histsNMJ = []
  for flavour in ListOfFlavourPairs:
    ListOfPaths = ListOfMCPaths
    if flavour in ListOfInclusiveFlavourPairs:
      ListOfPaths = ListOfInclusiveMCPaths

    for jet in [ 'mj', 'nmj' ]:
      histname = MyConfig.GetMCHistName("Nom","Incl",flavour,jet+var).Data()

      histMC = histHelper.AddMCHists(histname,ListOfPaths)
      if histMC:
        histMC.SetName(flavour.Data())
        histMC.SetLineWidth(3);
        #Normalize to Unity
        histMC.Scale(1./histMC.Integral())
        if jet is 'nmj':
          histsNMJ.append(histMC)
        elif jet is 'mj':
          histsMJ.append(histMC)
      else:
        print("Could not find "+histname+" in all input files!")

  SetAxisLabels(canvMJ,'Muon-associated Track Jet '+MapOfAxisLabels[var],'Normalised to Unity')
  for hist in histsMJ:
    AddHistogram(canvMJ,hist,'hist')
  FullFormatCanvasDefault(canvMJ,36,simulation=True, doLogy=True)
  SetColors(canvMJ,colors)
  canvMJ.Print(outfilename+'_'+var+'.pdf(')

  SetAxisLabels(canvNMJ,'Non-Muon-associated Track Jet '+MapOfAxisLabels[var],'Normalised to Unity')
  for hist in histsNMJ:
    AddHistogram(canvNMJ,hist,'hist')
  FullFormatCanvasDefault(canvNMJ,36,simulation=True, doLogy=True)
  SetColors(canvNMJ,colors)
  canvNMJ.Print(outfilename+'_'+var+'.pdf)')

print "done"
