#!/bin/bash
#
# eg/qtest.sh -- test qhull runs or log a qhull run
#
# tags: #help #test-rbox #test-qhull #grep #grep-merge #grep-step
#       #log-qhull #log-t #log-QR
#
# Test for internal errors using difficult cases for Qhull (see q_benchmark)
#  ../eg/qtest.sh 100 '500 s W1e-13 C1,1e-13 D4' 'd' | grep -vE 'topology|precision|CPU|Maximum
#
# $Id: //main/2019/qhull/eg/qtest.sh#4 $$DateTime: 2019/06/25 15:24:47 $
# $Change: 2701 $$Author: bbarber $

QHULL=${QHULL:-qhull}
Tnz=${Tnz:-T4z}
PROG=$0
NOW=$(date +"%s")
TODAY=$(date +"%Y-%m-%d")
BASE=$(( (NOW * 234533477) % 1000000 ))
COUNT=0
QH_TMPDIR="$TMP/qtest-$TODAY"
QH_TEST_DEST="$QH_TMPDIR/qhull-$BASE.log"
QH_SHOW_DEST="$QH_TMPDIR/qhull-$BASE-show.log"
QH_LOG_DEST="qhull.log"
QH_STEP_DEST="qhull-step.log"
QH_TEST_BASE=1
QH_TEST_GREP='QH[67]| Statis|[0-9][0-9][.][0-9]x|CPU|FIXUP|retries, input joggled|initial hull is narrow'
QH_SHOW_GREPX="QH7[0-9][^8]|Stati"
QH_STEP_GREP="QH[0-367]|FIXUP"
QH_STEP_ARG="-A10"
QH_STEP_EXTRA="QH6| of [0-9]+ points in|:precision problems"
QH_MERGE_GREP="QH[67]|QH(1040|1050|3034)|qh_addpoint:|qh_checkflipped|qh_matchdupridge|qh_maxouter|qh_mergefacet|qh_test_centrum_merge|qh_test_nonsimplicial_merge|qtest.sh"
QH_MERGE_GREPX='keep 239874664' # 'mergetype (11|13)|mindist= -2, maxdist= -2$'
# QH_APPEND_DEST [env only] for copy of QH_TEST_DEST (e.g., eg/q_benchmark)
# QH_APPEND_SHOW [env only] for copy of QH_SHOW_DEST (e.g., eg/q_benchmark)
# QH_TEST        [env only]  grep expression appended to QH_TEST_GREP
# QH_TESTX       [env only]  grep expression to exclude from QH_TEST_GREP
# QH_SHOW        [env only]  grep expression to exclude from show log
# QH_SHOWX       [env only]  grep expression appended to QH_SHOW_GREPX
# QH_GREP        [env only]  grep expression appended to 'qtest.sh grep'
# QH_GREPX       [env only]  grep expression to exclude from 'qtest.sh grep'

if [[ $# -eq 0 || "$1" == "help" || "$1" == "--help" || "$1" == "-?" ]]; then
    echo
    #help
    echo "qtest.sh -- test qhull runs"
    echo "  qtest.sh  N 'rbox c | qhull'         -- N random qhull runs"
    echo "  qtest.sh  N 'rbox-opts' 'qhull-opts' -- N random rbox|qhull runs"
    echo "  qtest.sh -N 'rbox c | qhull'         -- N qhull runs"
    echo "  qtest.sh -N 'rbox opts' 'qhull opts' -- N rbox|qhull runs"
    echo "execute rbox and qhull"
    echo "  qtest.sh run 'rbox c | qhull'        -- execute 'qhull command'"
    echo "  qtest.sh run QR... 'rbox | qhull'    -- execute 'qhull command QR...'"
    echo "  qtest.sh run t... 'rbox-opts' 'qhull-opts' -- execute 'rbox t... | qhull'"
    echo "log execution of rbox and qhull"
    echo "  qtest.sh log 'rbox c | qhull'        -- log qhull to qhull.log/qhull-step.log"
    echo "  qtest.sh QR... 'rbox c | qhull'      -- log a qhull run"
    echo "  qtest.sh t... 'rbox-opts' 'qhull-opts' -- log a 'rbox|qhull' run"
    echo "  qtest.sh grep 'include-regexp'       -- grep qhull.log for events to grep.log"
    echo "  qtest.sh grep 'include-regexp' 'exclude-regexp'"
    echo "  qtest.sh grep-merge logfile          -- grep logfile for qh_mergefacet,etc. to stdout"
    echo "  qtest.sh grep-step logfile           -- grep logfile for steps to stdout"
    echo "  qtest.sh -v ...                      -- verbose output with environment vars"
    echo "variables defined in qtest.sh"
    echo "  QH_TEST_BASE        -- first tNN for negative counts"
    echo "  QH_TEST_DEST        -- qtest.sh log"
    echo "  QH_TEST_GREP        --   grep expression for qtest.sh log"
    echo "  QH_SHOW_DEST        -- qtest.sh show log"
    echo "  QH_SHOW_GREPX       --   exclude grep for show log"
    echo "  QH_LOG_DEST         -- qhull log"
    echo "  QH_LOG_GREP         --   grep expression for qhull log"
    echo "  QH_STEP_DEST        -- qhull step log"
    echo "  QH_STEP_GREP        --   grep expression for step log and grep-step"
    echo "  QH_MERGE_GREP       -- grep expression for grep-merge"
    echo "  QH_MERGE_GREPX      --   exclude grep for grep-merge"
    echo "environment variables"
    echo "  QHULL               -- qhull program, may be qconvex, etc"
    echo "  QH_APPEND_DEST      -- append qtest.sh log"
    echo "  QH_APPEND_SHOW      -- append show log"
    echo "  QH_TEST             -- additional grep expression for qtest.sh log"
    echo "  QH_TESTX            --   exclude grep for qtest.sh log"
    echo "  QH_SHOW             -- grep expression for show log"
    echo "  QH_SHOWX            --   additional exclude grep for show log"
    echo "  QH_GREP             -- additional grep expression for grep log"
    echo "  QH_GREPX            --   additional exclude grep for grep log"
    echo "  Tnz                 -- Trace setting for 'qtest.sh log'"
    echo "examples"
    echo "  qtest.sh help"
    echo "  qtest.sh 25 '1000 W1e-13 D4' ''"
    echo "  qtest.sh 25 '1000 W0 D4 C2,2e-13' 'Q14'"
    echo "  qtest.sh 25 '300 D4 C1,2e-13' 'd Q14'"
    echo "  qtest.sh -10 '10000 D4' ''"
    echo "  qtest.sh -10 '1000 s D4' ''"
    echo "  qtest.sh -10 'rbox 1000 s D4 | qhull'"
    echo "  qtest.sh run QR4 'rbox 1000 s D4 | qhull'"
    echo "  qtest.sh run t876333 '1000 s D4' ''"
    echo "  qtest.sh log 'rbox 500 s D4 | qhull'         # qhull.log qhull-step.log"
    echo "  qtest.sh t876333 '500 s D4' ''"
    echo "  qtest.sh QR4 'rbox 500 s D4 | qhull'"
    echo "  qtest.sh grep 'qh_addpoint|qh_mark_dupridges' | head -20 # QH_GREP QH_GREPX"
    echo "  qtest.sh grep 'qh_addpoint' 'added| qh_addpoint' | head -20"
    echo "  qtest.sh grep-merge qhull.log | head -20"
    echo "  qtest.sh grep-step qhull.log >qhull-step.log"
    echo "  qtest.sh -v -10 '10000 D4' ''                 # verbose"
    echo
    echo "$(grep Id: $0 | head -1)"
    exit
fi

function isnum {
   expr "$1" + 0 >/dev/null 2>&1
} 

if ! which rbox >/dev/null 2>&1; then
    if [[ ! -d bin ]]; then
        echo 'eg/qtest.sh: Run eg/qtest.sh from the Qhull directory with bin/, or add $QHULL' "'$QHULL' and 'rbox' to" '$PATH'
        exit 1
    fi
    if [[ ! -e bin/rbox && ! -e bin/rbox.exe ]]; then
        echo "eg/qtest.sh: Build '$QHULL' and 'rbox', or add" '$QHULL' "'$QHULL' and 'rbox' to" '$PATH'
        exit 1
    fi
    if [[ ! -e "bin/$QHULL" && ! -e "bin/$QHULL.exe" ]]; then
        echo "eg/qtest.sh: Build '$QHULL' and 'rbox', or add" '$QHULL' "'$QHULL' and 'rbox' to" '$PATH'
        exit 1
    fi
    PATH="$PWD/bin:$PATH"
    if ! which rbox >/dev/null 2>&1; then
        echo 'eg/qtest.sh: Failed to temporily add "$PWD/bin" to $PATH for access to $QHULL and rbox'  "'$QHULL'." 'Please execute "export PATH=$PWD/bin:$PATH"'
        exit 1
    fi
fi
if ! which $QHULL >/dev/null 2>&1; then
    echo 'eg/qtest.sh: $QHULL failed.  Cannot run' "'rbox c | $QHULL'.  Add '$QHULL' to" '$PATH'
    exit 1
fi
if [[ ! -d $QH_TMPDIR ]]; then
   mkdir -p "$QH_TMPDIR" || (echo "qtest.sh: 'mkdir -p $QH_TMPDIR'" && exit 1)
fi
ISLOG=0
if [[ "$1" == "-v" ]]; then
   ISLOG=1
   shift
fi
if [[ "$1" == "run" ]]; then
   if [[ $# -eq 2 ]]; then
     sh -c "$2"
   elif [[ $# -eq 3 && "${2:0:2}" == "QR" ]]; then
     sh -c "$3 $2"
   elif [[ $# -eq 3 ]]; then
     rbox $2 | $QHULL $3
   elif [[ $# -eq 4 && "${2:0:1}" == "t" ]]; then
     rbox $3 $2 | $QHULL $4
   elif [[ $# -eq 4 ]]; then
     echo "qtest.sh: Expecting 'run' followed by 'tNNN rbox-args qhull-args'. See 'qtest.sh help'" >&2
     exit 1
   else
     echo "qtest.sh: Expecting 'run' followed by 1 to 3 arguments. Got total of $# arguments. See 'qtest.sh help'" >&2
     exit 1
   fi
   exit
fi
if [[ $# -gt 3 ]]; then
   echo "qtest.sh: Expecting 1 to 3 arguments. Got $# arguments. See 'qtest.sh help'" >&2
   exit 1
fi

if [[ "$QH_TEST_DEST" == "" ||  "$QH_SHOW_DEST" == "" || "$QH_LOG_DEST" == "" ]]; then
    echo "qtest.sh: script does not define \$QH_TEST_DEST, \$QH_SHOW_DEST, or \$QH_LOG_DEST.  Required setting for 'qtest.sh help'" >&2
    echo -e "  QH_TEST_DEST='$QH_TEST_DEST'\n  QH_SHOW_DEST='$QH_SHOW_DEST'" >&2
    echo -e "  QH_LOG_DEST='$QH_TEST_DEST'" >&2
    exit 1
elif [[ "$QH_TEST_GREP" == "" ||  "$QH_SHOW_GREPX" == "" || "$QH_STEP_EXTRA" == "" ]]; then
    echo "qtest.sh: script does not define \$QH_TEST_GREP, \$QH_STEP_EXTRA, or \$QH_SHOW_GREPX.  Required setting for 'qtest.sh help'" >&2
    echo -e "  QH_TEST_GREP='$QH_TEST_GREP'\n  QH_SHOW_GREPX='$QH_SHOW_GREPX'\n  QH_STEP_EXTRA='$QH_STEP_EXTRA'" >&2
    exit 1
fi

if isnum "$1"; then
    COUNT=$1
fi
if  [[ $COUNT -lt 0 ]]; then
    BASE=$(( QH_TEST_BASE ))
    COUNT=$(( -COUNT ))
fi
if  [[ $COUNT -gt 0 ]]; then
    DEST="$QH_TEST_DEST"
    GREP="$QH_TEST_GREP"
    GREPX="keep 239874664"
    DEST2="$QH_SHOW_DEST"
    GREP2="."
    GREPX2="$QH_SHOW_GREPX"
    if [[ "$QH_TEST" != "" ]]; then
        GREP="$GREP|$QH_TEST"
    fi
    if [[ "$QH_TESTX" != "" ]]; then
        GREPX="$QH_TESTX"
    fi
    if [[ "$QH_SHOW" != "" ]]; then
        GREP2="$QH_SHOW"
    fi
    if [[ "$QH_SHOWX" != "" ]]; then
        GREPX2="$GREPX2|$QH_SHOWX"
    fi
    if [[ $# -eq 3 ]]; then
        CMD="rbox $2 t$BASE | $QHULL $3"
        echo -e "\nTesting -- $CMD >$DEST" | tee "$DEST" | tee "$DEST2"
        echo "$PROG $COUNT '$2' '$3'" >> "$DEST"
        (date; pwd; ls -ld "$(which $QHULL)"; $QHULL -V) >> "$DEST"
        if ! rbox $2 1000 t$BASE | head -10 >/dev/null; then
            echo; echo "qtest.sh error: 'rbox $2 t$BASE' failed"
            exit 1
        fi
        if ! (rbox 20 | $QHULL $3 TA1) >/dev/null; then
            echo; echo "qtest.sh error: 'rbox 20 | $QHULL $3 TA1' failed"
            exit 1
        fi
        if [[ $ISLOG -gt 0 ]]; then
            echo "qtest.sh: rbox $2 t$i | $QHULL $3 Tsz | grep -E '$GREP' | grep -Ev '$GREPX' | tee -a '$DEST' | grep -E '$GREP2' | grep -Ev '$GREPX2' | tee -a '$DEST2' "
            echo -e "  QH_TEST_DEST='$QH_TEST_DEST'\n  QH_SHOW_DEST='$QH_SHOW_DEST'"
            echo -e "  QH_TEST_GREP='$QH_TEST_GREP'\n  QH_SHOW_GREPX='$QH_SHOW_GREPX'"
            echo -e "  QH_TEST='$QH_TEST'\n  QH_TESTX='$QH_TESTX'"
            echo -e "  QH_SHOW='$QH_SHOW'\n  QH_SHOWX='$QH_SHOWX'"
        fi
        #test-rbox
        START=$(date +"%s")
        for ((i=0; i<$COUNT; i++)); do 
            RUN=$(( BASE + i ))
            echo "Test -- t$RUN" | tee -a "$DEST" | tee -a "$DEST2"
            sh -c "rbox $2 t$RUN | $QHULL $3 Tsz" | grep -E "$GREP" | grep -Ev "$GREPX" | tee -a "$DEST" \
              | grep -E "$GREP2" | grep -Ev "$GREPX2" | tee -a "$DEST2"           
        done
        DONE=$(date +"%s")
        if [[ $COUNT -gt 2 ]]; then
            echo "$DONE Test $COUNT runs in $((DONE - START)) seconds (ave. $(( (DONE - START)*1000/COUNT )) msec) -- $CMD" | tee -a "$DEST" | tee -a "$DEST2" 
        fi
    elif [[ $# -eq 2 ]]; then
        CMD="$2"
        echo "Testing -- $CMD >$DEST" | tee "$DEST" | tee "$DEST2"
        echo "$PROG $COUNT '$CMD'" >> "$DEST"
        (date; pwd; ls -ld "$(which $QHULL)"; $QHULL -V) >> "$DEST"
        if ! sh -c "$CMD QR0 TA1" >/dev/null; then
            echo; echo "qtest.sh error: qhull failed"
            exit 1
        fi
        if [[ $ISLOG -gt 0 ]]; then
            echo "qtest.sh: $CMD QR$BASE Tsz | grep -E '$GREP' | grep -Ev '$GREPX' | tee -a '$DEST' | grep -E '$GREP2' | grep -Ev '$GREPX2' | tee -a '$DEST2' "      
            echo -e "  QH_TEST_DEST='$QH_TEST_DEST'\n  QH_SHOW_DEST='$QH_SHOW_DEST'"
            echo -e "  QH_TEST_GREP='$QH_TEST_GREP'\n  QH_SHOW_GREPX='$QH_SHOW_GREPX'"      
            echo -e "  QH_TEST='$QH_TEST'\n  QH_TESTX='$QH_TESTX'"      
            echo -e "  QH_SHOW='$QH_SHOW'\n  QH_SHOWX='$QH_SHOWX'"      
        fi
        #test-qhull
        START=$(date +"%s")
        for ((i=0; i<$COUNT; i++)); do 
            RUN=$(( BASE + i ))
            echo "Test -- QR$RUN" | tee -a "$DEST" | tee -a "$DEST2"
            sh -c "$CMD QR$RUN Tsz" | grep -E "$GREP" | grep -Ev "$GREPX" | tee -a "$DEST" \
              | grep -E "$GREP2" | grep -Ev "$GREPX2" | tee -a "$DEST2"           
        done
        DONE=$(date +"%s")
        if [[ $COUNT -gt 2 ]]; then
            echo "$DONE Test $COUNT runs in $((DONE - START)) seconds (average $(( (DONE - START)*1000/COUNT )) msec) -- $CMD" | tee -a "$DEST" | tee -a "$DEST2" 
        fi
    else
        echo "qtest.sh: Expecting run count followed by rbox and qhull options.  Only run count provided ($1)."
        exit 1
    fi
    if [[ "$QH_APPEND_DEST" != "" ]]; then
        cat "$QH_TEST_DEST" >>"$QH_APPEND_DEST"
    fi
    if [[ "$QH_APPEND_SHOW" != "" ]]; then
        cat "$QH_SHOW_DEST" >>"$QH_APPEND_SHOW"
    fi
elif [[ "$1" == "grep" ]]; then
    if [[ $# -ge 2 && "$QH_GREP" != "" ]]; then
        GREP="$2|$QH_GREP"
    elif [[ $# -ge 2 ]]; then
        GREP="$2"
    elif [[ "$QH_GREP" != "" ]]; then
        GREP="$QH_GREP"
    else
        GREP="."
    fi
    if [[ $# -eq 3 && "$QH_GREPX" != "" ]]; then
        GREPX="$3|$QH_GREPX"
    elif [[ $# -eq 3 ]]; then
        GREPX="$3"
    elif [[ "$QH_GREPX" != "" ]]; then
        GREPX="$QH_GREPX"
    else
        if [[ "$GREP" == "." ]]; then
            echo "qtest.sh: Expecting 'grep' followed by include and exclude regexps or \$QH_GREP or \$QH_GREPX.  None defined."
            exit 1
        fi
        GREPX="keep 239874664"
    fi
    echo "qtest.sh: grep from '$QH_LOG_DEST' to stdout"
    if [[ $ISLOG -gt 0 ]]; then
        echo "qtest.sh: grep -E '$GREP' '$QH_LOG_DEST' | grep -Ev '$GREPX'" >&2
        echo -e "  QH_LOG_DEST='$QH_TEST_DEST'"  >&2
        echo -e "  QH_GREP='$QH_GREP'\n  QH_GREPX='$QH_GREPX'"  >&2
    fi
    #grep
    grep -E "$GREP" "$QH_LOG_DEST" | grep -Ev "$GREPX"
elif [[ "$1" == "grep-merge" ]]; then
    if [[ $# -ne 2 || ! -r "$2" ]]; then
        echo "qtest.sh: Expecting 'grep-merge' followed by log file. Got '$1 $2 $3 ...'" >&2
        exit 1
    fi
    SRC="$2"
    # includes qh_appendvertexmerge, qh_findhorizon
    GREP="$QH_MERGE_GREP"
    GREPX="$QH_MERGE_GREPX"
    SED='s/^xxx[0-9]*://'
    SED2='s/horizon for point.*/horizon for point/'
    echo "qtest.sh: grep merges from '$SRC' to stdout" >&2
    if [[ $ISLOG -gt 0 ]]; then
        echo "qtest.sh: grep -E '$GREP' '$SRC' | grep -Ev '$GREPX' | sed -r -e '$SED' -e '$SED2'" >&2
    fi
    #grep-merge
    grep -E "$GREP" "$SRC" | grep -Ev "$GREPX" | sed -r -e "$SED" -e "$SED2"
elif [[ "$1" == "grep-step" ]]; then
    if [[ $# -ne 2 || ! -r "$2" ]]; then
        echo "qtest.sh: Expecting 'grep-step' followed by log file. Got '$1 $2 $3 ...'" >&2
        exit 1
    fi
    SRC="$2"
    GREP2="$QH_STEP_GREP"
    GREPX2="keep 239874664"
    ARG="$QH_STEP_ARG"
    EXTRA="$QH_STEP_EXTRA"
    if [[ "$QH_STEP" != "" ]]; then
        GREP2="$GREP2|$QH_STEP"
    fi
    if [[ "$QH_STEPX" != "" ]]; then
        GREPX2="$QH_STEPX"
    fi
    if [[ "$QH_ARG" != "" ]]; then
        ARG="$QH_ARG"
    fi
    if [[ "$QH_EXTRA" != "" ]]; then
        EXTRA="$EXTRA|$QH_EXTRA"
    fi
    echo "qtest.sh: grep step log from '$SRC' to stdout" >&2
    if [[ $ISLOG -gt 0 ]]; then
        echo "qtest.sh: (grep -E '$GREP2' '$SRC' | grep -Ev '$GREPX2'; echo; echo ==== EXTRA $(date) ====; echo; grep '$ARG' -E '$EXTRA' '$SRC')"  >&2
        echo -e "  QH_STEP_GREP='$QH_STEP_GREP'" >&2
        echo -e "  QH_STEP_ARG='$QH_STEP_ARG'\n  QH_STEP_EXTRA='$QH_STEP_EXTRA'" >&2
        echo -e "  QH_ARG='$QH_ARG'\n  QH_STEP='$QH_STEP'" >&2
    fi
    #grep-step
    grep -E "$GREP2" "$SRC" | grep -Ev "$GREPX2"; 
    echo; echo ==== EXTRA $(date) ====; echo; grep $ARG -E "$EXTRA" "$SRC"
elif [[ "$1" == "log" && $# -ne 2 ]]; then
    echo "qtest.sh: Expecting 'log' followed by a qhull command line in quotes.  Got $# arguments" >&2
    exit 1
elif [[ "${1:0:1}" == "t" && $# -eq 1 ]]; then
    echo "qtest.sh: Expecting txxxxxx followed by rbox and qhull options.  Got tag '$1' only" >&2
    exit 1
elif [[ "${1:0:1}" == "t" && $# -eq 2 ]]; then
    echo "qtest.sh: Expecting txxxxxx followed by rbox and qhull options.  Tag '$1' was followed by a qhull command -- $2" >&2
    exit 1
elif [[ "${1:0:2}" == "QR" && $# -eq 1 ]]; then
    echo "qtest.sh: Expecting QRxxxxxx followed by a qhull command.  Got tag '$1' only" >&2
    exit 1
elif [[ "${1:0:2}" == "QR" && $# -eq 3 ]]; then
    echo "qtest.sh: Expecting QRxxxxxx followed by a qhull command.  Got rbox options '$2' and qhull options '$3'" >&2
    exit 1
elif [[ "${1:0:1}" != "t" && $# -eq 3 ]]; then
    echo "qtest.sh: Expecting 'txxxx' with rbox and qhull options.  Got unknown tag '$1' with two additional arguments" >&2
    exit 1
elif [[ "${1:0:2}" != "QR" && "$1" != "log" && $# -eq 2 ]]; then
    echo "qtest.sh: Expecting 'log' or 'QRxxxx' followed by a qhull command.  Got unknown tag '$1' followed by one additional argument '$2'" >&2
    exit 1
else
    DEST="$QH_LOG_DEST"
    DEST2="$QH_STEP_DEST"
    GREP="$QH_LOG_GREP"
    GREPX="keep 239874664"
    GREP2="$QH_STEP_GREP"
    GREPX2="keep 239874664"
    ARG="$QH_STEP_ARG"
    EXTRA="$QH_STEP_EXTRA"
    if [[ "$QH_LOG" != "" ]]; then
        GREP="$GREP|$QH_LOG"
    fi
    if [[ "$QH_LOGX" != "" ]]; then
        GREPX="$QH_LOGX"
    fi
    if [[ "$QH_STEP" != "" ]]; then
        GREP2="$GREP2|$QH_STEP"
    fi
    if [[ "$QH_STEPX" != "" ]]; then
        GREPX2="$QH_STEPX"
    fi
    if [[ "$QH_ARG" != "" ]]; then
        ARG="$QH_ARG"
    fi
    if [[ "$QH_EXTRA" != "" ]]; then
        EXTRA="$EXTRA|$QH_EXTRA"
    fi
    if [[ "$1" == "log" ]]; then
        echo "qtest.sh: log '$2 $Tnz' to '$DEST' and '$DEST2'" | tee "$DEST" | tee "$DEST2" >&2
        (echo -n "$HOSTNAME "; date ; pwd; ls -ld "$(which $QHULL)"; $QHULL -V) >> "$DEST"
        if [[ $ISLOG -gt 0 ]]; then
            echo "qtest.sh: $2 $Tnz | grep -n . | grep -E '$GREP' | grep -Ev '$GREPX' | tee -a '$DEST' | grep -E '$GREP2' | grep -Ev '$GREPX2' | tee -a '$DEST2'; (echo; echo ==== EXTRA $(date) ====; echo; grep '$ARG' -E '$EXTRA' '$DEST') | tee -a '$DEST2' " >&2
            echo -e "  QH_LOG_DEST='$QH_LOG_DEST'\n  QH_STEP_DEST='$QH_STEP_DEST'" >&2
            echo -e "  QH_LOG_GREP='$QH_LOG_GREP'\n  QH_STEP_GREP='$QH_STEP_GREP'" >&2
            echo -e "  QH_STEP_ARG='$QH_STEP_ARG'\n  QH_STEP_EXTRA='$QH_STEP_EXTRA'" >&2
            echo -e "  QH_LOG='$QH_LOG'\n  QH_LOGX='$QH_LOGX'" >&2
            echo -e "  QH_ARG='$QH_ARG'\n  QH_STEP='$QH_STEP'" >&2
        fi
        #log-qhull
        sh -c "$2 $Tnz" | grep -n . | grep -E "$GREP" | grep -Ev "$GREPX" | tee -a "$DEST" \
              | grep -E "$GREP2" | grep -Ev "$GREPX2" >> "$DEST2"           
        (echo; echo ==== EXTRA $(date) ====; echo; grep $ARG -E "$EXTRA" "$DEST") | tee -a "$DEST2"
    elif [[ "${1:0:1}" == "t" ]]; then
        echo "qtest.sh: logging 'rbox $2 $1 | $QHULL T4sz $3' to '$DEST' and '$DEST2'" | tee "$DEST" | tee "$DEST2" >&2
        (date; pwd; ls -ld "$(which $QHULL)"; $QHULL -V) >> "$DEST"
        if [[ $ISLOG -gt 0 ]]; then
            echo "qtest.sh: rbox $2 $i | $QHULL T4sz $3 | grep -E '$GREP' | grep -Ev '$GREPX' | tee -a '$DEST' | grep -E '$GREP2' | grep -Ev '$GREPX2' | tee -a '$DEST2'; (echo; echo ==== EXTRA $(date) ====; echo; grep '$ARG' -E '$EXTRA' '$DEST') | tee -a '$DEST2'" >&2
            echo -e "  QH_TEST_DEST='$QH_TEST_DEST'\n  QH_SHOW_DEST='$QH_SHOW_DEST'" >&2
            echo -e "  QH_TEST_GREP='$QH_TEST_GREP'\n  QH_SHOW_GREPX='$QH_SHOW_GREPX'" >&2
            echo -e "  QH_TEST_ARG='$QH_TEST_ARG'\n  QH_TEST_EXTRA='$QH_TEST_EXTRA'" >&2
            echo -e "  QH_TEST='$QH_TEST'\n  QH_TESTX='$QH_TESTX'" >&2
            echo -e "  QH_SHOW='$QH_SHOW'\n  QH_SHOWX='$QH_SHOWX'" >&2
            echo -e "  QH_ARG='$QH_ARG'\n    QH_EXTRA='$QH_EXTRA'" >&2
        fi
        if ! rbox $2 $1 1000 >/dev/null; then
            echo; echo "qtest.sh error: 'rbox $2 $1' failed" >&2
            exit 1
        fi
        #log-t
        rbox $2 $1 | $QHULL $Tnz $3 | grep -n . | grep -E "$GREP" | grep -Ev "$GREPX" | tee -a "$DEST" \
              | grep -E "$GREP2" | grep -Ev "$GREPX2" >> "$DEST2"
        (echo; echo ==== EXTRA $(date) ====; echo; grep $ARG -E "$EXTRA" "$DEST") | tee -a "$DEST2"
    elif [[ "${1:0:2}" == "QR" ]]; then
        echo "qtest.sh: log '$2 $Tnz $1' to '$DEST' and '$DEST2'" | tee "$DEST" | tee "$DEST2" >&2
        (date; pwd; ls -ld "$(which $QHULL)"; $QHULL -V) >> "$DEST"
        if [[ $ISLOG -gt 0 ]]; then
            echo "qtest.sh: $2 $Tnz $1 | grep -n . | grep -E '$GREP' | grep -Ev '$GREPX' | tee -a '$DEST' | grep -E '$GREP2' | grep -Ev '$GREPX2' | tee -a '$DEST2'; (echo; echo ==== EXTRA $(date) ====; echo; grep '$ARG' -E '$EXTRA' '$DEST') | tee -a '$DEST2' " >&2
            echo -e "  QH_LOG_DEST='$QH_LOG_DEST'\n  QH_STEP_DEST='$QH_STEP_DEST'" >&2
            echo -e "  QH_LOG_GREP='$QH_LOG_GREP'\n  QH_STEP_GREP='$QH_STEP_GREP'" >&2
            echo -e "  QH_STEP_ARG='$QH_STEP_ARG'\n  QH_STEP_EXTRA='$QH_STEP_EXTRA'" >&2
            echo -e "  QH_LOG='$QH_LOG'\n  QH_LOGX='$QH_LOGX'" >&2
            echo -e "  QH_ARG='$QH_ARG'\n  QH_STEP='$QH_STEP'" >&2
        fi
        #log-QR
        sh -c "$2 $Tnz $1" | grep -n . | grep -E "$GREP" | grep -Ev "$GREPX" | tee -a "$DEST" \
              | grep -E "$GREP2" | grep -Ev "$GREPX2" >> "$DEST2"           
        (echo; echo ==== EXTRA $(date) ====; echo; grep $ARG -E "$EXTRA" "$DEST") | tee -a "$DEST2"
    else
        echo "qtest.sh: unexpected arguments, run 'qtest.sh' for help -- qtest.sh $1 $2 $3"
        exit 1
    fi
fi

# end of qtest.sh
