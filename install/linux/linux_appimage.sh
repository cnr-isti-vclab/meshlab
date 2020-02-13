#!/bin/bash
# this is a script shell for setting up the application bundle for linux
# Requires a properly built meshlab (does not require to run the 
# linux_deploy.sh script).
#
# This script can be run only in the oldest supported linux distro that you are using
# due to linuxdeployqt tool choice (see https://github.com/probonopd/linuxdeployqt/issues/340).
#
# Without given arguments, MeshLab AppImage will be placed in the meshlab/distrib
# directory.
# 
# You can give as argument the DISTRIB_PATH.

cd "${0%/*}" #move to script directory

#checking for parameters
if [ "$#" -eq 0 ]
then
    DISTRIB_PATH=$PWD/../../distrib
else
    DISTRIB_PATH=$1
fi

SOURCE_PATH=$PWD/../../src

INSTALL_PATH=$(pwd)
cd $DISTRIB_PATH
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)

#check if we have an exec in distrib
if ! [ -f meshlab ]
then
  echo "ERROR: meshlab bin not found inside distrib"
  exit 1
fi

cp $INSTALL_PATH/../meshlab.png .
cp $INSTALL_PATH/resources/default.desktop .
cp $SOURCE_PATH/../LICENSE.txt $DISTRIB_PATH
cp $SOURCE_PATH/../docs/readme.txt $DISTRIB_PATH

rm -r plugins/U3D_OSX/
rm -r plugins/U3D_W32/
rm -r README.md

$INSTALL_PATH/resources/linuxdeployqt meshlab -appimage -executable=meshlabserver
mv *.AppImage MeshLab$(date +%Y.%m)-linux.AppImage 

#at this point, distrib folder contains all the files necessary to execute meshlab
echo MeshLab$(date +%Y.%m)-linux.AppImage generated
