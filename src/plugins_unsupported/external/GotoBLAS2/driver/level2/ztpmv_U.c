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

int CNAME(BLASLONG m, FLOAT *a, FLOAT *b, BLASLONG incb, void *buffer){
  
  BLASLONG i;
#if (TRANSA == 2) || (TRANSA == 4)
  FLOAT _Complex temp;
#endif
#ifndef UNIT
  FLOAT atemp1, atemp2, btemp1, btemp2;
#endif
  FLOAT *gemvbuffer = (FLOAT *)buffer;
  FLOAT *B = b;

  if (incb != 1) {
    B = buffer;
    gemvbuffer = (FLOAT *)(((BLASLONG)buffer + m * sizeof(FLOAT) * 2 + 4095) & ~4095);
    COPY_K(m, b, incb, buffer, 1);
  }

    for (i = 0; i < m; i++) {

#if (TRANSA == 1) || (TRANSA == 3)
#if   TRANSA == 1
      if (i > 0) AXPYU_K (i, 0, 0, B[i * 2 + 0],  B[i * 2 + 1],
      			  a, 1, B, 1, NULL, 0);
#else
      if (i > 0) AXPYC_K(i, 0, 0, B[i * 2 + 0],  B[i * 2 + 1],
			  a, 1, B, 1, NULL, 0);
#endif
#endif

#ifndef UNIT
#if (TRANSA == 1) || (TRANSA == 3)
      atemp1 = a[i * 2 + 0];
      atemp2 = a[i * 2 + 1];
#else
      atemp1 = a[0];
      atemp2 = a[1];
#endif

      btemp1 = B[i * 2 + 0];
      btemp2 = B[i * 2 + 1];

#if (TRANSA == 1) || (TRANSA == 2)
      B[i * 2 + 0] = atemp1 * btemp1 - atemp2 * btemp2;
      B[i * 2 + 1] = atemp1 * btemp2 + atemp2 * btemp1;
#else
      B[i * 2 + 0] = atemp1 * btemp1 + atemp2 * btemp2;
      B[i * 2 + 1] = atemp1 * btemp2 - atemp2 * btemp1;
#endif
#endif

#if (TRANSA == 2) || (TRANSA == 4)
      if (i < m - 1) {
#if TRANSA == 2
	temp = DOTU_K(m - i - 1,
			  a + 2, 1,
			  B + (i + 1) * 2, 1);
#else
	temp = DOTC_K(m - i - 1,
			  a + 2, 1,
			  B + (i + 1) * 2, 1);
#endif

      B[i * 2 + 0] += CREAL(temp);
      B[i * 2 + 1] += CIMAG(temp);
      }
#endif

#if (TRANSA == 1) || (TRANSA == 3)
    a += (i + 1) * 2;
#else
    a += (m - i) * 2;
#endif
    }
    
  if (incb != 1) {
    COPY_K(m, buffer, 1, b, incb);
  }

  return 0;
}

