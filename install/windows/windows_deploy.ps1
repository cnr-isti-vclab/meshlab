# This is a powershell script for deploying a meshlab-portable app.
# Requires: 
# - a properly built meshlab;
# - the env variable PATH containing the bin folder of QT (windeployqt.exe must be directly accessible)
# - the env variable VCINSTALLDIR set to the VC of Visual Studio (example: C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC)
#
# Without given arguments, the folder that will be deployed is meshlab/distrib.
#
# You can give as argument the DISTRIB_PATH.
#
# After running this script, $DISTRIB_PATH will be a portable meshlab folder.
#
# To be run in a windows environment without Visual Studio installed,
# vc_redist.exe must be installed before.

#saving location where script has been run

write-host "N of arguments: $($args.count)"

$DIR = Get-Location
$INSTALL_PATH = $PSScriptRoot
$SOURCE_PATH = Join-Path $PSScriptRoot ..\..\src

if ($args.Count -gt 0){
    $DISTRIB_PATH = $args[0]
} else {
    $DISTRIB_PATH = Join-Path $PSScriptRoot ..\..\distrib #default distrib
} 

cd $DISTRIB_PATH

if(! (Test-Path meshlab.exe)){ #meshlab.exe not found inside $DISTRIB_PATH
    cd $DIR
	throw 'meshlab.exe not found in ' + ($DISTRIB_PATH) + '. Exiting.'
}

#Copy-Item (Join-Path $INSTALL_PATH ..\meshlab.png) .

windeployqt --no-translations meshlab.exe

Move-Item .\lib\meshlab\IFX* .
Copy-Item IFXCoreStatic.lib .\lib\meshlab\

#at this point, distrib folder contains all the files necessary to execute meshlab
echo "distrib folder is now a self contained meshlab application"

#going back to original location
cd $DIR
