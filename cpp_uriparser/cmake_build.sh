TARGTET_DIR=$(readlink -f .)
echo '*****************************************************'
BOOST_ROOT="$TARGTET_DIR/boost_1_46_1"
GTEST_DIR="$TARGTET_DIR/gtest-1.7.0"
echo $BOOST_DIR
echo $TARGTET_DIR

echo '*****************************************************'

cmake . -B"build" -Wno-dev -DBOOST_ROOT="$BOOST_DIR" -DGTEST_FOLDER="$GTEST_DIR" -DGTEST_LIBRARY="$GTEST_DIR"/cmake/libgtest.so
#cd build
#make
#cd ..

