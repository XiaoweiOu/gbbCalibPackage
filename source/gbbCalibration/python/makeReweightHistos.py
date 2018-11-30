import ROOT
#from PlotFunctions import *
#from TAxisFunctions import *
import ConfigFunctions as config
import argparse

ROOT.gROOT.SetBatch(True)
from ROOT import TCanvas,TPad,TH2,TTree

#SetupStyle()

parser = argparse.ArgumentParser(description='Make reweight histograms.')
parser.add_argument('outfile', help="Name of output ROOT file")
parser.add_argument('infiles', help="JSON file with paths for data and MC files. See README for format")
args = parser.parse_args()

outfilename = args.outfile
pathData, ListOfMCPaths, ListOfInclusiveMCPaths = config.GetPathsFromJSON(args.infiles)

Lumi = 36000.0 #in pb^-1

histHelper = config.HistHelper()

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
