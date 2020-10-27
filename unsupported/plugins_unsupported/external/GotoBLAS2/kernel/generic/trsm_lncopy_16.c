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

#include "common.h"

#ifndef UNIT
#define INV(a) (ONE / (a))
#else
#define INV(a) (ONE)
#endif

int CNAME(BLASLONG m, BLASLONG n, FLOAT *a, BLASLONG lda, BLASLONG offset, FLOAT *b){

  BLASLONG i, ii, j, jj, k;

  FLOAT *a1, *a2,  *a3,  *a4,  *a5,  *a6,  *a7,  *a8;
  FLOAT *a9, *a10, *a11, *a12, *a13, *a14, *a15, *a16;

  
  jj = offset;

  j = (n >> 4);
  while (j > 0){

    a1  = a +  0 * lda;
    a2  = a +  1 * lda;
    a3  = a +  2 * lda;
    a4  = a +  3 * lda;
    a5  = a +  4 * lda;
    a6  = a +  5 * lda;
    a7  = a +  6 * lda;
    a8  = a +  7 * lda;
    a9  = a +  8 * lda;
    a10 = a +  9 * lda;
    a11 = a + 10 * lda;
    a12 = a + 11 * lda;
    a13 = a + 12 * lda;
    a14 = a + 13 * lda;
    a15 = a + 14 * lda;
    a16 = a + 15 * lda;

    a += 16 * lda;
    ii = 0;

    for (i = 0; i < m; i++) {
      
      if ((ii >= jj ) && (ii - jj < 16)) {
	for (k = 0; k < ii - jj; k ++) {
	  *(b +  k) = *(a1 +  k * lda);
	}
	*(b +  ii - jj) = INV(*(a1 + (ii - jj) * lda));
      }
      
      if (ii - jj >= 16) {
	*(b +  0) = *(a1  +  0);
	*(b +  1) = *(a2  +  0);
	*(b +  2) = *(a3  +  0);
	*(b +  3) = *(a4  +  0);
	*(b +  4) = *(a5  +  0);
	*(b +  5) = *(a6  +  0);
	*(b +  6) = *(a7  +  0);
	*(b +  7) = *(a8  +  0);
	*(b +  8) = *(a9  +  0);
	*(b +  9) = *(a10 +  0);
	*(b + 10) = *(a11 +  0);
	*(b + 11) = *(a12 +  0);
	*(b + 12) = *(a13 +  0);
	*(b + 13) = *(a14 +  0);
	*(b + 14) = *(a15 +  0);
	*(b + 15) = *(a16 +  0);
      }

      a1  ++;
      a2  ++;
      a3  ++;
      a4  ++;
      a5  ++;
      a6  ++;
      a7  ++;
      a8  ++;
      a9  ++;
      a10 ++;
      a11 ++;
      a12 ++;
      a13 ++;
      a14 ++;
      a15 ++;
      a16 ++;
      b  += 16;
      ii ++;
    }

    jj += 16;
    j --;
  }

  if (n & 8) {

    a1  = a +  0 * lda;
    a2  = a +  1 * lda;
    a3  = a +  2 * lda;
    a4  = a +  3 * lda;
    a5  = a +  4 * lda;
    a6  = a +  5 * lda;
    a7  = a +  6 * lda;
    a8  = a +  7 * lda;
    a += 8 * lda;
    ii = 0;

    for (i = 0; i < m; i++) {
      
      if ((ii >= jj ) && (ii - jj < 8)) {
	for (k = 0; k < ii - jj; k ++) {
	  *(b +  k) = *(a1 +  k * lda);
	}
	*(b +  ii - jj) = INV(*(a1 + (ii - jj) * lda));
      }
      
      if (ii - jj >= 8) {
	*(b +  0) = *(a1  +  0);
	*(b +  1) = *(a2  +  0);
	*(b +  2) = *(a3  +  0);
	*(b +  3) = *(a4  +  0);
	*(b +  4) = *(a5  +  0);
	*(b +  5) = *(a6  +  0);
	*(b +  6) = *(a7  +  0);
	*(b +  7) = *(a8  +  0);
      }

      a1  ++;
      a2  ++;
      a3  ++;
      a4  ++;
      a5  ++;
      a6  ++;
      a7  ++;
      a8  ++;
      b  += 8;
      ii ++;
    }

    jj += 8;
  }

  if (n & 4) {

    a1  = a +  0 * lda;
    a2  = a +  1 * lda;
    a3  = a +  2 * lda;
    a4  = a +  3 * lda;
    a += 4 * lda;
    ii = 0;

    for (i = 0; i < m; i++) {
      
      if ((ii >= jj ) && (ii - jj < 4)) {
	for (k = 0; k < ii - jj; k ++) {
	  *(b +  k) = *(a1 +  k * lda);
	}
	*(b +  ii - jj) = INV(*(a1 + (ii - jj) * lda));
      }
      
      if (ii - jj >= 4) {
	*(b +  0) = *(a1  +  0);
	*(b +  1) = *(a2  +  0);
	*(b +  2) = *(a3  +  0);
	*(b +  3) = *(a4  +  0);
      }

      a1  ++;
      a2  ++;
      a3  ++;
      a4  ++;
      b  += 4;
      ii ++;
    }

    jj += 4;
  }

  if (n & 2) {

    a1  = a +  0 * lda;
    a2  = a +  1 * lda;
    a += 2 * lda;
    ii = 0;

    for (i = 0; i < m; i++) {
      
      if ((ii >= jj ) && (ii - jj < 2)) {
	for (k = 0; k < ii - jj; k ++) {
	  *(b +  k) = *(a1 +  k * lda);
	}
	*(b +  ii - jj) = INV(*(a1 + (ii - jj) * lda));
      }
      
      if (ii - jj >= 2) {
	*(b +  0) = *(a1  +  0);
	*(b +  1) = *(a2  +  0);
      }

      a1  ++;
      a2  ++;
      b  += 2;
      ii ++;
    }

    jj += 2;
  }

  if (n & 1) {

    a1  = a +  0 * lda;
    ii = 0;

    for (i = 0; i < m; i++) {
      
      if ((ii >= jj ) && (ii - jj < 1)) {
	for (k = 0; k < ii - jj; k ++) {
	  *(b +  k) = *(a1 +  k * lda);
	}
	*(b +  ii - jj) = INV(*(a1 + (ii - jj) * lda));
      }
      
      if (ii - jj >= 1) {
	*(b +  0) = *(a1  +  0);
      }

      a1  ++;
      b  += 1;
      ii ++;
    }
  }

  return 0;
}
