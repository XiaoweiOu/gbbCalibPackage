import argparse
# numpy arrays are contiguous in memory (unlike python lists) and
# can be implicitly converted to c-style arrays for input to ROOT
import numpy as np
import os

parser = argparse.ArgumentParser(description='Make prefit histograms.')
parser.add_argument('-i','--input', help="ROOT file containing data and MC histograms")
parser.add_argument('-o','--outdir', help="Directory in which to store output plots")
parser.add_argument('--thr', type=float, default=0.05,
    help="Only variations that change yield more than threshold are plotted [default: 5%]")
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

FlavList = MyConfig.GetFlavourPairs()

ListOfSystematics = MyConfig.GetSystematics()
#ListOfSd0Systematics = MyConfig.GetSystematics_Sd0()
#ListOfWeightVariations = MyConfig.GetSystematics_WeightVar()
# Reorganize systematics lists into 1-sided/2-sided
ListOf2SidedSysts = []
ListOf1SidedSysts = []
for sys in ListOfSystematics:
  twoSided = False
  if "__1up" in sys.Data():
    sysName = sys.Data()[:-5]
    for sys2 in ListOfSystematics:
      if sysName in sys2.Data() and "__1down" in sys2.Data():
        ListOf2SidedSysts.append(sysName)
        twoSided = True
    if not twoSided:
      ListOf1SidedSysts.append(sysName+"__1up")
  else:
    if "__1down" not in sys.Data():
      ListOf1SidedSysts.append(sys.Data())

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
# Helper Functions
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

def GetMCSum(var, region, syst):
  h_mcSum = None
  for j,flav in enumerate(FlavList):
    h_mc = config.getKey(infile,MyConfig.GetMCHistName(syst,region,flav,var).Data())
    EnsureNoZeros(h_mc)

    if not h_mcSum:
      h_mcSum = h_mc.Clone('total_'+region+'_'+var+'_'+syst)
    else:
      h_mcSum.Add(h_mc)
  return h_mcSum

#-----------------------------------------------
def MakeSystematicRatioPlots(var,syst,twoSidedSyst=True,setLogy=False):

  name = var
  tagText=''
  if '_2TAG' in var:
    tagText='double-b-tagged'

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
    leg = TLegend(0.64,0.55,0.94,0.7)
    leg.SetBorderSize(0)

    h_nomSum = GetMCSum(var, region.Data(), 'Nom')
    h_nomSum.SetLineColor(1); h_nomSum.SetLineWidth(2); h_nomSum.SetFillColor(0);
    leg.AddEntry(h_nomSum,'Nominal','l')
    if twoSidedSyst:
      h_upSum = GetMCSum(var, region.Data(), syst+'__1up')
      leg.AddEntry(h_upSum,syst+' up','l')
      h_dnSum = GetMCSum(var, region.Data(), syst+'__1down')
      leg.AddEntry(h_dnSum,syst+' down','l')
    else:
      h_upSum = GetMCSum(var, region.Data(), syst)
      leg.AddEntry(h_upSum,syst,'l')
      h_dnSum = None
    h_upSum.SetLineColor(2); h_upSum.SetLineWidth(2); h_upSum.SetFillColor(0);
    if h_dnSum:
      h_dnSum.SetLineColor(4); h_dnSum.SetLineWidth(2); h_dnSum.SetFillColor(0);

    pad1.cd()
    h_nomSum.Draw('hist e')
    h_nomSum.GetYaxis().SetLabelSize( 0.04 )
    h_nomSum.GetYaxis().SetTitleSize( 0.04 )
    h_nomSum.GetYaxis().SetTitleOffset(1.)
    if setLogy:
      h_nomSum.GetYaxis().SetRangeUser(0.1*h_nomSum.GetMinimum(),2*h_nomSum.GetMaximum())
    else:
      h_nomSum.GetYaxis().SetRangeUser(0.1*h_nomSum.GetMinimum(),1.2*h_nomSum.GetMaximum())
    h_upSum.Draw('hist same')
    if h_dnSum:
      h_dnSum.Draw('hist same')

    leg.Draw()
    text = TLatex()
    text.SetNDC()
    text.DrawLatex(0.69, 0.84, '#font[72]{ATLAS} #font[42]{%s}' % plotText)
    text.DrawLatex(0.69, 0.80, '#font[42]{#scale[0.8]{%s}}' % sqrtText)
    text.DrawLatex(0.69, 0.75, '#font[42]{#scale[0.8]{%s}}' % calibText)
    if tagText:
      text.DrawLatex(0.69, 0.70, '#font[42]{#scale[0.8]{%s}}' % tagText)

    pad2.cd()
    upRatio = h_upSum.Clone('ratio_up')
    upRatio.Divide(h_nomSum)
    upRatio.SetLineColor(2)
    upRatio.Draw('hist')
    if h_dnSum:
      dnRatio = h_dnSum.Clone('ratio_dn')
      dnRatio.Divide(h_nomSum)
      dnRatio.SetLineColor(4)
      dnRatio.Draw('hist same')
    line = TLine(upRatio.GetBinLowEdge(1),1.,upRatio.GetBinLowEdge(upRatio.GetNbinsX()+1),1.)
    line.SetLineStyle(2)
    line.Draw("same")

    upRatio.GetXaxis().SetLabelSize( 0.08 )
    upRatio.GetXaxis().SetTitleSize( 0.08 )
    upRatio.GetYaxis().SetLabelSize( 0.08 )
    upRatio.GetYaxis().SetTitleSize( 0.08 )
    upRatio.GetYaxis().SetTitleOffset(0.5)
    #upRatio.GetYaxis().SetRangeUser(0.99,1.1)
    upRatio.GetYaxis().SetTitle('Syst/Nom')

    canv.SaveAs(outdir+region.Data()+'_'+var+'_'+syst+'.pdf')
    # finally, clear the canvas for the next histograms
    canv.Clear('D')

#-----------------------------------------------
# Main function
#-----------------------------------------------
# Get input histograms
infile = TFile(args.input)

for syst in ListOf2SidedSysts:
  print(syst)
  makePlots = False
  for region in regions:
    for tmpl in MyConfig.GetTemplateVariables():
      h_nomSum = GetMCSum(tmpl.Data(), region.Data(), 'Nom')
      h_upSum = GetMCSum(tmpl.Data(), region.Data(), syst+'__1up')
      h_dnSum = GetMCSum(tmpl.Data(), region.Data(), syst+'__1down')

      nom_yield = h_nomSum.Integral(0,h_nomSum.GetNbinsX()+1)
      up_yield = h_upSum.Integral(0,h_upSum.GetNbinsX()+1)
      dn_yield = h_dnSum.Integral(0,h_dnSum.GetNbinsX()+1)
      if (abs(1- up_yield/nom_yield) > args.thr) \
         or (abs(1- dn_yield/nom_yield) > args.thr):
        print('{} up ratio: {}, down ratio: {}'.format(region.Data(),
               up_yield/nom_yield, dn_yield/nom_yield))
        makePlots = True
        break;
    if makePlots:
      break;
  if makePlots:
    for var in plotvars:
      # make pt, mass and Sd0 plots log scale
      logy=False
      #if any(x in var for x in ['Sd0','jpt','jm']):
      #  logy=True
      MakeSystematicRatioPlots(var, syst, twoSidedSyst=True, setLogy=logy)

for syst in ListOf1SidedSysts:
  print(syst)
  makePlots = False
  for region in regions:
    for tmpl in MyConfig.GetTemplateVariables():
      h_nomSum = GetMCSum(tmpl.Data(), region.Data(), 'Nom')
      h_upSum = GetMCSum(tmpl.Data(), region.Data(), syst)

      nom_yield = h_nomSum.Integral(0,h_nomSum.GetNbinsX()+1)
      up_yield = h_upSum.Integral(0,h_upSum.GetNbinsX()+1)
      if (abs(1- up_yield/nom_yield) > args.thr):
        print('{} ratio: {}'.format(region.Data(), up_yield/nom_yield))
        makePlots = True
        break;
    if makePlots:
      break;
  if makePlots:
    for var in plotvars:
      # make pt, mass and Sd0 plots log scale
      logy=False
      #if any(x in var for x in ['Sd0','jpt','jm']):
      #  logy=True
      MakeSystematicRatioPlots(var, syst, twoSidedSyst=False, setLogy=logy)
