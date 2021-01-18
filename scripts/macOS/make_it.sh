#!/bin/bash
# This is a script shell for compiling and deploying MeshLab in a MacOS environment.
#
# Requires a Qt environment which is set-up properly, and an accessible
# cmake binary.
#
# Without given arguments, MeshLab will be built in the meshlab/src/build,
# the folder meshlab/src/install will contain meshlab.app and
# the DMG will be placed in meshlab/src.
#
# You can give as argument the build path, the install path (that will contain
# the portable version of MeshLab), and the number of cores to use to build MeshLab
# (default: 4).
# The DMG will be placed in the parent directory of the install path.
#
# Example of call:
# bash make_it.sh --build_path=path/to/build --install_path=path/to/install -j8

SCRIPTS_PATH=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
SOURCE_PATH=$SCRIPTS_PATH/../../src
BUILD_PATH=$SOURCE_PATH/build
INSTALL_PATH=$SOURCE_PATH/install
CORES="-j4"
DOUBLE_PRECISION_OPTION=""

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
    -j*)
    CORES=$i
    shift # past argument=value
    ;;
    --double_precision)
    DOUBLE_PRECISION_OPTION="--double_precision"
    shift # past argument=value
    ;;
    *)
          # unknown option
    ;;
esac
done

sh $SCRIPTS_PATH/1_build.sh -b=$BUILD_PATH -i=$INSTALL_PATH $DOUBLE_PRECISION_OPTION $CORES
sh $SCRIPTS_PATH/2_deploy.sh -i=$INSTALL_PATH
sh $SCRIPTS_PATH/3_dmg.sh -i=$INSTALL_PATH $DOUBLE_PRECISION_OPTION
