import ROOT
import sys
import math
import string
#from PlotFunctions import *
#from TAxisFunctions import *
import ConfigFunctions as config
import os
import argparse

ROOT.gROOT.SetBatch(True)
from ROOT import TCanvas,TPad,TString

#SetupStyle()

Lumi = 36000.0; #in pb^-1

parser = argparse.ArgumentParser(description='Make reweight histograms.')
parser.add_argument('outfile', help="Name of output ROOT file")
parser.add_argument('infiles', help="JSON file with paths for data and MC files. See README for format")
args = parser.parse_args()

outfilename = args.outfile
pathData, ListOfMCPaths, ListOfInclusiveMCPaths = config.GetPathsFromJSON(args.infiles)

MyConfig = config.LoadGlobalConfig()
histHelper = config.HistHelper()

ListOfSystematics = [ ROOT.TString("Nom") ] #MyConfig.GetSystematics() 
ListOfFlavourPairs = MyConfig.GetFlavourPairs()
ListOfInclusiveFlavourPairs = [ 'LL' ]
ListOfTJpt = MyConfig.GetTrkJetRegions()

#TODO: Add paths for Herwig samples?

#ListOfVariables = [ 'fjpt', 'nmjpt', 'mjpt', 'mjmaxSd0', 'nmjmaxSd0' ]
ListOfVariables = [ 'fjpt','fjptsc','fjm', 'fjD2','mjpt','nmjpt','mjeta','nmjeta', 'mjmaxSd0', 'nmjmaxSd0', 'fjpt_PREFITPOSTTAG','fjptsc_PREFITPOSTTAG', 'fjm_PREFITPOSTTAG', 'fjD2_PREFITPOSTTAG','mjpt_PREFITPOSTTAG','nmjpt_PREFITPOSTTAG','mjeta_PREFITPOSTTAG','nmjeta_PREFITPOSTTAG', 'mjmaxSd0_PREFITPOSTTAG', 'nmjmaxSd0_PREFITPOSTTAG','fjpt_PREFITPOSTTAG_BTAGUP', 'fjm_PREFITPOSTTAG_BTAGUP', 'fjD2_PREFITPOSTTAG_BTAGUP','mjpt_PREFITPOSTTAG_BTAGUP','nmjpt_PREFITPOSTTAG_BTAGUP','mjeta_PREFITPOSTTAG_BTAGUP','nmjeta_PREFITPOSTTAG_BTAGUP','fjptsc_PREFITPOSTTAG_BTAGUP', 'fjpt_PREFITPOSTTAG_BTAGDOWN', 'fjm_PREFITPOSTTAG_BTAGDOWN', 'fjD2_PREFITPOSTTAG_BTAGDOWN','mjpt_PREFITPOSTTAG_BTAGDOWN','nmjpt_PREFITPOSTTAG_BTAGDOWN','mjeta_PREFITPOSTTAG_BTAGDOWN','nmjeta_PREFITPOSTTAG_BTAGDOWN','fjptsc_PREFITPOSTTAG_BTAGDOWN','trjpt','fjtau21','fjtau21_PREFITPOSTTAG','fjtau21_PREFITPOSTTAG_BTAGUP','fjtau21_PREFITPOSTTAG_BTAGDOWN']#,'fjpt_PREFITANTITAG', 'allsrjpt' , 'srjN','evemu','slR4jpt','trjptfjptratio','trjptgbbcandratio' ]

#make list of histograms in Data
ListOfDataHists = []
for tjpt in ListOfTJpt :
    for var in ListOfVariables :
       ListOfDataHists.append( MyConfig.GetDataHistName(tjpt,var).Data() )

ListOfDataHists.append('CutFlow_Nom');

#make list of histograms in MC
ListOfHists = []
ListOfHerwigHists = []
for sys in ListOfSystematics :
  print("Getting Hists for Systematic Variation: "+sys.Data())
  if sys.Data() is 'Nom':
    ListOfHists.append('CutFlow_Nom')

  for flavour in ListOfFlavourPairs :
    for tjpt in ListOfTJpt :
      for var in ListOfVariables :
#TODO: don't hard-code these exceptions
        if "BTAG" in var and "Nom" not in sys.Data():
          continue
        if "SD0_Smear" in var and "Nom" not in sys.Data():
          continue
        if "fjeta" in var and "fjphi" in var and "Nom" not in sys.Data():
          continue
        if sys.Data() is "Nom" and "maxSd0" in var :
          ListOfHists.append(MyConfig.GetMCHistName("SD0Smear__1up",tjpt,flavour,var).Data())
          ListOfHists.append(MyConfig.GetMCHistName("SD0Smear__1down",tjpt,flavour,var).Data())
          ListOfHists.append(MyConfig.GetMCHistName("Conversion__1down",tjpt,flavour,var).Data())
          ListOfHists.append(MyConfig.GetMCHistName("Conversion__1up",tjpt,flavour,var).Data())
          ListOfHists.append(MyConfig.GetMCHistName("HadMatInt__1up",tjpt,flavour,var).Data())
          ListOfHists.append(MyConfig.GetMCHistName("HadMatInt__1down",tjpt,flavour,var).Data())
          ListOfHists.append(MyConfig.GetMCHistName("LightLongLived__1up",tjpt,flavour,var).Data())
          ListOfHists.append(MyConfig.GetMCHistName("LightLongLived__1down",tjpt,flavour,var).Data())

        ListOfHists.append(MyConfig.GetMCHistName(sys,tjpt,flavour,var).Data())
        if sys.Data() is "Nom":
          ListOfHerwigHists.append(MyConfig.GetMCHistName(sys,tjpt,flavour,var).Data())
 

#open output file
outfile=ROOT.TFile(outfilename,"RECREATE")

#loop over MC histograms
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

#loop over data Histograms
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
  else:
    print("Cannot find hist "+histname+" in file "+pathData)

#TODO
##get Herwig histograms
#doHerwig=1
#if doHerwig>0:
#    print("Getting Herwig Histograms.")
#    for histname in ListOfHerwigHists :
#        hist_index=ListOfHerwigHists.index(histname)
#        hist_0=None
#        for path in ListOfHerwigMCPaths :
#            index=ListOfHerwigMCPaths.index(path)
#            file_curr=ROOT.TFile(path,"READ")
#            if not file_curr :
#                print("Cannot find file: "+path)
#            bookkeep_hist=file_curr.Get("Hist_BookKeeping") #Events in AOD is in Bin 3
#            if not bookkeep_hist :
#                print("Cannot find BookKeeping Histogram in path" + path)
#                break
#                continue
#            weight=ListOfHerwigCrossSections[index]*ListOfHerwigFilterEfficiencies[index]/bookkeep_hist.GetBinContent(3)*Lumi
#            #if hist_index==0:
#                #print("weight is",weight)
#            
#            if not file_curr.GetListOfKeys().Contains(histname):
#                #print("Cannot find hist "+histname)
#                if index is 0: 
#                    hist_0=None
#                
#            elif index is 0 or hist_0 is None :
#                hist_0=file_curr.Get(histname)
#                hist_0.SetDirectory(0)
#                hist_0.Scale(weight)
#            elif index is not 0 :
#                hist_tmp=file_curr.Get(histname)
#                hist_0.Add(hist_tmp,weight)
#
#            if index==len(ListOfHerwigMCPaths)-1 :
#                outfile.cd()
#                if hist_0 is None :
#                    print("ERROR: Could not find "+histname+" in all input Files!")
#                    if 'PREFITPOSTTAG' in histname:
#                        help_name=histname.rsplit('_',1)[0]
#                        if 'BTAGUP' in histname or 'BTAGDOWN' in histname:
#                            help_name=histname.rsplit('_',2)[0]
#
#                        hist_help=file_curr.Get(help_name)
#                        print("Writing "+help_name+" with 0 entries instead")
#                        hist_default=ROOT.TH1D(histname.replace("Nom","Herwig__1up",1),"",hist_help.GetNbinsX(),hist_help.GetXaxis().GetXmin(),hist_help.GetXaxis().GetXmax())
#                        hist_default.Write()
#
#                if hist_0 is not None:
#                    hist_0.SetName(histname.replace("Nom","Herwig__1up",1))
#                    hist_0.Write()
#    
#    
#    #loop over inclusive MC for LL histograms
#    for histname in ListOfLLHerwigHists :
#        hist_index=ListOfLLHerwigHists.index(histname)
#        hist_0=None
#        for path in ListOfHerwigInclusiveMCPaths :
#            index=ListOfHerwigInclusiveMCPaths.index(path)
#            file_curr=ROOT.TFile(path,"READ")
#
#            bookkeep_hist=file_curr.Get("Hist_BookKeeping") #Events in AOD is in Bin 3
#            if not bookkeep_hist :
#                print("Cannot find BookKeeping Histogram in path" + path)
#                break
#                #continue
#            weight=ListOfHerwigInclusiveCrossSections[index]*ListOfHerwigInclusiveFilterEfficiencies[index]/bookkeep_hist.GetBinContent(3)*Lumi
#           # if hist_index==0:
#                #print("inclusive weight is",weight)
#        
#
#            if not file_curr.GetListOfKeys().Contains(histname):
#                #print("Cannot find hist "+histname)
#                if index is 0:
#                    hist_0=None
#            elif index is 0 or hist_0 is None :
#                hist_0=file_curr.Get(histname)
#                hist_0.SetDirectory(0)
#                hist_0.Scale(weight)
#                #print('got '+hist_0.GetName())
#            elif index is not 0 :
#                hist_tmp=file_curr.Get(histname)
#                hist_0.Add(hist_tmp,weight)
#                #print('adding '+hist_tmp.GetName())
#
#            if index==len(ListOfHerwigInclusiveMCPaths)-1 :
#                outfile.cd()
#                if hist_0 is None :
#                    print("ERROR: Could not find "+histname+" in all input Files!")
#                    if 'PREFITPOSTTAG' in histname:
#                        help_name=histname.rsplit('_',1)[0]
#                        if 'BTAGUP' in histname or 'BTAGDOWN' in histname:
#                            help_name=histname.rsplit('_',2)[0]    
#                        hist_help=file_curr.Get(help_name)
#                        
#                        print("Writing "+help_name+" with 0 entries instead")
#                        hist_default=ROOT.TH1D(histname.replace("Nom","Herwig__1up",1),"",hist_help.GetNbinsX(),hist_help.GetXaxis().GetXmin(),hist_help.GetXaxis().GetXmax())
#                        hist_default.Write()
#
#                if hist_0 is not None:
#                    hist_0.SetName(histname.replace("Nom","Herwig__1up",1))
#                    hist_0.Write()
