import ROOT
from PlotFunctions import *
from TAxisFunctions import *
import ConfigFunctions as config
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

MyConfig = config.LoadGlobalConfig()
histHelper = config.HistHelper()

ListOfSystematics = [ ROOT.TString("Nom") ] #MyConfig.GetSystematics() 
ListOfFlavourPairs = MyConfig.GetFlavourPairs()
ListOfInclusiveFlavourPairs = [ 'LL' ]

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
