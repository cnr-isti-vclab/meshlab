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

#ifdef XDOUBLE
#define ERROR_NAME "XTRTRI"
#elif defined(DOUBLE)
#define ERROR_NAME "ZTRTRI"
#else
#define ERROR_NAME "CTRTRI"
#endif

static blasint (*trtri_single[])(blas_arg_t *, BLASLONG *, BLASLONG *, FLOAT *, FLOAT *, BLASLONG) ={
  TRTRI_UU_SINGLE, TRTRI_UN_SINGLE, TRTRI_LU_SINGLE, TRTRI_LN_SINGLE,
};

#ifdef SMP
static blasint (*trtri_parallel[])(blas_arg_t *, BLASLONG *, BLASLONG *, FLOAT *, FLOAT *, BLASLONG) ={
  TRTRI_UU_PARALLEL, TRTRI_UN_PARALLEL, TRTRI_LU_PARALLEL, TRTRI_LN_PARALLEL,
};
#endif

int NAME(char *UPLO, char *DIAG, blasint *N, FLOAT *a, blasint *ldA, blasint *Info){

  blas_arg_t args;

  blasint uplo_arg = *UPLO;
  blasint diag_arg = *DIAG;
  blasint uplo, diag;
  blasint info;
  FLOAT *buffer;
#ifdef PPC440
  extern
#endif
  FLOAT *sa, *sb;
  
  PRINT_DEBUG_NAME;

  args.n    = *N;
  args.a    = (void *)a;
  args.lda  = *ldA;
  
  TOUPPER(uplo_arg);
  TOUPPER(diag_arg);

  uplo = -1;
  if (uplo_arg == 'U') uplo = 0;
  if (uplo_arg == 'L') uplo = 1;
  diag = -1;
  if (diag_arg == 'U') diag = 0;
  if (diag_arg == 'N') diag = 1;

  info  = 0;
  if (args.lda  < MAX(1,args.n)) info = 5;
  if (args.n    < 0)             info = 3; 
  if (diag < 0)                  info = 2;
  if (uplo < 0)                  info = 1;
  if (info) {
    BLASFUNC(xerbla)(ERROR_NAME, &info, sizeof(ERROR_NAME));
    *Info = - info;
    return 0;
  }

  *Info = 0;

  if (args.n == 0) return 0;
  
  if (diag) {
    if (AMIN_K(args.n, args.a, args.lda + 1) == ZERO) {
      *Info = IAMIN_K(args.n, args.a, args.lda + 1);
      return 0;
    }
  }

  IDEBUG_START;

  FUNCTION_PROFILE_START();

#ifndef PPC440
  buffer = (FLOAT *)blas_memory_alloc(1);

  sa = (FLOAT *)((BLASLONG)buffer + GEMM_OFFSET_A);
  sb = (FLOAT *)(((BLASLONG)sa + ((GEMM_P * GEMM_Q * COMPSIZE * SIZE + GEMM_ALIGN) & ~GEMM_ALIGN)) + GEMM_OFFSET_B);
#endif

#ifdef SMP
  args.common = NULL;
  args.nthreads = num_cpu_avail(4);

  if (args.nthreads == 1) {
#endif

    *Info = (trtri_single[(uplo << 1) | diag])(&args, NULL, NULL, sa, sb, 0);
    
#ifdef SMP
  } else {

    *Info = (trtri_parallel[(uplo << 1) | diag])(&args, NULL, NULL, sa, sb, 0);
    
  }
#endif

#ifndef PPC440
  blas_memory_free(buffer);
#endif

  FUNCTION_PROFILE_END(1, .5 * args.n * args.n,
		          2. * args.n * (1./3. + args.n * ( 1./2. + args.n * 1./6.))
		       +  6. * args.n * (1./3. + args.n * (-1./2. + args.n * 1./6.)));

  IDEBUG_END;

  return 0;
}
