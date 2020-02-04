#!/bin/bash
# this is a script shell for setting up the application DMG for MacOS.
# Requires a properly built and deployed meshlab (requires to run the
# macos_deploy.sh script first).
#
# Without given arguments, meshlab.app will be looked for in meshlab/distrib
# folder. MeshLab DMG will be placed in the same directory of meshlab.app.
# 
# You can give as argument the DISTRIB_PATH containing meshlab.app.

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $DIR #move to script directory

#checking for parameters
if [ "$#" -eq 0 ]
then
    DISTRIB_PATH=$PWD/../../distrib
else
    DISTRIB_PATH=$1
fi

if ! [ -e $DISTRIB_PATH/meshlab.app -a -d $DISTRIB_PATH/meshlab.app ]
then
    echo "Started in the wrong dir: I have not found the meshlab.app"
    exit -1
fi

SOURCE_PATH=$DIR/../../src

# final step create the dmg using appdmg
# appdmg is installed with 'npm install -g appdmg'",
sed "s%DISTRIB_PATH%$DISTRIB_PATH%g" resources/meshlab_dmg_latest.json > resources/meshlab_dmg_final.json
sed -i '' "s%SOURCE_PATH%$SOURCE_PATH%g" resources/meshlab_dmg_final.json

rm -f $DISTRIB_PATH/*.dmg

echo "Running appdmg"
appdmg resources/meshlab_dmg_final.json $DISTRIB_PATH/MeshLab$(date +%Y.%m).dmg

#at this point, distrib folder contains a DMG MeshLab file
echo "distrib folder now contains a DMG file"
