#!/bin/bash

SCRIPTS_PATH="$(dirname "$(realpath "$0")")"/..

INSTALL_PATH=$SCRIPTS_PATH/../../install
NOT_USER=""
NOT_PASSWORD=""

#checking for parameters
for i in "$@"
do
case $i in
    -i=*|--install_path=*)
        INSTALL_PATH="${i#*=}"
        shift # past argument=value
        ;;
    -nu=*|--notarization_user=*)
        NOT_USER="${i#*=}"
        shift # past argument=value
        ;;
    -np=*|--notarization_password=*)
        NOT_PASSWORD="${i#*=}"
        shift # past argument=value
        ;;
    *)
        # unknown option
        ;;
esac
done

xcrun notarytool store-credentials "notarytool-profile" --apple-id "$NOT_USER" --password "$NOT_PASSWORD"

ditto -c -k --keepParent "$INSTALL_PATH/meshlab.app" "$INSTALL_PATH/notarization.zip"

xcrun notarytool submit "install/notarization.zip" --keychain-profile "notarytool-profile" --wait

xcrun stapler staple "$INSTALL_PATH/meshlab.app"

rm -rf $INSTALL_PATH/notarization.zip