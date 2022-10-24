$DIR = Get-Location

#default paths wrt the script folder
$SCRIPTS_PATH = $PSScriptRoot
$EXTERNAL_PATH = Join-Path $SCRIPTS_PATH ..\..\src\external

cd $EXTERNAL_PATH

# boost
Invoke-WebRequest -Uri "https://boostorg.jfrog.io/artifactory/main/release/1.75.0/source/boost_1_75_0.zip" -OutFile "boost_1_75_0.zip"
Expand-Archive -Path boost_1_75_0.zip -DestinationPath .\
Remove-Item boost_1_75_0.zip

# cgal
Invoke-WebRequest -Uri "https://github.com/CGAL/cgal/releases/download/v5.2.1/CGAL-5.2.1.zip" -OutFile "CGAL-5.2.1.zip"
Expand-Archive -Path CGAL-5.2.1.zip -DestinationPath .\
Remove-Item CGAL-5.2.1.zip

# gmp and mpfr
Invoke-WebRequest -Uri "https://github.com/CGAL/cgal/releases/download/v5.2.1/CGAL-5.2.1-win64-auxiliary-libraries-gmp-mpfr.zip" -OutFile "CGAL-5.2.1-win64-auxiliary-libraries-gmp-mpfr.zip"
Expand-Archive -Path CGAL-5.2.1-win64-auxiliary-libraries-gmp-mpfr.zip -DestinationPath .\CGAL-5.2.1\ -Force
Remove-Item CGAL-5.2.1-win64-auxiliary-libraries-gmp-mpfr.zip

# embree
Invoke-WebRequest -Uri "https://github.com/embree/embree/releases/download/v3.13.5/embree-3.13.5.x64.vc14.windows.zip" -OutFile "embree-3.13.5.zip"
Expand-Archive -Path embree-3.13.5.zip -DestinationPath .\
Remove-Item embree-3.13.5.zip

#going back to original location
cd $DIR
