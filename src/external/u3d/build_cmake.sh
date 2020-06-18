#!/usr/bin/env bash
BUILD_CONFIG=Release

mkdir ../build_cmake
cd ../build_cmake

cmake ../u3d/src/ -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=$BUILD_CONFIG \
    -DCMAKE_INSTALL_PREFIX=$PREFIX 

make
