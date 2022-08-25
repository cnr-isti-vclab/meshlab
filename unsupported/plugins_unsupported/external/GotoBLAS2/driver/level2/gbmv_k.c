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

#ifndef TRANS
#define M m
#define N n
#else
#define N m
#define M n
#endif

void CNAME(BLASLONG m, BLASLONG n, BLASLONG ku, BLASLONG kl, FLOAT alpha,
	  FLOAT *a, BLASLONG lda,
	  FLOAT *x, BLASLONG incx, FLOAT *y, BLASLONG incy, void *buffer){

  BLASLONG i, offset_u, offset_l, start, end, length;
  FLOAT *X = x;
  FLOAT *Y = y;
  FLOAT *gemvbuffer = (FLOAT *)buffer;
  FLOAT *bufferY    = gemvbuffer;
  FLOAT *bufferX    = gemvbuffer;

  if (incy != 1) {
    Y = bufferY;
    bufferX    = (FLOAT *)(((BLASLONG)bufferY + M * sizeof(FLOAT) + 4095) & ~4095);
    gemvbuffer = bufferX;
    COPY_K(M, y, incy, Y, 1);
  }

  if (incx != 1) {
    X = bufferX;
    gemvbuffer = (FLOAT *)(((BLASLONG)bufferX + N * sizeof(FLOAT) + 4095) & ~4095);
    COPY_K(N, x, incx, X, 1);
  }

  offset_u = ku;
  offset_l = ku + m;

  for (i = 0; i < MIN(n, m + ku); i++) {

    start = MAX(offset_u, 0);
    end   = MIN(offset_l, ku + kl + 1);

    length  = end - start;

#ifndef TRANS
    AXPYU_K(length, 0, 0,
	   alpha * X[i], 
	   a + start, 1, Y + start - offset_u, 1, NULL, 0);
#else
    Y[i] += alpha * DOTU_K(length, a + start, 1, X + start - offset_u, 1);
#endif
   
    offset_u --;
    offset_l --;

    a += lda;
  }

  if (incy != 1) {
    COPY_K(M, Y, 1, y, incy);
  }

  return;
}

