#!/bin/bash
# this is a script shell for setting up the application bundle for the mac
# it should be run in the meshlab/src/install dir.
# it moves plugins and frameworks into the package and runs the 
# install_tool on them to change the linking path to the local version of qt
# the build was issued with
# qmake-4.3 "CONFIG += debug_and_release warn_off" meshlabv12.pro -recursive -spec macx-g++
# make clean
# make release
# Note that sometimes you have to copy by hand the icons in the meshlab.app/Contents/Resources directory

cd ../distrib
QTPATH="/Library/Frameworks"
APPNAME="meshlab.app"

BUNDLE="MeshLabBundle"

QTCOMPONENTS="QtCore QtGui QtOpenGL QtNetwork QtXml QtScript"

QTCORE=QtCore.framework/Versions/4.0/QtCore
QTGUI=QtGui.framework/Versions/4.0/QtGui

if [ -e $APPNAME -a -d $APPNAME ]
then
  echo "------------------"
else
  echo "Started in the wrong dir: I have not found the MeshLab.app"
  exit 0
fi

echo "Starting to copying stuff in the bundle"

rm -r -f $BUNDLE

mkdir $BUNDLE
cp -r meshlab.app $BUNDLE

# copy the files icons into the app.
cp ../meshlab/images/meshlab_obj.icns $BUNDLE/$APPNAME/Contents/Resources
# copy the qt.conf file in the resources to avoid problems with qt's jpg dll
cp ../install/qt.conf $BUNDLE/$APPNAME/Contents/Resources



mkdir $BUNDLE/$APPNAME/Contents/Frameworks   
mkdir $BUNDLE/$APPNAME/Contents/plugins   
mkdir $BUNDLE/$APPNAME/Contents/plugins/imageformats 
  
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


for x in $QTCOMPONENTS
do
#    cp -R $QTPATH/$x.framework $BUNDLE/$APPNAME/Contents/Frameworks 
rsync -quiet -avu --exclude='*debug*' $QTPATH/$x.framework $BUNDLE/$APPNAME/Contents/Frameworks
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
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/$QTCORE $BUNDLE/meshlab.app/Contents/Frameworks/QtScript.framework/Versions/4.0/QtScript
install_name_tool -change QtGui.framework/Versions/4/QtGui   @executable_path/../Frameworks/$QTGUI  $BUNDLE/meshlab.app/Contents/Frameworks/QtOpenGL.framework/Versions/4.0/QtOpenGL

echo "Copying the plugins in the meshlab package" #--------------------------

PLUGINSNAMES="\
plugins/libfilter_aging.dylib \
plugins/libfilter_ao.dylib \
plugins/libfilter_autoalign.dylib \
plugins/libfilter_camera.dylib \
plugins/libfilter_clean.dylib \
plugins/libfilter_colorize.dylib \
plugins/libfilter_colorproc.dylib \
plugins/libfilter_create.dylib \
plugins/libfilter_csg.dylib \
plugins/libfilter_fractal.dylib \
plugins/libfilter_func.dylib \
plugins/libfilter_isoparametrization.dylib \
plugins/libfilter_layer.dylib \
plugins/libfilter_measure.dylib \
plugins/libfilter_meshing.dylib \
plugins/libfilter_mls.dylib \
plugins/libfilter_photosynth.dylib \
plugins/libfilter_plymc.dylib \
plugins/libfilter_poisson.dylib \
plugins/libfilter_qhull.dylib \
plugins/libfilter_quality.dylib \
plugins/libfilter_sampling.dylib \
plugins/libfilter_select.dylib \
plugins/libfilter_slice.dylib \
plugins/libfilter_ssynth.dylib \
plugins/libfilter_texture.dylib \
plugins/libfilter_trioptimize.dylib \
plugins/libfilter_unsharp.dylib \
plugins/libfilter_zippering.dylib \
plugins/libfiltercreateiso.dylib \
plugins/libfiltergeodesic.dylib \
plugins/libio_3ds.dylib \
plugins/libio_base.dylib \
plugins/libio_bre.dylib \
plugins/libio_collada.dylib \
plugins/libio_epoch.dylib \
plugins/libio_expe.dylib \
plugins/libio_gts.dylib \
plugins/libio_json.dylib \
plugins/libio_m.dylib \
plugins/libio_pdb.dylib \
plugins/libio_tri.dylib \
plugins/libio_u3d.dylib \
plugins/libio_x3d.dylib \
plugins/libedit_hole.dylib \
plugins/libedit_pickpoints.dylib \
plugins/libedit_quality.dylib \
plugins/libedit_select.dylib \
plugins/libedit_texture.dylib \
plugins/libeditalign.dylib \
plugins/libeditmeasure.dylib \
plugins/libeditpaint.dylib \
plugins/libeditsegment.dylib \
plugins/libdecorate_base.dylib \
plugins/libdecorate_background.dylib \
plugins/libdecorate_shadow.dylib \
plugins/librender_gdp.dylib \
plugins/librender_radiance_scaling.dylib \
plugins/librender_rfx.dylib \
plugins/librender_splatting.dylib \
plugins/libsampleedit.dylib \
plugins/libsamplefilter.dylib \
plugins/libsamplefilterdyn.dylib
"
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

EXECNAMES="MacOS/libcommon.1.dylib MacOS/meshlab $PLUGINSNAMES" 
for x in $EXECNAMES
do
  install_name_tool -change QtCore.framework/Versions/4/QtCore       @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore       $BUNDLE/meshlab.app/Contents/$x
  install_name_tool -change QtGui.framework/Versions/4/QtGui         @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui         $BUNDLE/meshlab.app/Contents/$x
  install_name_tool -change QtNetwork.framework/Versions/4/QtNetwork @executable_path/../Frameworks/QtNetwork.framework/Versions/4/QtNetwork $BUNDLE/meshlab.app/Contents/$x
  install_name_tool -change QtOpenGL.framework/Versions/4/QtOpenGL   @executable_path/../Frameworks/QtOpenGL.framework/Versions/4/QtOpenGL   $BUNDLE/meshlab.app/Contents/$x
  install_name_tool -change QtXml.framework/Versions/4/QtXml         @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml         $BUNDLE/meshlab.app/Contents/$x
  install_name_tool -change QtScript.framework/Versions/4/QtScript   @executable_path/../Frameworks/QtScript.framework/Versions/4/QtScript   $BUNDLE/meshlab.app/Contents/$x
  install_name_tool -change libcommon.1.dylib                        @executable_path/libcommon.1.dylib         $BUNDLE/meshlab.app/Contents/$x
done

cd ../install
