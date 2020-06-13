#!/bin/bash
# this is a script shell for deploying a meshlab-portable folder.
# Requires a properly built meshlab.
#
# This script can be run only in the oldest supported linux distro that you are using
# due to linuxdeployqt tool choice (see https://github.com/probonopd/linuxdeployqt/issues/340).
#
# Without given arguments, the folder that will be deployed is meshlab/distrib.
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
rm -r plugins/plugins.txt
rm -r README.md

mkdir -p usr/bin
mkdir -p usr/lib/meshlab
mkdir -p usr/share/applications
mkdir -p usr/share/meshlab
mkdir -p usr/share/doc/meshlab
mkdir -p usr/share/icons/hicolor/512x512/apps/

cp $INSTALL_PATH/resources/default.desktop usr/share/applications/meshlab.desktop
mv meshlab.png usr/share/icons/hicolor/512x512/apps/meshlab.png
mv meshlab usr/bin
mv meshlabserver usr/bin
mv LICENSE.txt usr/share/doc/meshlab/
mv privacy.txt usr/share/doc/meshlab/
mv readme.txt usr/share/doc/meshlab/
mv lib/libmeshlab-common* usr/lib/
mv plugins/ usr/lib/meshlab/
mv lib/linux/* usr/lib/meshlab/
mv usr/lib/meshlab/libIDTF* usr/lib/
mv usr/lib/meshlab/libIFX* usr/lib/
#chrpath -r ../lib/ usr/lib/meshlab/plugins/*.so
mv shaders/ usr/share/meshlab/

$INSTALL_PATH/resources/linuxdeployqt usr/share/applications/meshlab.desktop -bundle-non-qt-libs -executable=usr/bin/meshlabserver

rm -r lib

chmod +x usr/bin/meshlab
chmod +x usr/bin/meshlabserver

#at this point, distrib folder contains all the files necessary to execute meshlab
echo "distrib folder is now a self contained meshlab application"

