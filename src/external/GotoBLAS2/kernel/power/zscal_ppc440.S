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

#define ASSEMBLER
#include "common.h"
	
#define N	r3
#define XX	r4
#define PRE	r5

#ifdef linux
#ifndef __64BIT__
#define X r6
#define INCX r7
#else
#define X r8
#define INCX r9
#endif
#endif

#if defined(_AIX) || defined(__APPLE__)
#if !defined(__64BIT__) && defined(DOUBLE)
#define X r10
#define INCX r8
#else
#define X r8
#define INCX r9
#endif
#endif

#define INC1	r11

#define FZERO	f0
#define ALPHA_R	f1
#define ALPHA_I	f2
	
	PROLOGUE
	PROFCODE

	addi	SP, SP, -8
	li	r0,   0

	stw	r0,      0(SP)
	lfs	FZERO,   0(SP)
	addi	SP, SP,  8

#if (defined(_AIX) || defined(__APPLE__)) && !defined(__64BIT__) && defined(DOUBLE)
	lwz	INCX,    56(SP)
#endif
	
	slwi	INCX, INCX, ZBASE_SHIFT
	li	INC1, SIZE
	sub	X, X, INCX
	li	PRE, 3 * 16 * SIZE

	cmpwi	cr0, N, 0
	blelr-	cr0

	fcmpu	cr0, FZERO, ALPHA_R
	bne-	cr0, LL(A1I1)

	fcmpu	cr0, FZERO, ALPHA_I
	bne-	cr0, LL(A1I1)

LL(A0IN):
	srawi.	r0, N, 3
	mtspr	CTR,  r0
	beq-	LL(A0IN_Remain)
	.align 4

LL(A0IN_Kernel):
#ifdef PPCG4
	dcbtst	X, PRE
#endif
	STFDUX	FZERO, X, INCX
	STFDX	FZERO, X, INC1
	STFDUX	FZERO, X, INCX
	STFDX	FZERO, X, INC1
#if defined(PPCG4) && defined(DOUBLE)
	dcbtst	X, PRE
#endif
	STFDUX	FZERO, X, INCX
	STFDX	FZERO, X, INC1
	STFDUX	FZERO, X, INCX
	STFDX	FZERO, X, INC1
#ifdef PPCG4
	dcbtst	X, PRE
#endif
	STFDUX	FZERO, X, INCX
	STFDX	FZERO, X, INC1
	STFDUX	FZERO, X, INCX
	STFDX	FZERO, X, INC1
#if defined(PPCG4) && defined(DOUBLE)
	dcbtst	X, PRE
#endif
	STFDUX	FZERO, X, INCX
	STFDX	FZERO, X, INC1
	STFDUX	FZERO, X, INCX
	STFDX	FZERO, X, INC1
	bdnz	LL(A0IN_Kernel)
	.align 4

LL(A0IN_Remain):
	andi.	r0,  N, 7
	mtspr	CTR, r0
	beqlr+
	.align 4

LL(A0IN_RemainKernel):
	STFDUX	FZERO, X, INCX
	STFDX	FZERO, X, INC1
	bdnz	LL(A0IN_RemainKernel)
	blr
	.align 4

LL(A1I1):
	mr	XX, X

	srawi.	r0, N, 2
	mtspr	CTR,  r0
	beq-	LL(15)

	LFDUX	f0, X, INCX
	LFDX	f3, X, INC1
	LFDUX	f4, X, INCX
	LFDX	f5, X, INC1

	LFDUX	f6, X, INCX
	FMUL	f10, ALPHA_R, f0
	LFDX	f7, X, INC1
	FMUL	f11, ALPHA_R, f3
	LFDUX	f8, X, INCX
	FMUL	f12, ALPHA_R, f4
	FMUL	f13, ALPHA_R, f5
	bdz	LL(13)
	.align 4

LL(12):
#ifdef PPCG4
	dcbtst	X, PRE
#endif

	FNMSUB	f10, ALPHA_I, f3, f10
	LFDX	f9, X, INC1
	FMADD	f11, ALPHA_I, f0, f11
	LFDUX	f0, X, INCX
	FNMSUB	f12, ALPHA_I, f5, f12
	LFDX	f3, X, INC1
	FMADD	f13, ALPHA_I, f4, f13
	LFDUX	f4, X, INCX

#if defined(PPCG4) && defined(DOUBLE)
	dcbtst	X, PRE
#endif

	STFDUX	f10, XX, INCX
	FMUL	f10, ALPHA_R, f6
	STFDX	f11, XX, INC1
	FMUL	f11, ALPHA_R, f7
	STFDUX	f12, XX, INCX
	FMUL	f12, ALPHA_R, f8
	STFDX	f13, XX, INC1
	FMUL	f13, ALPHA_R, f9

#ifdef PPCG4
	dcbtst	X, PRE
#endif

	FNMSUB	f10, ALPHA_I, f7, f10
	LFDX	f5, X, INC1
	FMADD	f11, ALPHA_I, f6, f11
	LFDUX	f6, X, INCX
	FNMSUB	f12, ALPHA_I, f9, f12
	LFDX	f7, X, INC1
	FMADD	f13, ALPHA_I, f8, f13
	LFDUX	f8, X, INCX

#if defined(PPCG4) && defined(DOUBLE)
	dcbtst	X, PRE
#endif

	STFDUX	f10, XX, INCX
	FMUL	f10, ALPHA_R, f0
	STFDX	f11, XX, INC1
	FMUL	f11, ALPHA_R, f3
	STFDUX	f12, XX, INCX
	FMUL	f12, ALPHA_R, f4
	STFDX	f13, XX, INC1
	FMUL	f13, ALPHA_R, f5
	bdnz	LL(12)
	.align 4

LL(13):
	FNMSUB	f10, ALPHA_I, f3, f10
	LFDX	f9, X, INC1
	FMADD	f11, ALPHA_I, f0, f11
	FNMSUB	f12, ALPHA_I, f5, f12
	FMADD	f13, ALPHA_I, f4, f13

	STFDUX	f10, XX, INCX
	FMUL	f10, ALPHA_R, f6
	STFDX	f11, XX, INC1
	FMUL	f11, ALPHA_R, f7
	STFDUX	f12, XX, INCX
	FMUL	f12, ALPHA_R, f8
	STFDX	f13, XX, INC1
	FMUL	f13, ALPHA_R, f9

	FNMSUB	f10, ALPHA_I, f7, f10
	FMADD	f11, ALPHA_I, f6, f11
	FNMSUB	f12, ALPHA_I, f9, f12
	FMADD	f13, ALPHA_I, f8, f13

	STFDUX	f10, XX, INCX
	STFDX	f11, XX, INC1
	STFDUX	f12, XX, INCX
	STFDX	f13, XX, INC1
	.align 4

LL(15):
	andi.	r0,  N, 3
	mtspr	CTR, r0
	beqlr+
	.align 4

LL(A1IN_RemainKernel):
	LFDUX	f3, X, INCX
	LFDX	f4, X, INC1

	FMUL	f5, ALPHA_R, f3
	FMUL	f6, ALPHA_R, f4

	FNMSUB	f5, ALPHA_I, f4, f5
	FMADD	f6, ALPHA_I, f3, f6

	STFDUX	f5, XX, INCX
	STFDX	f6, XX, INC1
	bdnz	LL(A1IN_RemainKernel)
	blr

	EPILOGUE
