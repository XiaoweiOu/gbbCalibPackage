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

#Pythia inclusive dijet                                                                                                                                      
ListOfInclusiveCrossSections = [2.6454e7,254630.,4553.5,257.53,16.215] #in pb                                                                                
ListOfInclusiveFilterEfficiencies = [0.00032,0.00053,0.000922,0.000939,0.000392]

#Herwig
#ListOfCrossSections = [1.8831e7,173300.,2965.4,162.76,10.113] #in pb
#ListOfFilterEfficiencies = [1.9120e-05,4.2602e-5,9.0537e-5,0.00010259,4.7654e-5] #UPDATE for JZ3W 

basepath = '/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/'

pathJZ3W = basepath + 'Output_CrossCheck/CrossCheck_user.ruth.mc15_13TeV.427003.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e5660_s2726_r7773_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.427003.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e5660_s2726_r7773_r7676_p2949_tuple.root' 
pathJZ4W = basepath + 'Output_CrossCheck/CrossCheck_user.ruth.mc15_13TeV.427004.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e5660_s2726_r7773_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.427004.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e5660_s2726_r7773_r7676_p2949_tuple.root' 
pathJZ5W = basepath + 'Output_CrossCheck/CrossCheck_user.ruth.mc15_13TeV.427005.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e5660_s2726_r7773_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.427005.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e5660_s2726_r7773_r7676_p2949_tuple.root' 
pathJZ6W = basepath + 'Output_CrossCheck/CrossCheck_user.ruth.mc15_13TeV.427106.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e5839_s2726_r7773_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.427106.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e5839_s2726_r7773_r7676_p2949_tuple.root' 
pathJZ7W = basepath + 'Output_CrossCheck/CrossCheck_user.ruth.mc15_13TeV.427107.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e5839_s2726_r7773_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.427107.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e5839_s2726_r7773_r7676_p2949_tuple.root' 

ListOfMCPaths = [ pathJZ3W, pathJZ4W, pathJZ5W, pathJZ6W, pathJZ7W ]
pathData = basepath + 'Output_CrossCheck/user.ruth.data1516_13TeV.all.root/merged_hist_user.ruth.data1516_13TeV.all.root '


pathJZ3W_incl = basepath + 'Output_CrossCheck/CrossCheck_user.ruth.mc15_13TeV.361023.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e3668_s2576_s2132_r7725_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.361023.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e3668_s2576_s2132_r7725_r7676_p2949_tuple.root'
pathJZ4W_incl = basepath + 'Output_CrossCheck/CrossCheck_user.ruth.mc15_13TeV.361024.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e3668_s2576_s2132_r7725_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.361024.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e3668_s2576_s2132_r7725_r7676_p2949_tuple.root'
pathJZ5W_incl = basepath + 'Output_CrossCheck/CrossCheck_user.ruth.mc15_13TeV.361025.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e3668_s2576_s2132_r7725_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.361025.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e3668_s2576_s2132_r7725_r7676_p2949_tuple.root'
pathJZ6W_incl = basepath + 'Output_CrossCheck/CrossCheck_user.ruth.mc15_13TeV.361026.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e3569_s2608_s2183_r7725_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.361026.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e3569_s2608_s2183_r7725_r7676_p2949_tuple.root'
pathJZ7W_incl = basepath + 'Output_CrossCheck/CrossCheck_user.ruth.mc15_13TeV.361027.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e3668_s2608_s2183_r7725_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.361027.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e3668_s2608_s2183_r7725_r7676_p2949_tuple.root'

ListOfInclusiveMCPaths = [ pathJZ3W_incl, pathJZ4W_incl, pathJZ5W_incl, pathJZ6W_incl, pathJZ7W_incl ]

#make list of histograms
ListOfHists = ['hBBNom_mjmaxSd0','hBLNom_mjmaxSd0','hCCNom_mjmaxSd0','hCLNom_mjmaxSd0','hBBNom_nmjmaxSd0','hBLNom_nmjmaxSd0','hCCNom_nmjmaxSd0','hCLNom_nmjmaxSd0']

ListOfInclusiveHists = ['hLLNom_mjmaxSd0','hLLNom_nmjmaxSd0']

ListOfNames = ['BB','BL','CC','CL','BB','BL','CC','CL']


#make Canvas
ListOfCanvasses = []

ListOfCanvasses.append(ROOT.TCanvas("c_mujetSd0","",800,800));
ListOfCanvasses.append(ROOT.TCanvas("c_nonmujetSd0","",800,800));


colors=[ROOT.kBlue+1,ROOT.kAzure-4,ROOT.kCyan+3,ROOT.kGreen-9,ROOT.kOrange]
#colors = [ROOT.kBlue, ROOT.kBlack]
#SetColors(canvas,colors)

#loop over MC histograms
#index=0
hists = []
hists.append([])
hists.append([])


for histname in ListOfHists :
    hist_index = ListOfHists.index(histname)

    for path in ListOfMCPaths :        
        index = ListOfMCPaths.index(path)
        file_curr=ROOT.TFile(path,"READ")
        print("Open file "+path)
    
        bookkeep_hist=file_curr.Get("Hist_BookKeeping") #Events in AOD is in Bin 3
        weight=ListOfCrossSections[index]*ListOfFilterEfficiencies[index]/bookkeep_hist.GetBinContent(3)*Lumi
        print("weight is: "+str(weight))

        if not file_curr.GetListOfKeys().Contains(histname) :
            print("Cannot find first hist "+histname)
        elif index is 0 :
            hist_0=file_curr.Get(histname)
            hist_0.SetDirectory(0)
            hist_0.Scale(weight)
            hist_0.SetLineWidth(3);
            hist_0.SetName(histname+str(index))
        elif index is not 0 :
            hist_tmp=file_curr.Get(histname)
            hist_0.Add(hist_tmp,weight)

            print(hist_0)
    hist_0.SetName(ListOfNames[hist_index])
    print('Setting name to'+ListOfNames[hist_index])
    #Normalize to Unity
    hist_0.Scale(1./hist_0.Integral())
    if 'nmjmaxSd0' in histname :
        hists[1].append(hist_0)
    else :
        hists[0].append(hist_0)
    #get data histogram
    #file_data=ROOT.TFile(pathData,"READ")
    #hist_data=file_data.Get(histname)
    #hist_data.SetDirectory(0)
    #hist_data.SetName("Data")
    #hists[hist_index].insert(0,hist_data)



for histname in ListOfInclusiveHists :
    hist_index = ListOfInclusiveHists.index(histname)

    for path in ListOfInclusiveMCPaths :        
        index = ListOfInclusiveMCPaths.index(path)
        file_curr=ROOT.TFile(path,"READ")
        print("Open file "+path)
    
        bookkeep_hist=file_curr.Get("Hist_BookKeeping") #Events in AOD is in Bin 3
        weight=ListOfInclusiveCrossSections[index]*ListOfInclusiveFilterEfficiencies[index]/bookkeep_hist.GetBinContent(3)*Lumi
        print("weight is: "+str(weight))

        if not file_curr.GetListOfKeys().Contains(histname) :
            print("Cannot find first hist "+histname)
        elif index is 0 :
            hist_0=file_curr.Get(histname)
            hist_0.SetDirectory(0)
            hist_0.Scale(weight)
            hist_0.SetLineWidth(3);
            hist_0.SetName(histname+str(index))
        elif index is not 0 :
            hist_tmp=file_curr.Get(histname)
            hist_0.Add(hist_tmp,weight)

            print(hist_0)
    hist_0.SetName('LL')
    print('Setting Name to LL')
    #Normalize to Unity
    hist_0.Scale(1./hist_0.Integral())
    if 'nmjmaxSd0' in histname :
        hists[1].append(hist_0)
    else :
        hists[0].append(hist_0)



for i in range(0,len(ListOfCanvasses)) :
    for j  in range(0,len(hists[i])) :
        AddHistogram(ListOfCanvasses[i],hists[i][j],'hist')
        if i is 0 :
            SetAxisLabels(ListOfCanvasses[i],'Muon-associated Track Jet S_{d0}','Normalised to Unity')
        else :
            SetAxisLabels(ListOfCanvasses[i],'Non-muon-associated Track Jet S_{d0}','Normalised to Unity')

    FullFormatCanvasDefault(ListOfCanvasses[i],36,simulation=True, doLogy=True)
    SetColors(ListOfCanvasses[i],colors)
    ListOfCanvasses[i].Print(outfilename+'_'+ListOfHists[i]+'.pdf')

        
    
