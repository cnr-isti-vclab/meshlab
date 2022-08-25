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

#ifdef XDOUBLE
#define ERROR_NAME "QGEMV "
#elif defined(DOUBLE)
#define ERROR_NAME "DGEMV "
#else
#define ERROR_NAME "SGEMV "
#endif

#ifdef SMP
static int (*gemv_thread[])(BLASLONG, BLASLONG, FLOAT, FLOAT *, BLASLONG,  FLOAT * , BLASLONG, FLOAT *, BLASLONG, FLOAT *, int) = {
#ifdef XDOUBLE
  qgemv_thread_n, qgemv_thread_t,
#elif defined DOUBLE
  dgemv_thread_n, dgemv_thread_t,
#else
  sgemv_thread_n, sgemv_thread_t,
#endif
};
#endif

#ifndef CBLAS

void NAME(char *TRANS, blasint *M, blasint *N,
	   FLOAT *ALPHA, FLOAT *a, blasint *LDA,
	   FLOAT *x, blasint *INCX,
	   FLOAT *BETA, FLOAT *y, blasint *INCY){

  char trans = *TRANS;
  blasint m = *M;
  blasint n = *N;
  blasint lda = *LDA;
  blasint incx = *INCX;
  blasint incy = *INCY;
  FLOAT alpha = *ALPHA;
  FLOAT beta  = *BETA;
  FLOAT *buffer;
#ifdef SMP
  int nthreads;
#endif

  int (*gemv[])(BLASLONG, BLASLONG, BLASLONG, FLOAT, FLOAT *, BLASLONG,  FLOAT * , BLASLONG, FLOAT *, BLASLONG, FLOAT *) = {
    GEMV_N, GEMV_T,
  };
  
  blasint info;
  blasint lenx, leny;
  blasint i;

  PRINT_DEBUG_NAME;

  TOUPPER(trans);

  info = 0;

  i = -1;

  if (trans == 'N') i = 0;
  if (trans == 'T') i = 1;
  if (trans == 'R') i = 0;
  if (trans == 'C') i = 1;

  if (incy == 0)	info = 11;
  if (incx == 0)	info = 8;
  if (lda < MAX(1, m))	info = 6;
  if (n < 0)		info = 3;
  if (m < 0)		info = 2;
  if (i < 0)          info = 1;
  
  trans = i;

  if (info != 0){
    BLASFUNC(xerbla)(ERROR_NAME, &info, sizeof(ERROR_NAME));
    return;
  }

#else

void CNAME(enum CBLAS_ORDER order,
	   enum CBLAS_TRANSPOSE TransA,
	   blasint m, blasint n,
	   FLOAT alpha,
	   FLOAT  *a, blasint lda,
	   FLOAT  *x, blasint incx,
	   FLOAT beta,
	   FLOAT  *y, blasint incy){

  FLOAT *buffer;
  blasint lenx, leny;
  int trans;
  blasint info, t;
#ifdef SMP
  int nthreads;
#endif

  int (*gemv[])(BLASLONG, BLASLONG, BLASLONG, FLOAT, FLOAT *, BLASLONG,  FLOAT * , BLASLONG, FLOAT *, BLASLONG, FLOAT *) = {
    GEMV_N, GEMV_T,
  };

  PRINT_DEBUG_CNAME;

  trans = -1;
  info  =  0;

  if (order == CblasColMajor) {
    if (TransA == CblasNoTrans)     trans = 0;
    if (TransA == CblasTrans)       trans = 1;
    if (TransA == CblasConjNoTrans) trans = 0;
    if (TransA == CblasConjTrans)   trans = 1;
    
    info = -1;

    if (incy == 0)	  info = 11;
    if (incx == 0)	  info = 8;
    if (lda < MAX(1, m))  info = 6;
    if (n < 0)		  info = 3;
    if (m < 0)		  info = 2;
    if (trans < 0)        info = 1;
    
  }

  if (order == CblasRowMajor) {
    if (TransA == CblasNoTrans)     trans = 1;
    if (TransA == CblasTrans)       trans = 0;
    if (TransA == CblasConjNoTrans) trans = 1;
    if (TransA == CblasConjTrans)   trans = 0;

    info = -1;

    t = n;
    n = m;
    m = t;

    if (incy == 0)	  info = 11;
    if (incx == 0)	  info = 8;
    if (lda < MAX(1, m))  info = 6;
    if (n < 0)		  info = 3;
    if (m < 0)		  info = 2;
    if (trans < 0)        info = 1;

  }

  if (info >= 0) {
    BLASFUNC(xerbla)(ERROR_NAME, &info, sizeof(ERROR_NAME));
    return;
  }

#endif

  if ((m==0) || (n==0)) return;

  lenx = n;
  leny = m;
  if (trans) lenx = m;
  if (trans) leny = n;

  if (beta != ONE) SCAL_K(leny, 0, 0, beta, y, abs(incy), NULL, 0, NULL, 0);
  
  if (alpha == ZERO) return;

  IDEBUG_START;

  FUNCTION_PROFILE_START();

  if (incx < 0) x -= (lenx - 1) * incx;
  if (incy < 0) y -= (leny - 1) * incy;

  buffer = (FLOAT *)blas_memory_alloc(1);

#ifdef SMP
  nthreads = num_cpu_avail(2);

  if (nthreads == 1) {
#endif
    
    (gemv[(int)trans])(m, n, 0, alpha, a, lda, x, incx, y, incy, buffer);
    
#ifdef SMP
  } else {
    
    (gemv_thread[(int)trans])(m, n, alpha, a, lda, x, incx, y, incy, buffer, nthreads);
    
  }
#endif
  
  blas_memory_free(buffer);

  FUNCTION_PROFILE_END(1, m * n + m + n,  2 * m * n);

  IDEBUG_END;

  return;

}
