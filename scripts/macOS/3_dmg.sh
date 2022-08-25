#!/bin/bash
# this is a script shell for setting up the application DMG for MacOS.
# Requires a properly built and deployed meshlab (requires to run the
# 2_deploy.sh script first).
#
# Without given arguments, meshlab.app will be looked for in meshlab/install
# folder. MeshLab DMG will be placed in the same directory of meshlab.app.
#
# You can give as argument the INSTALL_PATH containing meshlab.app, with -i or
# --install_path option.

SCRIPTS_PATH=$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)
RESOURCES_PATH=$SCRIPTS_PATH/../../resources
INSTALL_PATH=$SCRIPTS_PATH/../../install

#checking for parameters
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

if ! [ -e $INSTALL_PATH/meshlab.app -a -d $INSTALL_PATH/meshlab.app ]
then
    echo "Started in the wrong dir: I have not found the meshlab.app"
    exit -1
fi

#get version
IFS=' ' #space delimiter
STR_VERSION=$($INSTALL_PATH/meshlab.app/Contents/MacOS/meshlab --version)
read -a strarr <<< "$STR_VERSION"
ML_VERSION=${strarr[1]} #get the meshlab version from the string

# final step create the dmg using appdmg
# appdmg is installed with 'npm install -g appdmg'",
sed "s%DISTRIB_PATH%$INSTALL_PATH%g" $RESOURCES_PATH/macos/meshlab_dmg_latest.json > $RESOURCES_PATH/macos/meshlab_dmg_final.json
sed -i '' "s%ML_VERSION%$ML_VERSION%g" $RESOURCES_PATH/macos/meshlab_dmg_final.json
sed -i '' "s%RESOURCES_PATH%$RESOURCES_PATH%g" $RESOURCES_PATH/macos/meshlab_dmg_final.json

rm -f $INSTALL_PATH/*.dmg

mv $INSTALL_PATH/meshlab.app $INSTALL_PATH/MeshLab$ML_VERSION.app

echo "Running appdmg"
appdmg $RESOURCES_PATH/macos/meshlab_dmg_final.json $INSTALL_PATH/MeshLab$ML_VERSION-macos.dmg

rm $RESOURCES_PATH/macos/meshlab_dmg_final.json

#at this point, distrib folder contains a DMG MeshLab file
echo "$INSTALL_PATH folder now contains a DMG file"
