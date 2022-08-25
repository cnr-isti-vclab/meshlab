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

#define OFFSET	48 + STACKSIZE(%rsp)

#define STACKSIZE 64

#define KK	%r11
#define AORIG	 48(%rsp)
	
#ifdef OPTERON
#define PREFETCH	prefetch
#define PREFETCHW	prefetchw
#else
#define PREFETCH	prefetcht0
#define PREFETCHW	prefetcht0
#endif

#define PREFETCHSIZE (5 + 4 * 10)

#ifndef CONJ
#define ADD1	  faddp
#define ADD2	  fsubrp
#define ADD3	  faddp
#define ADD4	  faddp
#elif defined(LN) || defined(LT)
#define ADD1	  faddp
#define ADD2	  faddp
#define ADD3	  fsubrp
#define ADD4	  faddp
#else
#define ADD1	  faddp
#define ADD2	  faddp
#define ADD3	  faddp
#define ADD4	  fsubrp
#endif

	PROLOGUE
	PROFCODE
	
	subq	$STACKSIZE, %rsp
	movq	%rbx,  0(%rsp)
	movq	%rbp,  8(%rsp)
	movq	%r12, 16(%rsp)
	movq	%r13, 24(%rsp)
	movq	%r14, 32(%rsp)
	movq	%r15, 40(%rsp)

	movq	40 + STACKSIZE(%rsp), LDC

	salq	$ZBASE_SHIFT, LDC

	addq	$8 * SIZE, A
	addq	$8 * SIZE, B
	
#ifdef LN
       movq	M, %rax
       salq	$ZBASE_SHIFT, %rax
       addq	%rax, C
       imulq	K, %rax
       addq	%rax, A
#endif

#ifdef RT
       movq	N, %rax
       salq	$ZBASE_SHIFT, %rax
       imulq	K, %rax
       addq	%rax, B

       movq	N, %rax
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

	cmpq	$0, M
	jle	.L999
	
	movq	N,   %rax
	movq	%rax, J
	testq	%rax, %rax
	jle	.L999
	ALIGN_4

.L01:
#if defined(LT) || defined(RN)
	movq	A, AO
#else
	movq	A, AORIG
#endif

#ifdef RT
	movq	K, %rax
	salq	$ZBASE_SHIFT, %rax
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
	movq	M,  I
	ALIGN_4

.L11:
#ifdef LN
       movq	K, %rax
       salq	$ZBASE_SHIFT, %rax
       subq	%rax, AORIG
#endif

#if defined(LN) || defined(RT)
	movq	KK, %rax
	salq	$ZBASE_SHIFT, %rax
	movq	AORIG, AO
	leaq	(AO, %rax, 1), AO
	leaq	(B,  %rax, 1), BO
#else
	movq	B, BO
#endif	

	fldz
	fldz
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
 	je	.L15
	ALIGN_4

.L12:
	PREFETCH	(PREFETCHSIZE + 0) * SIZE(AO)

	FLD	 -8 * SIZE(AO)

	FLD	 -8 * SIZE(BO)
	fld	 %st(1)
	fmul	 %st(1), %st
	ADD1	 %st, %st(3)

	FLD	 -7 * SIZE(BO)
	fmul	 %st, %st(2)
	 
	FLD	 -7 * SIZE(AO)
	fmul	 %st, %st(2)
	fmulp	 %st, %st(1)

	ADD2	 %st, %st(6)
	ADD3	 %st, %st(3)
	ADD4	 %st, %st(3)

	FLD	 -6 * SIZE(AO)

	FLD	 -6 * SIZE(BO)
	fld	 %st(1)
	fmul	 %st(1), %st
	ADD1	 %st, %st(3)

	FLD	 -5 * SIZE(BO)
	fmul	 %st, %st(2)
	 
	FLD	 -5 * SIZE(AO)
	fmul	 %st, %st(2)
	fmulp	 %st, %st(1)

	ADD2	 %st, %st(6)
	ADD3	 %st, %st(3)
	ADD4	 %st, %st(3)

	PREFETCH	(PREFETCHSIZE + 4) * SIZE(AO)

	FLD	 -4 * SIZE(AO)

	FLD	 -4 * SIZE(BO)
	fld	 %st(1)
	fmul	 %st(1), %st
	ADD1	 %st, %st(3)

	FLD	 -3 * SIZE(BO)
	fmul	 %st, %st(2)
	 
	FLD	 -3 * SIZE(AO)
	fmul	 %st, %st(2)
	fmulp	 %st, %st(1)

	ADD2	 %st, %st(6)
	ADD3	 %st, %st(3)
	ADD4	 %st, %st(3)

	FLD	 -2 * SIZE(AO)

	FLD	 -2 * SIZE(BO)
	fld	 %st(1)
	fmul	 %st(1), %st
	ADD1	 %st, %st(3)

	FLD	 -1 * SIZE(BO)
	fmul	 %st, %st(2)
	 
	FLD	 -1 * SIZE(AO)
	fmul	 %st, %st(2)
	fmulp	 %st, %st(1)

	ADD2	 %st, %st(6)
	ADD3	 %st, %st(3)
	ADD4	 %st, %st(3)

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
	ADD1	 %st, %st(3)

	FLD	 -7 * SIZE(BO)
	fmul	 %st, %st(2)
	 
	FLD	 -7 * SIZE(AO)
	fmul	 %st, %st(2)
	fmulp	 %st, %st(1)

	ADD2	 %st, %st(6)
	ADD3	 %st, %st(3)
	ADD4	 %st, %st(3)

	addq	$2 * SIZE,AO
	addq	$2 * SIZE,BO

	decq	%rax
	jne	 .L16
	ALIGN_4

.L18:
	faddp	%st, %st(3)
	faddp	%st, %st(1)
	
	fxch	%st(1)

#if defined(LN) || defined(RT)
	movq	KK, %rax
#ifdef LN
	subq	$1, %rax
#else
	subq	$1, %rax
#endif

	salq	$ZBASE_SHIFT, %rax

	movq	AORIG, AO
	leaq	(AO, %rax, 1), AO
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

#if defined(LN) || defined(LT)
	FLD	-8 * SIZE(AO)
	fmul	%st(1), %st
	FLD	-8 * SIZE(AO)
	fmul	%st(3), %st
	FLD	-7 * SIZE(AO)
	fmulp	%st, %st(3)
	FLD	-7 * SIZE(AO)
	fmulp	%st, %st(4)
#endif

#if defined(RN) || defined(RT)
	FLD	-8 * SIZE(BO)
	fmul	%st(1), %st
	FLD	-8 * SIZE(BO)
	fmul	%st(3), %st
	FLD	-7 * SIZE(BO)
	fmulp	%st, %st(3)
	FLD	-7 * SIZE(BO)
	fmulp	%st, %st(4)
#endif

#ifndef CONJ
	faddp	%st, %st(2)
	fsubp	%st, %st(2)
#else
	fsubp	%st, %st(2)
	faddp	%st, %st(2)
#endif

#if defined(LN) || defined(LT)
	fld	%st
	FST	-7 * SIZE(BO)
	fxch	%st(1)
	fld	%st
	FST	-8 * SIZE(BO)
#else
	fld	%st
	FST	-7 * SIZE(AO)
	fxch	%st(1)
	fld	%st
	FST	-8 * SIZE(AO)
#endif

#ifdef LN
	subq	$2 * SIZE, CO
#endif

	FST	0 * SIZE(CO)
	FST	1 * SIZE(CO)
	
#ifndef LN
	addq	$2 * SIZE, CO
#endif

#if defined(LT) || defined(RN)
	movq	K,  %rax
	subq	KK, %rax
	salq	$ZBASE_SHIFT, %rax
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
       salq	$ZBASE_SHIFT, %rax
       addq	%rax, AORIG
#endif

	decq	I
	jne	.L11

#ifdef LN
       movq	K, %rax
       salq	$ZBASE_SHIFT, %rax
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

	decq	J
	jne	.L01
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
