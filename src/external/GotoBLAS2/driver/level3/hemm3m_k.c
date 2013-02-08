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

#undef TIMING

#define BETA_OPERATION(M_FROM, M_TO, N_FROM, N_TO, BETA, C, LDC) \
	GEMM_BETA((M_TO) - (M_FROM), (N_TO - N_FROM), 0, \
		  BETA[0], BETA[1], NULL, 0, NULL, 0, \
		  (FLOAT *)(C) + (M_FROM) + (N_FROM) * (LDC) * COMPSIZE, LDC)

#ifndef RSIDE
#ifndef LOWER
#define ICOPYB_OPERATION(M, N, A, LDA, X, Y, BUFFER) HEMM3M_IUCOPYB(M, N, A, LDA, Y, X, BUFFER)
#define ICOPYR_OPERATION(M, N, A, LDA, X, Y, BUFFER) HEMM3M_IUCOPYR(M, N, A, LDA, Y, X, BUFFER)
#define ICOPYI_OPERATION(M, N, A, LDA, X, Y, BUFFER) HEMM3M_IUCOPYI(M, N, A, LDA, Y, X, BUFFER)
#else
#define ICOPYB_OPERATION(M, N, A, LDA, X, Y, BUFFER) HEMM3M_ILCOPYB(M, N, A, LDA, Y, X, BUFFER)
#define ICOPYR_OPERATION(M, N, A, LDA, X, Y, BUFFER) HEMM3M_ILCOPYR(M, N, A, LDA, Y, X, BUFFER)
#define ICOPYI_OPERATION(M, N, A, LDA, X, Y, BUFFER) HEMM3M_ILCOPYI(M, N, A, LDA, Y, X, BUFFER)
#endif
#endif

#ifdef RSIDE
#ifndef LOWER
#define OCOPYB_OPERATION(M, N, A, LDA, ALPHA_R, ALPHA_I, X, Y, BUFFER) \
	HEMM3M_OUCOPYB(M, N, A,  LDA, Y, X, ALPHA_R, ALPHA_I, BUFFER)
#define OCOPYR_OPERATION(M, N, A, LDA, ALPHA_R, ALPHA_I, X, Y, BUFFER) \
	HEMM3M_OUCOPYR(M, N, A,  LDA, Y, X, ALPHA_R, ALPHA_I, BUFFER)
#define OCOPYI_OPERATION(M, N, A, LDA, ALPHA_R, ALPHA_I, X, Y, BUFFER) \
	HEMM3M_OUCOPYI(M, N, A,  LDA, Y, X, ALPHA_R, ALPHA_I, BUFFER)
#else
#define OCOPYB_OPERATION(M, N, A, LDA, ALPHA_R, ALPHA_I, X, Y, BUFFER) \
	HEMM3M_OLCOPYB(M, N, A,  LDA, Y, X, ALPHA_R, ALPHA_I, BUFFER)
#define OCOPYR_OPERATION(M, N, A, LDA, ALPHA_R, ALPHA_I, X, Y, BUFFER) \
	HEMM3M_OLCOPYR(M, N, A,  LDA, Y, X, ALPHA_R, ALPHA_I, BUFFER)
#define OCOPYI_OPERATION(M, N, A, LDA, ALPHA_R, ALPHA_I, X, Y, BUFFER) \
	HEMM3M_OLCOPYI(M, N, A,  LDA, Y, X, ALPHA_R, ALPHA_I, BUFFER)
#endif
#endif

#ifndef RSIDE
#define K		args -> m
#ifndef LOWER
#define GEMM3M_LOCAL    HEMM3M_LU
#else
#define GEMM3M_LOCAL    HEMM3M_LL
#endif
#else
#define K		args -> n
#ifndef LOWER
#define GEMM3M_LOCAL    HEMM3M_RU
#else
#define GEMM3M_LOCAL    HEMM3M_RL
#endif
#endif

#ifdef THREADED_LEVEL3
#include "level3_gemm3m_thread.c"
#else
#include "gemm3m_level3.c"
#endif
