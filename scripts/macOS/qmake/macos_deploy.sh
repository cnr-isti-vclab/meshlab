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
    DISTRIB_PATH="../../../distrib"
else
    DISTRIB_PATH=$( realpath $1 )
fi

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $DIR #move to script directory

SOURCE_PATH=$PWD/../../../src

APPNAME="meshlab.app"

echo "Hopefully I should find" $DISTRIB_PATH/$APPNAME

if ! [ -e $DISTRIB_PATH/$APPNAME -a -d $DISTRIB_PATH/$APPNAME ]
then
    echo "Started in the wrong dir: I have not found the meshlab.app"
    exit -1
fi

echo "Copying the built app into the bundle"
mkdir $DISTRIB_PATH/$APPNAME/Contents/PlugIns
# copy the files icons into the app.
cp $SOURCE_PATH/meshlab/images/meshlab_obj.icns $DISTRIB_PATH/$APPNAME/Contents/Resources

for x in $DISTRIB_PATH/plugins/*.dylib
do
cp $x $DISTRIB_PATH/$APPNAME/Contents/PlugIns/
done

#copy libIFX libraries and change rpath u3d plugin
cp -a $DISTRIB_PATH/lib/meshlab/libIFX* $DISTRIB_PATH/$APPNAME/Contents/Frameworks
rm $DISTRIB_PATH/$APPNAME/Contents/Frameworks/libIFXCoreStatic.a
install_name_tool -change libIFXCore.1.so @rpath/libIFXCore.1.so $DISTRIB_PATH/$APPNAME/Contents/PlugIns/libio_u3d.dylib
install_name_tool -change libIFXExporting.1.so @rpath/libIFXExporting.1.so $DISTRIB_PATH/$APPNAME/Contents/PlugIns/libio_u3d.dylib
install_name_tool -change libIFXScheduling.1.so @rpath/libIFXScheduling.1.so $DISTRIB_PATH/$APPNAME/Contents/PlugIns/libio_u3d.dylib

echo 'Copying other files'

#shaders
mkdir $DISTRIB_PATH/$APPNAME/Contents/shaders
cp -r $DISTRIB_PATH/shaders/*.gdp $DISTRIB_PATH/shaders/*.vert $DISTRIB_PATH/shaders/*.frag $DISTRIB_PATH/shaders/*.txt  $DISTRIB_PATH/$APPNAME/Contents/shaders

#added rendermonkey shaders
mkdir $DISTRIB_PATH/$APPNAME/Contents/shaders/shadersrm
cp $DISTRIB_PATH/shaders/shadersrm/*.rfx $DISTRIB_PATH/$APPNAME/Contents/shaders/shadersrm
#added shadowmapping shaders
cp -r $DISTRIB_PATH/shaders/decorate_shadow $DISTRIB_PATH/$APPNAME/Contents/shaders

echo "Changing the paths of the qt component frameworks using the qt tool macdeployqt"

if [ -e $QTDIR/bin/macdeployqt ]
then
    $QTDIR/bin/macdeployqt $DISTRIB_PATH/$APPNAME -executable=$DISTRIB_PATH/$APPNAME/Contents/MacOS/meshlabserver
else
    macdeployqt $DISTRIB_PATH/$APPNAME -executable=$DISTRIB_PATH/$APPNAME/Contents/MacOS/meshlabserver
fi
