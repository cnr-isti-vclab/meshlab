/*<html><pre>  -<a                             href="../libqhull_r/qh-qhull_r.htm"
  >-------------------------------</a><a name="TOP">-</a>

   qvoronoi_r.c
     compute Voronoi diagrams and furthest-point Voronoi
     diagrams using qhull

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

/* duplicated in qvoron_f.htm and qvoronoi.htm
   QJ and Qt are deprecated, but allowed for backwards compatibility
*/
char hidden_options[]=" d n m v H U Qb QB Qc Qf Qg Qi Qm Qr Qv Qx TR E V Fa FA FC FM Fp FS Ft FV Gt Pv Q0 Q1 Q2 Q3 Q4 Q5 Q6 Q7 Q8 Q9 Q10 Q11 Q15 ";

char qh_prompta[]= "\n\
qvoronoi -- compute the Voronoi diagram\n\
    http://www.qhull.org  %s\n\
\n\
input (stdin):\n\
    first lines: dimension and number of points (or vice-versa).\n\
    other lines: point coordinates, best if one point per line\n\
    comments:    start with a non-numeric character\n\
\n\
options:\n\
    Qu   - compute furthest-site Voronoi diagram\n\
\n\
Qhull control options:\n\
    Qa   - allow input with fewer or more points than coordinates\n\
    QRn  - random rotation (n=seed, n=0 time, n=-1 time/no rotate)\n\
    Qs   - search all points for the initial simplex\n\
    Qz   - add point-at-infinity to Voronoi diagram\n\
%s%s%s%s";  /* split up qh_prompt for Visual C++ */
char qh_promptb[]= "\
\n\
Qhull extra options:\n\
    QGn  - Voronoi vertices if visible from point n, -n if not\n\
    QVn  - Voronoi vertices for input point n, -n if not\n\
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
    Wn   - min facet width for non-coincident point (before roundoff)\n\
\n\
Output formats (may be combined; if none, summary to stdout):\n\
    p    - Voronoi vertices\n\
    s    - summary to stderr\n\
    f    - facet dump\n\
    i    - Delaunay regions (use 'Pp' to avoid warning)\n\
    o    - OFF format (dim, Voronoi vertices, and Voronoi regions)\n\
\n\
";
char qh_promptd[]= "\
More formats:\n\
    Fc   - count plus coincident points (by Voronoi vertex)\n\
    Fd   - use cdd format for input (homogeneous with offset first)\n\
    FD   - use cdd format for output (offset first)\n\
    FF   - facet dump without ridges\n\
    Fi   - separating hyperplanes for bounded Voronoi regions\n\
    FI   - ID for each Voronoi vertex\n\
    Fm   - merge count for each Voronoi vertex (511 max)\n\
    Fn   - count plus neighboring Voronoi vertices for each Voronoi vertex\n\
    FN   - count and Voronoi vertices for each Voronoi region\n\
    Fo   - separating hyperplanes for unbounded Voronoi regions\n\
    FO   - options and precision constants\n\
    FP   - nearest point and distance for each coincident point\n\
    FQ   - command used for qvoronoi\n\
    Fs   - summary: #int (8), dimension, #points, tot vertices, tot facets,\n\
                    for output: #Voronoi regions, #Voronoi vertices,\n\
                                #coincident points, #non-simplicial regions\n\
                    #real (2), max outer plane and min vertex\n\
    Fv   - Voronoi diagram as Voronoi vertices between adjacent input sites\n\
    Fx   - extreme points of Delaunay triangulation (on convex hull)\n\
\n\
";
char qh_prompte[]= "\
Geomview output (2-d only)\n\
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
    PAn  - keep n largest Voronoi vertices by 'area'\n\
    Pdk:n - drop facet if normal[k] <= n (default 0.0)\n\
    PDk:n - drop facet if normal[k] >= n\n\
    PFn  - keep Voronoi vertices whose 'area' is at least n\n\
    Pg   - print good Voronoi vertices (needs 'QGn' or 'QVn')\n\
    PG   - print neighbors of good Voronoi vertices\n\
    PMn  - keep n Voronoi vertices with most merges\n\
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
qvoronoi -- compute the Voronoi diagram.  Qhull %s\n\
    input (stdin): dimension, number of points, point coordinates\n\
    comments start with a non-numeric character\n\
\n\
options (qvoronoi.htm):\n\
    Qu   - compute furthest-site Voronoi diagram\n\
    Tv   - verify result: structure, convexity, and in-circle test\n\
    .    - concise list of all options\n\
    -    - one-line description of all options\n\
    -?   - this message\n\
    -V   - version\n\
\n\
output options (subset):\n\
    Fi   - separating hyperplanes for bounded regions, 'Fo' for unbounded\n\
    FN   - count and Voronoi vertices for each Voronoi region\n\
    Fv   - Voronoi diagram as Voronoi vertices between adjacent input sites\n\
    G    - Geomview output (2-d only)\n\
    o    - OFF file format (dim, Voronoi vertices, and Voronoi regions)\n\
    p    - Voronoi vertices\n\
    QVn  - Voronoi vertices for input point n, -n if not\n\
    s    - summary of results (default)\n\
    TI file - input file, may be enclosed in single quotes\n\
    TO file - output file, may be enclosed in single quotes\n\
\n\
examples:\n\
    rbox c P0 D2 | qvoronoi s o         rbox c P0 D2 | qvoronoi Fi\n\
    rbox c P0 D2 | qvoronoi Fo          rbox c P0 D2 | qvoronoi Fv\n\
    rbox c P0 D2 | qvoronoi s Qu Fv     rbox c P0 D2 | qvoronoi Qu Fo\n\
    rbox c G1 d D2 | qvoronoi s p       rbox c P0 D2 | qvoronoi s Fv QV0\n\
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
 facet-dump     Geomview       i-delaunay     off-format     p-vertices\n\
 summary\n\
\n\
 Fcoincident    Fd-cdd-in      FD-cdd-out     FF-dump-xridge Fi-bounded\n\
 FIDs           Fmerges        Fneighbors     FNeigh-region  Fo-unbounded\n\
 FOptions       FPoint-near    FQvoronoi      Fsummary       Fvoronoi\n\
 Fxtremes\n\
\n\
 Gall-points    Gcentrums      GDrop-dim      Ghyperplanes   Ginner\n\
 Gno-planes     Gouter         Gpoints        Gridges        Gvertices\n\
\n\
 PArea-keep     Pdrop-d0:0D0   PFacet-area-keep  Pgood       PGood-neighbors\n\
 PMerge-keep    Poutput-forced Pprecision-not\n\
\n\
 Qallow-short   QG-vertex-good QRotate        Qsearch-all    Qupper-voronoi\n\
 QV-point-good  Qwarn-allow    Qzinfinite     Q12-allow-wide Q14-merge-pinched\n\
\n\
 TFacet-log     TInput-file    TOutput-file   Tstatistics    Tverify\n\
 Tz-stdout\n\
\n\
 T4-trace       Tannotate      TAdd-stop      Tcheck-often   TCone-stop\n\
 Tflush         TMerge-trace   TPoint-trace   TVertex-stop   TWide-trace\n\
\n\
 Angle-max      Centrum-size   Random-dist    Wide-outside\n\
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
    qh->NOerrexit= False;
    qh_option(qh, "voronoi  _bbound-last  _coplanar-keep", NULL, NULL);
    qh->DELAUNAY= True;     /* 'v'   */
    qh->VORONOI= True;
    qh->SCALElast= True;    /* 'Qbb' */
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

