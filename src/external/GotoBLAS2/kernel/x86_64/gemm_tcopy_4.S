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

#if defined(PENTIUM4) || defined(GENERIC)
#define RPREFETCHSIZE	16
#define WPREFETCHSIZE (RPREFETCHSIZE * 4)
#define PREFETCH      prefetcht0
#define PREFETCHW     prefetcht0
#endif

#if defined(CORE2) || defined(PENRYN) || defined(DUNNINGTON) || defined(NEHALEM)
#define RPREFETCHSIZE	12
#define WPREFETCHSIZE (RPREFETCHSIZE * 4)
#define PREFETCH      prefetcht0
#define PREFETCHW     prefetcht2
#endif

#ifdef ATOM
#define RPREFETCHSIZE	16
#define WPREFETCHSIZE (RPREFETCHSIZE * 4)
#define PREFETCH      prefetcht0
#define PREFETCHW     prefetcht0
#endif

#ifdef NANO
#define RPREFETCHSIZE	8
#define WPREFETCHSIZE (RPREFETCHSIZE * 4)
#define PREFETCH      prefetcht0
#define PREFETCHW     prefetcht0
#endif

#ifdef BARCELONA
#define RPREFETCHSIZE	8
#define WPREFETCHSIZE (RPREFETCHSIZE * 4)
#define PREFETCH      prefetch
#define PREFETCHW     prefetchw
#endif

#ifdef GENERIC
#define RPREFETCHSIZE	16
#define WPREFETCHSIZE (RPREFETCHSIZE * 4)
#define PREFETCH      prefetcht0
#define PREFETCHW     prefetcht0
#endif

#ifndef WINDOWS_ABI

#define M	ARG1	/* rdi */
#define N	ARG2	/* rsi */
#define A	ARG3	/* rdx */
#define LDA	ARG4	/* rcx */
#define B	ARG5	/* r8  */

#define I	%r10
#define J	%rbp

#define AO1	%r9
#define AO2	%r15
#define AO3	%r11
#define AO4	%r14
#define BO1	%r13
#define BO2	%r12
#define M8	%rbx
#define BO	%rax

#else

#define STACKSIZE 256

#define M	ARG1	/* rcx */
#define N	ARG2	/* rdx */
#define A	ARG3	/* r8  */
#define LDA	ARG4	/* r9  */
#define OLD_B		40 + 64 + STACKSIZE(%rsp)

#define B	%rdi

#define I	%r10
#define J	%r11

#define AO1	%r12
#define AO2	%r13
#define AO3	%r14
#define AO4	%r15

#define BO1	%rsi
#define BO2	%rbx
#define M8	%rbp
#define BO	%rax

#endif

	PROLOGUE
	PROFCODE
	
#ifdef WINDOWS_ABI
	pushq	%rdi
	pushq	%rsi
#endif
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbp
	pushq	%rbx

#ifdef WINDOWS_ABI
	subq	$STACKSIZE, %rsp

	movups	%xmm6,    0(%rsp)
	movups	%xmm7,   16(%rsp)
	movups	%xmm8,   32(%rsp)
	movups	%xmm9,   48(%rsp)
	movups	%xmm10,  64(%rsp)
	movups	%xmm11,  80(%rsp)
	movups	%xmm12,  96(%rsp)
	movups	%xmm13, 112(%rsp)
	movups	%xmm14, 128(%rsp)
	movups	%xmm15, 144(%rsp)

	movq	OLD_B,     B
#endif

	movq	N,    %rax
	movq	N,    %rbx
	andq	$-4,  %rax
	andq	$-2,  %rbx
	imulq	M,    %rax
	imulq	M,    %rbx

	leaq	(B, %rax, SIZE), BO1
	leaq	(B, %rbx, SIZE), BO2

	leaq	(,   LDA, SIZE), LDA
	leaq	(,   M,   SIZE), M8

	movq	M,  J
	sarq	$2, J
	jle	.L20
	ALIGN_4

.L11:
	movq	A, AO1
	leaq	(A,   LDA   ), AO2
	leaq	(A,   LDA, 2), AO3
	leaq	(AO2, LDA, 2), AO4
	leaq	(A,   LDA, 4), A

	movq	B, BO
	addq	$16 * SIZE, B

	movq	N,  I
	sarq	$2, I
	jle	.L13
	ALIGN_4

.L12:
#ifndef DOUBLE
	movlps	0 * SIZE(AO1), %xmm0
	movhps	2 * SIZE(AO1), %xmm0
	movlps	0 * SIZE(AO2), %xmm1
	movhps	2 * SIZE(AO2), %xmm1

	movlps	0 * SIZE(AO3), %xmm2
	movhps	2 * SIZE(AO3), %xmm2
	movlps	0 * SIZE(AO4), %xmm3
	movhps	2 * SIZE(AO4), %xmm3

#if defined(PENTIUM4) || defined(GENERIC)
	PREFETCH	RPREFETCHSIZE * SIZE(AO1)
	PREFETCH	RPREFETCHSIZE * SIZE(AO2)
	PREFETCH	RPREFETCHSIZE * SIZE(AO3)
	PREFETCH	RPREFETCHSIZE * SIZE(AO4)

	PREFETCHW	WPREFETCHSIZE * SIZE(BO)
#endif

	movaps	%xmm0,    0 * SIZE(BO)
	movaps	%xmm1,    4 * SIZE(BO)
	movaps	%xmm2,    8 * SIZE(BO)
	movaps	%xmm3,   12 * SIZE(BO)
#else

	PREFETCH	RPREFETCHSIZE * SIZE(AO1)
	movsd	0 * SIZE(AO1), %xmm0
	movhpd	1 * SIZE(AO1), %xmm0
	movsd	2 * SIZE(AO1), %xmm1
	movhpd	3 * SIZE(AO1), %xmm1

	PREFETCH	RPREFETCHSIZE * SIZE(AO2)
	movsd	0 * SIZE(AO2), %xmm2
	movhpd	1 * SIZE(AO2), %xmm2
	movsd	2 * SIZE(AO2), %xmm3
	movhpd	3 * SIZE(AO2), %xmm3

	PREFETCH	RPREFETCHSIZE * SIZE(AO3)
	movsd	0 * SIZE(AO3), %xmm4
	movhpd	1 * SIZE(AO3), %xmm4
	movsd	2 * SIZE(AO3), %xmm5
	movhpd	3 * SIZE(AO3), %xmm5

	PREFETCH	RPREFETCHSIZE * SIZE(AO4)
	movsd	0 * SIZE(AO4), %xmm6
	movhpd	1 * SIZE(AO4), %xmm6
	movsd	2 * SIZE(AO4), %xmm7
	movhpd	3 * SIZE(AO4), %xmm7

	PREFETCHW	WPREFETCHSIZE * SIZE(BO)
	movapd	%xmm0,    0 * SIZE(BO)
	movapd	%xmm1,    2 * SIZE(BO)
	movapd	%xmm2,    4 * SIZE(BO)
	movapd	%xmm3,    6 * SIZE(BO)

#if defined(CORE2) || defined(PENRYN) || defined(DUNNINGTON)
	PREFETCHW	(WPREFETCHSIZE + 8) * SIZE(B)
#endif
	movapd	%xmm4,    8 * SIZE(BO)
	movapd	%xmm5,   10 * SIZE(BO)
	movapd	%xmm6,   12 * SIZE(BO)
	movapd	%xmm7,   14 * SIZE(BO)
#endif

	leaq	(BO, M8, 4), BO
	addq	$4 * SIZE, AO1
	addq	$4 * SIZE, AO2
	addq	$4 * SIZE, AO3
	addq	$4 * SIZE, AO4
	decq	I
	jg	.L12
	ALIGN_4

.L13:
	testq	$2, N
	jle	.L14

#ifndef DOUBLE
	movlps	0 * SIZE(AO1), %xmm0
	movhps	0 * SIZE(AO2), %xmm0

	movlps	0 * SIZE(AO3), %xmm1
	movhps	0 * SIZE(AO4), %xmm1

	movaps	%xmm0,    0 * SIZE(BO1)
	movaps	%xmm1,    4 * SIZE(BO1)
#else
	movsd	0 * SIZE(AO1), %xmm0
	movhpd	1 * SIZE(AO1), %xmm0
	movsd	0 * SIZE(AO2), %xmm1
	movhpd	1 * SIZE(AO2), %xmm1

	movsd	0 * SIZE(AO3), %xmm2
	movhpd	1 * SIZE(AO3), %xmm2
	movsd	0 * SIZE(AO4), %xmm3
	movhpd	1 * SIZE(AO4), %xmm3

	movapd	%xmm0,    0 * SIZE(BO1)
	movapd	%xmm1,    2 * SIZE(BO1)
	movapd	%xmm2,    4 * SIZE(BO1)
	movapd	%xmm3,    6 * SIZE(BO1)
#endif

	addq	$2 * SIZE, AO1
	addq	$2 * SIZE, AO2
	addq	$2 * SIZE, AO3
	addq	$2 * SIZE, AO4
	addq	$8 * SIZE, BO1
	ALIGN_4

.L14:
	testq	$1, N
	jle	.L19

#ifndef DOUBLE
	movss	0 * SIZE(AO1), %xmm0
	movss	0 * SIZE(AO2), %xmm1
	movss	0 * SIZE(AO3), %xmm2
	movss	0 * SIZE(AO4), %xmm3

	movss	%xmm0,    0 * SIZE(BO2)
	movss	%xmm1,    1 * SIZE(BO2)
	movss	%xmm2,    2 * SIZE(BO2)
	movss	%xmm3,    3 * SIZE(BO2)
#else
	movsd	0 * SIZE(AO1), %xmm0
	movhpd	0 * SIZE(AO2), %xmm0
	movsd	0 * SIZE(AO3), %xmm1
	movhpd	0 * SIZE(AO4), %xmm1

	movapd	%xmm0,    0 * SIZE(BO2)
	movapd	%xmm1,    2 * SIZE(BO2)
#endif

	addq	$4 * SIZE, BO2
	ALIGN_4

.L19:
	decq	J
	jg	.L11
	ALIGN_4

.L20:
	testq	$2, M
	jle	.L30
	ALIGN_4

.L21:
	movq	A, AO1
	leaq	(A,   LDA   ), AO2
	leaq	(A,   LDA, 2), A

	movq	B, BO
	addq	$8 * SIZE, B

	movq	N,  I
	sarq	$2, I
	jle	.L23
	ALIGN_4

.L22:
#ifndef DOUBLE
	movlps	0 * SIZE(AO1), %xmm0
	movhps	2 * SIZE(AO1), %xmm0

	movlps	0 * SIZE(AO2), %xmm1
	movhps	2 * SIZE(AO2), %xmm1

#if defined(PENTIUM4) || defined(GENERIC)
	PREFETCH	RPREFETCHSIZE * SIZE(AO1)
	PREFETCH	RPREFETCHSIZE * SIZE(AO2)
	PREFETCHW	WPREFETCHSIZE * SIZE(BO)
#endif

	movaps	%xmm0,    0 * SIZE(BO)
	movaps	%xmm1,    4 * SIZE(BO)
#else
	movsd	0 * SIZE(AO1), %xmm0
	movhpd	1 * SIZE(AO1), %xmm0
	movsd	2 * SIZE(AO1), %xmm1
	movhpd	3 * SIZE(AO1), %xmm1

	movsd	0 * SIZE(AO2), %xmm2
	movhpd	1 * SIZE(AO2), %xmm2
	movsd	2 * SIZE(AO2), %xmm3
	movhpd	3 * SIZE(AO2), %xmm3

#if defined(PENTIUM4) || defined(GENERIC)
	PREFETCH	RPREFETCHSIZE * SIZE(AO1)
	PREFETCH	RPREFETCHSIZE * SIZE(AO2)
	PREFETCHW	WPREFETCHSIZE * SIZE(BO)
#endif

	movapd	%xmm0,    0 * SIZE(BO)
	movapd	%xmm1,    2 * SIZE(BO)
	movapd	%xmm2,    4 * SIZE(BO)
	movapd	%xmm3,    6 * SIZE(BO)
#endif

	addq	$4 * SIZE, AO1
	addq	$4 * SIZE, AO2
	leaq	(BO, M8, 4), BO
	decq	I
	jg	.L22
	ALIGN_4

.L23:
	testq	$2, N
	jle	.L24

#ifndef DOUBLE
	movlps	0 * SIZE(AO1), %xmm0
	movhps	0 * SIZE(AO2), %xmm0

	movaps	%xmm0,    0 * SIZE(BO1)
#else
	movsd	0 * SIZE(AO1), %xmm0
	movhpd	1 * SIZE(AO1), %xmm0
	movsd	0 * SIZE(AO2), %xmm1
	movhpd	1 * SIZE(AO2), %xmm1

	movapd	%xmm0,    0 * SIZE(BO1)
	movapd	%xmm1,    2 * SIZE(BO1)
#endif

	addq	$2 * SIZE, AO1
	addq	$2 * SIZE, AO2
	addq	$4 * SIZE, BO1
	ALIGN_4

.L24:
	testq	$1, N
	jle	.L30

#ifndef DOUBLE
	movss	0 * SIZE(AO1), %xmm0
	movss	0 * SIZE(AO2), %xmm1

	movss	%xmm0,    0 * SIZE(BO2)
	movss	%xmm1,    1 * SIZE(BO2)
#else
	movsd	0 * SIZE(AO1), %xmm0
	movhpd	0 * SIZE(AO2), %xmm0

	movapd	%xmm0,    0 * SIZE(BO2)
#endif
	addq	$2 * SIZE, BO2
	ALIGN_4

.L30:
	testq	$1, M
	jle	.L999
	ALIGN_4

.L31:
	movq	A, AO1
	movq	B, BO

	movq	N,  I
	sarq	$2, I
	jle	.L33
	ALIGN_4

.L32:
#ifndef DOUBLE
	movlps	0 * SIZE(AO1), %xmm0
	movhps	2 * SIZE(AO1), %xmm0

	movaps	%xmm0,    0 * SIZE(BO)
#else
	movsd	0 * SIZE(AO1), %xmm0
	movhpd	1 * SIZE(AO1), %xmm0
	movsd	2 * SIZE(AO1), %xmm1
	movhpd	3 * SIZE(AO1), %xmm1

	movapd	%xmm0,    0 * SIZE(BO)
	movapd	%xmm1,    2 * SIZE(BO)
#endif

	addq	$4 * SIZE, AO1
	leaq	(BO, M8, 4), BO
	decq	I
	jg	.L32
	ALIGN_4

.L33:
	testq	$2, N
	jle	.L34

#ifndef DOUBLE
	movlps	0 * SIZE(AO1), %xmm0

	movlps	%xmm0,    0 * SIZE(BO1)
#else
	movsd	0 * SIZE(AO1), %xmm0
	movhpd	1 * SIZE(AO1), %xmm0

	movapd	%xmm0,    0 * SIZE(BO1)
#endif

	addq	$2 * SIZE, AO1
	addq	$2 * SIZE, BO1
	ALIGN_4

.L34:
	testq	$1, N
	jle	.L999

#ifndef DOUBLE
	movss	0 * SIZE(AO1), %xmm0
	movss	%xmm0,    0 * SIZE(BO2)
#else
	movsd	0 * SIZE(AO1), %xmm0
	movsd	%xmm0,    0 * SIZE(BO2)
#endif
	addq	$1 * SIZE, BO2
	ALIGN_4

.L999:
#ifdef WINDOWS_ABI
	movups	  0(%rsp), %xmm6
	movups	 16(%rsp), %xmm7
	movups	 32(%rsp), %xmm8
	movups	 48(%rsp), %xmm9
	movups	 64(%rsp), %xmm10
	movups	 80(%rsp), %xmm11
	movups	 96(%rsp), %xmm12
	movups	112(%rsp), %xmm13
	movups	128(%rsp), %xmm14
	movups	144(%rsp), %xmm15

	addq	$STACKSIZE, %rsp
#endif

	popq	%rbx
	popq	%rbp
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
#ifdef WINDOWS_ABI
	popq	%rsi
	popq	%rdi
#endif

	ret

	EPILOGUE
