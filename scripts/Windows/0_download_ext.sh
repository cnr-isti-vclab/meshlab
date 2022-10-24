#!/bin/bash

#default paths wrt the script folder
SCRIPTS_PATH="$(dirname "$(realpath "$0")")"
EXTERNAL_PATH=$SCRIPTS_PATH/../../src/external

cd $EXTERNAL_PATH

# boost
wget https://boostorg.jfrog.io/artifactory/main/release/1.75.0/source/boost_1_75_0.zip
unzip boost_1_75_0.zip
rm boost_1_75_0.zip

# cgal
wget https://github.com/CGAL/cgal/releases/download/v5.2.1/CGAL-5.2.1.zip
unzip CGAL-5.2.1.zip
rm CGAL-5.2.1.zip

# gmp and mpfr
wget https://github.com/CGAL/cgal/releases/download/v5.2.1/CGAL-5.2.1-win64-auxiliary-libraries-gmp-mpfr.zip
unzip -o CGAL-5.2.1-win64-auxiliary-libraries-gmp-mpfr.zip -d CGAL-5.2.1/
rm CGAL-5.2.1-win64-auxiliary-libraries-gmp-mpfr.zip

# embree
wget https://github.com/embree/embree/releases/download/v3.13.5/embree-3.13.5.x64.vc14.windows.zip
unzip -o embree-3.13.5.x64.vc14.windows.zip
rm embree-3.13.5.x64.vc14.windows.zip
