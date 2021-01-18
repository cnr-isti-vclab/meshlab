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

int CNAME(BLASLONG m, BLASLONG n, FLOAT *a, BLASLONG lda, FLOAT *b){
  BLASLONG i, j;

  FLOAT *a_offset, *a_offset1, *a_offset2;
  FLOAT *b_offset, *b_offset1, *b_offset2;

  a_offset = a;
  b_offset = b;
  b_offset2 = b + m * (n & ~1);

  i = (m >> 1);

  if (i > 0) {
    do {
      a_offset1 = a_offset;
      a_offset2 = a_offset + lda;
      a_offset += 2 * lda;

      b_offset1 = b_offset;
      b_offset += 4;
      
      j = (n >> 1);
      if (j > 0){
	do {
	  *(b_offset1 + 0) = *(a_offset1 + 0);
	  *(b_offset1 + 1) = *(a_offset1 + 1);
	  *(b_offset1 + 2) = *(a_offset2 + 0);
	  *(b_offset1 + 3) = *(a_offset2 + 1);
	  a_offset1 += 2;
	  a_offset2 += 2;
	  b_offset1 += m * 2;
	  j--;
	} while (j > 0);
      }

      if (n & 1){
	  *(b_offset2 + 0) = *(a_offset1 + 0);
	  *(b_offset2 + 1) = *(a_offset2 + 0);
	  b_offset2 += 2;
      }
      i --;
    } while (i > 0);
  }

  if (m & 1) {
    j = (n >> 1);
    if (j > 0){
      do {
	*(b_offset + 0) = *(a_offset + 0);
	*(b_offset + 1) = *(a_offset + 1);
	a_offset += 2;
	b_offset += m * 2;
	j--;
      } while (j > 0);
    }
    
    if (n & 1){
      *(b_offset2 + 0) = *(a_offset + 0);
      }
  }

  return 0;
}
