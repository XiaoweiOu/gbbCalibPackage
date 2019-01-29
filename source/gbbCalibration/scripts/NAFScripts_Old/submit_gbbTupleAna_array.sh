#!/bin/bash

#Submit script for gbb Tuple Ana
echo "Submit jobs gbbTupleAna......"

DO_SYS=${1}

CFG_NAME=${2}

OUT_TAG=${3}

if [ -z "${DO_SYS}" ] 
    then
    DO_SYS=0
    echo "Running without systematics, to turn on, do: source scripts/submit_gbbTupleAna_array.sh 1"
    echo "To run only systematics run: source scripts/submit_gbbTupleAna_array.sh 2"
fi

INPATH="/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Inputs/FTagNTuples_Calib/v00-01-07/"
#INPATH="/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Inputs/FTagNtuples"
#OUTPATH="/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/Output_CrossCheck"
OUTPATH="/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/Output_${OUT_TAG}"
LOG_FOLDER="/nfs/dust/atlas/user/ruth/Ruth/QualiTask/JobLogs/"

#for now don't run over Herwig samples
listOfSamplesNoHerwig=$(ls -d ${INPATH}/*v00-01-07* | xargs -n1 basename | grep -v "42711" | grep -v "42604")
listOfMCSamplesNoHerwig=$(ls -d ${INPATH}/*mc15*v00-01-07* | xargs -n1 basename | grep -v "42711" | grep -v "42604")
listOfSamples=$(ls -d ${INPATH}/*v00-01-07* | xargs -n1 basename )
listOfMCSamples=$(ls -d ${INPATH}/*mc15*v00-01-07* | xargs -n1 basename)
listOfDataSamples=$(ls -d ${INPATH}/*data*v00-01-07* | xargs -n1 basename)
listOfHerwigMCSamples=$(ls -d ${INPATH}/*mc15*v00-01-07* | xargs -n1 basename | grep -e "42711" -e "42604")
listOfTestMCSamples=$(ls -d ${INPATH}/*mc15*v00-01-07* | xargs -n1 basename | grep "3610")
#listOfSamples=$(ls -d ${INPATH}/*v00-01-03* | xargs -n1 basename)
#listOfMCSamples=$(ls -d ${INPATH}/*mc15*v00-01-03* | xargs -n1 basename)

#echo ${listOfSamples}
#echo ${listOfMCSamples}

echo "Logs in: ${LOG_FOLDER}"
echo "Output in: ${OUTPATH}"

NFILESPERJOB=10


#=========================
#Nominal
#=========================

if [ "$DO_SYS" -ne 2 ]
then
    echo "Running Nominal!!!"
    mkdir -p /nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/Output_${OUT_TAG}

    #for dir in /nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/Output_${OUT_TAG}/${OUT_TAG}_*.root
    #do
	#if [ -d "${dir}" ];
	#then
	    #rm -rv $dir
	    #echo "directory exists: ${dir}"
	#fi
    #done


    
    #for samp in ${listOfTestMCSamples}
    #for samp in ${listOfSamples}
    #for samp in ${listOfSamplesNoHerwig}
    for samp in ${listOfMCSamples}
    #for samp in ${listOfDataSamples}
    #for samp in ${listOfMCSamplesNoHerwig}
    do
	echo "Sample is: $samp"
	
	NTUP=$(ls ${INPATH}/${samp})
	export NTUP
	NJOBS=$(echo ${NTUP} | wc -w)
	NJOBS_SPLIT=$(echo $((NJOBS/NFILESPERJOB)) )
	if (( ${NJOBS} % ${NFILESPERJOB} != 0 )); then NJOBS_SPLIT=$((NJOBS_SPLIT+1)); fi 
	if [ ${NJOBS_SPLIT} -eq 0 ]; then NJOBS_SPLIT=1; fi

	echo "Number of jobs ${NJOBS}"
	echo "Number of split jobs ${NJOBS_SPLIT}"

	if [ -d "${OUTPATH}/${OUT_TAG}_${samp}" ];
	then
	    echo "directory exists: ${OUTPATH}/${OUT_TAG}_${samp}"  
	    rm -rv ${OUTPATH}/${OUT_TAG}_${samp}
	fi

        #   echo "Ntup: ${NTUP}"

###	qsub -t 1-${NJOBS} -o ${LOG_FOLDER} -j y -l h_rt=01:00:00,distro=sld6,h_vmem=4G -v IN_PATH="${INPATH}",SAMP="${samp}",OUTDIR="${OUTPATH}/${OUT_TAG}_${samp}",TREENAME="FlavourTagging_Nominal",CONFIGNAME="${CFG_NAME}" scripts/NAFScripts/run_gbbTupleAna_array.sh  
###	qsub -t 1-${NJOBS_SPLIT} -o ${LOG_FOLDER} -j y -l h_rt=01:00:00,distro=sld6,h_vmem=6G -v IN_PATH="${INPATH}",SAMP="${samp}",OUTDIR="${OUTPATH}/${OUT_TAG}_${samp}",TREENAME="FlavourTagging_Nominal",CONFIGNAME="${CFG_NAME}",N_FILESPERJOB="${NFILESPERJOB}" scripts/NAFScripts/run_gbbTupleAna_arrayList.sh
condor_submit  IN_PATH="${INPATH}" SAMP="${samp}" OUTDIR="${OUTPATH}/${OUT_TAG}_${samp}" TREENAME="FlavourTagging_Nominal" CONFIGNAME="${CFG_NAME}" N_FILESPERJOB="${NFILESPERJOB}" NJOBS="${NJOBS_SPLIT}" scripts/HTCscripts/run_gbbTupleAna_array.sub
	
    done
fi

#========================
# 2-sided systematics
#========================

if [ "$DO_SYS" -gt 0 ]
then
    echo "Running 2-sided Kin systematics!!!"

    for sys in JET_Rtrk_Baseline_Kin JET_Rtrk_Modelling_Kin JET_Rtrk_Tracking_Kin JET_Rtrk_TotalStat_Kin JET_Rtrk_Baseline_Sub JET_Rtrk_Modelling_Sub JET_Rtrk_Tracking_Sub JET_Rtrk_TotalStat_Sub JET_JvtEfficiency
#MUON_ID MUON_MS MUON_SCALE MUON_SAGITTA_RESBIAS MUON_SAGITTA_RHO MUON_EFF_STAT MUON_EFF_SYS MUON_EFF_STAT_LOWPT MUON_EFF_SYS_LOWPT MUON_TTVA_STAT MUON_TTVA_SYS 
    do
	for var in __1up __1down
	do
	    mkdir -p /nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/Output_${OUT_TAG}_${sys}$var

	    for dir in /nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/Output_${OUT_TAG}_${sys}$var/*.root
	    do
		if [ -d "${dir}" ];
		then
		    rm -rv $dir
		fi
	    done


	    
	    for samp in ${listOfMCSamplesNoHerwig}
	    do
		NTUP=$(ls ${INPATH}/${samp})
		export NTUP
		NJOBS=$(echo ${NTUP} | wc -w)
		#NJOBS=$(expr ${NJOBS} - 1)
		NJOBS_SPLIT=$(echo $((NJOBS/NFILESPERJOB)) )
		if (( ${NJOBS} % ${NFILESPERJOB} != 0 )); then NJOBS_SPLIT=$((NJOBS_SPLIT+1)); fi 
		if [ ${NJOBS_SPLIT} -eq 0 ]; then NJOBS_SPLIT=1; fi

		echo "Number of jobs ${NJOBS}"
		echo "Number of split jobs ${NJOBS_SPLIT}"


		
###		qsub -t 1-${NJOBS} -o ${LOG_FOLDER} -j y -l h_rt=01:00:00,distro=sld6,h_vmem=4G -v IN_PATH="${INPATH}",SAMP="${samp}",OUTDIR="${OUTPATH}_${sys}${var}/${OUT_TAG}_${samp}",TREENAME="FlavourTagging_${sys}${var}",CONFIGNAME="${CFG_NAME}" scripts/NAFScripts/run_gbbTupleAna_array.sh
###		qsub -t 1-${NJOBS_SPLIT} -o ${LOG_FOLDER} -j y -l h_rt=01:00:00,distro=sld6,h_vmem=4G -v IN_PATH="${INPATH}",SAMP="${samp}",OUTDIR="${OUTPATH}_${sys}${var}/${OUT_TAG}_${samp}",TREENAME="FlavourTagging_${sys}${var}",CONFIGNAME="${CFG_NAME}",N_FILESPERJOB="${NFILESPERJOB}" scripts/NAFScripts/run_gbbTupleAna_arrayList.sh
condor_submit IN_PATH="${INPATH}" SAMP="${samp}" OUTDIR="${OUTPATH}_${sys}${var}/${OUT_TAG}_${samp}" TREENAME="FlavourTagging_${sys}${var}" CONFIGNAME="${CFG_NAME}" N_FILESPERJOB="${NFILESPERJOB}" NJOBS="${NJOBS_SPLIT}" scripts/HTCscripts/run_gbbTupleAna_array.sub

	    done


	done

    done
fi



#========================                                                                                                                                                                                                                   
# 1-sided systematics                                                                                                                                                                       
#========================

if [ "$DO_SYS" -gt 0 ]
then
    echo "Running 1-sided systematics!!!"

    for sys in FATJET_JER FATJET_JMR
    do
	for var in __1up
	do
	     mkdir -p /nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/Output_${OUT_TAG}_${sys}$var

            for dir in /nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/Output_${OUT_TAG}_${sys}$var/*.root
            do
		if [ -d "${dir}" ];
		    then
                    rm -rv $dir
		fi
            done



            for samp in ${listOfMCSamplesNoHerwig}
            do
		NTUP=$(ls ${INPATH}/${samp})
		export NTUP
		NJOBS=$(echo ${NTUP} | wc -w)
		#NJOBS=$(expr ${NJOBS} - 1)
		NJOBS_SPLIT=$(echo $((NJOBS/NFILESPERJOB)) )
		if (( ${NJOBS} % ${NFILESPERJOB} != 0 )); then NJOBS_SPLIT=$((NJOBS_SPLIT+1)); fi 
		if [ ${NJOBS_SPLIT} -eq 0 ]; then NJOBS_SPLIT=1; fi

###		qsub -t 1-${NJOBS} -o ${LOG_FOLDER} -j y -l h_rt=01:00:00,distro=sld6,h_vmem=4G -v IN_PATH="${INPATH}",SAMP="${samp}",OUTDIR="${OUTPATH}_${sys}${var}/${OUT_TAG}_${samp}",TREENAME="FlavourTagging_${sys}${var}",CONFIGNAME="${CFG_NAME}" scripts/NAFScripts/run_gbbTupleAna_array.sh
###		qsub -t 1-${NJOBS_SPLIT} -o ${LOG_FOLDER} -j y -l h_rt=01:00:00,distro=sld6,h_vmem=4G -v IN_PATH="${INPATH}",SAMP="${samp}",OUTDIR="${OUTPATH}_${sys}${var}/${OUT_TAG}_${samp}",TREENAME="FlavourTagging_${sys}${var}",CONFIGNAME="${CFG_NAME}",N_FILESPERJOB="${NFILESPERJOB}" scripts/NAFScripts/run_gbbTupleAna_arrayList.sh
condor_submit IN_PATH="${INPATH}" SAMP="${samp}" OUTDIR="${OUTPATH}_${sys}${var}/${OUT_TAG}_${samp}" TREENAME="FlavourTagging_${sys}${var}" CONFIGNAME="${CFG_NAME}" N_FILESPERJOB="${NFILESPERJOB}" NJOBS="${NJOBS_SPLIT}" scripts/HTCscripts/run_gbbTupleAna_array.sub
            done


done

    done
fi
