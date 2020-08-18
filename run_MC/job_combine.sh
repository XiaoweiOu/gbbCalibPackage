#!/bin/bash

present=`pwd`

#tupe file
SEND_THREAD_NUM=10
tmp_fifofile=$$.fifo
mkfifo $tmp_fifofile
exec 1000<>"$tmp_fifofile"
rm -f $tmp_fifofile

for ((i=0;i<$SEND_THREAD_NUM;i++))
do
         echo >&1000
done

#test the number of parameter
if (($#!=2));then
echo "ERROR:the number of parameter is invalid!"
echo "usage:systematics? 0 / 1 / 2 -- nom / nom+sys / sys"
exit
fi

DO_SYS=${1}

#syslist
syslist=("JET_EffectiveNP_R10_6restTerm__1up" "JET_EffectiveNP_R10_6restTerm__1down" "JET_EtaIntercalibration_Modelling__1up" "JET_EtaIntercalibration_Modelling__1down" "JET_EtaIntercalibration_NonClosure_2018data__1up" "JET_EtaIntercalibration_NonClosure_2018data__1down" "JET_EtaIntercalibration_R10_TotalStat__1up" "JET_EtaIntercalibration_R10_TotalStat__1down" "JET_EffectiveNP_R10_1__1up" "JET_EffectiveNP_R10_1__1down" "JET_EffectiveNP_R10_2__1up" "JET_EffectiveNP_R10_2__1down" "JET_EffectiveNP_R10_3__1up" "JET_EffectiveNP_R10_3__1down" "JET_EffectiveNP_R10_4__1up" "JET_EffectiveNP_R10_4__1down" "JET_EffectiveNP_R10_5__1up" "JET_EffectiveNP_R10_5__1down" "JET_Flavor_Composition__1up" "JET_Flavor_Composition__1down" "JET_Flavor_Response__1up" "JET_Flavor_Response__1down" "MUON_ID__1up" "MUON_ID__1down" "MUON_SAGITTA_RESBIAS__1up" "MUON_SAGITTA_RESBIAS__1down" "MUON_SCALE__1up" "MUON_SCALE__1down" "MUON_EFF_TTVA_SYS__1up" "MUON_EFF_TTVA_SYS__1down" "TRK_RES_D0_MEAS__1up" "TRK_RES_D0_MEAS__1down" "TRK_RES_Z0_MEAS__1up" "TRK_RES_Z0_MEAS__1down" "TRK_RES_D0_DEAD" "TRK_RES_Z0_DEAD" "TRK_BIAS_D0_WM" "TRK_BIAS_QOVERP_SAGITTA_WM" "TRK_BIAS_Z0_WM" "TRK_EFF_LOOSE_GLOBAL" "TRK_EFF_LOOSE_IBL" "TRK_EFF_LOOSE_PHYSMODEL" "TRK_EFF_LOOSE_PP0" "TRK_FAKE_RATE_LOOSE" "TRK_EFF_LOOSE_TIDE" "TRK_FAKE_RATE_LOOSE_TIDE")

# systematics explanation
if [ "${DO_SYS}" -eq 0 ]
then
  varlist=("Nominal")
  echo "Running nominal only (setting ${DO_SYS})"
  echo
elif [ "${DO_SYS}" -eq 1 ]
then
  varlist=("Nominal" "${syslist[@]}")
  echo "Running nominal + systematics (setting ${DO_SYS})"
  echo
else
  varlist=("${syslist[@]}")
  echo "Running systematics only (setting ${DO_SYS})"
  echo
fi

echo "Varlist: ${varlist[@]}"
echo

for var in ${varlist[@]}  #loop of varlist 
do

path=/home/ouxiaowei/gbbCalibPackage/run_MC/result_MC/${var}
files=`ls $path`
for file in $files
do
 read -u1000
 {
 cd $path"/"$file
 RootName=(`ls`)
 RootNum=`ls | wc -l`

 cal_list=0
 cal_file=0

 while ((RootNum>0))
 do
  #echo $RootNum
  cal_list=$[cal_list+1]
  if ((5<RootNum));then
   min=5
   else min=$RootNum
  fi
  
  command="hadd result_$cal_list.root"
  for ((i=1;i<=min;i++))
  do
   command="$command ${RootName[$cal_file]}"
   cal_file=$[cal_file+1]
  done
  
  echo "$command"
  #execute command:
  $command

  RootNum=$[RootNum-min]
 done

 #add whole root file
 rm outhist_$file.root
 AddWhole="hadd outhist_$file.root"
 for ((i=1;i<=cal_list;i++))
 do
  AddWhole="$AddWhole result_$i.root"
 done

 echo "$AddWhole"
 #execute AddWhole:
 $AddWhole

 #remove help root file
 for ((i=1;i<=cal_list;i++))
 do
  rm result_$i.root
 done 
 
 echo >&1000

 }&
done
done

cd $present
