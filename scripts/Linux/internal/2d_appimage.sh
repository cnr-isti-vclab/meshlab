#!/bin/bash

SCRIPTS_PATH="$(dirname "$(realpath "$0")")"/..
RESOURCES_PATH=$SCRIPTS_PATH/../../resources
INSTALL_PATH=$SCRIPTS_PATH/../../install
PACKAGES_PATH=$SCRIPTS_PATH/../../packages

#checking for parameters
for i in "$@"
do
case $i in
    -i=*|--install_path=*)
        INSTALL_PATH="${i#*=}"
        shift # past argument=value
        ;;
    -p=*|--packages_path=*)
        PACKAGES_PATH="${i#*=}"
        shift # past argument=value
        ;;
    *)
        # unknown option
        ;;
esac
done

$RESOURCES_PATH/linux/x86_64/linuxdeploy --appdir=$INSTALL_PATH \
  --output appimage

#get version
IFS=' ' #space delimiter
STR_VERSION=$($INSTALL_PATH/AppRun --version)
read -a strarr <<< "$STR_VERSION"
ML_VERSION=${strarr[1]} #get the meshlab version from the string

# get running architecture
ARCH=$(uname -m)

mkdir $PACKAGES_PATH
mv MeshLab-*.AppImage $PACKAGES_PATH/MeshLab$ML_VERSION-linux_$ARCH.AppImage