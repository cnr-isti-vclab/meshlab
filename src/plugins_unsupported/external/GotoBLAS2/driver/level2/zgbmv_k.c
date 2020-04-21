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

#ifndef XCONJ
#ifndef CONJ
#define ZAXPY	AXPYU_K
#define ZDOT	DOTU_K
#else
#define ZAXPY	AXPYC_K
#define ZDOT	DOTC_K
#endif
#else
#ifndef CONJ
#define ZAXPY	AXPYU_K
#define ZDOT	DOTC_K
#else
#define ZAXPY	AXPYC_K
#define ZDOT	DOTU_K
#endif
#endif

#ifndef TRANS
#define M m
#define N n
#else
#define N m
#define M n
#endif

void CNAME(BLASLONG m, BLASLONG n, BLASLONG ku, BLASLONG kl, FLOAT alpha_r, FLOAT alpha_i,
	  FLOAT *a, BLASLONG lda,
	  FLOAT *x, BLASLONG incx, FLOAT *y, BLASLONG incy, void *buffer){

  BLASLONG i, offset_u, offset_l, start, end, length;
  FLOAT *X = x;
  FLOAT *Y = y;
  FLOAT *gemvbuffer = (FLOAT *)buffer;
  FLOAT *bufferY    = gemvbuffer;
  FLOAT *bufferX    = gemvbuffer;
#ifdef TRANS
  FLOAT _Complex temp;
#endif

  if (incy != 1) {
    Y = bufferY;
    bufferX    = (FLOAT *)(((BLASLONG)bufferY + M * sizeof(FLOAT) * 2 + 4095) & ~4095);
    gemvbuffer = bufferX;
    COPY_K(M, y, incy, Y, 1);
  }

  if (incx != 1) {
    X = bufferX;
    gemvbuffer = (FLOAT *)(((BLASLONG)bufferX + N * sizeof(FLOAT) * 2 + 4095) & ~4095);
    COPY_K(N, x, incx, X, 1);
  }

  offset_u = ku;
  offset_l = ku + m;

  for (i = 0; i < MIN(n, m + ku); i++) {

    start = MAX(offset_u, 0);
    end   = MIN(offset_l, ku + kl + 1);

    length  = end - start;

#ifndef TRANS
    ZAXPY(length, 0, 0,
#ifndef XCONJ
	  alpha_r * X[i * 2 + 0] - alpha_i * X[i * 2 + 1],
	  alpha_i * X[i * 2 + 0] + alpha_r * X[i * 2 + 1],
#else
	  alpha_r * X[i * 2 + 0] + alpha_i * X[i * 2 + 1],
	  alpha_i * X[i * 2 + 0] - alpha_r * X[i * 2 + 1],
#endif
	  a + start * 2, 1, Y + (start - offset_u) * 2, 1, NULL, 0);
#else

#ifndef XCONJ
    temp = ZDOT(length, a + start * 2, 1, X + (start - offset_u) * 2, 1);
#else
    temp = ZDOT(length, X + (start - offset_u) * 2, 1, a + start * 2, 1);
#endif

#if !defined(XCONJ) || !defined(CONJ)
    Y[i * 2 + 0] += alpha_r * CREAL(temp) - alpha_i * CIMAG(temp);
    Y[i * 2 + 1] += alpha_i * CREAL(temp) + alpha_r * CIMAG(temp);
#else
    Y[i * 2 + 0] += alpha_r * CREAL(temp) + alpha_i * CIMAG(temp);
    Y[i * 2 + 1] += alpha_i * CREAL(temp) - alpha_r * CIMAG(temp);
#endif
#endif
   
    offset_u --;
    offset_l --;

    a += lda * 2;
  }

  if (incy != 1) {
    COPY_K(M, Y, 1, y, incy);
  }

  return;
}

