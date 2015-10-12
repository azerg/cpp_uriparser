cmake_minimum_required(VERSION 2.8)

set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin )
set ( URIPARSER_FOLDER ${CMAKE_HOME_DIRECTORY}/dependencies/uriparser-0.8.2 )
set ( BOOST_FOLDER ${BOOST_ROOT} )

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
message ("${CMAKE_VS_PLATFORM_TOOLSET}")
message ("-------------")

add_subdirectory (testConsole)
