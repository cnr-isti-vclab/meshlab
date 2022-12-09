#!/bin/bash

SCRIPTS_PATH=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
SOURCE_PATH=$SCRIPTS_PATH/../..
BUILD_PATH=$SOURCE_PATH/build
INSTALL_PATH=$SOURCE_PATH/install
PACKAGES_PATH=$SOURCE_PATH/packages

DOUBLE_PRECISION_OPTION=""
NIGHTLY_OPTION=""
QT_DIR_OPTION=""
CCACHE_OPTION=""

#check parameters
for i in "$@"
do
case $i in
    -b=*|--build_path=*)
        BUILD_PATH="${i#*=}"
        shift # past argument=value
        ;;
    -i=*|--install_path=*)
        INSTALL_PATH="${i#*=}"/usr/
        shift # past argument=value
        ;;
    -p=*|--packages_path=*)
        PACKAGES_PATH="${i#*=}"
        shift # past argument=value
        ;;
    --double_precision)
        DOUBLE_PRECISION_OPTION="--double_precision"
        shift # past argument=value
        ;;
    -n|--nightly)
        NIGHTLY_OPTION="--nightly"
        shift # past argument=value
        ;;
    -qt=*|--qt_dir=*)
        QT_DIR_OPTION=-qt=${i#*=}
        shift # past argument=value
        ;;
    --ccache)
        CCACHE_OPTION="--ccache"
        shift # past argument=value
        ;;
    *)
        # unknown option
        ;;
esac
done

bash $SCRIPTS_PATH/1_build.sh -b=$BUILD_PATH -i=$INSTALL_PATH $NIGHTLY_OPTION $DOUBLE_PRECISION_OPTION $QT_DIR_OPTION $CCACHE_OPTION
bash $SCRIPTS_PATH/2_deploy.sh -i=$INSTALL_PATH -p=$PACKAGES_PATH $QT_DIR_OPTION

