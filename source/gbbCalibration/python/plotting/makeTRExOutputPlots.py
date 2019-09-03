#import ROOT
from PlotFunctions import *
from TAxisFunctions import *
import ConfigFunctions as config
import argparse
# numpy arrays are contiguous in memory (unlike python lists) and
# can be implicitly converted to c-style arrays for input to ROOT
import numpy as np
import os

#ROOT.gROOT.SetBatch(True)
from ROOT import gROOT,gStyle,TCanvas,TH2D,TLatex,TGraphAsymmErrors,TLine

#ROOT.gStyle.SetOptStat(0)
#SetupStyle()
gROOT.SetStyle('ATLAS')

parser = argparse.ArgumentParser(description='Make fit result histograms.')
parser.add_argument('input', help="Folder containing TRExFitter results")
parser.add_argument('--bins', type=str, default="trkjet",
    help="Use trkjet bins, fatjet bins or no bins (incl) [default: trkjet]")
parser.add_argument('--year', type=str, default="2015+2016",
    help="Year determines luminosity to normalize to [default: 2015+2016]")
parser.add_argument('--debug', action='store_true',
    help="Add extra printouts")
#TODO: add flags for which types of plots to make
parser.add_argument('--plots', nargs='+',
    help="List of plots to make. Options include 'SF','NF','CorrMat',...")
args = parser.parse_args()

# set variables from args
outdir = args.input+'/Plots/'
if not os.path.isdir(outdir):
  os.mkdir(outdir)

MyConfig = config.LoadGlobalConfig()
if args.bins == 'trkjet':
  regions = MyConfig.GetTrkJetRegions()
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
# FIXME: rightarrow not being drawn, don't know why
calibText = '#bf{g #rightarrow bb calibration}'

#===============================================
class FitResult():
  def __init__(self,nuisPars,corrMat,nll):
    self.nuisPars = nuisPars
    self.corrMat = corrMat
    self.nll = nll

  def GetPar(self,name):
    return self.nuisPars[name][0]
  def GetErr(self,name):
    # TRExFitter errors are symmetrized (I think)
    return self.nuisPars[name][1] #- nuisPars[name][2])/2.

#-----------------------------------------------
def ReadFitResults():

  if args.debug:
    print("####################")
    print("Reading Fit Results")
    print("####################")
  fitResults = {}
  for region in regions:
    # Read in fit from TRExFitter output
    # TODO: add option for Asimov fit
    path = "{0}/TRexFit/Fit_{1}_Data/Fits/Fit_{1}_Data.txt".format(args.input,region.Data())

    nuisPars = {}
    nll = 0
    with open(path) as f:
      for line in f:
        if line.isspace():
          continue
        elif 'NUISANCE_PARAMETERS' in line:
          if args.debug:
            print("--------------------")
            print("Reading Nuisance Parameters...")
            print("--------------------")
          readingNP = True
          continue
        elif 'CORRELATION_MATRIX' in line:
          if args.debug:
            print("--------------------")
            print("Reading Correlation Matrix...")
            print("--------------------")
          readingNP = False
          readingCM = True
          Nsyst = 0
          row = 0
          continue
        elif 'NLL' in line:
          if args.debug:
            print("--------------------")
            print("Reading Negative Log-Likelihood (NLL) value...")
            print("--------------------")
          readingNP = False
          readingCM = False
          readingNLL = True
          continue

        #while(input.find("\\")!=string::npos) input = input.replace(input.find("\\"),1,"");
        tokens = line.split()
        if readingNP:
          name = tokens[0]
          name.replace('alpha_','')
          name.replace('gamma_','')
          nuisPars[name] = [ float(x) for x in tokens[1:4] ]
          if args.debug:
            print('  '+name+': '+str(tokens[1:4]))
        elif readingCM:
          # first line of correlation matrix block has N rows/columns
          if Nsyst == 0:
            Nsyst = int(tokens[0])
            matrix = np.zeros( (Nsyst,Nsyst) )
            continue
          # matrix is inverted
          else:
            for col, corr in enumerate(tokens):
              matrix[Nsyst-col-1][row] = corr
            row += 1
        elif readingNLL:
          nll = float(tokens[0])
          if args.debug:
            print('  '+nll)

    fitResults[region.Data()] = FitResult(nuisPars,matrix,nll)
  if args.debug:
    print("####################")
    print("Done")
    print("####################")
  return fitResults

#-----------------------------------------------
def makeTrkJetLabels(bins,jetName):
  labels = []
  labels.append("p_{T}(%s) < %i GeV" % (jetName,int(bins[0])) )
  for i in range(0,len(bins)-1):
    labels.append("p_{T}(%s) #splitline{ >%i GeV}{< %i GeV}"
                    % (jetName,int(bins[i]),int(bins[i+1])) )
  labels.append("p_{T}(%s) > %i GeV" % (jetName,int(bins[len(bins)-1])) )
  return labels

#-----------------------------------------------
def Make2DFitPlot(fitResults, nuisPar):
  bin_vals = np.zeros(len(regions))
  bin_errs = np.zeros(len(regions))
  for i,region in enumerate(regions):
    result = fitResults[region.Data()]
    bin_vals[i] = result.GetPar(nuisPar)
    bin_errs[i] = result.GetErr(nuisPar)

  # Set up bins and labels for 2D plots
  mj_labels = makeTrkJetLabels(MyConfig.GetMuonJetPtBins(),"#mu-jet")
  nmj_labels = makeTrkJetLabels(MyConfig.GetNonMuJetPtBins(),"non-#mu-jet")

  # Create canvas
  canv = TCanvas('canv','',800,600)
  canv.SetTopMargin(0.05)
  canv.SetLeftMargin(0.28)
  canv.SetRightMargin(0.05)
  # FIXME: ticks aren't displaying properly. turning them off for now
  canv.SetTickx(0);
  canv.SetTicky(0);
  #canv.cd();

  # Add extra bin of blank space on y-axis for header
  hist = TH2D("hist","",len(mj_labels),0,len(mj_labels)+1,len(nmj_labels)+1,0,len(nmj_labels)+2)
  hist_err_up = TH2D("hist_err_up","",len(mj_labels),0,len(mj_labels)+1,len(nmj_labels)+1,0,len(nmj_labels)+2)
  hist_err_down = TH2D("hist_err_down","",len(mj_labels),0,len(mj_labels)+1,len(nmj_labels)+1,0,len(nmj_labels)+2)

  for i in range(0,len(mj_labels)): # loop over mu jet pt bins
    for j in range(0,len(nmj_labels)): # loop over non-mu jet pt bins
      n_region = j + i*len(nmj_labels)
      hist.SetBinContent(i+1,j+1,bin_vals[n_region])
      hist_err_up.SetBinContent(i+1,j+1,bin_errs[n_region])
      hist_err_down.SetBinContent(i+1,j+1,bin_errs[n_region])

  for i in range(0,len(mj_labels)):
    hist.GetXaxis().SetBinLabel(i+1,mj_labels[i])
  for i in range(0,len(nmj_labels)):
    hist.GetYaxis().SetBinLabel(i+1,nmj_labels[i])

  hist.LabelsOption("u","Y")
  hist.SetLabelSize(0.05,"X")
  hist.SetLabelSize(0.05,"Y")

  canv.cd()
  hist.SetMarkerSize(2)
  gStyle.SetPaintTextFormat("4.2f")
  hist.Draw("COL TEXT")
  hist_err_up.SetBarOffset(0.23)
  hist_err_up.SetMarkerSize(1.3)
  hist_err_up.Draw("TEXT SAME")
  hist_err_down.SetBarOffset(-0.23)
  hist_err_down.SetMarkerSize(1.3)
  hist_err_down.Draw("TEXT SAME")

  if hist.GetMaximum() > 1.:
    hist.GetZaxis().SetRangeUser(0.,2.)
  else:
    hist.GetZaxis().SetRangeUser(0.,1.)

  #Add plot labels
  text = TLatex()
  text.SetNDC()
  text.DrawLatex(0.675, 0.90, '#font[72]{ATLAS} #font[42]{%s}' % plotText)
  text.DrawLatex(0.675, 0.86, '#font[42]{#scale[0.8]{%s}}' % sqrtText)
  text.DrawLatex(0.675, 0.81, '#font[42]{#scale[0.8]{%s}}' % calibText)

  canv.SaveAs(outdir+'2D'+nuisPar+'.pdf')
  delete canv

#-----------------------------------------------
def Make1DFitPlot(fitResults, nuisPar):
  bin_vals = np.zeros(len(regions))
  bin_yerr = np.zeros(len(regions))
  for i,region in enumerate(regions):
    result = fitResults[region.Data()]
    bin_vals[i] = result.GetPar(nuisPar)
    bin_yerr[i] = result.GetErr(nuisPar)

  # Set up bins and labels for 1D plots
  fj_bins = MyConfig.GetFatJetPtBins()
  N = len(fj_bins)+1
  bin_x = np.zeros(N)
  bin_xerr = np.zeros(N)

  #FIXME: will need to change this if/when we calibrate lower pT
  bin_xerr[0] = 0.5*(fj_bins[0]-500)
  bin_x[0] = 500+bin_xerr[0]
  for i in range(0,len(fj_bins)-1):
    bin_xerr[i+1] = 0.5*(fj_bins[i+1]-fj_bins[i])
    bin_x[i+1] = fj_bins[i]+bin_xerr[i+1]

  # Create canvas
  canv = TCanvas('canv','',800,600)

  # Make TGraph with error bands
  gr = TGraphAsymmErrors(N,bin_x[0],bin_vals[0],bin_xerr[0],bin_xerr[0],bin_yerr[0],bin_yerr[0])
  gr.SetMarkerSize(0)
  gr.SetFillColor(kGreen+3)
  gr.SetFillStyle(3001)
  gr.SetTitle("")

  gr.GetXaxis().SetTitle("Large-R Jet p_{T} [GeV]")
  gr.GetYaxis().SetTitle(nuisPar)

  gr.GetXaxis().SetTitleSize(0.04)
  gr.GetYaxis().SetTitleSize(0.04)

  #gr.GetYaxis().SetRangeUser(0.6,1.6)
  #if(plot_type.EqualTo("Eff")) gr.GetYaxis().SetRangeUser(0.,1.6)

  # TODO: get stat error from TRExFitter
  #SF_data_stat = ROOT.TGraphErrors(N,&(bin_x[0]),&(val_y[0]),&(bin_xerr[0]),&(stat_y_err[0]))
  #SF_data_stat.SetMarkerStyle(20)
  #SF_data_stat.SetTitle("")

  #if(plot_type.EqualTo("Eff")){
  #  EFF_mc_stat = ROOT.TGraphErrors(N,&(bin_x[0]),&(val_ymc[0]),&(bin_xerr[0]),&(stat_ymc_err[0]))
  #  EFF_mc_stat.SetMarkerStyle(25)
  #  EFF_mc_stat.SetTitle("")
  #}

  canv.cd()
  gr.Draw("a2")
  #SF_data_stat.Draw("p")
  #if(plot_type.EqualTo("Eff")) EFF_mc_stat.Draw("ep")

  ##prepare Legend
  #leg = ROOT.TLegend(0.5,0.5,0.88,0.65);
  #leg.SetBorderSize(0)
  #leg.SetFillStyle(0)

  #if plot_type == "SF":
  #  leg.AddEntry(SF_band_sys,"bb-tagging SF (stat.+sys.)","f")
  #  leg.AddEntry(SF_data_stat,"bb-tagging SF (stat.)","lep")
  #elif plot_type == "Eff":
  #  leg.AddEntry(SF_band_sys,"bb-tagging data eff. (stat.+sys.)","f")
  #  leg.AddEntry(SF_data_stat,"bb-tagging data eff. (stat.)","lep")
  #  leg.AddEntry(EFF_mc_stat,"bb-tagging mc eff. (stat.)","lep")

  #leg.SetEntrySeparation(0.15)
  #leg.Draw()

  #Add plot labels
  text = TLatex()
  text.SetNDC()
  text.DrawLatex(0.675, 0.90, '#font[72]{ATLAS} #font[42]{%s}' % plotText)
  text.DrawLatex(0.675, 0.86, '#font[42]{#scale[0.8]{%s}}' % sqrtText)
  text.DrawLatex(0.675, 0.81, '#font[42]{#scale[0.8]{%s}}' % calibText)

  if 'ScaleFactor' in nuisPar:
    line = TLine(fj_bins[0],1.,fj_bins[len(fj_bins)-1],1.)
    line.SetLineStyle(2)
    line.SetLineWidth(1.5)
    line.Draw("same")

  canv.SaveAs(outdir+nuisPar+'.pdf')
  delete canv

##-----------------------------------------------
#def MakeRatioPlot(dataHist,mcHists,uncertHists=None,doChi2=False,setLogy=False):
#
#  tagText=''
#  if 'POSTTAG' in dataHist.GetName():
#    tagText='double-b-tagged'
#
#  colors = [ROOT.kBlack, ROOT.kBlue+1, ROOT.kAzure-4,
#            ROOT.kCyan+3, ROOT.kGreen-9, ROOT.kOrange,
#            ROOT.kGreen+1]
#  canv = RatioCanvas('c',"",800,800,0.3,'pE1',doLogy=setLogy)
#  #prepare legend
#  #leg = ROOT.TLegend(0.68,0.4,0.88,0.75);
#  #leg.SetBorderSize(0)
#  #leg.SetFillStyle(0)
#
#  is_stack = issubclass(type(mcHists),ROOT.THStack)
#  if is_stack:
#    mcSumHist = None
#    for hist in mcHists.GetHists():
#     hist.SetName(myconfig.GetFlavour(hist.GetName()))
#      if not mcSumHist:
#        mcSumHist = hist
#      else:
#        mcSumHist.Add(hist)
#  else:
#    mcHists.SetName("dijet MC")
#    mcSumHist = mcHists
#
#  dataHist.SetName("Data")
#  chi2Text=''
#  if doChi2:
#    chi2 = dataHist.Chi2Test(mcSumHist,"UW CHI2/NDF");
#    #print(str(chi2))
#    chi2Text='#scale[0.6]{{#chi^{{2}}/NDF = {:.2f}}}'.format(chi2)
#
#  AddHistogram(canv,dataHist,'e')
#  AddHistogram(canv,mcHists,'histe')
#  if uncertHists:
#    AddErrorGraph(canv,uncertHists)
#    AddRatioErrorGraph(canv,uncertHists,mcSumHist)
#  AddRatio(canv,dataHist,mcSumHist)
#  SetAxisLabels(canv,dataHist.GetXaxis().GetTitle(),dataHist.GetYaxis().GetTitle())
#  FullFormatCanvasDefault(canv,lumi=Lumi/1000,additionaltext1=tagText,additionaltext2=chi2Text)
#  SetColors(canv,colors)
#  SetYaxisRangesRatio(canv,0.5,1.5)
#  return canv

#-----------------------------------------------
# Main function
#-----------------------------------------------
results = ReadFitResults()

if args.bins == 'trkjet':
  Make2DFitPlot(results,'ScaleFactor')
  for flav in MyConfig.GetFlavourPairs():
    Make2DFitPlot(results,flav.Data())
elif args.bins == 'fatjet':
  Make1DFitPlot(results,'ScaleFactor')
  for flav in MyConfig.GetFlavourPairs():
    Make1DFitPlot(results,flav.Data())
