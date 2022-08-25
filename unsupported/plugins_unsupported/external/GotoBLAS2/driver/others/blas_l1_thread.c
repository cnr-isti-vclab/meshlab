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

int blas_level1_thread(int mode, BLASLONG m, BLASLONG n, BLASLONG k, void *alpha,
		       void *a, BLASLONG lda,
		       void *b, BLASLONG ldb, 
		       void *c, BLASLONG ldc, int (*function)(), int nthreads){
  
  blas_queue_t queue[MAX_CPU_NUMBER];
  blas_arg_t   args [MAX_CPU_NUMBER];

  BLASLONG i, width, astride, bstride;
  int num_cpu, calc_type;

  calc_type = (mode & BLAS_PREC) + ((mode & BLAS_COMPLEX) != 0) + 2;
  
  mode |= BLAS_LEGACY;

  for (i = 0; i < nthreads; i++) blas_queue_init(&queue[i]);

  num_cpu = 0;
  i = m;
  
  while (i > 0){
    
    /* Adjust Parameters */
    width  = blas_quickdivide(i + nthreads - num_cpu - 1,
			      nthreads - num_cpu);

    i -= width;
    if (i < 0) width = width + i;
    
    astride = width * lda;

    if (!(mode & BLAS_TRANSB_T)) {
      bstride = width * ldb;
    } else {
      bstride = width;
    }

    astride <<= calc_type;
    bstride <<= calc_type;

    args[num_cpu].m = width;
    args[num_cpu].n = n;
    args[num_cpu].k = k;
    args[num_cpu].a = (void *)a;
    args[num_cpu].b = (void *)b;
    args[num_cpu].c = (void *)c;
    args[num_cpu].lda = lda;
    args[num_cpu].ldb = ldb;
    args[num_cpu].ldc = ldc;
    args[num_cpu].alpha = alpha;

    queue[num_cpu].mode    = mode;
    queue[num_cpu].routine = function;
    queue[num_cpu].args    = &args[num_cpu];
    queue[num_cpu].next    = &queue[num_cpu + 1];
    
    a = (void *)((BLASULONG)a + astride);
    b = (void *)((BLASULONG)b + bstride);
  
    num_cpu ++;
  }

  if (num_cpu) {
    queue[num_cpu - 1].next = NULL;

    exec_blas(num_cpu, queue);
  }

  return 0;
}
