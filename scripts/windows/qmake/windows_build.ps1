# This is a powershell script compiling meshlab in a Windows environment.
# Requires: 
# - MSVC installed;
# - Qt installed, and qmake and jom binaries directly accessible
#
# # Without given arguments, MeshLab will be built in the meshlab/src
# directory, and binaries will be placed in meshlab/distrib.
# 
# You can give as argument the BUILD_PATH, and meshlab binaries will be
# then placed inside BUILD_PATH/distrib.

#saving location where script has been run

write-host "N of arguments: $($args.count)"

$DIR = Get-Location
$SOURCE_PATH = Join-Path $PSScriptRoot ..\..\..\src

if ($args.Count -gt 0){
    $BUILD_PATH = Resolve-Path -Path $args[0]
} else {
    $BUILD_PATH = $SOURCE_PATH #default build
} 

New-Item -ItemType Directory -Force -Path $BUILD_PATH
cd $BUILD_PATH

write-host "Build path is: $($BUILD_PATH)"

qmake $SOURCE_PATH\meshlab.pro
jom -j4 #Qt nmake for parallel build

#going back to original location
cd $DIR
