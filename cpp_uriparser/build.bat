RMDIR /S /Q build
cmake . -B"build" -G"Visual Studio 12" -Wno-dev -DBOOST_FOLDER:STRING="F:\Work_Installs\boost_1_56_0" -DGTEST_FOLDER:STRING="F:\Work_Installs\gtest-1.7.0"