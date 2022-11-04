#!/bin/bash
# this is a script shell sets up an ubuntu (18.04, 20.04 and 22.04) environment where
# MeshLab can be compiled.
#
# Run this script if you never installed any of the MeshLab dependencies.

DONT_INSTALL_QT=false
DONT_INSTALL_CGAL_BOOST=false
DONT_INSTALL_EMBREE=false

#checking for parameters
for i in "$@"
do
case $i in
    --dont_install_qt)
        DONT_INSTALL_QT=true
        shift # past argument=value
        ;;
    --dont_install_cgal_and_boost)
        DONT_INSTALL_CGAL_BOOST=true
        shift # past argument=value
        ;;
    --dont_install_embree)
        DONT_INSTALL_EMBREE=true
        shift # past argument=value
        ;;
    *)
        # unknown option
        ;;
esac
done

sudo apt-get update
echo "=== installing mesa packages..."
sudo apt-get install -y mesa-common-dev libglu1-mesa-dev

if [ "$DONT_INSTALL_QT" = false ] ; then
    echo "=== installing qt packages..."
    sudo apt-get install -y qt5-default qttools5-dev-tools qtdeclarative5-dev
else
    echo "=== jumping installation of qt packages..."
fi

echo "=== installing cmake, patchelf, gmp, mpfr, tbb, glfw3 and xcerces-c..."
sudo apt-get install -y cmake patchelf libgmp-dev libmpfr-dev libtbb-dev libglfw3-dev libxerces-c-dev

# possibility to use always system libraries starting from ubuntu 20.04, since cgal is header only
if [ "$DONT_INSTALL_CGAL_BOOST" = false ] ; then
    echo "=== installing cgal and boost..."
    sudo apt-get install -y libcgal-dev libboost-all-dev
else
    echo "=== jumping installation of cgal and boost packages..."
fi

if [ "$DONT_INSTALL_EMBREE" = false ] ; then
    echo "=== installing embree..."
    sudo apt-get install -y libembree-dev
else
	echo "=== jumping installation of embree package..."
    #default paths wrt the script folder
    #SCRIPTS_PATH="$(dirname "$(realpath "$0")")"
    #EXTERNAL_PATH=$SCRIPTS_PATH/../../src/external

    #cd $EXTERNAL_PATH

    #wget https://github.com/ispc/ispc/releases/download/v1.18.1/ispc-v1.18.1-linux.tar.gz
    #tar -xf ispc-v1.18.1-linux.tar.gz
    #rm ispc-v1.18.1-linux.tar.gz

    #wget https://github.com/embree/embree/archive/refs/tags/v3.13.5.zip
    #unzip v3.13.5.zip
    #rm v3.13.5.zip
fi
