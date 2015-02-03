set (test_executable_name cppUriparserTest)

add_executable (${test_executable_name} testMain.cpp uriparser_test.cpp query_test.cpp)

set (URIPARSER_FOLDER "${PROJECT_SOURCE_DIR}/dependencies/uriparser-0.8.1")
include_directories ("${PROJECT_SOURCE_DIR}" "${URIPARSER_FOLDER}/include" "${GTEST_FOLDER}/include" "${BOOST_FOLDER}")

if (WIN32)
    message ("---linking windows libs")
	target_link_libraries (${test_executable_name} optimized ${GTEST_FOLDER}/msvc/gtest/Release/gtest.lib)
	target_link_libraries (${test_executable_name} debug ${GTEST_FOLDER}/msvc/gtest/Debug/gtestd.lib)
	target_link_libraries (${test_executable_name} ${URIPARSER_FOLDER}/win32/uriparser.lib)
	
	########################################################################
	# replacing MD with MT, MDd with MTd
    set(variables CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
	foreach(variable ${variables})
		if(${variable} MATCHES "/MD")
		  string(REGEX REPLACE "/MD" "/MT" ${variable} "${${variable}}")
		elseif(${variable} MATCHES "/MDd")
		  string(REGEX REPLACE "/MDd" "/MTd" ${variable} "${${variable}}")
		endif()		
	endforeach()
	########################################################################
else()
	message ("---linking linux gtest libs")
	target_link_libraries (${test_executable_name} ${GTEST_FOLDER}/cmake/libgtest.so)
	target_link_libraries (${test_executable_name} ${URIPARSER_FOLDER}/deploy/lib/liburiparser.so)
endif()