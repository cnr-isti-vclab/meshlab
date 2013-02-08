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
#include <ctype.h>
#include "common.h"

const static FLOAT dm1 = -1.;

int CNAME(BLASLONG m, FLOAT *a, BLASLONG lda, FLOAT *b, BLASLONG incb, void *buffer){
  
  BLASLONG i, is, min_i;
#if (TRANSA == 2) || (TRANSA == 4)
  FLOAT _Complex result;
#endif
#ifndef UNIT
  FLOAT ar, ai, br, bi, ratio, den;
#endif
  FLOAT *gemvbuffer = (FLOAT *)buffer;
  FLOAT *B = b;

  if (incb != 1) {
    B = buffer;
    gemvbuffer = (FLOAT *)(((BLASLONG)buffer + m * sizeof(FLOAT) * 2 + 4095) & ~4095);
    COPY_K(m, b, incb, buffer, 1);
  }

  for (is =0; is < m; is += DTB_ENTRIES){

    min_i = MIN(m - is, DTB_ENTRIES);

#if (TRANSA == 2) || (TRANSA == 4)
    if (is > 0){
#if TRANSA == 2
      GEMV_T(is, min_i, 0, dm1, ZERO,
	      a + is * lda * COMPSIZE,  lda,
	      B,                 1,
	      B + is * COMPSIZE, 1, gemvbuffer);
#else
      GEMV_C(is, min_i, 0, dm1, ZERO,
	      a + is * lda  * COMPSIZE,  lda,
	      B,                 1,
	      B + is * COMPSIZE, 1, gemvbuffer);
#endif
    }
#endif

    for (i = 0; i < min_i; i++) {
      FLOAT *AA = a + (is + (i + is) * lda) * COMPSIZE;
      FLOAT *BB = B + is * COMPSIZE;

#if (TRANSA == 2) || (TRANSA == 4)
      if (i > 0) {
#if TRANSA == 2
      result = DOTU_K(i, AA, 1, BB, 1);
#else
      result = DOTC_K(i, AA, 1, BB, 1);
#endif

      BB[i * COMPSIZE + 0] -= CREAL(result);
      BB[i * COMPSIZE + 1] -= CIMAG(result);
    }
#endif

#ifndef UNIT
      ar = AA[i * COMPSIZE + 0];
      ai = AA[i * COMPSIZE + 1];
      
      if (fabs(ar) >= fabs(ai)){
	ratio = ai / ar;
	den = 1./(ar * ( 1 + ratio * ratio));
	
	ar =  den;
#if TRANSA < 3
	ai = -ratio * den;
#else
	ai =  ratio * den;
#endif
      } else {
	ratio = ar / ai;
	den = 1./(ai * ( 1 + ratio * ratio));
	ar =  ratio * den;
#if TRANSA < 3
	ai = -den;
#else
	ai =  den;
#endif
    }

      br = BB[i * COMPSIZE + 0];
      bi = BB[i * COMPSIZE + 1];
      
      BB[i * COMPSIZE + 0] = ar*br - ai*bi;
      BB[i * COMPSIZE + 1] = ar*bi + ai*br;
#endif


#if (TRANSA == 1) || (TRANSA == 3)
      if (i < min_i - 1) {
#if TRANSA == 1
	AXPYU_K(min_i - i  - 1 , 0, 0,
	       - BB[i * COMPSIZE + 0], - BB[i * COMPSIZE + 1],
	       AA + (i + 1) * COMPSIZE, 1, BB + (i + 1) * COMPSIZE, 1, NULL, 0);
#else
	AXPYC_K(min_i - i  - 1 , 0, 0,
	       - BB[i * COMPSIZE + 0], - BB[i * COMPSIZE + 1],
	       AA + (i + 1) * COMPSIZE, 1, BB + (i + 1) * COMPSIZE, 1, NULL, 0);
#endif
    }
#endif
    }

#if (TRANSA == 1) || (TRANSA == 3)
    if (m - is > min_i){
#if   TRANSA == 1
      GEMV_N(m - is - min_i, min_i, 0, dm1, ZERO,
	      a + (is + min_i + is * lda) * COMPSIZE, lda,
	      B + is           * COMPSIZE, 1,
	      B + (is + min_i) * COMPSIZE, 1, gemvbuffer);
#else
      GEMV_R(m - is - min_i, min_i, 0, dm1, ZERO,
	      a + (is + min_i + is * lda) * COMPSIZE, lda,
	      B + is           * COMPSIZE, 1,
	      B + (is + min_i) * COMPSIZE, 1, gemvbuffer);
#endif
    }
#endif
  }

  if (incb != 1) {
    COPY_K(m, buffer, 1, b, incb);
  }

  return 0;
}

