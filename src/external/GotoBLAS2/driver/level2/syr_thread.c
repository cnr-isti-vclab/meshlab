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
#include <stdlib.h>
#include "common.h"

static int syr_kernel(blas_arg_t *args, BLASLONG *range_m, BLASLONG *range_n, FLOAT *dummy1, FLOAT *buffer, BLASLONG pos){

  FLOAT *a, *x;
  BLASLONG lda, incx;
  BLASLONG i, m_from, m_to;
  FLOAT alpha_r;
#if defined(COMPLEX) && !defined(HER) && !defined(HERREV)
  FLOAT alpha_i;
#endif


  x = (FLOAT *)args -> a;
  a = (FLOAT *)args -> b;

  incx = args -> lda;
  lda  = args -> ldb;

  alpha_r = *((FLOAT *)args -> alpha + 0);
#if defined(COMPLEX) && !defined(HER) && !defined(HERREV)
  alpha_i = *((FLOAT *)args -> alpha + 1);
#endif

  m_from = 0;
  m_to   = args -> m;

  if (range_m) {
    m_from = *(range_m + 0);
    m_to   = *(range_m + 1);
  }

  if (incx != 1) {
#ifndef LOWER
    COPY_K(m_to, x, incx, buffer, 1);
#else
    COPY_K(args -> m - m_from, x + m_from * incx * COMPSIZE, incx, buffer + m_from * COMPSIZE, 1);
#endif

    x = buffer;
  }

  a += m_from * lda * COMPSIZE;

  for (i = m_from; i < m_to; i++){
#if !defined(HER) && !defined(HERREV)
#ifndef COMPLEX
    if (x[i * COMPSIZE] != ZERO) {
#ifndef LOWER
      AXPYU_K(i + 1,         0, 0, alpha_r * x[i], x,     1, a,     1, NULL, 0);
#else
      AXPYU_K(args -> m - i, 0, 0, alpha_r * x[i], x + i, 1, a + i, 1, NULL, 0);
#endif
    }
#else
    if ((x[i * COMPSIZE + 0] != ZERO) || (x[i * COMPSIZE + 1] != ZERO)) {
#ifndef LOWER
      AXPYU_K(i + 1,         0, 0, 
	      alpha_r * x[i * COMPSIZE + 0] - alpha_i * x[i * COMPSIZE + 1],
	      alpha_i * x[i * COMPSIZE + 0] + alpha_r * x[i * COMPSIZE + 1],
	      x,                1, a,                1, NULL, 0);
#else
      AXPYU_K(args -> m - i, 0, 0,
	      alpha_r * x[i * COMPSIZE + 0] - alpha_i * x[i * COMPSIZE + 1],
	      alpha_i * x[i * COMPSIZE + 0] + alpha_r * x[i * COMPSIZE + 1],
	      x + i * COMPSIZE, 1, a + i * COMPSIZE, 1, NULL, 0);
#endif
    }
#endif
#else
    if ((x[i * COMPSIZE + 0] != ZERO) || (x[i * COMPSIZE + 1] != ZERO)) {
#ifndef HERREV
#ifndef LOWER
      AXPYU_K(i + 1,         0, 0, 
	      alpha_r * x[i * COMPSIZE + 0], -alpha_r * x[i * COMPSIZE + 1],
	      x,                1, a,                1, NULL, 0);
#else
      AXPYU_K(args -> m - i, 0, 0,
	      alpha_r * x[i * COMPSIZE + 0], -alpha_r * x[i * COMPSIZE + 1],
	      x + i * COMPSIZE, 1, a + i * COMPSIZE, 1, NULL, 0);
#endif
#else
#ifndef LOWER
      AXPYC_K(i + 1,         0, 0, 
	      alpha_r * x[i * COMPSIZE + 0],  alpha_r * x[i * COMPSIZE + 1],
	      x,                1, a,                1, NULL, 0);
#else
      AXPYC_K(args -> m - i, 0, 0,
	      alpha_r * x[i * COMPSIZE + 0],  alpha_r * x[i * COMPSIZE + 1],
	      x + i * COMPSIZE, 1, a + i * COMPSIZE, 1, NULL, 0);
#endif
#endif

    }
      a[i * COMPSIZE + 1] = ZERO;
#endif
      a += lda * COMPSIZE;

  }
  
  return 0;
}

#if !defined(COMPLEX) || defined(HER) || defined(HERREV)
int CNAME(BLASLONG m, FLOAT  alpha, FLOAT *x, BLASLONG incx, FLOAT *a, BLASLONG lda, FLOAT *buffer, int nthreads){
#else
int CNAME(BLASLONG m, FLOAT *alpha, FLOAT *x, BLASLONG incx, FLOAT *a, BLASLONG lda, FLOAT *buffer, int nthreads){
#endif

  blas_arg_t args;
  blas_queue_t queue[MAX_CPU_NUMBER];
  BLASLONG range_m[MAX_CPU_NUMBER + 1];

  BLASLONG width, i, num_cpu;

  double dnum;
  int mask = 7;

#ifdef SMP
#ifndef COMPLEX
#ifdef XDOUBLE
  int mode  =  BLAS_XDOUBLE | BLAS_REAL;
#elif defined(DOUBLE)
  int mode  =  BLAS_DOUBLE  | BLAS_REAL;
#else
  int mode  =  BLAS_SINGLE  | BLAS_REAL;
#endif  
#else
#ifdef XDOUBLE
  int mode  =  BLAS_XDOUBLE | BLAS_COMPLEX;
#elif defined(DOUBLE)
  int mode  =  BLAS_DOUBLE  | BLAS_COMPLEX;
#else
  int mode  =  BLAS_SINGLE  | BLAS_COMPLEX;
#endif  
#endif
#endif

  args.m = m;
  
  args.a = (void *)x;
  args.b = (void *)a;
    
  args.lda = incx;
  args.ldb = lda;
#if !defined(COMPLEX) || defined(HER) || defined(HERREV)
  args.alpha = (void *)&alpha;
#else
  args.alpha = (void *)alpha;
#endif

  dnum = (double)m * (double)m / (double)nthreads;
  num_cpu  = 0;
  
#ifndef LOWER

  range_m[MAX_CPU_NUMBER] = m;
  i          = 0;
    
  while (i < m){
    
    if (nthreads - num_cpu > 1) {
      
      double di = (double)(m - i);
      if (di * di - dnum > 0) {
	width = ((BLASLONG)(-sqrt(di * di - dnum) + di) + mask) & ~mask;
      } else {
	width = m - i;
      }

      if (width < 16) width = 16;
      if (width > m - i) width = m - i;
	
    } else {
      width = m - i;
    }
    
    range_m[MAX_CPU_NUMBER - num_cpu - 1] = range_m[MAX_CPU_NUMBER - num_cpu] - width;
      
    queue[num_cpu].mode    = mode;
    queue[num_cpu].routine = syr_kernel;
    queue[num_cpu].args    = &args;
    queue[num_cpu].range_m = &range_m[MAX_CPU_NUMBER - num_cpu - 1];
    queue[num_cpu].range_n = NULL;
    queue[num_cpu].sa      = NULL;
    queue[num_cpu].sb      = NULL;
    queue[num_cpu].next    = &queue[num_cpu + 1];
    
    num_cpu ++;
    i += width;
  }
  
#else

  range_m[0] = 0;
  i          = 0;
    
  while (i < m){
    
    if (nthreads - num_cpu > 1) {
      
      double di = (double)(m - i);
      if (di * di - dnum > 0) {
	width = ((BLASLONG)(-sqrt(di * di - dnum) + di) + mask) & ~mask;
      } else {
	width = m - i;
      }

      if (width < 16) width = 16;
      if (width > m - i) width = m - i;
	
    } else {
      width = m - i;
    }
    
    range_m[num_cpu + 1] = range_m[num_cpu] + width;
      
    queue[num_cpu].mode    = mode;
    queue[num_cpu].routine = syr_kernel;
    queue[num_cpu].args    = &args;
    queue[num_cpu].range_m = &range_m[num_cpu];
    queue[num_cpu].range_n = NULL;
    queue[num_cpu].sa      = NULL;
    queue[num_cpu].sb      = NULL;
    queue[num_cpu].next    = &queue[num_cpu + 1];
    
    num_cpu ++;
    i += width;
  }
  
#endif

  if (num_cpu) {
    queue[0].sa = NULL;
    queue[0].sb = buffer;
    
    queue[num_cpu - 1].next = NULL;
    
    exec_blas(num_cpu, queue);
  }
   
  return 0;
}
