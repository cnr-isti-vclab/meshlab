#!/bin/bash
# this is a script shell for setting up the application bundle for the mac
# it should be run in the meshlab/src/install dir.
# it moves plugins and frameworks into the package and runs the 
# install_tool on them to change the linking path to the local version of qt
# the build was issued with
# qmake-4.3 "CONFIG += debug_and_release warn_off" meshlabv11.pro -recursive -spec macx-g++
# make clean
# make release
# Note that sometimes you have to copy by hand the icons in the meshlab.app/Contents/Resources directory

cd ../meshlab
QTPATH="/Library/Frameworks"
APPNAME="meshlab.app"

BUNDLE="MeshLabBundle"

QTCOMPONENTS="QtCore QtGui QtOpenGL QtNetwork QtXml"

QTCORE=QtCore.framework/Versions/4.0/QtCore
QTGUI=QtGui.framework/Versions/4.0/QtGui

if [ -e $APPNAME -a -d $APPNAME ]
then
  echo "------------------"
else
  echo "Started in the wrong dir"
  exit 0
fi

echo "Starting to copying stuff in the bundle"

rm -r -f $BUNDLE

mkdir $BUNDLE
cp -r meshlab.app $BUNDLE

# copy the files icons into the app.
cp images/meshlab_obj.icns $BUNDLE/$APPNAME/Contents/Resources

mkdir $BUNDLE/$APPNAME/Contents/Frameworks   
mkdir $BUNDLE/$APPNAME/Contents/plugins   
mkdir $BUNDLE/$APPNAME/Contents/plugins/imageformats 
  
mkdir $BUNDLE/$APPNAME/Contents/plugins/U3D_OSX  
cp plugins/U3D_OSX/IDTFConverter.out  $BUNDLE/$APPNAME/Contents/plugins/U3D_OSX
cp plugins/U3D_OSX/IDTFConverter.sh  $BUNDLE/$APPNAME/Contents/plugins/U3D_OSX
cp plugins/U3D_OSX/IFXCore.dylib  $BUNDLE/$APPNAME/Contents/plugins/U3D_OSX
mkdir $BUNDLE/$APPNAME/Contents/plugins/U3D_OSX/Plugins
cp plugins/U3D_OSX/Plugins/IFXExporting.dylib  $BUNDLE/$APPNAME/Contents/plugins/U3D_OSX/Plugins

cp ../../docs/gpl.txt $BUNDLE
cp ../../docs/readme.txt $BUNDLE

mkdir $BUNDLE/sample
mkdir $BUNDLE/sample/images

cp ../sample/texturedknot.ply $BUNDLE/sample
cp ../sample/texturedknot.obj $BUNDLE/sample
cp ../sample/texturedknot.mtl $BUNDLE/sample
cp ../sample/TextureDouble_A.png $BUNDLE/sample
cp ../sample/Laurana50k.ply $BUNDLE/sample
cp ../sample/duck_triangulate.dae $BUNDLE/sample
cp ../sample/images/duckCM.jpg $BUNDLE/sample/images

mkdir $BUNDLE/$APPNAME/Contents/textures   
cp textures/*.png $BUNDLE/$APPNAME/Contents/textures/
mkdir $BUNDLE/$APPNAME/Contents/textures/cubemaps   
cp textures/cubemaps/uffizi*.jpg $BUNDLE/$APPNAME/Contents/textures/cubemaps

mkdir $BUNDLE/$APPNAME/Contents/shaders   
cp shaders/*.gdp shaders/*.vert shaders/*.frag shaders/*.txt  $BUNDLE/$APPNAME/Contents/shaders

for x in $QTCOMPONENTS
do
    cp -R $QTPATH/$x.framework $BUNDLE/$APPNAME/Contents/Frameworks 
done

echo "now trying to change the paths in the meshlab executable"

  for x in $QTCOMPONENTS
  do
   install_name_tool -id  @executable_path/../Frameworks/$x.framework/Versions/4.0/$x $BUNDLE/meshlab.app/Contents/Frameworks/$x.framework/Versions/4.0/$x
  done

install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/$QTCORE $BUNDLE/meshlab.app/Contents/Frameworks/QtGui.framework/Versions/4.0/QtGui
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/$QTCORE $BUNDLE/meshlab.app/Contents/Frameworks/QtXml.framework/Versions/4.0/QtXml
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/$QTCORE $BUNDLE/meshlab.app/Contents/Frameworks/QtNetwork.framework/Versions/4.0/QtNetwork
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/$QTCORE $BUNDLE/meshlab.app/Contents/Frameworks/QtOpenGL.framework/Versions/4.0/QtOpenGL
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/$QTCORE $BUNDLE/meshlab.app/Contents/Frameworks/QtOpenGL.framework/Versions/4.0/QtOpenGL
install_name_tool -change QtGui.framework/Versions/4/QtGui   @executable_path/../Frameworks/$QTGUI  $BUNDLE/meshlab.app/Contents/Frameworks/QtOpenGL.framework/Versions/4.0/QtOpenGL

echo "Copying the plugins in the meshlab package" #--------------------------

PLUGINSNAMES="\
plugins/libmeshfilter.dylib \
plugins/libbaseio.dylib \
plugins/libmeshio.dylib \
plugins/libcolladaio.dylib \
plugins/libepoch_io.dylib \
plugins/libu3d_io.dylib \
plugins/libmeshrender.dylib \
plugins/libcleanfilter.dylib \
plugins/libfilter_ao.dylib \
plugins/libfilter_poisson.dylib \
plugins/libmeshcolorize.dylib \
plugins/libmeshselect.dylib \
plugins/libmeshdecorate.dylib \
plugins/libmeshedit.dylib \
plugins/libeditslice.dylib \
plugins/libeditpaint.dylib \
plugins/libeditalign.dylib \
plugins/libeditmeasure.dylib \
plugins/libsampleedit.dylib \
plugins/libsamplefilter.dylib \
plugins/libsamplefilterdoc.dylib \
plugins/libsampledecoration.dylib \
plugins/libfilterborder.dylib \
plugins/libfiltergeodesic.dylib \
plugins/libfiltercreateiso.dylib"
pwd
for x in $PLUGINSNAMES
do
cp ./$x $BUNDLE/meshlab.app/Contents/plugins/
done


IMAGEFORMATSPLUGINS="libqjpeg.dylib libqgif.dylib libqtiff.dylib"
for x in $IMAGEFORMATSPLUGINS
do
cp /Developer/Applications/Qt/plugins/imageformats/$x $BUNDLE/meshlab.app/Contents/plugins/imageformats
install_name_tool -change QtCore.framework/Versions/4/QtCore  @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore  $BUNDLE/meshlab.app/Contents/plugins/imageformats/$x 
install_name_tool -change QtGui.framework/Versions/4/QtGui    @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui    $BUNDLE/meshlab.app/Contents/plugins/imageformats/$x 
done

echo "Now Changing " #--------------------------

EXECNAMES="MacOS/meshlab $PLUGINSNAMES" 
QTLIBPATH="/usr/local/Trolltech/Qt-4.3.3/lib"
for x in $EXECNAMES
do
  install_name_tool -change QtCore.framework/Versions/4/QtCore       @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore       $BUNDLE/meshlab.app/Contents/$x
  install_name_tool -change QtGui.framework/Versions/4/QtGui         @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui         $BUNDLE/meshlab.app/Contents/$x
  install_name_tool -change QtNetwork.framework/Versions/4/QtNetwork @executable_path/../Frameworks/QtNetwork.framework/Versions/4/QtNetwork $BUNDLE/meshlab.app/Contents/$x
  install_name_tool -change QtOpenGL.framework/Versions/4/QtOpenGL   @executable_path/../Frameworks/QtOpenGL.framework/Versions/4/QtOpenGL   $BUNDLE/meshlab.app/Contents/$x
  install_name_tool -change QtXml.framework/Versions/4/QtXml         @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml         $BUNDLE/meshlab.app/Contents/$x
done
