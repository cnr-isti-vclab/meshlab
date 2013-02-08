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

#ifndef TRANSA
#if   !defined(CONJ) && !defined(XCONJ)
#define GEMV	GEMV_N
#elif  defined(CONJ) && !defined(XCONJ)
#define GEMV	GEMV_R
#elif !defined(CONJ) &&  defined(XCONJ)
#define GEMV	GEMV_O
#else
#define GEMV	GEMV_S
#endif
#else
#if   !defined(CONJ) && !defined(XCONJ)
#define GEMV	GEMV_T
#elif  defined(CONJ) && !defined(XCONJ)
#define GEMV	GEMV_C
#elif !defined(CONJ) &&  defined(XCONJ)
#define GEMV	GEMV_U
#else
#define GEMV	GEMV_D
#endif
#endif

static int gemv_kernel(blas_arg_t *args, BLASLONG *range_m, BLASLONG *range_n, FLOAT *dummy1, FLOAT *buffer, BLASLONG pos){

  FLOAT *a, *x, *y;
  BLASLONG lda, incx, incy;
  BLASLONG m_from, m_to, n_from, n_to;

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

    a += m_from        * COMPSIZE;
#ifndef TRANSA
    y += m_from * incy * COMPSIZE;
#endif
  }

  n_from = 0;
  n_to   = args -> n;

  if (range_n) {
    n_from = *(range_n + 0);
    n_to   = *(range_n + 1);

    a += n_from * lda  * COMPSIZE;
#ifdef TRANSA
    y += n_from * incy * COMPSIZE;
#endif
  }

  //  fprintf(stderr, "M_From = %d  M_To = %d  N_From = %d  N_To = %d\n", m_from, m_to, n_from, n_to);

  GEMV(m_to - m_from, n_to - n_from, 0,
       *((FLOAT *)args -> alpha + 0),
#ifdef COMPLEX
       *((FLOAT *)args -> alpha + 1),
#endif
       a, lda, x, incx, y, incy, buffer);
  
  return 0;
}

#ifndef COMPLEX
int CNAME(BLASLONG m, BLASLONG n, FLOAT  alpha, FLOAT *a, BLASLONG lda, FLOAT *x, BLASLONG incx, FLOAT *y, BLASLONG incy, FLOAT *buffer, int nthreads){
#else
int CNAME(BLASLONG m, BLASLONG n, FLOAT *alpha, FLOAT *a, BLASLONG lda, FLOAT *x, BLASLONG incx, FLOAT *y, BLASLONG incy, FLOAT *buffer, int nthreads){
#endif

  blas_arg_t args;
  blas_queue_t queue[MAX_CPU_NUMBER];
  BLASLONG range[MAX_CPU_NUMBER + 1];

  BLASLONG width, i, num_cpu;

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
  args.n = n;
  
  args.a = (void *)a;
  args.b = (void *)x;
  args.c = (void *)y;
    
  args.lda = lda;
  args.ldb = incx;
  args.ldc = incy;

#ifndef COMPLEX
  args.alpha = (void *)&alpha;
#else
  args.alpha = (void *) alpha;
#endif

  num_cpu  = 0;
  
  range[0] = 0;
#ifndef TRANSA
  i        = m;
#else
  i        = n;
#endif
    
  while (i > 0){

    width  = blas_quickdivide(i + nthreads - num_cpu - 1, nthreads - num_cpu);
    if (width < 4) width = 4;
    if (i < width) width = i;

    range[num_cpu + 1] = range[num_cpu] + width;
      
    queue[num_cpu].mode    = mode;
    queue[num_cpu].routine = gemv_kernel;
    queue[num_cpu].args    = &args;
#ifndef TRANSA
    queue[num_cpu].range_m = &range[num_cpu];
    queue[num_cpu].range_n = NULL;
#else
    queue[num_cpu].range_m = NULL;
    queue[num_cpu].range_n = &range[num_cpu];
#endif
    queue[num_cpu].sa      = NULL;
    queue[num_cpu].sb      = NULL;
    queue[num_cpu].next    = &queue[num_cpu + 1];
    
    num_cpu ++;
    i -= width;
  }

  if (num_cpu) {
    queue[0].sa = NULL;
    queue[0].sb = buffer;
    queue[num_cpu - 1].next = NULL;
    
    exec_blas(num_cpu, queue);
  }
  
  return 0;
}
