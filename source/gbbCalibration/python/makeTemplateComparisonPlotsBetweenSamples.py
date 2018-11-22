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

colors=[]
colors.append([ROOT.kBlue+1,ROOT.kBlue+1])
colors.append([ROOT.kAzure-4,ROOT.kAzure-4])
colors.append([ROOT.kCyan+3,ROOT.kCyan+3])
colors.append([ROOT.kGreen-9,ROOT.kGreen-9])
colors.append([ROOT.kOrange,ROOT.kOrange])
colors.append([ROOT.kBlue+1,ROOT.kBlue+1])
colors.append([ROOT.kAzure-4,ROOT.kAzure-4])
colors.append([ROOT.kCyan+3,ROOT.kCyan+3])
colors.append([ROOT.kGreen-9,ROOT.kGreen-9])
colors.append([ROOT.kOrange,ROOT.kOrange])

#colors = [ROOT.kBlue, ROOT.kBlack]
#SetColors(canvas,colors)

#loop over MC histograms
for var in ListOfVariables :
  for flavour in ListOfFlavourPairs:
    for jet in [ 'mj', 'nmj' ]:
      histname = MyConfig.GetMCHistName("Nom","Incl",flavour,jet+var).Data()

      hist = histHelper.AddMCHists(histname,ListOfMCPaths)
      histIncl = histHelper.AddMCHists(histname,ListOfInclusiveMCPaths)
      if hist:
        hist.Scale(Lumi)
        hist.SetName(flavour.Data()+" #mu-filtered dijet");
        hist.SetLineWidth(3);
      else:
        print("Could not find "+histname+" in mu-filtered input files!")
        continue

      if histIncl:
        histIncl.Scale(Lumi)
        histIncl.SetName(flavour.Data()+" inclusive dijet");
        histIncl.SetLineWidth(3);
      else:
        print("Could not find "+histname+" in inclusive input files!")

      #Print Integral
      print('##########################')
      print('# Integral report for '+histname)
      print('# mu-filtered: '+str(hist.Integral()))
      print('# inclusive: '+str(histIncl.Integral()))
      print('##########################')
      #Normalize to Unity
      hist.Scale(1./hist.Integral())
      histIncl.Scale(1./histIncl.Integral())

      #Make Canvas
      canv = ROOT.TCanvas('c_'+histname,"",800,800);
      if jet is 'mj':
        SetAxisLabels(canv,'Muon-associated Track Jet '+MapOfAxisLabels[var],'Normalised to Unity')
      elif jet is 'nmj':
        SetAxisLabels(canv,'Non-Muon-associated Track Jet '+MapOfAxisLabels[var],'Normalised to Unity')
      AddHistogram(canv,hist,'hist')
      AddHistogram(canv,histIncl,'hist')
      #SetColors(canv,colors)
      FullFormatCanvasDefault(canv,36,simulation=True, doLogy=True)
      canv.Print(outfilename+'_'+histname+'.pdf')
