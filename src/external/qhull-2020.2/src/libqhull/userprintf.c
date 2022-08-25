/*<html><pre>  -<a                             href="qh-user.htm"
  >-------------------------------</a><a name="TOP">-</a>

  userprintf.c
  user redefinable function -- qh_fprintf

  see README.txt  see COPYING.txt for copyright information.

  If you recompile and load this file, then userprintf.o will not be loaded
  from qhull.a or qhull.lib

  See libqhull.h for data structures, macros, and user-callable functions.
  See user.c for qhull-related, redefinable functions
  see user.h for user-definable constants
  See usermem.c for qh_exit(), qh_free(), and qh_malloc()
  see Qhull.cpp and RboxPoints.cpp for examples.

  qh_printf is a good location for debugging traps, checked on each log line

  Please report any errors that you fix to qhull@qhull.org
*/

#include "libqhull.h"
#include "poly.h" /* for qh.tracefacet */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/*-<a                             href="qh-user.htm#TOC"
  >-------------------------------</a><a name="qh_fprintf">-</a>

  qh_fprintf( fp, msgcode, format, list of args )
    print arguments to *fp according to format
    Use qh_fprintf_rbox() for rboxlib.c

  notes:
    sets qh.last_errcode if msgcode is error 6000..6999
    same as fprintf()
    fgets() is not trapped like fprintf()
    exit qh_fprintf via qh_errexit()
    may be called for errors in qh_initstatistics and qh_meminit
*/

void qh_fprintf(FILE *fp, int msgcode, const char *fmt, ... ) {
  va_list args;
  facetT *neighbor, **neighborp;

  if (!fp) {
    /* could use qhmem.ferr, but probably better to be cautious */
    qh_fprintf_stderr(6028, "qhull internal error (userprintf.c): fp is 0.  Perhaps the wrong qh_fprintf was called.\n");
    qh last_errcode= 6028;
    qh_errexit(qh_ERRqhull, NULL, NULL);
  }
#if qh_QHpointer
  if ((qh_qh && qh ANNOTATEoutput) || msgcode < MSG_TRACE4) {
#else
  if (msgcode < MSG_TRACE4) {
#endif
    fprintf(fp, "[QH%.4d]", msgcode);
  }else if (msgcode >= MSG_ERROR && msgcode < MSG_STDERR ) {
    fprintf(fp, "QH%.4d ", msgcode);
  }
  va_start(args, fmt);
  vfprintf(fp, fmt, args);
  va_end(args);
    
#if qh_QHpointer
  if (qh_qh) {
#else
  {
#endif
    if (msgcode >= MSG_ERROR && msgcode < MSG_WARNING)
      qh last_errcode= msgcode;
    /* Place debugging traps here. Use with trace option 'Tn' 
       Set qh.tracefacet_id, qh.traceridge_id, and/or qh.tracevertex_id in global.c
    */
    if (False) { /* in production skip test for debugging traps */
      if (qh tracefacet && qh tracefacet->tested) {
        if (qh_setsize(qh tracefacet->neighbors) < qh hull_dim)
          qh_errexit(qh_ERRdebug, qh tracefacet, qh traceridge);
        FOREACHneighbor_(qh tracefacet) {
          if (neighbor != qh_DUPLICATEridge && neighbor != qh_MERGEridge && neighbor->visible)
            qh_errexit2(qh_ERRdebug, qh tracefacet, neighbor);
        }
      } 
      if (qh traceridge && qh traceridge->top->id == 234342223) {
        qh_errexit(qh_ERRdebug, qh tracefacet, qh traceridge);
      }
      if (qh tracevertex && qh_setsize(qh tracevertex->neighbors)>3434334) {
        qh_errexit(qh_ERRdebug, qh tracefacet, qh traceridge);
      }
    }
    if (qh FLUSHprint)
      fflush(fp);
  }
} /* qh_fprintf */

