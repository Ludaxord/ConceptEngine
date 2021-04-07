# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\ConceptEditor_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\ConceptEditor_autogen.dir\\ParseCache.txt"
  "ConceptEditor_autogen"
  )
endif()
