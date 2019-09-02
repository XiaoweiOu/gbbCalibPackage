import ROOT
import ConfigFunctions as config
import numpy as np
import argparse
from ROOT import TString

ROOT.gROOT.SetBatch(True)
ROOT.TH1.SetDefaultSumw2()

# geting input variables
parser = argparse.ArgumentParser(description='Make reweight histograms.')
parser.add_argument('outfile', help="Name of output ROOT file")
parser.add_argument('infile', help="Name of input ROOT file")
parser.add_argument('--nosys', help="Rebin only nominal histograms",
                    action="store_true")
parser.add_argument('-r','--rename', help="Rename histograms for TRexFitter",
                    action="store_true")
parser.add_argument('--stat', type=float, default=0.25,
                    help="Stat threshold (err/N) for rebinning [default: 0.25]")
parser.add_argument('--force', type=int, default=1,
                    help="Force rebinning by at least n [default: 1]")
args = parser.parse_args()

def ReverseName(name):
  # Rename for TRexFitter
  tokens = name.split('_',1)
  newname = tokens[1]+'_'+tokens[0]
  return newname

# open input file
infile = ROOT.TFile(args.infile,"READ")
if infile.IsZombie():
  print("Cannot open file "+args.infile)
  exit()

# open output file
outfile=ROOT.TFile(args.outfile,"CREATE")
if outfile.IsZombie():
  print("Cannot create file "+args.outfile)
  exit()

MyConfig = config.LoadGlobalConfig()

# getting config variables
if (args.nosys):
  ListOfSystematics = [ TString("Nom") ]
  ListOfSd0Systematics = [ ]
  ListOfWeightVariations = [ ]
else:
  ListOfSystematics = MyConfig.GetSystematics()
  ListOfSystematics.push_back(TString("Nom"))
  ListOfSd0Systematics = MyConfig.GetSystematics_Sd0()
  ListOfWeightVariations = MyConfig.GetSystematics_WeightVar()

ListOfFlavourPairs = MyConfig.GetFlavourPairs()
ListOfTmplVars = MyConfig.GetTemplateVariables()
ListOfPlotVars = MyConfig.GetPlotVariables()

ListOfFJpt = MyConfig.GetFatJetRegions()
ListOfTJpt = MyConfig.GetTrkJetRegions()
#ListOfTJpt = [ TString("Incl") ]
isR20p7 = MyConfig.GetIsR20p7()

def MakeNEventHist(region):
  # Read in data histogram
  histname = MyConfig.GetDataHistName(region,ListOfTmplVars[0].Data()+'_PREFITPOSTTAG').Data()
  dataHist = infile.Get(histname)
  if not dataHist:
    print(histname+' not found. Exiting...')
    exit()
  newDataHist = ROOT.TH1F(region+'_NEvts_PREFITPOSTTAG_hDataNom','',1,0,1)
  newDataHist.SetBinContent(1,dataHist.Integral(0,dataHist.GetNbinsX()+1))
  outfile.cd()
  newDataHist.Write()

  # Read in nominal MC histograms
  for flav in ListOfFlavourPairs:
    histname = MyConfig.GetMCHistName("Nom",region,flav,ListOfTmplVars[0].Data()+'_PREFITPOSTTAG').Data()
    mcHist = infile.Get(histname)
    if not mcHist:
      print(histname+' not found. Exiting...')
      exit()
    newMcHist = ROOT.TH1F(region+'_NEvts_PREFITPOSTTAG_h'+flav.Data()+'Nom','',1,0,1)
    newMcHist.SetBinContent(1,mcHist.Integral(0,mcHist.GetNbinsX()+1))
    outfile.cd()
    newMcHist.Write()

def RebinHist(region,var):
  # Read in data histogram
  histname = MyConfig.GetDataHistName(region,var).Data()
  dataHist = infile.Get(histname)
  if not dataHist:
    print(histname+' not found. Exiting...')
    exit()

  # Read in nominal MC histograms
  mcHists = []
  for flav in ListOfFlavourPairs:
    histname = MyConfig.GetMCHistName("Nom",region,flav,var).Data()
    mcHist = infile.Get(histname)
    if not mcHist:
      print(histname+' not found. Exiting...')
      exit()
    else:
      mcHists.append(mcHist)

  # bins will hold new binning vector
  bins = []
  bins.append(dataHist.GetXaxis().GetBinLowEdge(1))

  print("==================================")
  print("| AutoMerger channel "+region.Data()+' '+var.Data())
  print("|")
  # Loop over histograms, summing bins until stat threshold is reached
  curr_bin = 1
  createBin = True
  binningChanged = False
  for i_bin in range(1,dataHist.GetNbinsX()+1):
    if (createBin):
      n = np.zeros(len(mcHists))
      err = np.zeros(len(mcHists))

    createBin = True
    for i_h, hist in enumerate(mcHists):
      n[i_h] += hist.GetBinContent(i_bin)
      err[i_h] += hist.GetBinError(i_bin)*hist.GetBinError(i_bin)
      if (i_bin+1 - curr_bin) < args.force:
        createBin = False
        continue
      if (n[i_h] < 1e-5 or np.sqrt(err[i_h])/n[i_h] > args.stat):
        createBin = False
        continue

    if createBin:
      bins.append(dataHist.GetXaxis().GetBinLowEdge(i_bin+1))
      curr_bin = i_bin
    else:
      #print("| Info: Merge bin "+str(i_bin+1)+" into "+str(curr_bin+1))
      binningChanged=True

  if not binningChanged:
    print("|")
    print("| Binning in channel "+region.Data()+' '+var.Data()+" is ok! Proceed to fit... ")
    outfile.cd()
    dataHist.Write(ReverseName(dataHist.GetName()) if args.rename else dataHist.GetName())
    for hist in mcHists:
      hist.Write(ReverseName(hist.GetName()) if args.rename else hist.GetName())
  else:
    if dataHist.GetXaxis().GetBinLowEdge(dataHist.GetNbinsX()+1) not in bins:
      bins.append(dataHist.GetXaxis().GetBinLowEdge(dataHist.GetNbinsX()+1))
    print("|")
    print("| New Binning in channel "+region.Data()+' '+var.Data()+" :")
    print("| NBins = "+str(len(bins)))
    print("| ")
    for i,xbin in enumerate(bins):
      print("| [bin edge "+str(i)+"] "+str(xbin))
    print("==================================")

    outfile.cd()
    newDataHist = dataHist.Rebin(len(bins)-1,dataHist.GetName()+'_REBIN',np.array(bins))
    newDataHist.Write(ReverseName(dataHist.GetName()) if args.rename else dataHist.GetName())
    for hist in mcHists:
      newMcHist = hist.Rebin(len(bins)-1,hist.GetName()+'_REBIN',np.array(bins))
      newMcHist.Write(ReverseName(hist.GetName()) if args.rename else hist.GetName())

def CopyHists(region,var):
  # Read in data histogram
  histname = MyConfig.GetDataHistName(region,var).Data()
  dataHist = infile.Get(histname)
  if not dataHist:
    print(histname+' not found. Skipping...')
  else:
    outfile.cd()
    dataHist.Write(ReverseName(dataHist.GetName()) if args.rename else dataHist.GetName())

  # Read in nominal MC histograms
  for flav in ListOfFlavourPairs:
    histname = MyConfig.GetMCHistName("Nom",region,flav,var).Data()
    mcHist = infile.Get(histname)
    if not mcHist:
      print(histname+' not found. Skipping...')
    else:
      outfile.cd()
      mcHist.Write(ReverseName(mcHist.GetName()) if args.rename else mcHist.GetName())

# pt-inclusive bin first
# Rebin template variables
MakeNEventHist('Incl')
for var in ListOfTmplVars:
  RebinHist(TString('Incl'),var)
  RebinHist(TString('Incl'),TString(var.Data()+'_PREFITPOSTTAG'))
# Copy plot variables from infile to outfile
for var in ListOfPlotVars:
  if var in ListOfTmplVars:
    continue
  CopyHists(TString('Incl'),var)
  CopyHists(TString('Incl'),var.Data()+'_PREFITPOSTTAG')

# track-jet bins
for region in ListOfTJpt:
  MakeNEventHist(region.Data())
  # Rebin template variables
  for var in ListOfTmplVars:
    RebinHist(region,var)
    RebinHist(region,TString(var.Data()+'_PREFITPOSTTAG'))
  # Copy plot variables from infile to outfile
  for var in ListOfPlotVars:
    if var in ListOfTmplVars:
      continue
    CopyHists(region,var)
    CopyHists(region,var.Data()+'_PREFITPOSTTAG')

# fat-jet bins
for region in ListOfFJpt:
  if 'l500' in region.Data():
    continue
  MakeNEventHist(region.Data())
  # Rebin template variables
  for var in ListOfTmplVars:
    RebinHist(region,var)
    RebinHist(region,TString(var.Data()+'_PREFITPOSTTAG'))
  # Copy plot variables from infile to outfile
  for var in ListOfPlotVars:
    if var in ListOfTmplVars:
      continue
    CopyHists(region,var)
    CopyHists(region,var.Data()+'_PREFITPOSTTAG')

infile.Close()
outfile.Close()
print('done!')
