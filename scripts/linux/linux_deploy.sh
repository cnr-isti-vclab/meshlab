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

SCRIPTS_PATH="$(dirname "$(realpath "$0")")"

#checking for parameters
if [ "$#" -eq 0 ]
then
    BUNDLE_PATH=$SCRIPTS_PATH/../../src/install
else
    BUNDLE_PATH=$(realpath $1)
fi

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$BUNDLE_PATH/usr/lib/meshlab
$SCRIPTS_PATH/resources/linuxdeployqt $BUNDLE_PATH/usr/share/applications/meshlab.desktop -bundle-non-qt-libs -exclude-libs=

chmod +x $BUNDLE_PATH/usr/bin/meshlab
rm $BUNDLE_PATH/AppRun

cp $SCRIPTS_PATH/resources/AppRunMeshLab $BUNDLE_PATH/
mv $BUNDLE_PATH/AppRunMeshLab $BUNDLE_PATH/AppRun
chmod +x $BUNDLE_PATH/AppRun

#at this point, distrib folder contains all the files necessary to execute meshlab
echo "$BUNDLE_PATH is now a self contained meshlab application"
