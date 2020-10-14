#!/bin/bash
# this is a script shell for setting up the application bundle for linux
# Requires a properly built meshlab.
#
# Without given arguments, the application boundle will be placed in the meshlab/src/install
# directory.
#
# You can give as argument the path were meshlab has been installed.

#checking for parameters
if [ "$#" -eq 0 ]
then
    INSTALL_PATH="../../src/install"
else
    INSTALL_PATH=$(realpath $1)
fi

cd "$(dirname "$(realpath "$0")")"; #move to script directory

SOURCE_PATH=$PWD/../../src
DISTRIB_PATH=$PWD/../../distrib

SCRIPTS_PATH=$(pwd)
cd $INSTALL_PATH
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)

#check if we have an exec in distrib
if ! [ -f usr/bin/meshlab ]
then
  echo "ERROR: meshlab bin not found inside $INSTALL_PATH/usr/bin/"
  exit 1
fi

mkdir -p usr/share/doc/meshlab
mkdir -p usr/share/icons/hicolor/512x512/apps/
mkdir -p usr/share/icons/Yaru/512x512/apps/

cp $SCRIPTS_PATH/resources/meshlab_appimage.desktop usr/share/applications/meshlab.desktop
cp $DISTRIB_PATH/meshlab.png usr/share/icons/hicolor/512x512/apps/meshlab.png
cp $DISTRIB_PATH/meshlab.png usr/share/icons/Yaru/512x512/apps/meshlab.png
cp $DISTRIB_PATH/LICENSE.txt usr/share/doc/meshlab/
cp $DISTRIB_PATH/privacy.txt usr/share/doc/meshlab/
cp $DISTRIB_PATH/readme.txt usr/share/doc/meshlab/

for filename in usr/lib/meshlab/plugins/*.so; do
    patchelf --set-rpath '$ORIGIN/../' $filename
done
