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

int CNAME(int mode, blas_arg_t *arg, BLASLONG *range_m, BLASLONG *range_n, int (*function)(), void *sa, void *sb, BLASLONG nthreads) {

  blas_queue_t queue[MAX_CPU_NUMBER];
  BLASLONG range[MAX_CPU_NUMBER + 1];

  BLASLONG width, i, num_cpu;

  if (!range_m) {
    range[0] = 0;
    i        = arg -> m;
  } else {
    range[0] = range_m[0];
    i        = range_m[1] - range_m[0];
  }

  num_cpu  = 0;

  while (i > 0){
    
    width  = blas_quickdivide(i + nthreads - num_cpu - 1, nthreads - num_cpu);

    i -= width;
    if (i < 0) width = width + i;

    range[num_cpu + 1] = range[num_cpu] + width;

    queue[num_cpu].mode    = mode;
    queue[num_cpu].routine = function;
    queue[num_cpu].args    = arg;
    queue[num_cpu].range_m = &range[num_cpu];
    queue[num_cpu].range_n = range_n;
    queue[num_cpu].sa      = NULL;
    queue[num_cpu].sb      = NULL;
    queue[num_cpu].next    = &queue[num_cpu + 1];
    num_cpu ++;
  }
  
  if (num_cpu) {
    queue[0].sa = sa;
    queue[0].sb = sb;

    queue[num_cpu - 1].next = NULL;
    
    exec_blas(num_cpu, queue);
  }
   
  return 0;
}
