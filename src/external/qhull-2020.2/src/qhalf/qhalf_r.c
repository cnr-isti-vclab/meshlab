/*<html><pre>  -<a                             href="../libqhull_r/qh-qhull_r.htm"
  >-------------------------------</a><a name="TOP">-</a>

   qhalf_r.c
     compute the intersection of halfspaces about a point

   see unix_r.c for full interface

   Copyright (c) 1993-2020, The Geometry Center
*/

#include "libqhull_r/libqhull_r.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
  int isatty(int);
}

#elif defined(_MSC_VER)
#include <io.h>
#define isatty _isatty
/* int _isatty(int); */

#else
int isatty(int);  /* returns 1 if stdin is a tty
                   if "Undefined symbol" this can be deleted along with call in main() */
#endif

/*-<a                             href="../libqhull_r/qh-qhull_r.htm#TOC"
  >-------------------------------</a><a name="prompt">-</a>

  qh_prompt
    long prompt for qhull

  notes:
    restricted version of libqhull_r.c
    same text as unix_r.c
    see concise prompt below
    limit maximum literal to 1800 characters
*/

/* duplicated in qhalf.htm */
char hidden_options[]=" d n v Qbb QbB Qf Qg Qm Qr Qv Qx Qz TR E V Fa FA FC FD FS Ft FV Gt Q0 Q1 Q2 Q3 Q4 Q5 Q6 Q7 Q8 Q9 Q10 Q11 Q15 ";

char qh_prompta[]= "\n\
qhalf -- compute the intersection of halfspaces about a point\n\
    http://www.qhull.org  %s\n\
\n\
input (stdin):\n\
    optional interior point: dimension, 1, coordinates\n\
    first lines: dimension+1 and number of halfspaces\n\
    other lines: halfspace coefficients followed by offset\n\
    comments:    start with a non-numeric character\n\
\n\
options:\n\
    Hn,n - specify coordinates of interior point\n\
    Qc   - keep coplanar halfspaces\n\
    Qi   - keep other redundant halfspaces\n\
    QJ   - joggled input instead of merged facets\n\
    Qt   - triangulated output\n\
\n\
Qhull control options:\n\
    Qa   - allow input with fewer or more points than coordinates\n\
    Qbk:0Bk:0 - remove k-th coordinate from input\n\
    QJn  - randomly joggle input in range [-n,n]\n\
    QRn  - random rotation (n=seed, n=0 time, n=-1 time/no rotate)\n\
    Qs   - search all halfspaces for the initial simplex\n\
\n\
%s%s%s%s";  /* split up qh_prompt for Visual C++ */
char qh_promptb[]= "\
Qhull extra options:\n\
    QGn  - print intersection if visible to halfspace n, -n for not\n\
    QVn  - print intersections for halfspace n, -n if not\n\
    Qw   - allow option warnings\n\
    Q12  - allow wide facets and wide dupridge\n\
    Q14  - merge pinched vertices that create a dupridge\n\
\n\
T options:\n\
    TFn  - report summary when n or more facets created\n\
    TI file - input file, may be enclosed in single quotes\n\
    TO file - output file, may be enclosed in single quotes\n\
    Ts   - statistics\n\
    Tv   - verify result: structure, convexity, and in-circle test\n\
    Tz   - send all output to stdout\n\
\n\
";
char qh_promptc[]= "\
Trace options:\n\
    T4   - trace at level n, 4=all, 5=mem/gauss, -1= events\n\
    Ta   - annotate output with message codes\n\
    TAn  - stop qhull after adding n vertices\n\
     TCn - stop qhull after building cone for point n\n\
     TVn - stop qhull after adding point n, -n for before\n\
    Tc   - check frequently during execution\n\
    Tf   - flush each qh_fprintf for debugging segfaults\n\
    TPn - turn on tracing when point n added to hull\n\
    TMn  - turn on tracing at merge n\n\
    TWn  - trace merge facets when width > n\n\
\n\
Precision options:\n\
    Cn   - radius of centrum (roundoff added).  Merge facets if non-convex\n\
     An  - cosine of maximum angle.  Merge facets if cosine > n or non-convex\n\
           C-0 roundoff, A-0.99/C-0.01 pre-merge, A0.99/C0.01 post-merge\n\
    Rn   - randomly perturb computations by a factor of [1-n,1+n]\n\
    Un   - max distance below plane for a new, coplanar halfspace\n\
    Wn   - min facet width for outside halfspace (before roundoff)\n\
\n\
Output formats (may be combined; if none, produces a summary to stdout):\n\
    f    - facet dump\n\
    G    - Geomview output (dual convex hull)\n\
    i    - non-redundant halfspaces incident to each intersection\n\
    m    - Mathematica output (dual convex hull)\n\
    o    - OFF format (dual convex hull: dimension, points, and facets)\n\
    p    - vertex coordinates of dual convex hull (coplanars if 'Qc' or 'Qi')\n\
    s    - summary (stderr)\n\
\n\
";
char qh_promptd[]= "\
More formats:\n\
    Fc   - count plus redundant halfspaces for each intersection\n\
         -   Qc (default) for coplanar and Qi for other redundant\n\
    Fd   - use cdd format for input (homogeneous with offset first)\n\
    FF   - facet dump without ridges\n\
    FI   - ID of each intersection\n\
    Fm   - merge count for each intersection (511 max)\n\
    FM   - Maple output (dual 2-d or 3-d convex hull)\n\
    Fn   - count plus neighboring intersections for each intersection\n\
    FN   - count plus intersections for each halfspace\n\
    FO   - options and precision constants\n\
    Fp   - dim, count, and intersection coordinates\n\
    FP   - nearest halfspace and distance for each redundant halfspace\n\
    FQ   - command used for qhalf\n\
    Fs   - summary: #int (8), dim, #halfspaces, #non-redundant, #intersections\n\
                      output: #non-redundant, #intersections, #coplanar\n\
                                  halfspaces, #non-simplicial intersections\n\
                    #real (2), max outer plane, min vertex\n\
    Fv   - count plus non-redundant halfspaces for each intersection\n\
    Fx   - non-redundant halfspaces\n\
\n\
";
char qh_prompte[]= "\
Geomview output (2-d, 3-d and 4-d; dual convex hull)\n\
    Ga   - all points (i.e., transformed halfspaces) as dots\n\
     Gp  -  coplanar points and vertices as radii\n\
     Gv  -  vertices (i.e., non-redundant halfspaces) as spheres\n\
    Gc   - centrums\n\
    GDn  - drop dimension n in 3-d and 4-d output\n\
    Gh   - hyperplane intersections\n\
    Gi   - inner planes (i.e., halfspace intersections) only\n\
     Gn  -  no planes\n\
     Go  -  outer planes only\n\
    Gr   - ridges\n\
\n\
Print options:\n\
    PAn  - keep n largest facets (i.e., intersections) by area\n\
    Pdk:n- drop facet if normal[k] <= n (default 0.0)\n\
    PDk:n- drop facet if normal[k] >= n\n\
    PFn  - keep facets whose area is at least n\n\
    Pg   - print good facets (needs 'QGn' or 'QVn')\n\
    PG   - print neighbors of good facets\n\
    PMn  - keep n facets with most merges\n\
    Po   - force output.  If error, output neighborhood of facet\n\
    Pp   - do not report precision problems\n\
\n\
    .    - list of all options\n\
    -    - one line descriptions of all options\n\
    -?   - help with examples\n\
    -V   - version\n\
";
/* for opts, don't assign 'e' or 'E' to a flag (already used for exponent) */

/*-<a                             href="../libqhull_r/qh-qhull_r.htm#TOC"
  >-------------------------------</a><a name="prompt2">-</a>

  qh_prompt2
    synopsis for qhull
*/
char qh_prompt2[]= "\n\
qhalf -- halfspace intersection about a point.  Qhull %s\n\
    input (stdin): [dimension, 1, interior point]\n\
                       dimension+1, number of halfspaces, coefficients+offset\n\
    comments start with a non-numeric character\n\
\n\
options (qhalf.htm):\n\
    Hn,n - specify coordinates of interior point\n\
    Qt   - triangulated output\n\
    QJ   - joggled input instead of merged facets\n\
    Tv   - verify result: structure, convexity, and redundancy\n\
    .    - concise list of all options\n\
    -    - one-line description of each option\n\
    -?   - this message\n\
    -V   - version\n\
\n\
output options (subset):\n\
    s    - summary of results (default)\n\
    Fp   - intersection coordinates\n\
    Fv   - non-redundant halfspaces incident to each intersection\n\
    Fx   - non-redundant halfspaces\n\
    G    - Geomview output (dual convex hull)\n\
    m    - Mathematica output (dual convex hull)\n\
    o    - OFF file format (dual convex hull)\n\
    QVn  - print intersections for halfspace n, -n if not\n\
    TI file - input file, may be enclosed in single quotes\n\
    TO file - output file, may be enclosed in single quotes\n\
\n\
examples:\n\
    rbox d | qconvex FQ n | qhalf s H0,0,0 Fp\n\
    rbox c | qconvex FQ FV n | qhalf s i\n\
    rbox c | qconvex FQ FV n | qhalf s o\n\
\n\
";
/* for opts, don't assign 'e' or 'E' to a flag (already used for exponent) */

/*-<a                             href="../libqhull_r/qh-qhull_r.htm#TOC"
  >-------------------------------</a><a name="prompt3">-</a>

  qh_prompt3
    concise prompt for qhull
*/
char qh_prompt3[]= "\n\
Qhull %s\n\
Except for 'F.' and 'PG', upper_case options take an argument.\n\
\n\
 facet-dump     Geomview       H0,0-interior incidences      mathematica\n\
 off-format     point-dual     summary\n\
\n\
 Fc-redundant   Fd-cdd-in      FF-dump-xridge FIDs           Fmerges\n\
 FMaple         Fneighbors     FN-intersect   FOptions       Fp-coordinates\n\
 FP-nearest     FQhalf         Fsummary       Fv-halfspace   Fx-non-redundant\n\
\n\
 Gall-points    Gcentrums      GDrop-dim      Ghyperplanes   Ginner\n\
 Gno-planes     Gouter         Gpoints        Gridges        Gvertices\n\
\n\
 PArea-keep     Pdrop-d0:0D0   PFacet-area-keep  Pgood       PGood-neighbors\n\
 PMerge-keep    Poutput-forced Pprecision-not\n\
\n\
 Qallow-short   Qbk:0Bk:0-drop Qcoplanar      QG-half-good   Qi-redundant\n\
 QJoggle        QRotate        Qsearch-all    Qtriangulate   QVertex-good\n\
 Qwarn-allow    Q12-allow-wide Q14-merge-pinched\n\
\n\
 TFacet-log     TInput-file    TOutput-file   Tstatistics    Tverify\n\
 Tz-stdout\n\
\n\
 T4-trace       Tannotate      TAdd-stop      Tcheck-often   TCone-stop\n\
 Tflush         TMerge-trace   TPoint-trace   TVertex-stop   TWide-trace\n\
\n\
 Angle-max      Centrum-size   Random-dist    Ucoplanar-max  Wide-outside\n\
";

/*-<a                             href="../libqhull_r/qh-qhull_r.htm#TOC"
  >-------------------------------</a><a name="main">-</a>

  main( argc, argv )
    processes the command line, calls qhull() to do the work, and exits

  design:
    initializes data structures
    reads points
    finishes initialization
    computes convex hull and other structures
    checks the result
    writes the output
    frees memory
*/
int main(int argc, char *argv[]) {
  int curlong, totlong; /* used !qh_NOmem */
  int exitcode, numpoints, dim;
  coordT *points;
  boolT ismalloc;
  qhT qh_qh;
  qhT *qh= &qh_qh;

  QHULL_LIB_CHECK /* Check for compatible library */

  if ((argc == 1) && isatty( 0 /*stdin*/)) {
    fprintf(stdout, qh_prompt2, qh_version);
    exit(qh_ERRnone);
  }
  if (argc > 1 && *argv[1] == '-' && (*(argv[1] + 1) == '?' || *(argv[1] + 1) == '-')) { /* -? or --help */
    fprintf(stdout, qh_prompt2, qh_version);
    exit(qh_ERRnone);
  }
  if (argc > 1 && *argv[1] == '-' && !*(argv[1]+1)) {
    fprintf(stdout, qh_prompta, qh_version,
        qh_promptb, qh_promptc, qh_promptd, qh_prompte);
    exit(qh_ERRnone);
  }
  if (argc > 1 && *argv[1] == '.' && !*(argv[1]+1)) {
    fprintf(stdout, qh_prompt3, qh_version);
    exit(qh_ERRnone);
  }
  if (argc > 1 && *argv[1] == '-' && *(argv[1]+1)=='V') {
      fprintf(stdout, "%s\n", qh_version2);
      exit(qh_ERRnone);
  }
  qh_init_A(qh, stdin, stdout, stderr, argc, argv);  /* sets qh->qhull_command */
  exitcode= setjmp(qh->errexit); /* simple statement for CRAY J916 */
  if (!exitcode) {
    qh->NOerrexit = False;
    qh_option(qh, "Halfspace", NULL, NULL);
    qh->HALFspace= True;    /* 'H'   */
    qh_checkflags(qh, qh->qhull_command, hidden_options);
    qh_initflags(qh, qh->qhull_command);
    if (qh->SCALEinput) {
      fprintf(qh->ferr, "\
qhull error: options 'Qbk:n' and 'QBk:n' are not used with qhalf.\n\
             Use 'Qbk:0Bk:0 to drop dimension k.\n");
      qh_errexit(qh, qh_ERRinput, NULL, NULL);
    }
    points= qh_readpoints(qh, &numpoints, &dim, &ismalloc);
    qh_init_B(qh, points, numpoints, dim, ismalloc);
    qh_qhull(qh);
    qh_check_output(qh);
    qh_produce_output(qh);
    if (qh->VERIFYoutput && !qh->FORCEoutput && !qh->STOPpoint && !qh->STOPcone)
      qh_check_points(qh);
    exitcode= qh_ERRnone;
  }
  qh->NOerrexit= True;  /* no more setjmp */
#ifdef qh_NOmem
  qh_freeqhull(qh, qh_ALL);
#else
  qh_freeqhull(qh, !qh_ALL);
  qh_memfreeshort(qh, &curlong, &totlong);
  if (curlong || totlong)
    qh_fprintf_stderr(7079, "qhull internal warning (main): did not free %d bytes of long memory(%d pieces)\n",
       totlong, curlong);
#endif
  return exitcode;
} /* main */

