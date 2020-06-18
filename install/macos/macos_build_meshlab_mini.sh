#!/bin/bash
# this is a script shell for compiling meshlab in a MacOS environment.
# Requires a Qt environment which is set-up properly from brew, and an accessible
# qmake binary.
#
# Without given arguments, MeshLab will be built in the meshlab/src
# directory, and binaries will be placed in meshlab/distrib.
# 
# You can give as argument the BUILD_PATH, and meshlab binaries will be
# then placed inside BUILD_PATH/distrib.

#realpath function
realpath() {
    [[ $1 = /* ]] && echo "$1" || echo "$PWD/${1#./}"
}

#checking for parameters
if [ "$#" -eq 0 ]
then
    BUILD_PATH="../../src"
else
    BUILD_PATH=$( realpath $1 )
fi

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $DIR #move to script directory

if ! [ -d $BUILD_PATH ]
then
    mkdir $BUILD_PATH
fi

echo "Build path is: " $(realpath $BUILD_PATH)
SOURCE_PATH=$PWD/../../src

cd $BUILD_PATH
qmake $SOURCE_PATH/meshlab.pro "CONFIG+=meshlab_mini"
make -j4
