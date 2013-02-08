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

const static FLOAT dp1 = 1.;

int CNAME(BLASLONG n, BLASLONG k, FLOAT *a, BLASLONG lda, FLOAT *b, BLASLONG incb, void *buffer){

  BLASLONG i;
  FLOAT *gemvbuffer = (FLOAT *)buffer;
  FLOAT *B = b;
  BLASLONG length;
#if (TRANSA == 2) || (TRANSA == 4)
  FLOAT _Complex temp;
#endif
#ifndef UNIT
  FLOAT ar, ai, br, bi, ratio, den;
#endif

  if (incb != 1) {
    B = buffer;
    gemvbuffer = (FLOAT *)(((BLASLONG)buffer + n * sizeof(FLOAT) * COMPSIZE+ 4095) & ~4095);
    COPY_K(n, b, incb, buffer, 1);
  }
  
  a += (n - 1) * lda * COMPSIZE;

  for (i = n - 1; i >= 0; i--) {

#if (TRANSA == 2) || (TRANSA == 4)
    length  = n - i - 1;
    if (length > k) length = k;

    if (length > 0) {
#if TRANSA == 2
      temp = DOTU_K(length, a + COMPSIZE, 1, B + (i + 1) * COMPSIZE, 1);
#else
      temp = DOTC_K(length, a + COMPSIZE, 1, B + (i + 1) * COMPSIZE, 1);
#endif

      B[i * 2 + 0] -= CREAL(temp);
      B[i * 2 + 1] -= CIMAG(temp);
    }
#endif

#ifndef UNIT
#if (TRANSA == 1) || (TRANSA == 3)
    ar = a[k * 2 + 0];
    ai = a[k * 2 + 1];
#else
    ar = a[0];
    ai = a[1];
#endif
    
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
    
    br = B[i * 2 + 0];
    bi = B[i * 2 + 1];
    
    B[i * 2 + 0] = ar*br - ai*bi;
    B[i * 2 + 1] = ar*bi + ai*br;
#endif

#if (TRANSA == 1) || (TRANSA == 3)
    length  = i;
    if (length > k) length = k;

    if (length > 0) {
#if   TRANSA == 1
      AXPYU_K(length, 0, 0,
	      -B[i * 2 + 0],  -B[i * 2 + 1],
	      a + (k - length) * COMPSIZE, 1, B + (i - length) * COMPSIZE, 1, NULL, 0);
#else
      AXPYC_K(length, 0, 0,
	      -B[i * 2 + 0],  -B[i * 2 + 1],
	      a + (k - length) * COMPSIZE, 1, B + (i - length) * COMPSIZE, 1, NULL, 0);
#endif

    }
#endif

    a -= lda * COMPSIZE;
  }
    
  if (incb != 1) {
    COPY_K(n, buffer, 1, b, incb);
  }

  return 0;
}

