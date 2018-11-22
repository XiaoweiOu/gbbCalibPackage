import ROOT
import sys
import math
import string
#from PlotFunctions import *
#from TAxisFunctions import *
import ConfigFunctions as config
import os

ROOT.gROOT.SetBatch(True)
from ROOT import TCanvas,TPad,TH1,TFile

#SetupStyle()

outfilename = sys.argv[1]

Lumi = 36000.0; #in pb^-1

MyConfig = config.LoadGlobalConfig()
histHelper = config.HistHelper()

ListOfSystematics = [ ROOT.TString("Nom") ] #MyConfig.GetSystematics() 
ListOfFlavourPairs = MyConfig.GetFlavourPairs()
ListOfTJpt = MyConfig.GetTrkJetRegions()

basepath = '/data/users/aemerman/gbbCalibPackage/arcond/'

#pathJZ3W = basepath + 'mc16_13TeV.427003.FTAG1.FTNtupCalib.v00-06-01VRa.181126.gbbHist.root'
pathJZ4W = basepath + 'mc16_13TeV.427004.FTAG1.FTNtupCalib.v00-06-01VRa.181126.gbbHist.root'
pathJZ5W = basepath + 'mc16_13TeV.427005.FTAG1.FTNtupCalib.v00-06-01VRa.181126.gbbHist.root'
pathJZ6W = basepath + 'mc16_13TeV.427106.FTAG1.FTNtupCalib.v00-06-01VRa.181126.gbbHist.root'
pathJZ7W = basepath + 'mc16_13TeV.427107.FTAG1.FTNtupCalib.v00-06-01VRa.181126.gbbHist.root'
#ListOfMCPaths = [ pathJZ3W, pathJZ4W, pathJZ5W, pathJZ6W, pathJZ7W ]
ListOfMCPaths = [ pathJZ4W, pathJZ5W, pathJZ6W, pathJZ7W ]

pathData = basepath + 'data16_13TeV.FTAG1.FTNtupCalib.v00-05-02VRa.181126.gbbHist.root'

#ListOfVariables = [ 'fjpt', 'nmjpt', 'mjpt', 'mjmaxSd0', 'nmjmaxSd0' ]
ListOfVariables = [ 'fjpt' ] #, 'mjmaxSd0', 'nmjmaxSd0', 'fjpt_PREFITPOSTTAG', 'mjmaxSd0_PREFITPOSTTAG', 'nmjmaxSd0_PREFITPOSTTAG' ]
  
#make list of histograms in MC
ListOfHists = []
for flavour in ListOfFlavourPairs :
  for sys in ListOfSystematics :
    for tjpt in ListOfTJpt :
      for var in ListOfVariables :
        ListOfHists.append(MyConfig.GetMCHistName(sys,tjpt,flavour,var).Data())
        #print MyConfig.GetMCHistName(sys,tjpt,flavour,var).Data()

#make list of histograms in Data
ListOfDataHists = []
for tjpt in ListOfTJpt :
  for var in ListOfVariables :
    ListOfDataHists.append(MyConfig.GetDataHistName(tjpt,var).Data())
    #print MyConfig.GetDataHistName(tjpt,var).Data()

#open output file
outfile=ROOT.TFile(outfilename,"RECREATE")

#loop over MC histograms
for histname in ListOfHists :
  histMC = histHelper.AddMCHists(histname,ListOfMCPaths)
  if histMC:
    histMC.Scale(Lumi)
    outfile.cd()
    histMC.Write()
  else:
    print("Could not find "+histname+" in all input files!")
        
#loop over Data Histograms
file_curr = ROOT.TFile(pathData,"READ")
if not file_curr:
  print("Cannot open file "+pathData)
  exit()

for histname in ListOfDataHists :
  histData = file_curr.Get(histname)
  if histData:
    histData.SetDirectory(0)
    outfile.cd()
    histData.Write()    
  else:
    print("Cannot find hist "+histname+" in file "+pathData)
