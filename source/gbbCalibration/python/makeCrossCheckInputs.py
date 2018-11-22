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

Lumi = 36000.0; #in pb^-1

MapOfChannelWeights = config.GetChannelWeights() #values are cross-section (in pb) * filter efficiency * k-factor
MyConfig = config.LoadGlobalConfig()

basepath = '/data/users/aemerman/gbbCalibPackage/arcond/'

#pathJZ3W = basepath + 'mc16_13TeV.427003.FTAG1.FTNtupCalib.v00-05-02VRa.ggbHist.root'
pathJZ4W = basepath + 'mc16_13TeV.427004.FTAG1.FTNtupCalib.v00-05-02VRa.ggbHist.root'
pathJZ5W = basepath + 'mc16_13TeV.427005.FTAG1.FTNtupCalib.v00-05-02VRa.ggbHist.root'
pathJZ6W = basepath + 'mc16_13TeV.427106.FTAG1.FTNtupCalib.v00-05-02VRa.ggbHist.root'
pathJZ7W = basepath + 'mc16_13TeV.427107.FTAG1.FTNtupCalib.v00-05-02VRa.ggbHist.root'
#ListOfMCPaths = [ pathJZ3W, pathJZ4W, pathJZ5W, pathJZ6W, pathJZ7W ]
ListOfMCPaths = [ pathJZ4W, pathJZ5W, pathJZ6W, pathJZ7W ]

pathData = basepath + 'data16_13TeV.FTAG1.FTNtupCalib.v00-05-02VRa.gbbHist.root'

#ListOfVariables = [ 'fjpt', 'nmjpt', 'mjpt', 'mjmaxSd0', 'nmjmaxSd0' ]
ListOfVariables = [ 'fjpt' ] #, 'mjmaxSd0', 'nmjmaxSd0', 'fjpt_PREFITPOSTTAG', 'mjmaxSd0_PREFITPOSTTAG', 'nmjmaxSd0_PREFITPOSTTAG' ]
  
ListOfSystematics = [ 'Nom' ]

ListOfFlavourPairs = MyConfig.GetFlavourPairs()

ListOfTJpt = MyConfig.GetTrkJetRegions()

outfilename = 'testinputs.root'

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
  hist_index=ListOfHists.index(histname)
  histMC=None
  for path in ListOfMCPaths :
    file_curr = ROOT.TFile(path,"READ")
    if not file_curr:
      print("Cannot open file "+path)
      exit()

    channel = config.GetChannelNumber(path)
    bookkeep_hist=file_curr.Get("Hist_BookKeeping") #Events in AOD is in Bin 3
    weight = MapOfChannelWeights[channel]/bookkeep_hist.GetBinContent(3)*Lumi
    
    if not histMC:
      histMC = file_curr.Get(histname)
      if histMC:
        histMC.SetDirectory(0)
        histMC.Scale(weight)
      else:
        print("Cannot find hist "+histname+" in file "+path)
    else :
      histTemp = file_curr.Get(histname)
      if histTemp:
        histMC.Add(histTemp,weight)
      else:
        print("Cannot find hist "+histname+" in file "+path)
  if histMC:
    outfile.cd()
    histMC.Write()
        
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
