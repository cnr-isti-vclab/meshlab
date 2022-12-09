#!/bin/bash

SCRIPTS_PATH="$(dirname "$(realpath "$0")")"/..

INSTALL_PATH=$SCRIPTS_PATH/../../install
QT_DIR=""
APPNAME="meshlab.app"

#checking for parameters
for i in "$@"
do
case $i in
    -i=*|--install_path=*)
        INSTALL_PATH="${i#*=}"
        shift # past argument=value
        ;;
    -qt=*|--qt_dir=*)
        QT_DIR=${i#*=}/bin/
        shift # past argument=value
        ;;
    *)
        # unknown option
        ;;
esac
done

ARGUMENTS=""

for plugin in $INSTALL_PATH/$APPNAME/Contents/PlugIns/*.so
do
    ARGUMENTS="${ARGUMENTS} -executable=${plugin}"
done

${QT_DIR}macdeployqt $INSTALL_PATH/$APPNAME \
    $ARGUMENTS

# remove everything from install path, except the appbundle
cd $INSTALL_PATH
ls | grep -xv "${APPNAME}" | xargs rm