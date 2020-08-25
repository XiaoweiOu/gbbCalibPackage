import argparse

#----------------- variables ------------------------

ListOfVariables_general = [
  'fjpt','fjm',
  'fjpt_2TAG','fjm_2TAG',
  'mjpt','nmjpt','mjeta','nmjeta',
  'mjpt_2TAG','nmjpt_2TAG','mjeta_2TAG','nmjeta_2TAG',
  #'fjD2','fjtau21','mjMV2c10','nmjMV2c10',
  #'fjD2_2TAG','fjtau21_2TAG','mjMV2c10_2TAG','nmjMV2c10_2TAG',
  #'mjptfrac','nmjptfrac','mjptfracVsmjtrkmult','nmjptfracVsnmjtrkmult','hasleading2trackjets',
  #'muSd0','muSd0BL',

  #'mjmaxSd0_pt','nmjmaxSd0_pt','mjmaxd0_pt','nmjmaxd0_pt','mjmaxd0err_pt','nmjmaxd0err_pt',
  #'mjsubSd0_pt','nmjsubSd0_pt','mjsubd0_pt','nmjsubd0_pt','mjsubd0err_pt','nmjsubd0err_pt',
  #'mjthirdSd0_pt','nmjthirdSd0_pt','mjthirdd0_pt','nmjthirdd0_pt','mjthirdd0err_pt','nmjthirdd0err_pt',
  #'mjmaxSd0_pt_2TAG','nmjmaxSd0_pt_2TAG',
  #'mjsubSd0_pt_2TAG','nmjsubSd0_pt_2TAG',
  #'mjthirdSd0_pt_2TAG','nmjthirdSd0_pt_2TAG',
  'mjmeanSd0','nmjmeanSd0',
  'mjmeanSd0_2TAG','nmjmeanSd0_2TAG',
  'mjmeanSd0_NOT2TAG','nmjmeanSd0_NOT2TAG',
  'mjpt_NOT2TAG',

  'mjmeanSd0_2TAG_2MUON','nmjmeanSd0_2TAG_2MUON',
  'mjmeanSd0_NOT2TAG_2MUON','nmjmeanSd0_NOT2TAG_2MUON',
  'mjmeanSd0_2TAG_1MUON','nmjmeanSd0_2TAG_1MUON',
  'mjmeanSd0_NOT2TAG_1MUON','nmjmeanSd0_NOT2TAG_1MUON'

  'mjmeanSd0_B0','mjmeanSd0_Bpm','mjmeanSd0_B_s','mjmeanSd0_Lambda_B','mjmeanSd0_Cascade_Bm','mjmeanSd0_Cascade_B0'
  ]

#ListOfVariables_general = ['mjmeanSd0_B0','mjmeanSd0_Bpm','mjmeanSd0_B_s','mjmeanSd0_Lambda_B','mjmeanSd0_Cascade_Bm','mjmeanSd0_Cascade_B0']

ListOfVariables_r21 = ['SubjetBScore_Higgs','SubjetBScore_Top','SubjetBScore_QCD','SubjetBScore_f0','SubjetBScore_f0p25','SubjetBScore_Higgs_2TAG','SubjetBScore_Top_2TAG','SubjetBScore_QCD_2TAG','SubjetBScore_f0_2TAG','SubjetBScore_f0p25_2TAG']

ListOfVariables_r20p7 = []

ListOfVariables_minimal = [
  #'j1meanSd0','j2meanSd0','j3meanSd0',
  #'j1meanSd0_2TAG','j2meanSd0_2TAG','j3meanSd0_2TAG',
  #'j1meanSd0_NOT2TAG','j2meanSd0_NOT2TAG','j3meanSd0_NOT2TAG',

  'j3meanSd0','j3meanSd0_2TAG','j3meanSd0_NOT2TAG',

  'fjpt',#'fjm',
  #'j1pt','j2pt','j3pt',
  'fjpt_NOT2TAG','fjpt_2TAG',
  #'fjm_NOT2TAG','fjm_2TAG',
  #'j1pt_NOT2TAG','j1pt_2TAG','j3pt_2TAG',
  #'j2pt_NOT2TAG','j2pt_NOT2TAG','j3pt_NOT2TAG',
  ]

# variables for which only the pt-inclusive plots are desired
ListOfVariables_inclusive = [
  'muE','mueta','muphi','mupt',
  'muptrel','mud0','muz0','muz0sintheta',
  'trkptVstrkd0','trkptVstrkz0sintheta'
  ]
#,'mjthirdSd0himu','mjthirdSd0medmu','mjthirdSd0lowmu','mjsubSd0himu','mjsubSd0medmu','mjsubSd0lowmu','mjmaxSd0himu','mjmaxSd0medmu','mjmaxSd0lowmu','mjmeanSd0himu','mjmeanSd0medmu','mjmeanSd0lowmu','nmjthirdSd0himu','nmjthirdSd0medmu','nmjthirdSd0lowmu','nmjsubSd0himu','nmjsubSd0medmu','nmjsubSd0lowmu','nmjmaxSd0himu','nmjmaxSd0medmu','nmjmaxSd0lowmu','nmjmeanSd0himu','nmjmeanSd0medmu','nmjmeanSd0lowmu']

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
parser.add_argument('--year', type=str, default="15+16",
    help="Year determines luminosity to normalize to [default: 15+16]")
args = parser.parse_args()

import ROOT
#from PlotFunctions import *
#from TAxisFunctions import *
import ConfigFunctions as config

ROOT.gROOT.SetBatch(True)
from ROOT import TCanvas,TPad,TString

# setting variables
Lumi = config.GetLumi(args.year)
print('Lumi is {}'.format(Lumi))

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

ListOfFatJetFlavours = MyConfig.GetFatJetFlavours()

'''
for flav in ListOfFatJetFlavours:
  print flav
exit(0)
ListOfFatJetFlavours = [TString('BB'),TString('BL')]
'''

if args.mcflag == 'incl':
  print("Using inclusive samples for all flavours")
  ListOfInclusiveFatJetFlavours = ListOfFatJetFlavours
elif args.mcflag == 'mufilt':
  print("Using mu-filtered samples for all flavours")
  ListOfInclusiveFatJetFlavours = []
else:
  print("Using inclusive samples for LL template only")
  ListOfInclusiveFatJetFlavours = [ TString('Lxx'), TString('LLx'), TString('LLL') ]

ListOfPtBins = ['Incl']
for ptBin in MyConfig.GetDiTrkJetRegions():
  ListOfPtBins.append(ptBin.Data())
for ptBin in MyConfig.GetFatJetRegions():
  ListOfPtBins.append(ptBin.Data())

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
for ptBin in ListOfPtBins :
    for var in ListOfVariables :
        ListOfDataHists.append( MyConfig.GetDataHistName(ptBin,var).Data() )
if not args.tiny :
    for var in ListOfVariables_inclusive :
        ListOfDataHists.append( MyConfig.GetDataHistName("Incl",var).Data() )

ListOfDataHists.append('CutFlow_Nom');

# make list of MC hists
ListOfHists = ['CutFlow_Nom']
ListOfNomOnlyHists = []
#ListOfHerwigHists = []
for flavour in ListOfFatJetFlavours :
  if not args.tiny :
    for var in ListOfVariables_inclusive :
      ListOfNomOnlyHists.append( MyConfig.GetMCHistName("Nom","Incl",flavour,var).Data() )
  for ptBin in ListOfPtBins :
    for var in ListOfVariables :
      #ListOfHerwigHists.append(MyConfig.GetMCHistName("Nom",ptBin,flavour,var).Data())
      ListOfHists.append(MyConfig.GetMCHistName("Nom",ptBin,flavour,var).Data())
      if "Sd0" in var :
        for sys in ListOfSd0Systematics :
          ListOfNomOnlyHists.append(MyConfig.GetMCHistName(sys,ptBin,flavour,var).Data())
      if "2TAG" in var :
        for sys in ListOfWeightVariations :
          ListOfNomOnlyHists.append(MyConfig.GetMCHistName("Nom",ptBin,flavour,var+"_"+sys.Data()).Data())

def WriteMCHist(histname, syspath):
  ListOfPaths = ListOfMCPaths[syspath]
  for inclFlav in ListOfInclusiveFatJetFlavours:
    if inclFlav.Data() in histname:
      ListOfPaths = ListOfInclusiveMCPaths[syspath]
  histMC = histHelper.AddMCHists(histname,ListOfPaths)
  outfile.cd()
  if histMC:
    histMC.Scale(Lumi)
    if histname is 'CutFlow_Nom':
      histMC.SetName('CutFlow_MC')
    histMC.Write()
    #print("Wrote "+histname)
  else:
    print("Could not find "+histname+" in all input files!")

#--------------------- output -----------------------

# open output file
outfile=ROOT.TFile(outfilename,"RECREATE")

# loop over and write MC histograms
for sys in ListOfSystematics:
  histHelper.ClearFileMap()
  syspath = sys.Data()+"/"
  if "Nom" in sys.Data():
    syspath = "Nominal/"

    for histname in ListOfNomOnlyHists :
      WriteMCHist(histname, syspath)

  for histname in ListOfHists :
    if not "Nom" in sys.Data():
      #FIXME need solution that doesn't rely on lack of extra 'Nom's in histname
      histname = histname.replace("Nom",sys.Data())

    WriteMCHist(histname, syspath)

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
    #exit()

file_curr.Close()
outfile.Close()
