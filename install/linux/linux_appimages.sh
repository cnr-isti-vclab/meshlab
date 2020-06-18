#!/bin/bash
# This is a script shell for setting up the AppImage bundle for linux
# Requires a properly built meshlab, boundled and deployed (see linux_deploy.sh)
# inside the directory given as argument
#
# Without given arguments, MeshLab AppImage(s) will be placed in the meshlab
# directory.
#
# You can give as argument the DISTRIB_PATH.

#checking for parameters
if [ "$#" -eq 0 ]
then
    DISTRIB_PATH="../../distrib"
else
    DISTRIB_PATH=$(realpath $1)
fi

cd "$(dirname "$(realpath "$0")")"; #move to script directory
INSTALL_PATH=$(pwd)

cd $DISTRIB_PATH
PARENT_NAME="$(basename $DISTRIB_PATH)"

export VERSION=$(cat $INSTALL_PATH/../../ML_VERSION)

cd ..

#mv $PARENT_NAME/usr/share/applications/meshlab.desktop .

mv $PARENT_NAME/AppRun $PARENT_NAME/AppRunMeshLab
mv $PARENT_NAME/AppRunMeshLabServer $PARENT_NAME/AppRun
rm $PARENT_NAME/*.desktop
cp $PARENT_NAME/usr/share/applications/meshlab_server.desktop $PARENT_NAME/

$INSTALL_PATH/resources/appimagetool $PARENT_NAME
mv MeshLabServer-$VERSION*.AppImage MeshLabServer$VERSION-linux.AppImage
#chmod +x MeshLabServer$VERSION-linux.AppImage

#mv $PARENT_NAME/usr/share/applications/meshlab_server.desktop .
#mv meshlab.desktop $PARENT_NAME/usr/share/applications/
mv $PARENT_NAME/AppRun $PARENT_NAME/AppRunMeshLabServer
mv $PARENT_NAME/AppRunMeshLab $PARENT_NAME/AppRun
rm $PARENT_NAME/*.desktop
cp $PARENT_NAME/usr/share/applications/meshlab.desktop $PARENT_NAME/

$INSTALL_PATH/resources/appimagetool $PARENT_NAME
mv MeshLab-$VERSION*.AppImage MeshLab$VERSION-linux.AppImage
#chmod +x MeshLab$VERSION-linux.AppImage

#at this point, distrib folder contains all the files necessary to execute meshlab
echo MeshLab$VERSION-linux.AppImage and MeshLabServer$VERSION-linux.AppImage generated
