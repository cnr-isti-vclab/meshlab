#!/bin/bash

SCRIPTS_PATH="$(dirname "$(realpath "$0")")"/../
RESOURCES_PATH=$SCRIPTS_PATH/../../resources
SOURCE_PATH=$SCRIPTS_PATH/../../src
INSTALL_PATH=$SOURCE_PATH/../install

#check parameters
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

#check if we have an exec in distrib
if ! [ -f $INSTALL_PATH/usr/bin/meshlab ]
then
    echo "ERROR: meshlab bin not found inside $INSTALL_PATH/usr/bin/"
    exit 1
fi

mkdir -p $INSTALL_PATH/usr/share/doc/meshlab
mkdir -p $INSTALL_PATH/usr/share/icons/Yaru/256x256@2x/apps/
mkdir -p $INSTALL_PATH/usr/share/icons/Yaru/256x256/apps/
mkdir -p $INSTALL_PATH/usr/share/icons/Yaru/48x48@2x/apps/
mkdir -p $INSTALL_PATH/usr/share/icons/Yaru/48x48/apps/
mkdir -p $INSTALL_PATH/usr/share/icons/Yaru/32x32@2x/apps/
mkdir -p $INSTALL_PATH/usr/share/icons/Yaru/32x32/apps/
mkdir -p $INSTALL_PATH/usr/share/icons/Yaru/24x24@2x/apps/
mkdir -p $INSTALL_PATH/usr/share/icons/Yaru/24x24/apps/
mkdir -p $INSTALL_PATH/usr/share/icons/Yaru/22x22@2x/apps/
mkdir -p $INSTALL_PATH/usr/share/icons/Yaru/22x22/apps/
mkdir -p $INSTALL_PATH/usr/share/icons/Yaru/16x16@2x/apps/
mkdir -p $INSTALL_PATH/usr/share/icons/Yaru/16x16/apps/
mkdir -p $INSTALL_PATH/usr/share/icons/Yaru/8x8@2x/apps/
mkdir -p $INSTALL_PATH/usr/share/icons/Yaru/8x8/apps/

cp $RESOURCES_PATH/linux/meshlab.desktop $INSTALL_PATH/usr/share/applications/meshlab.desktop
cp $RESOURCES_PATH/icons/meshlab512.png $INSTALL_PATH/usr/share/icons/Yaru/256x256@2x/apps/meshlab.png
cp $RESOURCES_PATH/icons/meshlab256.png $INSTALL_PATH/usr/share/icons/Yaru/256x256/apps/meshlab.png
cp $RESOURCES_PATH/icons/meshlab96.png $INSTALL_PATH/usr/share/icons/Yaru/48x48@2x/apps/meshlab.png
cp $RESOURCES_PATH/icons/meshlab48.png $INSTALL_PATH/usr/share/icons/Yaru/48x48/apps/meshlab.png
cp $RESOURCES_PATH/icons/meshlab64.png $INSTALL_PATH/usr/share/icons/Yaru/32x32@2x/apps/meshlab.png
cp $RESOURCES_PATH/icons/meshlab32.png $INSTALL_PATH/usr/share/icons/Yaru/32x32/apps/meshlab.png
cp $RESOURCES_PATH/icons/meshlab48.png $INSTALL_PATH/usr/share/icons/Yaru/24x24@2x/apps/meshlab.png
cp $RESOURCES_PATH/icons/meshlab24.png $INSTALL_PATH/usr/share/icons/Yaru/24x24/apps/meshlab.png
cp $RESOURCES_PATH/icons/meshlab44.png $INSTALL_PATH/usr/share/icons/Yaru/22x22@2x/apps/meshlab.png
cp $RESOURCES_PATH/icons/meshlab22.png $INSTALL_PATH/usr/share/icons/Yaru/22x22/apps/meshlab.png
cp $RESOURCES_PATH/icons/meshlab32.png $INSTALL_PATH/usr/share/icons/Yaru/16x16@2x/apps/meshlab.png
cp $RESOURCES_PATH/icons/meshlab16.png $INSTALL_PATH/usr/share/icons/Yaru/16x16/apps/meshlab.png
cp $RESOURCES_PATH/icons/meshlab16.png $INSTALL_PATH/usr/share/icons/Yaru/8x8@2x/apps/meshlab.png
cp $RESOURCES_PATH/icons/meshlab8.png $INSTALL_PATH/usr/share/icons/Yaru/8x8/apps/meshlab.png
cp $RESOURCES_PATH/LICENSE.txt $INSTALL_PATH/usr/share/doc/meshlab/
cp $RESOURCES_PATH/privacy.txt $INSTALL_PATH/usr/share/doc/meshlab/
cp $RESOURCES_PATH/readme.txt $INSTALL_PATH/usr/share/doc/meshlab/

for plugin in $INSTALL_PATH/usr/lib/meshlab/plugins/*.so
do
    # allow plugins to find linked libraries in usr/lib, usr/lib/meshlab and usr/lib/meshlab/plugins
    patchelf --set-rpath '$ORIGIN/../../:$ORIGIN/../:$ORIGIN' $plugin
done

chmod +x $INSTALL_PATH/usr/bin/meshlab