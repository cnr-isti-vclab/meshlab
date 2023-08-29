#!/bin/bash

SCRIPTS_PATH="$(dirname "$(realpath "$0")")"
INSTALL_PATH=$SCRIPTS_PATH/../../install
QT_DIR_OPTION=""
PACKAGES_PATH=$SCRIPTS_PATH/../../packages

#checking for parameters
for i in "$@"
do
case $i in
    -i=*|--install_path=*)
        INSTALL_PATH="${i#*=}"
        shift # past argument=value
        ;;
    -qt=*|--qt_dir=*)
        QT_DIR_OPTION=-qt=${i#*=}
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

# The script will terminate after the first line that fails
set -e

bash $SCRIPTS_PATH/internal/2a_make_bundle.sh -i=$INSTALL_PATH

echo "======= Bundle created ======="

bash $SCRIPTS_PATH/internal/2b_deb.sh -i=$INSTALL_PATH -p=$PACKAGES_PATH

echo "======= Deb Created ======="

bash $SCRIPTS_PATH/internal/2c_portable.sh -i=$INSTALL_PATH $QT_DIR_OPTION

echo "======= Portable Version Created ======="

bash $SCRIPTS_PATH/internal/2d_appimage.sh -i=$INSTALL_PATH -p=$PACKAGES_PATH

echo "======= AppImage Created ======="
