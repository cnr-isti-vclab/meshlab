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
#include <math.h>
#include "common.h"

static FLOAT dm1 = -1.;

#ifndef SQRT
#define SQRT(x)	sqrt(x)
#endif

blasint CNAME(blas_arg_t *args, BLASLONG *range_m, BLASLONG *range_n, FLOAT *sa, FLOAT *sb, BLASLONG myid) {

  BLASLONG n, lda;
  FLOAT *a;

  FLOAT ajj[2];
  BLASLONG i, j;

  n      = args -> n;
  a      = (FLOAT *)args -> a;
  lda    = args -> lda;
  
  if (range_n) {
    n      = range_n[1] - range_n[0];
    a     += range_n[0] * (lda + 1) * COMPSIZE;
  }

  for (j = 0; j < n; j++) {

    ajj[0] = DOTC_K(j, a, 1, a, 1);
    GET_IMAGE(ajj[1]);

    ajj[0] = *(a + j * 2) - ajj[0]; 

    if (ajj[0] <= 0){
      *(a + j * 2 + 0) = ajj[0];
      *(a + j * 2 + 1) = ZERO;
      return j + 1;
    }

    ajj[0] = SQRT(ajj[0]);
    *(a + j * 2 + 0)  = ajj[0];
    *(a + j * 2 + 1)  = ZERO;

    i = n - j - 1;

    if (i > 0){
      GEMV_U(j, i, 0, dm1, ZERO,
	      a + lda * 2, lda,
	      a, 1,
	      a + (j + lda) * 2, lda, sb);
      
      SCAL_K(i, 0, 0, ONE / ajj[0], ZERO,
	      a + (j + lda) * 2, lda, NULL, 0, NULL, 0);
    }

    a += 2 * lda;
  }

  return 0;
}
