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

QTCOMPONENTS="QtCore QtGui QtOpenGL QtNetwork QtXml QtXmlPatterns QtScript QtWidgets"

QTCORE=QtCore.framework/Versions/5/QtCore
QTGUI=QtGui.framework/Versions/5/QtGui
QTNETWORK=QtNetwork.framework/Versions/5/QtNetwork
QTSCRIPT=QtScript.framework/Versions/5/QtScript
QTXMLPATTERNS=QtXmlPatterns.framework/Versions/5/QtXmlPatterns
QTXML=QtXml.framework/Versions/5/QtXml
QTOPENGL=QtOpenGL.framework/Versions/5/QtOpenGL
QTWIDGETS=QtWidgets.framework/Versions/5/QtWidgets

if [ -e $APPFOLDER -a -d $APPFOLDER ]
then
  echo "------------------"
else
  echo "Started in the wrong dir: I have not found the MeshLab.app"
  exit 0
fi

echo "Copying the built app into the bundle"

rm -r -f $BUNDLE

mkdir $BUNDLE
cp -r $APPFOLDER $BUNDLE

# copy the files icons into the app.
cp ../meshlab/images/meshlab_obj.icns $BUNDLE/$APPNAME/Contents/Resources
# copy the qt.conf file in the resources to avoid problems with qt's jpg dll
cp ../install/qt.conf $BUNDLE/$APPNAME/Contents/Resources

read -rsp $'Copying samples and other files. Press enter to continue...\n'

mkdir $BUNDLE/$APPNAME/Contents/Frameworks   
mkdir $BUNDLE/$APPNAME/Contents/plugins   
mkdir $BUNDLE/$APPNAME/Contents/plugins/imageformats 
mkdir $BUNDLE/$APPNAME/Contents/plugins/platforms 
  
mkdir $BUNDLE/$APPNAME/Contents/plugins/U3D_OSX  
cp plugins/U3D_OSX/IDTFConverter.out  $BUNDLE/$APPNAME/Contents/plugins/U3D_OSX
cp plugins/U3D_OSX/IDTFConverter.sh  $BUNDLE/$APPNAME/Contents/plugins/U3D_OSX
cp plugins/U3D_OSX/libIFXCore.so  $BUNDLE/$APPNAME/Contents/plugins/U3D_OSX
mkdir $BUNDLE/$APPNAME/Contents/plugins/U3D_OSX/Plugins
cp plugins/U3D_OSX/Plugins/libIFXExporting.so  $BUNDLE/$APPNAME/Contents/plugins/U3D_OSX/Plugins

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
echo 'Copying Qt Components into the app'
#read -rsp $'Press enter to continue...\n'

for x in $QTCOMPONENTS
do
#    cp -R $QTPATH/lib/$x.framework $BUNDLE/$APPNAME/Contents/Frameworks 
rsync -quiet -avu --exclude='*debug*' $QTPATH/lib/$x.framework $BUNDLE/$APPNAME/Contents/Frameworks
done

echo "Changing the paths of the qt component frameworks"
#read -rsp $'Press enter to continue...\n'

  for x in $QTCOMPONENTS
  do
   install_name_tool -id  @executable_path/../Frameworks/$x.framework/Versions/Current/$x $BUNDLE/meshlab.app/Contents/Frameworks/$x.framework/Versions/Current/$x
  done

install_name_tool -change $QTPATH/lib/$QTCORE    @executable_path/../Frameworks/$QTCORE    $BUNDLE/meshlab.app/Contents/Frameworks/$QTGUI
install_name_tool -change $QTPATH/lib/$QTCORE    @executable_path/../Frameworks/$QTCORE    $BUNDLE/meshlab.app/Contents/Frameworks/$QTXML
install_name_tool -change $QTPATH/lib/$QTCORE    @executable_path/../Frameworks/$QTCORE    $BUNDLE/meshlab.app/Contents/Frameworks/$QTXMLPATTERNS
install_name_tool -change $QTPATH/lib/$QTNETWORK @executable_path/../Frameworks/$QTNETWORK $BUNDLE/meshlab.app/Contents/Frameworks/$QTXMLPATTERNS
install_name_tool -change $QTPATH/lib/$QTCORE    @executable_path/../Frameworks/$QTCORE    $BUNDLE/meshlab.app/Contents/Frameworks/$QTNETWORK
install_name_tool -change $QTPATH/lib/$QTCORE    @executable_path/../Frameworks/$QTCORE    $BUNDLE/meshlab.app/Contents/Frameworks/$QTSCRIPT
install_name_tool -change $QTPATH/lib/$QTCORE    @executable_path/../Frameworks/$QTCORE    $BUNDLE/meshlab.app/Contents/Frameworks/$QTOPENGL
install_name_tool -change $QTPATH/lib/$QTGUI     @executable_path/../Frameworks/$QTGUI     $BUNDLE/meshlab.app/Contents/Frameworks/$QTOPENGL

echo "Copying the meshlab plugins and xml's in the meshlab package" #--------------------------
#read -rsp $'Press enter to continue...\n'


for x in $BUILDPATH/distrib/plugins/*.dylib
do
cp ./$x $BUNDLE/meshlab.app/Contents/plugins/
done
pwd
for x in $BUILDPATH/distrib/plugins/*.xml
do
cp ./$x $BUNDLE/meshlab.app/Contents/plugins/
done

echo "Copying the qt plugins" 
#read -rsp $'Press enter to continue...\n'

for x in $QTPATH/plugins/imageformats/*.dylib
do
  PLUGINNAME=$(basename $x)
  cp $x $BUNDLE/meshlab.app/Contents/plugins/imageformats
  install_name_tool -change $QTPATH/lib/$QTCORE  @executable_path/../Frameworks/$QTCORE  $BUNDLE/meshlab.app/Contents/plugins/imageformats/$PLUGINNAME
  install_name_tool -change $QTPATH/lib/$QTGUI   @executable_path/../Frameworks/$QTGUI   $BUNDLE/meshlab.app/Contents/plugins/imageformats/$PLUGINNAME
done
for x in $QTPATH/plugins/platforms/*.dylib
do
  PLUGINNAME=$(basename $x)
  cp $x $BUNDLE/meshlab.app/Contents/plugins/platforms
  install_name_tool -change $QTPATH/lib/$QTCORE  @executable_path/../Frameworks/$QTCORE  $BUNDLE/meshlab.app/Contents/plugins/platforms/$PLUGINNAME
  install_name_tool -change $QTPATH/lib/$QTGUI   @executable_path/../Frameworks/$QTGUI   $BUNDLE/meshlab.app/Contents/plugins/platforms/$PLUGINNAME
done

echo "Now Changing how each exec (meshlab and all the dll refers) " #--------------------------
#read -rsp $'Press enter to continue...\n'

#EXECNAMES="MacOS/libcommon.1.dylib MacOS/meshlab $PLUGINSNAMESLIB" 

for x in $BUNDLE/meshlab.app/Contents/plugins/*.dylib
do
PLUGINNAME=$(basename $x)
EXECNAMES="$EXECNAMES plugins/$PLUGINNAME"
done
#EXECNAMES="MacOS/libcommon.1.dylib MacOS/meshlab $EXECNAMES" 
EXECNAMES="MacOS/meshlab" 
echo "iii $EXECNAMES"
#read -rsp $'Press enter to continue...\n'

for x in $EXECNAMES 
do
  install_name_tool -change @rpath/$QTCORE @executable_path/../Frameworks/$QTCORE       $BUNDLE/meshlab.app/Contents/$x
  install_name_tool -change @rpath/$QTGUI @executable_path/../Frameworks/$QTGUI         $BUNDLE/meshlab.app/Contents/$x
  install_name_tool -change @rpath/$QTNETWORK @executable_path/../Frameworks/$QTNETWORK $BUNDLE/meshlab.app/Contents/$x
  install_name_tool -change @rpath/$QTOPENGL @executable_path/../Frameworks/$QTOPENGL   $BUNDLE/meshlab.app/Contents/$x
  install_name_tool -change @rpath/$QTXML @executable_path/../Frameworks/$QTXML         $BUNDLE/meshlab.app/Contents/$x
  install_name_tool -change @rpath/$QTXMLPATTERNS @executable_path/../Frameworks/$QTXMLPATTERNS $BUNDLE/meshlab.app/Contents/$x
  install_name_tool -change @rpath/$QTSCRIPT @executable_path/../Frameworks/$QTSCRIPT   $BUNDLE/meshlab.app/Contents/$x
  install_name_tool -change @rpath/$QTWIDGETS @executable_path/../Frameworks/$QTWIDGETS $BUNDLE/meshlab.app/Contents/$x
  install_name_tool -change libcommon.1.dylib @executable_path/libcommon.1.dylib        $BUNDLE/meshlab.app/Contents/$x
done

cd ../install
