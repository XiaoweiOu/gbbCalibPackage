#!/bin/bash

echo 'Checking for crashing jobs'

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' #no colour
#RED='tput setaf 1'
#GREEN='tput setaf 2'

DATE=$1

LOGPATH=/nfs/dust/atlas/user/ruth/Ruth/QualiTask/JobLogs

HOMEDIR=$PWD

cd $LOGPATH

NFAILS=0

for file in $(ls -ltr *.o* | grep "$DATE" | awk '{print $9}' )
do
    if ! grep -q 'Write Output File to:' $file ; then
	echo "Job failed, please check JobLog ${LOGPATH}/${file}"
	NFAILS=$((NFAILS+1))
    fi
done

if [ ${NFAILS} -eq 0 ] ; then
    echo -e "${GREEN}Congratulations: All your jobs were successful!"
else
    echo -e "${RED}Sorry, ${NFAILS} of your jobs failed!"
fi

cd $HOMEDIR