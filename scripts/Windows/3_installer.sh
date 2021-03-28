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
INSTALL_PATH=$SCRIPTS_PATH/../../src/install


#checking for parameters
for i in "$@"
do
case $i in
    -i=*|--install_path=*)
    INSTALL_PATH="${i#*=}"
    shift # past argument=value
    ;;
    *)
          # unknown option
    ;;
esac
done

sh $SCRIPTS_PATH/resources/nsis_script.sh -i=$INSTALL_PATH

makensis.exe $SCRIPTS_PATH/resources/meshlab_final.nsi

rm $SCRIPTS_PATH/resources/meshlab_final.nsi
