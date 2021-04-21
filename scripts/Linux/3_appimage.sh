#!/bin/bash
# This is a script shell for setting up the AppImage bundle for linux
# Requires a properly built meshlab, boundled and deployed (see linux_deploy.sh)
# inside the directory given as argument
#
# Without given arguments, MeshLab AppImage will be placed in the src
# directory.
#
# You can give as argument the path of the bundle (default is src/install),
# and the AppImage will be placed in the parent directory (src)

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

PARENT_NAME="$(dirname $INSTALL_PATH)"

#get version
IFS=' ' #space delimiter
STR_VERSION=$($INSTALL_PATH/usr/bin/meshlab --version)
read -a strarr <<< "$STR_VERSION"
ML_VERSION=${strarr[1]} #get the meshlab version from the string


export VERSION=$ML_VERSION

$SCRIPTS_PATH/resources/appimagetool $INSTALL_PATH
mv MeshLab-$VERSION*.AppImage $PARENT_NAME/MeshLab$VERSION-linux.AppImage

#at this point, distrib folder contains all the files necessary to execute meshlab
echo MeshLab$VERSION-linux.AppImage generated
