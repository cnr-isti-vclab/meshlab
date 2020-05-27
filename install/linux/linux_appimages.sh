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

rm -r lib/macx64/
rm -r lib/win32-msvc/
rm -r lib/win32-msvc2008/
rm -r lib/win32-msvc2015/
rm -r lib/readme.txt
rm -r plugins/U3D_OSX/
rm -r plugins/U3D_W32/
rm -r plugins/plugins.txt
rm -r README.md

mkdir -p usr/bin
mkdir -p usr/lib/meshlab
mkdir -p usr/share/applications
mkdir -p usr/share/meshlab
mkdir -p usr/share/doc/meshlab
mkdir -p usr/share/icons/hicolor/512x512/apps/
mkdir -p usr/share/icons/Yaru/512x512/apps/

cp $INSTALL_PATH/resources/meshlab_server.desktop usr/share/applications/
cp meshlab.png usr/share/icons/hicolor/512x512/apps/meshlab.png
mv meshlab.png usr/share/icons/Yaru/512x512/apps/meshlab.png
mv meshlabserver usr/bin
mv LICENSE.txt usr/share/doc/meshlab/
mv privacy.txt usr/share/doc/meshlab/
mv readme.txt usr/share/doc/meshlab/
mv lib/libmeshlab-common* usr/lib/
mv plugins/ usr/lib/meshlab/
mv lib/linux/* usr/lib/meshlab/plugins
chrpath -r ../lib/ usr/lib/meshlab/plugins/*.so
mv shaders/ usr/share/meshlab/

export VERSION=$(cat $INSTALL_PATH/../../ML_VERSION)

$INSTALL_PATH/resources/linuxdeployqt usr/share/applications/meshlab_server.desktop -appimage
mv *.AppImage ../MeshLabServer$VERSION-linux.AppImage

rm AppRun
rm *.desktop
rm *.png
rm usr/share/applications/meshlab_server.desktop
cp $INSTALL_PATH/resources/default.desktop usr/share/applications/meshlab.desktop
mv usr/bin/meshlabserver .
mv meshlab usr/bin

$INSTALL_PATH/resources/linuxdeployqt usr/share/applications/meshlab.desktop -appimage
mv *.AppImage ../MeshLab$VERSION-linux.AppImage

cp $INSTALL_PATH/resources/meshlab_server.desktop usr/share/applications/
mv meshlabserver usr/bin/

rm -r lib

#at this point, distrib folder contains all the files necessary to execute meshlab
echo MeshLab$VERSION-linux.AppImage generated
