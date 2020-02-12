#!/bin/bash
# this is a script shell for compiling and deploying meshlab in a MacOS environment.
#
# Requires a Qt environment which is set-up properly, and an accessible
# qmake binary.
#
# Without given arguments, MeshLab will be built in the meshlab/src
# directory, and binaries and AppImage will be placed in meshlab/distrib.
# 
# You can give as argument the BUILD_PATH, and meshlab binaries and
# AppImage will be then placed inside BUILD_PATH/distrib.

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $DIR #move to script directory

#checking for parameters
if [ "$#" -eq 0 ]
then
    BUILD_PATH="../../src"
else
    BUILD_PATH=$1
fi

sh macos_build.sh $BUILD_PATH
sh macos_deploy.sh $BUILD_PATH/distrib
sh macos_dmg.sh $BUILD_PATH/distrib
