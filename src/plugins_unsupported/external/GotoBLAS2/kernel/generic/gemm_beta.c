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

int CNAME(BLASLONG m, BLASLONG n, BLASLONG dummy1, FLOAT beta, 
	  FLOAT *dummy2, BLASLONG dummy3, FLOAT *dummy4, BLASLONG dummy5,
	  FLOAT *c, BLASLONG ldc){

  BLASLONG i, j;
  FLOAT *c_offset1, *c_offset;
  FLOAT ctemp1, ctemp2, ctemp3, ctemp4;
  FLOAT ctemp5, ctemp6, ctemp7, ctemp8;

  c_offset = c;

  if (beta == ZERO){

    j = n;
    do {
      c_offset1 = c_offset;
      c_offset += ldc;

      i = (m >> 3);
      if (i > 0){
	do {
	  *(c_offset1 + 0) = ZERO;
	  *(c_offset1 + 1) = ZERO;
	  *(c_offset1 + 2) = ZERO;
	  *(c_offset1 + 3) = ZERO;
	  *(c_offset1 + 4) = ZERO;
	  *(c_offset1 + 5) = ZERO;
	  *(c_offset1 + 6) = ZERO;
	  *(c_offset1 + 7) = ZERO;
	  c_offset1 += 8;
	  i --;
	} while (i > 0);
      }

      i = (m & 7);
      if (i > 0){
	do {
	  *c_offset1 = ZERO;
	  c_offset1 ++;
	  i --;
	} while (i > 0);
      }
      j --;
    } while (j > 0);

  } else {

    j = n;
    do {
      c_offset1 = c_offset;
      c_offset += ldc;
      
      i = (m >> 3);
      if (i > 0){
	do {
	  ctemp1 = *(c_offset1 + 0);
	  ctemp2 = *(c_offset1 + 1);
	  ctemp3 = *(c_offset1 + 2);
	  ctemp4 = *(c_offset1 + 3);
	  ctemp5 = *(c_offset1 + 4);
	  ctemp6 = *(c_offset1 + 5);
	  ctemp7 = *(c_offset1 + 6);
	  ctemp8 = *(c_offset1 + 7);

	  ctemp1 *= beta;
	  ctemp2 *= beta;
	  ctemp3 *= beta;
	  ctemp4 *= beta;
	  ctemp5 *= beta;
	  ctemp6 *= beta;
	  ctemp7 *= beta;
	  ctemp8 *= beta;

	  *(c_offset1 + 0) = ctemp1;
	  *(c_offset1 + 1) = ctemp2;
	  *(c_offset1 + 2) = ctemp3;
	  *(c_offset1 + 3) = ctemp4;
	  *(c_offset1 + 4) = ctemp5;
	  *(c_offset1 + 5) = ctemp6;
	  *(c_offset1 + 6) = ctemp7;
	  *(c_offset1 + 7) = ctemp8;
	  c_offset1 += 8;
	  i --;
	} while (i > 0);
      }

      i = (m & 7);
      if (i > 0){
	do {
	  ctemp1 = *c_offset1;
	  ctemp1 *= beta;
	  *c_offset1 = ctemp1;
	  c_offset1 ++;
	  i --;
	} while (i > 0);
      }
      j --;
    } while (j > 0);

  }
  return 0;
};
