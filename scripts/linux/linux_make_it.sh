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

SCRIPTS_PATH="$(dirname "$(realpath "$0")")"
SOURCE_PATH=$SCRIPTS_PATH/../../src
BUILD_PATH=$SOURCE_PATH/build
INSTALL_PATH=$SOURCE_PATH/install
CORES="-j4"

#check parameters
for i in "$@"
do
case $i in
    -b=*|--build_path=*)
    BUILD_PATH="${i#*=}"
    shift # past argument=value
    ;;
    -i=*|--install_path=*)
    INSTALL_PATH="${i#*=}"/usr/
    shift # past argument=value
    ;;
    -j*)
    CORES=$i
    shift # past argument=value
    ;;
    *)
          # unknown option
    ;;
esac
done

sh $SCRIPTS_PATH/linux_build.sh -b=$BUILD_PATH -i=$INSTALL_PATH $CORES
sh $SCRIPTS_PATH/linux_make_bundle.sh $INSTALL_PATH
sh $SCRIPTS_PATH/linux_deploy.sh $INSTALL_PATH
sh $SCRIPTS_PATH/linux_appimages.sh $INSTALL_PATH
