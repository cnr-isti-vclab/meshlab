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

#ifndef XCONJ
#define AXPY	AXPYU_K
#else
#define AXPY	AXPYC_K
#endif

static int ger_kernel(blas_arg_t *args, BLASLONG *range_m, BLASLONG *range_n, FLOAT *dummy1, FLOAT *buffer, BLASLONG pos){

  FLOAT *a, *x, *y;
  FLOAT alpha_r;
#ifdef COMPLEX
  FLOAT alpha_i;
#endif
  BLASLONG lda, incx, incy;
  BLASLONG m, n_from, n_to;
  BLASLONG i;

  x = (FLOAT *)args -> a;
  y = (FLOAT *)args -> b;
  a = (FLOAT *)args -> c;

  incx = args -> lda;
  incy = args -> ldb;
  lda  = args -> ldc;

  m = args -> m;

  alpha_r = *((FLOAT *)args -> alpha + 0);
#ifdef COMPLEX
  alpha_i = *((FLOAT *)args -> alpha + 1);
#endif

  n_from = 0;
  n_to   = args -> n;

  if (range_n) {
    n_from = *(range_n + 0);
    n_to   = *(range_n + 1);

    y += n_from * incy * COMPSIZE;
    a += n_from * lda  * COMPSIZE;
  }

  if (incx != 1) {
    COPY_K(m, x, incx, buffer, 1);
    x = buffer;
  }

  for (i = n_from; i < n_to; i ++) {

    AXPY(m, 0, 0,
#ifndef COMPLEX
	 alpha_r * *y,
#else
#ifndef CONJ
	 alpha_r * *(y + 0) - alpha_i * *(y + 1),   alpha_r * *(y + 1) + alpha_i * *(y + 0),
#else
	 alpha_r * *(y + 0) + alpha_i * *(y + 1), - alpha_r * *(y + 1) + alpha_i * *(y + 0),
#endif
#endif
	 x, 1, a, 1, NULL, 0);
    
    y += incy * COMPSIZE;
    a += lda  * COMPSIZE;
  }

  return 0;
}

#ifndef COMPLEX
int CNAME(BLASLONG m, BLASLONG n, FLOAT  alpha, FLOAT *x, BLASLONG incx, FLOAT *y, BLASLONG incy, FLOAT *a, BLASLONG lda, FLOAT *buffer, int nthreads){
#else
int CNAME(BLASLONG m, BLASLONG n, FLOAT *alpha, FLOAT *x, BLASLONG incx, FLOAT *y, BLASLONG incy, FLOAT *a, BLASLONG lda, FLOAT *buffer, int nthreads){
#endif

  blas_arg_t args;
  blas_queue_t queue[MAX_CPU_NUMBER];
  BLASLONG range_n[MAX_CPU_NUMBER + 1];

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
  
  args.a = (void *)x;
  args.b = (void *)y;
  args.c = (void *)a;
    
  args.lda = incx;
  args.ldb = incy;
  args.ldc = lda;

#ifndef COMPLEX
  args.alpha = (void *)&alpha;
#else
  args.alpha = (void *) alpha;
#endif

  num_cpu  = 0;
  
  range_n[0] = 0;
  i          = n;
    
  while (i > 0){
    
    width  = blas_quickdivide(i + nthreads - num_cpu - 1, nthreads - num_cpu);
    if (width < 4) width = 4;
    if (i < width) width = i;

    range_n[num_cpu + 1] = range_n[num_cpu] + width;
      
    queue[num_cpu].mode    = mode;
    queue[num_cpu].routine = ger_kernel;
    queue[num_cpu].args    = &args;
    queue[num_cpu].range_n = &range_n[num_cpu];
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
