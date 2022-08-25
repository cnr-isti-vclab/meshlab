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
#include <stdlib.h>
#include <ctype.h>
#include "common.h"

int CNAME(BLASLONG m, BLASLONG n, BLASLONG dummy1, 
	  FLOAT beta_r, FLOAT beta_i,
	  FLOAT *dummy2, BLASLONG dummy3,
	  FLOAT *dummy4, BLASLONG dummy5,
	  FLOAT *c, BLASLONG ldc){
  BLASLONG i, j;

  FLOAT *c_offset, *c_offset1;
  FLOAT atemp1, atemp2, atemp3, atemp4;
  FLOAT btemp1, btemp2, btemp3, btemp4;
  FLOAT ctemp1, ctemp2, ctemp3, ctemp4;

  ldc *= 2;

  c_offset = c;

  if (beta_r == 0. && beta_i == 0.) {
    j = n;
    do {
      c_offset1 = c_offset;
      c_offset += ldc;

      i = (m >> 2);
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
	  i--;
	} while (i > 0);
      }

      i = (m & 3);
      if (i > 0){
	do {
	  *(c_offset1 + 0) = ZERO;
	  *(c_offset1 + 1) = ZERO;
	  c_offset1 += 2;
	  i--;
	} while (i > 0);
      }
      j --;
    } while (j > 0);

  } else {

    j = n;
    do {
      c_offset1 = c_offset;
      c_offset += ldc;
      
      i = (m >> 1);
      if (i > 0){
	do {
	  atemp1 = *(c_offset1 + 0);
	  atemp2 = *(c_offset1 + 1);
	  atemp3 = *(c_offset1 + 2);
	  atemp4 = *(c_offset1 + 3);
	  
	  btemp1 = beta_r * atemp1;
	  btemp2 = beta_i * atemp2;
	  btemp3 = beta_r * atemp2;
	  btemp4 = beta_i * atemp1;
	  
	  ctemp1 = btemp1 - btemp2;
	  ctemp2 = btemp3 + btemp4;

	  btemp1 = beta_r * atemp3;
	  btemp2 = beta_i * atemp4;
	  btemp3 = beta_r * atemp4;
	  btemp4 = beta_i * atemp3;
	  
	  ctemp3 = btemp1 - btemp2;
	  ctemp4 = btemp3 + btemp4;

	  *(c_offset1 + 0) = ctemp1;
	  *(c_offset1 + 1) = ctemp2;
	  *(c_offset1 + 2) = ctemp3;
	  *(c_offset1 + 3) = ctemp4;
	  c_offset1 += 4;
	  i --;
	} while (i > 0);
      }

      i = (m & 1);
      if (i > 0){
	do {
	  atemp1 = *(c_offset1 + 0);
	  atemp2 = *(c_offset1 + 1);
	  
	  btemp1 = beta_r * atemp1;
	  btemp2 = beta_i * atemp2;
	  btemp3 = beta_r * atemp2;
	  btemp4 = beta_i * atemp1;
	  
	  ctemp1 = btemp1 - btemp2;
	  ctemp2 = btemp3 + btemp4;
	  
	  *(c_offset1 + 0) = ctemp1;
	  *(c_offset1 + 1) = ctemp2;
	  c_offset1 += 2;
	  i --;
	} while (i > 0);
      }
      j --;
    } while (j > 0);
  }
  return 0;
}
