TARGTET_DIR=$(readlink -f .)
echo '*****************************************************'
BOOST_DIR="$TARGTET_DIR/boost_1_46_1"
GTEST_DIR="$TARGTET_DIR/gtest-1.7.0"
echo $BOOST_DIR
echo $TARGTET_DIR

echo '*****************************************************'
# -DCMAKE_CXX_COMPILER=g++-4.8
cmake . -B"build" -Wno-dev -DBOOST_FOLDER:STRING="$BOOST_DIR" -DGTEST_FOLDER:STRING="$GTEST_DIR"
#cd build
#make
#cd ..

