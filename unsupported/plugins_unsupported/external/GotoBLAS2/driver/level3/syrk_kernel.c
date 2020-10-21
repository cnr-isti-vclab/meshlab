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

#ifndef CONJA
#ifndef CONJB
#define GEMM_KERNEL	GEMM_KERNEL_N
#else
#define GEMM_KERNEL	GEMM_KERNEL_R
#endif
#else
#ifndef CONJB
#define GEMM_KERNEL	GEMM_KERNEL_L
#else
#define GEMM_KERNEL	GEMM_KERNEL_B
#endif
#endif

int CNAME(BLASLONG m, BLASLONG n, BLASLONG k, FLOAT alpha_r,
#ifdef COMPLEX
	   FLOAT alpha_i,
#endif
	   FLOAT *a, FLOAT *b, FLOAT *c, BLASLONG ldc, BLASLONG offset){

  BLASLONG i, j;
  BLASLONG loop;
  FLOAT *cc, *ss;
  FLOAT subbuffer[GEMM_UNROLL_MN * (GEMM_UNROLL_MN + 1) * COMPSIZE];

  if (m + offset < 0) {
#ifndef LOWER
    GEMM_KERNEL(m, n, k,
		alpha_r,
#ifdef COMPLEX
		alpha_i,
#endif
		a, b, c, ldc); 
#endif
    return 0;
  }

  if (n < offset) {
#ifdef LOWER
    GEMM_KERNEL(m, n, k,
		alpha_r,
#ifdef COMPLEX
		alpha_i,
#endif
		a, b, c, ldc); 
#endif
    return 0;
  }

  if (offset > 0) {
#ifdef LOWER
    GEMM_KERNEL(m, offset, k,
		alpha_r,
#ifdef COMPLEX
		alpha_i,
#endif
		a, b, c, ldc); 
#endif
    b += offset * k   * COMPSIZE;
    c += offset * ldc * COMPSIZE;
    n -= offset;
    offset = 0;

    if (n <= 0) return 0;
  }

  if (n > m + offset) {
#ifndef LOWER
      GEMM_KERNEL(m, n - m - offset, k,
		  alpha_r,
#ifdef COMPLEX
		  alpha_i,
#endif
		  a,
		  b + (m + offset) * k   * COMPSIZE,
		  c + (m + offset) * ldc * COMPSIZE, ldc); 
#endif

    n = m + offset;
    if (n <= 0) return 0;
  }

  if (offset < 0) {
#ifndef LOWER
    GEMM_KERNEL(-offset, n, k,
		alpha_r,
#ifdef COMPLEX
		alpha_i,
#endif
		a, b, c, ldc); 
#endif
    a -= offset * k   * COMPSIZE;
    c -= offset       * COMPSIZE;
    m += offset;
    offset = 0;

  if (m <= 0) return 0;
  }

  if (m > n - offset) {
#ifdef LOWER
    GEMM_KERNEL(m - n + offset, n, k,
		alpha_r,
#ifdef COMPLEX
		alpha_i,
#endif
		a + (n - offset) * k * COMPSIZE,
		b,
		c + (n - offset)     * COMPSIZE, ldc); 
#endif
    m = n + offset;

  if (m <= 0) return 0;
  }

  for (loop = 0; loop < n; loop += GEMM_UNROLL_MN) {

    int mm, nn;

    mm = (loop & ~(GEMM_UNROLL_MN - 1));
    nn = MIN(GEMM_UNROLL_MN, n - loop);

#ifndef LOWER
    GEMM_KERNEL(mm, nn, k,
		  alpha_r,
#ifdef COMPLEX
		  alpha_i,
#endif
		  a, b + loop * k * COMPSIZE, c + loop * ldc * COMPSIZE, ldc); 
#endif

    GEMM_BETA(nn, nn, 0, ZERO, 
#ifdef COMPLEX
	      ZERO,
#endif
	      NULL, 0, NULL, 0, subbuffer, nn);
    
    GEMM_KERNEL(nn, nn, k,
		alpha_r,
#ifdef COMPLEX
		alpha_i,
#endif
		a + loop * k * COMPSIZE, b + loop * k * COMPSIZE, subbuffer, nn); 

    cc = c + (loop + loop * ldc) * COMPSIZE;
    ss = subbuffer;

#ifndef LOWER
 for (j = 0; j < nn; j ++) {
    for (i = 0; i <= j; i ++) {
#ifndef COMPLEX
      cc[i] += ss[i];
#else
      cc[i * 2 + 0] += ss[i * 2 + 0];
      cc[i * 2 + 1] += ss[i * 2 + 1];
#endif
    }
    ss += nn  * COMPSIZE;
    cc += ldc * COMPSIZE;
  }
#else
  for (j = 0; j < nn; j ++) {
    for (i = j; i < nn; i ++) {
#ifndef COMPLEX
      cc[i] += ss[i];
#else
      cc[i * 2 + 0] += ss[i * 2 + 0];
      cc[i * 2 + 1] += ss[i * 2 + 1];
#endif
    }
    ss += nn  * COMPSIZE;
    cc += ldc * COMPSIZE;
  }
#endif

#ifdef LOWER
    GEMM_KERNEL(m - mm - nn, nn, k,
		  alpha_r,
#ifdef COMPLEX
		  alpha_i,
#endif
		  a + (mm + nn) * k * COMPSIZE, b + loop * k * COMPSIZE, 
		  c + (mm + nn + loop * ldc) * COMPSIZE, ldc); 
#endif

  }

    return 0;
}
