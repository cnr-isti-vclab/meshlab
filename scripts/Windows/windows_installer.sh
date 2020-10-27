# This is a powershell script for computing an installer for MeshLab.
# Requires:
# - a properly deployed meshlab (see windows_deploy.ps1);
# - the env variable PATH containing the NSIS folder (makensis.exe must be directly accessible)
#
# Without given arguments, the folder that will be deployed is meshlab/distrib.
#
# You can give as argument the DISTRIB_PATH.
#
# After running this script, the installer can be found inside the resources folder.

SCRIPTS_PATH="$(dirname "$(realpath "$0")")"

#checking for parameters
if [ "$#" -eq 0 ]
then
    BUNDLE_PATH=$SCRIPTS_PATH/../../src/install
else
    BUNDLE_PATH=$(realpath $1)
fi

sh $SCRIPTS_PATH/resources/windows_nsis_script.sh $BUNDLE_PATH

makensis.exe $SCRIPTS_PATH/resources/meshlab_final.nsi

rm $SCRIPTS_PATH/resources/meshlab_final.nsi
