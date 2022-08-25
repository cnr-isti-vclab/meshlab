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

blasint CNAME(blas_arg_t *args, BLASLONG *range_m, BLASLONG *range_n, FLOAT *sa, FLOAT *sb, BLASLONG myid) {

  BLASLONG n, bk, i, blocking, lda;
  BLASLONG info;
  int mode;
  blas_arg_t newarg;
  FLOAT *a;
  FLOAT alpha[2] = { -ONE, ZERO};

#ifndef COMPLEX
#ifdef XDOUBLE
  mode  =  BLAS_XDOUBLE | BLAS_REAL;
#elif defined(DOUBLE)
  mode  =  BLAS_DOUBLE  | BLAS_REAL;
#else
  mode  =  BLAS_SINGLE  | BLAS_REAL;
#endif  
#else
#ifdef XDOUBLE
  mode  =  BLAS_XDOUBLE | BLAS_COMPLEX;
#elif defined(DOUBLE)
  mode  =  BLAS_DOUBLE  | BLAS_COMPLEX;
#else
  mode  =  BLAS_SINGLE  | BLAS_COMPLEX;
#endif  
#endif

  if (args -> nthreads  == 1) {
    info = POTRF_U_SINGLE(args, NULL, NULL, sa, sb, 0); 
    return info;
  }

  n  = args -> n;
  a  = (FLOAT *)args -> a;
  lda = args -> lda;

  if (range_n) n  = range_n[1] - range_n[0];

  if (n <= GEMM_UNROLL_N * 4) {
    info = POTRF_U_SINGLE(args, NULL, range_n, sa, sb, 0);
    return info;
  }

  newarg.lda = lda;
  newarg.ldb = lda;
  newarg.ldc = lda;
  newarg.alpha = alpha;
  newarg.beta = NULL;
  newarg.nthreads = args -> nthreads;

  blocking = (n / 2 + GEMM_UNROLL_N - 1) & ~(GEMM_UNROLL_N - 1);
  if (blocking > GEMM_Q) blocking = GEMM_Q;
    
  for (i = 0; i < n; i += blocking) {
    bk = n - i;
    if (bk > blocking) bk = blocking;

    newarg.m = bk;
    newarg.n = bk;
    newarg.a = a + (i + i * lda) * COMPSIZE;

    info = CNAME(&newarg, NULL, NULL, sa, sb, 0);
    if (info) return info + i;

    if (n - i - bk > 0) {
      newarg.m = bk;
      newarg.n = n - i - bk;
      newarg.a = a + (i +  i       * lda) * COMPSIZE;
      newarg.b = a + (i + (i + bk) * lda) * COMPSIZE;
      
      gemm_thread_n(mode | BLAS_TRANSA_T,
		    &newarg, NULL, NULL, (void *)TRSM_LCUN, sa, sb, args -> nthreads);
      
      newarg.n = n - i - bk;
      newarg.k = bk;
      newarg.a = a + ( i       + (i + bk) * lda) * COMPSIZE;
      newarg.c = a + ((i + bk) + (i + bk) * lda) * COMPSIZE;
      
#ifndef USE_SIMPLE_THREADED_LEVEL3
      HERK_THREAD_UC(&newarg, NULL, NULL, sa, sb, 0);
#else
      syrk_thread(mode | BLAS_TRANSA_N | BLAS_TRANSB_T,
		  &newarg, NULL, NULL, (void *)HERK_UC, sa, sb, args -> nthreads);
#endif
    }
  }

  return 0;
}
