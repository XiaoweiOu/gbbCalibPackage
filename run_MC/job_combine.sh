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

path=/home/ouxiaowei/gbbCalibPackage/run_MC/result_MC/Nominal
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

cd $present
