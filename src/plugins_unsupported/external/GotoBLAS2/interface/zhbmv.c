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
#define ERROR_NAME "XHBMV "
#elif defined(DOUBLE)
#define ERROR_NAME "ZHBMV "
#else
#define ERROR_NAME "CHBMV "
#endif

static  int (*hbmv[])(BLASLONG, BLASLONG, FLOAT, FLOAT, FLOAT *, BLASLONG, FLOAT *, BLASLONG, FLOAT *, BLASLONG, void *) = {
#ifdef XDOUBLE
  xhbmv_U, xhbmv_L, xhbmv_V, xhbmv_M,
#elif defined(DOUBLE)
  zhbmv_U, zhbmv_L, zhbmv_V, zhbmv_M,
#else
  chbmv_U, chbmv_L, chbmv_V, chbmv_M,
#endif
};

#ifdef SMP
static  int (*hbmv_thread[])(BLASLONG, BLASLONG, FLOAT *, FLOAT *, BLASLONG, FLOAT *, BLASLONG, FLOAT *, BLASLONG, FLOAT *, int) = {
#ifdef XDOUBLE
  xhbmv_thread_U, xhbmv_thread_L, xhbmv_thread_V, xhbmv_thread_M,
#elif defined(DOUBLE)
  zhbmv_thread_U, zhbmv_thread_L, zhbmv_thread_V, zhbmv_thread_M,
#else
  chbmv_thread_U, chbmv_thread_L, chbmv_thread_V, chbmv_thread_M,
#endif
};
#endif

#ifndef CBLAS

void NAME(char *UPLO, blasint *N, blasint *K, FLOAT  *ALPHA, FLOAT *a, blasint *LDA, 
            FLOAT  *x, blasint *INCX, FLOAT *BETA, FLOAT *y, blasint *INCY){

  char uplo_arg = *UPLO;
  blasint n	= *N;
  blasint k	= *K;
  FLOAT alpha_r	= ALPHA[0];
  FLOAT alpha_i	= ALPHA[1];
  blasint lda	= *LDA;
  blasint incx	= *INCX;
  FLOAT beta_r	= BETA[0];
  FLOAT beta_i	= BETA[1];
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
  if (uplo_arg  == 'V') uplo  = 2;
  if (uplo_arg  == 'M') uplo  = 3;
 
  info = 0;

  if (incy == 0)          info = 11;
  if (incx == 0)          info =  8;
  if (lda  < k + 1)       info =  6;
  if (k < 0)              info =  3;
  if (n < 0)              info =  2;
  if (uplo  < 0)          info =  1;

  if (info != 0) {
    BLASFUNC(xerbla)(ERROR_NAME, &info, sizeof(ERROR_NAME));
    return;
  }
  
#else

void CNAME(enum CBLAS_ORDER order,
	   enum CBLAS_UPLO Uplo,
	   blasint n, blasint k,
	   FLOAT *ALPHA,
	   FLOAT  *a, blasint lda,
	   FLOAT  *x, blasint incx,
	   FLOAT *BETA,
	   FLOAT  *y, blasint incy){

  FLOAT alpha_r	= ALPHA[0];
  FLOAT alpha_i	= ALPHA[1];
  FLOAT beta_r	= BETA[0];
  FLOAT beta_i	= BETA[1];
  FLOAT *buffer;
  int uplo;
  blasint info;
#ifdef SMP
  int nthreads;
#endif

  PRINT_DEBUG_CNAME;

  uplo  = -1;
  info  =  0;

  if (order == CblasColMajor) {
    if (Uplo == CblasUpper)         uplo  = 0;
    if (Uplo == CblasLower)         uplo  = 1;
    
    info = -1;

    if (incy == 0)          info = 11;
    if (incx == 0)          info =  8;
    if (lda  < k + 1)       info =  6;
    if (k < 0)              info =  3;
    if (n < 0)              info =  2;
    if (uplo  < 0)          info =  1;
  }

  if (order == CblasRowMajor) {
    if (Uplo == CblasUpper)         uplo  = 3;
    if (Uplo == CblasLower)         uplo  = 2;

    info = -1;

    if (incy == 0)          info = 11;
    if (incx == 0)          info =  8;
    if (lda  < k + 1)       info =  6;
    if (k < 0)              info =  3;
    if (n < 0)              info =  2;
    if (uplo  < 0)          info =  1;
  }

  if (info >= 0) {
    BLASFUNC(xerbla)(ERROR_NAME, &info, sizeof(ERROR_NAME));
    return;
  }

#endif

  if (n == 0) return;

  if ((beta_r != ONE) || (beta_i != ZERO)) SCAL_K(n, 0, 0, beta_r, beta_i, y, abs(incy), NULL, 0, NULL, 0);

  if ((alpha_r == ZERO) && (alpha_i == ZERO)) return;

  IDEBUG_START;

  FUNCTION_PROFILE_START();

  if (incx < 0 ) x -= (n - 1) * incx * COMPSIZE;
  if (incy < 0 ) y -= (n - 1) * incy * COMPSIZE;

  buffer = (FLOAT *)blas_memory_alloc(1);

#ifdef SMP
  nthreads = num_cpu_avail(2);

  if (nthreads == 1) {
#endif

    (hbmv[uplo])(n, k, alpha_r, alpha_i, a, lda, x, incx, y, incy, buffer);

#ifdef SMP
  } else {

    (hbmv_thread[uplo])(n, k, ALPHA, a, lda, x, incx, y, incy, buffer, nthreads);

  }
#endif

  blas_memory_free(buffer);

  FUNCTION_PROFILE_END(4, n * k / 2 + n,  n * k);

  IDEBUG_END;

  return;
}
