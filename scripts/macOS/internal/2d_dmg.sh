#!/bin/bash

SCRIPTS_PATH="$(dirname "$(realpath "$0")")"/..
RESOURCES_PATH=$SCRIPTS_PATH/../../resources
INSTALL_PATH=$SCRIPTS_PATH/../../install
PACKAGES_PATH=$SCRIPTS_PATH/../../packages

#checking for parameters
for i in "$@"
do
case $i in
    -i=*|--install_path=*)
        INSTALL_PATH="${i#*=}"
        shift # past argument=value
        ;;
    -p=*|--packages_path=*)
        PACKAGES_PATH="${i#*=}"
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

mkdir $PACKAGES_PATH

# get running architecture
ARCH=$(uname -m)

appdmg $RESOURCES_PATH/macos/meshlab_dmg_final.json $PACKAGES_PATH/MeshLab$ML_VERSION-macos_$ARCH.dmg

rm $RESOURCES_PATH/macos/meshlab_dmg_final.json