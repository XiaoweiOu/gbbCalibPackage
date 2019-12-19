import argparse
# numpy arrays are contiguous in memory (unlike python lists) and
# can be implicitly converted to c-style arrays for input to ROOT
import numpy as np
import os

parser = argparse.ArgumentParser(description='Make fit result histograms.')
parser.add_argument('input', help="Folder containing TRExFitter results")
parser.add_argument('--bins', type=str, default="trkjet",
    help="Use trkjet bins, fatjet bins or no bins (incl) [default: trkjet]")
parser.add_argument('--year', type=str, default="2015+2016",
    help="Year determines luminosity to normalize to [default: 2015+2016]")
parser.add_argument('--debug', action='store_true',
    help="Add extra printouts")
parser.add_argument('--plots', nargs='+',
    help="List of plots to make. Options include 'SF','NF','Tmpl','Kine',...")
args = parser.parse_args()

#import ROOT
from PlotFunctions import *
from TAxisFunctions import *
import ConfigFunctions as config
from ROOT import gROOT,gStyle,TFile,Double
from ROOT import TCanvas,TPad,TLegend,TH2D,THStack,TLatex,TGraphAsymmErrors,TLine

gROOT.SetBatch(True)
gROOT.SetStyle('ATLAS')
gStyle.SetErrorX(0.5)

# set variables from args
outdir = args.input+'/Plots/'
if not os.path.isdir(outdir):
  os.mkdir(outdir)

MyConfig = config.LoadGlobalConfig()
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
  def GetCorr(self,name1,name2):
    idx1 = self.nuisPars[name1][3]
    idx2 = self.nuisPars[name2][3]
    return self.corrMat[idx1][idx2]

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
    path = "{0}/TRExFit/Fit_{1}_Data/Fits/Fit_{1}_Data.txt".format(args.input,region.Data())

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
          row = 0
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
          # index to look for in correlation matrix
          nuisPars[name].append(row)
          row += 1
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
def Make2DPlot(bin_vals, bin_errs, name):
  # Set up bins and labels for 2D plots
  mj_labels = makeTrkJetLabels(MyConfig.GetMuonJetPtBins(),"#mu-jet")
  nmj_labels = makeTrkJetLabels(MyConfig.GetNonMuJetPtBins(),"non-#mu-jet")

  # Create canvas
  canv = TCanvas('c','',800,600)
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

  canv.SaveAs(outdir+'2D'+name+'.pdf')
  del canv

#-----------------------------------------------
def Make1DPlot(bin_vals, bin_yerr, name):

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
  canv = TCanvas('c','',800,600)

  # Make TGraph with error bands
  gr = TGraphAsymmErrors(N,bin_x[0],bin_vals[0],bin_xerr[0],bin_xerr[0],bin_yerr[0],bin_yerr[0])
  gr.SetMarkerSize(0)
  gr.SetFillColor(419)
  gr.SetFillStyle(3001)
  gr.SetTitle("")

  gr.GetXaxis().SetTitle("Large-R Jet p_{T} [GeV]")
  gr.GetYaxis().SetTitle(name)

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

  gr.GetXaxis().SetRangeUser(fj_low_cut,fj_bins[len(fj_bins)-1])
  gr.GetYaxis().SetRangeUser(0.,2.)
  if 'Frac' in name:
    gr.GetYaxis().SetRangeUser(0.,0.5)

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

  if 'ScaleFactor' in name:
    line = TLine(fj_low_cut,1.,fj_bins[len(fj_bins)-1],1.)
    line.SetLineStyle(2)
    line.SetLineWidth(1.5)
    line.Draw("same")

  canv.SaveAs(outdir+name+'.pdf')
  del canv

#-----------------------------------------------
def MakeFitPlot(fitResults, nuisPar):
  bin_vals = np.zeros(len(regions))
  bin_errs = np.zeros(len(regions))
  for i,region in enumerate(regions):
    result = fitResults[region.Data()]
    bin_vals[i] = result.GetPar(nuisPar)
    bin_errs[i] = result.GetErr(nuisPar)
  if args.bins == 'trkjet':
    Make2DPlot(bin_vals, bin_errs, nuisPar)
  elif args.bins == 'fatjet':
    Make1DPlot(bin_vals, bin_errs, nuisPar)

#-----------------------------------------------
def MakeFlavFracPlot(fitResults, nuisPar, prefit=True):
  # make sure the nuisPar provided is a valid flavor pair
  flavCheck = False
  FlavList = MyConfig.GetFlavourPairs()
  for flav in FlavList:
    if flav.Data() in nuisPar:
      flavCheck = True
      break
  if not flavCheck:
    print(nuisPar+" is not in the list of flavor pairs")
    return

  name = nuisPar + 'Frac'
  if prefit:
    name += 'Prefit'
  bin_vals = np.zeros(len(regions))
  bin_errs = np.zeros(len(regions))
  for i,region in enumerate(regions):
    result = results[region.Data()]
    mc = {}
    stat_err = {}
    mc_tot = 0
    for flav in FlavList:
      h_temp = config.getKey(infile,MyConfig.GetMCHistName('Nom',region,flav,'mjmeanSd0').Data())
      err = Double(0)
      n_temp = h_temp.IntegralAndError(0,h_temp.GetNbinsX()+1,err)
      mc[flav.Data()] = n_temp
      stat_err[flav.Data()] = err
      if prefit:
        mc_tot += n_temp
      else:
        mc_tot += n_temp * result.GetPar(flav.Data())

    if prefit:
      bin_vals[i] = mc[nuisPar] / mc_tot
      bin_errs[i] = stat_err[nuisPar] / mc_tot
    else :
      this_frac = mc[nuisPar] * result.GetPar(nuisPar) / mc_tot
      bin_vals[i] = this_frac
      err_temp = 0
      stat_err_temp = pow(stat_err[nuisPar] * result.GetPar(flav.Data()) / mc_tot,2)
      for flav_i in FlavList:
        dFdxi = -this_frac * mc[flav_i.Data()] / mc_tot
        if nuisPar in flav_i.Data():
          dFdxi += mc[nuisPar] / mc_tot
        for flav_j in FlavList:
          dFdxj = -this_frac * mc[flav_j.Data()] / mc_tot
          if nuisPar in flav_j.Data():
            dFdxj += mc[nuisPar] / mc_tot
          err_temp += dFdxi * dFdxj * result.GetCorr(flav_i.Data(),flav_j.Data()) \
                      * result.GetErr(flav_i.Data()) * result.GetErr(flav_j.Data())
      bin_errs[i] = np.sqrt(err_temp + stat_err_temp)

  if args.bins == 'trkjet':
    Make2DPlot(bin_vals,bin_errs,name)
  elif args.bins == 'fatjet':
    Make1DPlot(bin_vals,bin_errs,name)

#-----------------------------------------------
def MakeRatioPlots(var,prefit=True,doErr=True,doChi2=False,setLogy=False):

  name = var
  if prefit:
    name += '_prefit'
  else:
    name += '_postfit'
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
    result = results[region.Data()]
    h_mcSum = None
    h_fitErr = None
    h_sysErr = None
    mcStack = THStack()

    leg = TLegend(0.84,0.58,0.94,0.88)
    leg.SetBorderSize(0)

    h_data = config.getKey(infile,MyConfig.GetDataHistName(region,var).Data())
    h_data.SetLineColor(1)
    h_data.SetMarkerStyle(20)
    leg.AddEntry(h_data,'Data','epl')

    for j,flav in enumerate(FlavList):
      h_mc = config.getKey(infile,MyConfig.GetMCHistName('Nom',region,flav,var).Data())
      if not prefit:
        h_mc.Scale(result.GetPar(flav.Data()))
        if doErr:
          h_temp = h_mc.Clone()
          h_temp.Scale( 1+result.GetErr(flav.Data())/result.GetPar(flav.Data()) )
          if not h_fitErr:
            h_fitErr = h_temp
          else:
            h_fitErr.Add(h_temp)

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
      # rebinning means data is weighted histogram too
      chi2 = h_data.Chi2Test(h_mcSum,"WW CHI2/NDF");
      #print(str(chi2))
      chi2Text='#chi^{{2}}/NDF = {:.2f}'.format(chi2)

    pad1.cd()
    h_data.Draw('EP')
    h_data.GetYaxis().SetLabelSize( 0.04 )
    h_data.GetYaxis().SetTitleSize( 0.04 )
    h_data.GetYaxis().SetTitleOffset(1.)
    if setLogy:
      h_data.GetYaxis().SetRangeUser(0.1*mcStack.GetMinimum(),2*h_data.GetMaximum())
    else:
      h_data.GetYaxis().SetRangeUser(0.1*mcStack.GetMinimum(),1.2*h_data.GetMaximum())
    mcStack.Draw('hist same')
    h_data.Draw('EP same') # redraw data so it sits in front
    #AddHistogram(canv,h_data,'e')
    #AddHistogram(canv,mcStack,'hist')
    if doErr and h_fitErr:
      nbins = h_mcSum.GetNbinsX()
      bin_x = np.zeros(nbins)
      err_x = np.zeros(nbins)
      bin_y = np.zeros(nbins)
      err_y = np.zeros(nbins)
      for ibin in range(1,nbins+1):
        err_x[ibin-1] = (h_fitErr.GetBinLowEdge(ibin) - h_fitErr.GetBinLowEdge(ibin))/2
        bin_x[ibin-1] = h_fitErr.GetBinLowEdge(ibin) + err_x[ibin-1]
        bin_y[ibin-1] = h_mcSum.GetBinContent(ibin)
        err_y[ibin-1] = abs(h_fitErr.GetBinContent(ibin) - bin_y[ibin-1])
      g_fitErr = TGraphAsymmErrors(nbins,bin_x,bin_y,err_x,err_x,err_y,err_y)
      g_fitErr.Draw('2')
      #AddHistogram(canv,g_fitErr,'2')
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

    #AddRatio(canv,h_data,h_mcSum)
    #SetAxisLabels(canv,h_data.GetXaxis().GetTitle(),h_data.GetYaxis().GetTitle())
    #FullFormatCanvasDefault(canv,lumi=Lumi/1000,additionaltext1=tagText,additionaltext2=chi2Text)
    #SetColors(canv,colors)
    #SetYaxisRangesRatio(canv,0.5,1.5)
    canv.SaveAs(outdir+region.Data()+'_'+name+'.pdf')
    # finally, clear the canvas for the next histograms
    canv.Clear('D')

def MakeTemplatePlots():
  for var in MyConfig.GetTemplateVariables():
    MakeRatioPlots(var.Data(),prefit=True ,doChi2=True,setLogy=True)
    MakeRatioPlots(var.Data(),prefit=False,doChi2=True,setLogy=True)

    MakeRatioPlots(var.Data()+'_2TAG',prefit=True ,doChi2=True,setLogy=True)
    MakeRatioPlots(var.Data()+'_2TAG',prefit=False,doChi2=True,setLogy=True)

    #MakeRatioPlots(var.Data()+'_NOT2TAG',prefit=True ,doChi2=True,setLogy=True)
    #MakeRatioPlots(var.Data()+'_NOT2TAG',prefit=False,doChi2=True,setLogy=True)

def MakeKinematicPlots():
  for var in ['fjpt','fjm','mjpt','nmjpt']:
    MakeRatioPlots(var,prefit=True ,doChi2=True)
    MakeRatioPlots(var,prefit=False,doChi2=True)

    MakeRatioPlots(var+'_2TAG',prefit=True ,doChi2=True)
    MakeRatioPlots(var+'_2TAG',prefit=False,doChi2=True)

    #MakeRatioPlots(var+'_NOT2TAG',prefit=True ,doChi2=True)
    #MakeRatioPlots(var+'_NOT2TAG',prefit=False,doChi2=True)

#-----------------------------------------------
# Main function
#-----------------------------------------------
# Get fit results
results = ReadFitResults()
# Get input histograms
infile = TFile("{0}/trex_input.root".format(args.input))

if 'SF' in args.plots:
  MakeFitPlot(results, 'ScaleFactor')
if 'Tmpl' in args.plots:
  MakeTemplatePlots()
if 'Kine' in args.plots:
  MakeKinematicPlots()
if 'NF' in args.plots:
  for flav in MyConfig.GetFlavourPairs():
    MakeFitPlot(results, flav.Data())
    MakeFlavFracPlot(results, flav.Data(), prefit=True)
    MakeFlavFracPlot(results, flav.Data(), prefit=False)
