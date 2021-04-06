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

#checking for parameters
if [ "$#" -eq 0 ]
then
    DISTRIB_PATH="../../../distrib"
else
    DISTRIB_PATH=$( realpath $1 )
fi

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $DIR #move to script directory

DISTRIB_PATH=$( realpath $DISTRIB_PATH)

if ! [ -e $DISTRIB_PATH/meshlab.app -a -d $DISTRIB_PATH/meshlab.app ]
then
    echo "Started in the wrong dir: I have not found the meshlab.app"
    exit -1
fi

SOURCE_PATH=$DIR/../../../src

#get version
IFS=' ' #space delimiter
STR_VERSION=$($DISTRIB_PATH/meshlab.app/Contents/MacOS/meshlab --version)
read -a strarr <<< "$STR_VERSION"
ML_VERSION=${strarr[1]} #get the meshlab version from the string

# final step create the dmg using appdmg
# appdmg is installed with 'npm install -g appdmg'",
sed "s%DISTRIB_PATH%$DISTRIB_PATH%g" ../resources/meshlab_dmg_latest.json > ../resources/meshlab_dmg_final.json
sed -i '' "s%ML_VERSION%$ML_VERSION%g" ../resources/meshlab_dmg_final.json
sed -i '' "s%SOURCE_PATH%$SOURCE_PATH%g" ../resources/meshlab_dmg_final.json

rm -f $DISTRIB_PATH/*.dmg

mv $DISTRIB_PATH/meshlab.app $DISTRIB_PATH/MeshLab$ML_VERSION.app

echo "Running appdmg"
appdmg ../resources/meshlab_dmg_final.json $DISTRIB_PATH/MeshLab$ML_VERSION.dmg

#at this point, distrib folder contains a DMG MeshLab file
echo "distrib folder now contains a DMG file"
