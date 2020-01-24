#!/bin/bash
# this is a script shell for setting up the application bundle for MacOS
# Default directories (BUILD_PATH and SOURCE_PATH) are set in order to work if
# you did not use shadow build and you are running (not sourcing) this script
# from the install/macx directory.
# If you built MeshLab with shadow build, run this script with the path to the
# build directory as a parameter:
# sh macinstall_latest.sh /path/to/shadow/build
#
# This script does all the dirty work of moving all the needed plugins and
# frameworks into the package and runs the install_tool on them to change the
# linking path to the local version of qt.
#
# The env variable $QTDIR should be correctly set, and all the stuff to be copied
# should be in the '$BUILD_PATH/distrib' folder.
 
#checking for parameters
if [ "$#" -eq 0 ]
then
    BUILD_PATH="../.."
else
    BUILD_PATH=$1
fi

echo "Build path is: " $BUILD_PATH

# change this if this script is not in the directory meshlab/install/macx
SOURCE_PATH="../../src"

APPNAME="meshlab.app"

echo "Hopefully I should find" $BUILD_PATH/distrib/$APPNAME

APPFOLDER=$BUILD_PATH/distrib/$APPNAME
BUNDLE=$BUILD_PATH/distrib/MeshLabBundle


if [ -e $APPFOLDER -a -d $APPFOLDER ]
then
  echo "------------------"
else
  echo "Started in the wrong dir: I have not found the MeshLab.app"
  exit 0
fi

# Start by erasing everything
rm -r -f $BUNDLE

echo "Copying the built app into the bundle"
mkdir $BUNDLE
cp -r $APPFOLDER $BUNDLE
mkdir $BUNDLE/$APPNAME/Contents/PlugIns
# copy the files icons into the app.
cp $SOURCE_PATH/meshlab/images/meshlab_obj.icns $BUNDLE/$APPNAME/Contents/Resources

for x in $BUILD_PATH/distrib/plugins/*.dylib
do
cp $x $BUNDLE/meshlab.app/Contents/PlugIns/
done

for x in $BUILD_PATH/distrib/plugins/*.xml
do
cp $x $BUNDLE/meshlab.app/Contents/PlugIns/
done

for x in $BUNDLE/meshlab.app/Contents/PlugIns/*.dylib
do
 install_name_tool -change libcommon.1.dylib @executable_path/libcommon.1.dylib $x
done

echo 'Copying other files'

cp $SOURCE_PATH/../LICENSE.txt $BUNDLE
cp $SOURCE_PATH/../docs/readme.txt $BUNDLE

cp -r $BUILD_PATH/distrib/plugins/U3D_OSX  $BUNDLE/$APPNAME/Contents/PlugIns/

mkdir $BUNDLE/$APPNAME/Contents/shaders   
cp $BUILD_PATH/distrib/shaders/*.gdp $BUILD_PATH/distrib/shaders/*.vert $BUILD_PATH/distrib/shaders/*.frag $BUILD_PATH/distrib/shaders/*.txt  $BUNDLE/$APPNAME/Contents/shaders

#added rendermonkey shaders
mkdir $BUNDLE/$APPNAME/Contents/shaders/shadersrm   
cp $BUILD_PATH/distrib/shaders/shadersrm/*.rfx $BUNDLE/$APPNAME/Contents/shaders/shadersrm
#added shadowmapping shaders
cp -r $BUILD_PATH/distrib/shaders/decorate_shadow $BUNDLE/$APPNAME/Contents/shaders

echo "Changing the paths of the qt component frameworks using the qt tool macdeployqt"

if [ -e $QTDIR/bin/macdeployqt ]
then
echo
$QTDIR/bin/macdeployqt $BUNDLE/$APPNAME -executable=$BUNDLE/$APPNAME/Contents/MacOS/meshlabserver
else
macdeployqt $BUNDLE/$APPNAME -executable=$BUNDLE/$APPNAME/Contents/MacOS/meshlabserver
fi

# final step create the dmg using appdmg
# appdmg is installed with 'npm install -g appdmg'",
sed "s%BUILD_PATH%$BUILD_PATH%g" meshlab_dmg_latest.json > meshlab_dmg_final.json
sed -i '' "s%SOURCE_PATH%$SOURCE_PATH%g" meshlab_dmg_final.json

rm -f $BUILD_PATH/distrib/*.dmg

echo "Running appdmg"
appdmg meshlab_dmg_final.json $BUILD_PATH/distrib/MeshLab$(date +%Y.%m).dmg
