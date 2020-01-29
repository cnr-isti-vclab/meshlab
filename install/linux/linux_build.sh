#!/bin/bash
# this is a script shell for compiling meshlab in a Linux environment.
# Requires a Qt environment which is set-up properly, and an accessible
# qmake binary.
#
# Without given arguments, MeshLab will be built in the meshlab/src
# directory, and binaries will be placed in meshlab/distrib.
# 
# You can give as argument the BUILD_PATH, and meshlab binaries will be
# then placed inside BUILD_PATH/distrib.

#checking for parameters
if [ "$#" -eq 0 ]
then
    BUILD_PATH="../.."
else
    BUILD_PATH=$1
fi

echo "Build path is: " $BUILD_PATH
SOURCE_PATH="../../src"

cd BUILD_PATH
qmake SOURCE_PATH
make