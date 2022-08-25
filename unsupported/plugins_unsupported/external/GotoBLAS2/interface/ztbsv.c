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
#define ERROR_NAME "XTBSV "
#elif defined(DOUBLE)
#define ERROR_NAME "ZTBSV "
#else
#define ERROR_NAME "CTBSV "
#endif

static int (*tbsv[])(BLASLONG, BLASLONG, FLOAT *, BLASLONG, FLOAT *, BLASLONG, void *) = {
#ifdef XDOUBLE
  xtbsv_NUU, xtbsv_NUN, xtbsv_NLU, xtbsv_NLN,
  xtbsv_TUU, xtbsv_TUN, xtbsv_TLU, xtbsv_TLN,
  xtbsv_RUU, xtbsv_RUN, xtbsv_RLU, xtbsv_RLN,
  xtbsv_CUU, xtbsv_CUN, xtbsv_CLU, xtbsv_CLN,
#elif defined(DOUBLE)
  ztbsv_NUU, ztbsv_NUN, ztbsv_NLU, ztbsv_NLN,
  ztbsv_TUU, ztbsv_TUN, ztbsv_TLU, ztbsv_TLN,
  ztbsv_RUU, ztbsv_RUN, ztbsv_RLU, ztbsv_RLN,
  ztbsv_CUU, ztbsv_CUN, ztbsv_CLU, ztbsv_CLN,
#else
  ctbsv_NUU, ctbsv_NUN, ctbsv_NLU, ctbsv_NLN,
  ctbsv_TUU, ctbsv_TUN, ctbsv_TLU, ctbsv_TLN,
  ctbsv_RUU, ctbsv_RUN, ctbsv_RLU, ctbsv_RLN,
  ctbsv_CUU, ctbsv_CUN, ctbsv_CLU, ctbsv_CLN,
#endif
};

#ifndef CBLAS

void NAME(char *UPLO, char *TRANS, char *DIAG,
	 blasint *N, blasint *K, 
	 FLOAT *a, blasint *LDA, FLOAT *x, blasint *INCX){
  
  char uplo_arg  = *UPLO;
  char trans_arg = *TRANS;
  char diag_arg  = *DIAG;
  
  blasint n    = *N;
  blasint k    = *K;
  blasint lda  = *LDA;
  blasint incx = *INCX;

  blasint info;
  int uplo;
  int unit;
  int trans;
  FLOAT *buffer;

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

  if (incx == 0)          info =  9;
  if (lda < k + 1)        info =  7;
  if (k < 0)              info =  5;
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
	   blasint n, blasint k, FLOAT  *a, blasint lda, FLOAT  *x, blasint incx) {

  int trans, uplo, unit;
  blasint info;
  FLOAT *buffer;

  PRINT_DEBUG_CNAME;

  unit  = -1;
  uplo  = -1;
  trans = -1;
  info  =  0;

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

    if (incx == 0)          info =  9;
    if (lda < k + 1)        info =  7;
    if (k < 0)              info =  5;
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

    if (incx == 0)          info =  9;
    if (lda < k + 1)        info =  7;
    if (k < 0)              info =  5;
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

  (tbsv[(trans<<2) | (uplo<<1) | unit])(n, k, a, lda, x, incx, buffer);

  blas_memory_free(buffer);

  FUNCTION_PROFILE_END(4, n * k / 2 + n, n * k);

  IDEBUG_END;

  return;
}
