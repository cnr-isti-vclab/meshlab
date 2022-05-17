#!/bin/bash
# this is a script shell for setting up the application bundle for linux
# Requires a properly built meshlab.
#
# Without given arguments, the application boundle will be placed in the meshlab/src/install
# directory.
#
# You can give as argument the path were meshlab has been installed.

SCRIPTS_PATH="$(dirname "$(realpath "$0")")"/../
RESOURCES_PATH=$SCRIPTS_PATH/../../resources
SOURCE_PATH=$SCRIPTS_PATH/../../src

#checking for parameters
if [ "$#" -eq 0 ]
then
    INSTALL_PATH=$SOURCE_PATH/install
else
    INSTALL_PATH=$(realpath $1)
fi

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$INSTALL_PATH

#check if we have an exec in distrib
if ! [ -f $INSTALL_PATH/usr/bin/meshlab ]
then
    echo "ERROR: meshlab bin not found inside $INSTALL_PATH/usr/bin/"
    exit 1
fi

mkdir -p $INSTALL_PATH/usr/share/doc/meshlab
mkdir -p $INSTALL_PATH/usr/share/icons/Yaru/512x512/apps/

cp $RESOURCES_PATH/linux/meshlab.desktop $INSTALL_PATH/usr/share/applications/meshlab.desktop
cp $RESOURCES_PATH/icons/meshlab.png $INSTALL_PATH/usr/share/icons/Yaru/512x512/apps/meshlab.png
cp $RESOURCES_PATH/LICENSE.txt $INSTALL_PATH/usr/share/doc/meshlab/
cp $RESOURCES_PATH/privacy.txt $INSTALL_PATH/usr/share/doc/meshlab/
cp $RESOURCES_PATH/readme.txt $INSTALL_PATH/usr/share/doc/meshlab/

for filename in $INSTALL_PATH/usr/lib/meshlab/plugins/*.so; do
    patchelf --set-rpath '$ORIGIN/../' $filename
done
