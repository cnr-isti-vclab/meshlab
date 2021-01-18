#!/bin/bash
# This is a script shell for deploying a meshlab-portable folder.
# Requires a properly built meshlab (see windows_build.sh).
#
# Without given arguments, the folder that will be deployed is meshlab/src/install.
#
# You can give as argument the path where meshlab is installed.

SCRIPTS_PATH="$(dirname "$(realpath "$0")")"
DISTRIB_PATH=$SCRIPTS_PATH/../../distrib
INSTALL_PATH=$SCRIPTS_PATH/../../src/install

#checking for parameters
for i in "$@"
do
case $i in
    -i=*|--install_path=*)
    INSTALL_PATH="${i#*=}"
    shift # past argument=value
    ;;
    *)
          # unknown option
    ;;
esac
done

windeployqt $INSTALL_PATH/meshlab.exe

windeployqt $INSTALL_PATH/plugins/filter_sketchfab.dll --libdir $INSTALL_PATH/

mv $INSTALL_PATH/lib/meshlab/IFX* $INSTALL_PATH
cp $INSTALL_PATH/IFXCoreStatic.lib $INSTALL_PATH/lib/meshlab/
cp $DISTRIB_PATH/LICENSE.txt $INSTALL_PATH/
cp $DISTRIB_PATH/privacy.txt $INSTALL_PATH/

#at this point, distrib folder contains all the files necessary to execute meshlab
echo "$INSTALL_PATH is now a self contained meshlab application"
