/*********************************************************************/
/*                                                                   */
/*             Optimized BLAS libraries                              */
/*                     By Kazushige Goto <kgoto@tacc.utexas.edu>     */
/*                                                                   */
/* Copyright (c) The University of Texas, 2009. All rights reserved. */
/* UNIVERSITY EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES CONCERNING  */
/* THIS SOFTWARE AND DOCUMENTATION, INCLUDING ANY WARRANTIES OF      */
/* MERCHANTABILITY, FITNESS FOR ANY PARTICULAR PURPOSE,              */
/* NON-INFRINGEMENT AND WARRANTIES OF PERFORMANCE, AND ANY WARRANTY  */
/* THAT MIGHT OTHERWISE ARISE FROM COURSE OF DEALING OR USAGE OF     */
/* TRADE. NO WARRANTY IS EITHER EXPRESS OR IMPLIED WITH RESPECT TO   */
/* THE USE OF THE SOFTWARE OR DOCUMENTATION.                         */
/* Under no circumstances shall University be liable for incidental, */
/* special, indirect, direct or consequential damages or loss of     */
/* profits, interruption of business, or related expenses which may  */
/* arise from use of Software or Documentation, including but not    */
/* limited to those resulting from defects in Software and/or        */
/* Documentation, or loss or inaccuracy of data of any kind.         */
/*********************************************************************/

#include <stdio.h>
#include "common.h"
#ifdef FUNCTION_PROFILE
#include "functable.h"
#endif

#undef MAX_K

#ifdef USE_ABS

#ifndef USE_MIN

/* ABS & MAX */
#ifndef COMPLEX
#ifdef XDOUBLE
#define MAX_K	QAMAX_K
#elif defined(DOUBLE)
#define MAX_K	DAMAX_K
#else
#define MAX_K	SAMAX_K
#endif
#else
#ifdef XDOUBLE
#define MAX_K	XAMAX_K
#elif defined(DOUBLE)
#define MAX_K	ZAMAX_K
#else
#define MAX_K	CAMAX_K
#endif
#endif

#else

/* ABS & MIN */
#ifndef COMPLEX
#ifdef XDOUBLE
#define MAX_K	QAMIN_K
#elif defined(DOUBLE)
#define MAX_K	DAMIN_K
#else
#define MAX_K	SAMIN_K
#endif
#else
#ifdef XDOUBLE
#define MAX_K	XAMIN_K
#elif defined(DOUBLE)
#define MAX_K	ZAMIN_K
#else
#define MAX_K	CAMIN_K
#endif
#endif

#endif

#else

#ifndef USE_MIN

/* MAX */
#ifdef XDOUBLE
#define MAX_K	QMAX_K
#elif defined(DOUBLE)
#define MAX_K	DMAX_K
#else
#define MAX_K	SMAX_K
#endif

#else

/* MIN */
#ifdef XDOUBLE
#define MAX_K	QMIN_K
#elif defined(DOUBLE)
#define MAX_K	DMIN_K
#else
#define MAX_K	SMIN_K
#endif

#endif

#endif

#ifndef CBLAS

FLOATRET NAME(blasint *N, FLOAT *x, blasint *INCX){
  
  BLASLONG n    = *N;
  BLASLONG incx = *INCX;
  FLOATRET ret;

  PRINT_DEBUG_NAME;

  if (n <= 0) return 0;

  IDEBUG_START;

  FUNCTION_PROFILE_START();

  ret = (FLOATRET)MAX_K(n, x, incx);

  FUNCTION_PROFILE_END(COMPSIZE, n, 0);

  IDEBUG_END;

  return ret;
}

#else

FLOAT CNAME(blasint n, FLOAT *x, blasint incx){
  
  FLOAT ret;

  PRINT_DEBUG_CNAME;

  if (n <= 0) return 0;

  IDEBUG_START;

  FUNCTION_PROFILE_START();

  ret = MAX_K(n, x, incx);

  FUNCTION_PROFILE_END(COMPSIZE, n, 0);

  IDEBUG_END;

  return ret;
}

#endif
