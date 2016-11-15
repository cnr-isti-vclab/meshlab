#!/bin/bash
# this is a script shell for setting up the application bundle for the mac
# It should be run (not sourced) in the meshlab/src/install dir.
# It moves plugins and frameworks into the package and runs the 
# install_tool on them to change the linking path to the local version of qt
# the build was issued with
# qmake-4.3 "CONFIG += debug_and_release warn_off" meshlabv12.pro -recursive -spec macx-g++
# make clean
# make release
# Note that sometimes you have to copy by hand the icons in the meshlab.app/Contents/Resources directory
# 
cd ../distrib
QTPATH=$HOME/Qt/5.7/clang_64
# change this according to the shadow build dir.
BUILDPATH="../../build-meshlab_full-Desktop_Qt_5_7_0_clang_64bit-Release"
#BUILDPATH=""

APPNAME="meshlab.app"
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

for x in $BUNDLE/meshlab.app/Contents/plugins/*.dylib
do
 install_name_tool -change libcommon.1.dylib @executable_path/libcommon.1.dylib $x
done

echo 'Copying samples and other files'

cp ../../docs/gpl.txt $BUNDLE
cp ../../docs/readme.txt $BUNDLE

mkdir $BUNDLE/sample
mkdir $BUNDLE/sample/images
mkdir $BUNDLE/sample/normalmap

cp sample/texturedknot.ply $BUNDLE/sample
cp sample/texturedknot.obj $BUNDLE/sample
cp sample/texturedknot.mtl $BUNDLE/sample
cp sample/TextureDouble_A.png $BUNDLE/sample
cp sample/Laurana50k.ply $BUNDLE/sample
cp sample/duck_triangulate.dae $BUNDLE/sample
cp sample/images/duckCM.jpg $BUNDLE/sample/images
cp sample/seashell.gts $BUNDLE/sample
cp sample/chameleon4k.pts $BUNDLE/sample
cp sample/normalmap/laurana500.* $BUNDLE/sample/normalmap
cp sample/normalmap/matteonormb.* $BUNDLE/sample/normalmap


mkdir $BUNDLE/$APPNAME/Contents/plugins/U3D_OSX  
cp plugins/U3D_OSX/IDTFConverter.out  $BUNDLE/$APPNAME/Contents/plugins/U3D_OSX
cp plugins/U3D_OSX/IDTFConverter.sh  $BUNDLE/$APPNAME/Contents/plugins/U3D_OSX
cp plugins/U3D_OSX/libIFXCore.so  $BUNDLE/$APPNAME/Contents/plugins/U3D_OSX
mkdir $BUNDLE/$APPNAME/Contents/plugins/U3D_OSX/Plugins
cp plugins/U3D_OSX/Plugins/libIFXExporting.so  $BUNDLE/$APPNAME/Contents/plugins/U3D_OSX/Plugins

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
$QTPATH/bin/macdeployqt $BUNDLE/$APPNAME -verbose=2
cd ../install
