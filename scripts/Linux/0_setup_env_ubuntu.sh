#!/bin/bash
# this is a script shell sets up an ubuntu (16.04, 18.04, 20.04) environment where
# MeshLab can be compiled.
#
# Run this script if you never installed any of the MeshLab dependencies.

sudo apt-get update 
sudo apt-get install -y qt5-default qttools5-dev-tools qtdeclarative5-dev
sudo apt-get install -y mesa-common-dev libglu1-mesa-dev libglew-dev lib3ds-dev libeigen3-dev 
sudo apt-get install -y libopenctm-dev libgmp-dev libqhull-dev libcgal-dev libboost-all-dev patchelf cmake
