#!/bin/bash
# this is a script shell for setting up the application bundle for MacOS
# Default directories (DISTRIB_PATH and SOURCE_PATH) are set in order to work if
# you did not use shadow build and you are running (not sourcing) this script
# from the install/macx directory.
# If you built MeshLab with shadow build, run this script with the path to the
# ditrib directory as a parameter:
# sh macinstall_latest.sh /path/to/shadow/build/distrib
#
# This script does all the dirty work of moving all the needed plugins and
# frameworks into the package and runs the install_tool on them to change the
# linking path to the local version of qt.
#
# The env variable $QTDIR should be correctly set, and all the stuff to be copied
# should be in the '$DISTRIB_PATH' folder.

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $DIR #move to script directory
 
#checking for parameters
if [ "$#" -eq 0 ]
then
    DISTRIB_PATH=$DIR/../../distrib
else
    DISTRIB_PATH=$1
fi

echo "Build path is: " $DISTRIB_PATH

# change this if this script is not in the directory meshlab/install/macx
SOURCE_PATH=$DIR/../../src

APPNAME="meshlab.app"

echo "Hopefully I should find" $DISTRIB_PATH/$APPNAME

APPFOLDER=$DISTRIB_PATH/$APPNAME
BUNDLE=$DISTRIB_PATH/MeshLabBundle


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

for x in $DISTRIB_PATH/plugins/*.dylib
do
cp $x $BUNDLE/meshlab.app/Contents/PlugIns/
done

for x in $DISTRIB_PATH/plugins/*.xml
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

cp -r $DISTRIB_PATH/plugins/U3D_OSX  $BUNDLE/$APPNAME/Contents/PlugIns/

mkdir $BUNDLE/$APPNAME/Contents/shaders   
cp $DISTRIB_PATH/shaders/*.gdp $DISTRIB_PATH/shaders/*.vert $DISTRIB_PATH/shaders/*.frag $DISTRIB_PATH/shaders/*.txt  $BUNDLE/$APPNAME/Contents/shaders

#added rendermonkey shaders
mkdir $BUNDLE/$APPNAME/Contents/shaders/shadersrm   
cp $DISTRIB_PATH/shaders/shadersrm/*.rfx $BUNDLE/$APPNAME/Contents/shaders/shadersrm
#added shadowmapping shaders
cp -r $DISTRIB_PATH/shaders/decorate_shadow $BUNDLE/$APPNAME/Contents/shaders

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
sed "s%DISTRIB_PATH%$BUNDLE%g" resources/meshlab_dmg_latest.json > resources/meshlab_dmg_final.json
sed -i '' "s%SOURCE_PATH%$SOURCE_PATH%g" resources/meshlab_dmg_final.json

rm -f $DISTRIB_PATH/*.dmg

echo "Running appdmg"
appdmg resources/meshlab_dmg_final.json $DISTRIB_PATH/MeshLab$(date +%Y.%m).dmg
