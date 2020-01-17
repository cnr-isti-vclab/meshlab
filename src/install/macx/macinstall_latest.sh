#!/bin/bash
# this is a script shell for setting up the application bundle for the mac
# It should be run (not sourced) in the meshlab/src/install/macx dir.
#
# It does all the dirty work of moving all the needed plugins and frameworks into the package and runs the 
# install_tool on them to change the linking path to the local version of qt
#
# The env variable $QTDIR should be correctly set, and all the stuff to be copied
# should be in the 'distrib' folder.
 
cd ../../distrib

# change this according to the shadow build dir.
# is the root of the build e.g. where the meshlab_full.pro it can be something like 
BUILDPATH="../../build-meshlab_full-Desktop_Qt_5_9_4_clang_64bit2-Release"

APPNAME="meshlab.app"

BUILDPATH=..
echo "Hopefully I should find" $BUILDPATH/distrib/$APPNAME

APPFOLDER=$BUILDPATH/distrib/$APPNAME
BUNDLE="MeshLabBundle"


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
cp ../meshlab/images/meshlab_obj.icns $BUNDLE/$APPNAME/Contents/Resources

for x in $BUILDPATH/distrib/plugins/*.dylib
do
cp ./$x $BUNDLE/meshlab.app/Contents/PlugIns/
done

for x in $BUILDPATH/distrib/plugins/*.xml
do
cp ./$x $BUNDLE/meshlab.app/Contents/PlugIns/
done

for x in $BUNDLE/meshlab.app/Contents/PlugIns/*.dylib
do
 install_name_tool -change libcommon.1.dylib @executable_path/libcommon.1.dylib $x
done

echo 'Copying other files'

cp ../../LICENSE.txt $BUNDLE
cp ../../docs/readme.txt $BUNDLE

mkdir $BUNDLE/$APPNAME/Contents/PlugIns/U3D_OSX  
cp plugins/U3D_OSX/IDTFConverter.out  $BUNDLE/$APPNAME/Contents/PlugIns/U3D_OSX
cp plugins/U3D_OSX/IDTFConverter.sh  $BUNDLE/$APPNAME/Contents/PlugIns/U3D_OSX
cp plugins/U3D_OSX/libIFXCore.so  $BUNDLE/$APPNAME/Contents/PlugIns/U3D_OSX
mkdir $BUNDLE/$APPNAME/Contents/PlugIns/U3D_OSX/Plugins
cp plugins/U3D_OSX/Plugins/libIFXExporting.so  $BUNDLE/$APPNAME/Contents/PlugIns/U3D_OSX/Plugins

mkdir $BUNDLE/$APPNAME/Contents/textures   
cp textures/*.png $BUNDLE/$APPNAME/Contents/textures/
mkdir $BUNDLE/$APPNAME/Contents/textures/cubemaps   
cp textures/cubemaps/uffizi*.jpg $BUNDLE/$APPNAME/Contents/textures/cubemaps
mkdir $BUNDLE/$APPNAME/Contents/textures/litspheres   
cp textures/litspheres/*.png $BUNDLE/$APPNAME/Contents/textures/litspheres

mkdir $BUNDLE/$APPNAME/Contents/shaders   
cp shaders/*.gdp shaders/*.vert shaders/*.frag shaders/*.txt  $BUNDLE/$APPNAME/Contents/shaders

#added rendermonkey shaders
mkdir $BUNDLE/$APPNAME/Contents/shaders/shadersrm   
cp shaders/shadersrm/*.rfx $BUNDLE/$APPNAME/Contents/shaders/shadersrm
#added shadowmapping shaders
cp -r shaders/decorate_shadow $BUNDLE/$APPNAME/Contents/shaders

echo "Changing the paths of the qt component frameworks using the qt tool macdeployqt"

if [ -e $QTDIR/bin/macdeployqt ]
then
echo
$QTDIR/bin/macdeployqt $BUNDLE/$APPNAME -verbose=2 -executable=$BUNDLE/$APPNAME/Contents/MacOS/meshlabserver
else
macdeployqt $BUNDLE/$APPNAME -verbose=2 -executable=$BUNDLE/$APPNAME/Contents/MacOS/meshlabserver
fi

otool -L $BUNDLE/$APPNAME/Contents/MacOS/meshlabserver

cd ../install/macx
# final step create the dmg using appdmg
# appdmg is installed with 'npm install -g appdmg'",

rm -f ../../distrib/MeshLab*.dmg  
appdmg meshlab_dmg_latest.json ../../distrib/MeshLab$(date +%Y.%m).dmg
