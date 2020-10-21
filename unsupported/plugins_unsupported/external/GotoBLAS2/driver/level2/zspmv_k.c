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

int CNAME(BLASLONG m, FLOAT alpha_r, FLOAT alpha_i,
	  FLOAT *a, FLOAT *x, BLASLONG incx, FLOAT *y, BLASLONG incy, void *buffer){

  BLASLONG i;
  FLOAT *X = x;
  FLOAT *Y = y;
  FLOAT *gemvbuffer = (FLOAT *)buffer;
  FLOAT *bufferY    = gemvbuffer;
  FLOAT *bufferX    = gemvbuffer;
  FLOAT _Complex result;

  if (incy != 1) {
    Y = bufferY;
    bufferX    = (FLOAT *)(((BLASLONG)bufferY + m * sizeof(FLOAT) * 2 + 4095) & ~4095);
    gemvbuffer = bufferX;
    COPY_K(m, y, incy, Y, 1);
  }

  if (incx != 1) {
    X = bufferX;
    gemvbuffer = (FLOAT *)(((BLASLONG)bufferX + m * sizeof(FLOAT) * 2 + 4095) & ~4095);
    COPY_K(m, x, incx, X, 1);
  }

  for (i = 0; i < m; i++) {
#ifndef LOWER

    if (i > 0) {
      result = DOTU_K(i, a, 1, X, 1);
      
      Y[i * 2 + 0] += alpha_r * CREAL(result) - alpha_i * CIMAG(result);
      Y[i * 2 + 1] += alpha_r * CIMAG(result) + alpha_i * CREAL(result);
    }

    AXPYU_K(i + 1, 0, 0,
	   alpha_r * X[i * 2 + 0] - alpha_i * X[i * 2 + 1], 
	   alpha_r * X[i * 2 + 1] + alpha_i * X[i * 2 + 0], 
	   a, 1, Y, 1, NULL, 0);

    a += (i + 1) * 2;
  
#else

    result = DOTU_K(m - i, a + i * 2, 1, X + i * 2, 1);
    
    Y[i * 2 + 0] += alpha_r * CREAL(result) - alpha_i * CIMAG(result);
    Y[i * 2 + 1] += alpha_r * CIMAG(result) + alpha_i * CREAL(result);
      
    if (m - i > 1)
      AXPYU_K(m - i - 1, 0, 0,
	      alpha_r * X[i * 2 + 0] - alpha_i * X[i * 2 + 1], 
	      alpha_r * X[i * 2 + 1] + alpha_i * X[i * 2 + 0], 
	      a + (i + 1) * 2, 1, Y + (i + 1) * 2, 1, NULL, 0);

    a += (m - i - 1) * 2;

#endif
  }

  if (incy != 1) {
    COPY_K(m, Y, 1, y, incy);
  }

  return 0;
}

