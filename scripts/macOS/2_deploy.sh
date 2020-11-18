#!/bin/bash
# this is a script shell for deploying a meshlab-portable app.
# Requires a properly built meshlab.
#
# Without given arguments, the folder that will be deployed is meshlab/distrib.
#
# You can give as argument the DISTRIB_PATH.
#
# After running this script, $DISTRIB_PATH/meshlab.app will be a portable meshlab application.

#realpath function
realpath() {
    [[ $1 = /* ]] && echo "$1" || echo "$PWD/${1#./}"
}

SCRIPTS_PATH=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
INSTALL_PATH=$SCRIPTS_PATH/../../src/install
APPNAME="meshlab.app"

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

echo "Hopefully I should find " $INSTALL_PATH/$APPNAME

if ! [ -e $INSTALL_PATH/$APPNAME -a -d $INSTALL_PATH/$APPNAME ]
then
    echo "Started in the wrong dir: I have not found the meshlab.app"
    exit -1
fi

if [ -e $QTDIR/bin/macdeployqt ]
then
    $QTDIR/bin/macdeployqt $INSTALL_PATH/$APPNAME -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_sketchfab.so
else
    macdeployqt $INSTALL_PATH/$APPNAME -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_sketchfab.so
fi
