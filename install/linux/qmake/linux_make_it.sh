#!/bin/bash
# this is a script shell for compiling and deploying meshlab in a Linux environment.
#
# This script can be run only in the oldest supported linux distro that you are using
# due to linuxdeployqt tool choice (see https://github.com/probonopd/linuxdeployqt/issues/340).
#
# Requires a Qt environment which is set-up properly, and an accessible
# qmake binary.
#
# Without given arguments, MeshLab will be built in the meshlab/src
# directory, and binaries and AppImage will be placed in meshlab/distrib.
# 
# You can give as argument the BUILD_PATH, and meshlab binaries and
# AppImage will be then placed inside BUILD_PATH/distrib.

#checking for parameters
if [ "$#" -eq 0 ]
then
    BUILD_PATH="../../../src"
    DISTRIB_PATH="../../../distrib"
else
    BUILD_PATH=$(realpath $1)
    BUILD_PATH=$BUILD_PATH/distrib
fi

cd "$(dirname "$(realpath "$0")")"; #move to script directory

sh linux_build.sh $BUILD_PATH
sh linux_make_bundle.sh $DISTRIB_PATH
sh linux_deploy.sh $DISTRIB_PATH
sh linux_appimages.sh $DISTRIB_PATH
