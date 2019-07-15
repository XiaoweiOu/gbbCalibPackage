import ROOT
#from PlotFunctions import *
#from TAxisFunctions import *
import ConfigFunctions as config
import argparse

ROOT.gROOT.SetBatch(True)
from ROOT import TCanvas,TPad,TString


#----------------- variables ------------------------

ListOfVariables_general = [ 'fjpt','fjptsc','fjm', 'fjD2','mjpt','nmjpt','mjeta','nmjeta', 'mjmaxSd0', 'mjmeanSd0','mjsubSd0','mjthirdSd0', 'nmjmaxSd0', 'nmjmeanSd0','nmjsubSd0','nmjthirdSd0',  'fjpt_PREFITPOSTTAG','fjptsc_PREFITPOSTTAG', 'fjm_PREFITPOSTTAG', 'fjD2_PREFITPOSTTAG','mjpt_PREFITPOSTTAG','nmjpt_PREFITPOSTTAG','mjeta_PREFITPOSTTAG','nmjeta_PREFITPOSTTAG', 'mjmeanSd0_PREFITPOSTTAG', 'mjmaxSd0_PREFITPOSTTAG','mjsubSd0_PREFITPOSTTAG', 'mjthirdSd0_PREFITPOSTTAG','nmjmeanSd0_PREFITPOSTTAG', 'nmjmaxSd0_PREFITPOSTTAG', 'nmjsubSd0_PREFITPOSTTAG', 'nmjthirdSd0_PREFITPOSTTAG','fjtau21','fjtau21_PREFITPOSTTAG','mjpt_PREFITUNTAG','mjMV2c10','nmjMV2c10','mjMV2c10_PREFITPOSTTAG','nmjMV2c10_PREFITPOSTTAG','mjptfrac','nmjptfrac','mjptfracVsmjtrkmult','nmjptfracVsnmjtrkmult','hasleading2trackjets']

ListOfVariables_r21 = [ 'XbbScoreHiggs','XbbScoreTop','XbbScoreQCD','XbbScoreRatiof0','XbbScoreRatiofp2','XbbScoreHiggs_PREFITPOSTTAG','XbbScoreTop_PREFITPOSTTAG','XbbScoreQCD_PREFITPOSTTAG','XbbScoreRatiof0_PREFITPOSTTAG','XbbScoreRatiofp2_PREFITPOSTTAG']

ListOfVariables_r20p7 = []

ListOfVariables_minimal = [ 'mjmaxSd0', 'mjmeanSd0','mjsubSd0','mjthirdSd0', 'nmjmaxSd0', 'nmjmeanSd0','nmjsubSd0','nmjthirdSd0', 'mjmeanSd0_PREFITPOSTTAG', 'mjmaxSd0_PREFITPOSTTAG','mjsubSd0_PREFITPOSTTAG', 'mjthirdSd0_PREFITPOSTTAG','nmjmeanSd0_PREFITPOSTTAG', 'nmjmaxSd0_PREFITPOSTTAG', 'nmjsubSd0_PREFITPOSTTAG', 'nmjthirdSd0_PREFITPOSTTAG','mjpt_PREFITUNTAG']

# variables for which only the pt-inclusive plots are desired
ListOfVariables_inclusive = ['muE','mueta','muphi','mupt','muptrel','mud0','muz0','muz0sintheta','mjthirdSd0himu','mjthirdSd0medmu','mjthirdSd0lowmu','mjsubSd0himu','mjsubSd0medmu','mjsubSd0lowmu','mjmaxSd0himu','mjmaxSd0medmu','mjmaxSd0lowmu','mjmeanSd0himu','mjmeanSd0medmu','mjmeanSd0lowmu','nmjthirdSd0himu','nmjthirdSd0medmu','nmjthirdSd0lowmu','nmjsubSd0himu','nmjsubSd0medmu','nmjsubSd0lowmu','nmjmaxSd0himu','nmjmaxSd0medmu','nmjmaxSd0lowmu','nmjmeanSd0himu','nmjmeanSd0medmu','nmjmeanSd0lowmu']

#------------------ setup ---------------------------

#SetupStyle()

# geting input variables
parser = argparse.ArgumentParser(description='Make reweight histograms.')
parser.add_argument('outfile', help="Name of output ROOT file")
parser.add_argument('infiles', help="JSON file with paths for data and MC files. See README for format")
parser.add_argument('--tiny', help="Store only the histograms needed to run the fit",
                    action="store_true")
parser.add_argument('--nosys', help="Store only nominal histograms",
                    action="store_true")
parser.add_argument('--mcflag', default="comb", choices=["incl","mufilt","comb"],
                    help="Tell script how to make MC sample. Options are inclusive-only (incl), mu-filtered only (mufilt) or LL-inclusive (comb) [default: comb]")
parser.add_argument('--year', type=str, default="2015+2016",
    help="Year determines luminosity to normalize to [default: 2015+2016]")
args = parser.parse_args()

# setting variables
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
  Lumi += 44307.4 # in /pb
elif args.year == "2018" :
  Lumi += 58450.1 # in /pb

outfilename = args.outfile
pathData, ListOfMCPaths, ListOfInclusiveMCPaths, xsecFile = config.GetPathsFromJSON(args.infiles)

MyConfig = config.LoadGlobalConfig()
histHelper = config.HistHelper(xsecFile)

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
if args.mcflag == 'incl':
  print("Using inclusive samples for all flavours")
  ListOfInclusiveFlavourPairs = ListOfFlavourPairs
elif args.mcflag == 'mufilt':
  print("Using mu-filtered samples for all flavours")
  ListOfInclusiveFlavourPairs = []
else:
  print("Using inclusive samples for LL template only")
  ListOfInclusiveFlavourPairs = [ 'LL' ]

ListOfTJpt = MyConfig.GetTrkJetRegions()
ListOfTJpt.push_back(TString("Incl"))
isR20p7 = MyConfig.GetIsR20p7()

# setting variables

if args.tiny:
  ListOfVariables = ListOfVariables_minimal
else:
  ListOfVariables = ListOfVariables_general
  if isR20p7 :
    print("--- R20p7 Sample ---")
    ListOfVariables.extend(ListOfVariables_r20p7)
  else:
    print("--- R21 Sample ---")
    ListOfVariables.extend(ListOfVariables_r21)


print("List of Variables:")
for var in ListOfVariables :
  print(var)
print("--- end of varlist ---")

#----------- listing desired histograms -------------

# make list of data hists
ListOfDataHists = []
for tjpt in ListOfTJpt :
    for var in ListOfVariables :
        ListOfDataHists.append( MyConfig.GetDataHistName(tjpt,var).Data() )
if not args.tiny :
    for var in ListOfVariables_inclusive :
        ListOfDataHists.append( MyConfig.GetDataHistName("Incl",var).Data() )

ListOfDataHists.append('CutFlow_Nom');

# make list of MC hists
ListOfHists = []
ListOfHerwigHists = []
ListOfHists.append('CutFlow_Nom')
for flavour in ListOfFlavourPairs :
  if not args.tiny :
    for var in ListOfVariables_inclusive :
      ListOfHists.append( MyConfig.GetMCHistName("Nom","Incl",flavour,var).Data() )
  for tjpt in ListOfTJpt :
    for var in ListOfVariables :
      ListOfHerwigHists.append(MyConfig.GetMCHistName("Nom",tjpt,flavour,var).Data())
      for sys in ListOfSystematics :
        #TODO: don't hard-code these exceptions
        if ("fjeta" in var or "fjphi" in var or "PREFITUNTAG" in var) and "Nom" not in sys.Data():
          continue
        ListOfHists.append(MyConfig.GetMCHistName(sys,tjpt,flavour,var).Data())
      if "Sd0" in var :
        for sys in ListOfSd0Systematics :
          ListOfHists.append(MyConfig.GetMCHistName(sys,tjpt,flavour,var).Data())
      if "PREFITPOSTTAG" in var :
        for sys in ListOfWeightVariations :
          ListOfHists.append(MyConfig.GetMCHistName("Nom",tjpt,flavour,var+"_"+sys.Data()).Data())

#--------------------- output -----------------------

# open output file
outfile=ROOT.TFile(outfilename,"RECREATE")

# loop over and write MC histograms
for histname in ListOfHists :
  ListOfPaths = ListOfMCPaths
  for inclFlav in ListOfInclusiveFlavourPairs:
    if inclFlav in histname:
      ListOfPaths = ListOfInclusiveMCPaths
  histMC = histHelper.AddMCHists(histname,ListOfPaths)
  outfile.cd()
  if histMC:
    histMC.Scale(Lumi)
    if histname is 'CutFlow_Nom':
      histMC.SetName('CutFlow_MC')
    histMC.Write()
    print("Wrote "+histname)
  else:
    print("Could not find "+histname+" in all input files!")
    if 'PREFITPOSTTAG' in histname:
      help_name=histname.rsplit('_',1)[0]
      if 'BTAGUP' in histname or 'BTAGDOWN' in histname:
        help_name=histname.rsplit('_',2)[0]

      print("Writing "+help_name+" with 0 entries instead")
      path = ListOfPaths[0]
      file_curr = ROOT.TFile(path,"READ")
      if not file_curr:
        print("Cannot open file "+path)
        exit()
      hist_help = file_curr.Get(help_name)
      if hist_help:
        hist_default=ROOT.TH1D(histname,"",hist_help.GetNbinsX(),hist_help.GetXaxis().GetXmin(),hist_help.GetXaxis().GetXmax())
        hist_default.Write()
      else:
        print("Cannot find hist "+help_name+" in file "+path)

# loop over and write data histograms
file_curr = ROOT.TFile(pathData,"READ")
if not file_curr:
  print("Cannot open file "+pathData)
  exit()

for histname in ListOfDataHists :
  histData = file_curr.Get(histname)
  if histData:
    histData.SetDirectory(0)
    outfile.cd()
    if histname is 'CutFlow_Nom':
      histData.SetName('CutFlow_Data')
    histData.Write()
    print("Wrote "+histname)
  else:
    print("Cannot find hist "+histname+" in file "+pathData)
