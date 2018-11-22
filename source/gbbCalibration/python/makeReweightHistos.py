import ROOT
import sys
import math
import string
#from PlotFunctions import *
#from TAxisFunctions import *
import ConfigFunctions as config
import os
import argparse

ROOT.gROOT.SetBatch(True)
from ROOT import TCanvas,TPad,TH2,TTree

#SetupStyle()

#parser = argparse.ArgumentParser(description='Make reweight histograms.')
#parser.add_argument('outfile', type=argparse.FileType('w'))
#parser.add_argument('--mc', nargs='+')
#parser.add_argument('--data', nargs=1)

outfilename = sys.argv[1]

Lumi = 36000.0 #in pb^-1

histHelper = config.HistHelper()

basepath = '/data/users/aemerman/gbbCalibPackage/arcond/'

pathJZ3W = basepath + 'mc16_13TeV.427003.FTAG1.FTNtupCalib.v00-05-02VRa.Reweight.root'
pathJZ4W = basepath + 'mc16_13TeV.427004.FTAG1.FTNtupCalib.v00-05-02VRa.Reweight.root'
pathJZ5W = basepath + 'mc16_13TeV.427005.FTAG1.FTNtupCalib.v00-05-02VRa.Reweight.root'
pathJZ6W = basepath + 'mc16_13TeV.427106.FTAG1.FTNtupCalib.v00-05-02VRa.Reweight.root'
pathJZ7W = basepath + 'mc16_13TeV.427107.FTAG1.FTNtupCalib.v00-05-02VRa.Reweight.root'

pathData = basepath + 'data16_13TeV.FTAG1.FTNtupCalib.v00-05-02VRa.Reweight.root'

ListOfMCPaths = [ pathJZ3W, pathJZ4W, pathJZ5W, pathJZ6W, pathJZ7W ]

ListOfTriggers = [ 'HLT_j380' ]
#ListOfTriggers = [ 'HLT_j150', 'HLT_j175', 'HLT_j260', 'HLT_j380' ]

#make list of histograms
ListOfHists = []
for trig in ListOfTriggers :
    ListOfHists.append('reweight_trigjet_pt_eta_'+trig)

#open output file
outfile=ROOT.TFile(outfilename,"RECREATE")

#loop over MC histograms
for histname in ListOfHists :
  file_curr = ROOT.TFile(pathData,"READ")
  if not file_curr:
    print("Cannot open file "+path)
    exit()
  histData = file_curr.Get(histname)
  if histData:
    histData.SetDirectory(0)
    histData.SetName('h_'+histname)
  else:
    print("Cannot find hist "+histname+" in file "+pathData)
    continue

  histMC = histHelper.AddMCHists(histname,ListOfMCPaths)
  if histMC:
    histMC.Scale(Lumi)
    histData.Divide(histMC)
    histData.SetName('h_'+histname)
    outfile.cd()
    histData.Write() 
  else:
    print("Could not find "+histname+" in all input files!")
