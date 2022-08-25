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
static FLOAT dm1 = -1.;

blasint CNAME(blas_arg_t *args, BLASLONG *range_m, BLASLONG *range_n, FLOAT *sa, FLOAT *sb, BLASLONG myid) {

  BLASLONG m, n, lda;
  blasint *ipiv, offset;
  FLOAT *a;

  FLOAT temp1, temp2;
  blasint i, j;
  blasint ip, jp;
  blasint info;
  BLASLONG len;
  FLOAT *b;

  m      = args -> m;
  n      = args -> n;
  a      = (FLOAT *)args -> a;
  lda    = args -> lda;
  ipiv   = (blasint *)args -> c;
  offset = 0;
  
  if (range_n) {
    m     -= range_n[0];
    n      = range_n[1] - range_n[0];
    offset = range_n[0];
    a     += range_n[0] * (lda + 1) * COMPSIZE;
  }

  info = 0;
  b = a;
  
  for (j = 0; j < n; j++) {

    len = MIN(j, m);

    for (i = 0; i < len; i++) {
      ip = ipiv[i + offset] - 1 - offset; 
      if (ip != i) {
	temp1 = *(b + i);
	temp2 = *(b + ip);
	*(b + i) = temp2;
	*(b + ip) = temp1;
      }
    }
    
    for (i = 1; i < len; i++) {
      b[i] -= DOTU_K(i, a + i, lda, b, 1);
    }

    if (j < m) {
      GEMV_N(m - j, j, 0, dm1,  a + j, lda, b, 1, b + j, 1, sb);

      jp = j + IAMAX_K(m - j, b + j, 1);
      ipiv[j + offset] = jp + offset;
      jp--;
      temp1 = *(b + jp);

      if (temp1 != ZERO) {
	temp1 = dp1 / temp1;

	if (jp != j) {
	  SWAP_K(j + 1, 0, 0, ZERO, a + j, lda, a + jp, lda, NULL, 0);
	}
	if (j + 1 < m) {
	  SCAL_K(m - j - 1, 0, 0, temp1, b + j + 1, 1, NULL, 0, NULL, 0);
	}
      } else {
	if (!info) info = j + 1;
      }
    }
    b += lda;
  }
  return info;
}
