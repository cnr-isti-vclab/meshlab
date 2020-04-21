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

const static FLOAT dp1 = 1.;

int CNAME(BLASLONG n, BLASLONG k, FLOAT *a, BLASLONG lda, FLOAT *b, BLASLONG incb, void *buffer){

  BLASLONG i;
  FLOAT *gemvbuffer = (FLOAT *)buffer;
  FLOAT *B = b;
  BLASLONG length;

  if (incb != 1) {
    B = buffer;
    gemvbuffer = (FLOAT *)(((BLASLONG)buffer + n * sizeof(FLOAT) + 4095) & ~4095);
    COPY_K(n, b, incb, buffer, 1);
  }
  
  a += (n - 1) * lda;

  for (i = n - 1; i >= 0; i--) {

#ifdef TRANSA
    length  = n - i - 1;
    if (length > k) length = k;

    if (length > 0) {
      B[i] -= DOTU_K(length, a + 1, 1, B + i + 1, 1);
    }
#endif

#ifndef UNIT
#ifdef TRANSA
    B[i] /= a[0];
#else
    B[i] /= a[k];
#endif
#endif
    
#ifndef TRANSA
    length  = i;
    if (length > k) length = k;

    if (length > 0) {
      AXPYU_K(length, 0, 0,
	     - B[i], 
	     a + k - length, 1, B + i - length, 1, NULL, 0);
    }
#endif

    a -= lda;
  }
    
  if (incb != 1) {
    COPY_K(n, buffer, 1, b, incb);
  }

  return 0;
}

