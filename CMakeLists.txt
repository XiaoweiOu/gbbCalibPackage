################################################################################
# Build configuration for GbbCalibration
################################################################################

# Declare the name of the package:
atlas_subdir( GbbCalibration )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC 
   )

find_package( ROOT COMPONENTS Core RIO Hist Tree)

# Install files from the package:
atlas_add_executable( run_gbbTupleAna
   util/run_gbbTupleAna.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES}
   )
atlas_add_executable( run_calculateSF
   util/run_calculateSF.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES}
   )

atlas_install_python_modules( python/*.py )
atlas_install_data( data/* )
