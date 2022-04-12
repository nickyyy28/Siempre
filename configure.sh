#!/bin/bash
git submodule update --init --recursive
cd ./github/yaml-cpp
rm -rf build
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=../../../3rd-party/
make -j10
make install
cd ../../cityhash/
./configure
make -j10
echo $(pwd)
cd src/.libs
cp libcityhash.a libcityhash.so libcityhash.so.0 libcityhash.so.0.0.0 ../../../../3rd-party/lib
cd ..
mkdir ../../../3rd-party/include/city/
cp city.h citycrc.h ../../../3rd-party/include/city/
cd ../../../
