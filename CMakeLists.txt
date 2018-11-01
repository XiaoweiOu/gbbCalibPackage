################################################################################
# Build configuration for GbbCalibration
################################################################################

# Declare the name of the package:
atlas_subdir( GbbCalibration )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC 
   Tools/PathResolver
   )

find_package( ROOT COMPONENTS Core MathCore RIO Hist Tree Matrix Gpad Graf Physics Minuit)

# Build a dictionary for the library:
atlas_add_root_dictionary( GbbTupleLib _dictionarySource
   ROOT_HEADERS GbbCalibration/TupleAna.h GbbCalibration/GbbTupleAna.h Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

atlas_add_library( GbbTupleLib
   Root/GbbTupleAna.cxx Root/GbbSelectionHelp.cxx Root/TupleAna.cxx
   Root/HistogramService.cxx Root/FlavourFracCorrector.cxx
   GbbCalibration/GbbTupleAna.h GbbCalibration/TupleAna.h GbbCalibration/HistogramService.h GbbCalibration/FlavourFracCorrector.h
   Root/*.h ${_dictionarySource}
   PUBLIC_HEADERS GbbTupleAna
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} PathResolver
   )
atlas_add_library( ScaleFactorCalcLib
   Root/ScaleFactorCalculator.cxx Root/FitData.cxx Root/Fitter.cxx Root/Config.cxx
   GbbCalibration/ScaleFactorCalculator.h GbbCalibration/FitData.h GbbCalibration/Fitter.h GbbCalibration/Config.h
   PUBLIC_HEADERS ScaleFactorCalc
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} PathResolver
   )

# Install files from the package:
atlas_add_executable( run_gbbTupleAna
   util/run_gbbTupleAna.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} GbbTupleLib
   )
atlas_add_executable( run_calculateSF
   util/run_calculateSF.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ScaleFactorCalcLib
   )

atlas_install_python_modules( python/*.py )
atlas_install_data( data/* )
