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
args = parser.parse_args()

#import ROOT
from PlotFunctions import *
from TAxisFunctions import *
import ConfigFunctions as config
from ROOT import gROOT,gStyle,TFile,Double
from ROOT import TCanvas,TPad,TLegend,TH2D,THStack,TLatex,TGraphAsymmErrors,TLine,TString

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
plotvars = {}
for var in args.plots:
  if var == "templates":
    for tmpl in MyConfig.GetTemplateVariables():
      plotvars.add(tmpl.Data())
  elif var == "kinematics":
    for kine in ['fjpt','fjm','mjpt','nmjpt']:
      plotvars.add(kine)
  else:
    plotvars.add(var)

# Get list of pt regions
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
def EnsureNoZeros(hist):
  minval = 999.
  for i in range(1,hist.GetNbinsX()+1):
    binval = hist.GetBinContent(i)
    if binval == 0:
      hist.SetBinContent(i,1e-6)
    elif binval < minval:
      minval = binval
  hist.SetMinimum(minval)

#-----------------------------------------------
def MakeDataMCRatioPlots(var,doChi2=False,setLogy=False):

  name = var
  tagText=''
  if '_2TAG' in var:
    tagText='double-b-tagged'

  colors = [1,601,856,435,407,800,417]
  FlavList = MyConfig.GetFlavourPairs()
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

    leg = TLegend(0.84,0.58,0.94,0.88)
    leg.SetBorderSize(0)

    h_data = config.getKey(infile,MyConfig.GetDataHistName(region,var).Data())
    EnsureNoZeros(h_data)
    h_data.SetLineColor(1)
    h_data.SetMarkerStyle(20)
    leg.AddEntry(h_data,'Data','epl')

    for j,flav in enumerate(FlavList):
      h_mc = config.getKey(infile,MyConfig.GetMCHistName('Nom',region,flav,var).Data())
      EnsureNoZeros(h_mc)

      #h_mc.SetName(flav.Data()) # Name goes on plot legend
      h_mc.SetFillColor(colors[j+1])
      h_mc.SetLineColor(1)
      h_mc.SetMarkerSize(0)
      leg.AddEntry(h_mc,flav.Data(),'f')
      mcStack.Add(h_mc)
      if not h_mcSum:
        h_mcSum = h_mc.Clone('total_mc')
      else:
        h_mcSum.Add(h_mc)

    chi2Text=''
    if doChi2:
      chi2 = h_data.Chi2Test(h_mcSum,"UW CHI2/NDF");
      #print(str(chi2))
      chi2Text='#chi^{{2}}/NDF = {:.2f}'.format(chi2)

    pad1.cd()
    h_data.Draw('EP')
    h_data.GetYaxis().SetLabelSize( 0.04 )
    h_data.GetYaxis().SetTitleSize( 0.04 )
    h_data.GetYaxis().SetTitleOffset(1.)
    if setLogy:
      h_data.GetYaxis().SetRangeUser(0.1*h_data.GetMinimum(),2*h_data.GetMaximum())
    else:
      h_data.GetYaxis().SetRangeUser(0.1*h_data.GetMinimum(),1.2*h_data.GetMaximum())
    mcStack.Draw('hist same')
    h_data.Draw('EP same') # redraw data so it sits in front
    leg.Draw()
    text = TLatex()
    text.SetNDC()
    text.DrawLatex(0.6, 0.84, '#font[72]{ATLAS} #font[42]{%s}' % plotText)
    text.DrawLatex(0.6, 0.80, '#font[42]{#scale[0.8]{%s}}' % sqrtText)
    text.DrawLatex(0.6, 0.75, '#font[42]{#scale[0.8]{%s}}' % calibText)
    if tagText:
      text.DrawLatex(0.6, 0.70, '#font[42]{#scale[0.8]{%s}}' % tagText)

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

    canv.SaveAs(outdir+region.Data()+'_'+name+'.pdf')
    # finally, clear the canvas for the next histograms
    canv.Clear('D')

#-----------------------------------------------
def MakeSystematicRatioPlots(var,doChi2=False,setLogy=False):

  name = var
  tagText=''
  if '_2TAG' in var:
    tagText='double-b-tagged'

  colors = [1,600,632]
  FlavList = MyConfig.GetFlavourPairs()
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
    h_nomMC = None
    h_upMC = None
    h_dnMC = None

    leg = TLegend(0.84,0.58,0.94,0.88)
    leg.SetBorderSize(0)

    for j,flav in enumerate(FlavList):
      h_mc = config.getKey(infile,MyConfig.GetMCHistName('Nom',region,flav,var).Data())
      EnsureNoZeros(h_mc)

      if not h_mcSum:
        h_mcSum = h_mc.Clone('total_mc')
      else:
        h_mcSum.Add(h_mc)

    for syst in ListOfTwoSidedSystematics:
      for j,flav in enumerate(FlavList):
        h_mc = config.getKey(infile,MyConfig.GetMCHistName(syst.Data(),region,flav,var).Data())
        EnsureNoZeros(h_mc)

        if not h_mcSum:
          h_mcSum = h_mc.Clone('total_mc')
        else:
          h_mcSum.Add(h_mc)

    h_data = config.getKey(infile,MyConfig.GetDataHistName(region,var).Data())
    EnsureNoZeros(h_data)
    h_data.SetLineColor(1)
    h_data.SetMarkerStyle(20)
    leg.AddEntry(h_data,'Data','epl')

    for j,flav in enumerate(FlavList):
      h_mc = config.getKey(infile,MyConfig.GetMCHistName('Nom',region,flav,var).Data())
      EnsureNoZeros(h_mc)

      #h_mc.SetName(flav.Data()) # Name goes on plot legend
      h_mc.SetFillColor(colors[j+1])
      h_mc.SetLineColor(1)
      h_mc.SetMarkerSize(0)
      leg.AddEntry(h_mc,flav.Data(),'f')
      mcStack.Add(h_mc)
      if not h_mcSum:
        h_mcSum = h_mc.Clone('total_mc')
      else:
        h_mcSum.Add(h_mc)

    chi2Text=''
    if doChi2:
      chi2 = h_data.Chi2Test(h_mcSum,"UW CHI2/NDF");
      #print(str(chi2))
      chi2Text='#chi^{{2}}/NDF = {:.2f}'.format(chi2)

    pad1.cd()
    h_data.Draw('EP')
    h_data.GetYaxis().SetLabelSize( 0.04 )
    h_data.GetYaxis().SetTitleSize( 0.04 )
    h_data.GetYaxis().SetTitleOffset(1.)
    if setLogy:
      h_data.GetYaxis().SetRangeUser(0.1*h_data.GetMinimum(),2*h_data.GetMaximum())
    else:
      h_data.GetYaxis().SetRangeUser(0.1*h_data.GetMinimum(),1.2*h_data.GetMaximum())
    mcStack.Draw('hist same')
    h_data.Draw('EP same') # redraw data so it sits in front
    leg.Draw()
    text = TLatex()
    text.SetNDC()
    text.DrawLatex(0.6, 0.84, '#font[72]{ATLAS} #font[42]{%s}' % plotText)
    text.DrawLatex(0.6, 0.80, '#font[42]{#scale[0.8]{%s}}' % sqrtText)
    text.DrawLatex(0.6, 0.75, '#font[42]{#scale[0.8]{%s}}' % calibText)
    if tagText:
      text.DrawLatex(0.6, 0.70, '#font[42]{#scale[0.8]{%s}}' % tagText)

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

    canv.SaveAs(outdir+region.Data()+'_'+name+'.pdf')
    # finally, clear the canvas for the next histograms
    canv.Clear('D')

#-----------------------------------------------
# Main function
#-----------------------------------------------
# Get input histograms
infile = TFile(args.input)

for var in plotvars:
  # make pt, mass and Sd0 plots log scale
  logy=False
  if any(x in var for x in ['Sd0','jpt','jm']):
    logy=True
  MakeDataMCRatioPlots(var, doChi2=True, setLogy=logy)
