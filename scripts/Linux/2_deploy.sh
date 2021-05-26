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

bash $SCRIPTS_PATH/resources/make_bundle.sh $INSTALL_PATH

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$INSTALL_PATH/usr/lib/meshlab
$SCRIPTS_PATH/resources/linuxdeployqt $INSTALL_PATH/usr/share/applications/meshlab.desktop \
  -bundle-non-qt-libs \
  -executable=$INSTALL_PATH/usr/lib/meshlab/plugins/libfilter_sketchfab.so \
  -executable=$INSTALL_PATH/usr/lib/meshlab/plugins/libio_3ds.so \
  -executable=$INSTALL_PATH/usr/lib/meshlab/plugins/libio_ctm.so \
  -executable=$INSTALL_PATH/usr/lib/meshlab/plugins/libfilter_mesh_booleans.so


chmod +x $INSTALL_PATH/usr/bin/meshlab
rm $INSTALL_PATH/AppRun

cp $SCRIPTS_PATH/resources/AppRunMeshLab $INSTALL_PATH/
mv $INSTALL_PATH/AppRunMeshLab $INSTALL_PATH/AppRun
chmod +x $INSTALL_PATH/AppRun

#at this point, distrib folder contains all the files necessary to execute meshlab
echo "$INSTALL_PATH is now a self contained meshlab application"
