# This is a powershell script for computing the meshlab_final.nsi script.
# Requires:
# - a properly deployed meshlab (see windows_deploy.ps1);
#
# Without given arguments, the folder that will be deployed is meshlab/distrib.
#
# You can give as argument the DISTRIB_PATH.
#
# After running this script, a meshlab_final.script can be found in the resources folder.
# This script is ready to be run by makensis.exe

SCRIPTS_PATH="$(dirname "$(realpath "$0")")"/..
SOURCE_PATH=$SCRIPTS_PATH/../../src

#checking for parameters
if [ "$#" -eq 0 ]
then
    BUNDLE_PATH=$SOURCE_PATH/install
else
    BUNDLE_PATH=$(realpath $1)
fi

#if(! (Test-Path meshlab.exe)){ #meshlab.exe not found inside $DISTRIB_PATH
#    cd $DIR
#	throw 'meshlab.exe not found in ' + ($BUNDLE_PATH) + '. Exiting.'
#}

VERSION=$(cat $SOURCE_PATH/../ML_VERSION)

sed "s%MESHLAB_VERSION%$VERSION%g" $SCRIPTS_PATH/resources/meshlab.nsi > $SCRIPTS_PATH/resources/meshlab_final.nsi
sed -i "s%DISTRIB_PATH%$BUNDLE_PATH%g" $SCRIPTS_PATH/resources/meshlab_final.nsi
