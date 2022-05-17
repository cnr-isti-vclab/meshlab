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
RESOURCES_PATH=$SCRIPTS_PATH/../../resources
SOURCE_PATH=$SCRIPTS_PATH/../../src
INSTALL_PATH=$SOURCE_PATH/install

#check parameters
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

#get version
IFS=' ' #space delimiter
STR_VERSION=$($INSTALL_PATH/meshlab.exe --version)
read -a strarr <<< "$STR_VERSION"
ML_VERSION=${strarr[1]} #get the meshlab version from the string

sed "s%MESHLAB_VERSION%$ML_VERSION%g" $RESOURCES_PATH/windows/meshlab.nsi > $RESOURCES_PATH/windows/meshlab_final.nsi
sed -i "s%DISTRIB_PATH%.%g" $RESOURCES_PATH/windows/meshlab_final.nsi

mv $RESOURCES_PATH/windows/meshlab_final.nsi $INSTALL_PATH/
cp $RESOURCES_PATH/windows/ExecWaitJob.nsh $INSTALL_PATH/
cp $RESOURCES_PATH/windows/FileAssociation.nsh $INSTALL_PATH/
