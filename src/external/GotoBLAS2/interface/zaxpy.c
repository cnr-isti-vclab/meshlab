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
#ifdef FUNCTION_PROFILE
#include "functable.h"
#endif

#ifndef CBLAS

void NAME(blasint *N, FLOAT *ALPHA, FLOAT *x, blasint *INCX, FLOAT *y, blasint *INCY){
  
  blasint n    = *N;
  blasint incx = *INCX;
  blasint incy = *INCY;

#else

void CNAME(blasint n, FLOAT *ALPHA, FLOAT *x, blasint incx, FLOAT *y, blasint incy){

#endif

  FLOAT alpha_r = *(ALPHA + 0);
  FLOAT alpha_i = *(ALPHA + 1);

#ifdef SMP
  int mode, nthreads;
#endif

#ifndef CBLAS
  PRINT_DEBUG_CNAME;
#else
  PRINT_DEBUG_CNAME;
#endif

  if (n <= 0) return;

  if ((alpha_r == ZERO) && (alpha_i == ZERO)) return;

  IDEBUG_START;

  FUNCTION_PROFILE_START();

  if (incx < 0) x -= (n - 1) * incx * 2;
  if (incy < 0) y -= (n - 1) * incy * 2;

#ifdef SMP
  nthreads = num_cpu_avail(1);

  if (nthreads == 1) {
#endif

#ifndef CONJ
    AXPYU_K (n, 0, 0, alpha_r, alpha_i, x, incx, y, incy, NULL, 0);
#else
    AXPYC_K(n, 0, 0, alpha_r, alpha_i, x, incx, y, incy, NULL, 0);
#endif
    
#ifdef SMP
  } else {
    
#ifdef XDOUBLE
    mode  =  BLAS_XDOUBLE | BLAS_COMPLEX;
#elif defined(DOUBLE)
    mode  =  BLAS_DOUBLE  | BLAS_COMPLEX;
#else
    mode  =  BLAS_SINGLE  | BLAS_COMPLEX;
#endif  
    
    blas_level1_thread(mode, n, 0, 0, ALPHA, x, incx, y, incy, NULL, 0,
#ifndef CONJ
		       (void *)AXPYU_K,
#else
		       (void *)AXPYC_K,
#endif
		       nthreads);
  }
#endif
  
  FUNCTION_PROFILE_END(4, 2 * n, 2 * n);

  IDEBUG_END;

  return;
  
}
