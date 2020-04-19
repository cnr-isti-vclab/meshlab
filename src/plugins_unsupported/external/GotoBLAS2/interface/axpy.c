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
  
  BLASLONG n    = *N;
  BLASLONG incx = *INCX;
  BLASLONG incy = *INCY;
  FLOAT alpha = *ALPHA;

#else

void CNAME(blasint n, FLOAT alpha, FLOAT *x, blasint incx, FLOAT *y, blasint incy){

#endif

#ifdef SMP
  int mode, nthreads;
#endif

#ifndef CBLAS
  PRINT_DEBUG_NAME;
#else
  PRINT_DEBUG_CNAME;
#endif

  if (n <= 0) return;

  if (alpha == ZERO) return;

  IDEBUG_START;

  FUNCTION_PROFILE_START();

  if (incx < 0) x -= (n - 1) * incx;
  if (incy < 0) y -= (n - 1) * incy;

#ifdef SMP
  nthreads = num_cpu_avail(1);

  if (nthreads == 1) {
#endif

  AXPYU_K(n, 0, 0, alpha, x, incx, y, incy, NULL, 0);

#ifdef SMP
  } else {

#ifdef XDOUBLE
    mode  =  BLAS_XDOUBLE | BLAS_REAL;
#elif defined(DOUBLE)
    mode  =  BLAS_DOUBLE  | BLAS_REAL;
#else
    mode  =  BLAS_SINGLE  | BLAS_REAL;
#endif  
    
    blas_level1_thread(mode, n, 0, 0, &alpha, 
		       x, incx, y, incy, NULL, 0, (void *)AXPYU_K, nthreads);

  }
#endif

  FUNCTION_PROFILE_END(1, 2 * n, 2 * n);

  IDEBUG_END;

  return;
  
}
