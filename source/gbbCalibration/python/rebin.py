import numpy as np
import argparse
from itertools import chain

# geting input variables
parser = argparse.ArgumentParser(description='Make reweight histograms.')
parser.add_argument('outfile', help="Name of output ROOT file")
parser.add_argument('infile', help="Name of input ROOT file")
parser.add_argument('--nosys', help="Rebin only nominal histograms",
                    action="store_true")
parser.add_argument('--stat', type=float, default=0.25,
                    help="Stat threshold (err/N) for rebinning [default: 0.25]")
parser.add_argument('--force', type=int, default=2,
                    help="Force rebinning by at least n [default: 2]")
parser.add_argument('--hist', type=str, default='BB',
                    help="Changes which hists are checked for stat threshold. Options are 'data', 'MC', or any of the flavour-pair labels [default: BB].")
parser.add_argument('--split', action='store_true',
                    help="Rebin negative tail separately (merging from right to left)")
args = parser.parse_args()

# import ROOT after argument parsing so it can't steal arguments
import ConfigFunctions as config
from ROOT import gROOT,TFile,TString,TH1,TH1D

gROOT.SetBatch(True)
TH1.SetDefaultSumw2()

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
ListOfTJpt = MyConfig.GetDiTrkJetRegions()

def GetBinsByStats(hists,thr,n_f,split=False):
  '''
  Define a new binning for input histograms such that
  (bin error)/(bin value) < threshold for all bins
  in all histograms. Assumes all inputs have the same binning.
  New minimum bin width is N_force*(bin width).
  Inputs are: histogram, threshold, leftmost bin to check.
  Output is: list of bin edges.
  '''
  # TODO: First do some basic sanity checks

  nbins = hists[0].GetNbinsX()+1
  # Main body of the algorithm
  # Loops over histogram bins storing contents and sum of squares of errors
  # When it find a bin (or combination of bins) with err/val < threshold
  # it stores a new bin edge and resets the counter
  # Also requires that all bins have non-zero contents
  def GetBinningInRange(imin,imax,scanRight=True):
    val = np.zeros(len(hists))
    err2 = np.zeros(len(hists))
    ctr = 0
    if scanRight:
      rng = range(imin,imax)
      bins = [ hists[0].GetBinLowEdge(imin) ]
    else:
      rng = range(imax,imin,-1)
      bins = [ hists[0].GetBinLowEdge(imax+1) ]

    for ibin in rng:
      ctr += 1
      make_bin = True
      for ih in range(0,len(hists)):
        val[ih]  += hists[ih].GetBinContent(ibin)
        err2[ih] += hists[ih].GetBinError(ibin)*hists[ih].GetBinError(ibin)
        if (val[ih] < 1e-6 or np.sqrt(err2[ih])/val[ih] > thr or ctr < n_f):
          make_bin = False
      # if conditions are passed for all histograms then define a new bin edge
      if make_bin:
        if scanRight:
          bins.append( hists[0].GetBinLowEdge(ibin+1) )
        else:
          bins.append( hists[0].GetBinLowEdge(ibin) )

        val[...] = 0
        err2[...] = 0
        ctr = 0

    # if last bin would fail threshold, append it to second-to-last
    if scanRight:
      edge = hists[0].GetBinLowEdge(nbins)
    else:
      edge = hists[0].GetBinLowEdge(1)

    if edge not in bins:
      # in case histogram has only 1 bin left don't remove it
      if len(bins) == 1:
        bins.append(edge)
      else:
        bins[len(bins)-1] = edge

    return bins

  if split:
    # Find bin whose lower edge is zero and run rebinning separately
    # on positive and negative tails
    izero = 1
    for ibin in range(1,nbins):
      if hists[0].GetBinLowEdge(ibin) >= 0:
        izero = ibin
        break
    print(izero)
    bins_l = GetBinningInRange(1,izero-1,scanRight=False)
    bins_r = GetBinningInRange(izero,nbins)
    # lower edge of izero contained in both lists so remove it from
    # bins_r when merging
    return bins_l[::-1] + bins_r[1:]
  else:
    imin = 1 # 41 to remove negative Sd0
    return GetBinningInRange(imin,nbins)

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
  bins = []
  h_data = getKey(infile, MyConfig.GetDataHistName(region,var).Data() )
  if args.hist == 'data':
    bins = GetBinsByStats([h_data], args.stat, args.force, args.split)
  elif args.hist == 'MC':
    h_mc_vec = []
    for histname in MyConfig.GetMCHistNamesBySys('Nom',region,var):
      h_mc_vec.append(getKey(infile, histname.Data()))
    bins = GetBinsByStats(h_mc_vec, args.stat, args.force, args.split)
  else:
    for flav in ListOfFlavourPairs:
      if args.hist == flav.Data():
        h_mc = getKey(infile, MyConfig.GetMCHistName('Nom',region,flav,var).Data())
        bins = GetBinsByStats([h_mc], args.stat, args.force, args.split)
        break
  if not len(bins):
    print("ERROR: could not define binning in channel "+region.Data()+' '+var.Data())
    print("Asked for hist "+args.hist+". Must be data, MC, or a valid flavour-pair label")
    exit()

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
