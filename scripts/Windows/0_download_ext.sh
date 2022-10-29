#!/bin/bash

#default paths wrt the script folder
SCRIPTS_PATH="$(dirname "$(realpath "$0")")"
EXTERNAL_PATH=$SCRIPTS_PATH/../../src/external

cd $EXTERNAL_PATH

wget https://github.com/CGAL/cgal/releases/download/v5.2.1/CGAL-5.2.1.zip
unzip CGAL-5.2.1.zip
rm CGAL-5.2.1.zip

wget https://github.com/CGAL/cgal/releases/download/v5.2.1/CGAL-5.2.1-win64-auxiliary-libraries-gmp-mpfr.zip
unzip -o CGAL-5.2.1-win64-auxiliary-libraries-gmp-mpfr.zip -d CGAL-5.2.1/
rm CGAL-5.2.1-win64-auxiliary-libraries-gmp-mpfr.zip
