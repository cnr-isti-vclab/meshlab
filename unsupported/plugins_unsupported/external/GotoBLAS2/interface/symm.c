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

#ifndef COMPLEX
#ifdef XDOUBLE
#define ERROR_NAME "QSYMM "
#elif defined(DOUBLE)
#define ERROR_NAME "DSYMM "
#else
#define ERROR_NAME "SSYMM "
#endif
#else
#ifndef GEMM3M
#ifndef HEMM
#ifdef XDOUBLE
#define ERROR_NAME "XSYMM "
#elif defined(DOUBLE)
#define ERROR_NAME "ZSYMM "
#else
#define ERROR_NAME "CSYMM "
#endif
#else
#ifdef XDOUBLE
#define ERROR_NAME "XHEMM "
#elif defined(DOUBLE)
#define ERROR_NAME "ZHEMM "
#else
#define ERROR_NAME "CHEMM "
#endif
#endif
#else
#ifndef HEMM
#ifdef XDOUBLE
#define ERROR_NAME "XSYMM3M "
#elif defined(DOUBLE)
#define ERROR_NAME "ZSYMM3M "
#else
#define ERROR_NAME "CSYMM3M "
#endif
#else
#ifdef XDOUBLE
#define ERROR_NAME "XHEMM3M "
#elif defined(DOUBLE)
#define ERROR_NAME "ZHEMM3M "
#else
#define ERROR_NAME "CHEMM3M "
#endif
#endif
#endif
#endif

static int (*symm[])(blas_arg_t *, BLASLONG *, BLASLONG *, FLOAT *, FLOAT *, BLASLONG) = {
#ifndef GEMM3M
#ifndef HEMM
  SYMM_LU, SYMM_LL, SYMM_RU, SYMM_RL,
#if defined(SMP) && !defined(USE_SIMPLE_THREADED_LEVEL3)
  SYMM_THREAD_LU, SYMM_THREAD_LL, SYMM_THREAD_RU, SYMM_THREAD_RL,
#endif
#else
  HEMM_LU, HEMM_LL, HEMM_RU, HEMM_RL,
#if defined(SMP) && !defined(USE_SIMPLE_THREADED_LEVEL3)
  HEMM_THREAD_LU, HEMM_THREAD_LL, HEMM_THREAD_RU, HEMM_THREAD_RL,
#endif
#endif
#else
#ifndef HEMM
  SYMM3M_LU, SYMM3M_LL, SYMM3M_RU, SYMM3M_RL,
#if defined(SMP) && !defined(USE_SIMPLE_THREADED_LEVEL3)
  SYMM3M_THREAD_LU, SYMM3M_THREAD_LL, SYMM3M_THREAD_RU, SYMM3M_THREAD_RL,
#endif
#else
  HEMM3M_LU, HEMM3M_LL, HEMM3M_RU, HEMM3M_RL,
#if defined(SMP) && !defined(USE_SIMPLE_THREADED_LEVEL3)
  HEMM3M_THREAD_LU, HEMM3M_THREAD_LL, HEMM3M_THREAD_RU, HEMM3M_THREAD_RL,
#endif
#endif
#endif
};

#ifndef CBLAS

void NAME(char *SIDE, char *UPLO, 
         blasint *M, blasint *N, 
         FLOAT *alpha, FLOAT *a, blasint *ldA, 
         FLOAT *b, blasint *ldB, 
         FLOAT *beta,  FLOAT *c, blasint *ldC){
  
  char side_arg  = *SIDE;
  char uplo_arg  = *UPLO;

  blas_arg_t args;

  FLOAT *buffer;
  FLOAT *sa, *sb;

#ifdef SMP
#ifdef XDOUBLE
  int mode  =  BLAS_XDOUBLE | BLAS_REAL;
#elif defined(DOUBLE)
  int mode  =  BLAS_DOUBLE  | BLAS_REAL;
#else
  int mode  =  BLAS_SINGLE  | BLAS_REAL;
#endif  
#endif

#if defined(SMP) && !defined(NO_AFFINITY)
  int nodes;
#endif

  blasint info;
  int side;
  int uplo;

  PRINT_DEBUG_NAME;

  args.alpha = (void *)alpha;
  args.beta  = (void *)beta;

  TOUPPER(side_arg);
  TOUPPER(uplo_arg);

  side  = -1;
  uplo  = -1;

  if (side_arg  == 'L') side  = 0;
  if (side_arg  == 'R') side  = 1;

  if (uplo_arg  == 'U') uplo  = 0;
  if (uplo_arg  == 'L') uplo  = 1;
  
  args.m = *M;
  args.n = *N;

  args.c = (void *)c;
  args.ldc = *ldC;
    
  info = 0;

  if (args.ldc < MAX(1, args.m)) info = 12;

  if (!side) {
    args.a = (void *)a;
    args.b = (void *)b;
    
    args.lda = *ldA;
    args.ldb = *ldB;
    
    if (args.ldb < MAX(1, args.m)) info =  9;
    if (args.lda < MAX(1, args.m)) info =  7;

  } else {
    args.a = (void *)b;
    args.b = (void *)a;
    
    args.lda = *ldB;
    args.ldb = *ldA;

  if (args.lda < MAX(1, args.m)) info =  9;
  if (args.ldb < MAX(1, args.n)) info =  7;
  }

  if (args.n   < 0)              info =  4;
  if (args.m   < 0)              info =  3;
  if (uplo     < 0)              info =  2;
  if (side     < 0)              info =  1;

  if (info != 0) {
    BLASFUNC(xerbla)(ERROR_NAME, &info, sizeof(ERROR_NAME));
    return;
  }
  
#else

void CNAME(enum CBLAS_ORDER order, enum CBLAS_SIDE Side, enum CBLAS_UPLO Uplo,
	   blasint m, blasint n,
#ifndef COMPLEX
	   FLOAT alpha,
#else
	   FLOAT *alpha,
#endif
	   FLOAT *a, blasint lda,
	   FLOAT *b, blasint ldb,
#ifndef COMPLEX
	   FLOAT beta,
#else
	   FLOAT *beta,
#endif
	   FLOAT *c, blasint ldc) {

  blas_arg_t args;
  int side, uplo;
  blasint info;

  FLOAT *buffer;
  FLOAT *sa, *sb;

#ifdef SMP
#ifdef XDOUBLE
  int mode  =  BLAS_XDOUBLE | BLAS_REAL;
#elif defined(DOUBLE)
  int mode  =  BLAS_DOUBLE  | BLAS_REAL;
#else
  int mode  =  BLAS_SINGLE  | BLAS_REAL;
#endif  
#endif

#if defined(SMP) && !defined(NO_AFFINITY)
  int nodes;
#endif

  PRINT_DEBUG_CNAME;

#ifndef COMPLEX
  args.alpha = (void *)&alpha;
  args.beta  = (void *)&beta;
#else
  args.alpha = (void *)alpha;
  args.beta  = (void *)beta;
#endif

  args.c = (void *)c;
  args.ldc = ldc;

  side  = -1;
  uplo  = -1;
  info  =  0;

  if (order == CblasColMajor) {
    if (Side == CblasLeft)  side = 0;
    if (Side == CblasRight) side = 1;

    if (Uplo == CblasUpper) uplo  = 0;
    if (Uplo == CblasLower) uplo  = 1;

    info = -1;

    args.m = m;
    args.n = n;

    if (args.ldc < MAX(1, args.m)) info = 12;

    if (!side) {
      args.a = (void *)a;
      args.b = (void *)b;
      
      args.lda = lda;
      args.ldb = ldb;
      
      if (args.ldb < MAX(1, args.m)) info =  9;
      if (args.lda < MAX(1, args.m)) info =  7;
      
    } else {
      args.a = (void *)b;
      args.b = (void *)a;
      
      args.lda = ldb;
      args.ldb = lda;
      
      if (args.lda < MAX(1, args.m)) info =  9;
      if (args.ldb < MAX(1, args.n)) info =  7;
    }
    
    if (args.n   < 0)              info =  4;
    if (args.m   < 0)              info =  3;
    if (uplo     < 0)              info =  2;
    if (side     < 0)              info =  1;
  }

  if (order == CblasRowMajor) {
    if (Side == CblasLeft)  side = 1;
    if (Side == CblasRight) side = 0;

    if (Uplo == CblasUpper) uplo  = 1;
    if (Uplo == CblasLower) uplo  = 0;

    info = -1;

    args.m = n;
    args.n = m;

    if (args.ldc < MAX(1, args.m)) info = 12;

    if (!side) {
      args.a = (void *)a;
      args.b = (void *)b;
      
      args.lda = lda;
      args.ldb = ldb;
      
      if (args.ldb < MAX(1, args.m)) info =  9;
      if (args.lda < MAX(1, args.m)) info =  7;
      
    } else {
      args.a = (void *)b;
      args.b = (void *)a;
      
      args.lda = ldb;
      args.ldb = lda;
      
      if (args.lda < MAX(1, args.m)) info =  9;
      if (args.ldb < MAX(1, args.n)) info =  7;
    }
    
    if (args.n   < 0)              info =  4;
    if (args.m   < 0)              info =  3;
    if (uplo     < 0)              info =  2;
    if (side     < 0)              info =  1;
  }

  if (info >= 0) {
    BLASFUNC(xerbla)(ERROR_NAME, &info, sizeof(ERROR_NAME));
    return;
  }

#endif

  if (args.m == 0 || args.n == 0) return;

  IDEBUG_START;

  FUNCTION_PROFILE_START();

  buffer = (FLOAT *)blas_memory_alloc(0);
    
  sa = (FLOAT *)((BLASLONG)buffer + GEMM_OFFSET_A);
  sb = (FLOAT *)(((BLASLONG)sa + ((GEMM_P * GEMM_Q * COMPSIZE * SIZE + GEMM_ALIGN) & ~GEMM_ALIGN)) + GEMM_OFFSET_B);
  
#ifdef SMP
  args.common = NULL;
  args.nthreads = num_cpu_avail(3);

  if (args.nthreads == 1) {
#endif

    (symm[(side << 1) | uplo ])(&args, NULL, NULL, sa, sb, 0);

#ifdef SMP

  } else {

#ifndef NO_AFFINITY
    nodes = get_num_nodes();
    
    if (nodes > 1) {
      
      args.nthreads /= nodes;
      
      gemm_thread_mn(mode, &args, NULL, NULL, 
		     symm[4 | (side << 1) | uplo ], sa, sb, nodes);
      
    } else {
#endif

#ifndef USE_SIMPLE_THREADED_LEVEL3
      
      (symm[4 | (side << 1) | uplo ])(&args, NULL, NULL, sa, sb, 0);
      
#else
      
      GEMM_THREAD(mode, &args, NULL, NULL, symm[(side << 1) | uplo ], sa, sb, args.nthreads);
      
#endif

#ifndef NO_AFFINITY
    }
#endif

  }
#endif

 blas_memory_free(buffer);

  FUNCTION_PROFILE_END(COMPSIZE * COMPSIZE,
		       (!side)? args.m * (args.m / 2 + args.n) : args.n * (args.m + args.n / 2),
		       (!side)? 2 * args.m * args.m * args.n : 2 * args.m * args.n * args.n);

  IDEBUG_END;

  return;
}
