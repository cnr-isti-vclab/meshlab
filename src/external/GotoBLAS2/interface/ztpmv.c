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
#define ERROR_NAME "XTPMV "
#elif defined(DOUBLE)
#define ERROR_NAME "ZTPMV "
#else
#define ERROR_NAME "CTPMV "
#endif

static int (*tpmv[])(BLASLONG, FLOAT *, FLOAT *, BLASLONG, void *) = {
#ifdef XDOUBLE
  xtpmv_NUU, xtpmv_NUN, xtpmv_NLU, xtpmv_NLN,
  xtpmv_TUU, xtpmv_TUN, xtpmv_TLU, xtpmv_TLN,
  xtpmv_RUU, xtpmv_RUN, xtpmv_RLU, xtpmv_RLN,
  xtpmv_CUU, xtpmv_CUN, xtpmv_CLU, xtpmv_CLN,
#elif defined(DOUBLE)
  ztpmv_NUU, ztpmv_NUN, ztpmv_NLU, ztpmv_NLN,
  ztpmv_TUU, ztpmv_TUN, ztpmv_TLU, ztpmv_TLN,
  ztpmv_RUU, ztpmv_RUN, ztpmv_RLU, ztpmv_RLN,
  ztpmv_CUU, ztpmv_CUN, ztpmv_CLU, ztpmv_CLN,
#else
  ctpmv_NUU, ctpmv_NUN, ctpmv_NLU, ctpmv_NLN,
  ctpmv_TUU, ctpmv_TUN, ctpmv_TLU, ctpmv_TLN,
  ctpmv_RUU, ctpmv_RUN, ctpmv_RLU, ctpmv_RLN,
  ctpmv_CUU, ctpmv_CUN, ctpmv_CLU, ctpmv_CLN,
#endif
};

#ifdef SMP
static int (*tpmv_thread[])(BLASLONG, FLOAT *, FLOAT *, BLASLONG, FLOAT *, int) = {
#ifdef XDOUBLE
  xtpmv_thread_NUU, xtpmv_thread_NUN, xtpmv_thread_NLU, xtpmv_thread_NLN,
  xtpmv_thread_TUU, xtpmv_thread_TUN, xtpmv_thread_TLU, xtpmv_thread_TLN,
  xtpmv_thread_RUU, xtpmv_thread_RUN, xtpmv_thread_RLU, xtpmv_thread_RLN,
  xtpmv_thread_CUU, xtpmv_thread_CUN, xtpmv_thread_CLU, xtpmv_thread_CLN,
#elif defined(DOUBLE)
  ztpmv_thread_NUU, ztpmv_thread_NUN, ztpmv_thread_NLU, ztpmv_thread_NLN,
  ztpmv_thread_TUU, ztpmv_thread_TUN, ztpmv_thread_TLU, ztpmv_thread_TLN,
  ztpmv_thread_RUU, ztpmv_thread_RUN, ztpmv_thread_RLU, ztpmv_thread_RLN,
  ztpmv_thread_CUU, ztpmv_thread_CUN, ztpmv_thread_CLU, ztpmv_thread_CLN,
#else
  ctpmv_thread_NUU, ctpmv_thread_NUN, ctpmv_thread_NLU, ctpmv_thread_NLN,
  ctpmv_thread_TUU, ctpmv_thread_TUN, ctpmv_thread_TLU, ctpmv_thread_TLN,
  ctpmv_thread_RUU, ctpmv_thread_RUN, ctpmv_thread_RLU, ctpmv_thread_RLN,
  ctpmv_thread_CUU, ctpmv_thread_CUN, ctpmv_thread_CLU, ctpmv_thread_CLN,
#endif
};
#endif

#ifndef CBLAS

void NAME(char *UPLO, char *TRANS, char *DIAG,
	   blasint *N, FLOAT *a, FLOAT *x, blasint *INCX){
  
  char uplo_arg  = *UPLO;
  char trans_arg = *TRANS;
  char diag_arg  = *DIAG;

  blasint n    = *N;
  blasint incx = *INCX;

  blasint info;
  int uplo;
  int unit;
  int trans;
  FLOAT *buffer;
#ifdef SMP
  int nthreads;
#endif

  PRINT_DEBUG_NAME;

  TOUPPER(uplo_arg);
  TOUPPER(trans_arg);
  TOUPPER(diag_arg);

  trans = -1;
  unit  = -1;
  uplo  = -1;

  if (trans_arg == 'N') trans = 0;
  if (trans_arg == 'T') trans = 1;
  if (trans_arg == 'R') trans = 2;
  if (trans_arg == 'C') trans = 3;

  if (diag_arg  == 'U') unit  = 0;
  if (diag_arg  == 'N') unit  = 1;

  if (uplo_arg  == 'U') uplo  = 0;
  if (uplo_arg  == 'L') uplo  = 1;
 
  info = 0;

  if (incx == 0)          info =  7;
  if (n < 0)              info =  4;
  if (unit  < 0)          info =  3;
  if (trans < 0)          info =  2;
  if (uplo  < 0)          info =  1;

  if (info != 0) {
    BLASFUNC(xerbla)(ERROR_NAME, &info, sizeof(ERROR_NAME));
    return;
  }
  
#else

void CNAME(enum CBLAS_ORDER order, enum CBLAS_UPLO Uplo,
	   enum CBLAS_TRANSPOSE TransA, enum CBLAS_DIAG Diag,
	   blasint n, FLOAT  *a, FLOAT  *x, blasint incx) {

  int trans, uplo, unit;
  blasint info;
  FLOAT *buffer;

  PRINT_DEBUG_CNAME;

  unit  = -1;
  uplo  = -1;
  trans = -1;
  info  =  0;
#ifdef SMP
  int nthreads;
#endif

  if (order == CblasColMajor) {
    if (Uplo == CblasUpper)         uplo  = 0;
    if (Uplo == CblasLower)         uplo  = 1;

    if (TransA == CblasNoTrans)     trans = 0;
    if (TransA == CblasTrans)       trans = 1;
    if (TransA == CblasConjNoTrans) trans = 2;
    if (TransA == CblasConjTrans)   trans = 3;
    
    if (Diag == CblasUnit)          unit  = 0;
    if (Diag == CblasNonUnit)       unit  = 1;

    info = -1;

    if (incx == 0)          info =  7;
    if (n < 0)              info =  4;
    if (unit  < 0)          info =  3;
    if (trans < 0)          info =  2;
    if (uplo  < 0)          info =  1;
  }

  if (order == CblasRowMajor) {
    if (Uplo == CblasUpper)         uplo  = 1;
    if (Uplo == CblasLower)         uplo  = 0;

    if (TransA == CblasNoTrans)     trans = 1;
    if (TransA == CblasTrans)       trans = 0;
    if (TransA == CblasConjNoTrans) trans = 3;
    if (TransA == CblasConjTrans)   trans = 2;

    if (Diag == CblasUnit)          unit  = 0;
    if (Diag == CblasNonUnit)       unit  = 1;

    info = -1;

    if (incx == 0)          info =  7;
    if (n < 0)              info =  4;
    if (unit  < 0)          info =  3;
    if (trans < 0)          info =  2;
    if (uplo  < 0)          info =  1;
  }

  if (info >= 0) {
    BLASFUNC(xerbla)(ERROR_NAME, &info, sizeof(ERROR_NAME));
    return;
  }

#endif

  if (n == 0) return;
  
  IDEBUG_START;

  FUNCTION_PROFILE_START();

  if (incx < 0 ) x -= (n - 1) * incx * 2;

  buffer = (FLOAT *)blas_memory_alloc(1);

#ifdef SMP
  nthreads = num_cpu_avail(2);

  if (nthreads == 1) {
#endif

  (tpmv[(trans<<2) | (uplo<<1) | unit])(n, a, x, incx, buffer);

#ifdef SMP

  } else {

    (tpmv_thread[(trans<<2) | (uplo<<1) | unit])(n, a, x, incx, buffer, nthreads);

  }
#endif

  blas_memory_free(buffer);

  FUNCTION_PROFILE_END(4, n * n / 2 + n, n * n);

  IDEBUG_END;

  return;
}
