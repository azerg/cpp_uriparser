RMDIR /S /Q build
::**********************************************************************
set BOOST_ROOT="F:/Work_Installs/boost_1_56_0"
set GTEST_FOLDER="F:/Work_Installs/gtest-1.7.0"
set GTEST_LIBRARY=%GTEST_FOLDER%/msvc/gtest/Release/gtest.lib
set GTESTD_LIBRARY=%GTEST_FOLDER%/msvc/gtest/Debug/gtestd.lib
::**********************************************************************
cmake . -B"build" -G"Visual Studio 14" -Wno-dev -DBOOST_ROOT=%BOOST_ROOT% -DGTEST_FOLDER=%GTEST_FOLDER% -DGTEST_LIBRARY=%GTEST_LIBRARY% -DGTESTD_LIBRARY=%GTESTD_LIBRARY%