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

# Make nsis script

#get version
IFS=' ' #space delimiter
STR_VERSION=$($INSTALL_PATH/meshlab.exe --version)
read -a strarr <<< "$STR_VERSION"
ML_VERSION=${strarr[1]} #get the meshlab version from the string

sed "s%MESHLAB_VERSION%$ML_VERSION%g" $RESOURCES_PATH/windows/meshlab.nsi > $RESOURCES_PATH/windows/meshlab_final.nsi
sed -i "s%DISTRIB_PATH%.%g" $RESOURCES_PATH/windows/meshlab_final.nsi

mv $RESOURCES_PATH/windows/meshlab_final.nsi $INSTALL_PATH/
cp $RESOURCES_PATH/windows/ExecWaitJob.nsh $INSTALL_PATH/
cp $RESOURCES_PATH/windows/FileAssociation.nsh $INSTALL_PATH/

# Make Installer

makensis.exe $INSTALL_PATH/meshlab_final.nsi

rm $INSTALL_PATH/meshlab_final.nsi
rm $INSTALL_PATH/ExecWaitJob.nsh
rm $INSTALL_PATH/FileAssociation.nsh

mkdir $PACKAGES_PATH
mv $INSTALL_PATH/MeshLab*-windows.exe $PACKAGES_PATH