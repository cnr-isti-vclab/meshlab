#!/bin/bash
# This is a script shell for deploying a meshlab-portable folder and create an AppImage.
# Requires a properly built MeshLab (see 1_build.sh).
#
# Without given arguments, the folder that will be deployed is meshlab/install, which
# should be the path where MeshLab has been installed (default output of 1_build.sh).
# The AppImage will be placed in the directory where the script is run.
#
# You can give as argument the path where you installed MeshLab.

SCRIPTS_PATH="$(dirname "$(realpath "$0")")"
RESOURCES_PATH=$SCRIPTS_PATH/../../resources
INSTALL_PATH=$SCRIPTS_PATH/../../install
PACKAGE_PATH=$SCRIPTS_PATH/../../package

#checking for parameters
for i in "$@"
do
case $i in
    -i=*|--install_path=*)
        INSTALL_PATH="${i#*=}"
        shift # past argument=value
        ;;
    -p=*|--package_path=*)
        PACKAGE_PATH="${i#*=}"
        shift # past argument=value
        ;;
    *)
        # unknown option
        ;;
esac
done

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$INSTALL_PATH/usr/lib
$RESOURCES_PATH/linux/linuxdeploy --appdir=$INSTALL_PATH \
  --output appimage

#get version
IFS=' ' #space delimiter
STR_VERSION=$($INSTALL_PATH/AppRun --version)
read -a strarr <<< "$STR_VERSION"
ML_VERSION=${strarr[1]} #get the meshlab version from the string

mkdir $PACKAGE_PATH
mv MeshLab-*.AppImage $PACKAGE_PATH/MeshLab$ML_VERSION-linux.AppImage
