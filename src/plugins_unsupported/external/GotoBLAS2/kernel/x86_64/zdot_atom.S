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

	salq	$ZBASE_SHIFT, INCX
	pxor	%xmm0, %xmm0
	salq	$ZBASE_SHIFT, INCY
	pxor	%xmm1, %xmm1

	pxor	%xmm2, %xmm2
	cmpq	$0, N
	pxor	%xmm3, %xmm3
	jle	.L999

	cmpq	$2 * SIZE, INCX
	jne	.L20
	cmpq	$2 * SIZE, INCY
	jne	.L20

	movq	N,  %rax
	sarq	$2, %rax
	jle	.L15

	movsd	 0 * SIZE(X), %xmm4
	movsd	 0 * SIZE(Y), %xmm6
	movsd	 1 * SIZE(X), %xmm5
	movsd	 1 * SIZE(Y), %xmm7

	movaps	 %xmm4, %xmm8
	mulsd	 %xmm6, %xmm4
	movsd	 2 * SIZE(X), %xmm10
	mulsd	 %xmm7, %xmm8
	movsd	 2 * SIZE(Y), %xmm11
	movaps	 %xmm5, %xmm9
	mulsd	 %xmm7, %xmm5
	movsd	 3 * SIZE(X), %xmm12
	mulsd	 %xmm6, %xmm9
	movsd	 3 * SIZE(Y), %xmm13

	decq	%rax
	jle	.L12
	ALIGN_3

.L11:
#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	addsd	 %xmm4, %xmm0
	movaps	 %xmm10, %xmm14
	mulsd	 %xmm11, %xmm10
	movsd	 4 * SIZE(X), %xmm4
	addsd	 %xmm8, %xmm1
	mulsd	 %xmm13, %xmm14
	movsd	 4 * SIZE(Y), %xmm6
	addsd	 %xmm5, %xmm2
	movaps	 %xmm12, %xmm15
	mulsd	 %xmm13, %xmm12
	movsd	 5 * SIZE(X), %xmm5
	addsd	 %xmm9, %xmm3
	mulsd	 %xmm11, %xmm15
	movsd	 5 * SIZE(Y), %xmm7

	addsd	 %xmm10, %xmm0
	movaps	 %xmm4, %xmm8
	mulsd	 %xmm6, %xmm4
	movsd	 6 * SIZE(X), %xmm10
	addsd	 %xmm14, %xmm1
	mulsd	 %xmm7, %xmm8
	movsd	 6 * SIZE(Y), %xmm11
	addsd	 %xmm12, %xmm2
	movaps	 %xmm5, %xmm9
	mulsd	 %xmm7, %xmm5
	movsd	 7 * SIZE(X), %xmm12
	addsd	 %xmm15, %xmm3
	mulsd	 %xmm6, %xmm9
	movsd	 7 * SIZE(Y), %xmm13

#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(Y)
#endif

	addsd	 %xmm4, %xmm0
	movaps	 %xmm10, %xmm14
	mulsd	 %xmm11, %xmm10
	movsd	 8 * SIZE(X), %xmm4
	addsd	 %xmm8, %xmm1
	mulsd	 %xmm13, %xmm14
	movsd	 8 * SIZE(Y), %xmm6
	addsd	 %xmm5, %xmm2
	movaps	 %xmm12, %xmm15
	mulsd	 %xmm13, %xmm12
	movsd	 9 * SIZE(X), %xmm5
	addsd	 %xmm9, %xmm3
	mulsd	 %xmm11, %xmm15
	movsd	 9 * SIZE(Y), %xmm7

	addsd	 %xmm10, %xmm0
	movaps	 %xmm4, %xmm8
	mulsd	 %xmm6, %xmm4
	movsd	10 * SIZE(X), %xmm10
	addsd	 %xmm14, %xmm1
	mulsd	 %xmm7, %xmm8
	movsd	10 * SIZE(Y), %xmm11
	addsd	 %xmm12, %xmm2
	movaps	 %xmm5, %xmm9
	mulsd	 %xmm7, %xmm5
	movsd	11 * SIZE(X), %xmm12
	addsd	 %xmm15, %xmm3
	mulsd	 %xmm6, %xmm9
	movsd	11 * SIZE(Y), %xmm13

	addq	$8 * SIZE, X
	addq	$8 * SIZE, Y

	decq	%rax
	jg	.L11
	ALIGN_3

.L12:
	addsd	 %xmm4, %xmm0
	movaps	 %xmm10, %xmm14
	mulsd	 %xmm11, %xmm10
	movsd	 4 * SIZE(X), %xmm4
	addsd	 %xmm8, %xmm1
	mulsd	 %xmm13, %xmm14
	movsd	 4 * SIZE(Y), %xmm6
	addsd	 %xmm5, %xmm2
	movaps	 %xmm12, %xmm15
	mulsd	 %xmm13, %xmm12
	movsd	 5 * SIZE(X), %xmm5
	addsd	 %xmm9, %xmm3
	mulsd	 %xmm11, %xmm15
	movsd	 5 * SIZE(Y), %xmm7

	addsd	 %xmm10, %xmm0
	movaps	 %xmm4, %xmm8
	mulsd	 %xmm6, %xmm4
	movsd	 6 * SIZE(X), %xmm10
	addsd	 %xmm14, %xmm1
	mulsd	 %xmm7, %xmm8
	movsd	 6 * SIZE(Y), %xmm11
	addsd	 %xmm12, %xmm2
	movaps	 %xmm5, %xmm9
	mulsd	 %xmm7, %xmm5
	movsd	 7 * SIZE(X), %xmm12
	addsd	 %xmm15, %xmm3
	mulsd	 %xmm6, %xmm9
	movsd	 7 * SIZE(Y), %xmm13

	addsd	 %xmm4, %xmm0
	movaps	 %xmm10, %xmm14
	mulsd	 %xmm11, %xmm10
	addsd	 %xmm8, %xmm1
	mulsd	 %xmm13, %xmm14
	addsd	 %xmm5, %xmm2
	movaps	 %xmm12, %xmm15
	mulsd	 %xmm13, %xmm12
	addsd	 %xmm9, %xmm3
	mulsd	 %xmm11, %xmm15

	addsd	 %xmm10, %xmm0
	addsd	 %xmm14, %xmm1
	addsd	 %xmm12, %xmm2
	addsd	 %xmm15, %xmm3

	addq	$8 * SIZE, X
	addq	$8 * SIZE, Y
	ALIGN_3

.L15:
	movq	N,  %rax
	andq	$2, %rax
	jle	.L17

	movsd	 0 * SIZE(X), %xmm4
	movsd	 0 * SIZE(Y), %xmm6
	movsd	 1 * SIZE(X), %xmm5
	movsd	 1 * SIZE(Y), %xmm7

	movaps	 %xmm4, %xmm8
	mulsd	 %xmm6, %xmm4
	movsd	 2 * SIZE(X), %xmm10
	mulsd	 %xmm7, %xmm8
	movsd	 2 * SIZE(Y), %xmm11
	movaps	 %xmm5, %xmm9
	mulsd	 %xmm7, %xmm5
	movsd	 3 * SIZE(X), %xmm12
	mulsd	 %xmm6, %xmm9
	movsd	 3 * SIZE(Y), %xmm13

	addsd	 %xmm4, %xmm0
	movaps	 %xmm10, %xmm14
	mulsd	 %xmm11, %xmm10
	addsd	 %xmm8, %xmm1
	mulsd	 %xmm13, %xmm14
	addsd	 %xmm5, %xmm2
	movaps	 %xmm12, %xmm15
	mulsd	 %xmm13, %xmm12
	addsd	 %xmm9, %xmm3
	mulsd	 %xmm11, %xmm15

	addsd	 %xmm10, %xmm0
	addsd	 %xmm14, %xmm1
	addsd	 %xmm12, %xmm2
	addsd	 %xmm15, %xmm3

	addq	$4 * SIZE, X
	addq	$4 * SIZE, Y
	ALIGN_3

.L17:
	movq	N,  %rax
	andq	$1, %rax
	jle	.L999

	movsd	 0 * SIZE(X), %xmm4
	movsd	 0 * SIZE(Y), %xmm6
	movsd	 1 * SIZE(X), %xmm5
	movsd	 1 * SIZE(Y), %xmm7

	movaps	 %xmm4, %xmm8
	mulsd	 %xmm6, %xmm4
	mulsd	 %xmm7, %xmm8
	movaps	 %xmm5, %xmm9
	mulsd	 %xmm7, %xmm5
	mulsd	 %xmm6, %xmm9

	addsd	 %xmm4, %xmm0
	addsd	 %xmm8, %xmm1
	addsd	 %xmm5, %xmm2
	addsd	 %xmm9, %xmm3
	jmp	 .L999
	ALIGN_3

.L20:
	movq	N,  %rax
	sarq	$2, %rax
	jle	.L25
	ALIGN_3

.L23:
	movsd	 0 * SIZE(X), %xmm4
	movsd	 0 * SIZE(Y), %xmm6
	movsd	 1 * SIZE(X), %xmm5
	movsd	 1 * SIZE(Y), %xmm7

	movaps	 %xmm4, %xmm8
	mulsd	 %xmm6, %xmm4
	mulsd	 %xmm7, %xmm8
	movaps	 %xmm5, %xmm9
	mulsd	 %xmm7, %xmm5
	addq	 INCX, X
	mulsd	 %xmm6, %xmm9
	addq	 INCY, Y

	addsd	 %xmm4, %xmm0
	movsd	 0 * SIZE(X), %xmm4
	addsd	 %xmm8, %xmm1
	movsd	 0 * SIZE(Y), %xmm6
	addsd	 %xmm5, %xmm2
	movsd	 1 * SIZE(X), %xmm5
	addsd	 %xmm9, %xmm3
	movsd	 1 * SIZE(Y), %xmm7

	movaps	 %xmm4, %xmm8
	mulsd	 %xmm6, %xmm4
	mulsd	 %xmm7, %xmm8
	movaps	 %xmm5, %xmm9
	mulsd	 %xmm7, %xmm5
	addq	 INCX, X
	mulsd	 %xmm6, %xmm9
	addq	 INCY, Y

	addsd	 %xmm4, %xmm0
	movsd	 0 * SIZE(X), %xmm4
	addsd	 %xmm8, %xmm1
	movsd	 0 * SIZE(Y), %xmm6
	addsd	 %xmm5, %xmm2
	movsd	 1 * SIZE(X), %xmm5
	addsd	 %xmm9, %xmm3
	movsd	 1 * SIZE(Y), %xmm7

	movaps	 %xmm4, %xmm8
	mulsd	 %xmm6, %xmm4
	mulsd	 %xmm7, %xmm8
	movaps	 %xmm5, %xmm9
	mulsd	 %xmm7, %xmm5
	addq	 INCX, X
	mulsd	 %xmm6, %xmm9
	addq	 INCY, Y

	addsd	 %xmm4, %xmm0
	movsd	 0 * SIZE(X), %xmm4
	addsd	 %xmm8, %xmm1
	movsd	 0 * SIZE(Y), %xmm6
	addsd	 %xmm5, %xmm2
	movsd	 1 * SIZE(X), %xmm5
	addsd	 %xmm9, %xmm3
	movsd	 1 * SIZE(Y), %xmm7

	movaps	 %xmm4, %xmm8
	mulsd	 %xmm6, %xmm4
	mulsd	 %xmm7, %xmm8
	movaps	 %xmm5, %xmm9
	mulsd	 %xmm7, %xmm5
	addq	 INCX, X
	mulsd	 %xmm6, %xmm9
	addq	 INCY, Y

	addsd	 %xmm4, %xmm0
	addsd	 %xmm8, %xmm1
	addsd	 %xmm5, %xmm2
	addsd	 %xmm9, %xmm3

	decq	%rax
	jg	.L23
	ALIGN_3

.L25:
	testq	$3, N
	je	.L999

	movq	N,  %rax
	andq	$2, %rax
	jle	.L27

	movsd	 0 * SIZE(X), %xmm4
	movsd	 0 * SIZE(Y), %xmm6
	movsd	 1 * SIZE(X), %xmm5
	movsd	 1 * SIZE(Y), %xmm7

	movaps	 %xmm4, %xmm8
	mulsd	 %xmm6, %xmm4
	mulsd	 %xmm7, %xmm8
	movaps	 %xmm5, %xmm9
	mulsd	 %xmm7, %xmm5
	addq	 INCX, X
	mulsd	 %xmm6, %xmm9
	addq	 INCY, Y

	addsd	 %xmm4, %xmm0
	movsd	 0 * SIZE(X), %xmm4
	addsd	 %xmm8, %xmm1
	movsd	 0 * SIZE(Y), %xmm6
	addsd	 %xmm5, %xmm2
	movsd	 1 * SIZE(X), %xmm5
	addsd	 %xmm9, %xmm3
	movsd	 1 * SIZE(Y), %xmm7

	movaps	 %xmm4, %xmm8
	mulsd	 %xmm6, %xmm4
	mulsd	 %xmm7, %xmm8
	movaps	 %xmm5, %xmm9
	mulsd	 %xmm7, %xmm5
	addq	 INCX, X
	mulsd	 %xmm6, %xmm9
	addq	 INCY, Y

	addsd	 %xmm4, %xmm0
	addsd	 %xmm8, %xmm1
	addsd	 %xmm5, %xmm2
	addsd	 %xmm9, %xmm3

	ALIGN_3

.L27:
	movq	N,  %rax
	andq	$1, %rax
	jle	.L999

	movsd	 0 * SIZE(X), %xmm4
	movsd	 0 * SIZE(Y), %xmm6
	movsd	 1 * SIZE(X), %xmm5
	movsd	 1 * SIZE(Y), %xmm7

	movaps	 %xmm4, %xmm8
	mulsd	 %xmm6, %xmm4
	mulsd	 %xmm7, %xmm8
	movaps	 %xmm5, %xmm9
	mulsd	 %xmm7, %xmm5
	mulsd	 %xmm6, %xmm9

	addsd	 %xmm4, %xmm0
	addsd	 %xmm8, %xmm1
	addsd	 %xmm5, %xmm2
	addsd	 %xmm9, %xmm3
	ALIGN_3

.L999:
#ifndef CONJ
	subsd	 %xmm2, %xmm0
	addsd	 %xmm3, %xmm1
#else
	addsd	 %xmm2, %xmm0
	subsd	 %xmm3, %xmm1
#endif

	RESTOREREGISTERS

	ret

	EPILOGUE
