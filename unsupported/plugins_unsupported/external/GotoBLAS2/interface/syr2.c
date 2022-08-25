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
#include <ctype.h>
#include "common.h"
#ifdef FUNCTION_PROFILE
#include "functable.h"
#endif

#ifdef XDOUBLE
#define ERROR_NAME "QSYR2 "
#elif defined(DOUBLE)
#define ERROR_NAME "DSYR2 "
#else
#define ERROR_NAME "SSYR2 "
#endif

static int (*syr2[])(BLASLONG, FLOAT, FLOAT *, BLASLONG, FLOAT *, BLASLONG, FLOAT *, BLASLONG, FLOAT *) = {
#ifdef XDOUBLE
  qsyr2_U, qsyr2_L,
#elif defined(DOUBLE)
  dsyr2_U, dsyr2_L,
#else
  ssyr2_U, ssyr2_L,
#endif
};

#ifdef SMP
static int (*syr2_thread[])(BLASLONG, FLOAT, FLOAT *, BLASLONG, FLOAT *, BLASLONG, FLOAT *, BLASLONG, FLOAT *, int) = {
#ifdef XDOUBLE
  qsyr2_thread_U, qsyr2_thread_L,
#elif defined(DOUBLE)
  dsyr2_thread_U, dsyr2_thread_L,
#else
  ssyr2_thread_U, ssyr2_thread_L,
#endif
};
#endif

#ifndef CBLAS

void NAME(char *UPLO, blasint *N, FLOAT  *ALPHA, 
	 FLOAT  *x, blasint *INCX, FLOAT *y, blasint *INCY, FLOAT *a, blasint *LDA){

  char uplo_arg = *UPLO;
  blasint n		= *N;
  FLOAT alpha	= *ALPHA;
  blasint lda	= *LDA;
  blasint incx	= *INCX;
  blasint incy	= *INCY;

  blasint info;
  int uplo;
  FLOAT *buffer;
#ifdef SMP
  int nthreads;
#endif

  PRINT_DEBUG_NAME;

  TOUPPER(uplo_arg);
  uplo  = -1;

  if (uplo_arg  == 'U') uplo  = 0;
  if (uplo_arg  == 'L') uplo  = 1;
 
  info = 0;

  if (lda  < MAX(1, n))   info =  9;
  if (incy == 0)          info =  7;
  if (incx == 0)          info =  5;
  if (n < 0)              info =  2;
  if (uplo  < 0)          info =  1;

  if (info != 0) {
    BLASFUNC(xerbla)(ERROR_NAME, &info, sizeof(ERROR_NAME));
    return;
  }

#else

void CNAME(enum CBLAS_ORDER order, enum CBLAS_UPLO Uplo, blasint n, FLOAT alpha, FLOAT *x, blasint incx, FLOAT *y, blasint incy, FLOAT *a, blasint lda) {

  FLOAT *buffer;
  int trans, uplo;
  blasint info;
#ifdef SMP
  int nthreads;
#endif

  PRINT_DEBUG_CNAME;

  trans = -1;
  uplo  = -1;
  info  =  0;

  if (order == CblasColMajor) {

    if (Uplo == CblasUpper) uplo  = 0;
    if (Uplo == CblasLower) uplo  = 1;

    info = -1;

    if (lda  < MAX(1, n))   info =  9;
    if (incy == 0)          info =  7;
    if (incx == 0)          info =  5;
    if (n < 0)              info =  2;
    if (uplo  < 0)          info =  1;
  }

  if (order == CblasRowMajor) {

    if (Uplo == CblasUpper) uplo  = 1;
    if (Uplo == CblasLower) uplo  = 0;

    info = -1;

    if (lda  < MAX(1, n))   info =  9;
    if (incy == 0)          info =  7;
    if (incx == 0)          info =  5;
    if (n < 0)              info =  2;
    if (uplo  < 0)          info =  1;
  }

  if (info >= 0) {
    BLASFUNC(xerbla)(ERROR_NAME, &info, sizeof(ERROR_NAME));
    return;
  }

#endif
  
  if (n == 0) return;

  if (alpha == ZERO) return;

  IDEBUG_START;

  FUNCTION_PROFILE_START();

  if (incx < 0 ) x -= (n - 1) * incx;
  if (incy < 0 ) y -= (n - 1) * incy;

  buffer = (FLOAT *)blas_memory_alloc(1);

#ifdef SMP
  nthreads = num_cpu_avail(2);

  if (nthreads == 1) {
#endif

    (syr2[uplo])(n, alpha, x, incx, y, incy, a, lda, buffer);

#ifdef SMP
  } else {
    
    (syr2_thread[uplo])(n, alpha, x, incx, y, incy, a, lda, buffer, nthreads);

  }
#endif

  blas_memory_free(buffer);

  FUNCTION_PROFILE_END(1, n * n / 2 + 2 * n, 2 * n * n);

  IDEBUG_END;

  return;
}
