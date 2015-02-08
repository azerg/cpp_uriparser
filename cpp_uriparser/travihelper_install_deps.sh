wget -c 'http://sourceforge.net/projects/boost/files/boost/1.46.1/boost_1_46_1.tar.bz2/download'
tar xf download
rm download
echo '-----boost downloaded & extracted-----'
#-----------------------------------------------------------------
GTEST_BUILD_NAME=gtest-1.7.0
wget 'http://googletest.googlecode.com/files/'$GTEST_BUILD_NAME'.zip'
unzip $GTEST_BUILD_NAME.zip
rm $GTEST_BUILD_NAME.zip
cd $GTEST_BUILD_NAME/cmake
cmake -DBUILD_SHARED_LIBS=ON -Dgtest_build_samples=ON -G"Unix Makefiles" ..
make
cd ../../
echo '-----gtest compiled-----'
#-----------------------------------------------------------------
# build uriparser lib for target system
URIPARSER_NAME=uriparser-0.8.1
TARGTET_DIR=$(readlink -f dependencies)
cd dependencies/$URIPARSER_NAME
chmod 777 configure
./configure --disable-test --disable-doc --prefix=$TARGTET_DIR/$URIPARSER_NAME/deploy
make install
cd ../../
echo '-----uriparser compiled-----'
