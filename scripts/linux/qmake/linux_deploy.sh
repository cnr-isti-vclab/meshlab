#!/bin/bash
# This is a script shell for deploying a meshlab-portable folder.
# Requires a properly built meshlab boundle (see linux_make_boundle.sh).
#
# This script can be run only in the oldest supported linux distro that you are using
# due to linuxdeployqt tool choice (see https://github.com/probonopd/linuxdeployqt/issues/340).
#
# Without given arguments, the folder that will be deployed is meshlab/distrib.
# 
# You can give as argument the DISTRIB_PATH.

#checking for parameters
if [ "$#" -eq 0 ]
then
    DISTRIB_PATH="../../../distrib"
else
    DISTRIB_PATH=$(realpath $1)
fi

cd "$(dirname "$(realpath "$0")")"; #move to script directory
INSTALL_PATH=$(pwd)/../

cd $DISTRIB_PATH

$INSTALL_PATH/resources/linuxdeployqt usr/share/applications/meshlab.desktop -bundle-non-qt-libs -executable=usr/bin/meshlabserver

chmod +x usr/bin/meshlab
chmod +x usr/bin/meshlabserver
rm AppRun

cp $INSTALL_PATH/resources/AppRunMeshLab .
mv AppRunMeshLab AppRun
cp $INSTALL_PATH/resources/AppRunMeshLabServer .
chmod +x AppRun
chmod +x AppRunMeshLabServer

#at this point, distrib folder contains all the files necessary to execute meshlab
echo "distrib folder is now a self contained meshlab application"