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

int CNAME(BLASLONG m, FLOAT *a, BLASLONG lda, FLOAT *b, BLASLONG incb, FLOAT *buffer){

  BLASLONG i, is, min_i;
  FLOAT *gemvbuffer = (FLOAT *)buffer;
  FLOAT *B = b;

  if (incb != 1) {
    B = buffer;
    gemvbuffer = (FLOAT *)(((BLASLONG)buffer + m * sizeof(FLOAT) + 4095) & ~4095);
    COPY_K(m, b, incb, buffer, 1);
  }
  
  for (is = m; is > 0; is -= DTB_ENTRIES){

    min_i = MIN(is, DTB_ENTRIES);
    
#ifndef TRANSA
    if (m - is > 0){
      GEMV_N(m - is, min_i, 0, dp1, 
	     a + is + (is - min_i) * lda, lda,
	     B + is - min_i, 1,
	     B + is, 1, gemvbuffer);
    }
#endif

    for (i = 0; i < min_i; i++) {
      FLOAT *AA = a + (is - i - 1) + (is - i - 1) * lda;
      FLOAT *BB = B + (is - i - 1);

#ifndef TRANSA
      if (i > 0) AXPYU_K(i, 0, 0, BB[0], AA + 1, 1, BB + 1, 1, NULL, 0);
#endif

#ifndef UNIT
      BB[0] *= AA[0];
#endif

#ifdef TRANSA
      if (i < min_i - 1) BB[0] += DOTU_K(min_i - i - 1, AA - (min_i - i - 1), 1, BB - (min_i - i - 1), 1);
#endif
    }
    
#ifdef TRANSA
    if (is - min_i > 0){
      GEMV_T(is - min_i, min_i, 0, dp1, 
	     a +  (is - min_i) * lda, lda,
	     B,              1,
	     B + is - min_i, 1, gemvbuffer);
    }
#endif
  }

  if (incb != 1) {
    COPY_K(m, buffer, 1, b, incb);
  }

  return 0;
}

