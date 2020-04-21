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

static FLOAT dp1 =  1.;

blasint CNAME(blas_arg_t *args, BLASLONG *range_m, BLASLONG *range_n, FLOAT *sa, FLOAT *sb, BLASLONG myid) {

  BLASLONG n, lda;
  FLOAT *a;

  FLOAT temp[2];
  BLASLONG i;

  n      = args -> n;
  a      = (FLOAT *)args -> a;
  lda    = args -> lda;
  
  if (range_n) {
    n      = range_n[1] - range_n[0];
    a     += range_n[0] * (lda + 1) * COMPSIZE;
  }
  for (i = 0; i < n; i++) {

    SCAL_K(i + 1, 0, 0, 
	    *(a + (i + i * lda) * COMPSIZE + 0), ZERO,
	    a + i * lda * COMPSIZE, 1, NULL, 0, NULL, 0);
    
    if (i < n - 1) {
      temp[0] = DOTC_K(n - i - 1, a + (i + (i + 1) * lda) * COMPSIZE, lda, a + (i + (i + 1) * lda) * COMPSIZE, lda);
      GET_IMAGE(temp[1]);
      
      *(a + (i + i * lda) * COMPSIZE + 0) += temp[0];
      *(a + (i + i * lda) * COMPSIZE + 1)  = ZERO;
      
      GEMV_O(i, n - i - 1, 0, dp1, ZERO,
	      a + (    (i + 1) * lda) * COMPSIZE, lda,
	      a + (i + (i + 1) * lda) * COMPSIZE, lda,
	      a + (     i      * lda) * COMPSIZE, 1, sb);
    }
  }

  return 0;
}
