import ROOT
import sys
import math
import string
#from PlotFunctions import *
#from TAxisFunctions import *
import os

ROOT.gROOT.SetBatch(True)
from ROOT import TCanvas,TPad,TH2

#SetupStyle()

outfilename = sys.argv[1]

Lumi = 36000.0 #in pb^-1

#Pythia
ListOfCrossSections = [2.6454e7,254620.,4553.5,257.54,16.215] #in pb
ListOfFilterEfficiencies=[1.6117e-5,3.5545e-5,7.1955e-5,7.6347e-5,3.1782e-5]

#Herwig
#ListOfCrossSections = [1.8831e7,173300.,2965.4,162.76,10.113] #in pb
#ListOfFilterEfficiencies = [1.9120e-05,4.2602e-5,9.0537e-5,0.00010259,4.7654e-5] #UPDATE for JZ3W 

basepath = '/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/'

pathJZ3W = basepath + 'Output_Reweight/Reweight_user.ruth.mc15_13TeV.427003.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root/merged_hist_Reweight_user.ruth.mc15_13TeV.427003.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root' 
pathJZ4W = basepath + 'Output_Reweight/Reweight_user.ruth.mc15_13TeV.427004.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root/merged_hist_Reweight_user.ruth.mc15_13TeV.427004.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root' 
pathJZ5W = basepath + 'Output_Reweight/Reweight_user.ruth.mc15_13TeV.427005.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root/merged_hist_Reweight_user.ruth.mc15_13TeV.427005.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root' 
pathJZ6W = basepath + 'Output_Reweight/Reweight_user.ruth.mc15_13TeV.427106.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5839_s2726_r7773_r7676_p2949_tuple.root/merged_hist_Reweight_user.ruth.mc15_13TeV.427106.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5839_s2726_r7773_r7676_p2949_tuple.root' 
pathJZ7W = basepath + 'Output_Reweight/Reweight_user.ruth.mc15_13TeV.427107.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5839_s2726_r7773_r7676_p2949_tuple.root/merged_hist_Reweight_user.ruth.mc15_13TeV.427107.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5839_s2726_r7773_r7676_p2949_tuple.root' 

pathData = basepath + 'Output_Reweight/user.ruth.data1516_13TeV.all.root/merged_hist_user.ruth.data1516_13TeV.all.root'

ListOfMCPaths = [ pathJZ3W, pathJZ4W, pathJZ5W, pathJZ6W, pathJZ7W ]

ListOfTriggers = [ 'HLT_j150', 'HLT_j175', 'HLT_j260', 'HLT_j380' ]

#make list of histograms
ListOfHists = []
for trig in ListOfTriggers :
    ListOfHists.append('reweight_trigjet_pt_eta_'+trig)

#open output file
outfile=ROOT.TFile(outfilename,"RECREATE")

#loop over MC histograms
for histname in ListOfHists :
    index=0
    for path in ListOfMCPaths :
        
        file_curr=ROOT.TFile(path,"READ")
        print("Open file "+path)

        bookkeep_hist=file_curr.Get("Hist_BookKeeping") #Events in AOD is in Bin 3
        weight=ListOfCrossSections[index]*ListOfFilterEfficiencies[index]/bookkeep_hist.GetBinContent(3)*Lumi
        print("weight is: "+str(weight))

        if index is 0 :
            hist_0=None
            if not file_curr.GetListOfKeys().Contains(histname) :
                 print("Cannot find first hist "+histname)
            else :
                hist_0=file_curr.Get(histname)
                hist_0.SetDirectory(0)
                hist_0.Scale(weight)
                print("found hist "+str(hist_0))

        else :
            if file_curr.GetListOfKeys().Contains(histname) :
                hist_tmp=file_curr.Get(histname)               

                if hist_0 is None:
                    hist_tmp.SetDirectory(0)
                    hist_0=hist_tmp
                else:
                    print("found hist "+str(hist_tmp))
                    hist_0.Add(hist_tmp,weight)
            else :
                print("Cannot find hist "+str(histname))
           
        print("\n\n\n")

        index += 1

    outfile.cd()    
    hist_0.SetName(histname+'_INPUT')

    file_curr=ROOT.TFile(pathData,"READ")
    print("Open file "+pathData)

    print("Get data hist: "+histname)
    hist_Data_0=file_curr.Get(histname)
    hist_Data_0.SetDirectory(0)
    hist_Data_0.SetName(histname+'_INPUTDATA')
    hist_Data_0.Divide(hist_0)
    hist_Data_0.SetName('h_'+histname)
    
    outfile.cd()
    hist_Data_0.Write()
    
    
    
