

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
ListOfHerwigCrossSections = [1.8831e7,173290.,2965.4,162.76,10.113] #in pb
#ListOfHerwigFilterEfficiencies = [1.9120e-05,4.2602e-5,9.0537e-5,0.00010259,4.7654e-5] #UPDATE for JZ3W 
ListOfHerwigFilterEfficiencies = [1.9e-05,4.8e-5,9.5e-5,0.000108,5.3e-5] 


#Herwig inclusive dijet
#ListOfHerwigInclusiveCrossSections = [1.8831e7,173300.,2965.4,162.76,10.113] #in pb
ListOfHerwigInclusiveCrossSections = [1.8116e7,167550.,2871.6,157.59,9.7879] #in pb
ListOfHerwigInclusiveFilterEfficiencies = [2.3548e-04,3.9541e-04,0.00071,0.000767,0.000346]  



ListOfSystematics = [ 'Nom' , 'JET_Rtrk_Baseline_Kin__1up', 'JET_Rtrk_Baseline_Kin__1down', 'JET_Rtrk_Modelling_Kin__1up', 'JET_Rtrk_Modelling_Kin__1down', 'JET_Rtrk_Tracking_Kin__1up', 'JET_Rtrk_Tracking_Kin__1down', 'JET_Rtrk_TotalStat_Kin__1up', 'JET_Rtrk_TotalStat_Kin__1down', 'JET_Rtrk_Baseline_Sub__1up', 'JET_Rtrk_Baseline_Sub__1down','JET_Rtrk_Modelling_Sub__1up', 'JET_Rtrk_Modelling_Sub__1down', 'JET_Rtrk_Tracking_Sub__1up','JET_Rtrk_Tracking_Sub__1down', 'JET_Rtrk_TotalStat_Sub__1up', 'JET_Rtrk_TotalStat_Sub__1down', 'FATJET_JMR__1up','FATJET_JER__1up','MUON_ID__1up','MUON_ID__1down','MUON_MS__1up','MUON_MS__1down','MUON_SCALE__1up','MUON_SCALE__1down','MUON_SAGITTA_RESBIAS__1up','MUON_SAGITTA_RESBIAS__1down','MUON_SAGITTA_RHO__1up','MUON_SAGITTA_RHO__1down','MUON_EFF_STAT__1up','MUON_EFF_STAT__1down','MUON_EFF_SYS__1up','MUON_EFF_SYS__1down','MUON_EFF_STAT_LOWPT__1up','MUON_EFF_STAT_LOWPT__1down','MUON_EFF_SYS_LOWPT__1up','MUON_EFF_SYS_LOWPT__1down','MUON_TTVA_STAT__1up','MUON_TTVA_STAT__1down','MUON_TTVA_SYS__1up','MUON_TTVA_SYS__1down']

basepath = '/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/'

pathData = basepath + 'Output_Calib/user.ruth.data1516_13TeV.all.root/merged_hist_user.ruth.data1516_13TeV.all.root'

ListOfMCPathsSys=[]
ListOfInclusiveMCPathsSys=[]

for sys in ListOfSystematics:
    if sys is 'Nom':
        sysdir='Output_Calib'
    else:
        sysdir='Output_Calib_'+sys

    pathJZ3W = basepath + sysdir + '/Calib_user.ruth.mc15_13TeV.427003.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07.e5660_s2726_r7773_r7676_p3323_tuple.root/merged_hist_Calib_user.ruth.mc15_13TeV.427003.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07.e5660_s2726_r7773_r7676_p3323_tuple.root' 
    pathJZ4W = basepath + sysdir + '/Calib_user.ruth.mc15_13TeV.427004.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07b.e5660_s2726_r7773_r7676_p3323_tuple.root/merged_hist_Calib_user.ruth.mc15_13TeV.427004.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07b.e5660_s2726_r7773_r7676_p3323_tuple.root' 
    pathJZ5W = basepath + sysdir + '/Calib_user.ruth.mc15_13TeV.427005.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07b.e5660_s2726_r7773_r7676_p3323_tuple.root/merged_hist_Calib_user.ruth.mc15_13TeV.427005.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07b.e5660_s2726_r7773_r7676_p3323_tuple.root' 
    pathJZ6W = basepath + sysdir + '/Calib_user.ruth.mc15_13TeV.427106.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07b.e5839_s2726_r7773_r7676_p3323_tuple.root/merged_hist_Calib_user.ruth.mc15_13TeV.427106.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07b.e5839_s2726_r7773_r7676_p3323_tuple.root' 
    pathJZ7W = basepath + sysdir + '/Calib_user.ruth.mc15_13TeV.427107.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07b.e5839_s2726_r7773_r7676_p3323_tuple.root/merged_hist_Calib_user.ruth.mc15_13TeV.427107.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07b.e5839_s2726_r7773_r7676_p3323_tuple.root' 

    
    ListOfMCPaths = [ pathJZ3W, pathJZ4W, pathJZ5W, pathJZ6W, pathJZ7W ]
    ListOfMCPathsSys.append(ListOfMCPaths)

    
    pathJZ3W_incl = basepath + sysdir + '/Calib_user.ruth.mc15_13TeV.361023.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07b.e3668_s2576_s2132_r7725_r7676_p3323_tuple.root/merged_hist_Calib_user.ruth.mc15_13TeV.361023.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07b.e3668_s2576_s2132_r7725_r7676_p3323_tuple.root'
    pathJZ4W_incl = basepath + sysdir + '/Calib_user.ruth.mc15_13TeV.361024.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07.e3668_s2576_s2132_r7725_r7676_p3323_tuple.root/merged_hist_Calib_user.ruth.mc15_13TeV.361024.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07.e3668_s2576_s2132_r7725_r7676_p3323_tuple.root'
    pathJZ5W_incl = basepath + sysdir + '/Calib_user.ruth.mc15_13TeV.361025.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07b.e3668_s2576_s2132_r7725_r7676_p3323_tuple.root/merged_hist_Calib_user.ruth.mc15_13TeV.361025.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07b.e3668_s2576_s2132_r7725_r7676_p3323_tuple.root'
    pathJZ6W_incl = basepath + sysdir + '/Calib_user.ruth.mc15_13TeV.361026.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07b.e3569_s2608_s2183_r7725_r7676_p3323_tuple.root/merged_hist_Calib_user.ruth.mc15_13TeV.361026.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07b.e3569_s2608_s2183_r7725_r7676_p3323_tuple.root'
    pathJZ7W_incl = basepath + sysdir + '/Calib_user.ruth.mc15_13TeV.361027.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07b.e3668_s2608_s2183_r7725_r7676_p3323_tuple.root/merged_hist_Calib_user.ruth.mc15_13TeV.361027.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07b.e3668_s2608_s2183_r7725_r7676_p3323_tuple.root'


    ListOfInclusiveMCPaths = [ pathJZ3W_incl, pathJZ4W_incl, pathJZ5W_incl, pathJZ6W_incl, pathJZ7W_incl ]
    ListOfInclusiveMCPathsSys.append(ListOfInclusiveMCPaths)

    if sys is 'Nom':
        pathJZ3W_H=basepath + sysdir + '/Calib_user.ruth.mc15_13TeV.427113.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07d.e5839_s2726_r7773_r7676_p3323_tuple.root/merged_hist_Calib_user.ruth.mc15_13TeV.427113.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07d.e5839_s2726_r7773_r7676_p3323_tuple.root'
        pathJZ4W_H=basepath + sysdir + '/Calib_user.ruth.mc15_13TeV.427114.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07c.e5839_s2726_r7773_r7676_p3323_tuple.root/merged_hist_Calib_user.ruth.mc15_13TeV.427114.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07c.e5839_s2726_r7773_r7676_p3323_tuple.root'
        pathJZ5W_H=basepath + sysdir + '/Calib_user.ruth.mc15_13TeV.427115.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07.e5839_s2726_r7773_r7676_p3323_tuple.root/merged_hist_Calib_user.ruth.mc15_13TeV.427115.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07.e5839_s2726_r7773_r7676_p3323_tuple.root'
        pathJZ6W_H=basepath + sysdir + '/Calib_user.ruth.mc15_13TeV.427116.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07.e5839_s2726_r7773_r7676_p3323_tuple.root/merged_hist_Calib_user.ruth.mc15_13TeV.427116.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07.e5839_s2726_r7773_r7676_p3323_tuple.root'
        pathJZ7W_H=basepath + sysdir + '/Calib_user.ruth.mc15_13TeV.427117.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07.e5839_s2726_r7773_r7676_p3323_tuple.root/merged_hist_Calib_user.ruth.mc15_13TeV.427117.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07.e5839_s2726_r7773_r7676_p3323_tuple.root'

        ListOfHerwigMCPaths = [ pathJZ3W_H, pathJZ4W_H, pathJZ5W_H, pathJZ6W_H, pathJZ7W_H]

        pathJZ3W_Hincl=basepath + sysdir +'/Calib_user.ruth.mc15_13TeV.426043.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07b.e4410_s2608_r7725_r7676_p3323_tuple.root/merged_hist_Calib_user.ruth.mc15_13TeV.426043.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07b.e4410_s2608_r7725_r7676_p3323_tuple.root'
        pathJZ4W_Hincl=basepath + sysdir +'/Calib_user.ruth.mc15_13TeV.426044.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07b.e4410_s2608_r7725_r7676_p3323_tuple.root/merged_hist_Calib_user.ruth.mc15_13TeV.426044.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07b.e4410_s2608_r7725_r7676_p3323_tuple.root'
        pathJZ5W_Hincl=basepath + sysdir + '/Calib_user.ruth.mc15_13TeV.426045.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07.e4410_s2608_r7725_r7676_p3323_tuple.root/merged_hist_Calib_user.ruth.mc15_13TeV.426045.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07.e4410_s2608_r7725_r7676_p3323_tuple.root'
        pathJZ6W_Hincl=basepath + sysdir + '/Calib_user.ruth.mc15_13TeV.426046.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07.e4410_s2608_r7725_r7676_p3323_tuple.root/merged_hist_Calib_user.ruth.mc15_13TeV.426046.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07.e4410_s2608_r7725_r7676_p3323_tuple.root'
        pathJZ7W_Hincl=basepath + sysdir + '/Calib_user.ruth.mc15_13TeV.426047.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07b.e4410_s2608_r7725_r7676_p3323_tuple.root/merged_hist_Calib_user.ruth.mc15_13TeV.426047.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07b.e4410_s2608_r7725_r7676_p3323_tuple.root'

        ListOfHerwigInclusiveMCPaths = [ pathJZ3W_Hincl, pathJZ4W_Hincl, pathJZ5W_Hincl, pathJZ6W_Hincl, pathJZ7W_Hincl]
        

       

#ListOfVariables = [ 'fjpt', 'nmjpt', 'mjpt', 'mjmaxSd0', 'nmjmaxSd0' ]
ListOfVariables = [ 'fjpt','fjptsc','fjm', 'fjD2','mjpt','nmjpt','mjeta','nmjeta', 'mjmaxSd0', 'nmjmaxSd0', 'fjpt_PREFITPOSTTAG','fjptsc_PREFITPOSTTAG', 'fjm_PREFITPOSTTAG', 'fjD2_PREFITPOSTTAG','mjpt_PREFITPOSTTAG','nmjpt_PREFITPOSTTAG','mjeta_PREFITPOSTTAG','nmjeta_PREFITPOSTTAG', 'mjmaxSd0_PREFITPOSTTAG', 'nmjmaxSd0_PREFITPOSTTAG','fjpt_PREFITPOSTTAG_BTAGUP', 'fjm_PREFITPOSTTAG_BTAGUP', 'fjD2_PREFITPOSTTAG_BTAGUP','mjpt_PREFITPOSTTAG_BTAGUP','nmjpt_PREFITPOSTTAG_BTAGUP','mjeta_PREFITPOSTTAG_BTAGUP','nmjeta_PREFITPOSTTAG_BTAGUP','fjptsc_PREFITPOSTTAG_BTAGUP', 'fjpt_PREFITPOSTTAG_BTAGDOWN', 'fjm_PREFITPOSTTAG_BTAGDOWN', 'fjD2_PREFITPOSTTAG_BTAGDOWN','mjpt_PREFITPOSTTAG_BTAGDOWN','nmjpt_PREFITPOSTTAG_BTAGDOWN','mjeta_PREFITPOSTTAG_BTAGDOWN','nmjeta_PREFITPOSTTAG_BTAGDOWN','fjptsc_PREFITPOSTTAG_BTAGDOWN','trjpt','fjtau21','fjtau21_PREFITPOSTTAG','fjtau21_PREFITPOSTTAG_BTAGUP','fjtau21_PREFITPOSTTAG_BTAGDOWN']#,'fjpt_PREFITANTITAG', 'allsrjpt' , 'srjN','evemu','slR4jpt','trjptfjptratio','trjptgbbcandratio' ]

#fatjet pt in eta/phi bins
#BinFragments=['g-2l-1','g-1l0','g0l1','g1l2']
#PhiBinFragments=['l-2','g-2l-1','g-1l0','g0l1','g1l2','g2']
#for a in BinFragments :
#    for b in BinFragments :
#        ListOfVariables.append('fjeta_'+a+'_fjphi_'+b+'_fjpt')

ListOfFlavourPairs = [ 'BB', 'BL', 'CC', 'CL']
ListOfInclusiveFlavourPairs = [ 'LL' ]

ListOfmjpt=[100,200]
#ListOfnmjpt=[20,50,80]
ListOfnmjpt=[100,200,300]

#mjpt_l100_nmjpt_l20
#ListOfTJpt=['mjpt_l100_nmjpt_l20','mjpt_l100_nmjpt_g20l50','mjpt_l100_nmjpt_g50l80', 'mjpt_l100_nmjpt_g80',
#            'mjpt_g100l200_nmjpt_l20','mjpt_g100l200_nmjpt_g20l50','mjpt_g100l200_nmjpt_g50l80','mjpt_g100l200_nmjpt_g80',
#            'mjpt_g200_nmjpt_l20','mjpt_g200_nmjpt_g20l50','mjpt_g200_nmjpt_g50l80', 'mjpt_g200_nmjpt_g80']

ListOfTJpt=['mjpt_l100_nmjpt_l100','mjpt_l100_nmjpt_g100l200','mjpt_l100_nmjpt_g200l300', 'mjpt_l100_nmjpt_g300',
            'mjpt_g100l200_nmjpt_l100','mjpt_g100l200_nmjpt_g100l200','mjpt_g100l200_nmjpt_g200l300','mjpt_g100l200_nmjpt_g300',
            'mjpt_g200_nmjpt_l100','mjpt_g200_nmjpt_g100l200','mjpt_g200_nmjpt_g200l300', 'mjpt_g200_nmjpt_g300']


outfilename = '180614_v00-01-07_CalibInputsLLinclusive_pt500_newTJBins_AllSys_Herwig_avgsd0SortPt.root'

#make list of histograms in Data
ListOfDataHists = []
for tjpt in ListOfTJpt :
    for var in ListOfVariables :
        ListOfDataHists.append('hDataNom_'+tjpt+'_'+var)
       # print('hDataNom_'+tjpt+'_'+var)

ListOfDataHists.append('CutFlow');

#open output file
outfile=ROOT.TFile(outfilename,"RECREATE")

ListOfHerwigHists=[]
ListOfLLHerwigHists=[]


for sys in ListOfSystematics:
    sys_index=ListOfSystematics.index(sys)
    ListOfMCPaths=ListOfMCPathsSys[sys_index]
    ListOfInclusiveMCPaths=ListOfInclusiveMCPathsSys[sys_index]
    print("Getting Hists for Systematic Variation: "+sys)

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
                    ListOfHists.append('h'+flavour+'Conversion__1up_'+tjpt+'_'+var)
                    ListOfHists.append('h'+flavour+'Conversion__1down_'+tjpt+'_'+var)
                    ListOfHists.append('h'+flavour+'HadMatInt__1up_'+tjpt+'_'+var)
                    ListOfHists.append('h'+flavour+'HadMatInt__1down_'+tjpt+'_'+var)
                    ListOfHists.append('h'+flavour+'LightLongLived__1up_'+tjpt+'_'+var)
                    ListOfHists.append('h'+flavour+'LightLongLived__1down_'+tjpt+'_'+var)

                    
                ListOfHists.append('h'+flavour+sys+'_'+tjpt+'_'+var)
                if sys is "Nom":
                    ListOfHerwigHists.append('h'+flavour+sys+'_'+tjpt+'_'+var)
                #print('h'+flavour+sys+'_'+tjpt+'_'+var)
                
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
                    ListOfLLHists.append('h'+flavour+'Conversion__1up_'+tjpt+'_'+var)
                    ListOfLLHists.append('h'+flavour+'Conversion__1down_'+tjpt+'_'+var)
                    ListOfLLHists.append('h'+flavour+'HadMatInt__1up_'+tjpt+'_'+var)
                    ListOfLLHists.append('h'+flavour+'HadMatInt__1down_'+tjpt+'_'+var)
                    ListOfLLHists.append('h'+flavour+'LightLongLived__1up_'+tjpt+'_'+var)
                    ListOfLLHists.append('h'+flavour+'LightLongLived__1down_'+tjpt+'_'+var)


                ListOfLLHists.append('h'+flavour+sys+'_'+tjpt+'_'+var)
                if sys is "Nom":
                    ListOfLLHerwigHists.append('h'+flavour+sys+'_'+tjpt+'_'+var)
                #print('h'+flavour+sys+'_'+tjpt+'_'+var)

    if sys is 'Nom':            
        ListOfLLHists.append('CutFlow')

    #for dummytests
    #continue
    
    #loop over MC histograms
    for histname in ListOfHists :
        hist_index=ListOfHists.index(histname)
        hist_0=None
        for path in ListOfMCPaths :
            index=ListOfMCPaths.index(path)
            #if index is 0 : #Skip JZ3W for now
                #hist_0=None
                #continue
    #for path in ListOfInclusiveMCPaths :
            #index=ListOfInclusiveMCPaths.index(path)
            file_curr=ROOT.TFile(path,"READ")
            if not file_curr :
                print("Cannot find file: "+path)
            bookkeep_hist=file_curr.Get("Hist_BookKeeping") #Events in AOD is in Bin 3
            if not bookkeep_hist :
                print("Cannot find BookKeeping Histogram in path" + path)
                break
                continue
            weight=ListOfCrossSections[index]*ListOfFilterEfficiencies[index]/bookkeep_hist.GetBinContent(3)*Lumi
            #weight=ListOfInclusiveCrossSections[index]*ListOfInclusiveFilterEfficiencies[index]/bookkeep_hist.GetBinContent(3)*Lumi
            #if hist_index==0:
                #print("weight is",weight)
            
            if not file_curr.GetListOfKeys().Contains(histname):
                #print("Cannot find hist "+histname)
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
                    if 'PREFITPOSTTAG' in histname:
                        help_name=histname.rsplit('_',1)[0]
                        if 'BTAGUP' in histname or 'BTAGDOWN' in histname:
                            help_name=histname.rsplit('_',2)[0]

                        hist_help=file_curr.Get(help_name)
                        print("Writing "+help_name+" with 0 entries instead")
                        hist_default=ROOT.TH1D(histname,"",hist_help.GetNbinsX(),hist_help.GetXaxis().GetXmin(),hist_help.GetXaxis().GetXmax())
                        hist_default.Write()

                if histname is 'CutFlow':
                    hist_0.SetName('CutFlow_MC')
                if hist_0 is not None:
                    hist_0.Write()


    #loop over inclusive MC for LL histograms
    for histname in ListOfLLHists :
        hist_index=ListOfLLHists.index(histname)
        #print('Hello')
        hist_0=None
        for path in ListOfInclusiveMCPaths :
            index=ListOfInclusiveMCPaths.index(path)
            #if index is 0:
                #hist_0=None
                #continue
            file_curr=ROOT.TFile(path,"READ")

            bookkeep_hist=file_curr.Get("Hist_BookKeeping") #Events in AOD is in Bin 3
            if not bookkeep_hist :
                print("Cannot find BookKeeping Histogram in path" + path)
                break
                #continue
            weight=ListOfInclusiveCrossSections[index]*ListOfInclusiveFilterEfficiencies[index]/bookkeep_hist.GetBinContent(3)*Lumi
            #if hist_index==0:
                #print("inclusive weight is",weight)
        

            if not file_curr.GetListOfKeys().Contains(histname):
                #print("Cannot find hist "+histname)
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
                if hist_0 is None :
                    print("ERROR: Could not find "+histname+" in all input Files!")
                    if 'PREFITPOSTTAG' in histname:
                        help_name=histname.rsplit('_',1)[0]
                        if 'BTAGUP' in histname or 'BTAGDOWN' in histname:
                            help_name=histname.rsplit('_',2)[0]    
                        hist_help=file_curr.Get(help_name)
                        
                        print("Writing "+help_name+" with 0 entries instead")
                        hist_default=ROOT.TH1D(histname,"",hist_help.GetNbinsX(),hist_help.GetXaxis().GetXmin(),hist_help.GetXaxis().GetXmax())
                        hist_default.Write()

                if histname is 'CutFlow':
                    hist_0.SetName('CutFlow_MC')
                #new_hist_0=(ROOT.TH1F)hist_0
                if hist_0 is not None:
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
    
    
#get Herwig histograms
doHerwig=1
if doHerwig>0:
    print("Getting Herwig Histograms.")
    for histname in ListOfHerwigHists :
        hist_index=ListOfHerwigHists.index(histname)
        hist_0=None
        for path in ListOfHerwigMCPaths :
            index=ListOfHerwigMCPaths.index(path)
            file_curr=ROOT.TFile(path,"READ")
            if not file_curr :
                print("Cannot find file: "+path)
            bookkeep_hist=file_curr.Get("Hist_BookKeeping") #Events in AOD is in Bin 3
            if not bookkeep_hist :
                print("Cannot find BookKeeping Histogram in path" + path)
                break
                continue
            weight=ListOfHerwigCrossSections[index]*ListOfHerwigFilterEfficiencies[index]/bookkeep_hist.GetBinContent(3)*Lumi
            #if hist_index==0:
                #print("weight is",weight)
            
            if not file_curr.GetListOfKeys().Contains(histname):
                #print("Cannot find hist "+histname)
                if index is 0: 
                    hist_0=None
                
            elif index is 0 or hist_0 is None :
                hist_0=file_curr.Get(histname)
                hist_0.SetDirectory(0)
                hist_0.Scale(weight)
            elif index is not 0 :
                hist_tmp=file_curr.Get(histname)
                hist_0.Add(hist_tmp,weight)

            if index==len(ListOfHerwigMCPaths)-1 :
                outfile.cd()
                if hist_0 is None :
                    print("ERROR: Could not find "+histname+" in all input Files!")
                    if 'PREFITPOSTTAG' in histname:
                        help_name=histname.rsplit('_',1)[0]
                        if 'BTAGUP' in histname or 'BTAGDOWN' in histname:
                            help_name=histname.rsplit('_',2)[0]

                        hist_help=file_curr.Get(help_name)
                        print("Writing "+help_name+" with 0 entries instead")
                        hist_default=ROOT.TH1D(histname.replace("Nom","Herwig__1up",1),"",hist_help.GetNbinsX(),hist_help.GetXaxis().GetXmin(),hist_help.GetXaxis().GetXmax())
                        hist_default.Write()

                if hist_0 is not None:
                    hist_0.SetName(histname.replace("Nom","Herwig__1up",1))
                    hist_0.Write()
    
    
    #loop over inclusive MC for LL histograms
    for histname in ListOfLLHerwigHists :
        hist_index=ListOfLLHerwigHists.index(histname)
        hist_0=None
        for path in ListOfHerwigInclusiveMCPaths :
            index=ListOfHerwigInclusiveMCPaths.index(path)
            file_curr=ROOT.TFile(path,"READ")

            bookkeep_hist=file_curr.Get("Hist_BookKeeping") #Events in AOD is in Bin 3
            if not bookkeep_hist :
                print("Cannot find BookKeeping Histogram in path" + path)
                break
                #continue
            weight=ListOfHerwigInclusiveCrossSections[index]*ListOfHerwigInclusiveFilterEfficiencies[index]/bookkeep_hist.GetBinContent(3)*Lumi
           # if hist_index==0:
                #print("inclusive weight is",weight)
        

            if not file_curr.GetListOfKeys().Contains(histname):
                #print("Cannot find hist "+histname)
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

            if index==len(ListOfHerwigInclusiveMCPaths)-1 :
                outfile.cd()
                if hist_0 is None :
                    print("ERROR: Could not find "+histname+" in all input Files!")
                    if 'PREFITPOSTTAG' in histname:
                        help_name=histname.rsplit('_',1)[0]
                        if 'BTAGUP' in histname or 'BTAGDOWN' in histname:
                            help_name=histname.rsplit('_',2)[0]    
                        hist_help=file_curr.Get(help_name)
                        
                        print("Writing "+help_name+" with 0 entries instead")
                        hist_default=ROOT.TH1D(histname.replace("Nom","Herwig__1up",1),"",hist_help.GetNbinsX(),hist_help.GetXaxis().GetXmin(),hist_help.GetXaxis().GetXmax())
                        hist_default.Write()

                if hist_0 is not None:
                    hist_0.SetName(histname.replace("Nom","Herwig__1up",1))
                    hist_0.Write()
    



#  LocalWords:  sys
