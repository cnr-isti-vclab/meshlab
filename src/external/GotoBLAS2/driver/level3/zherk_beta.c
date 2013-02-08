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

int CNAME(BLASLONG dummy1, BLASLONG n,  BLASLONG dummy2, FLOAT alpha_r, FLOAT alpha_i,
	  FLOAT *dummy3, BLASLONG dummy4, FLOAT *dummy5, BLASLONG dummy6,
	  FLOAT *c, BLASLONG ldc,
	  FLOAT *dummy7, FLOAT *dummy8, BLASLONG from, BLASLONG to){

  BLASLONG i;

#ifndef LOWER
  for (i = from; i < to; i++){
    SCAL_K(i * 2, 0, 0, alpha_r, c + i * ldc * 2, 1, NULL, 0, NULL, 0);
    if (alpha_r == ZERO ){
      c[i * 2 + 0 + i * ldc * 2] = ZERO;
      c[i * 2 + 1 + i * ldc * 2] = ZERO;
    } else {
      c[i * 2 + 0 + i * ldc * 2] *= alpha_r;
      c[i * 2 + 1 + i * ldc * 2] = ZERO;
    }
  }
#else
  for (i = from; i < to; i++){
    if (alpha_r == ZERO) {
      c[i * 2 + 0 + i * ldc * 2] = ZERO;
      c[i * 2 + 1 + i * ldc * 2] = ZERO;
    } else {
      c[i * 2 + 0 + i * ldc * 2] *= alpha_r;
      c[i * 2 + 1 + i * ldc * 2] = ZERO;
    }
    SCAL_K((n - i - 1) * 2, 0, 0, alpha_r, c + 2 + i * (ldc + 1) * 2, 1, NULL, 0, NULL, 0);
  }
#endif

  return 0;
}
