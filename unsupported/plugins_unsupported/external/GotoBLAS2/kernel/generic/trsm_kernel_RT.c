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

#include "common.h"

static FLOAT dm1 = -1.;

#ifdef CONJ
#define GEMM_KERNEL   GEMM_KERNEL_R
#else
#define GEMM_KERNEL   GEMM_KERNEL_N
#endif

#if GEMM_DEFAULT_UNROLL_M == 1
#define GEMM_UNROLL_M_SHIFT 0
#endif

#if GEMM_DEFAULT_UNROLL_M == 2
#define GEMM_UNROLL_M_SHIFT 1
#endif

#if GEMM_DEFAULT_UNROLL_M == 4
#define GEMM_UNROLL_M_SHIFT 2
#endif

#if GEMM_DEFAULT_UNROLL_M == 8
#define GEMM_UNROLL_M_SHIFT 3
#endif

#if GEMM_DEFAULT_UNROLL_M == 16
#define GEMM_UNROLL_M_SHIFT 4
#endif

#if GEMM_DEFAULT_UNROLL_N == 1
#define GEMM_UNROLL_N_SHIFT 0
#endif

#if GEMM_DEFAULT_UNROLL_N == 2
#define GEMM_UNROLL_N_SHIFT 1
#endif

#if GEMM_DEFAULT_UNROLL_N == 4
#define GEMM_UNROLL_N_SHIFT 2
#endif

#if GEMM_DEFAULT_UNROLL_N == 8
#define GEMM_UNROLL_N_SHIFT 3
#endif

#if GEMM_DEFAULT_UNROLL_N == 16
#define GEMM_UNROLL_N_SHIFT 4
#endif


#ifndef COMPLEX

static inline void solve(BLASLONG m, BLASLONG n, FLOAT *a, FLOAT *b, FLOAT *c, BLASLONG ldc) {

  FLOAT aa,  bb;

  int i, j, k;

  a += (n - 1) * m;
  b += (n - 1) * n;

  for (i = n - 1; i >= 0; i--) {

    bb = *(b + i);
    
    for (j = 0; j < m; j ++) {
      aa = *(c + j + i * ldc);
      aa *= bb;
      *a   = aa;
      *(c + j + i * ldc) = aa;
      a ++;

      for (k = 0; k < i; k ++){
	*(c + j + k * ldc) -= aa * *(b + k);
      }

    }
    b -= n;
    a -= 2 * m;
  }

}

#else

static inline void solve(BLASLONG m, BLASLONG n, FLOAT *a, FLOAT *b, FLOAT *c, BLASLONG ldc) {

  FLOAT aa1, aa2;
  FLOAT bb1, bb2;
  FLOAT cc1, cc2;

  int i, j, k;

  ldc *= 2;

  a += (n - 1) * m * 2;
  b += (n - 1) * n * 2;

  for (i = n - 1; i >= 0; i--) {

    bb1 = *(b + i * 2 + 0);
    bb2 = *(b + i * 2 + 1);
    
    for (j = 0; j < m; j ++) {

      aa1 = *(c + j * 2 + 0 + i * ldc);
      aa2 = *(c + j * 2 + 1 + i * ldc);

#ifndef CONJ
      cc1 = aa1 * bb1 - aa2 * bb2;
      cc2 = aa1 * bb2 + aa2 * bb1;
#else
      cc1 =  aa1 * bb1  + aa2 * bb2;
      cc2 = - aa1 * bb2 + aa2 * bb1;
#endif

      *(a + 0) = cc1;
      *(a + 1) = cc2;

      *(c + j * 2 + 0 + i * ldc) = cc1;
      *(c + j * 2 + 1 + i * ldc) = cc2;
      a += 2;

      for (k = 0; k < i; k ++){
#ifndef CONJ
	*(c + j * 2 + 0 + k * ldc) -= cc1 * *(b + k * 2 + 0) - cc2 * *(b + k * 2 + 1);
	*(c + j * 2 + 1 + k * ldc) -= cc1 * *(b + k * 2 + 1) + cc2 * *(b + k * 2 + 0);
#else
	*(c + j * 2 + 0 + k * ldc) -=   cc1 * *(b + k * 2 + 0) + cc2 * *(b + k * 2 + 1);
	*(c + j * 2 + 1 + k * ldc) -=  -cc1 * *(b + k * 2 + 1) + cc2 * *(b + k * 2 + 0);
#endif
      }

    }
    b -= n * 2;
    a -= 4 * m;
  }

}

#endif

int CNAME(BLASLONG m, BLASLONG n, BLASLONG k,  FLOAT dummy1,
#ifdef COMPLEX
	   FLOAT dummy2,
#endif
	   FLOAT *a, FLOAT *b, FLOAT *c, BLASLONG ldc, BLASLONG offset){

  BLASLONG i, j;
  FLOAT *aa, *cc;
  BLASLONG  kk;
  
#if 0
  fprintf(stderr, "TRSM RT KERNEL m = %3ld  n = %3ld  k = %3ld offset = %3ld\n",
	  m, n, k, offset);
#endif

  kk = n - offset;
  c += n * ldc * COMPSIZE;
  b += n * k   * COMPSIZE;

  if (n & (GEMM_UNROLL_N - 1)) {

    j = 1;
    while (j < GEMM_UNROLL_N) {
      if (n & j) {
	
	aa  = a;
	b -= j * k  * COMPSIZE;
	c -= j * ldc* COMPSIZE;
	cc  = c;
	
	i = (m >> GEMM_UNROLL_M_SHIFT);
	if (i > 0) {

	  do {
	    if (k - kk > 0) {
	      GEMM_KERNEL(GEMM_UNROLL_M, j, k - kk, dm1, 
#ifdef COMPLEX
			  ZERO,
#endif
			  aa + GEMM_UNROLL_M * kk * COMPSIZE,
			  b  +  j            * kk * COMPSIZE, 
			  cc,
			  ldc); 
	    }

	    solve(GEMM_UNROLL_M, j, 
		  aa + (kk - j) * GEMM_UNROLL_M * COMPSIZE,
		  b  + (kk - j) * j             * COMPSIZE,
		  cc, ldc);
	    
	    aa += GEMM_UNROLL_M * k * COMPSIZE;
	    cc += GEMM_UNROLL_M     * COMPSIZE;
	    i --;
	  } while (i > 0);
	}

	if (m & (GEMM_UNROLL_M - 1)) {
	  i = (GEMM_UNROLL_M >> 1);
	  do {
	    if (m & i) {

	      if (k - kk > 0) {
		GEMM_KERNEL(i, j, k - kk, dm1, 
#ifdef COMPLEX
			    ZERO,
#endif
			    aa + i * kk * COMPSIZE,
			    b  + j * kk * COMPSIZE, 
			    cc, ldc); 
	      }

	      solve(i, j, 
		    aa + (kk - j) * i * COMPSIZE,
		    b  + (kk - j) * j * COMPSIZE,
		    cc, ldc);

	      aa += i * k * COMPSIZE;
	      cc += i     * COMPSIZE;
	  
	    }
	    i >>= 1;
	  } while (i > 0);
	}
	kk -= j;
      }
      j <<= 1;
    }
  }

  j = (n >> GEMM_UNROLL_N_SHIFT);

  if (j > 0) {

    do {
      aa  = a;
      b -= GEMM_UNROLL_N * k   * COMPSIZE;
      c -= GEMM_UNROLL_N * ldc * COMPSIZE;
      cc  = c;

      i = (m >> GEMM_UNROLL_M_SHIFT);
      if (i > 0) {
	do {
	  if (k - kk > 0) {
	    GEMM_KERNEL(GEMM_UNROLL_M, GEMM_UNROLL_N, k - kk, dm1, 
#ifdef COMPLEX
			ZERO,
#endif
			aa + GEMM_UNROLL_M * kk * COMPSIZE,
			b  + GEMM_UNROLL_N * kk * COMPSIZE, 
			cc,
			ldc); 
	  }
	  
	  solve(GEMM_UNROLL_M, GEMM_UNROLL_N, 
		aa + (kk - GEMM_UNROLL_N) * GEMM_UNROLL_M * COMPSIZE, 
		b  + (kk - GEMM_UNROLL_N) * GEMM_UNROLL_N * COMPSIZE,
		cc, ldc);
	  
	  aa += GEMM_UNROLL_M * k * COMPSIZE;
	  cc += GEMM_UNROLL_M     * COMPSIZE;
	  i --;
	} while (i > 0);
      }

      if (m & (GEMM_UNROLL_M - 1)) {
	i = (GEMM_UNROLL_M >> 1);
	do {
	  if (m & i) {
	    if (k - kk > 0) {
	      GEMM_KERNEL(i, GEMM_UNROLL_N, k - kk, dm1, 
#ifdef COMPLEX
			  ZERO,
#endif
			  aa + i             * kk * COMPSIZE,
			  b  + GEMM_UNROLL_N * kk * COMPSIZE, 
			  cc,
			  ldc); 
	    }
	    
	    solve(i, GEMM_UNROLL_N, 
		  aa + (kk - GEMM_UNROLL_N) * i             * COMPSIZE, 
		  b  + (kk - GEMM_UNROLL_N) * GEMM_UNROLL_N * COMPSIZE,
		  cc, ldc);
	    
	    aa += i * k * COMPSIZE;
	    cc += i     * COMPSIZE;
	  }
	  i >>= 1;
	} while (i > 0);
      }
     
      kk -= GEMM_UNROLL_N;
      j --;
    } while (j > 0);
  }

  return 0;
}


