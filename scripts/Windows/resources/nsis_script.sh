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
ML_VERSION=$(cat $SOURCE_PATH/../ML_VERSION)
INSTALL_PATH=$SOURCE_PATH/install

#check parameters
for i in "$@"
do
case $i in
    -i=*|--install_path=*)
    INSTALL_PATH="${i#*=}"
    shift # past argument=value
    ;;
    --double_precision)
    ML_VERSION=${ML_VERSION}d
    shift # past argument=value
    ;;
    *)
          # unknown option
    ;;
esac
done

sed "s%MESHLAB_VERSION%$ML_VERSION%g" $SCRIPTS_PATH/resources/meshlab.nsi > $SCRIPTS_PATH/resources/meshlab_final.nsi
sed -i "s%DISTRIB_PATH%.%g" $SCRIPTS_PATH/resources/meshlab_final.nsi

mv $SCRIPTS_PATH/resources/meshlab_final.nsi $INSTALL_PATH/
cp $SCRIPTS_PATH/resources/ExecWaitJob.nsh $INSTALL_PATH/
cp $SCRIPTS_PATH/resources/FileAssociation.nsh $INSTALL_PATH/
