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
#define TRMV	TRMV_NUU
#else
#define TRMV	TRMV_NUN
#endif

blasint CNAME(blas_arg_t *args, BLASLONG *range_m, BLASLONG *range_n, FLOAT *sa, FLOAT *sb, BLASLONG myid) {

  BLASLONG  n, lda;
  FLOAT *a;

  FLOAT ajj;
  BLASLONG j;

  n      = args -> n;
  a      = (FLOAT *)args -> a;
  lda    = args -> lda;
  
  if (range_n) {
    n      = range_n[1] - range_n[0];
    a     += range_n[0] * (lda + 1) * COMPSIZE;
  }

  for (j = 0; j < n; j++) {

    ajj =  ONE;

#ifndef UNIT
    ajj /= *(a + j + j * lda);
    *(a + j + j * lda) = ajj;
#endif

    TRMV  (j,
	   a          , lda, 
	   a + j * lda, 1,
	   sb);

    SCAL_K(j, 0, 0, 
	   -ajj, 
	   a + j * lda, 1,
	   NULL, 0, NULL, 0);

  }

  return 0;
}
