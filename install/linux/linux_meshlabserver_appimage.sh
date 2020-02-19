#!/bin/bash
# this is a script shell for setting up the application bundle of meshlabserver for linux
# Requires a properly built meshlab (does not require to run the linux_deploy.sh script).
#
# This script can be run only in the oldest supported linux distro that you are using
# due to linuxdeployqt tool choice (see https://github.com/probonopd/linuxdeployqt/issues/340).
#
# Without given arguments, MeshLabServer AppImage will be placed in the meshlab
# directory.
# 
# You can give as argument the DISTRIB_PATH, and the output AppImage will be placed
# in the directory DISTRIB_PATH/../

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

ACTUAL_DISTRIB=$DISTRIB_PATH/../server_distrib
mkdir $ACTUAL_DISTRIB
cp -R $DISTRIB_PATH/. $ACTUAL_DISTRIB
cd $ACTUAL_DISTRIB

cp $INSTALL_PATH/../meshlab.png .
cp $INSTALL_PATH/resources/meshlab_server.desktop .
mv meshlab_server.desktop default.desktop
cp $SOURCE_PATH/../LICENSE.txt $DISTRIB_PATH
cp $SOURCE_PATH/../docs/readme.txt $DISTRIB_PATH

rm -r lib/macx64/
rm -r lib/win32-msvc/
rm -r lib/win32-msvc2008/
rm -r lib/win32-msvc2015/
rm -r lib/readme.txt
rm -r plugins/U3D_OSX/
rm -r plugins/U3D_W32/
rm -r plugins/plugins.txt
rm -r README.md
rm -r shaders/
rm -r textures/
rm -r meshlab

$INSTALL_PATH/resources/linuxdeployqt meshlabserver -appimage
mv *.AppImage ../MeshLabServer$(date +%Y.%m)-linux.AppImage 
cd ..
rm -r $ACTUAL_DISTRIB

echo MeshLabServer$(date +%Y.%m)-linux.AppImage generated
