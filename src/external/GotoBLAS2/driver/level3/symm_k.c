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

#ifndef RSIDE
#ifndef LOWER
#define ICOPY_OPERATION(M, N, A, LDA, X, Y, BUFFER) SYMM_IUTCOPY(M, N, A, LDA, Y, X, BUFFER);
#else
#define ICOPY_OPERATION(M, N, A, LDA, X, Y, BUFFER) SYMM_ILTCOPY(M, N, A, LDA, Y, X, BUFFER);
#endif
#endif

#ifdef RSIDE
#ifndef LOWER
#define OCOPY_OPERATION(M, N, A, LDA, X, Y, BUFFER) SYMM_OUTCOPY(M, N, A, LDA, Y, X, BUFFER);
#else
#define OCOPY_OPERATION(M, N, A, LDA, X, Y, BUFFER) SYMM_OLTCOPY(M, N, A, LDA, Y, X, BUFFER);
#endif
#endif

#ifndef RSIDE
#define K		args -> m
#ifndef LOWER
#define GEMM_LOCAL    SYMM_LU
#else
#define GEMM_LOCAL    SYMM_LL
#endif
#else
#define K		args -> n
#ifndef LOWER
#define GEMM_LOCAL    SYMM_RU
#else
#define GEMM_LOCAL    SYMM_RL
#endif
#endif

#ifdef THREADED_LEVEL3
#include "level3_thread.c"
#else
#include "level3.c"
#endif
