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

int CNAME(BLASLONG m, FLOAT alpha, FLOAT *x,
		      BLASLONG incx, FLOAT *a, FLOAT *buffer){

  BLASLONG i;
  FLOAT *X;

  X = x;

  if (incx != 1) {
    COPY_K(m, x, incx, buffer, 1);
    X = buffer;
  }

  for (i = 0; i < m; i++){
#ifndef HEMVREV
#ifndef LOWER
    AXPYU_K(i + 1, 0, 0, alpha * X[i * 2 + 0], -alpha * X[i * 2 + 1], X,         1, a, 1, NULL, 0);
    a[i * 2 + 1] = ZERO;
    a += (i + 1) * 2;
#else
    AXPYU_K(m - i, 0, 0, alpha * X[i * 2 + 0], -alpha * X[i * 2 + 1], X + i * 2, 1, a, 1, NULL, 0);
    a[1] = ZERO;
    a += (m - i) * 2;
#endif
#else
#ifndef LOWER
    AXPYC_K(i + 1, 0, 0, alpha * X[i * 2 + 0], alpha * X[i * 2 + 1], X,         1, a, 1, NULL, 0);
    a[i * 2 + 1] = ZERO;
    a += (i + 1) * 2;
#else
    AXPYC_K(m - i, 0, 0, alpha * X[i * 2 + 0], alpha * X[i * 2 + 1], X + i * 2, 1, a, 1, NULL, 0);
    a[1] = ZERO;
    a += (m - i) * 2;
#endif
#endif
    }

  return 0;
}
