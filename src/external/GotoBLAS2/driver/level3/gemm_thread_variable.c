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

int CNAME(int mode,
	  blas_arg_t *arg, BLASLONG *range_m, BLASLONG *range_n,
	  int (*function)(), void *sa, void *sb, BLASLONG divM, BLASLONG divN) {

  blas_queue_t queue[MAX_CPU_NUMBER];

  BLASLONG range_M[MAX_CPU_NUMBER + 1], range_N[MAX_CPU_NUMBER + 1];
  BLASLONG procs, num_cpu_m, num_cpu_n;

  BLASLONG width, i, j;

  if (!range_m) {
    range_M[0] = 0;
    i          = arg -> m;
  } else {
    range_M[0] = range_M[0];
    i          = range_M[1] - range_M[0];
  }

  num_cpu_m  = 0;

  while (i > 0){
    
    width  = blas_quickdivide(i + divM - num_cpu_m - 1, divM - num_cpu_m);

    i -= width;
    if (i < 0) width = width + i;

    range_M[num_cpu_m + 1] = range_M[num_cpu_m] + width;

    num_cpu_m ++;
  }

  if (!range_n) {
    range_N[0] = 0;
    i          = arg -> n;
  } else {
    range_N[0] = range_n[0];
    i          = range_n[1] - range_n[0];
  }

  num_cpu_n  = 0;

  while (i > 0){
    
    width  = blas_quickdivide(i + divN - num_cpu_n - 1, divN - num_cpu_n);

    i -= width;
    if (i < 0) width = width + i;

    range_N[num_cpu_n + 1] = range_N[num_cpu_n] + width;

    num_cpu_n ++;
  }

  procs = 0;

  for (j = 0; j < num_cpu_n; j++) {
    for (i = 0; i < num_cpu_m; i++) {

    queue[procs].mode    = mode;
    queue[procs].routine = function;
    queue[procs].args    = arg;
    queue[procs].range_m = &range_M[i];
    queue[procs].range_n = &range_N[j];
    queue[procs].sa      = NULL;
    queue[procs].sb      = NULL;
    queue[procs].next    = &queue[procs + 1];

    procs ++;
    }
  }
  
  if (procs) {
    queue[0].sa = sa;
    queue[0].sb = sb;

    queue[procs - 1].next = NULL;

    exec_blas(procs, queue);
  }
  
  return 0;
}

