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
from ROOT import TCanvas,TPad,TH1,TFile

#SetupStyle()

parser = argparse.ArgumentParser(description='Make reweight histograms.')
parser.add_argument('outfile', help="Name of output ROOT file")
parser.add_argument('infiles', help="JSON file with paths for data and MC files. See README for format")
args = parser.parse_args()

outfilename = args.outfile
pathData, ListOfMCPaths, ListOfInclusiveMCPaths = config.GetPathsFromJSON(args.infiles)

Lumi = 36000.0; #in pb^-1

MyConfig = config.LoadGlobalConfig()
histHelper = config.HistHelper()

ListOfSystematics = [ ROOT.TString("Nom") ] #MyConfig.GetSystematics() 
ListOfFlavourPairs = MyConfig.GetFlavourPairs()
ListOfTJpt = MyConfig.GetTrkJetRegions()

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
