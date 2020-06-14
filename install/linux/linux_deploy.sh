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
INSTALL_PATH=$(pwd)

#checking for parameters
if [ "$#" -eq 0 ]
then
    DISTRIB_PATH=$PWD/../../distrib
else
    DISTRIB_PATH=$1
fi

sh linux_make_boundle.sh $@

cd $DISTRIB_PATH

$INSTALL_PATH/resources/linuxdeployqt usr/share/applications/meshlab.desktop -bundle-non-qt-libs -executable=usr/bin/meshlabserver

chmod +x usr/bin/meshlab
chmod +x usr/bin/meshlabserver

#at this point, distrib folder contains all the files necessary to execute meshlab
echo "distrib folder is now a self contained meshlab application"

