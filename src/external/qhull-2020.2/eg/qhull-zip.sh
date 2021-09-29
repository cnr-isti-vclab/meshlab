#!/bin/sh
#
# qhull-zip.sh version -- Make zip and tgz files for Qhull release
#
# requires road-script.sh from http://www.qhull.org/road/
#   for check_err_log, log_step, log_note, exit_err, exit_if_fail, etc.
# 
# wzzip from http://www.winzip.com/wzcline.htm
#   can not use path with $zip_file 
#   odd error messages if can't locate directory
#
# $Id: //main/2019/qhull/eg/qhull-zip.sh#39 $$Change: 3039 $
# $DateTime: 2020/09/03 21:26:22 $$Author: bbarber $

if [[ $# -ne 3 ]]; then
        echo 'Missing date stamp -- eg/qhull-zip.sh 2020 2020.2 8.0.2' 
        echo 'Build Release Win32 binaries -- build/... from Perforce, make cleanall, retarget vcxproj files, restart DevStudio'
        echo 'Check libqhull_r and libqhull are the same.  See qh_code.htm#convert'
        exit
fi
versionyear=$1
version=$2
versionunix=$3

echo $TMP

err_program=qhull-zip
err_log=$TMP/qhull-zip.log
[[ -e $HOME/bash/etc/road-script.sh ]] && source $HOME/bash/etc/road-script.sh \
    || source /etc/road-script.sh

check_err_log $LINENO "$err_log"
check_err_log $LINENO "$err_step_log"
log_step $LINENO "Logging to $err_log\n... and $err_step_log"

log_note $LINENO "Find Qhull directory" 
if [[ ! -d qhull/eg && ! -d ../qhull/eg && -d ../../qhull/eg ]]; then
    exit_err $LINENO "qhull/eg directory not found at or above $PWD"
fi
if [[ ! -d qhull/eg ]]; then
    if [[ -d ../qhull/eg ]]; then
        cd ..
    else
        cd ../..  # Tested above
    fi
fi
root_dir=$(pwd)
echo root_dir= $root_dir

TEMP_DIR="$TMP/qhull-zip-$(ro_today2)"
TEMP_FILE="$TMP/qhull-zip-$(ro_today2).txt"

qhull_zip_file=qhull-$version.zip # no path or spaces
qhull_tgz_file=qhull-$versionyear-src-$versionunix.tgz
qhullmd5_file=qhull-$version.md5sum

exit_if_fail $LINENO "rm -f $qhull_zip_file $qhull_tgz_file $qhullmd5_file"

if [[ -e /bin/msysinfo || -e /bin/msys-z.dll ]]; then

    #############################
    log_step $LINENO "Check for DevStudio 32-bit release build without q_test2"
    #############################

    ls -l qhull/bin/*.exe qhull/bin/*.dll
    qhull/bin/rbox 4 D2 | qhull/bin/qhull Tsz | grep "size in bytes"
    qhull/bin/rbox 4 D2 | qhull/bin/qhull Tsz | grep "size in bytes" | grep "ridge 20"
    exit_if_err $LINENO "Not a 32-bit build (expecting 'ridge 20')"
   
    if grep eg/q_test2 qhull/Makefile; then
       exit_err $LINENO "Change eg/q_test2 to eg/q_test in Makefile.testall"
    fi
    
    log_step $LINENO "Check 'p4 diff se' for modified files"
    if (p4 diff -se qhull/... | grep -v vcxp | grep qhull); then
       exit_err $LINENO "Submit modified files to Perforce"
    fi
    
    if [[ -d src/qhull_qh ]]; then
       exit_err $LINENO "Delete src/qhull_qh/"
    fi
fi

#############################
log_step $LINENO "Check files"
#############################

exit_if_fail $LINENO "cd qhull"
if ! grep "if 0 .* QHULL_CRTDBG" src/libqhull/user.h >/dev/null; then
    exit_err $LINENO "QHULL_CRTDBG is defined in src/libqhull/user.h"
elif ! grep "if 0 .* QHULL_CRTDBG" src/libqhull_r/user_r.h >/dev/null; then
    exit_err $LINENO "QHULL_CRTDBG is defined in src/libqhull_r/user_r.h"
elif ! grep "^#define qh_QHpointer 0" src/libqhull/user.h >/dev/null; then
    exit_err $LINENO "qh_QHpointer is defined in src/libqhull/user.h"
fi

if (od -a Makefile src/libqhull/Makefile src/libqhull_r/Makefile build/*.pc.in html/*.man eg/*.sh eg/q_eg eg/q_egtest eg/q_test | grep cr >/dev/null); then
    for f in Makefile src/*/Makefile build/*.pc.in html/*.man eg/*.sh eg/q_eg eg/q_egtest eg/q_test; do echo $f; od -a $f | grep cr | head -1; done
    exit_err $LINENO "A UNIX file contains DOS line endings.  Use d2u and binary type"
fi

NOT_OK=$(grep -h "^ " Makefile src/libqhull/Makefile src/libqhull_r/Makefile | grep -vE '^[ \t]+[$()a-zA-Z_/]+\.[oh]|bin/testqset_r qtest')
if [[ -n "$NOT_OK" ]]; then
    exit_err $LINENO "A Makefile contains a leading space instead of tabs:\n$NOT_OK"
fi

TXTFILES=$(find *.diz *.htm *.txt html/ src/ working/qhull-news.html -type f -name '*.c' -o -name '*.cpp' -o -name '*.def' -o -name '*.diz' -o -name '*.h' -o -name '*.htm*' -o -name '*.txt' -o -name '*.pri' -o -name '*.pro' -o -name '*.txt' -o -name '*.txt' -o -name '*.xml')
NOT_OK=$(for f in $TXTFILES; do if (od -a $f | grep '[^r0-9 ] *nl' >/dev/null); then echo $f; fi; done)
if [[ -n "$NOT_OK" ]]; then
    exit_err $LINENO "Text files with Unix line endings:\n$NOT_OK\nu2d *.diz *.htm *.txt html/*.txt html/*.htm html/*.xml src/*.pr* src/*.txt src/*/*.c;\nu2d src/*/*.cpp src/*/*.def src/*/*.h src/*/*.htm src/*/*.pro src/*/*.txt working/qhull-news.html"
fi
exit_if_fail $LINENO "cd .."


#############################
log_step $LINENO "Check environment"
#############################

[[ $(type -p md5sum) ]] || exit_err $LINENO "md5sum is missing"
[[ $(cp --help || grep '[-]-parents') ]] ||  exit_err $LINENO "cp does not have --parents option"

#############################
log_step $LINENO "Define functions"
#############################

function check_zip_file #zip_file
{
    local zip_file=$1
    local HERE=$(ro_here)
    log_note $HERE "Check $zip_file"
    ls -l $zip_file >>$err_log
    exit_if_err $HERE "Did not create $zip_file"
    wzunzip -ybc -t $zip_file | grep -E -v -e '( OK|Zip)' >>$err_log
    exit_if_err $HERE "Error while checking $zip_file"
}

function check_tgz_file #tgz_file
{
    local tgz_file=$1
    local HERE=$(ro_here)
    log_note $HERE "Check $tgz_file"
    ls -l $tgz_file >>$err_log
    exit_if_err $HERE "Did not create $tgz_file"
    tar -tzf $tgz_file >/dev/null 2>>$err_log
    exit_if_err $HERE "Can not extract -- tar -tzf $tgz_file"
}

function convert_to_unix #dir $qhull_2ufiles -- convert files to Unix, preserving modtime from $root_dir
{
    local temp_dir=$1
    local HERE=$(ro_here)
    log_note $HERE "Convert files to unix format in $1"
    for f in $(find $temp_dir -type f | grep -E '^([^.]*|.*\.(ac|am|bashrc|c|cfg|cpp|css|d|dpatch|h|htm|html|man|pl|pri|pro|profile|sh|sql|termcap|txt|xml|xsd|xsl))$'); do
        exit_if_fail $HERE "d2u '$f' && touch -r '$root_dir/${f#$temp_dir/}' '$f'"
    done
    for f in $qhull_2ufiles; do
        exit_if_fail $HERE "d2u '$temp_dir/$f' && touch -r '$root_dir/$f' '$temp_dir/$f'"
    done
}

function create_md5sum #md5_file -- create md5sum of current directory
{
    local md5_file=$1
    local HERE=$(ro_here)
    log_step $HERE "Compute $md5_file"
    exit_if_fail $HERE "rm -f $md5_file"
    find . -type f | sed 's|^\./||' | LC_COLLATE=C sort | xargs md5sum >>$md5_file
    exit_if_err $HERE "md5sum failed"
    log_note $HERE "$(md5sum $md5_file)"
}

function findf #name [path]
{
    find . -type f -name "*$2*" -not -path "*.git*" | egrep -i "${1:-.}"
}

#############################
log_step $LINENO "Configure $0 for $(pwd)/qhull"
#############################

md5_zip_file=qhull-$version-zip.md5sum
md5_tgz_file=qhull-$versionyear-src-$versionunix-tgz.md5sum

# recursive 
qhull_dirs="qhull/build/CMakeModules qhull/eg qhull/html qhull/src"
qhull_files="qhull/build/*.sln qhull/build/*.vcproj qhull/build/qhulltest/*.vcproj \
    qhull/build/*.vcxproj qhull/build/config.cmake.in qhull/build/qhulltest/*.vcxproj \
    qhull/Announce.txt qhull/CMakeLists.txt qhull/COPYING.txt qhull/File_id.diz \
    qhull/build/qhull.pc.in qhull/build/README-build.txt qhull/QHULL-GO.lnk \
    qhull/README.txt qhull/REGISTER.txt qhull/index.htm qhull/Makefile  \
    qhull/bin/qconvex.exe qhull/bin/qdelaunay.exe qhull/bin/qhalf.exe \
    qhull/bin/qhull.exe qhull/bin/*qhull_r.dll qhull/bin/qvoronoi.exe \
    qhull/bin/rbox.exe qhull/bin/user_eg.exe qhull/bin/user_eg2.exe \
    qhull/bin/testqset_r.exe \
    qhull/bin/user_eg3.exe qhull/bin/testqset.exe qhull/bin/msvcr80.dll"
qhull_ufiles="$qhull_dirs qhull/build/*.sln qhull/build/*.vcproj \
    qhull/build/*.vcxproj qhull/build/config.cmake.in \
    qhull/Announce.txt qhull/CMakeLists.txt qhull/COPYING.txt \
    qhull/File_id.diz qhull/build/qhull.pc.in qhull/QHULL-GO.lnk qhull/README.txt \
    qhull/REGISTER.txt qhull/index.htm qhull/Makefile"
qhull_d2ufiles="Makefile src/libqhull/Makefile src/libqhull_r/Makefile \
    qhull/build/config.cmake.in \
    src/*/DEPRECATED.txt src/*/*.pro src/*/*.htm html/*.htm html/*.txt \
    src/libqhull/MBorland eg/q_eg eg/q_egtest eg/q_test "
    

if [[ -e /bin/msysinfo || -e /bin/msys-z.dll ]]; then

    #############################
    log_step $LINENO "Check qhull_files for zip directory"
    #############################

    ls -l $qhull_files $qhull_dirs >>$err_log 
    exit_if_err $LINENO "Missing files for zip directory.  Release build only"
fi

#############################
log_step $LINENO "Clean distribution directories"
#############################

rm -r qhull/build/*
p4 sync -f qhull/build/...
exit_if_err $LINENO "Can not 'p4 sync -f qhull.sln *.vcproj'"
rm qhull/build/user_egp.vcproj qhull/build/qhullp.vcproj
cd qhull && make clean
exit_if_err $LINENO "Can not 'make clean'"
cd ..
# Includes many files from 'cleanall' (Makefile)
rm -f qhull/src/qhull-all.pro.user* qhull/src/libqhull/BCC32tmp.cfg
rm -f qhull/eg/eg.* qhull/eg/qhull-benchmark.log qhull/eg/qhull-benchmark-show.log
rm -f qhull/bin/qhulltest.exe qhull/bin/qhulltest qhull/bin/qhullp.exe
rm -f qhull/bin/user_egp.exe qhull/bin/libqhull_*.dll
rm -f qhull/src/libqhull/*.exe qhull/src/libqhull/*.a qhull/src/libqhull/*.dll
rm -f qhull/src/libqhull_r/*.exe qhull/src/libqhull_r/*.a qhull/src/libqhull_r/*.dll
rm -f qhull/src/libqhull/qconvex.c qhull/src/libqhull/unix.c 
rm -f qhull/src/libqhull/qdelaun.c qhull/src/libqhull/qhalf.c
rm -f qhull/src/libqhull/qvoronoi.c qhull/src/libqhull/rbox.c
rm -f qhull/src/libqhull/user_eg.c qhull/src/libqhull/user_eg2.c  
rm -f qhull/src/libqhull/testqset.c 
rm -f qhull/src/libqhull_r/qconvex_r.c qhull/src/libqhull_r/unix_r.c 
rm -f qhull/src/libqhull_r/qdelaun_r.c qhull/src/libqhull_r/qhalf_r.c
rm -f qhull/src/libqhull_r/qvoronoi_r.c qhull/src/libqhull_r/rbox_r.c
rm -f qhull/src/libqhull_r/user_eg_r.c qhull/src/libqhull_r/user_eg2_r.c  
rm -f qhull/src/libqhull_r/testqset_r.c 
find qhull/ -type f -name x -o -name 'x.*' -o -name '*.x' | xargs -r rm
set noglob

if [[ (-e /bin/msysinfo || -e /bin/msys-z.dll) && $(type -p wzzip) && $(type -p wzunzip) ]]; then

    #############################
    log_step $LINENO "Build zip directory as $TEMP_DIR/qhull"
    #############################

    ls -l $qhull_files $qhull_dirs >>$err_log 
    exit_if_err $LINENO "Missing files for zip directory.  Release build only"

    log_note $LINENO "Copy \$qhull_files \$qhull_dirs to $TEMP_DIR/qhull"
    exit_if_fail $LINENO "rm -rf $TEMP_DIR && mkdir $TEMP_DIR"
    exit_if_fail $LINENO "cp -r -p --parents $qhull_files $qhull_dirs $TEMP_DIR"

    #############################
    log_step $LINENO "Write md5sum to $md5_tgz_file"
    #############################

    exit_if_fail $LINENO "pushd $TEMP_DIR/qhull"
    create_md5sum $md5_zip_file
    exit_if_fail $LINENO "cp -p $md5_zip_file $root_dir"
    
    #############################
    log_step $LINENO "Write $qhull_zip_file"
    #############################

    log_note $LINENO "Write \$qhull_files to $qhull_zip_file"
    exit_if_fail $LINENO "cd .. && mv qhull qhull-$version && md5sum qhull-$version/$md5_zip_file >>$root_dir/$qhullmd5_file"
    wzzip -P -r -u $qhull_zip_file qhull-$version >>$err_log
    exit_if_err $LINENO "wzzip does not exist or error while zipping files"
    check_zip_file $qhull_zip_file
    exit_if_fail $LINENO "popd"
    exit_if_fail $LINENO "mv $TEMP_DIR/$qhull_zip_file ."
fi

#############################
log_step $LINENO "Build tgz directory as $TEMP_DIR/qhull"
#############################

log_note $LINENO "Archive these files as $qhull_tgz_file"
ls -l $qhull_ufiles >>$err_log 
exit_if_err $LINENO "Missing files for tgz"

exit_if_fail $LINENO "rm -rf $TEMP_DIR && mkdir -p $TEMP_DIR"
exit_if_fail $LINENO "cp -r -p --parents $qhull_ufiles $TEMP_DIR"

if [[ $IS_WINDOWS && $(type -p d2u) ]]; then
    log_step $LINENO "Convert to Unix line endings"
    convert_to_unix "$TEMP_DIR"
fi

#############################
log_step $LINENO "Write md5sum to $md5_tgz_file"
#############################

exit_if_fail $LINENO "pushd $TEMP_DIR && cd qhull"
create_md5sum $md5_tgz_file
exit_if_fail $LINENO "cp -p $md5_tgz_file $root_dir"

exit_if_fail $LINENO "cd .. && mv qhull qhull-$version && md5sum qhull-$version/$md5_tgz_file >>$root_dir/$qhullmd5_file"

#############################
log_step $LINENO "Write $qhull_tgz_file"
#############################

exit_if_fail $LINENO "tar -zcf $root_dir/$qhull_tgz_file * && popd"
check_tgz_file $qhull_tgz_file

log_note $LINENO "md5sum of zip and tgz files"

for f in $qhull_zip_file $qhull_tgz_file; do
    if [[ -r $f ]]; then
        exit_if_fail $LINENO "md5sum $f >>$qhullmd5_file"
    fi
done

#############################
log_step $LINENO "Extract zip and tgz files to $TEMP_DIR"
#############################

exit_if_fail $LINENO "rm -rf $TEMP_DIR"
if [[ -r $root_dir/$qhull_zip_file ]]; then
    exit_if_fail $LINENO "mkdir -p $TEMP_DIR/zip && cd $TEMP_DIR/zip"
    log_step $LINENO "Current directory is /c/Git/$TEMP_DIR/zip"
    exit_if_fail $LINENO "wzunzip -yb -d $root_dir/$qhull_zip_file"
    log_step $LINENO "Search for date stamps into zip/Dates.txt"
    find . -type f | grep -vE '/bin/|q_benchmark|q_test' | xargs grep '\-20' | grep -v -E '(page=|ISBN|sql-2005|utility-2000|written 2002-2003|tail -n -20|Spinellis|WEBSIDESTORY|D:06-5-2007|server-2005)' >Dates.txt
    find . -type f | grep -vE '/bin/|q_benchmark|q_test' | xargs grep -i 'qhull *20' >>Dates.txt
    find . -type f | grep -vE '/bin/|q_benchmark|q_test' | xargs grep -E 'SO=|SO |VERSION|FIXUP' >>Dates.txt
    log_step $LINENO "Search for error codes into zip/Errors-matched.txt"
    (find */src -type f) | grep -vE '_test\.cpp|\.log|Changes\.txt' | xargs grep -Eh ', [67][0-9][0-9][0-9]|"QH[67][0-9]|qh_fprintf_stderr\([67][0-9][0-9][0-9]' | sed -r 's/^[^Q67]*QH//' | sed -r 's/^.*qh_fprintf_stderr\(//' | sed -r 's/^[^67]*(errfile|ferr|fp|stderr), //' | sed 's/\\n"[,\)].*/ EOL/' | sed -r 's/_r([: ])/\1/' | sort >Errors.txt
    (cat Errors.txt | sed 's/, .*//'; for ((i=6001; i<6400; i++)); do echo $i; done;  for ((i=7001; i<7200; i++)); do echo $i; done) | sort | uniq -c | grep -v '^ *3 ' | sed -r 's/^[^0-9]*([0-9]) (.*)/\2 \1 NOT-MATCHED/' >Errors-not-matched.txt
    cat Errors.txt | grep -v 'EOL$' | sort -u >Errors-matched.txt
    log_step $LINENO "Search for mismatched '*_r.h' references to zip/FileRef.txt"
    grep -E '[^_][^_][^ *][.][ch]($|[^a-z>])|/libqhull/' */src/*/*_r.* */src/*/*_ra.* */src/libqhull_r/Makefile | grep -vE 'float.h|/html/| l.h.s. |libqhullcpp|mem.c for a standalone|qglobal.h|QhullError.|QhullSet.|string.h|unused.h|user.h and user_r.h' >FileRef.txt
    grep -E '_r[.]|_ra[.]|/libqhull_r/' */src/qconvex/qconvex.c */src/qconvex/qconvex.c */src/qdelaunay/qdelaun.c */src/qhalf/qhalf.c */src/qvoronoi/qvoronoi.c */src/testqset/* | grep -vE 'user.h and user_r.h' >>FileRef.txt
    log_step $LINENO "Search for mismatched option links in htm files to Links-single.txt"
    findf htm | xargs grep 'qh-opt.*\#' | tee Links-all.txt | sed -r -e 's/^[^:]*://' -e 's/qh-opt/\nqh-opt/g' | grep 'qh-opt.*<' | sed -r -e 's/<.*//' | LC_ALL=C  sort | uniq -c >Links-counts.txt
    grep ' 1 ' Links-counts.txt >Links-single.txt
    log_step $LINENO "Search for other internal links to Links-check-other.txt"
    findf htm | xargs grep -E 'http[^#]*htm#|href="?#|name='  | tee Links-other.txt | sed -e 's/#/\n#/g' -e 's/name="/\n#/g' -e 's/name=/\nname=/g' | grep -E '^#|name=' | sed -r -e 's/[" ].*//' | grep -vE ';|#[1-9][0-9]|#3dd|#[Hdv]$|[0-9][0-9]$|[a-z]T$|QJ$|convex|option|startup|voronoi|name=(as_q|num|sitesearch)' | LC_ALL=C  sort | uniq -c | grep ' 1 ' >Links-check-other.txt
    log_step $LINENO "Search for mismatched link quotes"
    findf htm | xargs grep -E '"'  | sed -e 's/"[^"]*"//g' | grep '"' | grep -vE 'html/qhull|html/rbox|working/debian' > Links-check-quotes.txt 
fi
if [[ -r $root_dir/$qhull_tgz_file ]]; then
    exit_if_fail $LINENO "mkdir -p $TEMP_DIR/tgz && cd $TEMP_DIR/tgz"
    log_step $LINENO "Current directory is $TEMP_DIR/tgz"
    exit_if_fail $LINENO "tar -zxf $root_dir/$qhull_tgz_file"
fi
log_step $LINENO "Check Changes.txt"
head -60 */src/Changes.txt | tail -17

#############################
log_step $LINENO "====================================================================="
log_step $LINENO "Check *qhull-zip-.../zip/Dates.txt and rbox.c for timestamps that need updating"
log_step $LINENO "Check *qhull-zip-.../zip/Errors-matched.txt for mismatched codes, errors not ending in NL, errors on multiple lines, and recently missing codes"
log_step $LINENO "  OK (\\n in error message) -- counters 6429,7027,.., 6023, 6233, 6237, 7089"
log_step $LINENO "Check *qhull-zip-.../zip/Errors-not-matched.txt for unused error codes (count==1, without text) or multiply-defined codes (count>2, 7079 OK)"
log_step $LINENO "Check *qhull-zip-.../zip/Links-single.txt for single use option links (matched names OK) in Links-all.txt"
log_step $LINENO "Check *qhull-zip-.../zip/Links-check-other.txt for unknown tags in Links-other.txt"
log_step $LINENO "Check *qhull-zip-.../zip/Links-check-quotes.txt for quotes that cross a line"
log_step $LINENO "Check q_egtest examples in Geomview"
log_step $LINENO "Check for 16 projects in Release mode, including qhulltest"
log_step $LINENO "Check build dependencies for programs."
log_step $LINENO "Check source dependencies and help prompts once a release"
log_step $LINENO " prompts: see qhull-zip.sh for command"
# N=qvoronoi; ($N . | grep -vE '^$|^Except|^Qhull' | sed 's/  */\n/g'; $N - | grep -vE '^ *#|^Qhull|0 roundoff|comments|options:' | sed 's/^  *//') | grep -vE '^$' | sort >x.1
log_step $LINENO " check QhullFacet/qh_printfacetheader, QhullRidge/qh_printridge, QhullVertex/qh_printvertex"
log_step $LINENO " check for internal errors while merging with pinched vertices, see qhull-zip.sh for command"
# ../eg/qtest.sh 10 '10000 s C1,2e-13 D3' 'd Q14' | grep -vE 'topology|precision|CPU|Maximum'
log_step $LINENO " check solution for ' = ' (comments OK)"
log_step $LINENO "Test qhull with 32-bit devstudio release, compare and update with q_test-ok.txt"
log_step $LINENO "  make testall 2>&1 | tee eg/q_test.x"

log_step $LINENO "Test CMake build"
log_step $LINENO " cd $TEMP_DIR/tgz/qhull*/build"
log_step $LINENO " cmake -G \"MSYS Makefiles\" .. && cmake .."
log_step $LINENO " make"
log_step $LINENO " mkdir -p ../bin/ && cp -p lib*.dll *.exe ../bin/"
log_step $LINENO " cd ..; make test"
log_step $LINENO "Test Linux compile"
log_step $LINENO " cd .. && scp $qhull_tgz_file qhull@qhull.org:"
log_step $LINENO " rm -rf qhull-$version; tar zxf $qhull_tgz_file && cd qhull-$version && make >../make.x 2>&1"
log_step $LINENO " more ../make.x"
log_step $LINENO " export DESTDIR=.; make install; cd usr/local/lib"
log_step $LINENO " make test"
log_step $LINENO " eg/q_test >eg/q_test.x 2>&1"
log_step $LINENO "Update 'Qhull wiki.md', qhull-news.htm (#problems, #bugs), qh-code.htm#enhance"
log_step $LINENO "Test qhull and compare to q_test-ok.txt"
log_step $LINENO " cd $TEMP_DIR/zip/qhull* && make testall >/d/bash/local/qhull/eg/q_test.x 2>&1"
log_step $LINENO "Build and test 64-bit qhulltest.  Compare to eg/qhulltest-ok.txt"
log_step $LINENO " cd /d/bash/local/qhull && bin/qhulltest --all >eg/qhulltest.x 2>&1"
log_step $LINENO "Compare source directories:  Copy 'qhull/' to 'downloads/qhull-YYYY/'"
log_step $LINENO "  If comparing 2020 to 2019 -- find qhull-2020 -type f ! -path "./.git*" ! -path "*working*" | xargs sed -i 's/2020/2019/g'"
log_step $LINENO "Benchmark qhull.  Compare to eg/q_benchmark-ok.txt"
log_step $LINENO "  cd $TEMP_DIR/zip/qhull* && make benchmark >/d/bash/local/qhull/eg/q_benchmark.x 2>&1"
log_step $LINENO "Build qhull with gcc"
log_step $LINENO " cd $TEMP_DIR/zip/qhull* && make SO=dll" 
log_step $LINENO " cp -p lib/libqhull*.dll bin && make testall >/d/bash/local/qhull/eg/q_test-make.x 2>&1"
log_step $LINENO "Create qhull_qh and compare with libqhull, qconvex, etc.  See qh_code.htm#convert"
log_step $LINENO " eg/make-qhull_qh.sh libqhull_r"
log_step $LINENO " Ignore 'Id: //.*' and 'DateTime: 20.*'  Edit>FullRefresh"
log_step $LINENO " Compare qhull*exports.def and add new functions"
log_step $LINENO "Build and test libqhull.  user_eg3 not built"
log_step $LINENO " make cleanall && cd src/libqhull && make cleanall && make && cp *.exe ../../bin && cd ../.. && make test && ls -l bin/qhull.exe"
log_step $LINENO " make testall >/d/bash/local/qhull/eg/q_test-libqhull.x 2>&1"
log_step $LINENO "Build and test libqhull with qh_QHpointer"
log_step $LINENO " e src/libqhull/user.h"
log_step $LINENO " make cleanall && cd src/libqhull && make cleanall && make && cp *.exe ../../bin && cd ../.. && make test && ls -l bin/qhull.exe"
log_step $LINENO " bin/rbox c | bin/qhull FO Tz | grep QHpointer"
log_step $LINENO " make testall 2>&1 | tee eg/q_test-qh_QHpointer.x"
log_step $LINENO "Build and test libqhull_r.  user_eg3 not built"
log_step $LINENO " make cleanall && cd src/libqhull_r && make cleanall && make && cp *.exe ../../bin && cd ../.. && make test && ls -l bin/qhull.exe"
log_step $LINENO " make testall >/d/bash/local/qhull/eg/q_test-libqhull_r.x 2>&1"
log_step $LINENO "Build and test libqhull_r with qh_NOmem"
log_step $LINENO "Build and test libqhull_r with qh_NOmerge"
log_step $LINENO "Build and test libqhull_r with qh_NOtrace"
log_step $LINENO "Build and test libqhull_r with qh_KEEPstatistics 0"
log_step $LINENO "Build and check Makefile/qhullx.  user_eg* not built"
log_step $LINENO " make cleanall && make qhullx && make test && ls -l bin/qhull.exe"
log_step $LINENO "Benchmark libqhull_r with gcc"
log_step $LINENO " make benchmark >/d/bash/local/qhull/eg/q_benchmark-libqhull_r.x 2>&1"
log_step $LINENO "Check Qhull-go (double-click)"
log_step $LINENO "Compare Changes.txt with previous release"
log_step $LINENO "Compare README.txt with previous release"
log_step $LINENO "Compare previous zip release, Dates.txt, and md5sum"
log_step $LINENO "Compare zip and tgz for CRLF vs LF"
log_step $LINENO "  C:/Git$TEMP_DIR/"
log_step $LINENO "Compare qh_prompt* for unix_r.c,qconvex.c,etc."
log_step $LINENO "Check html links with Firefox Link Analyzer (fast but doesn't check #..."
log_step $LINENO "  Replace old links with web.archive.org (archive.is)"
log_step $LINENO "Check all files for FIXUP comments, including Makefile, html, etc."
log_step $LINENO "Extract zip to download/ and compare directories"
log_step $LINENO "Retest 32-bit release builds and 64-bit qhulltest"
log_step $LINENO "Check for 32-bit release executables from DevStudio (<500K and 'Ts' 32-bit allocations)"
log_step $LINENO "Check for virus with Windows Defender"
log_step $LINENO "Check object dependencies with 'objdump -p user_eg.exe'"
log_step $LINENO "Copy tarballs to qhull.org"
log_step $LINENO " cd .. && ls -l qhull-2020.2* qhull*8.0.2*"
log_step $LINENO " scp -p qhull-2020.2* qhull*8.0.2* qhull@qhull.org:web/download/"
log_step $LINENO "Add md5sums to end of qh-get.htm"
log_step $LINENO "Add release labels to git"
log_step $LINENO "Finished successfully"
#############################

