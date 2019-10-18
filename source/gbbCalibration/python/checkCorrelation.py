import ROOT
#from PlotFunctions import *
#from TAxisFunctions import *
import ConfigFunctions as config
import argparse
import os

ROOT.gROOT.SetBatch(True)
from ROOT import TCanvas,TPad,TString


#----------------- variables ------------------------

ListOfVariables = [ 'mjmaxSd0', 'mjmeanSd0','mjsubSd0','mjthirdSd0', 'nmjmaxSd0', 'nmjmeanSd0','nmjsubSd0','nmjthirdSd0', 'mjmeanSd0_2TAG', 'mjmaxSd0_2TAG','mjsubSd0_2TAG', 'mjthirdSd0_2TAG','nmjmeanSd0_2TAG', 'nmjmaxSd0_2TAG', 'nmjsubSd0_2TAG', 'nmjthirdSd0_2TAG']

ListOfVariablePairs = [ 'mjmaxSd0_nmjmaxSd0','mjsubSd0_nmjsubSd0','mjthirdSd0_nmjthirdSd0','mjmeanSd0_nmjmeanSd0','mjmaxSd0_mjsubSd0','mjmaxSd0_mjthirdSd0','mjsubSd0_mjthirdSd0','nmjmaxSd0_nmjsubSd0','nmjmaxSd0_nmjthirdSd0','nmjsubSd0_nmjthirdSd0']

ListOfVariablePairs_posttag = []

for var in ListOfVariablePairs:
  posttag_var = var+'_2TAG'
  ListOfVariablePairs_posttag.append(posttag_var)
ListOfVariablePairs+=ListOfVariablePairs_posttag

#------------------ setup ---------------------------

#SetupStyle()

Lumi = 36000.0; #in pb^-1

# geting input variables
parser = argparse.ArgumentParser(description='Check correlation between fitting variables.')
parser.add_argument('--outputdir',default='',help="Directory to store output plots")
parser.add_argument('--outfile', default='', help="Name of output pdf")
parser.add_argument('--infiles', help="JSON file with paths for data and MC files. See README for format")
parser.add_argument('--nosys' ,default=False, help="Store only nominal histograms",
                    action="store_true")
parser.add_argument('--useIncl', type=int, default=1,
    help="specify which templates should be from the inclusive sample. 0=none, 1=LL only, 2=all [default: 1]")
args = parser.parse_args()

# setting variables
outputdir = args.outputdir
outfileprefix = args.outfile
pathData, ListOfMCPaths, ListOfInclusiveMCPaths, xsecFile = config.GetPathsFromJSON(args.infiles)

os.system('mkdir -p '+outputdir)

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
if (args.useIncl == 0):
  ListOfInclusiveFlavourPairs = [ ]
elif (args.useIncl == 1):
  ListOfInclusiveFlavourPairs = [ 'LL' ]
elif (args.useIncl == 2):
  ListOfInclusiveFlavourPairs = [ ]
  for flav in ListOfFlavourPairs:
    ListOfInclusiveFlavourPairs.append(flav.Data())

ListOfTJpt = MyConfig.GetTrkJetRegions()
ListOfTJpt.push_back(TString("Incl"))
isR20p7 = MyConfig.GetIsR20p7()

# setting variables

print("Checking correlation of variable pairs:")
for var in ListOfVariablePairs :
  print(var)
print("--- end of varlist ---")

#----------- listing desired histograms -------------

# make list of data hists
ListOfDataHists = []
MapHistToVar = {}
for tjpt in ListOfTJpt :
    for var in ListOfVariablePairs :
       ListOfDataHists.append( MyConfig.GetDataHistName(tjpt,var).Data() )
       MapHistToVar[MyConfig.GetDataHistName(tjpt,var).Data()] = tjpt.Data()+'_'+var

# make list of MC hists
#ListOfHists = []
#ListOfHerwigHists = []
#ListOfHists.append('CutFlow_Nom')
#for flavour in ListOfFlavourPairs :
#  for tjpt in ListOfTJpt :
#    for var in ListOfVariables :
#      ListOfHerwigHists.append(MyConfig.GetMCHistName("Nom",tjpt,flavour,var).Data())
#      for sys in ListOfSystematics :
#        #TODO: don't hard-code these exceptions
#        if ("fjeta" in var or "fjphi" in var or "NOT2TAG" in var) and "Nom" not in sys.Data():
#          continue
#        ListOfHists.append(MyConfig.GetMCHistName(sys,tjpt,flavour,var).Data())
#      if "Sd0" in var :
#        for sys in ListOfSd0Systematics :
#          ListOfHists.append(MyConfig.GetMCHistName(sys,tjpt,flavour,var).Data())
#      if "2TAG" in var :
#        for sys in ListOfWeightVariations :
#          ListOfHists.append(MyConfig.GetMCHistName("Nom",tjpt,flavour,var+"_"+sys.Data()).Data())

#--------------------- output -----------------------

# loop over and write MC histograms
#for histname in ListOfHists :
#  ListOfPaths = ListOfMCPaths
#  for inclFlav in ListOfInclusiveFlavourPairs:
#    if inclFlav in histname:
#      ListOfPaths = ListOfInclusiveMCPaths
#  histMC = histHelper.AddMCHists(histname,ListOfPaths)
#  outfile.cd()
#  if histMC:
#    histMC.Scale(Lumi)
#    if histname is 'CutFlow_Nom':
#      histMC.SetName('CutFlow_MC')
#    histMC.Write()
#    print("Wrote "+histname)
#  else:
#    print("Could not find "+histname+" in all input files!")
#    if '2TAG' in histname:
#      help_name=histname.rsplit('_',1)[0]
#      if 'BTAGUP' in histname or 'BTAGDOWN' in histname:
#        help_name=histname.rsplit('_',2)[0]
#
#      print("Writing "+help_name+" with 0 entries instead")
#      path = ListOfPaths[0]
#      file_curr = ROOT.TFile(path,"READ")
#      if not file_curr:
#        print("Cannot open file "+path)
#        exit()
#      hist_help = file_curr.Get(help_name)
#      if hist_help:
#        hist_default=ROOT.TH1D(histname,"",hist_help.GetNbinsX(),hist_help.GetXaxis().GetXmin(),hist_help.GetXaxis().GetXmax())
#        hist_default.Write()
#      else:
#        print("Cannot find hist "+help_name+" in file "+path)

# loop over and write data histograms
file_curr = ROOT.TFile(pathData,"READ")
if not file_curr:
  print("Cannot open file "+pathData)
  exit()

for histname in ListOfDataHists :
  histData = file_curr.Get(histname)
  if histData:
    histData.SetDirectory(0)
    corr_factor = histData.GetCorrelationFactor(1,2)
    corr_string = "Correlation factor "+str(round(corr_factor,2)) 
    text = ROOT.TText(-30,70,corr_string)
    text.SetTextColor(2)
    c1 = TCanvas()
    c1.cd()
#    ROOT.TText.DrawText(.8,.8,"Correlation factor = {0}".format(str(round(corr_factor,2)))
    histData.Draw()
    text.Draw('SAME')
    c1.SaveAs(outputdir+"/"+outfileprefix+MapHistToVar[histname]+".png") 
  else:
    print("Cannot find hist "+histname+" in file "+pathData)

#--------------------- output -----------------------
