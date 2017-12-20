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

ListOfSystematics = [ 'Nom' , 'JET_Rtrk_Baseline_Kin__1up', 'JET_Rtrk_Baseline_Kin__1down', 'JET_Rtrk_Modelling_Kin__1up', 'JET_Rtrk_Modelling_Kin__1down', 'JET_Rtrk_Tracking_Kin__1up', 'JET_Rtrk_Tracking_Kin__1down', 'JET_Rtrk_TotalStat_Kin__1up', 'JET_Rtrk_TotalStat_Kin__1down', 'JET_Rtrk_Baseline_Sub__1up', 'JET_Rtrk_Baseline_Sub__1down','JET_Rtrk_Modelling_Sub__1up', 'JET_Rtrk_Modelling_Sub__1down', 'JET_Rtrk_Tracking_Sub__1up','JET_Rtrk_Tracking_Sub__1down', 'JET_Rtrk_TotalStat_Sub__1up', 'JET_Rtrk_TotalStat_Sub__1down', 'FATJET_JMR__1up','FATJET_JER__1up']

basepath = '/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/'

pathData = basepath + 'Output_CrossCheck/user.ruth.data1516_13TeV.all.root/merged_hist_user.ruth.data1516_13TeV.all.root'

ListOfMCPathsSys=[]
ListOfInclusiveMCPathsSys=[]

for sys in ListOfSystematics:
    if sys is 'Nom':
        sysdir='Output_CrossCheck'
    else:
        sysdir='Output_CrossCheck_'+sys

    pathJZ3W = basepath + sysdir + '/CrossCheck_user.ruth.mc15_13TeV.427003.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04c.e5660_s2726_r7773_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.427003.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04c.e5660_s2726_r7773_r7676_p2949_tuple.root' 
    pathJZ4W = basepath + sysdir + '/CrossCheck_user.ruth.mc15_13TeV.427004.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04c.e5660_s2726_r7773_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.427004.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04c.e5660_s2726_r7773_r7676_p2949_tuple.root' 
    pathJZ5W = basepath + sysdir + '/CrossCheck_user.ruth.mc15_13TeV.427005.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04c.e5660_s2726_r7773_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.427005.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04c.e5660_s2726_r7773_r7676_p2949_tuple.root' 
    pathJZ6W = basepath + sysdir + '/CrossCheck_user.ruth.mc15_13TeV.427106.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04c.e5839_s2726_r7773_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.427106.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04c.e5839_s2726_r7773_r7676_p2949_tuple.root' 
    pathJZ7W = basepath + sysdir + '/CrossCheck_user.ruth.mc15_13TeV.427107.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04c.e5839_s2726_r7773_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.427107.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04c.e5839_s2726_r7773_r7676_p2949_tuple.root' 

    
    ListOfMCPaths = [ pathJZ3W, pathJZ4W, pathJZ5W, pathJZ6W, pathJZ7W ]
    ListOfMCPathsSys.append(ListOfMCPaths)

    pathJZ3W_incl = basepath + sysdir + '/CrossCheck_user.ruth.mc15_13TeV.361023.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e3668_s2576_s2132_r7725_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.361023.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e3668_s2576_s2132_r7725_r7676_p2949_tuple.root'
    pathJZ4W_incl = basepath + sysdir + '/CrossCheck_user.ruth.mc15_13TeV.361024.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e3668_s2576_s2132_r7725_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.361024.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e3668_s2576_s2132_r7725_r7676_p2949_tuple.root'
    pathJZ5W_incl = basepath + sysdir + '/CrossCheck_user.ruth.mc15_13TeV.361025.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e3668_s2576_s2132_r7725_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.361025.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e3668_s2576_s2132_r7725_r7676_p2949_tuple.root'
    pathJZ6W_incl = basepath + sysdir + '/CrossCheck_user.ruth.mc15_13TeV.361026.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e3569_s2608_s2183_r7725_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.361026.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e3569_s2608_s2183_r7725_r7676_p2949_tuple.root'
    pathJZ7W_incl = basepath + sysdir + '/CrossCheck_user.ruth.mc15_13TeV.361027.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e3668_s2608_s2183_r7725_r7676_p2949_tuple.root/merged_hist_CrossCheck_user.ruth.mc15_13TeV.361027.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-04.e3668_s2608_s2183_r7725_r7676_p2949_tuple.root'


    ListOfInclusiveMCPaths = [ pathJZ3W_incl, pathJZ4W_incl, pathJZ5W_incl, pathJZ6W_incl, pathJZ7W_incl ]
    ListOfInclusiveMCPathsSys.append(ListOfInclusiveMCPaths)
   

#ListOfVariables = [ 'fjpt', 'nmjpt', 'mjpt', 'mjmaxSd0', 'nmjmaxSd0' ]
ListOfVariables = [ 'fjpt','fjptsc','fjm', 'fjD2','mjpt','nmjpt','mjeta','nmjeta', 'mjmaxSd0', 'nmjmaxSd0', 'fjpt_PREFITPOSTTAG','fjptsc_PREFITPOSTTAG', 'fjm_PREFITPOSTTAG', 'fjD2_PREFITPOSTTAG','mjpt_PREFITPOSTTAG','nmjpt_PREFITPOSTTAG','mjeta_PREFITPOSTTAG','nmjeta_PREFITPOSTTAG', 'mjmaxSd0_PREFITPOSTTAG', 'nmjmaxSd0_PREFITPOSTTAG','fjpt_PREFITPOSTTAG_BTAGUP', 'fjm_PREFITPOSTTAG_BTAGUP', 'fjD2_PREFITPOSTTAG_BTAGUP','mjpt_PREFITPOSTTAG_BTAGUP','nmjpt_PREFITPOSTTAG_BTAGUP','mjeta_PREFITPOSTTAG_BTAGUP','nmjeta_PREFITPOSTTAG_BTAGUP','fjptsc_PREFITPOSTTAG_BTAGUP', 'fjpt_PREFITPOSTTAG_BTAGDOWN', 'fjm_PREFITPOSTTAG_BTAGDOWN', 'fjD2_PREFITPOSTTAG_BTAGDOWN','mjpt_PREFITPOSTTAG_BTAGDOWN','nmjpt_PREFITPOSTTAG_BTAGDOWN','mjeta_PREFITPOSTTAG_BTAGDOWN','nmjeta_PREFITPOSTTAG_BTAGDOWN','fjptsc_PREFITPOSTTAG_BTAGDOWN','trjpt','fjpt_PREFITANTITAG', 'allsrjpt' , 'srjN','evemu','slR4jpt','trjptfjptratio','trjptgbbcandratio' ]

#fatjet pt in eta/phi bins
#BinFragments=['g-2l-1','g-1l0','g0l1','g1l2']
#PhiBinFragments=['l-2','g-2l-1','g-1l0','g0l1','g1l2','g2']
#for a in BinFragments :
#    for b in BinFragments :
#        ListOfVariables.append('fjeta_'+a+'_fjphi_'+b+'_fjpt')



ListOfFlavourPairs = [ 'BB', 'BL', 'CC', 'CL']
ListOfInclusiveFlavourPairs = [ 'LL' ]

ListOfmjpt=[100,200]
ListOfnmjpt=[20,50,80]

#mjpt_l100_nmjpt_l20
ListOfTJpt=['mjpt_l100_nmjpt_l20','mjpt_l100_nmjpt_g20l50','mjpt_l100_nmjpt_g50l80', 'mjpt_l100_nmjpt_g80',
            'mjpt_g100l200_nmjpt_l20','mjpt_g100l200_nmjpt_g20l50','mjpt_g100l200_nmjpt_g50l80','mjpt_g100l200_nmjpt_g80',
            'mjpt_g200_nmjpt_l20','mjpt_g200_nmjpt_g20l50','mjpt_g200_nmjpt_g50l80', 'mjpt_g200_nmjpt_g80']

outfilename = '171219_testinputsLLinclusive_pt500_AllSys.root'

#make list of histograms in Data
ListOfDataHists = []
for tjpt in ListOfTJpt :
    for var in ListOfVariables :
        ListOfDataHists.append('hDataNom_'+tjpt+'_'+var)
        print('hDataNom_'+tjpt+'_'+var)

ListOfDataHists.append('CutFlow');

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
                if "SD0_Smear" in var and "Nom" not in sys:
                    continue
                if "fjeta" in var and "fjphi" in var and "Nom" not in sys:
                    continue
                if sys is "Nom" and "maxSd0" in var :
                    ListOfHists.append('h'+flavour+'SD0Smear__1up_'+tjpt+'_'+var)
                    ListOfHists.append('h'+flavour+'SD0SMEAR__1down_'+tjpt+'_'+var)
                    
                ListOfHists.append('h'+flavour+sys+'_'+tjpt+'_'+var)
                print('h'+flavour+sys+'_'+tjpt+'_'+var)
                
    if sys is 'Nom':
        ListOfHists.append('CutFlow')
        
    ListOfLLHists = []
    for flavour in ListOfInclusiveFlavourPairs:
        for tjpt in ListOfTJpt :
            for var in ListOfVariables :
                if "BTAG" in var and "Nom" not in sys:
                    continue
                if "SD0_Smear" in var and "Nom" not in sys:
                    continue
                if "fjeta" in var and "fjphi" in var and "Nom" not in sys:
                    continue
                if sys is "Nom" and "maxSd0" in var :
                    ListOfLLHists.append('h'+flavour+'SD0Smear__1up_'+tjpt+'_'+var)
                    ListOfLLHists.append('h'+flavour+'SD0SMEAR__1down_'+tjpt+'_'+var)

                ListOfLLHists.append('h'+flavour+sys+'_'+tjpt+'_'+var)
                print('h'+flavour+sys+'_'+tjpt+'_'+var)

    if sys is 'Nom':            
        ListOfLLHists.append('CutFlow')

    #loop over MC histograms
    for histname in ListOfHists :
        hist_index=ListOfHists.index(histname)
        hist_0=None
        for path in ListOfMCPaths :
            index=ListOfMCPaths.index(path)
        #for path in ListOfInclusiveMCPaths :
            #index=ListOfInclusiveMCPaths.index(path)
            file_curr=ROOT.TFile(path,"READ")
            if not file_curr :
                print("Cannot find file: "+path)
            bookkeep_hist=file_curr.Get("Hist_BookKeeping") #Events in AOD is in Bin 3
            if not bookkeep_hist :
                print("Cannot find BookKeeping Histogram in path" + path)
                continue
            weight=ListOfCrossSections[index]*ListOfFilterEfficiencies[index]/bookkeep_hist.GetBinContent(3)*Lumi
            #weight=ListOfInclusiveCrossSections[index]*ListOfInclusiveFilterEfficiencies[index]/bookkeep_hist.GetBinContent(3)*Lumi

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
                if hist_0 is None :
                    print("ERROR: Could not find "+histname+" in all input Files!")
                if histname is 'CutFlow':
                    hist_0.SetName('CutFlow_MC')
                hist_0.Write()


    #loop over inclusive MC for LL histograms
    for histname in ListOfLLHists :
        hist_index=ListOfLLHists.index(histname)
        #print('Hello')
        hist_0=None
        for path in ListOfInclusiveMCPaths :
            index=ListOfInclusiveMCPaths.index(path)
            file_curr=ROOT.TFile(path,"READ")

            bookkeep_hist=file_curr.Get("Hist_BookKeeping") #Events in AOD is in Bin 3
            if not bookkeep_hist :
                print("Cannot find BookKeeping Histogram in path" + path)
                continue
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
                if histname is 'CutFlow':
                    hist_0.SetName('CutFlow_MC')
                #new_hist_0=(ROOT.TH1F)hist_0
                hist_0.Write()
        


#loop over Data Histograms
file_curr=ROOT.TFile(pathData,"READ")

for histname in ListOfDataHists :
    hist_0=file_curr.Get(histname)
    if hist_0 is None :
        print("Could not find data histogram "+histname)
        continue
    hist_0.SetDirectory(0)
    #new_hist_0=(ROOT.TH1F)hist_0
    if histname is 'CutFlow':
        hist_0.SetName('CutFlow_Data')
    outfile.cd()
    hist_0.Write()
    
    
    
