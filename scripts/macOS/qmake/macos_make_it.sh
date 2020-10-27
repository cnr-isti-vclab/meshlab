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

#realpath function
realpath() {
    [[ $1 = /* ]] && echo "$1" || echo "$PWD/${1#./}"
}

#checking for parameters
if [ "$#" -eq 0 ]
then
    BUILD_PATH="../../../src"
    DISTRIB_PATH="../../../distrib"
else
    BUILD_PATH=$( realpath $1 )
    DISTRIB_PATH=$BUILD_PATH/distrib
fi

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $DIR #move to script directory

sh macos_build.sh $BUILD_PATH
sh macos_deploy.sh $DISTRIB_PATH
sh macos_dmg.sh $DISTRIB_PATH
