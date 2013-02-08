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

#ifndef USE_ALPHA
#define REAL_PART(a, b)  (a)
#define IMAGE_PART(a, b) (b)
#else
#define REAL_PART(a, b)  (alpha_r * (a) - alpha_i * (b))
#define IMAGE_PART(a, b) (alpha_i * (a) + alpha_r * (b))
#endif

#if defined(REAL_ONLY)
#define CMULT(a, b) (REAL_PART(a, b))
#elif defined(IMAGE_ONLY)
#define CMULT(a, b) (IMAGE_PART(a, b))
#else
#define CMULT(a, b) (REAL_PART(a, b) + IMAGE_PART(a, b))
#endif

int CNAME(BLASLONG m, BLASLONG n, FLOAT *a, BLASLONG lda,
#ifdef USE_ALPHA
	   FLOAT alpha_r, FLOAT alpha_i, 
#endif
	   FLOAT *b){

  BLASLONG i, j;

  FLOAT *a_offset, *a_offset1, *a_offset2;
  FLOAT *b_offset;
  FLOAT a1, a2, a3, a4;

  lda *= 2;

  a_offset = a;
  b_offset = b;
  
  j = (n >> 1);
  if (j > 0){
    do{
      a_offset1  = a_offset;
      a_offset2  = a_offset1 + lda;
      a_offset += 2 * lda;
      
      for (i = 0; i < m; i ++) {

	a1 = *(a_offset1 + 0);
	a2 = *(a_offset1 + 1);
	a3 = *(a_offset2 + 0);
	a4 = *(a_offset2 + 1);

	*(b_offset +  0) = CMULT(a1, a2);
	*(b_offset +  1) = CMULT(a3, a4);

	a_offset1 += 2;
	a_offset2 += 2;
	  
	b_offset +=  2;

      }

      j--;
    }while(j > 0);
  } /* end of if(j > 0) */
  
  if (n & 1) {
    a_offset1  = a_offset;
      
    for (i = 0; i < m; i ++) {
      
      a1 = *(a_offset1 + 0);
      a2 = *(a_offset1 + 1);
      
      *(b_offset +  0) = CMULT(a1, a2);
      
      a_offset1 += 2;
      
      b_offset +=  1;
    }
  }

  return 0;
}
