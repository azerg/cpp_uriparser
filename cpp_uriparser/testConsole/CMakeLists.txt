add_executable (cppUriparserTest testMain.cpp uriparser_test.cpp query_test.cpp)

set (URIPARSER_FOLDER "${PROJECT_SOURCE_DIR}/dependencies/uriparser-0.8.1/include")
include_directories ("${PROJECT_SOURCE_DIR}" "${URIPARSER_FOLDER}" "${GTEST_FOLDER}/include" "${BOOST_FOLDER}")

if (WIN32)
    message ("---linking windows libs")
	target_link_libraries (cppUriparserTest optimized ${GTEST_FOLDER}/msvc/gtest/Release/gtest.lib)
	target_link_libraries (cppUriparserTest debug ${GTEST_FOLDER}/msvc/gtest/Debug/gtestd.lib)
else()
	message ("---linking linux gtest libs")
	target_link_libraries (cppUriparserTest ${GTEST_FOLDER}/cmake/libgtest.so)
endif()