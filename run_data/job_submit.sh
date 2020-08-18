#!bin/bash

JobNumber=`ls | grep job_file_ - | wc -l`

echo $JobNumber

for((i=1;i<=$JobNumber;i++))
do
 echo "====== job submit $i ======"
 condor_submit job_file_${i}/job_file_${i}.sub
done
