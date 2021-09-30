#!/bin/bash
#
# make-vcproj.sh YYMMDD -- Make sln and vcproj files from CMakeLists.txt and qhull-all.pro
#       eg/make-vcproj.sh qhull-x -- Derive src/qhull-x/ from src/libqhull_r
#
# Design
#       CMake vcproj files includes absolute paths and does not handle 'd' annotatios for debug versions
#       Remove CMake targets
#       Change absolute paths to '../..'
#       Append '_d' to debug products
#       Write targets to '../lib/' and ../bin/'
#
# $Id: //main/2019/qhull/eg/make-vcproj.sh#1 $$Change: 2661 $
# $DateTime: 2019/05/24 20:09:58 $$Author: bbarber $

if [[ "$1" != "" && "$1" != "Win64" && "$1" != "qhull-x" && "$1" != "sed-only" ]]; then
    echo "eg/make-vcproj.sh Win64|sed-only|qhull-x"
    echo "Written for 'sed 4.0.7', 'Visual Studio 11 2012 Win64', and 'Visual Studio 8 2005'"
    echo "Other variations are likely to fail"
    exit
fi
if [[ "$1" == "qhull-x" ]]; then
    set -v
    win64=1
    if [[ ! -w build/qhull.sln || ! -r build/qhull-64.vcxproj || ! -d src/libqhull_r ]]; then
        echo "Excute 'eg/make-vcproj.sh qhull-x' from qhull directory with build/qhull.sln, build/qhull-64.vcxproj, src/libqhull_r"
        exit
    fi

    SOURCEDIR=.
    SOURCE=$SOURCEDIR/src/libqhull_r
    SOURCE2=$SOURCEDIR/src/qhull
    SOURCE3=$SOURCEDIR/src/rbox
    DEST=/c/bash/local/qhull/src/qhull-x
    DEST2=$DEST
    # SOURCE=../qhull-y/src/libqhull_r
    # SOURCE2=src/qhull
    # DEST=src/qhull-x
    # DEST2=../../../qhull/$DEST # relative to SOURCE
    if [[ -d $DEST ]]; then
        echo "To rebuild $DEST from $SOURCE and $SOURCE2 -- rm -r $DEST; eg/make-vcproj.sh qhull-x"
    else
        echo Create $DEST from $SOURCE and $SOURCE2
        mkdir $DEST || exit 1
        pushd $SOURCE || exit 1
        sed -e 's/_r/_x/g' -e 's/_xbox/_rbox/g' Makefile >$DEST2/Makefile || exit 1
        for F in *_r.c; do
            G=${F%_r.c}_x.c; echo $G; cp -p $F $DEST2/$G || exit 1
        done            
        for F in *_r.h; do
            G=${F%_r.h}_x.h; echo $G; cp -p $F $DEST2/$G || exit 1
        done
        for F in *_ra.h; do
            G=${F%_ra.h}_xa.h; echo $G; cp -p $F $DEST2/$G || exit 1
        done
        popd
        pushd $SOURCE2 || exit 1
        for F in *_r.c; do
            G=${F%_r.c}_x.c; echo $G; cp -p $F $DEST2/$G || exit 1
        done            
        popd
        pushd $SOURCE3 || exit 1
        for F in *_r.c; do
            G=${F%_r.c}_x.c; echo $G; cp -p $F $DEST2/$G || exit 1
        done            
        popd

        echo "Substitute *_x for *_r"
        sed -i 's/_r\.h/_x.h/' $DEST/*
        sed -i 's/_ra\.h/_xa.h/' $DEST/*
        sed -i 's|libqhull_r/libqhull_x|libqhull_x|' $DEST/*
        sed -i 's|libqhull_r/random_x|random_x|' $DEST/*
        
        echo "Restrict Makefile to qhull-x and rbox"
        sed -i ' /^all. / s/qhull_links//' $DEST/Makefile
        sed -i ' /-f .*exe/ s/qconvex.*exe/core rbox qhull-x *.exe/' $DEST/Makefile
        sed -i ' /-p .*BINDIR/ s/qconvex.*rbox/qhull-x/' $DEST/Makefile
        sed -i ' /-o qhull/ s/qhull/qhull-x/' $DEST/Makefile
        sed -i '\|\./qhull| s/qhull/qhull-x/' $DEST/Makefile
        sed -i 's|\.\./qconvex.*|unix_x.c|' $DEST/Makefile
        sed -i ' /^qhull_all. / s/qconvex.*testqset_x.o/unix_x.o rbox_x.o/' $DEST/Makefile
        for D in qconvex qdelaunay qhalf qvoronoi user_eg testqset libqhullstatic DOCDIR INCDIR LIBDIR MANDIR; do
          sed -i " /$D/ d" $DEST/Makefile
        done
        sed -i " /ln -s / d" $DEST/Makefile
    fi

    pushd build/ || exit 1
    SOURCE=../src/qhull-x
    VCXPROJ=qhull-x-64.vcxproj
    GUID=3192557A-C34A-426E-A51B-2BCE463EAD02
    VCXPROJ2=qhull-64.vcxproj
    GUID2=E8BF0EA0-A09D-4155-BCEC-CC1B0DF8A67E
    echo Create $VCXPROJ from $VCXPROJ2
    if ! grep $GUID qhull-64.sln >/dev/null; then
        echo add GUID for $VCXPROJ to qhull-64.sln -- $GUID
    fi
    cp $VCXPROJ2 $VCXPROJ || exit 1
    grep $GUID2 $VCXPROJ2 >/dev/null || (echo unknown GUID for $VCXPROJ2 -- $GUID2; exit 1)
    sed -i s/$GUID2/$GUID/g $VCXPROJ
    sed -i 's/ProjectReference Include=.*/ProjectReference>/' $VCXPROJ
    sed -i 's/<Project>.*//' $VCXPROJ
    # need to delete 3 lines
    sed -i -r 's/;...lib.qhullstatic_rd?.lib//' $VCXPROJ
    sed -i 's/qhull/qhull-x/g' $VCXPROJ
    # Since qhull pulls object files from libqhullstatic_r.lib, order unknown, may be in specified order
    for F in $SOURCE/*_x.h; do
        sed -i " /ClCompile.*_r.c/ i <ClInclude Include=\"$F\" />"  $VCXPROJ
    done            
    for F in $SOURCE/*_x.c; do
        sed -i " /ClCompile.*_r.c/ i <ClCompile Include=\"$F\" />"  $VCXPROJ
    done            
    sed -i ' /_r.c/ d' $VCXPROJ
    popd
    exit
elif [[ "$1" == "Win64" ]]; then
    if [[ "$2" != "" && "$2" != "sed-only" ]]; then
        echo "eg/make-vcproj.sh [Win64] [sed-only]"
        exit
    fi
    win64=1
else
    win64=0
fi

if [[ "$1" == "sed-only" || "$2" == "sed-only" ]]; then
    echo Skip creating buildvc/ and buildqt/
else # else execute to 'fi ... sed-only'

if [[ ! -f CMakeLists.txt || ! -f src/qhull-all.pro ]]; then
    echo "Excute eg/make-vcproj.sh from qhull directory with CMakeLists.txt and src/qhull-all.pro"
    exit
fi

echo "Set up build directories..."
if [[ ! -d build-prev ]]; then
    echo "Backup previous build"
    cp -r build build-prev && rm -rf build
fi
rm -rf buildvc buildqt
mkdir -p build
mkdir -p build/qhulltest
mkdir -p buildvc
mkdir -p buildqt
echo "Create vcproj files with cmake and qmake..."
if [[ $win64 -eq 1 ]]; then
    echo Running -- cmake -G "Visual Studio 11 2012 Win64" 
    cd buildvc && cmake -G "Visual Studio 11 2012 Win64" .. && cmake ..
else
    echo Running -- cmake -G "Visual Studio 8 2005"
    cd buildvc && cmake -G "Visual Studio 8 2005" .. && cmake ..
fi
cd ..
cd buildqt && qmake -tp vc -r ../src/qhull-all.pro
cd ..
if [[ ! -f CMakeLists.txt ]]; then
    echo "Missing CMakeLists.txt.  Cannot run cmake"
    exit
elif [[ (! -f buildvc/qhull.vcproj && ! -f buildvc/qhull.vcxproj) || (! -f buildqt/qhulltest/qhulltest.vcproj && ! -f buildqt/qhulltest/qhulltest.vcxproj) ]]; then
    echo "qmake and cmake did not build vcproj or vcxproj files in buildvc/ and buildqt/"
    exit
fi

echo
echo 'Create build/*.vcproj by converting absolute paths to relative paths'
echo Removes INSTALL/ALL_BUILD/ZERO_CHECK/RUN_TESTS projects
echo Turn off LinkIncremental and RuntimeTypeInfo
echo Add 'd' flags to debug lib/dll files...
echo 'Delete  <file> CMake...</file> and other instances of CMake'
echo 'WARN: These sed replacements depend on how CMake creates vcproj or vcxproj files.  It is likely to change.'

fi # Skipped if 'sed-only'

for f in buildvc/*.vc*proj buildqt/qhulltest/*.vc*proj; do 
    echo -n $f
    if [[ ! ${f##*INSTALL*} || ! ${f##*ALL_BUILD*} || ! ${f##*ZERO_CHECK*} || ! ${f##*RUN_TESTS*}  ]]; then
        echo " removed"
        continue
    fi
    if [[ $win64 -eq 1 ]]; then
        ext=${f##*.}
        base=${f#*\/}
        base=${base%.*}
        dest=build/$base-64.$ext
        echo " => $dest"
        # sed requires a blank or \ before address ranges
        # sed requires a \/ for s|...|...| !
        # the ConfigurationType clauses depend on Debug being first
        sed -r \
            -e ' /CustomBuild.*CMake/,/CustomBuild/ d' \
            -e ' /ZERO_CHECK/,/ProjectReference/ d' \
            -e ' />qhulltest<S/,/Project>/ s|[cC]:\\bash\\local\\qhull|..\\..|g' \
            -e ' />qhulltest</,/Project>/ s|[cC]:\/bash\/local\/qhull|..\/..|g' \
            -e 's|[cC]:\\bash\\local\\qhull|..|g' \
            -e 's|[cC]:\/bash\/local\/qhull|..|g' \
            -e '\|CMake| d' \
            -e '\|VCWebServiceProxyGeneratorTool| d' \
            -e 's/;CMAKE_INTDIR=[^;]*;/;/' \
            -e 's/;[a-zA-Z]*\\([_a-z0-9]*\.lib[;<])/;..\\lib\\\1/g' \
            -e ' /LinkIncremental/ s/true/false/' \
            -e 's/buildvc/build/g' \
            -e 's/buildqt/build/g' \
            -e 's/c:\\qt\\[0-9]\.[0-9]\.[0-9]/\$(QTDIR)/g' \
            -e 's|..\/build\/[a-zA-Z]*[\\/]([_a-z0-9]*.pdb)|..\/bin\/\1|g' \
            -e 's|..\/build\/[a-zA-Z]*[\\/]([_a-z0-9]*.exe)|..\/bin\/\1|g' \
            -e 's|..\/build\/[a-zA-Z]*[\\/]([_a-z0-9]*.lib)|..\/lib\/\1|g' \
            -e 's|..\/build\/[a-zA-Z]*[\\/]([_a-z0-9]*.dll)|..\/bin\/\1|g' \
            -e 's| [a-zA-Z]*[\\/]([_a-z0-9]*\.lib)| ..\\lib\\\1|g' \
            -e 's/"([_a-z0-9]*.exe)/"..\\bin\\\1/g' \
            -e ' /ConfigurationType>Application/,/ClInclude/ s/(OutDir.*\.\.)[\\a-zA-Z]*</\1\\bin\\</' \
            -e ' /ConfigurationType>DynamicLibrary/,/ClInclude/ s/(OutDir.*\.\.)[\\a-zA-Z]*</\1\\bin\\</' \
            -e ' /ConfigurationType>DynamicLibrary/,/ImportLibrary/ s/(ImportLibrary.*_[rp])\.lib</\1d.lib</' \
            -e ' /ConfigurationType>DynamicLibrary/,/ImportLibrary/ s/(ImportLibrary.*[^d])\.lib</\1_d.lib</' \
            -e ' /ConfigurationType>DynamicLibrary/,/TargetName.*Debug/ s/(TargetName.*_[pr])</\1d</' \
            -e ' /ConfigurationType>DynamicLibrary/,/TargetName.*Debug/ s/(TargetName.*Debug.*[^d])</\1_d</' \
            -e ' /ConfigurationType>DynamicLibrary/,/ProgramDataBaseFile/ s/(ProgramDataBaseFile.*_[pr])\.pdb/\1d.pdb/' \
            -e ' /ConfigurationType>DynamicLibrary/,/ProgramDataBaseFile/ s/(ProgramDataBaseFile.*[^d])\.pdb/\1_d.pdb/' \
            -e ' /ConfigurationType>StaticLibrary/,/ClInclude/ s/(OutDir.*\.\.)[\\a-zA-Z]*</\1\\lib\\</' \
            -e ' /ConfigurationType>StaticLibrary/,/ImportLibrary/ s/(ImportLibrary.*_[rp])\.lib</\1d.lib</' \
            -e ' /ConfigurationType>StaticLibrary/,/ImportLibrary/ s/(ImportLibrary.*[^d])\.lib</\1_d.lib</' \
            -e ' /ConfigurationType>StaticLibrary/,/TargetName.*Debug/ s/(TargetName.*_[pr])</\1d</' \
            -e ' /ConfigurationType>StaticLibrary/,/TargetName.*Debug/ s/(TargetName.*Debug.*[^d])</\1_d</' \
            -e ' /ConfigurationType>StaticLibrary/,/ProgramDataBaseFile/ s/(ProgramDataBaseFile.*_[pr])\.pdb/\1d.pdb/' \
            -e ' /ConfigurationType>StaticLibrary/,/ProgramDataBaseFile/ s/(ProgramDataBaseFile.*[^d])\.pdb/\1_d.pdb/' \
            -e ' /ItemDefinitionGroup.*Debug/,/AdditionalDependencies/ s/(AdditionalDependencies.*_r)\.lib/\1d.lib/g' \
            -e ' /ItemDefinitionGroup.*Debug/,/AdditionalDependencies/ s/(AdditionalDependencies.*qhull[a-z]*[^d])\.lib/\1_d.lib/g' \
            -e ' /AdditionalDependencies/ s/;[a-zA-Z]*\\/;..\\lib\\/g' \
            -e 's/[cC]:\\[qQ]t\\[qQt0-9.\\]*msvc[_0-9]*/\$(QTDIR)/g' \
            -e 's/[cC]:\\[qQ]t\\[qQt0-9.\\]*/\$(QTDIR)\\..\\/g' \
            -e 's/([|>])Win32/\1x64/' \
            -e 's/machine:X86/machine:x64/' \
            -e 's/\.vcxproj/-64.vcxproj/' \
            $f | awk '/<File$/ && !SkipFirstFile,/<\/File>/{ next } /<Filter$/{SkipFirstFile=1} {print $0}' > $dest
    else   
        dest=build/${f##*\/}
        # sed requires a blank or \ before address ranges
        sed -r -e 's|[cC]:\\bash\\local\\qhull|..|g' \
            -e 's|[cC]:/bash/local/qhull|..|g' \
            -e '\|CMake| d' \
            -e '\|VCWebServiceProxyGeneratorTool| d' \
            -e 's/;CMAKE_INTDIR=..quot;[A-Za-z]*..quot;//' \
            -e 's/LinkIncremental="2"/LinkIncremental="1"/' \
            -e 's/RuntimeLibrary[=]/RuntimeTypeInfo="false"  RuntimeLibrary=/' \
            -e 's/.*RuntimeTypeInfo."TRUE".*//' \
            -e 's/buildvc/build/g' \
            -e 's/buildqt/build/g' \
            -e 's/\.\.\\\.\./../g' \
            -e 's|\.\./\.\.|..|g' \
            -e 's/c:\\qt\\[0-9]\.[0-9]\.[0-9]/\$(QTDIR)/g' \
            -e 's|..\\build\\[a-zA-Z]*[\\/]([_a-z0-9]*.pdb)|..\\bin\\\1|g' \
            -e 's|..\\build\\[a-zA-Z]*[\\/]([_a-z0-9]*.exe)|..\\bin\\\1|g' \
            -e 's|..\\build\\[a-zA-Z]*[\\/]([_a-z0-9]*.lib)|..\\lib\\\1|g' \
            -e 's|..\\build\\[a-zA-Z]*[\\/]([_a-z0-9]*.dll)|..\\bin\\\1|g' \
            -e 's| [a-zA-Z]*[\\/]([_a-z0-9]*\.lib)| ..\\lib\\\1|g' \
            -e 's/"([_a-z0-9]*.exe)/"..\\bin\\\1/g' \
            -e ' /Name="Debug/,/OutputFile/ s/(OutputFile.*_[rp])\.(dll|lib)"/\1d.\2"/' \
            -e ' /Name="Debug/,/OutputFile/ s/(OutputFile.*[^d])\.(dll|lib)"/\1_d.\2"/' \
            -e ' /Name="Debug/,/ImportLibrary/ s/(ImportLibrary.*_[rp])\.lib"/\1d.lib"/' \
            -e ' /Name="Debug/,/ImportLibrary/ s/(ImportLibrary.*[^d])\.lib"/\1_d.lib"/' \
            -e ' /Name="Debug/,/AdditionalDependencies/ s/(AdditionalDependencies.*_[rp])\.lib/\1d.lib/' \
            -e ' /Name="Debug/,/AdditionalDependencies/ s/(AdditionalDependencies.*qhull[a-z]*[^d])\.lib/\1_d.lib/' \
            -e ' /Name="Debug/,/ProgramDatabaseFile/ s/(ProgramDatabaseFile.*_r)\.pdb/\1d.pdb/' \
            -e ' /Name="Debug/,/ProgramDatabaseFile/ s/(ProgramDatabaseFile.*qhull[a-z]*[^d])\.pdb/\1_d.pdb/' \
            -e 's/[cC]:\\Qt\\[0-9.]*/\$(QTDIR)/g' \
            -e 's/\.vcxproj/-32.vcxproj/' \
            $f | awk '/<File$/ && !SkipFirstFile,/<\/File>/{ next } /<Filter$/{SkipFirstFile=1} {print $0}' > $dest
    fi
    # grep -E '\.\.[\\/]|_[a-z]?d[^a-z]' $dest
done

echo
echo Create build/qhull.sln or build/qhull-64.sln from buildvc/qhull.sln
if [[ $win64 -eq 1 ]]; then
    echo Create qhull-64.sln
    sed -r \
        -e '\|Project.*ALL_BUILD|,\|EndProject$| d' \
        -e '\|Project.*INSTALL|,\|EndProject$| d' \
        -e '\|Project.*ZERO_CHECK|,\|EndProject$| d' \
        -e '\|Project.*RUN_TESTS|,\|EndProject$| d' \
        -e '\|ProjectDependencies|,\| = | s/^.* = \{.*$//' \
        -e 's/\.vcxproj/-64.vcxproj/' \
        buildvc/qhull.sln >build/qhull-64.sln

    echo Create qhull-32.sln from qhull-64 with 32 GUIDs
    # Need to keep the original GUIDs.  Otherwise get -- can't locate project ''
    sed -r \
        -e 's/-64\.vcxproj/-32.vcxproj/' \
        -e 's/([|>])x64/\1Win32/g' \
        build/qhull-64.sln >build/qhull-32.sln

    echo Create *-32.vcxproj with 32 GUIDs
    for f in build/*-64.vcxproj build/qhulltest/*-64.vcxproj; do
        dest=${f%64.vcxproj}32.vcxproj
        sed -r \
            -e 's/([|>])x64/\1Win32/' \
            -e 's/machine:x64/machine:X86/' \
             $f >$dest
    done

    echo Convert vcxproj/sln files to DOS format
    for f in build/*.vcxproj build/*-32.sln build/*-64.sln; do
        u2d $f
    done
else
    sed -e '\|Project.*ALL_BUILD|,\|EndProject$| d' \
        -e '\|Project.*INSTALL|,\|EndProject$| d' \
        -e '\|Project.*ZERO_CHECK|,\|EndProject$| d' \
        -e '\|Project.*RUN_TESTS|,\|EndProject$| d' \
        buildvc/qhull.sln >build/qhull.sln
    echo Convert vcproj/sln files to DOS format
    for f in build/*.vcproj build/*.sln; do
        u2d $f
    done
fi

echo
echo 'Add qhulltest.vcproj to qhull.sln'
echo '  Open qhull.sln with DevStudio'
echo '  Add build/qhulltest/qhulltest.vcproj or qhulltest-64.vcxproj'
echo '  Add dependencies on qhullcpp (user_eg3, qhulltest)'
echo '  Add dependencies on qhullstatic (rbox, qconvex, etc)'
echo '  Add dependencies on qhullstatic_r (qhull, user_eg2)'
echo '  Add dependencies on libqhull_r (user_eg) and qhullstatic_r'
echo '  Remove qhulltest via Configuration Manager from Release,Debug,etc. Qt is optional'
echo
echo 'Update Perforce for build/...vcproj, otherwise qhull-zip.sh will revert the files'
