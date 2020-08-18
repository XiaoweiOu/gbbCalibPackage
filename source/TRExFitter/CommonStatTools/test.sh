#!/bin/bash

WORKSPACEFILE=/afs/cern.ch/user/v/vippolit/public/workspace.root
WORKSPACENAME=combined
MODELCONFIGNAME=ModelConfig
DATASETNAME=obsData
PARAMNAME=mass
PARAMVALUE=123
WORKSPACETAG=test
OUTPUTFOLDER=validation
DOBLIND=0
CL=0.95
FIGUREFORMAT=".pdf"
SAMPLES="background1,background2"
EVALUATIONREGIONS="channel1"
FITREGIONS="channel1"
ALGORITHM="PlotHistosAfterFitGlobal"

root -b -q runAsymptoticsCLs.C\(\"$WORKSPACEFILE\",\"$WORKSPACENAME\",\"$MODELCONFIGNAME\",\"$DATASETNAME\",\"$PARAMNAME\",$PARAMVALUE,\"$WORKSPACETAG\",\"$OUTPUTFOLDER\",$DOBLIND,$CL\)
root -b -q runSig.C\(\"$WORKSPACEFILE\",\"$WORKSPACENAME\",\"$MODELCONFIGNAME\",\"$DATASETNAME\",\"$PARAMNAME\",$PARAMVALUE,\"$WORKSPACETAG\",\"$OUTPUTFOLDER\",$DOBLIND\)
root -b -q getGlobalP0.C\(3.4,2\)
python compareHistos.py -o $WORKSPACEFILE -n $WORKSPACEFILE
python obtainHistosFromWS.py -i $WORKSPACEFILE -o validation_histos.root
root -b -q getCorrMatrix.C\(\"$WORKSPACEFILE\",\"$WORKSPACENAME\",\"$MODELCONFIGNAME\",\"$DATASETNAME\",\"$WORKSPACETAG\",\"$OUTPUTFOLDER\",\"$FIGUREFORMAT\"\)
root -b -q getHFtables.C\(\"$WORKSPACEFILE\",\"$WORKSPACENAME\",\"$MODELCONFIGNAME\",\"$DATASETNAME\",\"$WORKSPACETAG\",\"$OUTPUTFOLDER\",\"$EVALUATIONREGIONS\",\"$FITREGIONS\",kTRUE,kTRUE,\"$SAMPLES\"\)
root -b -q runFitCrossCheckForLimits.C\($ALGORITHM,\"$WORKSPACEFILE\",\"$WORKSPACENAME\",\"$MODELCONFIGNAME\",\"$DATASETNAME\",\"$OUTPUTFOLDER\",true,0,1\)
