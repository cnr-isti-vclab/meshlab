/*<html><pre>  -<a                             href="../libqhull_r/qh-qhull_r.htm"
  >-------------------------------</a><a name="TOP">-</a>

   qconvex_r.c
      compute convex hulls using qhull

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
    long prompt for qconvex

  notes:
    restricted version of libqhull_r.c

  notes:
    same text as unix_r.c
    see concise prompt below
    limit maximum literal to 1800 characters
*/

/* duplicated in qconvex.htm */
char hidden_options[]=" d v H Qbb Qf Qg Qm Qr Qu Qv Qx Qz TR E V Fp Gt Q0 Q1 Q2 Q3 Q4 Q5 Q6 Q7 Q8 Q9 Q10 Q11 Q15 ";

char qh_prompta[]= "\n\
qconvex -- compute the convex hull\n\
    http://www.qhull.org  %s\n\
\n\
input (stdin):\n\
    first lines: dimension and number of points (or vice-versa).\n\
    other lines: point coordinates, best if one point per line\n\
    comments:    start with a non-numeric character\n\
\n\
options:\n\
    Qc   - keep coplanar points with nearest facet\n\
    Qi   - keep interior points with nearest facet\n\
    QJ   - joggled input instead of merged facets\n\
    Qt   - triangulated output\n\
\n\
Qhull control options:\n\
    Qa   - allow input with fewer or more points than coordinates\n\
    Qbk:n   - scale coord k so that low bound is n\n\
      QBk:n - scale coord k so that upper bound is n (QBk is %2.2g)\n\
    QbB  - scale input to unit cube centered at the origin\n\
    Qbk:0Bk:0 - remove k-th coordinate from input\n\
    QJn  - randomly joggle input in range [-n,n]\n\
    QRn  - random rotation (n=seed, n=0 time, n=-1 time/no rotate)\n\
    Qs   - search all points for the initial simplex\n\
\n\
%s%s%s%s";  /* split up qh_prompt for Visual C++ */
char qh_promptb[]= "\
Qhull extra options:\n\
    QGn  - good facet if visible from point n, -n for not visible\n\
    QVn  - good facet if it includes point n, -n if not\n\
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
    TPn  - turn on tracing when point n added to hull\n\
     TMn - turn on tracing at merge n\n\
     TWn - trace merge facets when width > n\n\
\n\
Precision options:\n\
    Cn   - radius of centrum (roundoff added).  Merge facets if non-convex\n\
     An  - cosine of maximum angle.  Merge facets if cosine > n or non-convex\n\
           C-0 roundoff, A-0.99/C-0.01 pre-merge, A0.99/C0.01 post-merge\n\
    Rn   - randomly perturb computations by a factor of [1-n,1+n]\n\
    Un   - max distance below plane for a new, coplanar point\n\
    Wn   - min facet width for outside point (before roundoff)\n\
\n\
Output formats (may be combined; if none, produces a summary to stdout):\n\
    f    - facet dump\n\
    G    - Geomview output (see below)\n\
    i    - vertices incident to each facet\n\
    m    - Mathematica output (2-d and 3-d)\n\
    n    - normals with offsets\n\
    o    - OFF file format (dim, points and facets)\n\
    p    - point coordinates \n\
    s    - summary (stderr)\n\
\n\
";
char qh_promptd[]= "\
More formats:\n\
    Fa   - area for each facet\n\
    FA   - compute total area and volume for option 's'\n\
    Fc   - count plus coplanar points for each facet\n\
           use 'Qc' (default) for coplanar and 'Qi' for interior\n\
    FC   - centrum for each facet\n\
    Fd   - use cdd format for input (homogeneous with offset first)\n\
    FD   - use cdd format for numeric output (offset first)\n\
    FF   - facet dump without ridges\n\
    Fi   - inner plane for each facet\n\
    FI   - ID for each facet\n\
    Fm   - merge count for each facet (511 max)\n\
    FM   - Maple output (2-d and 3-d)\n\
    Fn   - count plus neighboring facets for each facet\n\
    FN   - count plus neighboring facets for each point\n\
    Fo   - outer plane (or max_outside) for each facet\n\
    FO   - options and precision constants\n\
    FP   - nearest vertex for each coplanar point\n\
    FQ   - command used for qconvex\n\
    Fs   - summary: #int (8), dimension, #points, tot vertices, tot facets,\n\
                      output: #vertices, #facets, #coplanars, #nonsimplicial\n\
                    #real (2), max outer plane, min vertex\n\
    FS   - sizes:   #int (0) \n\
                    #real (2) tot area, tot volume\n\
    Ft   - triangulation with centrums for non-simplicial facets (OFF format)\n\
    Fv   - count plus vertices for each facet\n\
    FV   - average of vertices (a feasible point for 'H')\n\
    Fx   - extreme points (in order for 2-d)\n\
\n\
";
char qh_prompte[]= "\
Geomview output (2-d, 3-d, and 4-d)\n\
    Ga   - all points as dots\n\
     Gp  -  coplanar points and vertices as radii\n\
     Gv  -  vertices as spheres\n\
    Gc   - centrums\n\
    GDn  - drop dimension n in 3-d and 4-d output\n\
    Gh   - hyperplane intersections\n\
    Gi   - inner planes only\n\
     Gn  -  no planes\n\
     Go  -  outer planes only\n\
    Gr   - ridges\n\
\n\
Print options:\n\
    PAn  - keep n largest facets by area\n\
    Pdk:n - drop facet if normal[k] <= n (default 0.0)\n\
    PDk:n - drop facet if normal[k] >= n\n\
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
qconvex -- compute the convex hull.  Qhull %s\n\
    input (stdin): dimension, number of points, point coordinates\n\
    comments start with a non-numeric character\n\
\n\
options (qconvex.htm):\n\
    Qt   - triangulated output\n\
    QJ   - joggled input instead of merged facets\n\
    Tv   - verify result: structure, convexity, and point inclusion\n\
    .    - concise list of all options\n\
    -    - one-line description of each option\n\
    -?   - this message\n\
    -V   - version\n\
\n\
output options (subset):\n\
    s    - summary of results (default)\n\
    i    - vertices incident to each facet\n\
    n    - normals with offsets\n\
    p    - vertex coordinates (if 'Qc', includes coplanar points)\n\
    FA   - report total area and volume\n\
    FS   - total area and volume\n\
    Fx   - extreme points (convex hull vertices)\n\
    G    - Geomview output (2-d, 3-d, and 4-d)\n\
    m    - Mathematica output (2-d and 3-d)\n\
    o    - OFF format (dim, n, points, facets)\n\
    QVn  - print facets that include point n, -n if not\n\
    TI file - input file, may be enclosed in single quotes\n\
    TO file - output file, may be enclosed in single quotes\n\
\n\
examples:\n\
    rbox c D2 | qconvex s n                    rbox c D2 | qconvex i\n\
    rbox c D2 | qconvex o                      rbox 1000 s | qconvex s Tv FA\n\
    rbox c d D2 | qconvex s Qc Fx              rbox y 1000 W0 | qconvex Qc s n\n\
    rbox y 1000 W0 | qconvex s QJ              rbox d G1 D12 | qconvex QR0 FA Pp\n\
    rbox c D7 | qconvex FA TF1000\n\
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
Except for 'F.' and 'PG', upper-case options take an argument.\n\
\n\
 facet-dump     Geomview       incidences     mathematica    normals\n\
 off-format     points         summary\n\
\n\
 Farea          FArea-total    Fcoplanars     FCentrums      Fd-cdd-in\n\
 FD-cdd-out     FFacets-xridge Finner         FIDs           Fmerges\n\
 FMaple         Fneighbors     FNeigh-vertex  Fouter         FOptions\n\
 FPoint-near    FQhull         Fsummary       FSize          Ftriangles\n\
 Fvertices      FVertex-ave    Fxtremes\n\
\n\
 Gall-points    Gcentrums      GDrop-dim      Ghyperplanes   Ginner\n\
 Gno-planes     Gouter         Gpoints        Gridges        Gvertices\n\
\n\
 PArea-keep     Pdrop-d0:0D0   PFacet-area-keep  Pgood       PGood-neighbors\n\
 PMerge-keep    Poutput-forced Pprecision-not\n\
\n\
 Qallow-short   QbBound-0:0.5  QbB-scale-box  Qcoplanar      QGood-point\n\
 Qinterior      QJoggle        QRotate        Qsearch-all    Qtriangulate\n\
 QVertex-good   Qwarn-allow    Q12-allow-wide Q14-merge-pinched\n\
\n\
 TFacet-log     TInput-file    TOutput-file   Tstatistics    Tverify\n\
 Tz-stdout\n\
\n\
 T4-trace       Tannotate      TAdd-stop      Tcheck-often   TCone-stop\n\
 Tflush         TMerge-trace   TPoint-trace   TVertex-stop   TWide-trace\n\
\n\
 Angle-max      Centrum-size   Random-dist    Ucoplanar-max  Wide-outside\n\
";

/*-<a                             href="../libqhull_r/qh-qhull_r.htm"
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
    fprintf(stdout, qh_prompta, qh_version, qh_DEFAULTbox,
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
    qh_checkflags(qh, qh->qhull_command, hidden_options);
    qh_initflags(qh, qh->qhull_command);
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

