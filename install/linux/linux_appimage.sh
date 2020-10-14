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

#checking for parameters
if [ "$#" -eq 0 ]
then
    BUNDLE_PATH="../../src/install"
else
    BUNDLE_PATH=$(realpath $1)
fi

cd "$(dirname "$(realpath "$0")")"; #move to script directory
SCRIPTS_PATH=$(pwd)

cd $BUNDLE_PATH
PARENT_NAME="$(basename $BUNDLE_PATH)"

export VERSION=$(cat $SCRIPTS_PATH/../../ML_VERSION)

cd ..

$SCRIPTS_PATH/resources/appimagetool $PARENT_NAME
mv MeshLab-$VERSION*.AppImage MeshLab$VERSION-linux.AppImage
#chmod +x MeshLab$VERSION-linux.AppImage

#at this point, distrib folder contains all the files necessary to execute meshlab
echo MeshLab$VERSION-linux.AppImage and MeshLabServer$VERSION-linux.AppImage generated
