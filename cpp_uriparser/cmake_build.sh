#cmake . -B"build" -G"Visual Studio 12" -Wno-dev -DBOOST_FOLDER:STRING="F:\Work_Installs\boost_1_56_0" -#DGTEST_FOLDER:STRING="F:\Work_Installs\gtest-1.7.0"

TARGTET_DIR=$(readlink -f .)
echo '*****************************************************'
BOOST_DIR="$TARGTET_DIR/boost_1_46_1"
GTEST_DIR="$TARGTET_DIR/gtest-1.7.0"
echo $BOOST_DIR
echo $TARGTET_DIR

echo '*****************************************************'
cmake . -B"build" -Wno-dev -DBOOST_FOLDER:STRING="$BOOST_DIR" -DGTEST_FOLDER:STRING="$GTEST_DIR"
cd build
#make
#cd ..

