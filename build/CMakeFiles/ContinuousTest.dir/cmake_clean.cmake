file(REMOVE_RECURSE
  "generic/setup.sh"
  "generic/lib/WorkDir.rootmap"
  "CMakeFiles/ContinuousTest"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/ContinuousTest.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()