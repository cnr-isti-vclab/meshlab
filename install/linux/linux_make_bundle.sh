#!/bin/bash
# this is a script shell for setting up the application bundle for linux
# Requires a properly built meshlab.
#
# Without given arguments, the application boundle will be placed in the meshlab/distrib
# directory.
#
# You can give as argument the DISTRIB_PATH.

#checking for parameters
if [ "$#" -eq 0 ]
then
    DISTRIB_PATH="../../distrib"
else
    DISTRIB_PATH=$(realpath $1)
fi

cd "$(dirname "$(realpath "$0")")"; #move to script directory

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

mkdir -p usr/bin
mkdir -p usr/lib/meshlab
mkdir -p usr/share/applications
mkdir -p usr/share/meshlab
mkdir -p usr/share/doc/meshlab
mkdir -p usr/share/icons/hicolor/512x512/apps/
mkdir -p usr/share/icons/Yaru/512x512/apps/

cp $INSTALL_PATH/resources/meshlab_appimage.desktop usr/share/applications/meshlab.desktop
cp $INSTALL_PATH/resources/meshlab_server_appimage.desktop usr/share/applications/meshlab_server.desktop
cp meshlab.png usr/share/icons/hicolor/512x512/apps/meshlab.png
mv meshlab.png usr/share/icons/Yaru/512x512/apps/meshlab.png
mv meshlab usr/bin
mv meshlabserver usr/bin
mv LICENSE.txt usr/share/doc/meshlab/
mv privacy.txt usr/share/doc/meshlab/
mv readme.txt usr/share/doc/meshlab/
mv lib/libmeshlab-common* usr/lib/
mv plugins/ usr/lib/meshlab/
mv lib/meshlab/* usr/lib/meshlab/
#patchelf --set-rpath '$ORIGIN/..' usr/lib/meshlab/plugins/*.so
mv shaders/ usr/share/meshlab/
rm -r lib

patchelf --set-rpath '$ORIGIN/../lib/meshlab/' usr/bin/meshlab
patchelf --set-rpath '$ORIGIN/../lib/meshlab/' usr/bin/meshlabserver
