#!/bin/bash
PRJ_ROOT=$(pwd)
git submodule update --init --recursive
if [ -d $PRJ_ROOT/github/yaml-cpp/build  ]; then
    rm -rf $PRJ_ROOT/github/yaml-cpp/build
fi
cd $PRJ_ROOT/github/yaml-cpp
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$PRJ_ROOT/3rd-party/
make -j6
make install
cd $PRJ_ROOT/github/cityhash/
./configure
make clean
make -j10
cd src/.libs
cp libcityhash.a libcityhash.so libcityhash.so.0 libcityhash.so.0.0.0 $PRJ_ROOT/3rd-party/lib
cd ..
if [ -d $PRJ_ROOT/3rd-party/include/city ]; then
    rm -rf $PRJ_ROOT/3rd-party/include/city
fi

mkdir $PRJ_ROOT/3rd-party/include/city/

cp city.h citycrc.h $PRJ_ROOT/3rd-party/include/city/
cd $PRJ_ROOT

if [ -d $PRJ_ROOT/build ]; then
    rm -rf $PRJ_ROOT/build
fi
mkdir build && cd build
cmake ..
make -j6
