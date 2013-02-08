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

#define N	ARG1	/* rdi */
#define X	ARG2	/* rsi */
#define INCX	ARG3	/* rdx */
#define Y	ARG4	/* rcx */
#ifndef WINDOWS_ABI
#define INCY	ARG5	/* r8  */
#else
#define INCY	%r10
#endif

#include "l1param.h"

	PROLOGUE
	PROFCODE

#ifdef WINDOWS_ABI
	movq	40(%rsp), INCY
#endif

	SAVEREGISTERS

	leaq	(, INCX, SIZE), INCX	
	pxor	%xmm0, %xmm0
	leaq	(, INCY, SIZE), INCY	
	pxor	%xmm1, %xmm1

	pxor	%xmm2, %xmm2
	cmpq	$0, N
	pxor	%xmm3, %xmm3
	jle	.L999

	cmpq	$SIZE, INCX
	jne	.L50
	cmpq	$SIZE, INCY
	jne	.L50

	movq	N,  %rax
	sarq	$3, %rax
	jle	.L14

	movsd	 0 * SIZE(X), %xmm4
	movsd	 0 * SIZE(Y), %xmm8
	movsd	 1 * SIZE(X), %xmm5
	movsd	 1 * SIZE(Y), %xmm9

	movsd	 2 * SIZE(X), %xmm6
	mulsd	%xmm8,  %xmm4
	movsd	 2 * SIZE(Y), %xmm10
	mulsd	%xmm9,  %xmm5
	movsd	 3 * SIZE(X), %xmm7
	mulsd	%xmm10, %xmm6
	movsd	 3 * SIZE(Y), %xmm11
	mulsd	%xmm11, %xmm7

	decq	%rax
	jle	.L12

	ALIGN_3

.L11:
#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	addsd	%xmm4,  %xmm0
	movsd	 4 * SIZE(X), %xmm4
	addsd	%xmm5,  %xmm1
	movsd	 4 * SIZE(Y), %xmm8
	addsd	%xmm6,  %xmm2
	movsd	 5 * SIZE(X), %xmm5
	addsd	%xmm7,  %xmm3
	movsd	 5 * SIZE(Y), %xmm9

	movsd	 6 * SIZE(X), %xmm6
	mulsd	%xmm8,  %xmm4
	movsd	 6 * SIZE(Y), %xmm10
	mulsd	%xmm9,  %xmm5
	movsd	 7 * SIZE(X), %xmm7
	mulsd	%xmm10, %xmm6
	movsd	 7 * SIZE(Y), %xmm11
	mulsd	%xmm11, %xmm7

#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(Y)
#endif

	addsd	%xmm4,  %xmm0
	movsd	 8 * SIZE(X), %xmm4
	addsd	%xmm5,  %xmm1
	movsd	 8 * SIZE(Y), %xmm8
	addsd	%xmm6,  %xmm2
	movsd	 9 * SIZE(X), %xmm5
	addsd	%xmm7,  %xmm3
	movsd	 9 * SIZE(Y), %xmm9

	movsd	10 * SIZE(X), %xmm6
	mulsd	%xmm8,  %xmm4
	movsd	10 * SIZE(Y), %xmm10
	mulsd	%xmm9,  %xmm5
	movsd	11 * SIZE(X), %xmm7
	mulsd	%xmm10, %xmm6
	movsd	11 * SIZE(Y), %xmm11
	mulsd	%xmm11, %xmm7

	addq	$8 * SIZE, X
	addq	$8 * SIZE, Y

	decq	%rax
	jg	.L11
	ALIGN_3

.L12:
	addsd	%xmm4,  %xmm0
	movsd	 4 * SIZE(X), %xmm4
	addsd	%xmm5,  %xmm1
	movsd	 4 * SIZE(Y), %xmm8
	addsd	%xmm6,  %xmm2
	movsd	 5 * SIZE(X), %xmm5
	addsd	%xmm7,  %xmm3
	movsd	 5 * SIZE(Y), %xmm9

	movsd	 6 * SIZE(X), %xmm6
	mulsd	%xmm8,  %xmm4
	movsd	 6 * SIZE(Y), %xmm10
	mulsd	%xmm9,  %xmm5
	movsd	 7 * SIZE(X), %xmm7
	mulsd	%xmm10, %xmm6
	movsd	 7 * SIZE(Y), %xmm11
	mulsd	%xmm11, %xmm7

	addsd	%xmm4,  %xmm0
	addsd	%xmm5,  %xmm1
	addsd	%xmm6,  %xmm2
	addsd	%xmm7,  %xmm3

	addq	$ 8 * SIZE, X
	addq	$ 8 * SIZE, Y
	ALIGN_3

.L14:
	testq	$7, N
	jle	.L999

	testq	$4, N
	jle	.L16

	movsd	 0 * SIZE(X), %xmm4
	movsd	 0 * SIZE(Y), %xmm8
	movsd	 1 * SIZE(X), %xmm5
	movsd	 1 * SIZE(Y), %xmm9

	movsd	 2 * SIZE(X), %xmm6
	mulsd	%xmm8,  %xmm4
	movsd	 2 * SIZE(Y), %xmm10
	mulsd	%xmm9,  %xmm5
	movsd	 3 * SIZE(X), %xmm7
	mulsd	%xmm10, %xmm6
	movsd	 3 * SIZE(Y), %xmm11
	mulsd	%xmm11, %xmm7

	addsd	%xmm4,  %xmm0
	addsd	%xmm5,  %xmm1
	addsd	%xmm6,  %xmm2
	addsd	%xmm7,  %xmm3

	addq	$4 * SIZE, X
	addq	$4 * SIZE, Y
	ALIGN_3

.L16:
	testq	$2, N
	jle	.L17

	movsd	 0 * SIZE(X), %xmm4
	movsd	 0 * SIZE(Y), %xmm8
	movsd	 1 * SIZE(X), %xmm5
	movsd	 1 * SIZE(Y), %xmm9

	mulsd	%xmm8,  %xmm4
	mulsd	%xmm9,  %xmm5
	addsd	%xmm4,  %xmm0
	addsd	%xmm5,  %xmm1

	addq	$2 * SIZE, X
	addq	$2 * SIZE, Y
	ALIGN_3

.L17:
	testq	$1, N
	jle	.L999

	movsd	 0 * SIZE(X), %xmm4
	movsd	 0 * SIZE(Y), %xmm8

	mulsd	%xmm8,  %xmm4
	addsd	%xmm4,  %xmm0
	jmp	.L999
	ALIGN_3

.L50:
	movq	N,  %rax
	sarq	$2, %rax
	jle	.L55
	ALIGN_3

.L53:
	movsd	0 * SIZE(X), %xmm4
	addq	INCX, X
	mulsd	0 * SIZE(Y), %xmm4
	addq	INCY, Y
	movsd	0 * SIZE(X), %xmm5
	addq	INCX, X
	mulsd	0 * SIZE(Y), %xmm5
	addq	INCY, Y
	movsd	0 * SIZE(X), %xmm6
	addq	INCX, X
	mulsd	0 * SIZE(Y), %xmm6
	addq	INCY, Y
	movsd	0 * SIZE(X), %xmm7
	addq	INCX, X
	mulsd	0 * SIZE(Y), %xmm7
	addq	INCY, Y

	addsd	%xmm4, %xmm0
	addsd	%xmm5, %xmm1
	addsd	%xmm6, %xmm2
	addsd	%xmm7, %xmm3

	decq	%rax
	jg	.L53
	ALIGN_3

.L55:
	movq	N, %rax
	andq	$3,   %rax
	jle	.L999
	ALIGN_3

.L56:
	movsd	0 * SIZE(X), %xmm4
	addq	INCX, X
	mulsd	0 * SIZE(Y), %xmm4
	addq	INCY, Y
	addsd	%xmm4, %xmm0
	decq	%rax
	jg	.L56
	ALIGN_3

.L999:
	addsd	%xmm1, %xmm0
	addsd	%xmm3, %xmm2
	addsd	%xmm2, %xmm0

	RESTOREREGISTERS

	ret

	EPILOGUE
