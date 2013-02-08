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
#define ERROR_NAME "XGBMV "
#elif defined(DOUBLE)
#define ERROR_NAME "ZGBMV "
#else
#define ERROR_NAME "CGBMV "
#endif

static void (*gbmv[])(BLASLONG, BLASLONG, BLASLONG, BLASLONG, FLOAT, FLOAT,
		      FLOAT *, BLASLONG, FLOAT *, BLASLONG, FLOAT *, BLASLONG, void *) = {
#ifdef XDOUBLE
      xgbmv_n, xgbmv_t, xgbmv_r, xgbmv_c,
      xgbmv_o, xgbmv_u, xgbmv_s, xgbmv_d,
#elif defined(DOUBLE)
      zgbmv_n, zgbmv_t, zgbmv_r, zgbmv_c,
      zgbmv_o, zgbmv_u, zgbmv_s, zgbmv_d,
#else
      cgbmv_n, cgbmv_t, cgbmv_r, cgbmv_c,
      cgbmv_o, cgbmv_u, cgbmv_s, cgbmv_d,
#endif
};

#ifdef SMP
static int (*gbmv_thread[])(BLASLONG, BLASLONG, BLASLONG, BLASLONG, FLOAT *,
		      FLOAT *, BLASLONG, FLOAT *, BLASLONG, FLOAT *, BLASLONG, FLOAT *, int) = {
#ifdef XDOUBLE
      xgbmv_thread_n, xgbmv_thread_t, xgbmv_thread_r, xgbmv_thread_c,
      xgbmv_thread_o, xgbmv_thread_u, xgbmv_thread_s, xgbmv_thread_d,
#elif defined(DOUBLE)
      zgbmv_thread_n, zgbmv_thread_t, zgbmv_thread_r, zgbmv_thread_c,
      zgbmv_thread_o, zgbmv_thread_u, zgbmv_thread_s, zgbmv_thread_d,
#else
      cgbmv_thread_n, cgbmv_thread_t, cgbmv_thread_r, cgbmv_thread_c,
      cgbmv_thread_o, cgbmv_thread_u, cgbmv_thread_s, cgbmv_thread_d,
#endif
};
#endif

#ifndef CBLAS

void NAME(char *TRANS, blasint *M, blasint *N,
	 blasint *KU, blasint *KL,
	 FLOAT *ALPHA, FLOAT *a, blasint *LDA,
	 FLOAT *x, blasint *INCX,
	 FLOAT *BETA, FLOAT *y, blasint *INCY){

  char trans = *TRANS;
  blasint m = *M;
  blasint n = *N;
  blasint ku = *KU;
  blasint kl = *KL;
  blasint lda = *LDA;
  blasint incx = *INCX;
  blasint incy = *INCY;
  FLOAT *buffer;
#ifdef SMP
  int nthreads;
#endif

  FLOAT alpha_r = ALPHA[0];
  FLOAT alpha_i = ALPHA[1];
  FLOAT beta_r  = BETA[0];
  FLOAT beta_i  = BETA[1];

  blasint info;
  blasint lenx, leny;
  blasint i;

  PRINT_DEBUG_NAME;

  TOUPPER(trans);

  info = 0;

  i = -1;

  if (trans == 'N')  i = 0;
  if (trans == 'T')  i = 1;
  if (trans == 'R')  i = 2;
  if (trans == 'C')  i = 3;
  if (trans == 'O')  i = 4;
  if (trans == 'U')  i = 5;
  if (trans == 'S')  i = 6;
  if (trans == 'D')  i = 7;

  if (incy == 0)	 info = 13;
  if (incx == 0)	 info = 10;
  if (lda < kl + ku + 1) info = 8;
  if (kl < 0)		 info = 5;
  if (ku < 0)		 info = 4;
  if (n < 0)		 info = 3;
  if (m < 0)		 info = 2;
  if (i < 0)		 info = 1;
  
  trans = i;

  if (info != 0){
    BLASFUNC(xerbla)(ERROR_NAME, &info, sizeof(ERROR_NAME));
    return;
  }

#else

void CNAME(enum CBLAS_ORDER order,
	   enum CBLAS_TRANSPOSE TransA,
	   blasint m, blasint n,
	   blasint ku, blasint kl,
	   FLOAT *ALPHA,
	   FLOAT  *a, blasint lda,
	   FLOAT  *x, blasint incx,
	   FLOAT *BETA,
	   FLOAT  *y, blasint incy){

  FLOAT alpha_r = ALPHA[0];
  FLOAT alpha_i = ALPHA[1];
  FLOAT beta_r  = BETA[0];
  FLOAT beta_i  = BETA[1];

  FLOAT *buffer;
  blasint lenx, leny;
  int trans;
  blasint info, t;
#ifdef SMP
  int nthreads;
#endif

  PRINT_DEBUG_CNAME;

  trans = -1;
  info  =  0;

  if (order == CblasColMajor) {
    if (TransA == CblasNoTrans)     trans = 0;
    if (TransA == CblasTrans)       trans = 1;
    if (TransA == CblasConjNoTrans) trans = 2;
    if (TransA == CblasConjTrans)   trans = 3;
    
    info = -1;

    if (incy == 0)	 info = 13;
    if (incx == 0)	 info = 10;
    if (lda < kl + ku + 1) info = 8;
    if (kl < 0)		 info = 5;
    if (ku < 0)		 info = 4;
    if (n < 0)		 info = 3;
    if (m < 0)		 info = 2;
    if (trans < 0)	 info = 1;
  }

  if (order == CblasRowMajor) {
    if (TransA == CblasNoTrans)     trans = 1;
    if (TransA == CblasTrans)       trans = 0;
    if (TransA == CblasConjNoTrans) trans = 3;
    if (TransA == CblasConjTrans)   trans = 2;

    info = -1;

    t = n;
    n = m;
    m = t;

    t  = ku;
    ku = kl;
    kl = t;

    if (incy == 0)	 info = 13;
    if (incx == 0)	 info = 10;
    if (lda < kl + ku + 1) info = 8;
    if (kl < 0)		 info = 5;
    if (ku < 0)		 info = 4;
    if (n < 0)		 info = 3;
    if (m < 0)		 info = 2;
    if (trans < 0)	 info = 1;
  }

  if (info >= 0) {
    BLASFUNC(xerbla)(ERROR_NAME, &info, sizeof(ERROR_NAME));
    return;
  }

#endif

  if ((m==0) || (n==0)) return;

  lenx = n;
  leny = m;
  if (trans & 1) lenx = m;
  if (trans & 1) leny = n;

  if (beta_r != ONE || beta_i != ZERO) SCAL_K(leny, 0, 0, beta_r, beta_i, y, abs(incy), NULL, 0, NULL, 0);

  if (alpha_r == ZERO && alpha_i == ZERO) return;
  
  IDEBUG_START;

  FUNCTION_PROFILE_START();

  if (incx < 0) x -= (lenx - 1) * incx * 2;
  if (incy < 0) y -= (leny - 1) * incy * 2;

  buffer = (FLOAT *)blas_memory_alloc(1);

#ifdef SMP
  nthreads = num_cpu_avail(2);

  if (nthreads == 1) {
#endif

  (gbmv[(int)trans])(m, n, kl, ku, alpha_r, alpha_i, a, lda, x, incx, y, incy, buffer);

#ifdef SMP

  } else {

    (gbmv_thread[(int)trans])(m, n, kl, ku, ALPHA, a, lda, x, incx, y, incy, buffer, nthreads);

  }
#endif

  blas_memory_free(buffer);

  FUNCTION_PROFILE_END(4, m * n / 2 + n, m * n);

  IDEBUG_END;

  return;
}
