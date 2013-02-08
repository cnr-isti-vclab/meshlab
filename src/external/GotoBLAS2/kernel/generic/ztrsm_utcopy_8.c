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

int CNAME(BLASLONG m, BLASLONG n, FLOAT *a, BLASLONG lda, BLASLONG offset, FLOAT *b){

  BLASLONG i, ii, j, jj, k;

  FLOAT *a1, data1, data2;

  lda *= 2;

  jj = offset;

  j = (n >> 3);
  while (j > 0){

    a1 = a;
    a += 16;
    ii = 0;

    for (i = 0; i < m; i++) {
      
      if ((ii >= jj ) && (ii - jj < 8)) {
	for (k = 0; k < ii - jj; k ++) {
	  *(b +  k * 2 + 0) = *(a1 +  k * 2 + 0);
	  *(b +  k * 2 + 1) = *(a1 +  k * 2 + 1);
	}

	data1 = *(a1 + (ii - jj) * 2 + 0);
	data2 = *(a1 + (ii - jj) * 2 + 1);

	compinv(b +  (ii - jj) * 2, data1, data2);
      }
      
      if (ii - jj >= 8) {
	*(b +  0) = *(a1 +  0);
	*(b +  1) = *(a1 +  1);
	*(b +  2) = *(a1 +  2);
	*(b +  3) = *(a1 +  3);
	*(b +  4) = *(a1 +  4);
	*(b +  5) = *(a1 +  5);
	*(b +  6) = *(a1 +  6);
	*(b +  7) = *(a1 +  7);
	*(b +  8) = *(a1 +  8);
	*(b +  9) = *(a1 +  9);
	*(b + 10) = *(a1 + 10);
	*(b + 11) = *(a1 + 11);
	*(b + 12) = *(a1 + 12);
	*(b + 13) = *(a1 + 13);
	*(b + 14) = *(a1 + 14);
	*(b + 15) = *(a1 + 15);
      }

      b  += 16;
      a1 += lda;
      ii ++;
    }

    jj += 8;
    j --;
  }

  j = (n & 4);
  if (j > 0) {

    a1 = a;
    a += 8;
    ii = 0;

    for (i = 0; i < m; i++) {
      
      if ((ii >= jj ) && (ii - jj < 4)) {
	for (k = 0; k < ii - jj; k ++) {
	  *(b +  k * 2 + 0) = *(a1 +  k * 2 + 0);
	  *(b +  k * 2 + 1) = *(a1 +  k * 2 + 1);
	}

	data1 = *(a1 + (ii - jj) * 2 + 0);
	data2 = *(a1 + (ii - jj) * 2 + 1);

	compinv(b +  (ii - jj) * 2, data1, data2);
      }
      
      if (ii - jj >= 4) {
	*(b +  0) = *(a1 +  0);
	*(b +  1) = *(a1 +  1);
	*(b +  2) = *(a1 +  2);
	*(b +  3) = *(a1 +  3);
	*(b +  4) = *(a1 +  4);
	*(b +  5) = *(a1 +  5);
	*(b +  6) = *(a1 +  6);
	*(b +  7) = *(a1 +  7);
      }

      b  += 8;
      a1 += lda;
      ii ++;
    }

    jj += 4;
  }

  j = (n & 2);
  if (j > 0) {

    a1 = a;
    a += 4;
    ii = 0;

    for (i = 0; i < m; i++) {
      
      if ((ii >= jj ) && (ii - jj < 2)) {
	for (k = 0; k < ii - jj; k ++) {
	  *(b +  k * 2 + 0) = *(a1 +  k * 2 + 0);
	  *(b +  k * 2 + 1) = *(a1 +  k * 2 + 1);
	}

	data1 = *(a1 + (ii - jj) * 2 + 0);
	data2 = *(a1 + (ii - jj) * 2 + 1);

	compinv(b +  (ii - jj) * 2, data1, data2);
      }
      
      if (ii - jj >= 2) {
	*(b +  0) = *(a1 +  0);
	*(b +  1) = *(a1 +  1);
	*(b +  2) = *(a1 +  2);
	*(b +  3) = *(a1 +  3);
      }

      b  += 4;
      a1 += lda;
      ii ++;
    }

    jj += 2;
  }

  j = (n & 1);
  if (j > 0) {

    a1 = a;
    ii = 0;

    for (i = 0; i < m; i++) {
      
      if ((ii >= jj ) && (ii - jj < 1)) {
	for (k = 0; k < ii - jj; k ++) {
	  *(b +  k * 2 + 0) = *(a1 +  k * 2 + 0);
	  *(b +  k * 2 + 1) = *(a1 +  k * 2 + 1);
	}

	data1 = *(a1 + (ii - jj) * 2 + 0);
	data2 = *(a1 + (ii - jj) * 2 + 1);

	compinv(b +  (ii - jj) * 2, data1, data2);
      }
      
      if (ii - jj >= 1) {
	*(b +  0) = *(a1 +  0);
	*(b +  1) = *(a1 +  1);
      }

      b  += 2;
      a1 += lda;
      ii ++;
    }
  }

  return 0;
}
