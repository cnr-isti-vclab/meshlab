#!/bin/bash

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

$RESOURCES_PATH/linux/linuxdeploy --appdir=$INSTALL_PATH \
  --plugin qt

# after deploy, all required libraries are placed into usr/lib, therefore we can remove the ones in
# usr/lib/meshlab (except for the ones that are loaded at runtime)
shopt -s extglob

# If the script fails for some reason, it causes the deletion of files that weren't meant to be deleted.
# We can fix this with a simple if statement

if [[ -d $INSTALL_PATH/usr/lib/meshlab ]]
then

  cd $INSTALL_PATH/usr/lib/meshlab
  rm -v !("libIFXCore.so"|"libIFXExporting.so"|"libIFXScheduling.so")

  echo "$INSTALL_PATH is now a self contained meshlab application"

else
#at this point, distrib folder contains all the files necessary to execute meshlab
  echo "$INSTALL_PATH/usr/lib/meshlab was not created. Script could not continue running."
fi
