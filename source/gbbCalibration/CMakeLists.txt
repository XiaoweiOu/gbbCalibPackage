################################################################################
# Build configuration for gbbCalibration
################################################################################

# Declare the name of the package:
atlas_subdir( gbbCalibration )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Tools/PathResolver
   )

find_package( ROOT COMPONENTS Core MathCore RIO Hist Tree Matrix Gpad Graf Physics Minuit)

# Build a dictionary for the library:
atlas_add_root_dictionary( gbbCommonLib gbbCommonDictSource
   ROOT_HEADERS helpers/GbbUtil.h helpers/GlobalConfig.h helpers/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

atlas_add_library( gbbCommonLib
   helpers/GbbUtil.cxx helpers/GlobalConfig.cxx helpers/GlobalConfig.h
   ${gbbCommonDictSource}
   PUBLIC_HEADERS helpers
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} PathResolver
   )
atlas_add_library( gbbTupleLib
   histMaker/*.cxx histMaker/*.h
   histMaker/BTagging/*.cxx histMaker/BTagging/*.h
   PUBLIC_HEADERS histMaker
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} PathResolver xAODBTaggingEfficiencyLib gbbCommonLib
   )
atlas_add_library( ScaleFactorCalcLib
   scaleFactorCalculator/*.cxx scaleFactorCalculator/*.h
   helpers/atlasstyle/*.C helpers/atlasstyle/*.h
   PUBLIC_HEADERS scaleFactorCalculator
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} PathResolver gbbCommonLib
   )

# Install files from the package:
atlas_add_executable( run_gbbTupleAna
   util/run_gbbTupleAna.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} gbbTupleLib
   )
atlas_add_executable( run_calculateSF
   util/run_calculateSF.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ScaleFactorCalcLib
   )

## add a test exe
#atlas_add_executable( run_GbbTest
#  tests/run_tests.cxx
#  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
#  LINK_LIBRARIES ${ROOT_LIBRARIES} gbbTupleLib
#  )

#atlas_install_python_modules( python/makeReweightHistos.py LINK_LIBRARIES gbbCommonLib )
#atlas_install_python_modules( python/*.py )
atlas_install_data( data/* )
