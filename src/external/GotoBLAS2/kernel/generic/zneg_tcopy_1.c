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
  FLOAT *a_offset;
  FLOAT *b_offset, *b_offset1;
  FLOAT  ctemp1,  ctemp2,  ctemp3,  ctemp4;
  FLOAT  ctemp5,  ctemp6,  ctemp7,  ctemp8;

  a_offset = a;
  b_offset = b;
  
  lda *= 2;

  j = m;

  m *= 2;
  
  if (j > 0){
    do {
      b_offset1 = b_offset;
      b_offset += 2;
      
      i = (n >> 2);
      if (i > 0){
	do{
	  ctemp1  = *(a_offset +  0);
	  ctemp2  = *(a_offset +  1);
	  ctemp3  = *(a_offset +  2);
	  ctemp4  = *(a_offset +  3);
	  
	  ctemp5  = *(a_offset +  4);
	  ctemp6  = *(a_offset +  5);
	  ctemp7  = *(a_offset +  6);
	  ctemp8  = *(a_offset +  7);
	  
	  *(b_offset1 + 0) = -ctemp1;
	  *(b_offset1 + 1) = -ctemp2;
	  
	  b_offset1 += m;
	  
	  *(b_offset1 + 0) = -ctemp3;
	  *(b_offset1 + 1) = -ctemp4;
	  
	  b_offset1 += m;
	  
	  *(b_offset1 + 0) = -ctemp5;
	  *(b_offset1 + 1) = -ctemp6;

	  b_offset1 += m;
	  
	  *(b_offset1 + 0) = -ctemp7;
	  *(b_offset1 + 1) = -ctemp8;
	  
	  b_offset1 += m;
	  a_offset += 8;
	  i --;
	} while(i>0);
      }
      
      i = (n & 3);
      if (i > 0){
	do {
	  ctemp1  = *(a_offset +  0);
	  ctemp2  = *(a_offset +  1);
	  
	  *(b_offset1 + 0) = -ctemp1;
	  *(b_offset1 + 1) = -ctemp2;
	  
	  b_offset1 += m;
	  a_offset += 2;
	  i --;
	} while(i > 0);
      }
      a_offset += lda - n * 2;
      j --;
    } while (j > 0);
  }

  return 0;
}
