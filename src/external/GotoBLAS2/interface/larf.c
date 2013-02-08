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

static int (*larf[])(blas_arg_t *, BLASLONG *, BLASLONG *, FLOAT *, FLOAT *, BLASLONG) = {
  LARF_L, LARF_R,
};

int NAME(char *SIDE, blasint *M, blasint *N, FLOAT *v, blasint *incV, FLOAT *tau, FLOAT *c, blasint *ldC, FLOAT *work){

  blas_arg_t args;

  FLOAT *buffer;
#ifdef PPC440
  extern
#endif
  FLOAT *sa, *sb;

  char side_arg  = *SIDE;
  int side;
  
  PRINT_DEBUG_NAME;

  TOUPPER(side_arg);

  args.m    = *M;
  args.n    = *N;
  args.a    = (void *)v;
  args.lda  = *incV;
  args.c    = (void *)c;
  args.ldc  = *ldC;

  args.alpha = (void *)tau;

  side  = -1;
  if (side_arg  == 'L') side  = 0;
  if (side_arg  == 'R') side  = 1;

  if (args.m == 0 || args.n == 0) return 0;
  
#ifndef COMPLEX
  if (*tau == ZERO) return 0;
#else
  if ((*(tau + 0) == ZERO) && (*(tau + 1) == ZERO)) return 0;
#endif

  IDEBUG_START;

  FUNCTION_PROFILE_START();

#ifndef PPC440
  buffer = (FLOAT *)blas_memory_alloc(1);

  sa = (FLOAT *)((BLASLONG)buffer + GEMM_OFFSET_A);
  sb = (FLOAT *)(((BLASLONG)sa + ((GEMM_P * GEMM_Q * COMPSIZE * SIZE + GEMM_ALIGN) & ~GEMM_ALIGN)) + GEMM_OFFSET_B);
#endif

  larf[side](&args, NULL, NULL, sa, sb, 0);

#ifndef PPC440
  blas_memory_free(buffer);
#endif

  FUNCTION_PROFILE_END(COMPSIZE * COMPSIZE, args.m * args.n,  2. / 3. * args.m * args.n * args.n);

  IDEBUG_END;

  return 0;
}
