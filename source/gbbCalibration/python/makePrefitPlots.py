import argparse
# numpy arrays are contiguous in memory (unlike python lists) and
# can be implicitly converted to c-style arrays for input to ROOT
import numpy as np
import os

parser = argparse.ArgumentParser(description='Make prefit histograms.')
parser.add_argument('-i','--input', help="ROOT file containing data and MC histograms")
parser.add_argument('-o','--outdir', help="Directory in which to store output plots")
parser.add_argument('--bins', type=str, default="trkjet",
    help="Use trkjet bins, fatjet bins or no bins (incl) [default: trkjet]")
parser.add_argument('--year', type=str, default="15+16",
    help="Year determines luminosity to normalize to [default: 15+16]")
parser.add_argument('--debug', action='store_true',
    help="Add extra printouts")
parser.add_argument('--plots', nargs='+',
    help="List of variables to plot. Some sets are predefined: templates, kinematics.")
parser.add_argument('--ext', type=str, default="pdf",
    help="File type for plots [default: pdf]")
args = parser.parse_args()

#import ROOT
#from PlotFunctions import *
#from TAxisFunctions import *
import ConfigFunctions as config
from ROOT import gROOT,gStyle,TFile,Double,SetOwnership
from ROOT import TCanvas,TPad,TLegend,TH2D,THStack,TLatex,TGraphAsymmErrors,TLine,TString
from math import log

gROOT.SetBatch(True)
gROOT.SetStyle('ATLAS')
gStyle.SetErrorX(0.5)

# set variables from args
###
if args.outdir:
  outdir = args.outdir + '/'
else:
  # default outdir name is input file name with '.root' replaced by '_plots'
  outdir = args.input[:-5] + '_plots/'
# make outdir if it doesn't exist
if not os.path.isdir(outdir):
  os.mkdir(outdir)

# Load c++ config library
MyConfig = config.LoadGlobalConfig()

# Expand predefined sets in list of variables, and remove duplicates
plotlist = []
for var in args.plots:
  if var == "templates":
    for tmpl in MyConfig.GetTemplateVariables():
      plotlist.append(tmpl.Data())
  elif var == "kinematics":
    for kine in ['fjpt','fjm','mjpt','nmjpt']:
      plotlist.append(kine)
  else:
    plotlist.append(var)
# Make sure there are no duplicates
plotvars = set(plotlist)

# Get list ofpython pt regions
if args.bins == 'trkjet':
  regions = MyConfig.GetDiTrkJetRegions()
elif args.bins == 'fatjet':
  regions = MyConfig.GetFatJetRegions()
elif args.bins == 'incl':
  regions = [ TString('Incl') ]
else:
  print('Unrecognized argument for --bins: '+args.bins)
  exit()

Lumi = config.GetLumi(args.year)
print('Lumi is {}'.format(Lumi))

plotText = 'Internal'
sqrtText = '#sqrt{s} = 13 TeV, %1.1f fb^{-1}' % (Lumi/1000.)
calibText = '#bf{g #rightarrow bb calibration}'

#-----------------------------------------------
def MakeDataMCRatioPlots(var,doChi2=False,setLogy=False):

  name = var
  tagText=''
  if '_2TAG' in var:
    tagText='double-b-tagged'

  FatJetFlavours = MyConfig.GetFlavourGroups()
  if not MyConfig.MergeFlavours():
    FatJetFlavours = MyConfig.GetFatJetFlavours()
  #colors = [1,601,856,435,407,800,417]
  def getColor(flav):
    if flav.count('B') > 1:
      return 601
    elif flav.count('B') > 0:
      return 856
    elif flav.count('C') > 1:
      return 435
    elif flav.count('C') > 0:
      return 407
    else:
      return 800
  def sortFlav(flav):
      return 16*flav.count('B')+4*flav.count('C')+flav.count('L')
  FlavList = []
  for flav in FatJetFlavours:
    FlavList.append(flav.Data())
  FlavList.sort(key = sortFlav, reverse = True)
  # Define and format objects for plotting
  canv = TCanvas('c','',800,800)
  pad1 = TPad('top','',0.,0.35,1.,1.)
  pad1.SetRightMargin(0.05)
  pad1.SetLeftMargin( 0.15)
  pad1.SetTopMargin(  0.06)
  pad1.SetBottomMargin(0.005)
  pad1.SetFillColor(0)
  if setLogy:
    pad1.SetLogy()
  pad1.Draw()
  pad2 = TPad('bot','',0.,0.,1.,0.35)
  pad2.SetRightMargin(0.05)
  pad2.SetLeftMargin( 0.15)
  pad2.SetTopMargin(  0.05)
  pad2.SetBottomMargin(0.3)
  pad2.SetFillColor(0)
  pad2.Draw()

  for region in regions:
    h_mcSum = None
    mcStack = THStack()

    leg = TLegend(0.74,0.38,0.94,0.88)
    leg.SetBorderSize(0)
    leg.SetNColumns(4)

    h_data = config.getKey(infile,MyConfig.GetDataHistName(region,var).Data())
    config.EnsureNoZeros(h_data)
    h_data.SetLineColor(1)
    h_data.SetMarkerStyle(20)
    leg.AddEntry(h_data,'Data','epl')

    for flav in FlavList:
      if (var[:2] == 'j2' or var[:3] == 'nmj') and 'xx' in flav:
        continue
      if var[:2] == 'j3' and flav[-1] == 'x':
        continue
      h_mc = config.getKey(infile,MyConfig.GetMCHistName('Nom',region,flav,var).Data())
      config.EnsureNoZeros(h_mc)

      #h_mc.SetName(flav.Data()) # Name goes on plot legend
      h_mc.SetFillColor(getColor(flav))
      h_mc.SetLineColor(1)
      h_mc.SetMarkerSize(0)
      leg.AddEntry(h_mc,flav,'f')
      mcStack.Add(h_mc)
      h_mcSum = config.AddHists(h_mcSum, h_mc, 'total_mc')

    chi2Text=''
    if doChi2:
      chi2 = h_data.Chi2Test(h_mcSum,"WW CHI2/NDF");
      #print(str(chi2))
      chi2Text='#chi^{{2}}/NDF = {:.2f}'.format(chi2)

    pad1.cd()
    h_data.Draw('EP')
    h_data.GetYaxis().SetLabelSize( 0.04 )
    h_data.GetYaxis().SetTitleSize( 0.04 )
    h_data.GetYaxis().SetTitleOffset(1.)
    if setLogy:
      h_data.GetYaxis().SetRangeUser(0.5*mcStack.GetMinimum(),
       pow(10, 1.2*abs(log(h_data.GetMaximum(),10)-log(mcStack.GetMinimum(),10)))
       *mcStack.GetMinimum())
    else:
      h_data.GetYaxis().SetRangeUser(0.1*h_data.GetMinimum(),1.2*h_data.GetMaximum())
    mcStack.Draw('hist same')
    h_data.Draw('EP same') # redraw data so it sits in front
    leg.Draw()
    text = TLatex()
    text.SetNDC()
    text.DrawLatex(0.5, 0.84, '#font[72]{ATLAS} #font[42]{%s}' % plotText)
    text.DrawLatex(0.5, 0.80, '#font[42]{#scale[0.8]{%s}}' % sqrtText)
    text.DrawLatex(0.5, 0.75, '#font[42]{#scale[0.8]{%s}}' % calibText)
    if tagText:
      text.DrawLatex(0.5, 0.70, '#font[42]{#scale[0.8]{%s}}' % tagText)

    pad2.cd()
    ratio = h_data.Clone('ratio')
    ratio.Divide(h_mcSum)
    ratio.SetLineColor(1)
    ratio.Draw('EP')
    ratio.GetYaxis().SetRangeUser(0.4,1.6)
    ratio.GetYaxis().SetTitle('Data/MC')
    line = TLine(ratio.GetBinLowEdge(1),1.,ratio.GetBinLowEdge(ratio.GetNbinsX()+1),1.)
    line.SetLineStyle(2)
    #line.SetLineWidth(1.5)
    line.Draw("same")

    ratio.GetXaxis().SetLabelSize( 0.08 )
    ratio.GetXaxis().SetTitleSize( 0.08 )
    ratio.GetYaxis().SetLabelSize( 0.08 )
    ratio.GetYaxis().SetTitleSize( 0.08 )
    ratio.GetYaxis().SetTitleOffset(0.5)
    if doChi2:
      text.DrawLatex(0.18, 0.86, '#font[42]{#scale[1.6]{#bf{%s}}}' % chi2Text)

    canv.SaveAs(outdir+region.Data()+'_'+name+'.'+args.ext)
    # finally, clear the canvas for the next histograms
    canv.Clear('D')

#-----------------------------------------------
def MakeMCOnlyPlots(var,flavset,tag=False,setLogy=False,shapeOnly=False):

  doMerge = False
  mergeDict = {
    "BB" : '"BBB","BBC","BBL","BBx","BCB","BLB","CBB","LBB"',
    "BC" : '"BCC","BCL","BCx","CBC","CBL","CBx","CCB"',
    "BL" : '"BLC","BLL","BLx","LBC","LBL","LBx","CLB","LCB","LLB"', # Bxx,
    "CC" : '"CCC","CCL","CCx","CLC","LCC"',
    "CL" : '"CLL","CLx","LCL","LCx","LLC"', # Cxx,
    "LL" : '"LLL","LLx"' #,Lxx
    }

  colorDict = {
    'BB' : 601,
    'BC' : 591,
    'BL' : 433,
    'CC' : 418,
    'CL' : 407,
    'LL' : 800
    }

  colors = [601,417,800,807]

  name = var
  tagText=''
  if '_2TAG' in var:
    tagText='double-b-tagged'
  if tag:
    name = name + '_' + tag

  # Define and format objects for plotting
  canv = TCanvas('c','',800,600)
  canv.SetRightMargin(0.05)
  canv.SetLeftMargin( 0.15)
  canv.SetTopMargin(  0.05)
  canv.SetBottomMargin(0.1)
  canv.SetFillColor(0)
  if setLogy:
    canv.SetLogy()

  leg_len = len(flavset)*0.05
  if flavset[0] == "all":
    doMerge = True
    flavListCPP = MyConfig.GetFatJetFlavours()
    flavset = []
    for flav in flavListCPP:
      flavset.append(flav.Data())
    leg_len = 6*0.05

  for region in regions:
    print(region)

    leg = TLegend(0.84,0.94-leg_len,0.94,0.94)
    leg.SetBorderSize(0)
    #leg.SetNColumns(4)

    hists = []
    if doMerge:
      hists = [None, None, None, None, None, None]

    for i,flav in enumerate(flavset):
      if (var[:2] == 'j2' or var[:3] == 'nmj') and 'xx' in flav:
        continue
      if var[:2] == 'j3' and flav[-1] == 'x':
        continue
      h_mc = config.getKey(infile,MyConfig.GetMCHistName('Nom',region,flav,var).Data())
      h_mc.SetDirectory(0)
      config.EnsureNoZeros(h_mc)

      #h_mc.SetName(flav.Data()) # Name goes on plot legend
      h_mc.SetFillColor(0)
      h_mc.SetLineWidth(2)
      h_mc.SetMarkerSize(0)
      if not doMerge:
        h_mc.SetLineColor(colors[i])
        hists.append(h_mc)
        leg.AddEntry(h_mc,flav,'l')
      else:
        for j,key in enumerate(mergeDict):
          if flav in mergeDict[key]:
            h_mc.SetLineColor(colorDict[key])
            if hists[j]:
              hists[j].Add(h_mc)
            else:
              hists[j] = h_mc
              leg.AddEntry(h_mc,key,'l')

    if shapeOnly:
      hists[0].Scale(1/hists[0].Integral());
      hists[0].GetYaxis().SetTitle('Normalized to Unity')
    maxval = hists[0].GetMaximum()
    hists[0].Draw("hist")
    for hist in hists:
      if shapeOnly:
        hist.Scale(1/hist.Integral());
      hist.Draw("hist same")
      if hist.GetMaximum() > maxval:
        maxval = hist.GetMaximum()
    if setLogy:
      hists[0].GetYaxis().SetRangeUser(1e-4*maxval,4*maxval)
    else:
      hists[0].GetYaxis().SetRangeUser(0.,1.4*maxval)
    leg.Draw()
    text = TLatex()
    text.SetNDC()
    text.DrawLatex(0.55, 0.88, '#font[72]{ATLAS} #font[42]{%s}' % plotText)
    text.DrawLatex(0.55, 0.84, '#font[42]{#scale[0.8]{%s}}' % sqrtText)
    text.DrawLatex(0.55, 0.79, '#font[42]{#scale[0.8]{%s}}' % calibText)
    if tagText:
      text.DrawLatex(0.55, 0.75, '#font[42]{#scale[0.8]{%s}}' % tagText)

    canv.SaveAs(outdir+region.Data()+'_'+name+'.'+args.ext)
    # finally, clear the canvas for the next histograms
    canv.Clear('D')


#-----------------------------------------------
# Main function
#-----------------------------------------------
# Get input histograms
infile = TFile(args.input)

## For future reference
## Plots comparing Xbb scores for different (unmerged) flavor combinations
#MakeMCOnlyPlots('SubjetBScore_f0', ['BBB','BBC','BBL','BBx'], tag='BB', setLogy=False)
#MakeMCOnlyPlots('SubjetBScore_f0', ['BCB','BCC','BCL','BCx'], tag='BC', setLogy=False)
#MakeMCOnlyPlots('SubjetBScore_f0', ['BLB','BLC','BLL','BLx'], tag='BL', setLogy=False)
#MakeMCOnlyPlots('SubjetBScore_f0', ['CBB','CBC','CBL','CBx'], tag='CB', setLogy=False)
#MakeMCOnlyPlots('SubjetBScore_f0', ['CCB','CCC','CCL','CCx'], tag='CC', setLogy=False)
#MakeMCOnlyPlots('SubjetBScore_f0', ['CLB','CLC','CLL','CLx'], tag='CL', setLogy=False)
#MakeMCOnlyPlots('SubjetBScore_f0', ['LBB','LBC','LBL','LBx'], tag='LB', setLogy=False)
#MakeMCOnlyPlots('SubjetBScore_f0', ['LCB','LCC','LCL','LCx'], tag='LC', setLogy=False)
#MakeMCOnlyPlots('SubjetBScore_f0', ['LLB','LLC','LLL','LLx'], tag='LL', setLogy=False)
#MakeMCOnlyPlots('SubjetBScore_f0', ['BBx','BCx','BLx','Bxx'], tag='B',  setLogy=False)
#MakeMCOnlyPlots('SubjetBScore_f0', ['CBx','CCx','CLx','Cxx'], tag='C',  setLogy=False)
#MakeMCOnlyPlots('SubjetBScore_f0', ['LBx','LCx','LLx','Lxx'], tag='L',  setLogy=False)

for var in plotvars:
  # make pt, mass and Sd0 plots log scale
  logy=False
  if any(x in var for x in ['Sd0','jpt','jm']):
    logy=True
  MakeDataMCRatioPlots(var, doChi2=True, setLogy=logy)
