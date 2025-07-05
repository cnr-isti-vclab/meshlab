#!/bin/bash

SCRIPTS_PATH="$(dirname "$(realpath "$0")")"

INSTALL_PATH=$SCRIPTS_PATH/../../install
PACKAGES_PATH=$SCRIPTS_PATH/../../packages

bash $SCRIPTS_PATH/internal/2d_dmg.sh -i=$INSTALL_PATH -p=$PACKAGES_PATH

echo "======= DMG Created ======="