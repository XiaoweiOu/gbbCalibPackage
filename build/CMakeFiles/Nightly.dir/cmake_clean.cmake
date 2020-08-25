file(REMOVE_RECURSE
  "generic/setup.sh"
  "generic/lib/WorkDir.rootmap"
  "CMakeFiles/Nightly"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/Nightly.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
