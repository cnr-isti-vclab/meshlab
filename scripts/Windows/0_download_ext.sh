#!/bin/bash

#default paths wrt the script folder
SCRIPTS_PATH="$(dirname "$(realpath "$0")")"
EXTERNAL_PATH=$SCRIPTS_PATH/../../src/external

cd $EXTERNAL_PATH
