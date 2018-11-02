#!/bin/bash                                                                                                                                                                                                                                
 
#                                                                                                                                                                                                                                          
# Ruth Jacobs 16-07-01                                                                                                                                                                                                                     
#                                                                                                                                                                                                                                          
# Shell script                                                                                                                                                                                                                             
# add all output histograms of GbbTupleAna                                                                                                                                                                                                 
#                                                                                                                                                                                                                                          
# run: bash hadd_hist_allsys.sh                                                                                                                                                                                              
#                                                                                                                                                                                                                                          
# 

echo "Execute hadd for Nominal"
#bash scripts/NAFScripts/hadd_hist.sh /nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/Output_Reweight
bash scripts/NAFScripts/hadd_hist.sh /nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/Output_ReweightNew
##bash scripts/NAFScripts/hadd_hist.sh /nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/Output_Topo


for sys in JET_Rtrk_Baseline_Sub JET_Rtrk_Tracking_Sub JET_Rtrk_Modelling_Sub JET_Rtrk_TotalStat_Sub JET_Rtrk_Baseline_Kin JET_Rtrk_Tracking_Kin JET_Rtrk_Modelling_Kin JET_Rtrk_TotalStat_Kin 
do
    echo "Execute hadd for ${sys}"
    for token in __1up __1down
    do
	echo "Piep"
	#bash scripts/NAFScripts/hadd_hist_mc.sh /nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/Output_CrossCheck_${sys}${token}
    done

done


for sys1s in FATJET_JER FATJET_JMR
do
    echo "Execute hadd for ${sys1s}"
    for token1s in __1up 
    do
	echo "piep"
	#bash scripts/NAFScripts/hadd_hist_mc.sh /nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/Output_CrossCheck_${sys1s}${token1s}
    done

done


