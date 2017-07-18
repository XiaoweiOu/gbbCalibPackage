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

Lumi = 36000.0; #in pb^-1

#Pythia
ListOfCrossSections = [2.6454e7,254620.,4553.5,257.54,16.215] #in pb
ListOfFilterEfficiencies=[1.6117e-5,3.5545e-5,7.1955e-5,7.6347e-5,3.1782e-5]

#Pythia inclusive dijet
ListOfInclusiveCrossSections = [2.6454e7,254630.,4553.5,257.53,16.215] #in pb 
ListOfInclusiveFilterEfficiencies = [0.00032,0.00053,0.000922,0.000939,0.000392]


#Herwig
#ListOfCrossSections = [1.8831e7,173300.,2965.4,162.76,10.113] #in pb
#ListOfFilterEfficiencies = [1.9120e-05,4.2602e-5,9.0537e-5,0.00010259,4.7654e-5] #UPDATE for JZ3W 

ListOfSystematics = [ 'Nom' , 'JET_Rtrk_Baseline_Kin__1up', 'JET_Rtrk_Baseline_Kin__1down', 'JET_Rtrk_Modelling_Kin__1up', 'JET_Rtrk_Modelling_Kin__1down', 'JET_Rtrk_Tracking_Kin__1up', 'JET_Rtrk_Tracking_Kin__1down', 'JET_Rtrk_TotalStat_Kin__1up', 'JET_Rtrk_TotalStat_Kin__1down', 'JET_Rtrk_Baseline_D2__1up', 'JET_Rtrk_Baseline_D2__1down','JET_Rtrk_Modelling_D2__1up', 'JET_Rtrk_Modelling_D2__1down', 'JET_Rtrk_Tracking_D2__1up','JET_Rtrk_Tracking_D2__1down', 'JET_Rtrk_TotalStat_D2__1up', 'JET_Rtrk_TotalStat_D2__1down', 'FATJET_JMR__1up','FATJET_JER__1up']

basepath = '/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/'

pathData = basepath + 'Output_CrossCheck/user.ruth.data1516_13TeV.all.root/merged_hist_user.ruth.data1516_13TeV.all.root'

ListOfMCPathsSys=[]
ListOfInclusiveMCPathsSys=[]

for sys in ListOfSystematics:
    if sys is 'Nom':
        sysdir='Output_CrossCheck'
    else:
        sysdir='Output_CrossCheck_'+sys

    pathJZ3W = basepath + sysdir + '/CrossCheck_user.ruth.mc15_13TeV.427003.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.427003.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root' 
    pathJZ4W = basepath + sysdir + '/CrossCheck_user.ruth.mc15_13TeV.427004.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.427004.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root' 
    pathJZ5W = basepath + sysdir + '/CrossCheck_user.ruth.mc15_13TeV.427005.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.427005.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5660_s2726_r7773_r7676_p2949_tuple.root' 
    pathJZ6W = basepath + sysdir + '/CrossCheck_user.ruth.mc15_13TeV.427106.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5839_s2726_r7773_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.427106.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5839_s2726_r7773_r7676_p2949_tuple.root' 
    pathJZ7W = basepath + sysdir + '/CrossCheck_user.ruth.mc15_13TeV.427107.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5839_s2726_r7773_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.427107.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e5839_s2726_r7773_r7676_p2949_tuple.root' 

    
    ListOfMCPaths = [ pathJZ3W, pathJZ4W, pathJZ5W, pathJZ6W, pathJZ7W ]
    ListOfMCPathsSys.append(ListOfMCPaths)

    pathJZ3W_incl = basepath + sysdir + '/CrossCheck_user.ruth.mc15_13TeV.361023.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e3668_s2576_s2132_r7725_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.361023.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e3668_s2576_s2132_r7725_r7676_p2949_tuple.root'
    pathJZ4W_incl = basepath + sysdir + '/CrossCheck_user.ruth.mc15_13TeV.361024.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e3668_s2576_s2132_r7725_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.361024.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e3668_s2576_s2132_r7725_r7676_p2949_tuple.root'
    pathJZ5W_incl = basepath + sysdir + '/CrossCheck_user.ruth.mc15_13TeV.361025.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e3668_s2576_s2132_r7725_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.361025.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e3668_s2576_s2132_r7725_r7676_p2949_tuple.root'
    pathJZ6W_incl = basepath + sysdir + '/CrossCheck_user.ruth.mc15_13TeV.361026.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e3569_s2608_s2183_r7725_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.361026.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e3569_s2608_s2183_r7725_r7676_p2949_tuple.root'
    pathJZ7W_incl = basepath + sysdir + '/CrossCheck_user.ruth.mc15_13TeV.361027.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e3668_s2608_s2183_r7725_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.361027.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-02.e3668_s2608_s2183_r7725_r7676_p2949_tuple.root'


    ListOfInclusiveMCPaths = [ pathJZ3W_incl, pathJZ4W_incl, pathJZ5W_incl, pathJZ6W_incl, pathJZ7W_incl ]
    ListOfInclusiveMCPathsSys.append(ListOfInclusiveMCPaths)
    

#ListOfVariables = [ 'fjpt', 'nmjpt', 'mjpt', 'mjmaxSd0', 'nmjmaxSd0' ]
ListOfVariables = [ 'fjpt','fjm', 'fjD2', 'mjmaxSd0', 'nmjmaxSd0', 'fjpt_PREFITPOSTTAG', 'fjm_PREFITPOSTTAG', 'fjD2_PREFITPOSTTAG', 'mjmaxSd0_PREFITPOSTTAG', 'nmjmaxSd0_PREFITPOSTTAG','fjpt_PREFITPOSTTAG_BTAGUP', 'fjm_PREFITPOSTTAG_BTAGUP', 'fjD2_PREFITPOSTTAG_BTAGUP', 'fjpt_PREFITPOSTTAG_BTAGDOWN', 'fjm_PREFITPOSTTAG_BTAGDOWN', 'fjD2_PREFITPOSTTAG_BTAGDOWN' ]

ListOfFlavourPairs = [ 'BB', 'BL', 'CC', 'CL']

ListOfmjpt=[100,200]
ListOfnmjpt=[20,50,80]

#mjpt_l100_nmjpt_l20
ListOfTJpt=['mjpt_l100_nmjpt_l20','mjpt_l100_nmjpt_g20l50','mjpt_l100_nmjpt_g50l80', 'mjpt_l100_nmjpt_g80',
            'mjpt_g100l200_nmjpt_l20','mjpt_g100l200_nmjpt_g20l50','mjpt_g100l200_nmjpt_g50l80','mjpt_g100l200_nmjpt_g80',
            'mjpt_g200_nmjpt_l20','mjpt_g200_nmjpt_g20l50','mjpt_g200_nmjpt_g50l80', 'mjpt_g200_nmjpt_g80']

outfilename = 'testinputsLLinclusive_inclusiveReweighting_AllSys.root'

#make list of histograms in Data
ListOfDataHists = []
for tjpt in ListOfTJpt :
    for var in ListOfVariables :
        ListOfDataHists.append('hDataNom_'+tjpt+'_'+var)
        print('hDataNom_'+tjpt+'_'+var)

#open output file
outfile=ROOT.TFile(outfilename,"RECREATE")




for sys in ListOfSystematics:
    sys_index=ListOfSystematics.index(sys)
    ListOfMCPaths=ListOfMCPathsSys[sys_index]
    ListOfInclusiveMCPaths=ListOfInclusiveMCPathsSys[sys_index]

    #make list of histograms in MC
    ListOfHists = []
    for flavour in ListOfFlavourPairs :
        for tjpt in ListOfTJpt :
            for var in ListOfVariables :
                if "BTAG" in var and "Nom" not in sys:
                    continue
                ListOfHists.append('h'+flavour+sys+'_'+tjpt+'_'+var)
                print('h'+flavour+sys+'_'+tjpt+'_'+var)
            
    ListOfLLHists = []
    for tjpt in ListOfTJpt :
        for var in ListOfVariables :
            if "BTAG" in var and "Nom" not in sys:
                continue
            ListOfLLHists.append('hLL'+sys+'_'+tjpt+'_'+var)
            print('hLL'+sys+'_'+tjpt+'_'+var)


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
                if index is 0: 
                    hist_0=None
            elif index is 0 or hist_0 is None :
                hist_0=file_curr.Get(histname)
                hist_0.SetDirectory(0)
                hist_0.Scale(weight)
            elif index is not 0 :
                hist_tmp=file_curr.Get(histname)
                hist_0.Add(hist_tmp,weight)

            if index==len(ListOfMCPaths)-1 :
                outfile.cd()
                #new_hist_0=(ROOT.TH1F)hist_0
                hist_0.Write()


    #loop over inclusive MC for LL histograms
    for histname in ListOfLLHists :
        hist_index=ListOfLLHists.index(histname)
        #print('Hello')
        for path in ListOfInclusiveMCPaths :
            index=ListOfInclusiveMCPaths.index(path)
            file_curr=ROOT.TFile(path,"READ")

            bookkeep_hist=file_curr.Get("Hist_BookKeeping") #Events in AOD is in Bin 3
            weight=ListOfInclusiveCrossSections[index]*ListOfInclusiveFilterEfficiencies[index]/bookkeep_hist.GetBinContent(3)*Lumi
        
            if not file_curr.GetListOfKeys().Contains(histname):
                print("Cannot find hist "+histname)
                if index is 0:
                    hist_0=None
            elif index is 0 or hist_0 is None :
                hist_0=file_curr.Get(histname)
                hist_0.SetDirectory(0)
                hist_0.Scale(weight)
                #print('got '+hist_0.GetName())
            elif index is not 0 :
                hist_tmp=file_curr.Get(histname)
                hist_0.Add(hist_tmp,weight)
                #print('adding '+hist_tmp.GetName())

            if index==len(ListOfInclusiveMCPaths)-1 :
                outfile.cd()
                #new_hist_0=(ROOT.TH1F)hist_0
                hist_0.Write()
        


#loop over Data Histograms
file_curr=ROOT.TFile(pathData,"READ")

for histname in ListOfDataHists :
    hist_0=file_curr.Get(histname)
    hist_0.SetDirectory(0)
    #new_hist_0=(ROOT.TH1F)hist_0
    outfile.cd()
    hist_0.Write()
    
    
    
