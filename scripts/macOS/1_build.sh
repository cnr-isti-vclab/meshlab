#!/bin/bash

#default paths wrt the script folder
SCRIPTS_PATH=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
SOURCE_PATH=$SCRIPTS_PATH/../..
BUILD_PATH=$SOURCE_PATH/build
INSTALL_PATH=$SOURCE_PATH/install
DOUBLE_PRECISION_OPTION=""
NIGHTLY_OPTION=""
USE_BREW_LLVM=false
QT_DIR=""
CCACHE=""

#check parameters
for i in "$@"
do
case $i in
    -s=*|--source_path=*)
        SOURCE_PATH="${i#*=}"
        shift # past argument=value
        ;;
    -b=*|--build_path=*)
        BUILD_PATH="${i#*=}"
        shift # past argument=value
        ;;
    -i=*|--install_path=*)
        INSTALL_PATH="${i#*=}"
        shift # past argument=value
        ;;
    -d|--double_precision)
        DOUBLE_PRECISION_OPTION="-DMESHLAB_BUILD_WITH_DOUBLE_SCALAR=ON"
        shift # past argument=value
        ;;
    -n|--nightly)
        NIGHTLY_OPTION="-DMESHLAB_IS_NIGHTLY_VERSION=ON"
        shift # past argument=value
        ;;
    -qt=*|--qt_dir=*)
        QT_DIR=${i#*=}
        shift # past argument=value
        ;;
    --use_brew_llvm)
        USE_BREW_LLVM=true
        shift # past argument=value
        ;;
    --use_brew_qt)
        QT_DIR=$(brew --prefix qt@5)
        shift # past argument=value
        ;;
    --ccache)
        CCACHE="-DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache"
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

if [ ! -z "$QT_DIR" ]
then
    export Qt5_Dir=$QT_DIR
    export Qt5_DIR=$QT_DIR
fi

if [ "$USE_BREW_LLVM" = true ] ; then
    BREW_PATH="$(brew --prefix)"
    LLVM_PATH="$(brew --prefix llvm)"
    export SDKROOT=$(xcrun --sdk macosx --show-sdk-path)
    export LIBRARY_PATH="$LIBRARY_PATH:$SDKROOT/usr/lib"
    export PATH="$LLVM_PATH/bin:$PATH"
    export CC="$LLVM_PATH/bin/clang"
    export CXX="$LLVM_PATH/bin/clang++"
    export COMPILER=${CXX}
    export CFLAGS="-I $BREW_PATH/include -I $LLVM_PATH/include"
    export CXXFLAGS="-I $BREW_PATH/include -I $LLVM_PATH/include"
    export LDFLAGS="-L $LIBRARY_PATH -L $BREW_PATH/lib -L $LLVM_PATH/lib"
fi

BUILD_PATH=$(realpath $BUILD_PATH)
INSTALL_PATH=$(realpath $INSTALL_PATH)
OPENMP_PATH=$(brew --prefix libomp)

cd $BUILD_PATH
export NINJA_STATUS="[%p (%f/%t) ] "
cmake \
    -GNinja \
    -DCMAKE_BUILD_TYPE=Release \
    -DOpenMP_ROOT=$OPENMP_PATH \
    -DCMAKE_INSTALL_PREFIX=$INSTALL_PATH \
    $CCACHE \
    $DOUBLE_PRECISION_OPTION \
    $NIGHTLY_OPTION \
    $SOURCE_PATH

ninja
ninja install
