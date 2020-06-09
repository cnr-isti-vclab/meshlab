#!/usr/bin/env bash

U3D_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

BUILD_CONFIG=Release

cd $1

cmake $U3D_DIR -G "Unix Makefiles" \
    -DSTATIC=ON
    -DCMAKE_BUILD_TYPE=$BUILD_CONFIG \
    -DCMAKE_INSTALL_PREFIX=$PREFIX \
    -DU3D_SHARED:BOOL=OFF

make

rm -R CMakeFiles
rm CMakeCache.txt
rm HelloU3DWorld
rm Makefile
rm cmake_install.cmake
rm cmake_uninstall.cmake

