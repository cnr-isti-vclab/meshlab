#!/bin/bash
# This is a script shell for deploying a meshlab-portable folder.
# Requires a properly built meshlab (see windows_build.sh).
#
# Without given arguments, the folder that will be deployed is meshlab/src/install.
#
# You can give as argument the path where meshlab is installed.

SCRIPTS_PATH="$(dirname "$(realpath "$0")")"

#checking for parameters
if [ "$#" -eq 0 ]
then
    BUNDLE_PATH=$SCRIPTS_PATH/../../src/install
else
    BUNDLE_PATH=$(realpath $1)
fi

windeployqt $BUNDLE_PATH/meshlab.exe

mv $BUNDLE_PATH/lib/meshlab/IFX* $BUNDLE_PATH
cp $BUNDLE_PATH/IFXCoreStatic.lib $BUNDLE_PATH/lib/meshlab/

#at this point, distrib folder contains all the files necessary to execute meshlab
echo "$BUNDLE_PATH is now a self contained meshlab application"
