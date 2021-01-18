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

#ifdef CONJ
#define GEMM_KERNEL   GEMM_KERNEL_L
#define TRMM_KERNEL_N TRMM_KERNEL_LR
#define TRMM_KERNEL_T TRMM_KERNEL_LC
#else
#define GEMM_KERNEL   GEMM_KERNEL_N
#define TRMM_KERNEL_N TRMM_KERNEL_LN
#define TRMM_KERNEL_T TRMM_KERNEL_LT
#endif

#undef TIMING

#ifdef TIMING
#define START_RPCC()		rpcc_counter = rpcc()
#define STOP_RPCC(COUNTER)	COUNTER  += rpcc() - rpcc_counter
#else
#define START_RPCC()
#define STOP_RPCC(COUNTER)
#endif

int CNAME(blas_arg_t *args, BLASLONG *range_m, BLASLONG *range_n, FLOAT *sa, FLOAT *sb, BLASLONG dummy) {

  BLASLONG m, n, lda, ldb;
  FLOAT *beta, *a, *b;

  BLASLONG ls, is, js;
  BLASLONG min_l, min_i, min_j;
  BLASLONG jjs, min_jj;

#ifdef TIMING
  unsigned long long rpcc_counter;
  unsigned long long innercost  = 0;
  unsigned long long outercost  = 0;
  unsigned long long gemmcost = 0;
  unsigned long long trmmcost = 0;
  double total;
#endif

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

#if (defined(UPPER) && !defined(TRANSA)) || (!defined(UPPER) && defined(TRANSA))

    min_l = m;
    if (min_l > GEMM_Q) min_l = GEMM_Q;
    min_i = min_l;
    if (min_i > GEMM_P) min_i = GEMM_P;
    
    START_RPCC();

#ifndef TRANSA
    TRMM_IUTCOPY(min_l, min_i, a, lda, 0, 0, sa);
#else
    TRMM_ILNCOPY(min_l, min_i, a, lda, 0, 0, sa);
#endif
    
    STOP_RPCC(innercost);

    for(jjs = js; jjs < js + min_j; jjs += min_jj){
      min_jj = min_j + js - jjs;
      if (min_jj > GEMM_UNROLL_N) min_jj = GEMM_UNROLL_N;

      START_RPCC();

      GEMM_ONCOPY(min_l, min_jj, b + (jjs * ldb) * COMPSIZE, ldb, sb + min_l * (jjs - js) * COMPSIZE);
      
      STOP_RPCC(outercost);
      
      START_RPCC();
	
      TRMM_KERNEL_N(min_i, min_jj, min_l, dp1,
#ifdef COMPLEX
		    ZERO,
#endif
		    sa, sb + min_l * (jjs - js) * COMPSIZE, b + (jjs * ldb) * COMPSIZE, ldb, 0); 

      STOP_RPCC(trmmcost);
    }


    for(is = min_i; is < min_l; is += GEMM_P){
      min_i = min_l - is;
      if (min_i > GEMM_P) min_i = GEMM_P;
      
      START_RPCC();

#ifndef TRANSA
      TRMM_IUTCOPY(min_l, min_i, a, lda, 0, is, sa);
#else
      TRMM_ILNCOPY(min_l, min_i, a, lda, 0, is, sa);
#endif
      
      STOP_RPCC(innercost);
      
      START_RPCC();
	
      TRMM_KERNEL_N(min_i, min_j, min_l, dp1,
#ifdef COMPLEX
		    ZERO,
#endif
		    sa, sb, b + (is + js * ldb) * COMPSIZE, ldb, is); 

      STOP_RPCC(trmmcost);

    }

    for(ls = min_l; ls < m; ls += GEMM_Q){
      min_l = m - ls;
      if (min_l > GEMM_Q) min_l = GEMM_Q;
      min_i = ls;
      if (min_i > GEMM_P) min_i = GEMM_P;
      
      START_RPCC();

#ifndef TRANSA
      GEMM_ITCOPY(min_l, min_i, a + (ls * lda) * COMPSIZE, lda, sa);
#else
      GEMM_INCOPY(min_l, min_i, a + (ls      ) * COMPSIZE, lda, sa);
#endif

      STOP_RPCC(innercost);

      for(jjs = js; jjs < js + min_j; jjs += min_jj){
	min_jj = min_j + js - jjs;
	if (min_jj > GEMM_UNROLL_N) min_jj = GEMM_UNROLL_N;
	
	START_RPCC();

	GEMM_ONCOPY(min_l, min_jj, b + (ls + jjs * ldb) * COMPSIZE, ldb, sb + min_l * (jjs - js) * COMPSIZE);
	
	STOP_RPCC(gemmcost);
	
	START_RPCC();

	GEMM_KERNEL(min_i, min_jj, min_l, dp1, 
#ifdef COMPLEX
		    ZERO,
#endif
		    sa, sb + min_l * (jjs - js) * COMPSIZE, 
		    b + (jjs * ldb) * COMPSIZE, ldb); 

	STOP_RPCC(gemmcost);
      }

      for(is = min_i; is < ls; is += GEMM_P){
	min_i = ls - is;
	if (min_i > GEMM_P) min_i = GEMM_P;
	
	START_RPCC();

#ifndef TRANSA
	GEMM_ITCOPY(min_l, min_i, a + (is + ls * lda) * COMPSIZE, lda, sa);
#else
	GEMM_INCOPY(min_l, min_i, a + (ls + is * lda) * COMPSIZE, lda, sa);
#endif

	STOP_RPCC(innercost);

	START_RPCC();

	GEMM_KERNEL(min_i, min_j, min_l, dp1, 
#ifdef COMPLEX
		    ZERO,
#endif
		    sa, sb, b + (is + js * ldb) * COMPSIZE, ldb); 

	STOP_RPCC(gemmcost);
      }
      
      for(is = ls; is < ls + min_l; is += GEMM_P){
	min_i = ls + min_l - is;
	if (min_i > GEMM_P) min_i = GEMM_P;
	
	START_RPCC();

#ifndef TRANSA
	TRMM_IUTCOPY(min_l, min_i, a, lda, ls, is, sa);
#else
	TRMM_ILNCOPY(min_l, min_i, a, lda, ls, is, sa);
#endif
	
	STOP_RPCC(innercost);

	START_RPCC();

	TRMM_KERNEL_N(min_i, min_j, min_l, dp1,
#ifdef COMPLEX
		      ZERO,
#endif
		      sa, sb, b + (is + js * ldb) * COMPSIZE, ldb, is - ls); 

	STOP_RPCC(trmmcost);
      }
    }

#else
    min_l = m;
    if (min_l > GEMM_Q) min_l = GEMM_Q;
    min_i = min_l;
    if (min_i > GEMM_P) min_i = GEMM_P;
    
    START_RPCC();

#ifndef TRANSA
    TRMM_ILTCOPY(min_l, min_i, a, lda, m - min_l, m - min_l, sa);
#else
    TRMM_IUNCOPY(min_l, min_i, a, lda, m - min_l, m - min_l, sa);
#endif
    
    STOP_RPCC(innercost);
    
    for(jjs = js; jjs < js + min_j; jjs += min_jj){
      min_jj = min_j + js - jjs;
      if (min_jj > GEMM_UNROLL_N) min_jj = GEMM_UNROLL_N;
      
      START_RPCC();

      GEMM_ONCOPY(min_l, min_jj, b + (m - min_l + jjs * ldb) * COMPSIZE, ldb,
		  sb + min_l * (jjs - js) * COMPSIZE);
      
      STOP_RPCC(outercost);
    
      START_RPCC();

      TRMM_KERNEL_T(min_i, min_jj, min_l, dp1,
#ifdef COMPLEX
		    ZERO,
#endif
		    sa, sb + min_l * (jjs - js) * COMPSIZE,
		    b + (m - min_l + jjs * ldb) * COMPSIZE, ldb, 0); 

      STOP_RPCC(trmmcost);
    }

    for(is = m - min_l + min_i; is < m; is += GEMM_P){
      min_i = m - is;
      if (min_i > GEMM_P) min_i = GEMM_P;
      
      START_RPCC();

#ifndef TRANSA
      TRMM_ILTCOPY(min_l, min_i, a, lda, m - min_l, is, sa);
#else
      TRMM_IUNCOPY(min_l, min_i, a, lda, m - min_l, is, sa);
#endif
      
      STOP_RPCC(innercost);
    
      START_RPCC();

      TRMM_KERNEL_T(min_i, min_j, min_l, dp1,
#ifdef COMPLEX
		    ZERO,
#endif
		    sa, sb, b + (is + js * ldb) * COMPSIZE, ldb, is - m + min_l); 

      STOP_RPCC(trmmcost);
    }

    for(ls = m - min_l; ls > 0; ls -= GEMM_Q){
      min_l = ls;
      if (min_l > GEMM_Q) min_l = GEMM_Q;
      min_i = min_l;
      if (min_i > GEMM_P) min_i = GEMM_P;
      
      START_RPCC();

#ifndef TRANSA
      TRMM_ILTCOPY(min_l, min_i, a, lda, ls - min_l, ls - min_l, sa);
#else
      TRMM_IUNCOPY(min_l, min_i, a, lda, ls - min_l, ls - min_l, sa);
#endif
      
      STOP_RPCC(innercost);

      for(jjs = js; jjs < js + min_j; jjs += min_jj){
	min_jj = min_j + js - jjs;
	if (min_jj > GEMM_UNROLL_N) min_jj = GEMM_UNROLL_N;
	
	START_RPCC();

	GEMM_ONCOPY(min_l, min_jj, b + (ls - min_l + jjs * ldb) * COMPSIZE, ldb,
		    sb + min_l * (jjs - js) * COMPSIZE);
	
	STOP_RPCC(outercost);

	START_RPCC();

	TRMM_KERNEL_T(min_i, min_jj, min_l, dp1,
#ifdef COMPLEX
		      ZERO,
#endif
		      sa, sb + min_l * (jjs - js) * COMPSIZE,
		      b + (ls - min_l + jjs * ldb) * COMPSIZE, ldb, 0); 

	STOP_RPCC(trmmcost);
      }

      for(is = ls - min_l + min_i; is < ls; is += GEMM_P){
	min_i = ls - is;
	if (min_i > GEMM_P) min_i = GEMM_P;
	
	START_RPCC();

#ifndef TRANSA
	TRMM_ILTCOPY(min_l, min_i, a, lda, ls - min_l, is, sa);
#else
	TRMM_IUNCOPY(min_l, min_i, a, lda, ls - min_l, is, sa);
#endif
	
	STOP_RPCC(innercost);

	START_RPCC();

	TRMM_KERNEL_T(min_i, min_j, min_l, dp1,
#ifdef COMPLEX
		      ZERO,
#endif
		      sa, sb, b + (is + js * ldb) * COMPSIZE, ldb, is - ls + min_l); 

	STOP_RPCC(trmmcost);
      }


      for(is = ls; is < m; is += GEMM_P){
	min_i = m - is;
	if (min_i > GEMM_P) min_i = GEMM_P;
	
	START_RPCC();

#ifndef TRANSA
	GEMM_ITCOPY(min_l, min_i, a + (is + (ls - min_l) * lda) * COMPSIZE, lda, sa);
#else
	GEMM_INCOPY(min_l, min_i, a + ((ls - min_l) + is * lda) * COMPSIZE, lda, sa);
#endif

	STOP_RPCC(innercost);

	START_RPCC();

	GEMM_KERNEL(min_i, min_j, min_l, dp1, 
#ifdef COMPLEX
		    ZERO,
#endif
		    sa, sb, b + (is + js * ldb) * COMPSIZE, ldb); 

	STOP_RPCC(gemmcost);
      }
    }

#endif

  }

#ifdef TIMING
  total = (double)outercost + (double)innercost + (double)gemmcost + (double)trmmcost;

  printf( "Copy A : %5.2f Copy  B: %5.2f  GEMM Kernel : %5.2f  TRMM Kerlnel : %5.2f   kernel Effi. : %5.2f Total Effi. : %5.2f\n",
	  innercost / total * 100., outercost / total * 100.,
	  gemmcost / total * 100., trmmcost / total * 100.,
	  (double)n * (double)n * (double)n / (double)(trmmcost + gemmcost) * 100. * (double)COMPSIZE / 2.,
	  (double)n * (double)n * (double)n / total * 100. * (double)COMPSIZE / 2.);

#endif

  return 0;
}
