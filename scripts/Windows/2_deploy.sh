#!/bin/bash

SCRIPTS_PATH="$(dirname "$(realpath "$0")")"
RESOURCES_PATH=$SCRIPTS_PATH/../../resources
INSTALL_PATH=$SCRIPTS_PATH/../../install
QT_DIR_OPTION=""
PACKAGES_PATH=$SCRIPTS_PATH/../../packages
SIGN=false
CERT_FILE_OPTION=""
CERT_PSSW=""

#checking for parameters
for i in "$@"
do
case $i in
    -i=*|--install_path=*)
        INSTALL_PATH="${i#*=}"
        shift # past argument=value
        ;;
    -qt=*|--qt_dir=*)
        QT_DIR_OPTION=qt="${i#*=}"
        shift # past argument=value
        ;;
    -p=*|--packages_path=*)
        PACKAGES_PATH="${i#*=}"
        shift # past argument=value
        ;;
    -cf=*|--cert_file=*)
        CERT_FILE_OPTION=cf="${i#*=}"
        shift # past argument=value
        ;;
    -cp=*|--cert_pssw=*)
        CERT_PSSW="${i#*=}"
        if [ -n "$CERT_PSSW" ]; then
            SIGN=true
        fi
        shift # past argument=value
        ;;
    *)
        # unknown option
        ;;
esac
done

bash $SCRIPTS_PATH/internal/2a_portable.sh -i=$INSTALL_PATH $QT_DIR_OPTION

echo "======= Portable Version Created ======="

if [ "$SIGN" = true ] ; then
    bash $SCRIPTS_PATH/internal/2b_sign_dlls.sh -i=$INSTALL_PATH  $CERT_FILE_OPTION -cp=$CERT_PSSW

    echo "======= Portable Version Signed ======="
fi

bash $SCRIPTS_PATH/internal/2c_installer.sh -i=$INSTALL_PATH -p=$PACKAGES_PATH

echo "======= Installer Created ======="

if [ "$SIGN" = true ] ; then
    bash $SCRIPTS_PATH/internal/2b_sign_dlls.sh -i=$PACKAGES_PATH  $CERT_FILE_OPTION -cp=$CERT_PSSW

    echo "======= Installer Signed ======="
fi
