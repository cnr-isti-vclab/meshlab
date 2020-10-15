#!/bin/bash
# this is a script shell for compiling meshlab in a MacOS environment.
# Requires a Qt environment which is set-up properly, and an accessible
# cmake binary.
#
# Without given arguments, MeshLab will be built in the meshlab/src/build
# directory, and installed in $BUILD_PATH/../install.
#
# You can give as argument the BUILD_PATH and the INSTALL_PATH in the
# following way:
# sh macos_build.sh --build_path=/path/to/build --install_path=/path/to/install
# -b and -i arguments are also supported.

#realpath function
realpath() {
    [[ $1 = /* ]] && echo "$1" || echo "$PWD/${1#./}"
}

#default paths wrt the script folder
SCRIPTS_PATH=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
BUILD_PATH=$SCRIPTS_PATH/../../src/build
INSTALL_PATH=$SCRIPTS_PATH/../../src/install
CORES="-j4"

#check parameters
for i in "$@"
do
case $i in
    -b=*|--build_path=*)
    BUILD_PATH="${i#*=}"
    shift # past argument=value
    ;;
    -i=*|--install_path=*)
    INSTALL_PATH="${i#*=}"
    shift # past argument=value
    ;;
    -j*)
    CORES=$i
    shift # past argument=value
    ;;
    *)
          # unknown option
    ;;
esac
done

#create build path if necessary
if ! [ -d $BUILD_PATH ]
then
    mkdir -p $BUILD_PATH
fi

#create install path if necessary
if ! [ -d $INSTALL_PATH ]
then
    mkdir -p $INSTALL_PATH
fi

BUILD_PATH=$(realpath $BUILD_PATH)
INSTALL_PATH=$(realpath $INSTALL_PATH)

#move to script directory
cd $SCRIPTS_PATH

#source path wrt script path
SOURCE_PATH=$PWD/../../src

cd $BUILD_PATH
cmake -DCMAKE_INSTALL_PREFIX=$INSTALL_PATH $SOURCE_PATH
make $CORES
#make install
