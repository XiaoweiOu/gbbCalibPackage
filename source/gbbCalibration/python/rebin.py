import ROOT
import ConfigFunctions as config
import numpy as np
import argparse
from itertools import chain
from ROOT import TFile,TString,TH1D

ROOT.gROOT.SetBatch(True)
ROOT.TH1.SetDefaultSumw2()

# geting input variables
parser = argparse.ArgumentParser(description='Make reweight histograms.')
parser.add_argument('outfile', help="Name of output ROOT file")
parser.add_argument('infile', help="Name of input ROOT file")
parser.add_argument('--nosys', help="Rebin only nominal histograms",
                    action="store_true")
parser.add_argument('--stat', type=float, default=0.25,
                    help="Stat threshold (err/N) for rebinning [default: 0.25]")
parser.add_argument('--force', type=int, default=1,
                    help="Force rebinning by at least n [default: 1]")
args = parser.parse_args()

# open input file
infile = TFile(args.infile,"READ")
if infile.IsZombie():
  print("Cannot open file "+args.infile)
  exit()

# open output file
outfile=TFile(args.outfile,"CREATE")
if outfile.IsZombie():
  print("Cannot create file "+args.outfile)
  exit()

def getKey(infile,key):
  obj = infile.Get(key)
  if not obj:
    print('cannot find '+key+' in file '+infile.GetName())
    exit()
  return obj

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

def GetBinsByStats(hist,thr,n_f):
  '''
  Define a new binning for input histogram such that
  (bin error)/(bin value) < threshold for all bins.
  New minimum bin width is N_force*(bin width).
  Inputs are: histogram, threshold, leftmost bin to check.
  Output is: list of bin edges.
  '''
  # TODO: First do some basic sanity checks

  # Add bins below imin to output, keeping them unchanged
  bins = [ hist.GetBinLowEdge(1) ]

  # Main body of the algorithm
  # Loops over histogram bins storing contents and sum of squares of errors
  # When it find a bin (or combination of bins) with err/val < threshold
  # it stores a new bin edge and resets the counter
  # Also requires that all bins have non-zero contents
  nbins = hist.GetNbinsX()+1
  val = 0
  err2 = 0
  ctr = 0
  for ibin in range(1,nbins):
    val  += hist.GetBinContent(ibin)
    err2 += hist.GetBinError(ibin)*hist.GetBinError(ibin)
    ctr += 1
    if (val > 1e-6 and np.sqrt(err2)/val < thr and ctr >= n_f):
      bins.append( hist.GetBinLowEdge(ibin+1) )
      val = 0
      err2 = 0
      ctr = 0

  # if last bin would fail threshold, append it to second-to-last
  high_edge = hist.GetBinLowEdge(nbins)
  if high_edge not in bins:
    # edge case : histogram has only 1 bin left
    if len(bins) == 1:
      bins.append(high_edge)
    else:
      bins[len(bins)-1] = high_edge

  return bins

def Rebin(hist,bins,pseudo=False):
  '''
  Rebin input histogram using given list of bin edges. If pseudo=True
  then the returned histogram is re-parametrized to ensure fixed-width bins.
  Inputs are: histogram, bin edges, pseudo-histogram flag
  Output is: rebinned histogram
  '''
  # TODO: First do some basic sanity checks

  new_hist = None
  if not pseudo:
    new_hist = hist.Rebin(len(bins)-1,hist.GetName()+'_rebin',np.array(bins))
    # Divide bin content by change in bin width so that scale is unchanged
    # Assumes input histogram has constant bin width
    width = hist.GetBinWidth(1)
    for ibin in range(1,new_hist.GetNbinsX()+1):
      corr = new_hist.GetBinWidth(ibin) / width
      new_hist.SetBinContent(ibin, new_hist.GetBinContent(ibin) / corr )
      new_hist.SetBinError(ibin, new_hist.GetBinError(ibin) / corr )
  else:
    # HistFactory doesn't like variable-bin hists so instead
    # we compress the merged bins to the same size as the rest
    # Note that x-axis is meaningless after this
    new_nbins = len(bins)-1
    new_hist = TH1D(hist.GetName()+'_rebin_pseudo','',new_nbins,0,new_nbins)
    left_edge = 1
    for ibin in range(1,new_nbins+1):
      jbin = left_edge
      val  = 0
      err2 = 0
      # Loop through the input histogram until the next edge is crossed
      while bins[ibin] > hist.GetBinLowEdge(jbin):
        val  += hist.GetBinContent(jbin)
        err2 += hist.GetBinError(jbin)*hist.GetBinError(jbin)
        jbin += 1

      # Divide bin content by change in bin width so that scale is unchanged
      # Assumes input histogram has constant bin width
      val  = val  / (jbin - left_edge)
      err2 = err2 / (jbin - left_edge)

      new_hist.SetBinContent(ibin, val)
      new_hist.SetBinError(ibin, np.sqrt(err2))

      # Set left edge for next bin
      left_edge = jbin

  return new_hist

def RebinHistsAll(region,var):
  h_data = getKey(infile, MyConfig.GetDataHistName(region,var).Data() )
  h_mcBB = getKey(infile, MyConfig.GetMCHistName('Nom',region,'BB',var).Data() )
  bins = GetBinsByStats(h_mcBB, args.stat, args.force)

  print("==================================")
  print("| New Binning in channel "+region.Data()+' '+var.Data()+" :")
  print("| NBins = "+str(len(bins)))
  print("| ")
  for i,xbin in enumerate(bins):
    print("| [bin edge "+str(i)+"] "+str(xbin))
  print("==================================")

  new_data = Rebin(h_data,bins)
  new_data.GetYaxis().SetTitle('dN/d'+var.Data())
  new_data.Write(h_data.GetName())

  systs = ['Nom']
  if not args.nosys:
    systs = chain.from_iterable([['Nom'],ListOfSystematics,ListOfSd0Systematics])
  for sys in systs:
    for flav in ListOfFlavourPairs:
      histname = MyConfig.GetMCHistName(sys,region,flav,var).Data()
      h_mc = getKey(infile,histname)
      new_mc = Rebin(h_mc,bins)
      new_mc.GetYaxis().SetTitle('dN/d'+var.Data())
      new_mc.Write(h_mc.GetName())

def CopyHists(region,var):
  # Read in data histogram
  histname = MyConfig.GetDataHistName(region,var).Data()
  dataHist = infile.Get(histname)
  if not dataHist:
    print(histname+' not found. Skipping...')
  else:
    outfile.cd()
    dataHist.Write(dataHist.GetName())

  # Read in nominal MC histograms
  for flav in ListOfFlavourPairs:
    histname = MyConfig.GetMCHistName("Nom",region,flav,var).Data()
    mcHist = infile.Get(histname)
    if not mcHist:
      print(histname+' not found. Skipping...')
    else:
      outfile.cd()
      mcHist.Write(mcHist.GetName())

# pt-inclusive bin first
# Rebin template variables
for var in ListOfTmplVars:
  RebinHistsAll(TString('Incl'),var)
  RebinHistsAll(TString('Incl'),TString(var.Data()+'_2TAG'))
  RebinHistsAll(TString('Incl'),TString(var.Data()+'_NOT2TAG'))
# Copy plot variables from infile to outfile
for var in ListOfPlotVars:
  if var in ListOfTmplVars:
    continue
  CopyHists(TString('Incl'),var)
  CopyHists(TString('Incl'),var.Data()+'_2TAG')

# track-jet bins
for region in ListOfTJpt:
  # Rebin template variables
  for var in ListOfTmplVars:
    RebinHistsAll(region,var)
    RebinHistsAll(region,TString(var.Data()+'_2TAG'))
    RebinHistsAll(region,TString(var.Data()+'_NOT2TAG'))
  # Copy plot variables from infile to outfile
  for var in ListOfPlotVars:
    if var in ListOfTmplVars:
      continue
    CopyHists(region,var)
    CopyHists(region,var.Data()+'_2TAG')

# fat-jet bins
for region in ListOfFJpt:
  # Rebin template variables
  for var in ListOfTmplVars:
    RebinHistsAll(region,var)
    RebinHistsAll(region,TString(var.Data()+'_2TAG'))
    RebinHistsAll(region,TString(var.Data()+'_NOT2TAG'))
  # Copy plot variables from infile to outfile
  for var in ListOfPlotVars:
    if var in ListOfTmplVars:
      continue
    CopyHists(region,var)
    CopyHists(region,var.Data()+'_2TAG')

infile.Close()
outfile.Close()
print('done!')
