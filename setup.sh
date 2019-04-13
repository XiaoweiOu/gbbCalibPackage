source /global/project/projectdirs/atlas/scripts/setupATLAS.sh
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh
cd source
if [ -f ".asetup.save" ]; then
  asetup --restore
else
  asetup AnalysisBase,21.2.35,here
fi
cd ..
source build/x86_64-suse123-gcc62-opt/setup.sh
