cmake_minimum_required(VERSION 2.8)

# leaving only Release& Debug configurations
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug Release)
   set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
     "Reset the configurations to what we need"
     FORCE)  
ENDIF(NOT CMAKE_BUILD_TYPE)

set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin )
set ( URIPARSER_FOLDER ${CMAKE_HOME_DIRECTORY}/dependencies/uriparser-0.8.2 )

##################################################################################################################
project (cpp_uriparser_test)

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} -g -ftest-coverage -fprofile-arcs")
endif()

message ("Starting build on "+${CMAKE_SYSTEM})

message ("${CMAKE_CXX_FLAGS_RELEASE} :: ${CMAKE_CXX_FLAGS}")
message ("BOOST_FOLDER = ${BOOST_FOLDER}")
message ("GTEST_FOLDER = ${GTEST_FOLDER}")
message ("${CMAKE_CXX_COMPILER}")
message ("-------------")

add_subdirectory (testConsole)
