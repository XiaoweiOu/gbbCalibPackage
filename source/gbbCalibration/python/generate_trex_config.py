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
parser.add_argument('--fitSF', action='store_true',
    help="Fit scale factor directly (rather than only correcting flavor fractions)")
parser.add_argument('--cfg', type=str,
    help="Name of extra config file (located in gbbCalibration/data/configs)")
parser.add_argument('--debug', type=int, default=2,
    help="Debug level for TRexFitter [default: 2]")
parser.add_argument('--nosys', action='store_true',
    help="Run nominal-only fit")
args = parser.parse_args()

# define arguments
debug = args.debug
doAsimov = args.asimov
doSystematics = not args.nosys
fitSF = args.fitSF
bins = args.bins
useMCstats = not args.noMCstat
MCstatThreshold = 0

MyConfig = config.LoadGlobalConfig()
if args.cfg:
  trexConfig = config.GetDataFile('configs/'+args.cfg)
  if not trexConfig:
    exit()
  else:
    # Parse config file
    # Expect each line to be in the form;
    #   Key Value1(,Value2...)
    with open(trexConfig,'r') as f:
      line = f.readline()
      while line:
        line = line.strip()
        if not line:
          line = f.readline()
          continue
        elif line[0] == '#':
          line = f.readline()
          continue
        tokens = line.split()
        # check key against possible arguments and store value(s) in global variable
        if tokens[0] == 'Debug':
          debug = tokens[1]
        elif tokens[0] == 'doAsimov':
          doAsimov = int(tokens[1])
        elif tokens[0] == 'doSystematics':
          doSystematics = int(tokens[1])
        elif tokens[0] == 'fitSF':
          fitSF = int(tokens[1])
        elif tokens[0] == 'useMCstats':
          useMCstats = int(tokens[1])
        elif tokens[0] == 'MCstatThreshold':
          MCstatThreshold = float(tokens[1])
        elif tokens[0] == 'bins':
          bins = tokens[1]
        elif tokens[0] == 'year':
          year = tokens[1]
        else:
          print('Unrecognized option string: '+tokens[0])

        line = f.readline()

if doSystematics:
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
if bins == "trkjet":
  ListOfPtBins = MyConfig.GetDiTrkJetRegions()
elif bins == "fatjet":
  ListOfPtBins = MyConfig.GetFatJetRegions()
elif bins == "incl":
  ListOfPtBins = [ TString("Incl") ]
else:
  print("Please choose either trkjet or fatjet binning. "+bins+" is not an option")

Lumi = config.GetLumi(year)
print("Lumi is "+str(Lumi)+" /pb.")

colorStr = {
  'BB' : '0,0,202',
  'BL' : '109,176,253',
  'CC' : '0,83,0',
  'CL' : '0,253,172',
  'LL' : '255,204,0'
}

def GetVarTitle(var):
  title = ''
  if 'nmj' in var:
    title += 'Non-muon-jet'
  elif 'mj' in var:
    title += 'Muon-jet'

  if 'meanSd0' in var:
    title += ' #LT s_{d0} #GT'

  return title

def GetVarLabel(var):
  if 'NOT2TAG' in var:
    return 'failed double-B tag'
  elif '2TAG' in var:
    return 'passed double-B tag'
  else:
    return 'pre B-tagging'

def WriteRegionBlock(outfile,ptbin,var):
  outfile.write('Region: "'+var+'"\n')
  outfile.write('  Type: SIGNAL\n')
  outfile.write('  LogScale: TRUE\n')
  outfile.write('  HistoName: "h_'+ptbin+'_'+var+'"\n') #TODO: use config.GetName() functions
  outfile.write('  VariableTitle: "'+GetVarTitle(var)+'"\n')
  outfile.write('  Label: "'+GetVarLabel(var)+'"\n')
  outfile.write('\n')

def WriteSampleBlock(outfile,flav):
  outfile.write('Sample: "'+flav+'"\n')
  outfile.write('  Type: BACKGROUND\n')
  outfile.write('  Title: "'+flav+'"\n')
  outfile.write('  HistoNameSuff: "_'+flav+'"\n')
  outfile.write('  FillColorRGB: '+colorStr[flav]+'\n')
  outfile.write('  LineColorRGB: 0,0,0\n')
  outfile.write('  NormFactor: "'+flav+'",1,0,100\n')
  if useMCstats:
    outfile.write('  UseMCstat: TRUE\n')
  else:
    outfile.write('  UseMCstat: FALSE\n')
  outfile.write('\n')

  #outfile.write('NormFactor: "'+flav+'"\n')
  #outfile.write('  Samples: "'+flav+'"\n')
  #outfile.write('  Title: "'+flav+'"\n')
  #outfile.write('  Nominal: 1\n')
  #outfile.write('  Min: 0.1\n')
  #outfile.write('  Max: 10\n')
  #outfile.write('  Tau: 1\n')
  #outfile.write('\n')

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
    if doAsimov:
      jobStr += '_Asimov'
    else:
      jobStr += '_Data'
    # write Job block
    outfile.write('Job: "'+jobStr+'"\n')
    #outfile.write('  Label: "R21"\n')
    outfile.write('  CmeLabel: "13 TeV"\n')
    outfile.write('  LumiLabel: "%1.1f fb^{-1}"\n' % float(Lumi/1000.))
    if fitSF:
      outfile.write('  POI: "ScaleFactor"\n')
    else:
      outfile.write('  POI: "BB"\n')
    outfile.write('  ReadFrom: HIST\n')
    outfile.write('  HistoPath: "'+outdir+'/"\n')
    outfile.write('  HistoFile: "trex_input"\n')
    outfile.write('  HistoNameNominal: "_Nom"\n')
    outfile.write('  OutputDir: "'+outdir+'/TRExFit/"\n')
    outfile.write('  DebugLevel: 2\n')
    outfile.write('  SystControlPlots: FALSE\n')
    if useMCstats:
      outfile.write('  UseGammaPulls: TRUE\n')
      outfile.write('  MCstatThreshold: {:.2f}\n'.format(MCstatThreshold))
    else:
      outfile.write('  UseGammaPulls: FALSE\n')
    outfile.write('\n')

    # write Fit block
    outfile.write('Fit: "myFit"\n')
    outfile.write('  FitType: SPLUSB\n')
    outfile.write('  FitRegion: CRSR\n')
    outfile.write('  GetGoodnessOfFit: TRUE\n')
    if doAsimov:
      outfile.write('  FitBlind: TRUE\n')
      outfile.write('  POIAsimov: 1\n')
    else:
      outfile.write('  FitBlind: FALSE\n')
    outfile.write('\n')

    # write region blocks
    for var in ListOfTmplVars:
      if fitSF:
        WriteRegionBlock(outfile,ptbin,var.Data()+'_2TAG')
        WriteRegionBlock(outfile,ptbin,var.Data()+'_NOT2TAG')
        #WriteRegionBlock(outfile,ptbin,var.Data())
      else:
        WriteRegionBlock(outfile,ptbin,var.Data())

    if not doAsimov:
      # write block for data
      outfile.write('Sample: "Data"\n')
      outfile.write('  Title: "Data"\n')
      outfile.write('  Type: DATA\n')
      outfile.write('  HistoNameSuff: "_Data"\n')
      outfile.write('\n')

    # write blocks for mc samples
    for flav in ListOfFlavourPairs:
      WriteSampleBlock(outfile,flav.Data())

    if fitSF:
      #outfile.write('Region: "NEvts_2TAG"\n')
      #outfile.write('  Type: SIGNAL\n')
      #outfile.write('  HistoName: "h_'+ptbin+'_NEvts_2TAG"\n') #TODO: use config.GetName() functions
      #outfile.write('  VariableTitle: "N_{Events}^{BB-tagged}"\n')
      #outfile.write('  Label: "BB-tagged Events"\n')
      #outfile.write('\n')

      #outfile.write('NormFactor: "ScaleFactor"\n')
      #outfile.write('  Samples: "BB"\n')
      #outfile.write('  Regions: "NEvts_2TAG"\n')
      #outfile.write('  Title: "ScaleFactor"\n')
      #outfile.write('  Nominal: 1\n')
      #outfile.write('  Min: 0\n')
      #outfile.write('  Max: 10\n')
      #outfile.write('\n')

      outfile.write('NormFactor: "ScaleFactor"\n')
      outfile.write('  Samples: "BB"\n')
      outfile.write('  Regions: "mjmeanSd0_2TAG","nmjmeanSd0_2TAG"\n')
      outfile.write('  Title: "ScaleFactor"\n')
      outfile.write('  Nominal: 1\n')
      outfile.write('  Min: 0.1\n')
      outfile.write('  Max: 10\n')
      outfile.write('\n')
      outfile.write('NormFactor: "AntiScaleFactor"\n')
      outfile.write('  Samples: "BB"\n')
      outfile.write('  Regions: "mjmeanSd0_NOT2TAG","nmjmeanSd0_NOT2TAG"\n')
      outfile.write('  Title: "ScaleFactor"\n')
      outfile.write('  Nominal: 1\n')
      outfile.write('  Min: 0.1\n')
      outfile.write('  Max: 10\n')
      outfile.write('  Expression: (1/ScaleFactor):ScaleFactor[1.,0.1,10.]\n')
      outfile.write('\n')

    if doSystematics:
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

