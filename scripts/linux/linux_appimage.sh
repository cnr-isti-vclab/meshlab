#!/bin/bash
# This is a script shell for setting up the AppImage bundle for linux
# Requires a properly built meshlab, boundled and deployed (see linux_deploy.sh)
# inside the directory given as argument
#
# Without given arguments, MeshLab AppImage will be placed in the src
# directory.
#
# You can give as argument the path of the bundle (default is src/install),
# and the AppImage will be placed in the parent directory (src)

SCRIPTS_PATH="$(dirname "$(realpath "$0")")"

#checking for parameters
if [ "$#" -eq 0 ]
then
    BUNDLE_PATH=$SCRIPTS_PATH/../../src/install
else
    BUNDLE_PATH=$(realpath $1)
fi

PARENT_NAME="$(basename $BUNDLE_PATH)"

export VERSION=$(cat $SCRIPTS_PATH/../../ML_VERSION)

$SCRIPTS_PATH/resources/appimagetool $PARENT_NAME
mv MeshLab-$VERSION*.AppImage $PARENT_NAME/MeshLab$VERSION-linux.AppImage

#at this point, distrib folder contains all the files necessary to execute meshlab
echo MeshLab$VERSION-linux.AppImage generated
