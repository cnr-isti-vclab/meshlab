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
        QT_DIR=${i#*=}
        shift # past argument=value
        ;;
    --use_brew_qt)
        QT_DIR=$(brew --prefix qt@5)
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

QT_BASE_DIR=""

# if QT_DIR is not empty
if [ -n "$QT_DIR" ]; then
    # set QT_BASE_DIR to the path of QT_DIR/bin
    QT_BASE_DIR="${QT_DIR}/bin/"
fi

# save in message the output of macdeployqt
message=$(${QT_BASE_DIR}macdeployqt $INSTALL_PATH/$APPNAME \
    $ARGUMENTS 2>&1)

# if message contains "ERROR" then macdeployqt failed
if [[ $message == *"ERROR"* ]]; then
    echo "macdeployqt failed."
    echo "macdeployqt output:"
    echo $message
    exit 1
fi

# remove everything from install path, except the appbundle
cd $INSTALL_PATH
ls | grep -xv "${APPNAME}" | xargs rm

echo "$INSTALL_PATH is now a self contained meshlab application"
