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

#ifndef USE_SIMPLE_THREADED_LEVEL3

static FLOAT dm1 = -1.;

#ifndef KERNEL_FUNC
#ifndef LOWER
#define KERNEL_FUNC SYRK_KERNEL_U
#else
#define KERNEL_FUNC SYRK_KERNEL_L
#endif
#endif

#ifndef LOWER
#ifndef COMPLEX
#define TRSM_KERNEL   TRSM_KERNEL_LT
#else
#define TRSM_KERNEL   TRSM_KERNEL_LC
#endif
#else
#ifndef COMPLEX
#define TRSM_KERNEL   TRSM_KERNEL_RN
#else
#define TRSM_KERNEL   TRSM_KERNEL_RR
#endif
#endif

#ifndef CACHE_LINE_SIZE
#define CACHE_LINE_SIZE 8
#endif

#ifndef DIVIDE_RATE
#define DIVIDE_RATE 2
#endif

#ifndef SWITCH_RATIO
#define SWITCH_RATIO 2
#endif

#ifndef LOWER
#define TRANS
#endif

#ifndef SYRK_LOCAL
#if   !defined(LOWER) && !defined(TRANS)
#define SYRK_LOCAL    SYRK_UN
#elif !defined(LOWER) &&  defined(TRANS)
#define SYRK_LOCAL    SYRK_UT
#elif  defined(LOWER) && !defined(TRANS)
#define SYRK_LOCAL    SYRK_LN
#else
#define SYRK_LOCAL    SYRK_LT
#endif
#endif

typedef struct {
  volatile BLASLONG working[MAX_CPU_NUMBER][CACHE_LINE_SIZE * DIVIDE_RATE];
} job_t;


#ifndef KERNEL_OPERATION
#ifndef COMPLEX
#define KERNEL_OPERATION(M, N, K, ALPHA, SA, SB, C, LDC, X, Y) \
	KERNEL_FUNC(M, N, K, ALPHA[0], SA, SB, (FLOAT *)(C) + ((X) + (Y) * LDC) * COMPSIZE, LDC, (X) - (Y))
#else
#define KERNEL_OPERATION(M, N, K, ALPHA, SA, SB, C, LDC, X, Y) \
	KERNEL_FUNC(M, N, K, ALPHA[0], ALPHA[1], SA, SB, (FLOAT *)(C) + ((X) + (Y) * LDC) * COMPSIZE, LDC, (X) - (Y))
#endif
#endif

#ifndef ICOPY_OPERATION
#ifndef TRANS
#define ICOPY_OPERATION(M, N, A, LDA, X, Y, BUFFER) GEMM_ITCOPY(M, N, (FLOAT *)(A) + ((Y) + (X) * (LDA)) * COMPSIZE, LDA, BUFFER);
#else
#define ICOPY_OPERATION(M, N, A, LDA, X, Y, BUFFER) GEMM_INCOPY(M, N, (FLOAT *)(A) + ((X) + (Y) * (LDA)) * COMPSIZE, LDA, BUFFER);
#endif
#endif

#ifndef OCOPY_OPERATION
#ifdef TRANS
#define OCOPY_OPERATION(M, N, A, LDA, X, Y, BUFFER) GEMM_ONCOPY(M, N, (FLOAT *)(A) + ((X) + (Y) * (LDA)) * COMPSIZE, LDA, BUFFER);
#else
#define OCOPY_OPERATION(M, N, A, LDA, X, Y, BUFFER) GEMM_OTCOPY(M, N, (FLOAT *)(A) + ((Y) + (X) * (LDA)) * COMPSIZE, LDA, BUFFER);
#endif
#endif

#ifndef S
#define S	args -> a
#endif
#ifndef A
#define A	args -> b
#endif
#ifndef C
#define C	args -> c
#endif
#ifndef LDA
#define LDA	args -> lda
#endif
#ifndef N
#define N	args -> m
#endif
#ifndef K
#define K	args -> k
#endif

static int inner_thread(blas_arg_t *args, BLASLONG *range_m, BLASLONG *range_n, FLOAT *sa, FLOAT *sb, BLASLONG mypos){

  FLOAT *buffer[DIVIDE_RATE];

  BLASLONG k, lda;
  BLASLONG m_from, m_to;

  FLOAT *alpha;
  FLOAT *a, *c;
  job_t *job = (job_t *)args -> common;
  BLASLONG xxx, bufferside;

  BLASLONG jjs, min_jj;
  BLASLONG is, min_i, div_n;

  BLASLONG i, current;

  k = K;

  a = (FLOAT *)A;
  c = (FLOAT *)C;

  lda = LDA;

  alpha = (FLOAT *)args -> alpha;

  m_from = range_n[mypos + 0];
  m_to   = range_n[mypos + 1];

#if 0
  fprintf(stderr, "Thread[%ld]  m_from : %ld m_to : %ld\n",  mypos, m_from, m_to);
#endif

  div_n = ((m_to - m_from + DIVIDE_RATE - 1) / DIVIDE_RATE + GEMM_UNROLL_MN - 1) & ~(GEMM_UNROLL_MN - 1);

  buffer[0] = (FLOAT *)((((long)(sb + k * k * COMPSIZE) + GEMM_ALIGN) & ~GEMM_ALIGN) + GEMM_OFFSET_B);
  for (i = 1; i < DIVIDE_RATE; i++) {
    buffer[i] = buffer[i - 1] + GEMM_Q * div_n * COMPSIZE;
  }
  
#ifndef LOWER
  TRSM_IUNCOPY(k, k, (FLOAT *)S, lda, 0, sb);
#else
  TRSM_OLTCOPY(k, k, (FLOAT *)S, lda, 0, sb);
#endif
  
  for (xxx = m_from, bufferside = 0; xxx < m_to; xxx += div_n, bufferside ++) {
    
    for(jjs = xxx; jjs < MIN(m_to, xxx + div_n); jjs += min_jj){
      
      min_jj = MIN(m_to, xxx + div_n) - jjs;
      
#ifndef LOWER
      if (min_jj > GEMM_UNROLL_MN) min_jj = GEMM_UNROLL_MN;
#else
      if (min_jj > GEMM_P)         min_jj = GEMM_P;
#endif

#ifndef LOWER
      OCOPY_OPERATION (k, min_jj, a, lda, 0, jjs, buffer[bufferside] + k * (jjs - xxx) * COMPSIZE);

      TRSM_KERNEL     (k, min_jj, k, dm1, 
#ifdef COMPLEX
		       ZERO,
#endif
		       sb,
		       buffer[bufferside] + k * (jjs - xxx) * COMPSIZE,
		       a + jjs * lda * COMPSIZE, lda, 0);
#else
      ICOPY_OPERATION (k, min_jj, a, lda, 0, jjs, buffer[bufferside] + k * (jjs - xxx) * COMPSIZE);

      TRSM_KERNEL     (min_jj, k, k, dm1,
#ifdef COMPLEX
		       ZERO,
#endif
		       buffer[bufferside] + k * (jjs - xxx) * COMPSIZE,
		       sb,
		       a + jjs       * COMPSIZE, lda, 0);
#endif
    }
    
#ifndef LOWER
    for (i = 0; i <= mypos; i++)
      job[mypos].working[i][CACHE_LINE_SIZE * bufferside] = (BLASLONG)buffer[bufferside];
#else
    for (i = mypos; i < args -> nthreads; i++)
      job[mypos].working[i][CACHE_LINE_SIZE * bufferside] = (BLASLONG)buffer[bufferside];
#endif
    
    WMB;
  }
  
  min_i = m_to - m_from;
  
  if (min_i >= GEMM_P * 2) {
    min_i = GEMM_P;
  } else 
    if (min_i > GEMM_P) {
      min_i = ((min_i + 1) / 2 + GEMM_UNROLL_MN - 1) & ~(GEMM_UNROLL_MN - 1);
    }
  
#ifndef LOWER
  ICOPY_OPERATION(k, min_i, a, lda, 0, m_from, sa);
#else
  OCOPY_OPERATION(k, min_i, a, lda, 0, m_from, sa);
#endif
  
  current = mypos;

#ifndef LOWER
  while (current < args -> nthreads)
#else
  while (current >= 0)
#endif
    {
      div_n = ((range_n[current + 1]  - range_n[current] + DIVIDE_RATE - 1) / DIVIDE_RATE + GEMM_UNROLL_MN - 1) & ~(GEMM_UNROLL_MN - 1);
      
      for (xxx = range_n[current], bufferside = 0; xxx < range_n[current + 1]; xxx += div_n, bufferside ++) {
	
	/* thread has to wait */
	if (current != mypos) while(job[current].working[mypos][CACHE_LINE_SIZE * bufferside] == 0) {YIELDING;};
	
	KERNEL_OPERATION(min_i, MIN(range_n[current + 1] - xxx, div_n), k, alpha,
			 sa, (FLOAT *)job[current].working[mypos][CACHE_LINE_SIZE * bufferside],
			 c, lda, m_from, xxx);
	
	if (m_from + min_i >= m_to) {
	  job[current].working[mypos][CACHE_LINE_SIZE * bufferside] &= 0;
	  WMB;
	}
      }
      
#ifndef LOWER
      current ++;
#else
      current --;
#endif
    }
  
  for(is = m_from + min_i; is < m_to; is += min_i){
    min_i = m_to - is;
    
    if (min_i >= GEMM_P * 2) {
      min_i = GEMM_P;
    } else 
      if (min_i > GEMM_P) {
	min_i = ((min_i + 1) / 2 + GEMM_UNROLL_MN - 1) & ~(GEMM_UNROLL_MN - 1);
      }
    
#ifndef LOWER
    ICOPY_OPERATION(k, min_i, a, lda, 0, is, sa);
#else
    OCOPY_OPERATION(k, min_i, a, lda, 0, is, sa);
#endif
    
    current = mypos;
    
#ifndef LOWER
    while (current < args -> nthreads)
#else
      while (current >= 0)
#endif
	{
	  div_n = ((range_n[current + 1]  - range_n[current] + DIVIDE_RATE - 1) / DIVIDE_RATE + GEMM_UNROLL_MN - 1) & ~(GEMM_UNROLL_MN - 1);
	  
	  for (xxx = range_n[current], bufferside = 0; xxx < range_n[current + 1]; xxx += div_n, bufferside ++) {
	    
	    KERNEL_OPERATION(min_i, MIN(range_n[current + 1] - xxx, div_n), k, alpha,
			     sa, (FLOAT *)job[current].working[mypos][CACHE_LINE_SIZE * bufferside],
			     c, lda, is, xxx);
	    
	    if (is + min_i >= m_to) {
	      job[current].working[mypos][CACHE_LINE_SIZE * bufferside] &= 0;
	      WMB;
	    }
	  }	
#ifndef LOWER
	  current ++;
#else
	  current --;
#endif
	}
  }
  
  for (i = 0; i < args -> nthreads; i++) {
    if (i != mypos) {
      for (xxx = 0; xxx < DIVIDE_RATE; xxx++) {
	while (job[mypos].working[i][CACHE_LINE_SIZE * xxx] ) {YIELDING;};
      }
    }
  }
  
  return 0;
  }

static int thread_driver(blas_arg_t *args, FLOAT *sa, FLOAT *sb){

  blas_arg_t newarg;

  job_t          job[MAX_CPU_NUMBER];
  blas_queue_t queue[MAX_CPU_NUMBER];

  BLASLONG range[MAX_CPU_NUMBER + 100];

  BLASLONG num_cpu;

  BLASLONG nthreads = args -> nthreads;

  BLASLONG width, i, j, k;
  BLASLONG n, n_from, n_to;
  int  mode, mask;
  double dnum;

#ifndef COMPLEX
#ifdef XDOUBLE
  mode  =  BLAS_XDOUBLE | BLAS_REAL;
  mask  = MAX(QGEMM_UNROLL_M, QGEMM_UNROLL_N) - 1;
#elif defined(DOUBLE)
  mode  =  BLAS_DOUBLE  | BLAS_REAL;
  mask  = MAX(DGEMM_UNROLL_M, DGEMM_UNROLL_N) - 1;
#else
  mode  =  BLAS_SINGLE  | BLAS_REAL;
  mask  = MAX(SGEMM_UNROLL_M, SGEMM_UNROLL_N) - 1;
#endif  
#else
#ifdef XDOUBLE
  mode  =  BLAS_XDOUBLE | BLAS_COMPLEX;
  mask  = MAX(XGEMM_UNROLL_M, XGEMM_UNROLL_N) - 1;
#elif defined(DOUBLE)
  mode  =  BLAS_DOUBLE  | BLAS_COMPLEX;
  mask  = MAX(ZGEMM_UNROLL_M, ZGEMM_UNROLL_N) - 1;
#else
  mode  =  BLAS_SINGLE  | BLAS_COMPLEX;
  mask  = MAX(CGEMM_UNROLL_M, CGEMM_UNROLL_N) - 1;
#endif  
#endif

  newarg.m        = args -> m;
  newarg.k        = args -> k;
  newarg.a        = args -> a;
  newarg.b        = args -> b;
  newarg.c        = args -> c;
  newarg.lda      = args -> lda;
  newarg.alpha    = args -> alpha;
  newarg.common   = (void *)job;
   
  n_from = 0;
  n_to   = args -> m;

#ifndef LOWER

  range[MAX_CPU_NUMBER] = n_to - n_from;
  range[0] = 0;
  num_cpu  = 0;
  i        = 0;
  n        = n_to - n_from;

  dnum = (double)n * (double)n /(double)nthreads;

  while (i < n){
    
    if (nthreads - num_cpu > 1) {
      
      double di   = (double)i;
      
      width = (((BLASLONG)(sqrt(di * di + dnum) - di) + mask) & ~mask);
      
      if (num_cpu == 0) width = n - ((n - width) & ~mask);
      
      if ((width > n - i) || (width < mask)) width = n - i;
      
    } else {
      width = n - i;
    }

    range[MAX_CPU_NUMBER - num_cpu - 1] = range[MAX_CPU_NUMBER - num_cpu] - width;

    queue[num_cpu].mode    = mode;
    queue[num_cpu].routine = inner_thread;
    queue[num_cpu].args    = &newarg;
    queue[num_cpu].range_m = NULL;

    queue[num_cpu].sa      = NULL;
    queue[num_cpu].sb      = NULL;
    queue[num_cpu].next    = &queue[num_cpu + 1];
    
    num_cpu ++;
    i += width;
  }

   for (i = 0; i < num_cpu; i ++) queue[i].range_n = &range[MAX_CPU_NUMBER - num_cpu];

#else

  range[0] = 0;
  num_cpu  = 0;
  i        = 0;
  n        = n_to - n_from;

  dnum = (double)n * (double)n /(double)nthreads;

  while (i < n){
    
    if (nthreads - num_cpu > 1) {
      
	double di   = (double)i;
	
	width = (((BLASLONG)(sqrt(di * di + dnum) - di) + mask) & ~mask);
	
      if ((width > n - i) || (width < mask)) width = n - i;
	
    } else {
      width = n - i;
    }

    range[num_cpu + 1] = range[num_cpu] + width;
    
    queue[num_cpu].mode    = mode;
    queue[num_cpu].routine = inner_thread;
    queue[num_cpu].args    = &newarg;
    queue[num_cpu].range_m = NULL;
    queue[num_cpu].range_n = range;
    queue[num_cpu].sa      = NULL;
    queue[num_cpu].sb      = NULL;
    queue[num_cpu].next    = &queue[num_cpu + 1];
    
    num_cpu ++;
    i += width;
  }

#endif

  newarg.nthreads = num_cpu;

  if (num_cpu) {

    for (j = 0; j < num_cpu; j++) {
      for (i = 0; i < num_cpu; i++) {
	for (k = 0; k < DIVIDE_RATE; k++) {
	  job[j].working[i][CACHE_LINE_SIZE * k] = 0;
	}
      }
    }
    
    queue[0].sa = sa;
    queue[0].sb = sb;
    queue[num_cpu - 1].next = NULL;
    
    exec_blas(num_cpu, queue);
  }
 
  return 0;
}

#endif

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
#ifndef LOWER
    info = POTRF_U_SINGLE(args, NULL, NULL, sa, sb, 0); 
#else
    info = POTRF_L_SINGLE(args, NULL, NULL, sa, sb, 0); 
#endif
    return info;
  }

  n  = args -> n;
  a  = (FLOAT *)args -> a;
  lda = args -> lda;

  if (range_n) n  = range_n[1] - range_n[0];

  if (n <= GEMM_UNROLL_N * 2) {
#ifndef LOWER
    info = POTRF_U_SINGLE(args, NULL, range_n, sa, sb, 0);
#else
    info = POTRF_L_SINGLE(args, NULL, range_n, sa, sb, 0);
#endif
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
#ifndef USE_SIMPLE_THREADED_LEVEL3
      newarg.m = n - i - bk;
      newarg.k = bk;
#ifndef LOWER
      newarg.b = a + ( i       + (i + bk) * lda) * COMPSIZE;
#else
      newarg.b = a + ((i + bk) +  i       * lda) * COMPSIZE;
#endif
      newarg.c = a + ((i + bk) + (i + bk) * lda) * COMPSIZE;

      thread_driver(&newarg, sa, sb);
#else

#ifndef LOWER
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

#if 0
    HERK_THREAD_UC(&newarg, NULL, NULL, sa, sb, 0);
#else
    syrk_thread(mode | BLAS_TRANSA_N | BLAS_TRANSB_T,
                &newarg, NULL, NULL, (void *)HERK_UC, sa, sb, args -> nthreads);
#endif
#else
    newarg.m = n - i - bk;
    newarg.n = bk;
    newarg.a = a + (i      + i * lda) * COMPSIZE;
    newarg.b = a + (i + bk + i * lda) * COMPSIZE;

    gemm_thread_m(mode | BLAS_RSIDE | BLAS_TRANSA_T | BLAS_UPLO,
		  &newarg, NULL, NULL, (void *)TRSM_RCLN, sa, sb, args -> nthreads);

    newarg.n = n - i - bk;
    newarg.k = bk;
    newarg.a = a + (i + bk +  i       * lda) * COMPSIZE;
    newarg.c = a + (i + bk + (i + bk) * lda) * COMPSIZE;
    
#if 0
    HERK_THREAD_LN(&newarg, NULL, NULL, sa, sb, 0);
#else
    syrk_thread(mode | BLAS_TRANSA_N | BLAS_TRANSB_T | BLAS_UPLO,
                &newarg, NULL, NULL, (void *)HERK_LN, sa, sb, args -> nthreads);
#endif
#endif

#endif
     }
  }
  return 0;
}
