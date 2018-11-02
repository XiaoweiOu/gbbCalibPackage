

import ROOT
import sys
import math
import string
#from PlotFunctions import *
#from TAxisFunctions import *
import os

ROOT.gROOT.SetBatch(True)
from ROOT import TCanvas,TPad

#SetupStyle()

outfilename = sys.argv[1]

Lumi = 36000.0; #in pb^-1

#Pythia
ListOfCrossSections = [2.6454e7,254620.,4553.5,257.54,16.215] #in pb
ListOfFilterEfficiencies=[1.6117e-5,3.5545e-5,7.1955e-5,7.6347e-5,3.1782e-5]

#Herwig
#ListOfCrossSections = [1.8831e7,173300.,2965.4,162.76,10.113] #in pb
#ListOfFilterEfficiencies = [1.9120e-05,4.2602e-5,9.0537e-5,0.00010259,4.7654e-5] #UPDATE for JZ3W 

basepath = '/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/'

pathJZ3W = basepath + 'Output_TrigTurnOn/TrigTurnOn_user.ruth.mc15_13TeV.427003.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root/merged_hist_TrigTurnOn_user.ruth.mc15_13TeV.427003.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root' 
pathJZ4W = basepath + 'Output_TrigTurnOn/TrigTurnOn_user.ruth.mc15_13TeV.427004.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root/merged_hist_TrigTurnOn_user.ruth.mc15_13TeV.427004.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root' 
pathJZ5W = basepath + 'Output_TrigTurnOn/TrigTurnOn_user.ruth.mc15_13TeV.427005.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root/merged_hist_TrigTurnOn_user.ruth.mc15_13TeV.427005.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root' 
pathJZ6W = basepath + 'Output_TrigTurnOn/TrigTurnOn_user.ruth.mc15_13TeV.427106.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5839_s2726_r7773_r7676_p2949_tuple.root/merged_hist_TrigTurnOn_user.ruth.mc15_13TeV.427106.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5839_s2726_r7773_r7676_p2949_tuple.root' 
pathJZ7W = basepath + 'Output_TrigTurnOn/TrigTurnOn_user.ruth.mc15_13TeV.427107.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5839_s2726_r7773_r7676_p2949_tuple.root/merged_hist_TrigTurnOn_user.ruth.mc15_13TeV.427107.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5839_s2726_r7773_r7676_p2949_tuple.root' 

pathData = basepath + 'Output_TrigTurnOn/user.ruth.data15_13TeV.all.root/merged_hist_user.ruth.data15_13TeV.all.root'

ListOfMCPaths = [ pathJZ3W, pathJZ4W, pathJZ5W, pathJZ6W, pathJZ7W ]

ListOfTriggers = [ 'HLT85', 'HLT110', 'HLT150', 'HLT175', 'HLT260', 'HLT360', 'HLT380' ]

ListOfPrescales = [ 7.378e-5, 5.479e-4, 7.633e-4, 3.945e-3, 2.453e-2, 2.514e-1, 1.0]

BaseHistName = 'trigjet_pt_noHLTreq'

#make list of histograms in MC
ListOfHists = []
for trig in ListOfTriggers :
    ListOfHists.append('trigjet_pt_pass'+trig)
    print('trigjet_pt_pass'+trig)

#open output file
outfile=ROOT.TFile(outfilename,"RECREATE")

#loop over MC histograms
hist_index=0
for histname in ListOfHists :
    index=0
    for path in ListOfMCPaths :
        
        file_curr=ROOT.TFile(path,"READ")
        print("Opened file"+path)
        
        bookkeep_hist=file_curr.Get("Hist_BookKeeping") #Events in AOD is in Bin 3
        weight=ListOfCrossSections[index]*ListOfFilterEfficiencies[index]/bookkeep_hist.GetBinContent(3)*Lumi
        
        if index is 0 :
            hist_0=file_curr.Get(histname)
            hist_0.Scale(weight)
            hist_0.SetDirectory(0)

            if hist_index is 0:
                hist_base=file_curr.Get(BaseHistName)
                hist_base.Scale(weight)
                hist_base.SetDirectory(0)
    
        else :
            if file_curr.Get(histname) == 0 :
                print("Could not find "+histname)
            hist_tmp=file_curr.Get(histname)
            hist_0.Add(hist_tmp,weight)
           
            if hist_index is 0:
                hist_tmp2=file_curr.Get(BaseHistName)
                hist_base.Add(hist_tmp2,weight)

        if index==len(ListOfMCPaths)-1 :
            hist_0.Divide(hist_base)
            hist_0.SetName('trig_turnon_'+ListOfTriggers[hist_index])
            hist_0.SetMarkerStyle(20)
            outfile.cd()
            hist_0.Write()
            
            for i in range(1, hist_0.GetSize()-2):
                if hist_0.GetBinContent(i) >= 0.99:
                    print(ListOfTriggers[hist_index]+" fully efficient at pT="+str(hist_0.GetBinCenter(i)))
                    break
        index += 1

    hist_index += 1
    

