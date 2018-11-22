import ROOT
import sys
import math
import string
#from PlotFunctions import *
#from TAxisFunctions import *
import os

ROOT.gROOT.SetBatch(True)
from ROOT import TCanvas,TPad,TH1,TFile

#SetupStyle()

Lumi = 36000.0; #in pb^-1

#Pythia
ListOfCrossSections = [2.6454e7,254620.,4553.5,257.54,16.215] #in pb
ListOfFilterEfficiencies=[1.6117e-5,3.5545e-5,7.1955e-5,7.6347e-5,3.1782e-5]

#Herwig
#ListOfCrossSections = [1.8831e7,173300.,2965.4,162.76,10.113] #in pb
#ListOfFilterEfficiencies = [1.9120e-05,4.2602e-5,9.0537e-5,0.00010259,4.7654e-5] #UPDATE for JZ3W 

basepath = '/data/users/aemerman/gbbCalibPackage/arcond/'

pathJZ3W = basepath + 'mc16_13TeV.427003.FTAG1.FTNtupCalib.v00-05-02VRa.Calib.root'
pathJZ4W = basepath + 'mc16_13TeV.427004.FTAG1.FTNtupCalib.v00-05-02VRa.Calib.root'
pathJZ5W = basepath + 'mc16_13TeV.427005.FTAG1.FTNtupCalib.v00-05-02VRa.Calib.root'
pathJZ6W = basepath + 'mc16_13TeV.427106.FTAG1.FTNtupCalib.v00-05-02VRa.Calib.root'
pathJZ7W = basepath + 'mc16_13TeV.427107.FTAG1.FTNtupCalib.v00-05-02VRa.Calib.root'
ListOfMCPaths = [ pathJZ3W, pathJZ4W, pathJZ5W, pathJZ6W, pathJZ7W ]

pathData = basepath + 'data16_13TeV.FTAG1.FTNtupCalib.v00-05-02VRa.Template.root'

#ListOfVariables = [ 'fjpt', 'nmjpt', 'mjpt', 'mjmaxSd0', 'nmjmaxSd0' ]
ListOfVariables = [ 'fjpt', 'mjmaxSd0', 'nmjmaxSd0', 'fjpt_PREFITPOSTTAG', 'mjmaxSd0_PREFITPOSTTAG', 'nmjmaxSd0_PREFITPOSTTAG' ]
  

ListOfSystematics = [ 'Nom' ]

ListOfFlavourPairs = [ 'BB', 'BL', 'CC', 'CL', 'LL' ]

ListOfmjpt=[100,200]
ListOfnmjpt=[20,50,80]

#mjpt_l100_nmjpt_l20
ListOfTJpt=['mjpt_l100_nmjpt_l20','mjpt_l100_nmjpt_g20l50','mjpt_l100_nmjpt_g50l80', 'mjpt_l100_nmjpt_g80',
            'mjpt_g100l200_nmjpt_l20','mjpt_g100l200_nmjpt_g20l50','mjpt_g100l200_nmjpt_g50l80','mjpt_g100l200_nmjpt_g80',
            'mjpt_g200_nmjpt_l20','mjpt_g200_nmjpt_g20l50','mjpt_g200_nmjpt_g50l80', 'mjpt_g200_nmjpt_g80']

outfilename = 'testinputs.root'

#make list of histograms in MC
ListOfHists = []
for flavour in ListOfFlavourPairs :
    for sys in ListOfSystematics :
        for tjpt in ListOfTJpt :
            for var in ListOfVariables :
                ListOfHists.append('h'+flavour+sys+'_'+tjpt+'_'+var)
                print('h'+flavour+sys+'_'+tjpt+'_'+var)
                
#make list of histograms in Data
ListOfDataHists = []
for tjpt in ListOfTJpt :
    for var in ListOfVariables :
        ListOfDataHists.append('hDataNom_'+tjpt+'_'+var)
        print('hDataNom_'+tjpt+'_'+var)

#open output file
outfile=ROOT.TFile(outfilename,"RECREATE")

#loop over MC histograms
for histname in ListOfHists :
    hist_index=ListOfHists.index(histname)
    for path in ListOfMCPaths :
        index=ListOfMCPaths.index(path)
        file_curr=ROOT.TFile(path,"READ")

        bookkeep_hist=file_curr.Get("Hist_BookKeeping") #Events in AOD is in Bin 3
        weight=ListOfCrossSections[index]*ListOfFilterEfficiencies[index]/bookkeep_hist.GetBinContent(3)*Lumi
        
        if not file_curr.GetListOfKeys().Contains(histname):
            print("Cannot find hist "+histname)
        elif index is 0 :
            hist_0=file_curr.Get(histname)
            hist_0.SetDirectory(0)
            hist_0.Scale(weight)
        elif index is not 0 :
            hist_tmp=file_curr.Get(histname)
            hist_0.Add(hist_tmp,weight)

        if index==len(ListOfMCPaths)-1 :
            outfile.cd()
            #new_hist_0=(ROOT.TH1F)hist_0
            new_hist_0=hist_0.Clone()
            new_hist_0.Write()
        
#loop over Data Histograms
file_curr=ROOT.TFile(pathData,"READ")

for histname in ListOfDataHists :
    hist_0=file_curr.Get(histname)
    hist_0.SetDirectory(0)
    #new_hist_0=(ROOT.TH1F)hist_0
    new_hist_0=hist_0.Clone()
    outfile.cd()
    new_hist_0.Write()
    
    
    
