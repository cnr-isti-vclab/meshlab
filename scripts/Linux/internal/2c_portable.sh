#!/bin/bash

shopt -s extglob

SCRIPTS_PATH="$(dirname "$(realpath "$0")")"/..
RESOURCES_PATH=$SCRIPTS_PATH/../../resources
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
        QT_DIR=${i#*=}
        shift # past argument=value
        ;;
    *)
        # unknown option
        ;;
esac
done

if [ ! -z "$QT_DIR" ]
then
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$QT_DIR/lib
    export QMAKE=$QT_DIR/bin/qmake
fi

# Make sure that deploy succeeds before we start deleting files
if $RESOURCES_PATH/linux/x86_64/linuxdeploy --appdir=$INSTALL_PATH --plugin qt; then
  # after deploy, all required libraries are placed into usr/lib, therefore we can remove the ones in
  # usr/lib/meshlab (except for the ones that are loaded at runtime)
  cd $INSTALL_PATH/usr/lib/meshlab
  rm -v !("libIFXCore.so"|"libIFXExporting.so"|"libIFXScheduling.so")

  echo "$INSTALL_PATH is now a self contained meshlab application"

else
  echo "linuxdeploy failed with error code $?. Script was not completed successfully."
  exit 1
fi
