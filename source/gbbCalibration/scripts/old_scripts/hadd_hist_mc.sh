
#!/bin/bash

#
# Ruth Jacobs 10-03-16
#
# Shell script
# add all output histograms of GbbTupleAna
#
# run: source hadd_hist PATH_TO_OUTPUTFOLDER/
#
#



HOMEDIR=${PWD}

FILEDIR=${1}

if [ -z "${FILEDIR}" ] 
then
    FILEDIR=${PWD}
fi

echo "File directory:  ${FILEDIR}"

cd ${FILEDIR}

for dir in $(ls -d *mc* | grep -v "42711")
do
    cd ${dir}
    if [ -e "merged_hist_${dir}" ]
    then
        rm "merged_hist_${dir}"
    fi
    echo `hadd -k merged_hist_${dir} *.root*`
    
    cd ${FILEDIR}

done


cd ${HOMEDIR}