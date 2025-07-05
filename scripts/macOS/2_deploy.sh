#!/bin/bash

SCRIPTS_PATH="$(dirname "$(realpath "$0")")"

INSTALL_PATH=$SCRIPTS_PATH/../../install
QT_DIR_OPTION=""

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
    --use_brew_qt)
        QT_DIR_OPTION="--use_brew_qt"
        shift # past argument=value
        ;;
    *)
        # unknown option
        ;;
esac
done

bash $SCRIPTS_PATH/internal/2a_appbundle.sh -i=$INSTALL_PATH $QT_DIR_OPTION

echo "======= AppBundle Created ======="