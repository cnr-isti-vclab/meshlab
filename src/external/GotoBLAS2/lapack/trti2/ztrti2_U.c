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

#ifdef UNIT
#define ZTRMV	ZTRMV_NUU
#else
#define ZTRMV	ZTRMV_NUN
#endif

blasint CNAME(blas_arg_t *args, BLASLONG *range_m, BLASLONG *range_n, FLOAT *sa, FLOAT *sb, BLASLONG myid) {

  BLASLONG  n, lda;
  FLOAT *a;

  FLOAT ajj_r, ajj_i;
#ifndef UNIT
  FLOAT ratio, den;
#endif
  BLASLONG j;

  n      = args -> n;
  a      = (FLOAT *)args -> a;
  lda    = args -> lda;
  
  if (range_n) {
    n      = range_n[1] - range_n[0];
    a     += range_n[0] * (lda + 1) * COMPSIZE;
  }

  for (j = 0; j < n; j++) {

    ajj_r =  ONE;
    ajj_i =  ZERO;

#ifndef UNIT
    ajj_r = *(a + (j + j * lda) * COMPSIZE + 0);
    ajj_i = *(a + (j + j * lda) * COMPSIZE + 1);


  if (fabs(ajj_r) >= fabs(ajj_i)){
    ratio = ajj_i / ajj_r;
    den   = 1. / (ajj_r * ( 1 + ratio * ratio));
    ajj_r =  den;
    ajj_i = -ratio * den;
  } else {
    ratio = ajj_r / ajj_i;
    den   = 1. /(ajj_i * ( 1 + ratio * ratio));
    ajj_r =  ratio * den;
    ajj_i = -den;
  }

  *(a + (j + j * lda) * COMPSIZE + 0) = ajj_r;
  *(a + (j + j * lda) * COMPSIZE + 1) = ajj_i;
#endif

  ZTRMV (j,
	 a                     , lda, 
	 a + j * lda * COMPSIZE, 1,
	 sb);
  
  SCAL_K(j, 0, 0, 
	  -ajj_r, -ajj_i,
	  a + j * lda * COMPSIZE, 1,
	  NULL, 0, NULL, 0);
  
  }

  return 0;
}
