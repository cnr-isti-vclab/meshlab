#!/bin/bash
# This is a script shell for deploying a meshlab-portable folder.
# Requires a properly built MeshLab (see 1_build.sh).
#
# This script can be run only in the oldest supported linux distro
# due to linuxdeployqt tool choice (see https://github.com/probonopd/linuxdeployqt/issues/340).
#
# Without given arguments, the folder that will be deployed is meshlab/src/install, which
# should be the path where MeshLab has been installed (default output of 1_build.sh).
#
# You can give as argument the path where you installed MeshLab.

SCRIPTS_PATH="$(dirname "$(realpath "$0")")"

#checking for parameters
if [ "$#" -eq 0 ]
then
    BUNDLE_PATH=$SCRIPTS_PATH/../../src/install
else
    BUNDLE_PATH=$(realpath $1)
fi

bash resources/make_bundle.sh $BUNDLE_PATH

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$BUNDLE_PATH/usr/lib/meshlab
$SCRIPTS_PATH/resources/linuxdeployqt $BUNDLE_PATH/usr/share/applications/meshlab.desktop -bundle-non-qt-libs

chmod +x $BUNDLE_PATH/usr/bin/meshlab
rm $BUNDLE_PATH/AppRun

cp $SCRIPTS_PATH/resources/AppRunMeshLab $BUNDLE_PATH/
mv $BUNDLE_PATH/AppRunMeshLab $BUNDLE_PATH/AppRun
chmod +x $BUNDLE_PATH/AppRun

#at this point, distrib folder contains all the files necessary to execute meshlab
echo "$BUNDLE_PATH is now a self contained meshlab application"
