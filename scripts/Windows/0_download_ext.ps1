$DIR = Get-Location

#default paths wrt the script folder
$SCRIPTS_PATH = $PSScriptRoot
$EXTERNAL_PATH = Join-Path $SCRIPTS_PATH ..\..\src\external

cd $EXTERNAL_PATH

#going back to original location
cd $DIR
