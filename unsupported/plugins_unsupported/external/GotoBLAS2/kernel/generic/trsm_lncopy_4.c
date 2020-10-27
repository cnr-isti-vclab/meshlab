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

  FLOAT data01, data02, data03, data04, data05, data06, data07, data08;
  FLOAT data09, data10, data11, data12, data13, data14, data15, data16;
  FLOAT *a1, *a2, *a3, *a4;

  jj = offset;

  j = (n >> 2);
  while (j > 0){

    a1 = a + 0 * lda;
    a2 = a + 1 * lda;
    a3 = a + 2 * lda;
    a4 = a + 3 * lda;

    i = (m >> 2);
    ii = 0;
    while (i > 0) {

      if (ii == jj) {
#ifndef UNIT
	data01 = *(a1 + 0);
#endif
	data02 = *(a1 + 1);
	data03 = *(a1 + 2);
	data04 = *(a1 + 3);

#ifndef UNIT
	data06 = *(a2 + 1);
#endif
	data07 = *(a2 + 2);
	data08 = *(a2 + 3);

#ifndef UNIT
	data11 = *(a3 + 2);
#endif
	data12 = *(a3 + 3);

#ifndef UNIT
	data16 = *(a4 + 3);
#endif

	*(b +  0) = INV(data01);

	*(b +  4) = data02;
	*(b +  5) = INV(data06);

	*(b +  8) = data03;
	*(b +  9) = data07;
	*(b + 10) = INV(data11);

	*(b + 12) = data04;
	*(b + 13) = data08;
	*(b + 14) = data12;
	*(b + 15) = INV(data16);
      }

      if (ii > jj) {

	data01 = *(a1 + 0);
	data02 = *(a1 + 1);
	data03 = *(a1 + 2);
	data04 = *(a1 + 3);

	data05 = *(a2 + 0);
	data06 = *(a2 + 1);
	data07 = *(a2 + 2);
	data08 = *(a2 + 3);

	data09 = *(a3 + 0);
	data10 = *(a3 + 1);
	data11 = *(a3 + 2);
	data12 = *(a3 + 3);

	data13 = *(a4 + 0);
	data14 = *(a4 + 1);
	data15 = *(a4 + 2);
	data16 = *(a4 + 3);

	*(b +  0) = data01;
	*(b +  1) = data05;
	*(b +  2) = data09;
	*(b +  3) = data13;
	*(b +  4) = data02;
	*(b +  5) = data06;
	*(b +  6) = data10;
	*(b +  7) = data14;

	*(b +  8) = data03;
	*(b +  9) = data07;
	*(b + 10) = data11;
	*(b + 11) = data15;
	*(b + 12) = data04;
	*(b + 13) = data08;
	*(b + 14) = data12;
	*(b + 15) = data16;
      }
      
      a1 += 4;
      a2 += 4;
      a3 += 4;
      a4 += 4;
      b += 16;

      i  --;
      ii += 4;
    }

    if ((m & 2) != 0) {

      if (ii== jj) {
#ifndef UNIT
	data01 = *(a1 + 0);
#endif
	data02 = *(a1 + 1);

#ifndef UNIT
	data06 = *(a2 + 1);
#endif

	*(b +  0) = INV(data01);

	*(b +  4) = data02;
	*(b +  5) = INV(data06);
      }

      if (ii > jj) {
	data01 = *(a1 + 0);
	data02 = *(a1 + 1);
	data03 = *(a2 + 0);
	data04 = *(a2 + 1);
	data05 = *(a3 + 0);
	data06 = *(a3 + 1);
	data07 = *(a4 + 0);
	data08 = *(a4 + 1);

	*(b +  0) = data01;
	*(b +  1) = data03;
	*(b +  2) = data05;
	*(b +  3) = data07;
	*(b +  4) = data02;
	*(b +  5) = data04;
	*(b +  6) = data06;
	*(b +  7) = data08;
      }
      
      a1 += 2;
      a2 += 2;
      a3 += 2;
      a4 += 2;
      b += 8;

      ii += 2;
    }

    if ((m & 1) != 0) {

      if (ii== jj) {
#ifndef UNIT
	data01 = *(a1 + 0);
#endif
	*(b +  0) = INV(data01);
      }

      if (ii > jj)  {
	data01 = *(a1 + 0);
	data02 = *(a2 + 0);
	data03 = *(a3 + 0);
	data04 = *(a4 + 0);

	*(b +  0) = data01;
	*(b +  1) = data02;
	*(b +  2) = data03;
	*(b +  3) = data04;
      }
      b += 4;
    }

    a += 4 * lda;
    jj += 4;
    j  --;
  }

  if (n & 2) {
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
	*(b +  2) = data02;
	*(b +  3) = INV(data04);
      }

      if (ii > jj) {
	data01 = *(a1 + 0);
	data02 = *(a1 + 1);
	data03 = *(a2 + 0);
	data04 = *(a2 + 1);

	*(b +  0) = data01;
	*(b +  1) = data03;
	*(b +  2) = data02;
	*(b +  3) = data04;
      }
      
      a1 += 2;
      a2 += 2;
      b += 4;

      i  --;
      ii += 2;
    }

    if ((m & 1) != 0) {

      if (ii== jj) {
#ifndef UNIT
	data01 = *(a1 + 0);
#endif
	*(b +  0) = INV(data01);
      }

      if (ii > jj)  {
	data01 = *(a1 + 0);
	data02 = *(a2 + 0);
	*(b +  0) = data01;
	*(b +  1) = data02;
      }
      b += 2;
    }
    a += 2 * lda;
    jj += 2;
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

      if (ii > jj) {
	data01 = *(a1 + 0);
	*(b +  0) = data01;
      }
      
      a1+= 1;
      b += 1;
      i  --;
      ii += 1;
    }
  }

  return 0;
}
