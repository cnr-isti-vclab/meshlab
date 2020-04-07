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

#saving location where script has been runned
$DIR = Get-Location

$INSTALL_PATH = $PSScriptRoot

if ($args.Count -gt 0){
    $DISTRIB_PATH = $args[0]
} else {
    $DISTRIB_PATH = Join-Path $PSScriptRoot ..\..\distrib #default distrib
}

.\resources\windows_nsis_script.ps1 $DISTRIB_PATH

cd $INSTALL_PATH

makensis.exe .\resources\meshlab_final.nsi

Remove-Item .\resources\meshlab_final.nsi

#going back to original location
cd $DIR
