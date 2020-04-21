@echo off

rem
rem "Main"
rem

if not "%1"=="" (
    if not "%1"=="-library" (
        call :PrintUsage
        goto EOF
    )
)

rem only ask to accept the license text once
if exist .licenseAccepted goto HandleArgs

rem determine if free or commercial package
set edition=commercial
if exist LICENSE.LGPL. set edition=free

if %edition%==free (
    call :HandleFree
) else (
    call :RegionLoop
    call :Comm
)
echo .

if not exist .licenseAccepted (
    echo You are not licensed to use this software.
    goto EOF
)

:HandleArgs
if exist config.pri. del config.pri
if "%1"=="-library" (
    echo Configuring to build this component as a dynamic library.
    echo SOLUTIONS_LIBRARY = yes > config.pri
)

echo .
echo This component is now configured.
echo .
echo To build the component library (if requested) and example(s),
echo run qmake and your make or nmake command.
echo .
echo To remove or reconfigure, run make (nmake) distclean.
echo .

goto EOF

rem
rem "License acceptance loops"
rem

:RegionLoop
    echo .
    echo Please choose your region.
    echo .
    echo Type 1 for North or South America.
    echo Type 2 for anywhere outside North and South America.
    echo .
    set /p region=Select: 
    if %region%==1 (
	set licenseFile=LICENSE.US
	goto EOF
    )
    if %region%==2 (
	set licenseFile=LICENSE.NO
	goto EOF
    )
goto RegionLoop

:HandleFree
    echo .
    echo You are licensed to use this software under the terms of
    echo the GNU General Public License (GPL) version 3, or
    echo the GNU Lesser General Public License (LGPL) version 2.1
    echo with certain additional extra rights as specified in the
    echo Nokia Qt LGPL Exception version 1.1.
    echo .
    echo Type 'G' to view the GNU General Public License (GPL) version 3
    echo Type 'L' to view the GNU Lesser General Public License (LGPL) version 2.1
    echo Type 'E' to view the Nokia Qt LGPL Exception version 1.1.
    echo Type 'yes' to accept this license offer.
    echo Type 'no' to decline this license offer.
    echo .
    set /p answer=Do you accept the terms of this license? 

    if %answer%==no goto EOF
    if %answer%==yes (
	 echo license accepted > .licenseAccepted
	 goto EOF
    )
    if %answer%==g more LICENSE.GPL3
    if %answer%==G more LICENSE.GPL3
    if %answer%==l more LICENSE.LGPL
    if %answer%==L more LICENSE.LGPL
    if %answer%==e more LGPL_EXCEPTION.txt
    if %answer%==E more LGPL_EXCEPTION.txt
goto HandleFree

:Comm
    echo .
    echo License Agreement
    echo .
    echo Type '?' to view the Qt Solutions Commercial License.
    echo Type 'yes' to accept this license offer.
    echo Type 'no' to decline this license offer.
    echo .
    set /p answer=Do you accept the terms of this license? 

    if %answer%==no goto EOF
    if %answer%==yes (
	 echo license accepted > .licenseAccepted
	 copy %licenseFile% LICENSE
	 del LICENSE.US
	 del LICENSE.NO
	 goto EOF
    )
    if %answer%==? more %licenseFile%
goto Comm

:PrintUsage
echo Usage: configure.bat [-library]
echo .
echo -library: Build the component as a dynamic library (DLL). Default is to
echo           include the component source directly in the application.
echo           A DLL may be preferable for technical or licensing (LGPL) reasons.
echo .
goto EOF


:EOF
