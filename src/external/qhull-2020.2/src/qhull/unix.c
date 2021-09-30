/*<html><pre>  -<a                             href="../libqhull/qh-qhull.htm"
  >-------------------------------</a><a name="TOP">-</a>

   unix.c
     command line interface to qhull
         includes SIOUX interface for Macintoshes

   see qh-qhull.htm

   Copyright (c) 1993-2020 The Geometry Center.
   $Id: //main/2019/qhull/src/qhull/unix.c#4 $$Change: 2954 $
   $DateTime: 2020/05/21 22:30:09 $$Author: bbarber $
*/

#include "libqhull/libqhull.h"

#include <ctype.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(_MSC_VER)
#include <io.h>
#define isatty _isatty
/* int _isatty(int); */

#else
int isatty(int);  /* returns 1 if stdin is a tty
                   if "Undefined symbol" this can be deleted along with call in main() */
#endif

/*-<a                             href="../libqhull/qh-qhull.htm#TOC"
  >-------------------------------</a><a name="prompt">-</a>

  qh_prompt
    long prompt for qhull

  notes:
    see concise prompt below
    same text as qconvex.c, qdelanay.c, qhalf.c, qvoronoi.c
    limit maximum literal to 1800 characters
*/
char qh_prompta[]= "\n\
qhull -- compute convex hulls and related structures.\n\
    http://www.qhull.org  %s\n\
\n\
input (stdin):\n\
    first lines: dimension and number of points (or vice-versa).\n\
    other lines: point coordinates, best if one point per line\n\
    comments:    start with a non-numeric character\n\
    halfspaces:  use dim plus one and put offset after coefficients.\n\
                 May be preceded by a single interior point ('H').\n\
\n\
options:\n\
    d    - Delaunay triangulation by lifting points to a paraboloid\n\
    d Qu - furthest-site Delaunay triangulation (upper convex hull)\n\
    Hn,n,... - halfspace intersection about point [n,n,0,...]\n\
    Qc   - keep coplanar points with nearest facet\n\
    Qi   - keep interior points with nearest facet\n\
    QJ   - joggled input instead of merged facets\n\
    Qt   - triangulated output\n\
    v    - Voronoi diagram (dual of the Delaunay triangulation)\n\
    v Qu - furthest-site Voronoi diagram\n\
\n\
Qhull control options:\n\
    Qa   - allow input with fewer or more points than coordinates\n\
    Qbk:n   - scale coord k so that low bound is n\n\
      QBk:n - scale coord k so that upper bound is n (QBk is %2.2g)\n\
    QbB  - scale input to unit cube centered at the origin\n\
    Qbb  - scale last coordinate to [0,m] for Delaunay triangulations\n\
    Qbk:0Bk:0 - remove k-th coordinate from input\n\
    QJn  - randomly joggle input in range [-n,n]\n\
    QRn  - random rotation (n=seed, n=0 time, n=-1 time/no rotate)\n\
    Qs   - search all points for the initial simplex\n\
    Qu   - for 'd' or 'v', compute upper hull without point at-infinity\n\
              returns furthest-site Delaunay triangulation\n\
    QVn  - good facet if it includes point n, -n if not\n\
    Qx   - exact pre-merges (skips coplanar and angle-coplanar facets)\n\
    Qz   - add point-at-infinity to Delaunay triangulation\n\
\n\
%s%s%s%s";  /* split up qh_prompt for Visual C++ */
char qh_promptb[]= "\
Qhull extra options:\n\
    Qf   - partition point to furthest outside facet\n\
    Qg   - only build good facets (needs 'QGn', 'QVn', or 'PdD')\n\
    QGn  - good facet if visible from point n, -n for not visible\n\
    Qm   - only process points that would increase max_outside\n\
    Qr   - process random outside points instead of furthest ones\n\
    Qv   - test vertex neighbors for convexity\n\
    Qw   - allow option warnings\n\
    Q0   - turn off default premerge with 'C-0'/'Qx'\n\
    Q1   - merge by mergetype/angle instead of mergetype/distance\n\
    Q2   - merge all coplanar facets instead of merging independent sets\n\
    Q3   - do not merge redundant vertices\n\
    Q4   - avoid old->new merges\n\
    Q5   - do not correct outer planes at end of qhull\n\
    Q6   - do not pre-merge concave or coplanar facets\n\
    Q7   - depth-first processing instead of breadth-first\n\
    Q8   - do not process near-inside points\n\
    Q9   - process furthest of furthest points\n\
    Q10  - no special processing for narrow distributions\n\
    Q11  - copy normals and recompute centrums for tricoplanar facets\n\
    Q12  - allow wide facets and wide dupridge\n\
    Q14  - merge pinched vertices that create a dupridge\n\
    Q15  - check for duplicate ridges with the same vertices\n\
\n\
T options:\n\
    TFn  - report summary when n or more facets created\n\
    TI file - input file, may be enclosed in single quotes\n\
    TO file - output file, may be enclosed in single quotes\n\
    Ts   - statistics\n\
    Tv   - verify result: structure, convexity, and point inclusion\n\
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
     TP-1  turn on tracing after qh_buildhull and qh_postmerge\n\
     TMn - turn on tracing at merge n\n\
     TWn - trace merge facets when width > n\n\
    TRn  - rerun qhull n times for statistics to adjust 'QJn'\n\
\n\
Precision options:\n\
    Cn   - radius of centrum (roundoff added).  Merge facets if non-convex\n\
     An  - cosine of maximum angle.  Merge facets if cosine > n or non-convex\n\
           C-0 roundoff, A-0.99/C-0.01 pre-merge, A0.99/C0.01 post-merge\n\
    En   - max roundoff error for distance computation\n\
    Rn   - randomly perturb computations by a factor of [1-n,1+n]\n\
    Vn   - min distance above plane for a visible facet (default 3C-n or En)\n\
    Un   - max distance below plane for a new, coplanar point (default Vn)\n\
    Wn   - min facet width for outside point (before roundoff, default 2Vn)\n\
\n\
Output formats (may be combined; if none, produces a summary to stdout):\n\
    f    - facet dump\n\
    G    - Geomview output (see below)\n\
    i    - vertices incident to each facet\n\
    m    - Mathematica output (2-d and 3-d)\n\
    n    - normals with offsets\n\
    o    - OFF format (dim, points and facets; Voronoi regions)\n\
    p    - vertex coordinates or Voronoi vertices (coplanar points if 'Qc')\n\
    s    - summary (stderr)\n\
\n\
";
char qh_promptd[]= "\
More formats:\n\
    Fa   - area for each facet\n\
    FA   - compute total area and volume for option 's'\n\
    Fc   - count plus coplanar points for each facet\n\
           use 'Qc' (default) for coplanar and 'Qi' for interior\n\
    FC   - centrum or Voronoi center for each facet\n\
    Fd   - use cdd format for input (homogeneous with offset first)\n\
    FD   - use cdd format for numeric output (offset first)\n\
    FF   - facet dump without ridges\n\
    Fi   - inner plane for each facet\n\
           for 'v', separating hyperplanes for bounded Voronoi regions\n\
    FI   - ID of each facet\n\
    Fm   - merge count for each facet (511 max)\n\
    FM   - Maple output (2-d and 3-d)\n\
    Fn   - count plus neighboring facets for each facet\n\
    FN   - count plus neighboring facets for each point\n\
    Fo   - outer plane (or max_outside) for each facet\n\
           for 'v', separating hyperplanes for unbounded Voronoi regions\n\
    FO   - options and precision constants\n\
    Fp   - dim, count, and intersection coordinates (halfspace only)\n\
    FP   - nearest vertex and distance for each coplanar point\n\
    FQ   - command used for qhull\n\
    Fs   - summary: #int (8), dimension, #points, tot vertices, tot facets,\n\
                      output: #vertices, #facets, #coplanars, #nonsimplicial\n\
                    #real (2), max outer plane, min vertex\n\
    FS   - sizes:   #int (0)\n\
                    #real (2) tot area, tot volume\n\
    Ft   - triangulation with centrums for non-simplicial facets (OFF format)\n\
    Fv   - count plus vertices for each facet\n\
           for 'v', Voronoi diagram as Voronoi vertices for pairs of sites\n\
    FV   - average of vertices (a feasible point for 'H')\n\
    Fx   - extreme points (in order for 2-d)\n\
\n\
";
char qh_prompte[]= "\
Geomview output (2-d, 3-d, and 4-d; 2-d Voronoi)\n\
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
    Gt   - for 3-d 'd', transparent outer ridges\n\
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

/*-<a                             href="../libqhull/qh-qhull.htm#TOC"
  >-------------------------------</a><a name="prompt2">-</a>

  qh_prompt2
    synopsis for qhull

  notes:
    limit maximum literal to 1800 characters
*/
char qh_prompt2a[]= "\n\
qhull -- compute convex hulls and related structures.  Qhull %s\n\
    input (stdin): dimension, number of points, point coordinates\n\
    comments start with a non-numeric character\n\
    halfspace: use dim+1 and put offsets after coefficients\n\
\n\
options (qh-quick.htm):\n\
    d    - Delaunay triangulation by lifting points to a paraboloid\n\
    d Qu - furthest-site Delaunay triangulation (upper convex hull)\n\
    v    - Voronoi diagram as the dual of the Delaunay triangulation\n\
    v Qu - furthest-site Voronoi diagram\n\
    H1,1 - Halfspace intersection about [1,1,0,...] via polar duality\n\
    Qt   - triangulated output\n\
    QJ   - joggled input instead of merged facets\n\
    Tv   - verify result: structure, convexity, and point inclusion\n\
    .    - concise list of all options\n\
    -    - one-line description of each option\n\
    -?   - this message\n\
    -V   - version\n\
\n\
Output options (subset):\n\
    s    - summary of results (default)\n\
    i    - vertices incident to each facet\n\
    n    - normals with offsets\n\
    p    - vertex coordinates (if 'Qc', includes coplanar points)\n\
           if 'v', Voronoi vertices\n\
    FA   - report total area and volume\n\
    Fp   - halfspace intersections\n\
    FS   - total area and volume\n\
    Fx   - extreme points (convex hull vertices)\n\
    G    - Geomview output (2-d, 3-d and 4-d)\n\
    m    - Mathematica output (2-d and 3-d)\n\
    o    - OFF format (if 'v', outputs Voronoi regions)\n\
    QVn  - print facets that include point n, -n if not\n\
    TI file - input file, may be enclosed in single quotes\n\
    TO file - output file, may be enclosed in single quotes\n\
\n\
%s";  /* split literal */
char qh_prompt2b[]= "\
examples:\n\
    rbox D4 | qhull Tv                        rbox 1000 s | qhull Tv s FA\n\
    rbox 10 D2 | qhull d QJ s i TO result     rbox 10 D2 | qhull v Qbb Qt p\n\
    rbox 10 D2 | qhull d Qu QJ m              rbox 10 D2 | qhull v Qu QJ o\n\
    rbox c d D2 | qhull Qc s f Fx | more      rbox c | qhull FV n | qhull H Fp\n\
    rbox d D12 | qhull QR0 FA                 rbox c D7 | qhull FA TF1000\n\
    rbox y 1000 W0 | qhull Qc                 rbox c | qhull n\n\
\n\
";
/* for opts, don't assign 'e' or 'E' to a flag (already used for exponent) */

/*-<a                             href="../libqhull/qh-qhull.htm#TOC"
  >-------------------------------</a><a name="prompt3">-</a>

  qh_prompt3
    concise prompt for qhull

  notes:
    limit maximum literal to 1800 characters
*/
char qh_prompt3a[]= "\n\
Qhull %s\n\
Except for 'F.' and 'PG', upper-case options take an argument.\n\
\n\
 delaunay       facet-dump     Geomview       H0,0-interior  Halfspace\n\
 incidences     mathematica    normals        off-format     points\n\
 summary        voronoi\n\
\n\
 Farea          FArea-total    Fcoplanars     FCentrums      Fd-cdd-in\n\
 FD-cdd-out     FFacets-xridge Finner         FIDs           Fmerges\n\
 FMaple         Fneighbors     FNeigh-vertex  Fouter         FOptions\n\
 Fpoint-intersect  FPoint-near FQhull         Fsummary       FSize\n\
 Ftriangles     Fvertices      Fvoronoi       FVertex-ave    Fxtremes\n\
\n\
 Gall-points    Gcentrums      GDrop-dim      Ghyperplanes   Ginner\n\
 Gno-planes     Gouter         Gpoints        Gridges        Gtransparent\n\
 Gvertices\n\
\n\
 PArea-keep     Pdrop-d0:0D0   PFacet-area-keep  Pgood       PGood-neighbors\n\
 PMerge-keep    Poutput-forced Pprecision-not\n\
\n\
 Qallow-short   QbBound-0:0.5  QbB-scale-box  Qbb-scale-last Qbk:0Bk:0-drop\n\
 Qcoplanar      Qinterior      QJoggle        QRotate        Qsearch-all\n\
 Qtriangulate   QupperDelaunay Qwarn-allow    Qxact-merge    Qzinfinite\n\
\n\
 Qfurthest      Qgood-only     QGood-point    Qmax-outside   Qrandom\n\
 Qvneighbors    QVertex-good\n\
\n\
%s"; /* split literal */
char qh_prompt3b[]= "\
 Q0-no-premerge Q1-angle-merge     Q2-no-independ  Q3-no-redundant\n\
 Q4-no-old      Q5-no-check-out    Q6-no-concave   Q7-depth-first\n\
 Q8-no-near-in  Q9-pick-furthest   Q10-no-narrow   Q11-trinormals\n\
 Q12-allow-wide Q14-merge-pinched  Q15-duplicates\n\
\n\
 TFacet-log     TInput-file    TOutput-file   Tstatistics    Tverify\n\
 Tz-stdout\n\
\n\
 T4-trace       Tannotate      TAdd-stop      Tcheck-often   TCone-stop\n\
 Tflush         TMerge-trace   TPoint-trace   TRerun         TVertex-stop\n\
 TWide-trace\n\
\n\
 Angle-max      Centrum-size   Error-round    Random-dist    Ucoplanar-max\n\
 Visible-min    Wide-outside\n\
";

/*-<a                             href="../libqhull/qh-qhull.htm#TOC"
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

  QHULL_LIB_CHECK /* Check for compatible library */

  if ((argc == 1) && isatty( 0 /*stdin*/)) {
    fprintf(stdout, qh_prompt2a, qh_version, qh_prompt2b);
    exit(qh_ERRnone);
  }
  if (argc > 1 && *argv[1] == '-' && (*(argv[1] + 1) == '?' || *(argv[1] + 1) == '-')) { /* -? or --help */
    fprintf(stdout, qh_prompt2a, qh_version, qh_prompt2b);
    exit(qh_ERRnone);
  }
  if (argc > 1 && *argv[1] == '-' && (*(argv[1] + 1) == '?' || *(argv[1] + 1) == '-')) { /* -? or --help */
    fprintf(stdout, qh_prompt2a, qh_version, qh_prompt2b);
    exit(qh_ERRnone);
  }
  if (argc > 1 && *argv[1] == '-' && !*(argv[1]+1)) {
    fprintf(stdout, qh_prompta, qh_version, qh_DEFAULTbox,
                qh_promptb, qh_promptc, qh_promptd, qh_prompte);
    exit(qh_ERRnone);
  }
  if (argc > 1 && *argv[1] == '.' && !*(argv[1]+1)) {
    fprintf(stdout, qh_prompt3a, qh_version, qh_prompt3b);
    exit(qh_ERRnone);
  }
  if (argc > 1 && *argv[1] == '-' && *(argv[1]+1)=='V') {
      fprintf(stdout, "%s\n", qh_version2);
      exit(qh_ERRnone);
  }
  qh_init_A(stdin, stdout, stderr, argc, argv);  /* sets qh qhull_command */
  exitcode= setjmp(qh errexit); /* simple statement for CRAY J916 */
  if (!exitcode) {
    qh NOerrexit = False;
    qh_initflags(qh qhull_command);
    points= qh_readpoints(&numpoints, &dim, &ismalloc);
    qh_init_B(points, numpoints, dim, ismalloc);
    qh_qhull();
    qh_check_output();
    qh_produce_output();
    if (qh VERIFYoutput && !qh FORCEoutput && !qh STOPadd && !qh STOPcone && !qh STOPpoint)
      qh_check_points();
    exitcode= qh_ERRnone;
  }
  qh NOerrexit= True;  /* no more setjmp */
#ifdef qh_NOmem
  qh_freeqhull(qh_ALL);
#else
  qh_freeqhull(!qh_ALL);
  qh_memfreeshort(&curlong, &totlong);
  if (curlong || totlong)
    qh_fprintf_stderr(7079, "qhull internal warning (main): did not free %d bytes of long memory(%d pieces)\n",
       totlong, curlong);
#endif
  return exitcode;
} /* main */

