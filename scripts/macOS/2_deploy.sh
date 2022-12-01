#!/bin/bash

SCRIPTS_PATH="$(dirname "$(realpath "$0")")"

INSTALL_PATH=$SCRIPTS_PATH/../../install
QT_DIR_OPTION=""
PACKAGES_PATH=$SCRIPTS_PATH/../../packages
SIGN=false
CERT_ID=""

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
    -ci=*|--cert_id=*)
        SIGN=true
        CERT_ID="${i#*=}"
        shift # past argument=value
        ;;
    *)
        # unknown option
        ;;
esac
done

bash $SCRIPTS_PATH/internal/2a_appbundle.sh -i=$INSTALL_PATH $QT_DIR_OPTION

echo "======= AppBundle Created ======="

if [ "$SIGN" = true ] ; then
    bash $SCRIPTS_PATH/internal/2b_sign_appbundle.sh -i=$INSTALL_PATH -ci=$CERT_ID

    echo "======= AppBundle Signed ======="
fi

bash $SCRIPTS_PATH/internal/2c_dmg.sh -i=$INSTALL_PATH -p=$PACKAGES_PATH

echo "======= DMG Created ======="