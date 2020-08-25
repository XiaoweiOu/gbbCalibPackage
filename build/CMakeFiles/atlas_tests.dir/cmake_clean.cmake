file(REMOVE_RECURSE
  "generic/setup.sh"
  "generic/lib/WorkDir.rootmap"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/atlas_tests.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
