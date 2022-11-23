# This is a powershell script for computing an installer for MeshLab.
# Requires:
# - a properly deployed meshlab (see 1_deploy.sh);
# - the env variable PATH containing the NSIS folder (makensis.exe must be directly accessible)
#
# Without given arguments, the folder that will be deployed is meshlab/install.
#
# You can give as argument the INSTALL_PATH.
#
# After running this script, the installer can be found inside the install folder.

SCRIPTS_PATH="$(dirname "$(realpath "$0")")"
RESOURCES_PATH=$SCRIPTS_PATH/../../resources
INSTALL_PATH=$SCRIPTS_PATH/../../install
PACKAGE_PATH=$SCRIPTS_PATH/../../packages

#checking for parameters
for i in "$@"
do
case $i in
    -i=*|--install_path=*)
        INSTALL_PATH="${i#*=}"
        shift # past argument=value
        ;;
    -p=*|--package_path=*)
        PACKAGE_PATH="${i#*=}"
        shift # past argument=value
        ;;
    *)
        # unknown option
        ;;
esac
done

sh $SCRIPTS_PATH/internal/nsis_script.sh -i=$INSTALL_PATH

makensis.exe $INSTALL_PATH/meshlab_final.nsi

rm $INSTALL_PATH/meshlab_final.nsi
rm $INSTALL_PATH/ExecWaitJob.nsh
rm $INSTALL_PATH/FileAssociation.nsh

mkdir $PACKAGE_PATH
mv $INSTALL_PATH/MeshLab*-windows.exe  $PACKAGE_PATH
