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


int CNAME(BLASLONG m, BLASLONG n, BLASLONG dummy1, FLOAT alpha_r, FLOAT alpha_i,
	 FLOAT *x, BLASLONG incx,
	 FLOAT *y, BLASLONG incy,
	 FLOAT *a, BLASLONG lda, FLOAT *buffer){

  FLOAT *X    = x;

  if (incx != 1) {
    X = buffer;
    COPY_K(m, x, incx, X, 1);
  }

  lda  *= 2;
  incy *= 2;

  while (n > 0) { 
    FLOAT beta_r = y[0];
    FLOAT beta_i = y[1];

#ifndef XCONJ    
    AXPYU_K
#else
    AXPYC_K
#endif
      (m, 0, 0, 
#ifndef CONJ
       alpha_r * beta_r - alpha_i * beta_i,
       alpha_r * beta_i + alpha_i * beta_r,
#else
       alpha_r * beta_r + alpha_i * beta_i,
       -alpha_r * beta_i + alpha_i * beta_r,
#endif
       X, 1, a, 1, NULL, 0);
    
    a += lda;
    y += incy;
    n --;
  }

  return 0;
}

