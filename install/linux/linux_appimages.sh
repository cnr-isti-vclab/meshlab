#!/bin/bash
# this is a script shell for setting up the application bundle for linux
# Requires a properly built meshlab (does not require to run the
# linux_deploy.sh script).
#
# This script can be run only in the oldest supported linux distro that you are using
# due to linuxdeployqt tool choice (see https://github.com/probonopd/linuxdeployqt/issues/340).
#
# Without given arguments, MeshLab AppImage will be placed in the meshlab/distrib
# directory.
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

mv usr/bin/libIFX* .

export VERSION=$(cat $INSTALL_PATH/../../ML_VERSION)

$INSTALL_PATH/resources/linuxdeployqt usr/share/applications/meshlab_server.desktop -appimage
mv *.AppImage ../MeshLabServer$VERSION-linux.AppImage
chmod +x ../MeshLabServer$VERSION-linux.AppImage

rm AppRun 
rm *.desktop
rm *.png

#mv usr/bin/meshlabserver ..
$INSTALL_PATH/resources/linuxdeployqt usr/share/applications/meshlab.desktop -appimage
mv *.AppImage ../MeshLab$VERSION-linux.AppImage
chmod +x ../MeshLab$VERSION-linux.AppImage

#rm AppRun 
#rm *.desktop
#rm *.png
#rm usr/share/applications/meshlab.desktop
#mv usr/bin/meshlab ..
#mv ../meshlabserver usr/bin
#cp $INSTALL_PATH/resources/default.desktop usr/share/applications/meshlab.desktop
#mv ../meshlab usr/bin/

chmod +x usr/bin/meshlab
chmod +x usr/bin/meshlabserver

#at this point, distrib folder contains all the files necessary to execute meshlab
echo MeshLab$VERSION-linux.AppImage and MeshLabServer$VERSION-linux.AppImage generated
