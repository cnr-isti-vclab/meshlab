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

int CNAME(BLASLONG m, FLOAT alpha, FLOAT *a,
	  FLOAT *x, BLASLONG incx, FLOAT *y, BLASLONG incy, void *buffer){

  BLASLONG i;
  FLOAT *X = x;
  FLOAT *Y = y;
  FLOAT *gemvbuffer = (FLOAT *)buffer;
  FLOAT *bufferY    = gemvbuffer;
  FLOAT *bufferX    = gemvbuffer;

  if (incy != 1) {
    Y = bufferY;
    bufferX    = (FLOAT *)(((BLASLONG)bufferY + m * sizeof(FLOAT) + 4095) & ~4095);
    gemvbuffer = bufferX;
    COPY_K(m, y, incy, Y, 1);
  }

  if (incx != 1) {
    X = bufferX;
    gemvbuffer = (FLOAT *)(((BLASLONG)bufferX + m * sizeof(FLOAT) + 4095) & ~4095);
    COPY_K(m, x, incx, X, 1);
  }

  for (i = 0; i < m; i++) {
#ifndef LOWER
    if (i > 0) Y[i] += alpha * DOTU_K(i, a, 1, X, 1);
    AXPYU_K(i + 1, 0, 0, alpha * X[i], a, 1, Y, 1, NULL, 0);
    a += i + 1;
    
#else
    Y[i] += alpha * DOTU_K(m - i, a + i, 1, X + i, 1);
    if (m - i > 1) AXPYU_K(m - i - 1, 0, 0, alpha * X[i],
			  a + i + 1, 1, Y + i + 1, 1, NULL, 0);
    a += m - i - 1;
#endif
  }

  if (incy != 1) {
    COPY_K(m, Y, 1, y, incy);
  }

  return 0;
}

