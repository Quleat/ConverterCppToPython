# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\ConverterCppToPython_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\ConverterCppToPython_autogen.dir\\ParseCache.txt"
  "ConverterCppToPython_autogen"
  )
endif()
