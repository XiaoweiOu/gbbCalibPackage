export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh

cd source
asetup AnalysisBase,21.2.83,here
cd ..

source ./build/generic/setup.sh
# set paths to find config and executable files
export GBB_BUILD_DIR=`pwd`/build
