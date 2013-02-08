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

#ifdef RETURN_BY_STRUCT
#ifdef XDOUBLE
#define MYTYPE myxcomplex_t
#elif defined DOUBLE
#define MYTYPE myzcomplex_t
#else
#define MYTYPE myccomplex_t
#endif
#endif

#ifndef CBLAS

#ifdef RETURN_BY_STRUCT
MYTYPE         NAME(                        blasint *N, FLOAT *x, blasint *INCX, FLOAT *y, blasint *INCY) {
#elif defined RETURN_BY_STACK
void           NAME(FLOAT _Complex *result, blasint *N, FLOAT *x, blasint *INCX, FLOAT *y, blasint *INCY) {
#else
FLOAT _Complex NAME(                        blasint *N, FLOAT *x, blasint *INCX, FLOAT *y, blasint *INCY) {
#endif

  BLASLONG n    = *N;
  BLASLONG incx = *INCX;
  BLASLONG incy = *INCY;
#ifndef RETURN_BY_STACK
  FLOAT _Complex ret;
#endif
#ifdef RETURN_BY_STRUCT
  MYTYPE  myret;
#endif

  PRINT_DEBUG_NAME;

  if (n <= 0) {
#ifdef RETURN_BY_STRUCT
    myret.r = 0.;
    myret.i = 0.;
    return myret;
#elif defined RETURN_BY_STACK
    *result = ZERO;
    return;
#else
    return ZERO;
#endif
  }

  IDEBUG_START;

  FUNCTION_PROFILE_START();

  if (incx < 0) x -= (n - 1) * incx * 2;
  if (incy < 0) y -= (n - 1) * incy * 2;

#ifdef RETURN_BY_STRUCT

#ifndef CONJ
  ret = DOTU_K(n, x, incx, y, incy);
#else
  ret = DOTC_K(n, x, incx, y, incy);
#endif

  myret.r = CREAL ret;
  myret.i = CIMAG ret;

  FUNCTION_PROFILE_END(4, 2 * n, 2 * n);

  IDEBUG_END;

  return myret;

#elif defined RETURN_BY_STACK

#ifndef CONJ
  *result = DOTU_K(n, x, incx, y, incy);
#else
  *result = DOTC_K(n, x, incx, y, incy);
#endif

  FUNCTION_PROFILE_END(4, 2 * n, 2 * n);

  IDEBUG_END;

#else

#ifndef CONJ
  ret = DOTU_K(n, x, incx, y, incy);
#else
  ret = DOTC_K(n, x, incx, y, incy);
#endif

  FUNCTION_PROFILE_END(4, 2 * n, 2 * n);

  IDEBUG_END;

  return ret;

#endif

}

#else

#ifdef FORCE_USE_STACK
void           CNAME(blasint n, FLOAT *x, blasint incx, FLOAT *y, blasint incy, FLOAT _Complex *result){
#else
FLOAT _Complex CNAME(blasint n, FLOAT *x, blasint incx, FLOAT *y, blasint incy){

  FLOAT _Complex ret;
#endif

  PRINT_DEBUG_CNAME;

  if (n <= 0) {
#ifdef FORCE_USE_STACK
    *result = ZERO;
    return;
#else
    return ZERO;
#endif
  }

  if (incx < 0) x -= (n - 1) * incx * 2;
  if (incy < 0) y -= (n - 1) * incy * 2;

  IDEBUG_START;

  FUNCTION_PROFILE_START();

#ifdef FORCE_USE_STACK

#ifndef CONJ
  *result = DOTU_K(n, x, incx, y, incy);
#else
  *result = DOTC_K(n, x, incx, y, incy);
#endif

  FUNCTION_PROFILE_END(4, 2 * n, 2 * n);

  IDEBUG_END;

#else

#ifndef CONJ
  ret = DOTU_K(n, x, incx, y, incy);
#else
  ret = DOTC_K(n, x, incx, y, incy);
#endif

  FUNCTION_PROFILE_END(4, 2 * n, 2 * n);

  IDEBUG_END;

  return ret;

#endif

}

#endif
