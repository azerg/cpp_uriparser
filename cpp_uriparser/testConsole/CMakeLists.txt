set (test_executable_name cppUriparserTest)

add_executable (${test_executable_name} testMain.cpp uriparser_test.cpp query_test.cpp)

find_package(Boost 1.36.0)

include_directories ("${PROJECT_SOURCE_DIR}" "${URIPARSER_FOLDER}/include" "${GTEST_FOLDER}/include" "${BOOST_ROOT}")

if (WIN32)
    message ("---linking windows libs")
	target_link_libraries (${test_executable_name} optimized ${GTEST_LIBRARY} optimized ${URIPARSER_FOLDER}/win32/uriparser.lib)
	target_link_libraries (${test_executable_name} debug ${GTESTD_LIBRARY} debug ${URIPARSER_FOLDER}/win32/uriparserd.lib)
	
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
else()
	message ("---linking linux gtest libs")
	target_link_libraries (${test_executable_name} ${GTEST_LIBRARY})
	target_link_libraries (${test_executable_name} ${URIPARSER_FOLDER}/deploy/lib/liburiparser.so)
endif()