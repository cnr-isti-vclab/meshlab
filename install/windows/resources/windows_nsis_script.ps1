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

#saving location where script has been runned
$DIR = Get-Location

$INSTALL_PATH = Join-Path $PSScriptRoot ..\
$SOURCE_PATH = Join-Path $PSScriptRoot ..\..\..\src

if ($args.Count -gt 0){
    $DISTRIB_PATH = $args[0]
} else {
    $DISTRIB_PATH = Join-Path $PSScriptRoot ..\..\..\distrib #default distrib
}

cd $DISTRIB_PATH

if(! (Test-Path meshlab.exe)){ #meshlab.exe not found inside $DISTRIB_PATH
    cd $DIR
	throw 'meshlab.exe not found in ' + ($DISTRIB_PATH) + '. Exiting.'
}

$VERSION = Get-Date -Format "yyyy.MM"

cd $INSTALL_PATH

cat resources\meshlab.nsi | %{$_ -replace "MESHLAB_VERSION",$VERSION} > resources\meshlab_tmp.nsi
cat resources\meshlab_tmp.nsi | %{$_ -replace "DISTRIB_PATH",$DISTRIB_PATH} > resources\meshlab_final.nsi

Remove-Item resources\meshlab_tmp.nsi

#going back to original location
cd $DIR
