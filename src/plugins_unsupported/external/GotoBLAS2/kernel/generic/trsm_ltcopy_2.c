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

#ifndef UNIT
#define INV(a) (ONE / (a))
#else
#define INV(a) (ONE)
#endif

int CNAME(BLASLONG m, BLASLONG n, FLOAT *a, BLASLONG lda, BLASLONG offset, FLOAT *b){

  BLASLONG i, ii, j, jj;

  FLOAT data01, data02, data03, data04;
  FLOAT *a1, *a2;

  jj = offset;

  j = (n >> 1);
  while (j > 0){

    a1 = a + 0 * lda;
    a2 = a + 1 * lda;

    i = (m >> 1);
    ii = 0;
    while (i > 0) {

      if (ii == jj) {

#ifndef UNIT
	data01 = *(a1 + 0);
#endif
	data02 = *(a1 + 1);

#ifndef UNIT
	data04 = *(a2 + 1);
#endif

	*(b +  0) = INV(data01);
	*(b +  1) = data02;

	*(b +  3) = INV(data04);
      }

      if (ii < jj) {
	data01 = *(a1 + 0);
	data02 = *(a1 + 1);
	data03 = *(a2 + 0);
	data04 = *(a2 + 1);

	*(b +  0) = data01;
	*(b +  1) = data02;
	*(b +  2) = data03;
	*(b +  3) = data04;
      }
      
      a1 += 2 * lda;
      a2 += 2 * lda;
      b += 4;

      i  --;
      ii += 2;
    }

    if ((m & 1) != 0) {

      if (ii== jj) {

#ifndef UNIT
	data01 = *(a1 + 0);
#endif
	data02 = *(a1 + 1);
	
	*(b +  0) = INV(data01);
	*(b +  1) = data02;
      }

      if (ii < jj)  {
	data01 = *(a1 + 0);
	data02 = *(a1 + 1);

	*(b +  0) = data01;
	*(b +  1) = data02;
      }
      b += 2;
    }

    a += 2;
    jj += 2;
    j  --;
  }

  if (n & 1) {
    a1 = a + 0 * lda;

    i = m;
    ii = 0;
    while (i > 0) {

      if (ii == jj) {
#ifndef UNIT
	data01 = *(a1 + 0);
#endif
	*(b +  0) = INV(data01);
      }

      if (ii < jj) {
	data01 = *(a1 + 0);
	*(b +  0) = data01;
      }
      
      a1 += 1 * lda;
      b += 1;

      i  --;
      ii += 1;
    }
  }

  return 0;
}
