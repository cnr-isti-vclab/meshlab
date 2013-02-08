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

const static FLOAT dm1 = -1.;

#ifdef CONJ
#define GEMM_KERNEL   GEMM_KERNEL_L
#if (!defined(TRANSA) && defined(UPPER)) || (defined(TRANSA) && !defined(UPPER))
#define TRSM_KERNEL   TRSM_KERNEL_LR
#else
#define TRSM_KERNEL   TRSM_KERNEL_LC
#endif
#else
#define GEMM_KERNEL   GEMM_KERNEL_N
#if (!defined(TRANSA) && defined(UPPER)) || (defined(TRANSA) && !defined(UPPER))
#define TRSM_KERNEL   TRSM_KERNEL_LN
#else
#define TRSM_KERNEL   TRSM_KERNEL_LT
#endif
#endif

#if 0
#undef GEMM_P
#undef GEMM_Q
#undef GEMM_R

#define GEMM_P 8
#define GEMM_Q 12
#define GEMM_R 1600
#endif

int CNAME(blas_arg_t *args, BLASLONG *range_m, BLASLONG *range_n, FLOAT *sa, FLOAT *sb, BLASLONG dummy) {

  BLASLONG m, n, lda, ldb;
  FLOAT *beta, *a, *b;

  BLASLONG ls, is, js;
  BLASLONG min_l, min_i, min_j;
  BLASLONG jjs, min_jj;

  m = args -> m;
  n = args -> n;

  a = (FLOAT *)args -> a;
  b = (FLOAT *)args -> b;

  lda = args -> lda;
  ldb = args -> ldb;

  beta  = (FLOAT *)args -> beta;

  if (range_n) {
    BLASLONG n_from = *(((BLASLONG *)range_n) + 0);
    BLASLONG n_to   = *(((BLASLONG *)range_n) + 1);

    n = n_to - n_from;

    b += n_from * ldb * COMPSIZE;
  }

  if (beta) {
#ifndef COMPLEX
    if (beta[0] != ONE)
      GEMM_BETA(m, n, 0, beta[0], NULL, 0, NULL, 0, b, ldb);
    if (beta[0] == ZERO) return 0;
#else
    if ((beta[0] != ONE) || (beta[1] != ZERO))
      GEMM_BETA(m, n, 0, beta[0], beta[1], NULL, 0, NULL, 0, b, ldb);
    if ((beta[0] == ZERO) && (beta[1] == ZERO)) return 0;
#endif
  }

  for(js = 0; js < n; js += GEMM_R){
    min_j = n - js;
    if (min_j > GEMM_R) min_j = GEMM_R;
    
#if (!defined(UPPER) && !defined(TRANSA)) || (defined(UPPER) && defined(TRANSA))
    for(ls = 0; ls < m; ls += GEMM_Q){
      min_l = m - ls;
      if (min_l > GEMM_Q) min_l = GEMM_Q;
      min_i = min_l;
      if (min_i > GEMM_P) min_i = GEMM_P;
      
#ifndef TRANSA
      TRSM_ILTCOPY(min_l, min_i, a + (ls + ls * lda) * COMPSIZE, lda, 0, sa);
#else
      TRSM_IUNCOPY(min_l, min_i, a + (ls + ls * lda) * COMPSIZE, lda, 0, sa);
#endif
      
      for(jjs = js; jjs < js + min_j; jjs += min_jj){
	min_jj = min_j + js - jjs;
	if (min_jj > GEMM_UNROLL_N) min_jj = GEMM_UNROLL_N;

	GEMM_ONCOPY(min_l, min_jj, b + (ls + jjs * ldb) * COMPSIZE, ldb, sb + min_l * (jjs - js) * COMPSIZE);

	TRSM_KERNEL(min_i, min_jj, min_l, dm1,
#ifdef COMPLEX
		    ZERO,
#endif
		    sa, sb + min_l * (jjs - js) * COMPSIZE, 
		    b + (ls + jjs * ldb) * COMPSIZE, ldb, 0);
      }

      for(is = ls + min_i; is < ls + min_l; is += GEMM_P){
	min_i = ls + min_l - is;
	if (min_i > GEMM_P) min_i = GEMM_P;
	
#ifndef TRANSA
	TRSM_ILTCOPY(min_l, min_i, a + (is + ls * lda) * COMPSIZE, lda, is - ls, sa);
#else
	TRSM_IUNCOPY(min_l, min_i, a + (ls + is * lda) * COMPSIZE, lda, is - ls, sa);
#endif
	
	TRSM_KERNEL(min_i, min_j, min_l, dm1,
#ifdef COMPLEX
		    ZERO,
#endif
		    sa, sb, b + (is + js * ldb) * COMPSIZE, ldb, is - ls);
      }
      

      for(is = ls + min_l; is < m; is += GEMM_P){
	min_i = m - is;
	if (min_i > GEMM_P) min_i = GEMM_P;
	
#ifndef TRANSA
	GEMM_ITCOPY(min_l, min_i, a + (is + ls * lda) * COMPSIZE, lda, sa);
#else
	GEMM_INCOPY(min_l, min_i, a + (ls + is * lda) * COMPSIZE, lda, sa);
#endif
	
	GEMM_KERNEL(min_i, min_j, min_l, dm1, 
#ifdef COMPLEX
		    ZERO,
#endif
		    sa, sb, b + (is + js * ldb) * COMPSIZE, ldb); 
      }
    }
#else
    BLASLONG start_is;

    for(ls = m; ls > 0; ls -= GEMM_Q){
      min_l = ls;
      if (min_l > GEMM_Q) min_l = GEMM_Q;
      start_is = ls - min_l;
      while (start_is + GEMM_P < ls) start_is += GEMM_P;
      min_i = ls - start_is;
      if (min_i > GEMM_P) min_i = GEMM_P;

#ifndef TRANSA
      TRSM_IUTCOPY(min_l, min_i, a + (start_is + (ls - min_l) * lda) * COMPSIZE, lda, start_is - (ls - min_l), sa);
#else
      TRSM_ILNCOPY(min_l, min_i, a + ((ls - min_l) + start_is * lda) * COMPSIZE, lda, start_is - (ls - min_l), sa);
#endif

      for(jjs = js; jjs < js + min_j; jjs += min_jj){
	min_jj = min_j + js - jjs;
	if (min_jj > GEMM_UNROLL_N) min_jj = GEMM_UNROLL_N;

	GEMM_ONCOPY(min_l, min_jj, b + (ls - min_l + jjs * ldb) * COMPSIZE, ldb, sb + min_l * (jjs - js) * COMPSIZE);
	
	TRSM_KERNEL(min_i, min_jj, min_l, dm1,
#ifdef COMPLEX
		    ZERO,
#endif
		    sa, sb + min_l * (jjs - js) * COMPSIZE, 
		    b +  (start_is + jjs * ldb) * COMPSIZE, ldb,  start_is - ls + min_l);
      }
      
      for(is = start_is - GEMM_P; is >= ls - min_l; is -= GEMM_P){
	min_i = ls - is;
	if (min_i > GEMM_P) min_i = GEMM_P;
	
#ifndef TRANSA
	TRSM_IUTCOPY(min_l, min_i, a + (is + (ls - min_l) * lda) * COMPSIZE, lda, is - (ls - min_l), sa);
#else
	TRSM_ILNCOPY(min_l, min_i, a + ((ls - min_l) + is * lda) * COMPSIZE, lda, is - (ls - min_l), sa);
#endif
	TRSM_KERNEL(min_i, min_j, min_l, dm1,
#ifdef COMPLEX
		    ZERO,
#endif
		    sa, sb, 
		    b +  (is + js * ldb) * COMPSIZE, ldb,  + is - (ls - min_l) );
      }

      
      for(is = 0; is < ls - min_l; is += GEMM_P){
	min_i = ls - min_l - is;
	if (min_i > GEMM_P) min_i = GEMM_P;
	
#ifndef TRANSA
	GEMM_ITCOPY(min_l, min_i, a + (is + (ls - min_l) * lda) * COMPSIZE, lda, sa);
#else
	GEMM_INCOPY(min_l, min_i, a + ((ls - min_l) + is * lda) * COMPSIZE, lda, sa);
#endif

	GEMM_KERNEL(min_i, min_j, min_l, dm1, 
#ifdef COMPLEX
		    ZERO,
#endif
		    sa, sb, b + (is + js * ldb) * COMPSIZE, ldb); 
      }
    }

#endif
  }

  return 0;
}
