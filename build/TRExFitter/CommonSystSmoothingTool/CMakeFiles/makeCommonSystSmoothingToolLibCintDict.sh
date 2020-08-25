set -e
tmpdir=`mktemp -d makeCommonSystSmoothingToolLibCintDict.XXXXXX`
cd ${tmpdir}
/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/bin/rootcling -f CommonSystSmoothingToolLibCintDict.cxx -s libCommonSystSmoothingToolLib.so -rml libCommonSystSmoothingToolLib.so -rmf CommonSystSmoothingToolLib.dsomap -c -DHAVE_PRETTY_FUNCTION -DHAVE_64_BITS -D__IDENTIFIER_64BIT__ -DATLAS -DROOTCORE -DXAOD_STANDALONE -DXAOD_ANALYSIS -DROOTCORE_RELEASE_SERIES=25 -DPACKAGE_VERSION=\"CommonSystSmoothingTool-00-00-00\" -DPACKAGE_VERSION_UQ=CommonSystSmoothingTool-00-00-00 -I"/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonSystSmoothingTool" -I"/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonSystSmoothingTool" -I"/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonSystSmoothingTool" -I"/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonSystSmoothingTool" -I"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/include" -I"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/RootCore/include" -I"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/RootCore/include" -I"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/RootCore/include" -I"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/RootCore/include" "SmoothSystematics/PlotHist.h" "SmoothSystematics/SmoothHist.h" "SmoothSystematics/SmoothingTool.h" "SmoothSystematics/TRExTools.h" "Root/LinkDef.h"
/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/x86_64/Cmake/3.11.0/Linux-x86_64/bin/cmake -E copy CommonSystSmoothingToolLibCintDict.cxx  "/home/ouxiaowei/gbbCalibPackage/build/TRExFitter/CommonSystSmoothingTool/CMakeFiles/CommonSystSmoothingToolLibCintDict.cxx"
/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/x86_64/Cmake/3.11.0/Linux-x86_64/bin/cmake -E copy CommonSystSmoothingToolLib.dsomap "/home/ouxiaowei/gbbCalibPackage/build/TRExFitter/CommonSystSmoothingTool/CMakeFiles/CommonSystSmoothingToolLib.dsomap"
/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/x86_64/Cmake/3.11.0/Linux-x86_64/bin/cmake -E copy libCommonSystSmoothingToolLib_rdict.pcm "/home/ouxiaowei/gbbCalibPackage/build/generic/lib/libCommonSystSmoothingToolLib_rdict.pcm"
