#!/bin/bash
# this is a script shell sets up a MacOS environment where
# MeshLab can be compiled.
#
# Run this script if you never installed any of the MeshLab dependencies.
#
# Requires: homebrew

DONT_INSTALL_QT=false

#checking for parameters
for i in "$@"
do
case $i in
    --dont_install_qt)
        DONT_INSTALL_QT=true
        shift # past argument=value
        ;;
    *)
        # unknown option
        ;;
esac
done

brew install coreutils node cmake ninja libomp cgal xerces-c tbb embree
npm install -g appdmg

if [ "$DONT_INSTALL_QT" = false ] ; then
    echo "=== installing qt packages..."

    brew install qt5
else
    echo "=== jumping installation of qt packages..."
fi
