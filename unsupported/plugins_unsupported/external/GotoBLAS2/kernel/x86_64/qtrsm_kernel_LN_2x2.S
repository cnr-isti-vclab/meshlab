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

#define M	ARG1
#define N	ARG2
#define K	ARG3
#define A	ARG4
#define B	ARG5
#define C	ARG6
#define LDC	%r10
	
#define I	%r12
#define J	%r13
#define AO	%r14
#define BO	%r15
#define	CO	%rbp

#define KK	%r11
#define AORIG	 48(%rsp)

#define STACKSIZE 64

#define ALPHA	 8 + STACKSIZE(%rsp)
#define OFFSET	32 + STACKSIZE(%rsp)

#ifdef OPTERON
#define PREFETCH	prefetch
#define PREFETCHW	prefetchw
#else
#define PREFETCH	prefetcht0
#define PREFETCHW	prefetcht0
#endif

#define PREFETCHSIZE (5 + 4 * 10)

	PROLOGUE
	PROFCODE
	
	subq	$STACKSIZE, %rsp
	movq	%rbx,  0(%rsp)
	movq	%rbp,  8(%rsp)
	movq	%r12, 16(%rsp)
	movq	%r13, 24(%rsp)
	movq	%r14, 32(%rsp)
	movq	%r15, 40(%rsp)

	movq	24 + STACKSIZE(%rsp), LDC

#if defined(TRMMKERNEL) && !defined(LEFT)
	movq	OFFSET, %rax
	negq	%rax
	movq	%rax, KK
#endif
	
	addq	$8 * SIZE, A
	addq	$8 * SIZE, B
	
	salq	$BASE_SHIFT, LDC

#ifdef LN
       movq	M, %rax
       salq	$BASE_SHIFT, %rax
       addq	%rax, C
       imulq	K, %rax
       addq	%rax, A
#endif

#ifdef RT
       movq	N, %rax
       salq	$BASE_SHIFT, %rax
       imulq	K, %rax
       addq	%rax, B

       movq	N,   %rax
       imulq	LDC, %rax
       addq	%rax, C
#endif

#ifdef RN
       movq	OFFSET, %rax
       negq	%rax
       movq	%rax, KK
#endif	

#ifdef RT
       movq	N, %rax
       subq	OFFSET, %rax
       movq	%rax, KK
#endif

	movq	N,   %rax
	sarq	$1,  %rax
	movq	%rax, J
	je	.L30
	ALIGN_4

.L01:
#if defined(LT) || defined(RN)
	movq	A, AO
#else
	movq	A, %rax
	movq	%rax, AORIG
#endif

#ifdef RT
	movq	K, %rax
	salq	$1 + BASE_SHIFT, %rax
	subq	%rax, B
#endif

	lea	(, LDC, 2), %rax

#ifdef RT
	subq	%rax, C
#endif
	movq	C, CO
#ifndef RT
	addq	%rax, C
#endif

#ifdef LN
	movq	OFFSET, %rax
	addq	M, %rax
	movq	%rax, KK
#endif	

#ifdef LT
	movq	OFFSET, %rax
	movq	%rax, KK
#endif

	movq	 M, %rax
	andq	$1, %rax
	je	.L20
	ALIGN_4

.L21:
#ifdef LN
       movq	K, %rax
       salq	$0 + BASE_SHIFT, %rax
       subq	%rax, AORIG
#endif

#if defined(LN) || defined(RT)
	movq	KK, %rax
	salq	$BASE_SHIFT, %rax
	movq	AORIG, AO
	leaq	(AO, %rax, 1), AO
	leaq	(B,  %rax, 2), BO
#else
	movq	B, BO
#endif	

	fldz
	fldz

#if defined(LT) || defined(RN)
	movq	KK, %rax
#else
	movq	K,  %rax
	subq	KK, %rax
#endif
	sarq	$2, %rax
 	je	.L25
	ALIGN_4

.L22:
	PREFETCH	(PREFETCHSIZE + 0) * SIZE(AO)

	FLD	 -8 * SIZE(AO)

	FLD	 -8 * SIZE(BO)
	fmul	 %st(1), %st
	faddp	 %st, %st(2)

	FLD	 -7 * SIZE(BO)
	fmulp	 %st, %st(1)
	faddp	 %st, %st(2)

	FLD	 -7 * SIZE(AO)

	FLD	 -6 * SIZE(BO)
	fmul	 %st(1), %st
	faddp	 %st, %st(2)

	FLD	 -5 * SIZE(BO)
	fmulp	 %st, %st(1)
	faddp	 %st, %st(2)

	FLD	 -6 * SIZE(AO)

	FLD	 -4 * SIZE(BO)
	fmul	 %st(1), %st
	faddp	 %st, %st(2)

	FLD	 -3 * SIZE(BO)
	fmulp	 %st, %st(1)
	faddp	 %st, %st(2)

	FLD	 -5 * SIZE(AO)

	FLD	 -2 * SIZE(BO)
	fmul	 %st(1), %st
	faddp	 %st, %st(2)

	FLD	 -1 * SIZE(BO)
	fmulp	 %st, %st(1)
	faddp	 %st, %st(2)

	addq	$4 * SIZE,AO
	addq	$8 * SIZE,BO

	decq	%rax
	jne	.L22
	ALIGN_4

.L25:
#if defined(LT) || defined(RN)
	movq	KK, %rax
#else
	movq	K,  %rax
	subq	KK, %rax
#endif
	and	$3,  %rax
	je	.L28
	ALIGN_4

.L26:
	FLD	 -8 * SIZE(AO)

	FLD	 -8 * SIZE(BO)
	fmul	 %st(1), %st
	faddp	 %st, %st(2)

	FLD	 -7 * SIZE(BO)
	fmulp	 %st, %st(1)
	faddp	 %st, %st(2)

	addq	$1 * SIZE,AO
	addq	$2 * SIZE,BO

	decq	%rax
	jne	 .L26
	ALIGN_4

.L28:
#if defined(LN) || defined(RT)
	movq	KK, %rax
#ifdef LN
	subq	$1, %rax
#else
	subq	$2, %rax
#endif

	salq	$BASE_SHIFT, %rax

	movq	AORIG, AO
	leaq	(AO, %rax, 1), AO
	leaq	(B,  %rax, 2), BO
#endif

#if defined(LN) || defined(LT)
	FLD	-8 * SIZE(BO)
	fsubp	%st, %st(1)
	FLD	-7 * SIZE(BO)
	fsubp	%st, %st(2)
#else
	FLD	-8 * SIZE(AO)
	fsubp	%st, %st(1)
	FLD	-7 * SIZE(AO)
	fsubp	%st, %st(2)
#endif

#if defined(LN) || defined(LT)
       FLD	-8 * SIZE(AO)
       fmul	%st, %st(1)
       fmulp	%st, %st(2)
#endif

#ifdef RN
       FLD	-8 * SIZE(BO)
       fmulp	%st, %st(1)

       FLD	-7 * SIZE(BO)
       fmul	%st(1), %st

       fsubrp	%st, %st(2)

       FLD	-5 * SIZE(BO)
       fmulp	%st, %st(2)
#endif

#ifdef RT
       FLD	-5 * SIZE(BO)
       fmulp	%st, %st(2)

       FLD	-6 * SIZE(BO)
       fmul	%st(2), %st

       fsubrp	%st, %st(1)

       FLD	-8 * SIZE(BO)
       fmulp	%st, %st(1)
#endif

#ifdef LN
	subq	$1 * SIZE, CO
#endif

#if defined(LN) || defined(LT)
	fld	%st
	FST	-8 * SIZE(BO)
	fxch	%st(1)
	fld	%st
	FST	-7 * SIZE(BO)
#else
	fld	%st
	FST	-8 * SIZE(AO)
	fxch	%st(1)
	fld	%st
	FST	-7 * SIZE(AO)
#endif

	FST	0 * SIZE(CO, LDC)
	FST	0 * SIZE(CO)

#ifndef LN
	addq	$1 * SIZE, CO
#endif

#if defined(LT) || defined(RN)
	movq	K,  %rax
	subq	KK, %rax
	salq	$BASE_SHIFT, %rax
	leaq	(AO, %rax, 1), AO
	leaq	(BO, %rax, 2), BO
#endif

#ifdef LN
	subq	$1, KK
#endif

#ifdef LT
	addq	$1, KK
#endif

#ifdef RT
       movq	K, %rax
       salq	$0 + BASE_SHIFT, %rax
       addq	%rax, AORIG
#endif
	ALIGN_4

.L20:
	movq	M,  I
	sarq	$1, I
	je	.L29
	ALIGN_4

.L11:
#ifdef LN
       movq	K, %rax
       salq	$1 + BASE_SHIFT, %rax
       subq	%rax, AORIG
#endif

#if defined(LN) || defined(RT)
	movq	KK, %rax
	salq	$BASE_SHIFT, %rax
	movq	AORIG, AO
	leaq	(AO, %rax, 2), AO
	leaq	(B,  %rax, 2), BO
#else
	movq	B, BO
#endif	

	fldz
	fldz
	fldz
	fldz

#if   defined(HAVE_3DNOW)
	prefetchw	2 * SIZE(CO)
 	prefetchw	2 * SIZE(CO, LDC, 1)
#elif defined(HAVE_SSE)
	prefetchnta	2 * SIZE(CO)
 	prefetchnta	2 * SIZE(CO, LDC, 1)
#endif

#if defined(LT) || defined(RN)
	movq	KK, %rax
#else
	movq	K,  %rax
	subq	KK, %rax
#endif
	sarq	$2, %rax
 	je	.L15
	ALIGN_4

.L12:
	PREFETCH	(PREFETCHSIZE + 0) * SIZE(AO)

	FLD	 -8 * SIZE(AO)

	FLD	 -8 * SIZE(BO)
	fld	 %st(1)
	fmul	 %st(1), %st
	faddp	 %st, %st(3)

	FLD	 -7 * SIZE(BO)
	fmul	 %st, %st(2)
	 
	FLD	 -7 * SIZE(AO)
	fmul	 %st, %st(2)
	fmulp	 %st, %st(1)

	faddp	 %st, %st(6)
	faddp	 %st, %st(4)
	faddp	 %st, %st(2)

	FLD	 -6 * SIZE(AO)

	FLD	 -6 * SIZE(BO)
	fld	 %st(1)
	fmul	 %st(1), %st
	faddp	 %st, %st(3)

	FLD	 -5 * SIZE(BO)
	fmul	 %st, %st(2)
	 
	FLD	 -5 * SIZE(AO)
	fmul	 %st, %st(2)
	fmulp	 %st, %st(1)

	faddp	 %st, %st(6)
	faddp	 %st, %st(4)
	faddp	 %st, %st(2)

	PREFETCH	(PREFETCHSIZE + 4) * SIZE(AO)

	FLD	 -4 * SIZE(AO)

	FLD	 -4 * SIZE(BO)
	fld	 %st(1)
	fmul	 %st(1), %st
	faddp	 %st, %st(3)

	FLD	 -3 * SIZE(BO)
	fmul	 %st, %st(2)
	 
	FLD	 -3 * SIZE(AO)
	fmul	 %st, %st(2)
	fmulp	 %st, %st(1)

	faddp	 %st, %st(6)
	faddp	 %st, %st(4)
	faddp	 %st, %st(2)

	FLD	 -2 * SIZE(AO)

	FLD	 -2 * SIZE(BO)
	fld	 %st(1)
	fmul	 %st(1), %st
	faddp	 %st, %st(3)

	FLD	 -1 * SIZE(BO)
	fmul	 %st, %st(2)
	 
	FLD	 -1 * SIZE(AO)
	fmul	 %st, %st(2)
	fmulp	 %st, %st(1)

	faddp	 %st, %st(6)
	faddp	 %st, %st(4)
	faddp	 %st, %st(2)

	addq	$8 * SIZE,AO
	addq	$8 * SIZE,BO

	decq	%rax
	jne	.L12
	ALIGN_4

.L15:
#if defined(LT) || defined(RN)
	movq	KK, %rax
#else
	movq	K,  %rax
	subq	KK, %rax
#endif
	and	$3,  %rax
	je	.L18
	ALIGN_4

.L16:
	FLD	 -8 * SIZE(AO)

	FLD	 -8 * SIZE(BO)
	fld	 %st(1)
	fmul	 %st(1), %st
	faddp	 %st, %st(3)

	FLD	 -7 * SIZE(BO)
	fmul	 %st, %st(2)
	 
	FLD	 -7 * SIZE(AO)
	fmul	 %st, %st(2)
	fmulp	 %st, %st(1)

	faddp	 %st, %st(6)
	faddp	 %st, %st(4)
	faddp	 %st, %st(2)

	addq	$2 * SIZE,AO
	addq	$2 * SIZE,BO

	decq	%rax
	jne	 .L16
	ALIGN_4

.L18:
#if defined(LN) || defined(RT)
	movq	KK, %rax
#ifdef LN
	subq	$2, %rax
#else
	subq	$2, %rax
#endif

	salq	$BASE_SHIFT, %rax

	movq	AORIG, AO
	leaq	(AO, %rax, 2), AO
	leaq	(B,  %rax, 2), BO
#endif

#if defined(LN) || defined(LT)
	FLD	-8 * SIZE(BO)
	fsubp	%st, %st(1)
	FLD	-7 * SIZE(BO)
	fsubp	%st, %st(2)
	FLD	-6 * SIZE(BO)
	fsubp	%st, %st(3)
	FLD	-5 * SIZE(BO)
	fsubp	%st, %st(4)
#else
	FLD	-8 * SIZE(AO)
	fsubp	%st, %st(1)
	FLD	-7 * SIZE(AO)
	fsubp	%st, %st(3)
	FLD	-6 * SIZE(AO)
	fsubp	%st, %st(2)
	FLD	-5 * SIZE(AO)
	fsubp	%st, %st(4)
#endif

#ifdef LN
       FLD	-5 * SIZE(AO)
       fmul	%st, %st(3)
       fmulp	%st, %st(4)

       FLD	-6 * SIZE(AO)
       fmul	%st(3), %st
       FLD	-6 * SIZE(AO)
       fmul	%st(5), %st

       fsubrp	%st, %st(3)
       fsubrp	%st, %st(1)

       FLD	-8 * SIZE(AO)
       fmul	%st, %st(1)
       fmulp	%st, %st(2)
#endif

#ifdef LT
       FLD	-8 * SIZE(AO)
       fmul	%st, %st(1)
       fmulp	%st, %st(2)

       FLD	-7 * SIZE(AO)
       fmul	%st(1), %st
       FLD	-7 * SIZE(AO)
       fmul	%st(3), %st

       fsubrp	%st, %st(5)
       fsubrp	%st, %st(3)

       FLD	-5 * SIZE(AO)
       fmul	%st, %st(3)
       fmulp	%st, %st(4)
#endif

#ifdef RN
       FLD	-8 * SIZE(BO)
       fmul	%st, %st(1)
       fmulp	%st, %st(3)

       FLD	-7 * SIZE(BO)
       fmul	%st(1), %st
       FLD	-7 * SIZE(BO)
       fmul	%st(4), %st

       fsubrp	%st, %st(5)
       fsubrp	%st, %st(2)

       FLD	-5 * SIZE(BO)
       fmul	%st, %st(2)
       fmulp	%st, %st(4)
#endif

#ifdef RT
       FLD	-5 * SIZE(BO)
       fmul	%st, %st(2)
       fmulp	%st, %st(4)

       FLD	-6 * SIZE(BO)
       fmul	%st(2), %st
       FLD	-6 * SIZE(BO)
       fmul	%st(5), %st

       fsubrp	%st, %st(4)
       fsubrp	%st, %st(1)

       FLD	-8 * SIZE(BO)
       fmul	%st, %st(1)
       fmulp	%st, %st(3)
#endif

#ifdef LN
	subq	$2 * SIZE, CO
#endif

#if defined(LN) || defined(LT)
	fld	%st
	FST	-8 * SIZE(BO)
	fxch	%st(1)
	fld	%st
	FST	-7 * SIZE(BO)
	fxch	%st(2)
	fld	%st
	FST	-6 * SIZE(BO)
	fxch	%st(3)
	fld	%st
	FST	-5 * SIZE(BO)

	FST	1 * SIZE(CO, LDC)
	FST	0 * SIZE(CO)
	FST	0 * SIZE(CO, LDC)
	FST	1 * SIZE(CO)
#else
	fld	%st
	FST	-8 * SIZE(AO)
	fxch	%st(2)
	fld	%st
	FST	-7 * SIZE(AO)
	fxch	%st(1)
	fld	%st
	FST	-6 * SIZE(AO)
	fxch	%st(3)
	fld	%st
	FST	-5 * SIZE(AO)

	FST	1 * SIZE(CO, LDC)
	FST	1 * SIZE(CO)
	FST	0 * SIZE(CO)
	FST	0 * SIZE(CO, LDC)
#endif

#ifndef LN
	addq	$2 * SIZE, CO
#endif

#if defined(LT) || defined(RN)
	movq	K,  %rax
	subq	KK, %rax
	salq	$BASE_SHIFT, %rax
	leaq	(AO, %rax, 2), AO
	leaq	(BO, %rax, 2), BO
#endif

#ifdef LN
	subq	$2, KK
#endif

#ifdef LT
	addq	$2, KK
#endif

#ifdef RT
       movq	K, %rax
       salq	$1 + BASE_SHIFT, %rax
       addq	%rax, AORIG
#endif

	decq	I
	jne	.L11
	ALIGN_4

.L29:
#ifdef LN
       movq	K, %rax
       salq	$BASE_SHIFT, %rax
       leaq	(B, %rax, 2), B
#endif

#if defined(LT) || defined(RN)
	movq	BO, B
#endif

#ifdef RN
	addq	$2, KK
#endif

#ifdef RT
	subq	$2, KK
#endif

	decq	J
	jne	.L01
	ALIGN_4

.L30:
	movq	N,  %rax
	testq	$1, %rax	
	je	.L999

#if defined(LT) || defined(RN)
	movq	A, AO
#else
	movq	A, %rax
	movq	%rax, AORIG
#endif

#ifdef RT
	movq	K, %rax
	salq	$0 + BASE_SHIFT, %rax
	subq	%rax, B
#endif

#ifdef RT
	subq	LDC, C
#endif
	movq	C, CO
#ifndef RT
	addq	LDC, C
#endif

#ifdef LN
	movq	OFFSET, %rax
	addq	M, %rax
	movq	%rax, KK
#endif	

#ifdef LT
	movq	OFFSET, %rax
	movq	%rax, KK
#endif

	movq	 M, %rax
	andq	$1, %rax
	je	.L40
	ALIGN_4

.L41:
#ifdef LN
       movq	K, %rax
       salq	$0 + BASE_SHIFT, %rax
       subq	%rax, AORIG
#endif

#if defined(LN) || defined(RT)
	movq	KK, %rax
	salq	$BASE_SHIFT, %rax
	movq	AORIG, AO
	leaq	(AO, %rax, 1), AO
	leaq	(B,  %rax, 1), BO
#else
	movq	B, BO
#endif	

	fldz

#if defined(LT) || defined(RN)
	movq	KK, %rax
#else
	movq	K,  %rax
	subq	KK, %rax
#endif
	sarq	$2, %rax
 	je	.L45
	ALIGN_4

.L42:
	PREFETCH	(PREFETCHSIZE + 0) * SIZE(AO)

	FLD	 -8 * SIZE(AO)
	FLD	 -8 * SIZE(BO)
	fmulp	 %st, %st(1)
	faddp	 %st, %st(1)

	FLD	 -7 * SIZE(AO)
	FLD	 -7 * SIZE(BO)
	fmulp	 %st, %st(1)
	faddp	 %st, %st(1)

	FLD	 -6 * SIZE(AO)
	FLD	 -6 * SIZE(BO)
	fmulp	 %st, %st(1)
	faddp	 %st, %st(1)

	FLD	 -5 * SIZE(AO)
	FLD	 -5 * SIZE(BO)
	fmulp	 %st, %st(1)
	faddp	 %st, %st(1)

	addq	$4 * SIZE,AO
	addq	$4 * SIZE,BO

	decq	%rax
	jne	.L42
	ALIGN_4

.L45:
#if defined(LT) || defined(RN)
	movq	KK, %rax
#else
	movq	K,  %rax
	subq	KK, %rax
#endif
	and	$3,  %rax
	je	.L48
	ALIGN_4

.L46:
	FLD	 -8 * SIZE(AO)

	FLD	 -8 * SIZE(BO)
	fmulp	 %st, %st(1)
	faddp	 %st, %st(1)

	addq	$1 * SIZE,AO
	addq	$1 * SIZE,BO

	decq	%rax
	jne	 .L46
	ALIGN_4

.L48:
#if defined(LN) || defined(RT)
	movq	KK, %rax
#ifdef LN
	subq	$1, %rax
#else
	subq	$1, %rax
#endif

	salq	$BASE_SHIFT, %rax

	movq	AORIG, AO
	leaq	(AO, %rax, 1), AO
	leaq	(B,  %rax, 1), BO
#endif

#if defined(LN) || defined(LT)
	FLD	-8 * SIZE(BO)
	fsubp	%st, %st(1)
#else
	FLD	-8 * SIZE(AO)
	fsubp	%st, %st(1)
#endif

#ifdef LN
       FLD	-8 * SIZE(AO)
       fmulp	%st, %st(1)
#endif

#ifdef LT
       FLD	-8 * SIZE(AO)
       fmulp	%st, %st(1)
#endif

#ifdef RN
       FLD	-8 * SIZE(BO)
       fmulp	%st, %st(1)
#endif

#ifdef RT
       FLD	-8 * SIZE(BO)
       fmulp	%st, %st(1)
#endif

#ifdef LN
	subq	$1 * SIZE, CO
#endif

#if defined(LN) || defined(LT)
	fld	%st
	FST	-8 * SIZE(BO)
#else
	fld	%st
	FST	-8 * SIZE(AO)
#endif

	FST	0 * SIZE(CO)

#ifndef LN
	addq	$1 * SIZE, CO
#endif

#if defined(LT) || defined(RN)
	movq	K,  %rax
	subq	KK, %rax
	salq	$BASE_SHIFT, %rax
	leaq	(AO, %rax, 1), AO
	leaq	(BO, %rax, 1), BO
#endif

#ifdef LN
	subq	$1, KK
#endif

#ifdef LT
	addq	$1, KK
#endif

#ifdef RT
       movq	K, %rax
       salq	$0 + BASE_SHIFT, %rax
       addq	%rax, AORIG
#endif
	ALIGN_4

.L40:
	movq	M,  I
	sarq	$1, I
	je	.L49
	ALIGN_4

.L31:
#ifdef LN
       movq	K, %rax
       salq	$1 + BASE_SHIFT, %rax
       subq	%rax, AORIG
#endif

#if defined(LN) || defined(RT)
	movq	KK, %rax
	salq	$BASE_SHIFT, %rax
	movq	AORIG, AO
	leaq	(AO, %rax, 2), AO
	leaq	(B,  %rax, 1), BO
#else
	movq	B, BO
#endif	

	fldz
	fldz

#if   defined(HAVE_3DNOW)
	prefetchw	2 * SIZE(CO)
#elif defined(HAVE_SSE)
	prefetchnta	2 * SIZE(CO)
#endif

#if defined(LT) || defined(RN)
	movq	KK, %rax
#else
	movq	K,  %rax
	subq	KK, %rax
#endif
	sarq	$2, %rax
 	je	.L35
	ALIGN_4

.L32:
	PREFETCH	(PREFETCHSIZE + 0) * SIZE(AO)

	FLD	 -8 * SIZE(BO)
	FLD	 -8 * SIZE(AO)
	fmul	 %st(1), %st
	faddp	 %st, %st(2)

	FLD	 -7 * SIZE(AO)
	fmulp	 %st, %st(1)
	faddp	 %st, %st(2)

	FLD	 -7 * SIZE(BO)
	FLD	 -6 * SIZE(AO)
	fmul	 %st(1), %st
	faddp	 %st, %st(2)

	FLD	 -5 * SIZE(AO)
	fmulp	 %st, %st(1)
	faddp	 %st, %st(2)

	FLD	 -6 * SIZE(BO)
	FLD	 -4 * SIZE(AO)
	fmul	 %st(1), %st
	faddp	 %st, %st(2)

	FLD	 -3 * SIZE(AO)
	fmulp	 %st, %st(1)
	faddp	 %st, %st(2)

	FLD	 -5 * SIZE(BO)
	FLD	 -2 * SIZE(AO)
	fmul	 %st(1), %st
	faddp	 %st, %st(2)

	FLD	 -1 * SIZE(AO)
	fmulp	 %st, %st(1)
	faddp	 %st, %st(2)

	addq	$8 * SIZE,AO
	addq	$4 * SIZE,BO

	decq	%rax
	jne	.L32
	ALIGN_4

.L35:
#if defined(LT) || defined(RN)
	movq	KK, %rax
#else
	movq	K,  %rax
	subq	KK, %rax
#endif
	and	$3,  %rax
	je	.L38
	ALIGN_4

.L36:
	FLD	 -8 * SIZE(BO)

	FLD	 -8 * SIZE(AO)
	fmul	 %st(1), %st
	faddp	 %st, %st(2)

	FLD	 -7 * SIZE(AO)
	fmulp	 %st, %st(1)
	faddp	 %st, %st(2)

	addq	$2 * SIZE,AO
	addq	$1 * SIZE,BO

	decq	%rax
	jne	 .L36
	ALIGN_4

.L38:
#if defined(LN) || defined(RT)
	movq	KK, %rax
#ifdef LN
	subq	$2, %rax
#else
	subq	$1, %rax
#endif

	salq	$BASE_SHIFT, %rax

	movq	AORIG, AO
	leaq	(AO, %rax, 2), AO
	leaq	(B,  %rax, 1), BO
#endif

#if defined(LN) || defined(LT)
	FLD	-8 * SIZE(BO)
	fsubp	%st, %st(1)
	FLD	-7 * SIZE(BO)
	fsubp	%st, %st(2)
#else
	FLD	-8 * SIZE(AO)
	fsubp	%st, %st(1)
	FLD	-7 * SIZE(AO)
	fsubp	%st, %st(2)
#endif

#ifdef LN
       FLD	-5 * SIZE(AO)
       fmulp	%st, %st(2)

       FLD	-6 * SIZE(AO)
       fmul	%st(2), %st

       fsubrp	%st, %st(1)
       FLD	-8 * SIZE(AO)
       fmulp	%st, %st(1)
#endif

#ifdef LT
       FLD	-8 * SIZE(AO)
       fmulp	%st, %st(1)

       FLD	-7 * SIZE(AO)
       fmul	%st(1), %st

       fsubrp	%st, %st(2)

       FLD	-5 * SIZE(AO)
       fmulp	%st, %st(2)
#endif

#ifdef RN
       FLD	-8 * SIZE(BO)
       fmul	%st, %st(1)
       fmulp	%st, %st(2)
#endif

#ifdef RT
       FLD	-8 * SIZE(BO)
       fmul	%st, %st(1)
       fmulp	%st, %st(2)
#endif

#ifdef LN
	subq	$2 * SIZE, CO
#endif

#if defined(LN) || defined(LT)
	fld	%st
	FST	-8 * SIZE(BO)
	fxch	%st(1)
	fld	%st
	FST	-7 * SIZE(BO)
#else
	fld	%st
	FST	-8 * SIZE(AO)
	fxch	%st(1)
	fld	%st
	FST	-7 * SIZE(AO)
#endif

	FST	1 * SIZE(CO)
	FST	0 * SIZE(CO)

#ifndef LN
	addq	$2 * SIZE, CO
#endif

#if defined(LT) || defined(RN)
	movq	K,  %rax
	subq	KK, %rax
	salq	$BASE_SHIFT, %rax
	leaq	(AO, %rax, 2), AO
	leaq	(BO, %rax, 1), BO
#endif

#ifdef LN
	subq	$2, KK
#endif

#ifdef LT
	addq	$2, KK
#endif

#ifdef RT
       movq	K, %rax
       salq	$1 + BASE_SHIFT, %rax
       addq	%rax, AORIG
#endif

	decq	I
	jne	.L31
	ALIGN_4

.L49:
#ifdef LN
       movq	K, %rax
       salq	$BASE_SHIFT, %rax
       leaq	(B, %rax, 1), B
#endif

#if defined(LT) || defined(RN)
	movq	BO, B
#endif

#ifdef RN
	addq	$1, KK
#endif

#ifdef RT
	subq	$1, KK
#endif
	ALIGN_4

.L999:
	movq	  0(%rsp), %rbx
	movq	  8(%rsp), %rbp
	movq	 16(%rsp), %r12
	movq	 24(%rsp), %r13
	movq	 32(%rsp), %r14
	movq	 40(%rsp), %r15
	addq	$STACKSIZE, %rsp
	ret

	EPILOGUE
