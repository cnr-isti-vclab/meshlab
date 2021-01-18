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
#include "symcopy.h"

#if! defined(HEMV) && !defined(HEMVREV)
#define MYSYMV_U	SYMV_U
#define MYSYMV_L	SYMV_L
#elif defined HEMV
#define MYSYMV_U	HEMV_U
#define MYSYMV_L	HEMV_L
#else
#define MYSYMV_U	HEMV_V
#define MYSYMV_L	HEMV_M
#endif

static int symv_kernel(blas_arg_t *args, BLASLONG *range_m, BLASLONG *range_n, FLOAT *dummy1, FLOAT *buffer, BLASLONG pos){

  FLOAT *a, *x, *y;
  BLASLONG lda, incx, incy;
  BLASLONG m_from, m_to;

  a = (FLOAT *)args -> a;
  x = (FLOAT *)args -> b;
  y = (FLOAT *)args -> c;

  lda  = args -> lda;
  incx = args -> ldb;
  incy = args -> ldc;

  m_from = 0;
  m_to   = args -> m;

  if (range_m) {
    m_from = *(range_m + 0);
    m_to   = *(range_m + 1);
  }

  if (range_n) y += *range_n * COMPSIZE;

#ifndef LOWER

  SCAL_K(m_to, 0, 0, ZERO, 
#ifdef COMPLEX
	 ZERO,
#endif
	 y, 1, NULL, 0, NULL, 0);  

  MYSYMV_U (m_to,               m_to - m_from, ONE,
#ifdef COMPLEX
	  ZERO,
#endif
	  a, lda, x, incx, y, 1, buffer);

#else

  SCAL_K(args -> m - m_from, 0, 0, ZERO, 
#ifdef COMPLEX
	 ZERO,
#endif
	 y + m_from * COMPSIZE, 1, NULL, 0, NULL, 0);  

  MYSYMV_L (args -> m - m_from, m_to - m_from, ONE,
#ifdef COMPLEX
	  ZERO,
#endif
	  a + m_from * (lda + 1) * COMPSIZE, lda, x + m_from * incx * COMPSIZE, incx, y + m_from * COMPSIZE, 1, buffer);
#endif

  return 0;
}

#ifndef COMPLEX
int CNAME(BLASLONG m, FLOAT  alpha, FLOAT *a, BLASLONG lda, FLOAT *x, BLASLONG incx, FLOAT *y, BLASLONG incy, FLOAT *buffer, int nthreads){
#else
int CNAME(BLASLONG m, FLOAT *alpha, FLOAT *a, BLASLONG lda, FLOAT *x, BLASLONG incx, FLOAT *y, BLASLONG incy, FLOAT *buffer, int nthreads){
#endif

  blas_arg_t args;
  blas_queue_t queue[MAX_CPU_NUMBER];
  BLASLONG range_m[MAX_CPU_NUMBER + 1];
  BLASLONG range_n[MAX_CPU_NUMBER];

  BLASLONG width, i, num_cpu;

  double dnum;
  int mask = 3;

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
  
  args.a = (void *)a;
  args.b = (void *)x;
  args.c = (void *)buffer;
    
  args.lda = lda;
  args.ldb = incx;
  args.ldc = incy;

  dnum = (double)m * (double)m / (double)nthreads;
  num_cpu  = 0;
  
#ifndef LOWER

  range_m[0] = 0;
  i          = 0;
    
  while (i < m){
    
    if (nthreads - num_cpu > 1) {
      
      double di = (double)i;
      width = ((BLASLONG)(sqrt(di * di + dnum) - di) + mask) & ~mask;

      if (width < 4) width = 4;
      if (width > m - i) width = m - i;
	
    } else {
      width = m - i;
    }
    
    range_m[num_cpu + 1] = range_m[num_cpu] + width;
    range_n[num_cpu] = num_cpu * (((m + 15) & ~15) + 16);
      
    queue[MAX_CPU_NUMBER - num_cpu - 1].mode    = mode;
    queue[MAX_CPU_NUMBER - num_cpu - 1].routine = symv_kernel;
    queue[MAX_CPU_NUMBER - num_cpu - 1].args    = &args;
    queue[MAX_CPU_NUMBER - num_cpu - 1].range_m = &range_m[num_cpu];
    queue[MAX_CPU_NUMBER - num_cpu - 1].range_n = &range_n[num_cpu];
    queue[MAX_CPU_NUMBER - num_cpu - 1].sa      = NULL;
    queue[MAX_CPU_NUMBER - num_cpu - 1].sb      = NULL;
    queue[MAX_CPU_NUMBER - num_cpu - 1].next    = &queue[MAX_CPU_NUMBER - num_cpu];
    
    num_cpu ++;
    i += width;
  }
  
  if (num_cpu) {
    queue[MAX_CPU_NUMBER - num_cpu].sa = NULL;
    queue[MAX_CPU_NUMBER - num_cpu].sb = buffer + num_cpu * (((m + 255) & ~255) + 16) * COMPSIZE;
    
    queue[MAX_CPU_NUMBER - 1].next = NULL;
    
    exec_blas(num_cpu, &queue[MAX_CPU_NUMBER - num_cpu]);
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

      if (width < 4) width = 4;
      if (width > m - i) width = m - i;
	
    } else {
      width = m - i;
    }
    
    range_m[num_cpu + 1] = range_m[num_cpu] + width;
    range_n[num_cpu] = num_cpu * (((m + 15) & ~15) + 16);
      
    queue[num_cpu].mode    = mode;
    queue[num_cpu].routine = symv_kernel;
    queue[num_cpu].args    = &args;
    queue[num_cpu].range_m = &range_m[num_cpu];
    queue[num_cpu].range_n = &range_n[num_cpu];
    queue[num_cpu].sa      = NULL;
    queue[num_cpu].sb      = NULL;
    queue[num_cpu].next    = &queue[num_cpu + 1];
    
    num_cpu ++;
    i += width;
  }
  
  if (num_cpu) {
    queue[0].sa = NULL;
    queue[0].sb = buffer + num_cpu * (((m + 255) & ~255) + 16) * COMPSIZE;
    
    queue[num_cpu - 1].next = NULL;
    
    exec_blas(num_cpu, queue);
  }
   
#endif

#ifndef LOWER

  for (i = 0; i < num_cpu - 1; i ++) {
   
    AXPYU_K(range_m[i + 1], 0, 0, ONE,
#ifdef COMPLEX
	    ZERO, 
#endif
	    buffer + range_n[i] * COMPSIZE, 1, buffer + range_n[num_cpu - 1] * COMPSIZE, 1, NULL, 0);
  }    

  AXPYU_K(m, 0, 0,
#ifndef COMPLEX
	  alpha,
#else
	  alpha[0], alpha[1],
#endif
	  buffer + range_n[num_cpu - 1] * COMPSIZE, 1, y, incy, NULL, 0);

#else
    
  for (i = 1; i < num_cpu; i ++) {

    AXPYU_K(m - range_m[i], 0, 0, ONE,
#ifdef COMPLEX
	    ZERO, 
#endif
	    buffer + (range_n[i] + range_m[i]) * COMPSIZE, 1, buffer + range_m[i] * COMPSIZE, 1, NULL, 0);
  }

  AXPYU_K(m, 0, 0,
#ifndef COMPLEX
	  alpha,
#else
	  alpha[0], alpha[1],
#endif
	  buffer, 1, y, incy, NULL, 0);
    
#endif
  
  return 0;
}
