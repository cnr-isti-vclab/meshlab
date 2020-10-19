#!/bin/bash
# this is a script shell for deploying a meshlab-portable app.
# Requires a properly built meshlab.
#
# Without given arguments, the folder that will be deployed is meshlab/distrib.
#
# You can give as argument the DISTRIB_PATH.
#
# After running this script, $DISTRIB_PATH/meshlab.app will be a portable meshlab application.

#realpath function
realpath() {
    [[ $1 = /* ]] && echo "$1" || echo "$PWD/${1#./}"
}

#checking for parameters
if [ "$#" -eq 0 ]
then
    BUNDLE_PATH="../../src/install"
else
    BUNDLE_PATH=$(realpath $1)
fi

SCRIPTS_PATH=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $SCRIPTS_PATH
BUNDLE_PATH=$(realpath $BUNDLE_PATH)

APPNAME="meshlab.app"

echo "Hopefully I should find " $BUNDLE_PATH/$APPNAME

if ! [ -e $BUNDLE_PATH/$APPNAME -a -d $BUNDLE_PATH/$APPNAME ]
then
    echo "Started in the wrong dir: I have not found the meshlab.app"
    exit -1
fi

#copy libIFX libraries and change rpath u3d plugin
#cp -a $DISTRIB_PATH/lib/meshlab/libIFX* $DISTRIB_PATH/$APPNAME/Contents/Frameworks
#rm $DISTRIB_PATH/$APPNAME/Contents/Frameworks/libIFXCoreStatic.a
#install_name_tool -change libIFXCore.1.so @rpath/libIFXCore.1.so $DISTRIB_PATH/$APPNAME/Contents/PlugIns/libio_u3d.dylib
#install_name_tool -change libIFXExporting.1.so @rpath/libIFXExporting.1.so $DISTRIB_PATH/$APPNAME/Contents/PlugIns/libio_u3d.dylib
#install_name_tool -change libIFXScheduling.1.so @rpath/libIFXScheduling.1.so $DISTRIB_PATH/$APPNAME/Contents/PlugIns/libio_u3d.dylib

if [ -e $QTDIR/bin/macdeployqt ]
then
    $QTDIR/bin/macdeployqt $BUNDLE_PATH/$APPNAME
else
    macdeployqt $BUNDLE_PATH/$APPNAME
fi
