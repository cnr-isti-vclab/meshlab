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
# To be runned in a windows environment without Visual Studio installed,
# vc_redist.exe must be installed before.

#saving location where script has been runned
$DIR = Get-Location

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

cd ..
New-Item -Name "tmp" -ItemType "directory"
Move-Item -Path "distrib\*" -Destination "tmp"

Copy-Item -Path (Join-Path $INSTALL_PATH ..\qt\resources\MeshLab) -Destination "distrib" -Recurse
Move-Item -Path "tmp\*" -Destination "distrib\MeshLab\packages\com.vcg.meshlab\data"
Remove-Item "tmp"

cd $INSTALL_PATH
cd ..\qt\resources
$VERSION = Get-Date -Format "yyyy.MM"
$DATE = Get-Date -Format "yyyy-MM-dd"

cat config_template.xml | %{$_ -replace "MESHLAB_VERSION",$VERSION} > config.xml
Move-Item -Path config.xml -Destination (Join-Path $DISTRIB_PATH MeshLab\config\)

cat package_template.xml | %{$_ -replace "MESHLAB_VERSION",$VERSION} > package_tmp.xml
cat package_tmp.xml | %{$_ -replace "MESHLAB_DATE",$DATE} > package.xml
Remove-Item package_tmp.xml
Move-Item -Path package.xml -Destination (Join-Path $DISTRIB_PATH MeshLab\packages\com.vcg.meshlab\meta\)

cd (Join-Path $DISTRIB_PATH MeshLab)
#####
#binarycreator.exe --offline-only -c .\config\config.xml -p packages meshlab_setup.exe
#####

#going back to original location
cd $DIR
