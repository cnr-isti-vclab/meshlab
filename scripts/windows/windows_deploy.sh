#!/bin/bash
# This is a script shell for deploying a meshlab-portable folder.
# Requires a properly built meshlab (see windows_build.sh).
#
# Without given arguments, the folder that will be deployed is meshlab/src/install.
# 
# You can give as argument the path where meshlab is installed.

#saving location where script has been run
DIR = $PWD

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

windeployqt meshlab.exe

mv lib/meshlab/IFX* .
cp IFXCoreStatic.lib lib/meshlab/

#at this point, distrib folder contains all the files necessary to execute meshlab
echo "$BUNDLE_PATH is now a self contained meshlab application"

#going back to original location
cd $DIR
