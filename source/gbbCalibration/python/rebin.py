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

ListOfTJpt = MyConfig.GetTrkJetRegions()
ListOfTJpt.push_back(TString("Incl"))
#ListOfTJpt = [ TString("Incl") ]
isR20p7 = MyConfig.GetIsR20p7()

for tjpt in ListOfTJpt:
  for var in ListOfTmplVars:
    # Read in data histogram
    histname = MyConfig.GetDataHistName(tjpt,var).Data()
    dataHist = infile.Get(histname)
    if not dataHist:
      print(histname+' not found. Exiting...')
      exit()

    # Read in nominal MC histograms
    mcHists = []
    for flav in ListOfFlavourPairs:
      histname = MyConfig.GetMCHistName("Nom",tjpt,flav,var).Data()
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
    print("| AutoMerger channel "+tjpt.Data()+' '+var.Data())
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
      print("| Binning in channel "+tjpt.Data()+' '+var.Data()+" is ok! Proceed to fit... ")
      outfile.cd()
      if args.rename:
        dataHist.SetName(ReverseName(dataHist.GetName()))
      dataHist.Write()
      for hist in mcHists:
        if args.rename:
          hist.SetName(ReverseName(hist.GetName()))
        hist.Write()
    else:
      if dataHist.GetXaxis().GetBinLowEdge(dataHist.GetNbinsX()+1) not in bins:
        bins.append(dataHist.GetXaxis().GetBinLowEdge(dataHist.GetNbinsX()+1))
      print("|")
      print("| New Binning in channel "+tjpt.Data()+' '+var.Data()+" :")
      print("| NBins = "+str(len(bins)))
      print("| ")
      for i,xbin in enumerate(bins):
        print("| [bin edge "+str(i)+"] "+str(xbin))
      print("==================================")

      outfile.cd()
      newDataHist = dataHist.Rebin(len(bins)-1,dataHist.GetName(),np.array(bins))
      if args.rename:
        newDataHist.SetName(ReverseName(newDataHist.GetName()))
      newDataHist.Write()
      for hist in mcHists:
        newMcHist = hist.Rebin(len(bins)-1,hist.GetName(),np.array(bins))
        if args.rename:
          newMcHist.SetName(ReverseName(newMcHist.GetName()))
        newMcHist.Write()

# Copy plot variables from infile to outfile
for tjpt in ListOfTJpt:
  for var in ListOfPlotVars:
    # Read in data histogram
    histname = MyConfig.GetDataHistName(tjpt,var).Data()
    dataHist = infile.Get(histname)
    if not dataHist:
      print(histname+' not found. Skipping...')
    else:
      if args.rename:
        dataHist.SetName(ReverseName(dataHist.GetName()))
      outfile.cd()
      dataHist.Write()

    # Read in nominal MC histograms
    for flav in ListOfFlavourPairs:
      histname = MyConfig.GetMCHistName("Nom",tjpt,flav,var).Data()
      mcHist = infile.Get(histname)
      if not mcHist:
        print(histname+' not found. Skipping...')
      else:
        outfile.cd()
        if args.rename:
          mcHist.SetName(ReverseName(mcHist.GetName()))
        mcHist.Write()

infile.Close()
outfile.Close()
print('done!')
