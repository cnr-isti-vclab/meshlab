#!/bin/bash

SCRIPTS_PATH="$(dirname "$(realpath "$0")")"/..

INSTALL_PATH=$SCRIPTS_PATH/../../install
NOTAR_USER=""
NOTAR_PASSWORD=""
NOTAR_TEAM_ID=""

#checking for parameters
for i in "$@"
do
case $i in
    -i=*|--install_path=*)
        INSTALL_PATH="${i#*=}"
        shift # past argument=value
        ;;
    -nu=*|--notarization_user=*)
        NOTAR_USER="${i#*=}"
        shift # past argument=value
        ;;
    -nt=*|--notarization_team=*)
        NOTAR_TEAM_ID="${i#*=}"
        shift # past argument=value
        ;;
    -np=*|--notarization_pssw=*)
        NOTAR_PASSWORD="${i#*=}"
        shift # past argument=value
        ;;
    *)
        # unknown option
        ;;
esac
done

xcrun notarytool store-credentials "notarytool-profile" --apple-id $NOTAR_USER --team-id $NOTAR_TEAM_ID --password $NOTAR_PASSWORD

ditto -c -k --keepParent "$INSTALL_PATH/meshlab.app" "$INSTALL_PATH/notarization.zip"

xcrun notarytool submit "install/notarization.zip" --keychain-profile "notarytool-profile" --wait

xcrun stapler staple "$INSTALL_PATH/meshlab.app"

rm -rf $INSTALL_PATH/notarization.zip