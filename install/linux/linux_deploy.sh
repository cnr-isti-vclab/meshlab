#!/bin/bash
# This is a script shell for deploying a meshlab-portable folder.
# Requires a properly built meshlab bundle (see linux_make_bundle.sh).
#
# This script can be run only in the oldest supported linux distro that you are using
# due to linuxdeployqt tool choice (see https://github.com/probonopd/linuxdeployqt/issues/340).
#
# Without given arguments, the folder that will be deployed is meshlab/src/install.
# 
# You can give as argument the path of the bundle.

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

$SCRIPTS_PATH/resources/linuxdeployqt usr/share/applications/meshlab.desktop -bundle-non-qt-libs

chmod +x usr/bin/meshlab
rm AppRun

cp $SCRIPTS_PATH/resources/AppRunMeshLab .
mv AppRunMeshLab AppRun
chmod +x AppRun

#at this point, distrib folder contains all the files necessary to execute meshlab
echo "$BUNDLE_PATH is now a self contained meshlab application"