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
INSTALL_PATH=$SCRIPTS_PATH/../../src/install
QT_DIR=""

#checking for parameters
for i in "$@"
do
case $i in
    -i=*|--install_path=*)
        INSTALL_PATH="${i#*=}"
        shift # past argument=value
        ;;
    -qt=*|--qt_dir=*)
        QT_DIR=${i#*=}
        shift # past argument=value
        ;;
    *)
        # unknown option
        ;;
esac
done

bash $SCRIPTS_PATH/resources/make_bundle.sh $INSTALL_PATH

if [ ! -z "$QT_DIR" ]
then
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$QT_DIR/lib
    export QMAKE=$QT_DIR/bin/qmake
fi

chmod +x $INSTALL_PATH/usr/bin/meshlab

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$INSTALL_PATH/usr/lib/meshlab
$SCRIPTS_PATH/resources/linuxdeploy --appdir=$INSTALL_PATH \
  --plugin qt --output appimage

#get version
IFS=' ' #space delimiter
STR_VERSION=$($INSTALL_PATH/AppRun --version)
read -a strarr <<< "$STR_VERSION"
ML_VERSION=${strarr[1]} #get the meshlab version from the string

mv MeshLab-*.AppImage MeshLab$ML_VERSION-linux.AppImage

#at this point, distrib folder contains all the files necessary to execute meshlab
echo "$INSTALL_PATH is now a self contained meshlab application"
