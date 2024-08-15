#!/bin/bash

SCRIPTS_PATH="$(dirname "$(realpath "$0")")"

INSTALL_PATH=$SCRIPTS_PATH/../../install
QT_DIR_OPTION=""
PACKAGES_PATH=$SCRIPTS_PATH/../../packages
SIGN=false
NOTARIZE=false
CERT_ID=""
NOTAR_USER=""
NOTAR_TEAM_ID=""
NOTAR_PASSWORD=""

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
    -p=*|--packages_path=*)
        PACKAGES_PATH="${i#*=}"
        shift # past argument=value
        ;;
    -ci=*|--cert_id=*)
        CERT_ID="${i#*=}"
        if [ -n "$CERT_ID" ]; then
          SIGN=true
        fi
        shift # past argument=value
        ;;
    -nu=*|--notarization_user=*)
        NOTAR_USER="${i#*=}"
        if [ -n "$NOTAR_USER" ]; then
          NOTARIZE=true
        fi
        shift # past argument=value
        ;;
    -np=*|--notarization_pssw=*)
        NOTAR_PASSWORD="${i#*=}"
        shift # past argument=value
        ;;
    -nt=*|--notarization_team=*)
        NOTAR_TEAM_ID="${i#*=}"
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

if [ "$NOTARIZE" = true ] ; then
    bash $SCRIPTS_PATH/internal/2c_notarize_appbundle.sh -i=$INSTALL_PATH -nu=$NOTAR_USER -nt=$NOTAR_TEAM_ID -np=$NOTAR_PASSWORD

    echo "======= AppBundle Notarized ======="
fi

bash $SCRIPTS_PATH/internal/2d_dmg.sh -i=$INSTALL_PATH -p=$PACKAGES_PATH

echo "======= DMG Created ======="