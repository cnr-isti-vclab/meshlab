#!/bin/bash

SCRIPTS_PATH="$(dirname "$(realpath "$0")")"/..
RESOURCES_PATH=$SCRIPTS_PATH/../../resources
INSTALL_PATH=$SCRIPTS_PATH/../../install
PACKAGES_PATH=$SCRIPTS_PATH/../../packages

#checking for parameters
for i in "$@"
do
case $i in
    -i=*|--install_path=*)
        INSTALL_PATH="${i#*=}"
        shift # past argument=value
        ;;
    -p=*|--packages_path=*)
        PACKAGES_PATH="${i#*=}"
        shift # past argument=value
        ;;
    *)
        # unknown option
        ;;
esac
done

# Ensure appimagetool is in the PATH
if ! command -v appimagetool &> /dev/null
then
    echo "appimagetool could not be found, attempting to install it..."
    wget https://github.com/AppImage/appimagetool/releases/download/continuous/appimagetool-x86_64.AppImage \
        && chmod +x appimagetool-x86_64.AppImage \
        && sudo mv appimagetool-x86_64.AppImage /usr/local/bin/appimagetool

    if ! command -v appimagetool &> /dev/null
    then
        echo "Failed to install appimagetool. Please install it manually."
        exit 1
    fi
else
    echo "appimagetool is already installed."
fi

ARCH=$(uname -m)
$RESOURCES_PATH/linux/$ARCH/linuxdeploy --appdir=$INSTALL_PATH \
  --output appimage

#get version
IFS=' ' #space delimiter
STR_VERSION=$($INSTALL_PATH/AppRun --version)
read -a strarr <<< "$STR_VERSION"
ML_VERSION=${strarr[1]} #get the meshlab version from the string

# Ensure the packages path exists
mkdir -p $PACKAGES_PATH

# Move the AppImage to the packages directory
mkdir -p $PACKAGES_PATH
mv MeshLab-*.AppImage $PACKAGES_PATH/MeshLab$ML_VERSION-linux_$ARCH.AppImage
