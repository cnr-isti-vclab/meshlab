#!/bin/bash
# this is a script shell for deploying a meshlab-portable app.
# Requires a properly built meshlab.
#
# Without given arguments, the folder that will be deployed is meshlab/install.
#
# You can give as argument the INSTALL_PATH in the following way:
# bash 2_deploy.sh  --install_path=/path/to/install
# -i argument is also supported.
#
# After running this script, $INSTALL_PATH/meshlab.app will be a portable meshlab application.

SCRIPTS_PATH=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

INSTALL_PATH=$SCRIPTS_PATH/../../install
QT_DIR=""

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

${QT_DIR}macdeployqt $INSTALL_PATH/$APPNAME \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libedit_align.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_csg.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_isoparametrization.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_mesh_booleans.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_screened_poisson.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_sketchfab.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libio_e57.so
