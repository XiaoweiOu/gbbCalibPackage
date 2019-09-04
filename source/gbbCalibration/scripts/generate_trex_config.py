import argparse
import ConfigFunctions as config
import os
from ROOT import TString

parser = argparse.ArgumentParser(description='Make ratio plots.')
parser.add_argument('output', help="Name of output directory")
parser.add_argument('--bins', type=str, default="trkjet",
    help="Use trkjet bins, fatjet bins or no bins (incl) [default: trkjet]")
parser.add_argument('--year', type=str, default="2015+2016",
    help="Year determines luminosity to normalize to [default: 2015+2016]")
parser.add_argument('--asimov', action='store_true',
    help="Do Asimov fit rather than fitting to data")
parser.add_argument('--noMCstat', action='store_true',
    help="Don't use MC stat errors in fit for comparison to old framework")
parser.add_argument('--simul', action='store_true',
    help="Simultaneously fit pre- and post-tag regions")
parser.add_argument('--fitSF', action='store_true',
    help="Fit scale factor directly (rather than only correcting flavor fractions)")
parser.add_argument('--debug', type=int, default=2,
    help="Debug level for TRexFitter [default: 2]")
parser.add_argument('--nosys', action='store_true',
    help="Run nominal-only fit")
args = parser.parse_args()

useMCstat = not args.noMCstat
MyConfig = config.LoadGlobalConfig()

if not args.nosys:
  ListOfSystematics = MyConfig.GetSystematics()
  ListOfSystematicsSd0 = MyConfig.GetSystematics_Sd0()
  #ListOfSystematicsWeightVar = MyConfig.GetSystematics_WeightVar()
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
        ListOf1SidedSysts.append(sysName)
  for sys in ListOfSystematicsSd0:
    twoSided = False
    if "__1up" in sys.Data():
      sysName = sys.Data()[:-5]
      for sys2 in ListOfSystematicsSd0:
        if sysName in sys2.Data() and "__1down" in sys2.Data():
          ListOf2SidedSysts.append(sysName)
          twoSided = True
      if not twoSided:
        ListOf1SidedSysts.append(sysName)

ListOfFlavourPairs = MyConfig.GetFlavourPairs()
ListOfTmplVars = MyConfig.GetTemplateVariables()
if args.bins == "trkjet":
  ListOfPtBins = MyConfig.GetTrkJetRegions()
elif args.bins == "fatjet":
  ListOfPtBins = MyConfig.GetFatJetRegions()
elif args.bins == "incl":
  ListOfPtBins = [ TString("Incl") ]
else:
  print("Please choose either trkjet or fatjet binning. "+args.bins+" is not an option")

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
print("Lumi is "+str(Lumi)+" /pb.")

colorStr = {
  'BB' : '0,0,202',
  'BL' : '109,176,253',
  'CC' : '0,83,0',
  'CL' : '0,253,172',
  'LL' : '255,204,0'
}

varTitles = {
  'mjmeanSd0' : 'Muon-jet #LT s_{d0} #GT',
  'nmjmeanSd0' : 'Non-muon-jet #LT s_{d0} #GT',
  'mjmeanSd0_PREFITPOSTTAG' : 'BB-tagged Muon-jet #LT s_{d0} #GT',
  'nmjmeanSd0_PREFITPOSTTAG' : 'BB-tagged Non-muon-jet #LT s_{d0} #GT'
}

def WriteRegionBlock(outfile,ptbin,var):
  outfile.write('Region: "'+var+'"\n')
  outfile.write('  Type: SIGNAL\n')
  outfile.write('  LogScale: TRUE\n')
  outfile.write('  HistoName: "h_'+ptbin+'_'+var+'"\n') #TODO: use config.GetName() functions
  outfile.write('  VariableTitle: "'+varTitles[var]+'"\n')
  outfile.write('\n')

def WriteSampleBlock(outfile,flav):
  outfile.write('Sample: "'+flav+'"\n')
  outfile.write('  Type: BACKGROUND\n')
  outfile.write('  Title: "'+flav+'"\n')
  outfile.write('  HistoNameSuff: "_'+flav+'"\n')
  outfile.write('  FillColorRGB: '+colorStr[flav]+'\n')
  outfile.write('  LineColorRGB: 0,0,0\n')
  outfile.write('  NormFactor: "'+flav+'Fraction",1,0,100\n')
  if useMCstat:
    outfile.write('  UseMCstat: TRUE\n')
  else:
    outfile.write('  UseMCstat: FALSE\n')
  outfile.write('\n')

def WriteSystBlock2Sided(outfile,sys):
  outfile.write('Systematic: "'+sys+'"\n')
  outfile.write('  Type: HISTO\n')
  outfile.write('  Title: "'+sys+'"\n')
  outfile.write('  Samples: all\n')
  outfile.write('  HistoNameSufUp: "_'+sys+'__1up"\n')
  outfile.write('  HistoNameSufDown: "_'+sys+'__1down"\n')
  outfile.write('\n')

def WriteSystBlock1Sided(outfile,sys):
  outfile.write('Systematic: "'+sys+'"\n')
  outfile.write('  Type: HISTO\n')
  outfile.write('  Title: "'+sys+'"\n')
  outfile.write('  Samples: all\n')
  outfile.write('  HistoNameSufUp: "_'+sys+'__1up"\n')
  outfile.write('  Symmetrisation: ONESIDED\n')
  outfile.write('\n')

## TODO: Add systematics blocks

def WriteConfigFile(ptbin,outdir):
  with open(outdir+'/fit_'+ptbin+'.config','w') as outfile:
    jobStr = 'Fit_'+ptbin
    if args.asimov:
      jobStr += '_Asimov'
    else:
      jobStr += '_Data'
    # write Job block
    outfile.write('Job: "'+jobStr+'"\n')
    outfile.write('  Label: "R21"\n')
    outfile.write('  CmeLabel: "13 TeV"\n')
    outfile.write('  LumiLabel: "%1.1f fb^{-1}"\n' % float(Lumi/1000.))
    if args.fitSF:
      outfile.write('  POI: "ScaleFactor"\n')
    else:
      outfile.write('  POI: "BBFraction"\n')
    outfile.write('  ReadFrom: HIST\n')
    outfile.write('  HistoPath: "'+outdir+'/"\n')
    outfile.write('  HistoFile: "trex_input"\n')
    outfile.write('  HistoNameNominal: "_Nom"\n')
    outfile.write('  OutputDir: "'+outdir+'/TRexFit/"\n')
    outfile.write('  DebugLevel: 2\n')
    outfile.write('  SystControlPlots: FALSE\n')
    if useMCstat:
      outfile.write('  UseGammaPulls: TRUE\n')
    else:
      outfile.write('  UseGammaPulls: FALSE\n')
    outfile.write('\n')

    # write Fit block
    outfile.write('Fit: "myFit"\n')
    outfile.write('  FitType: SPLUSB\n')
    outfile.write('  FitRegion: CRSR\n')
    outfile.write('  GetGoodnessOfFit: TRUE\n')
    if args.asimov:
      outfile.write('  FitBlind: TRUE\n')
      outfile.write('  POIAsimov: 1\n')
    else:
      outfile.write('  FitBlind: FALSE\n')
    outfile.write('\n')

    # write region blocks
    for var in ListOfTmplVars:
      WriteRegionBlock(outfile,ptbin,var.Data())
      if args.simul:
        WriteRegionBlock(outfile,ptbin,var.Data()+'_PREFITPOSTTAG')

    if not args.asimov:
      # write block for data
      outfile.write('Sample: "Data"\n')
      outfile.write('  Title: "Data"\n')
      outfile.write('  Type: DATA\n')
      outfile.write('  HistoNameSuff: "_Data"\n')
      outfile.write('\n')

    # write blocks for mc samples
    for flav in ListOfFlavourPairs:
      WriteSampleBlock(outfile,flav.Data())

    if args.fitSF:
      outfile.write('Region: "NEvts_PREFITPOSTTAG"\n')
      outfile.write('  Type: SIGNAL\n')
      outfile.write('  HistoName: "h_'+ptbin+'_NEvts_PREFITPOSTTAG"\n') #TODO: use config.GetName() functions
      outfile.write('  VariableTitle: "N_{Events}^{BB-tagged}"\n')
      outfile.write('  Label: "BB-tagged Events"\n')
      outfile.write('\n')

      outfile.write('NormFactor: "ScaleFactor"\n')
      outfile.write('  Samples: "BB"\n')
      outfile.write('  Regions: "NEvts_PREFITPOSTTAG"\n')
      outfile.write('  Title: "ScaleFactor"\n')
      outfile.write('  Nominal: 1\n')
      outfile.write('  Min: 0\n')
      outfile.write('  Max: 10\n')
      outfile.write('\n')

    if not args.nosys:
      # write systematics blocks
      for sys in ListOf1SidedSysts:
        WriteSystBlock1Sided(outfile,sys)
      for sys in ListOf2SidedSysts:
        WriteSystBlock2Sided(outfile,sys)

### All necessary functions defined, now do the work ###
outdir = args.output
if not os.path.isdir(outdir):
  makeDir = raw_input('The output directory does not exist. Create? [y/n] ')
  if makeDir.lower() == 'y':
    os.mkdir(outdir)
  else:
    print('Exiting...')
    exit()

for ptbin in ListOfPtBins:
  WriteConfigFile(ptbin.Data(),outdir)

