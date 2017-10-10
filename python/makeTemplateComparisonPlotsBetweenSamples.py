import ROOT
import sys
import math
import string
from PlotFunctions import *
from TAxisFunctions import *
import os

ROOT.gROOT.SetBatch(True)
from ROOT import TCanvas,TPad,TH2,TColor

#ROOT.gStyle.SetOptStat(0)
SetupStyle()

outfilename = sys.argv[1]

Lumi = 36000.0 #in pb^-1

#Pythia
ListOfCrossSections = [2.6454e7,254620.,4553.5,257.54,16.215] #in pb
ListOfFilterEfficiencies=[1.6117e-5,3.5545e-5,7.1955e-5,7.6347e-5,3.1782e-5]

ListOfCrossSections_incl = [2.6454e7,254630.,4553.5,257.53,16.215] #in pb 
ListOfFilterEfficiencies_incl = [0.00032,0.00053,0.000922,0.000939,0.000392]

#Herwig
#ListOfCrossSections = [1.8831e7,173300.,2965.4,162.76,10.113] #in pb
#ListOfFilterEfficiencies = [1.9120e-05,4.2602e-5,9.0537e-5,0.00010259,4.7654e-5] #UPDATE for JZ3W 

basepath = '/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/'

pathJZ3W = basepath + 'Output_Calib/Calib_user.ruth.mc15_13TeV.427003.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root/merged_hist_Calib_user.ruth.mc15_13TeV.427003.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root' 
pathJZ4W = basepath + 'Output_Calib/Calib_user.ruth.mc15_13TeV.427004.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root/merged_hist_Calib_user.ruth.mc15_13TeV.427004.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root' 
pathJZ5W = basepath + 'Output_Calib/Calib_user.ruth.mc15_13TeV.427005.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root/merged_hist_Calib_user.ruth.mc15_13TeV.427005.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root' 
pathJZ6W = basepath + 'Output_Calib/Calib_user.ruth.mc15_13TeV.427106.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5839_s2726_r7773_r7676_p2949_tuple.root/merged_hist_Calib_user.ruth.mc15_13TeV.427106.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5839_s2726_r7773_r7676_p2949_tuple.root' 
pathJZ7W = basepath + 'Output_Calib/Calib_user.ruth.mc15_13TeV.427107.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5839_s2726_r7773_r7676_p2949_tuple.root/merged_hist_Calib_user.ruth.mc15_13TeV.427107.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5839_s2726_r7773_r7676_p2949_tuple.root' 


pathJZ3W_incl = basepath + 'Output_Calib/Calib_user.ruth.mc15_13TeV.361023.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e3668_s2576_s2132_r7725_r7676_p2949_tuple.root'
pathJZ4W_incl = basepath + 'Output_Calib/Calib_user.ruth.mc15_13TeV.361024.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e3668_s2576_s2132_r7725_r7676_p2949_tuple.root'
pathJZ5W_incl = basepath + 'Output_Calib/Calib_user.ruth.mc15_13TeV.361025.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e3668_s2576_s2132_r7725_r7676_p2949_tuple.root'
pathJZ6W_incl = basepath + 'Output_Calib/Calib_user.ruth.mc15_13TeV.361026.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e3569_s2608_s2183_r7725_r7676_p2949_tuple.root'
pathJZ7W_incl = basepath + 'Output_Calib/Calib_user.ruth.mc15_13TeV.361027.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e3668_s2608_s2183_r7725_r7676_p2949_tuple.root'


ListOfMCPaths = [ pathJZ3W, pathJZ4W, pathJZ5W, pathJZ6W, pathJZ7W ]
ListOfMCPaths_incl = [ pathJZ3W_incl, pathJZ4W_incl, pathJZ5W_incl, pathJZ6W_incl, pathJZ7W_incl ]
pathData = basepath + 'Output_Calib/user.ruth.data1516_13TeV.all.root/merged_hist_user.ruth.data1516_13TeV.all.root '

#make list of histograms
ListOfHists = ['hBBNom_mjmaxSd0','hBLNom_mjmaxSd0','hCCNom_mjmaxSd0','hCLNom_mjmaxSd0','hLLNom_mjmaxSd0','hBBNom_nmjmaxSd0','hBLNom_nmjmaxSd0','hCCNom_nmjmaxSd0','hCLNom_nmjmaxSd0','hLLNom_nmjmaxSd0' ]

ListOfNames = ['BB','BL','CC','CL','LL','BB','BL','CC','CL','LL']


#make Canvas
ListOfCanvasses = []

prefix="c_mujetSd0"
for name in ListOfNames :
    ListOfCanvasses.append(ROOT.TCanvas(prefix+name,"",800,800));
    if name is 'LL':
        prefix="c_nonmujetSd0"


colors=[]
colors.append([ROOT.kBlue+1,ROOT.kBlue+1])
colors.append([ROOT.kAzure-4,ROOT.kAzure-4])
colors.append([ROOT.kCyan+3,ROOT.kCyan+3])
colors.append([ROOT.kGreen-9,ROOT.kGreen-9])
colors.append([ROOT.kOrange,ROOT.kOrange])
colors.append([ROOT.kBlue+1,ROOT.kBlue+1])
colors.append([ROOT.kAzure-4,ROOT.kAzure-4])
colors.append([ROOT.kCyan+3,ROOT.kCyan+3])
colors.append([ROOT.kGreen-9,ROOT.kGreen-9])
colors.append([ROOT.kOrange,ROOT.kOrange])

#colors = [ROOT.kBlue, ROOT.kBlack]
#SetColors(canvas,colors)

#loop over MC histograms
#index=0
hists = []
for name in ListOfNames:
    hists.append([]):


for histname in ListOfHists :
    hist_index = ListOfHists.index(histname)

    for path in ListOfMCPaths :        
        index = ListOfMCPaths.index(path)
        file_curr=ROOT.TFile(path,"READ")
        file_curr_incl=ROOT.TFile(ListOfMCPaths_incl[index],"READ")
        
        print("Open file "+path)
        print("Open file "+ListOfMCPaths_incl[index])
    
        bookkeep_hist=file_curr.Get("Hist_BookKeeping") #Events in AOD is in Bin 3
        weight=ListOfCrossSections[index]*ListOfFilterEfficiencies[index]/bookkeep_hist.GetBinContent(3)*Lumi
        print("weight is: "+str(weight))

        bookkeep_hist_incl=file_curr_incl.Get("Hist_BookKeeping") #Events in AOD is in Bin 3
        weight_incl=ListOfCrossSections_incl[index]*ListOfFilterEfficiencies_incl[index]/bookkeep_hist_incl.GetBinContent(3)*Lumi
        print("weight is: "+str(weight))


        if not file_curr.GetListOfKeys().Contains(histname) or file_curr_incl.GetListOfKeys().Contains(histname):
            print("Cannot find first hist "+histname)
        elif index is 0 :
            hist_0=file_curr.Get(histname)
            hist_0.SetDirectory(0)
            hist_0.Scale(weight)
            hist_0.SetLineWidth(3);
            hist_0.SetName(histname+str(index))

            hist_0_incl=file_curr_incl.Get(histname)
            hist_0_incl.SetDirectory(0)
            hist_0_incl.Scale(weight_incl)
            hist_0_incl.SetLineWidth(3);
            hist_0_incl.SetName(histname+str(index))
            
            

        elif index is not 0 :
            hist_tmp=file_curr.Get(histname)
            hist_0.Add(hist_tmp,weight)

            hist_tmp_incl=file_curr_incl.Get(histname)
            hist_0_incl.Add(hist_tmp_incl,weight_incl)

            print(hist_0)
            print(hist_0_incl)

    hist_0.SetName(ListOfNames[hist_index])
    hist_0_incl.SetName(ListOfNames[hist_index]+" inclusive dijet")

    #Print Integral
    print('##########################')
    print('# Integral report for '+ListOfNames[hist_index])
    print('# mu-filtered: '+str(hist_0.Integral()))
    print('# inclusive: '+str(hist_0_incl.Integral()))
    print('##########################')
    
    #Normalize to Unity
    hist_0.Scale(1./hist_0.Integral())
    hist_0_incl.Scale(1./hist_0_incl.Integral())

    for name in ListOfNames:
        name_index = ListOfNames.index(name)
        hists[name_index].append(hist_0)
        hists[name_index].append(hist_0_incl)

    #get data histogram
    #file_data=ROOT.TFile(pathData,"READ")
    #hist_data=file_data.Get(histname)
    #hist_data.SetDirectory(0)
    #hist_data.SetName("Data")
    #hists[hist_index].insert(0,hist_data)


for i in range(0,len(ListOfCanvasses)) :
    for j  in range(0,len(hists[i])) :
        AddHistogram(ListOfCanvasses[i],hists[i][j],'hist')
        if i<5 :
            SetAxisLabels(ListOfCanvasses[i],'Muon-associated Track Jet S_{d0}','Normalised to Unity')
        else :
            SetAxisLabels(ListOfCanvasses[i],'Non-muon-associated Track Jet S_{d0}','Normalised to Unity')

    FullFormatCanvasDefault(ListOfCanvasses[i],36)
    SetColors(ListOfCanvasses[i],colors[i])
    SetLineStyles(ListOfCanvasses[i])
    ListOfCanvasses[i].Print(outfilename+'_'+ListOfHists[i]+'.pdf')

        
    
