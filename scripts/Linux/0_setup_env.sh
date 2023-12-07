#!/bin/bash
# this is a script shell sets up an ubuntu (18.04, 20.04 and 22.04) environment where
# MeshLab can be compiled.
#
# Run this script if you never installed any of the MeshLab dependencies.

DONT_INSTALL_QT=false
DONT_INSTALL_CGAL_BOOST=true
DONT_INSTALL_EMBREE=true

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

echo "=== installing cmake, patchelf, gmp, mpfr and xcerces-c..."
sudo apt-get install -y cmake ninja-build patchelf libgmp-dev libmpfr-dev libxerces-c-dev libtbb-dev

if [ "$DONT_INSTALL_QT" = false ] ; then
    echo "=== installing qt packages..."
    sudo apt-get install -y qt5-default qttools5-dev-tools qtdeclarative5-dev
else
    echo "=== jumping installation of qt packages..."
fi

# libraries that qt requires in any case
echo "=== intalling libraries required for qt deployment..."
sudo apt-get install -y libxkbcommon-x11-dev libxcb-icccm4-dev libxcb-image0-dev libxcb-keysyms1-dev libxcb-render-util0-dev libxcb-xinerama0-dev

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
fi
