#!/bin/bash
# this is a script shell for setting up the application DMG for MacOS.
# Requires a properly built and deployed meshlab (requires to run the
# macos_deploy.sh script first).
#
# Without given arguments, meshlab.app will be looked for in meshlab/distrib
# folder. MeshLab DMG will be placed in the same directory of meshlab.app.
#
# You can give as argument the DISTRIB_PATH containing meshlab.app.

#realpath function
realpath() {
    [[ $1 = /* ]] && echo "$1" || echo "$PWD/${1#./}"
}

SCRIPTS_PATH=$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)
INSTALL_PATH=$SCRIPTS_PATH/../../src/install
ML_VERSION=$(cat $SCRIPTS_PATH/../../ML_VERSION)

#checking for parameters
for i in "$@"
do
case $i in
    -i=*|--install_path=*)
    INSTALL_PATH="${i#*=}"
    shift # past argument=value
    ;;
    --double_precision)
    ML_VERSION=$ML_VERSIONd
    shift # past argument=value
    ;;
    *)
          # unknown option
    ;;
esac
done

if ! [ -e $INSTALL_PATH/meshlab.app -a -d $INSTALL_PATH/meshlab.app ]
then
    echo "Started in the wrong dir: I have not found the meshlab.app"
    exit -1
fi

SOURCE_PATH=$SCRIPTS_PATH/../../src

# final step create the dmg using appdmg
# appdmg is installed with 'npm install -g appdmg'",
sed "s%DISTRIB_PATH%$INSTALL_PATH%g" $SCRIPTS_PATH/resources/meshlab_dmg_latest.json > $SCRIPTS_PATH/resources/meshlab_dmg_final.json
sed -i '' "s%SOURCE_PATH%$SOURCE_PATH%g" $SCRIPTS_PATH/resources/meshlab_dmg_final.json

rm -f $INSTALL_PATH/*.dmg

echo "Running appdmg"
appdmg $SCRIPTS_PATH/resources/meshlab_dmg_final.json $INSTALL_PATH/MeshLab$ML_VERSION.dmg

rm $SCRIPTS_PATH/resources/meshlab_dmg_final.json

#at this point, distrib folder contains a DMG MeshLab file
echo "distrib folder now contains a DMG file"
