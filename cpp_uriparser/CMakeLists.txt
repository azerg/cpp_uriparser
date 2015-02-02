cmake_minimum_required(VERSION 2.8)

# leaving only Release& Debug configurations
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug Release)
   set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
     "Reset the configurations to what we need"
     FORCE)  
ENDIF(NOT CMAKE_BUILD_TYPE)

set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin )

##################################################################################################################
project (cpp_uriparser_test)

message ("Starting build on "+${CMAKE_SYSTEM})
message ("${CMAKE_CXX_FLAGS_RELEASE}")
message ("-------------")

add_subdirectory (testConsole)
