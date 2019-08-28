import ROOT
from PlotFunctions import *
from TAxisFunctions import *
import ConfigFunctions as config
import argparse
import os

ROOT.gROOT.SetBatch(True)
from ROOT import TCanvas,TPad,TH2,TColor

#ROOT.gStyle.SetOptStat(0)
SetupStyle()

parser = argparse.ArgumentParser(description='Make ratio plots.')
parser.add_argument('output', help="Prefix of output files")
parser.add_argument('--data', nargs='*', help="Input data file(s)")
parser.add_argument('--mc', nargs='*', help="Input mc folder(s)")
parser.add_argument('--norm1', action='store_true',
                    help="Normalize histograms to 1 (always true for data/data ratios) [default: false]")
parser.add_argument('--chi2', action='store_true',
                    help="Print chi^2 on plots [default: false]")
parser.add_argument('--pdf', action='store_true',
                    help="Store plots in a single pdf [default: separate files]")
parser.add_argument('--mcflag', default="comb", choices=["incl","mufilt","comb"],
                    help="Tell script how to make MC sample. Options are inclusive-only (incl), mu-filtered only (mufilt) or LL-inclusive (comb) [default: comb]")
parser.add_argument('--year', type=str, default="2017",
    help="Year determines luminosity to normalize to [default: 2017]")
parser.add_argument('--xsec', default="xsections_r21.txt", help="Name of cross-sections file [default: xsections_r21.txt]")
args = parser.parse_args()

dataOnly=False
if not args.data:
  print("No data file(s) given. Exiting...")
  exit()

if not args.mc:
  if len(args.data) == 2:
    print("No MC given. Making data/data ratios")
    dataOnly=True
  else:
    print("No MC given, and number of data files != 2. Exiting...")
    exit()
else:
  print("Making data/MC ratios")
  if len(args.data) > 1:
    print("Only first data file ("+args.data[0]+") will be used")

MyConfig = config.LoadGlobalConfig()
if not dataOnly:
  ListOfSystematics = [ ROOT.TString("Nominal") ] #MyConfig.GetSystematics()
  ListOfFlavourPairs = MyConfig.GetFlavourPairs()
  if args.mcflag == 'incl':
    print("Using inclusive samples for all flavours")
    ListOfInclusiveFlavourPairs = ListOfFlavourPairs
  elif args.mcflag == 'mufilt':
    print("Using mu-filtered samples for all flavours")
    ListOfInclusiveFlavourPairs = []
  else:
    print("Using inclusive samples for LL template only")
    ListOfInclusiveFlavourPairs = [ 'LL' ]

  print("Using x-sections file: "+args.xsec)
  histHelper = config.HistHelper(args.xsec)

  # luminosity values from https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/GoodRunListsForAnalysisRun2 and may change slightly if data is reprocessed
  # values used should match the GRL and lumicalc file used to generate the ntuples
  Lumi = 0
  if args.year == "2015" :
    Lumi = 3219.56 # in /pb
  elif args.year == "2016" :
    Lumi = 32988.1 # in /pb
  elif args.year == "2015+2016" :
    Lumi = 3219.56 + 32988.1 # in /pb
  elif args.year == "2017" :
    Lumi = 44307.4 # in /pb
  elif args.year == "2018" :
    Lumi = 58450.1 # in /pb
  print("Lumi is "+str(Lumi))

  ListOfMCPaths = []
  ListOfInclusiveMCPaths = []
  for mcdir in args.mc:
    for subdir, dirs, files in os.walk(mcdir):
      for mcfile in files:
        channel = config.GetChannelNumber(mcfile)
        if channel > 361019 and channel < 361033:
          ListOfInclusiveMCPaths.append(subdir + os.sep + mcfile)
        elif (channel > 427002 and channel < 427006) \
          or (channel > 427105 and channel < 427108):
          ListOfMCPaths.append(subdir + os.sep + mcfile)
        else:
          print("Not sure how to categorize file: "+mcfile)

colors = [ROOT.kBlack, ROOT.kRed, ROOT.kBlue]
canv = RatioCanvas('c',"",800,800,0.3,'pE1',doLogy=True)
#SetColors(canvas,colors)

if args.pdf:
  canv.Print(args.output+'.pdf[')
else:
  os.mkdir(args.output)

print("Finished setup! Now beginning histogram reading...")
fileData1 = ROOT.TFile(args.data[0],"READ")
if fileData1.IsZombie():
  print("Cannot open data file "+args.data[0])
  exit()
if dataOnly:
  fileData2 = ROOT.TFile(args.data[1],"READ")
  if fileData2.IsZombie():
    print("Cannot open data file "+args.data[1])
    exit()

# loop over all histograms in first data file
for key in fileData1.GetListOfKeys():
  splitByFlav = True
  #if 'hDataNom' not in key.GetName(): #FIXME: hardcodes naming convention = bad
  #  continue # skip plots inclusive in flavour for now
  #if any(x in key.GetName() for x  in ['hIncl']):
  if any(x in key.GetName() for x  in ['CutFlow','EventMu','EventAvgMu','EventPVz','PUDensity']):
    splitByFlav = False
  if splitByFlav:
    if '_Incl_' not in key.GetName():
      continue # only plots inclusive in pT for now
    if 'BTAG' in key.GetName():
      continue # skip btag systemic for now
    if 'unweighted' in key.GetName():
      continue # skip unweighted for now

  print("Found histogram "+key.GetName())
  histNum = fileData1.Get(key.GetName())
  if histNum.InheritsFrom("TH2"):
    continue # skip 2D histograms for now
  if histNum:
    if 'data17' in args.data[0]:
      histNum.SetName("data17")
    elif 'data1516' in args.data[0]:
      histNum.SetName("data15+16")
    elif 'data16' in args.data[0]:
      histNum.SetName("data16")
    elif 'data15' in args.data[0]:
      histNum.SetName("data15")
  else:
    print("Cannot find hist "+key.GetName()+" in file "+args.data[0])
    continue

  histDen = None
  if dataOnly: # data/data ratio
    histDen = fileData2.Get(key.GetName())
    if histDen:
      if 'data17' in args.data[1]:
        histDen.SetName("data17")
      elif 'data1516' in args.data[1]:
        histDen.SetName("data15+16")
      elif 'data16' in args.data[1]:
        histDen.SetName("data16")
      elif 'data15' in args.data[1]:
        histDen.SetName("data15")
    else:
      print("Cannot find hist "+key.GetName()+" in file "+args.data[1])
      continue

  else: # data/mc ratio
    histTest = histHelper.AddMCHists((key.GetName()).replace('Data','Incl'),ListOfMCPaths)
    if histTest:
      splitByFlav = False

    if splitByFlav:
      for flavour in ListOfFlavourPairs:
        ListOfPaths = ListOfMCPaths
        if flavour in ListOfInclusiveFlavourPairs:
          ListOfPaths = ListOfInclusiveMCPaths

        histname = (key.GetName()).replace('Data',flavour.Data())
        histTemp = histHelper.AddMCHists(histname,ListOfPaths)
        if histTemp:
          histTemp.Scale(Lumi)
          if not histDen:
            histDen = histTemp
          else:
            histDen.Add(histTemp)
        else:
          print("Could not find "+histname+" in all input files!")
          continue
      if histDen:
        histDen.SetLineWidth(3);
        histDen.SetName("Pythia8 MC")
      else:
        print("Cannot find hist "+histname+" in MC files")
        continue
    else: #plots inclusive in flavour
      #histDen = histHelper.AddMCHists(key.GetName(),ListOfMCPaths)
      histDen = histHelper.AddMCHists(key.GetName(),ListOfInclusiveMCPaths)
      if histDen:
        histDen.Scale(Lumi)
        histDen.SetLineWidth(3);
        histDen.SetName("Pythia8 Incl MC")
      else:
        print("Cannot find hist "+key.GetName()+" in inclusive MC files")
        continue

  # got numerator and denominator, now draw them
  tagText=''
  if 'POSTTAG' in key.GetName():
    tagText='double-b-tagged'
  chi2Text=''
  if args.chi2:
    if dataOnly:
      chi2 = histNum.Chi2Test(histDen,"UU CHI2/NDF");
    else:
      chi2 = histNum.Chi2Test(histDen,"UW CHI2/NDF");
      #chi2 = histNum.AndersonDarlingTest(histDen);
    #print(str(chi2))
    chi2Text='#scale[0.6]{{#chi^{{2}}/NDF = {:.2f}}}'.format(chi2)
    #print(chi2Text)
  if args.norm1 or dataOnly:
    histNum.Scale(1/histNum.Integral())
    histDen.Scale(1/histDen.Integral())
  AddHistogram(canv,histNum,'e')
  if dataOnly:
    AddHistogram(canv,histDen,'e')
  else:
    AddHistogram(canv,histDen,'histe')
  AddRatio(canv,histNum,histDen)
  lumi=Lumi/1000
  yTitle=histNum.GetYaxis().GetTitle()
  if args.norm1 or dataOnly:
    lumi=''
    yTitle='Arbitrary Units'
  SetAxisLabels(canv,histNum.GetXaxis().GetTitle(),yTitle)
  FullFormatCanvasDefault(canv,lumi=lumi,additionaltext1=tagText,additionaltext2=chi2Text)
  SetColors(canv,colors)
  SetYaxisRangesRatio(canv,0.5,1.5)
  if args.pdf:
    canv.Print(args.output+'.pdf')
  else:
    canv.Print(args.output+os.sep+(key.GetName()).replace('hDataNom_','')+'.eps')
    canv.Print(args.output+os.sep+(key.GetName()).replace('hDataNom_','')+'.png')
  # finally, clear the canvas for the next histograms
  canv.Clear('D')

if args.pdf:
  canv.Print(outfilename+'.pdf]')

canv.Close()
fileData1.Close()
if dataOnly:
  fileData2.Close()
