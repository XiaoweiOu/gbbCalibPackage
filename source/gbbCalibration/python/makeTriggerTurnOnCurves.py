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
from ROOT import TCanvas,TPad

#SetupStyle()

parser = argparse.ArgumentParser(description='Make reweight histograms.')
parser.add_argument('outfile', help="Name of output ROOT file")
parser.add_argument('infiles', help="JSON file with paths for data and MC files. See README for format")
args = parser.parse_args()

outfilename = args.outfile
pathData, ListOfMCPaths, ListOfInclusiveMCPaths = config.GetPathsFromJSON(args.infiles)

Lumi = 36000.0; #in pb^-1

histHelper = config.HistHelper()

ListOfTriggers = [ 'HLT85', 'HLT110', 'HLT150', 'HLT175', 'HLT260', 'HLT360', 'HLT380' ]

ListOfPrescales = [ 7.378e-5, 5.479e-4, 7.633e-4, 3.945e-3, 2.453e-2, 2.514e-1, 1.0]

BaseHistName = 'trigjet_pt_noHLTreq'

#open output file
outfile=ROOT.TFile(outfilename,"RECREATE")

#make base histogram
histBase = histHelper.AddMCHists(BaseHistName,ListOfMCPaths)
histBase.Scale(Lumi)
if not histBase:
  print("Could not make hist "+BaseHistName)
  exit()

#loop over MC histograms
for trig in ListOfTriggers:
  histname = 'trigjet_pt_pass'+trig
  histMC = histHelper.AddMCHists(histname,ListOfMCPaths)
  if histMC:
    histMC.Scale(Lumi)
    histMC.Divide(histBase)
    histMC.SetName('trig_turnon_'+trig)
    histMC.SetMarkerStyle(20)
    outfile.cd()
    histMC.Write()
    for i in range(1, histMC.GetSize()-2):
      if histMC.GetBinContent(i) >= 0.99:
        print(trig+" fully efficient at pT = "+str(histMC.GetBinCenter(i)))
        break
  else:
    print("Could not find "+histname+" in all input files!")
