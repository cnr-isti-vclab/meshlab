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

#ifndef WINDOWS_ABI

#define M	ARG1
#define N	ARG2
#define C	ARG3
#define LDC	ARG4
#define C1	ARG5

#define STACK_C	  16(%rsp)
#define STACK_LDC 24(%rsp)

#else

#define STACKSIZE 256

#define M	ARG1
#define N	ARG2
#define C	ARG3
#define LDC	ARG4
#define C1	%r10

#define STACK_C		72 + STACKSIZE(%rsp)
#define STACK_LDC	80 + STACKSIZE(%rsp)

#endif

#define I	%rax

	PROLOGUE
	PROFCODE

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

	movaps	%xmm3, %xmm0
#endif

	movq	STACK_C,   C
	movq	STACK_LDC, LDC

	pxor	%xmm1, %xmm1

	test	M, M
	jle	.L999
	test	N, N
	jle	.L999

#ifdef DOUBLE
	ucomisd	%xmm1, %xmm0
#else
	ucomiss	%xmm1, %xmm0
#endif
	jne	.L201
	ALIGN_4

.L101:
	movq	 C, C1
	leaq	(C, LDC, SIZE), C

	movq	M,  I
	sarq	$3, I
	jle	.L103
	ALIGN_4

.L102:
#ifdef OPTERON
	prefetchw	32 * SIZE(C1)
#endif
	
	MOVSD	%xmm0, 0 * SIZE(C1)
	MOVSD	%xmm0, 1 * SIZE(C1)
	MOVSD	%xmm0, 2 * SIZE(C1)
	MOVSD	%xmm0, 3 * SIZE(C1)
	MOVSD	%xmm0, 4 * SIZE(C1)
	MOVSD	%xmm0, 5 * SIZE(C1)
	MOVSD	%xmm0, 6 * SIZE(C1)
	MOVSD	%xmm0, 7 * SIZE(C1)
	addq	$8 * SIZE, C1
	decq	I
	jg	.L102
	ALIGN_4

.L103:
	movq	M,  I
	andq	$7, I
	jle	.L105
	ALIGN_4

.L104:
	MOVSD	%xmm0, 0 * SIZE(C1)
	addq	$SIZE, C1
	decq	I
	jg	.L104
	ALIGN_4

.L105:
	decq	N
	jg	.L101
	jmp	.L999
	ALIGN_3

.L201:
	movq	 C, C1			# c_offset = c
	leaq	(C, LDC, SIZE), C	# c += ldc
	movq	M, I
	sarq	$3,   I
	jle	.L203
	ALIGN_4

.L202:
#ifdef OPTERON
	prefetchw	32 * SIZE(C1)
#endif

	MOVSD	0 * SIZE(C1), %xmm8
	MOVSD	1 * SIZE(C1), %xmm9
	MOVSD	2 * SIZE(C1), %xmm10
	MOVSD	3 * SIZE(C1), %xmm11
	MOVSD	4 * SIZE(C1), %xmm12
	MOVSD	5 * SIZE(C1), %xmm13
	MOVSD	6 * SIZE(C1), %xmm14
	MOVSD	7 * SIZE(C1), %xmm15

	MULSD	%xmm0, %xmm8
	MULSD	%xmm0, %xmm9
	MULSD	%xmm0, %xmm10
	MULSD	%xmm0, %xmm11
	MULSD	%xmm0, %xmm12
	MULSD	%xmm0, %xmm13
	MULSD	%xmm0, %xmm14
	MULSD	%xmm0, %xmm15

	MOVSD	%xmm8,  0 * SIZE(C1)
	MOVSD	%xmm9,  1 * SIZE(C1)
	MOVSD	%xmm10, 2 * SIZE(C1)
	MOVSD	%xmm11, 3 * SIZE(C1)
	MOVSD	%xmm12, 4 * SIZE(C1)
	MOVSD	%xmm13, 5 * SIZE(C1)
	MOVSD	%xmm14, 6 * SIZE(C1)
	MOVSD	%xmm15, 7 * SIZE(C1)

	addq	$8 * SIZE, C1
	decq	I
	jg	.L202
	ALIGN_4

.L203:
	movq	M, I
	andq	$7,   I
	jle	.L205
	ALIGN_4

.L204:
	MOVSD	0 * SIZE(C1), %xmm8
	MULSD	%xmm0, %xmm8
	MOVSD	%xmm8,  0 * SIZE(C1)
	addq	$SIZE, C1
	decq	I
	jg	.L204
	ALIGN_4

.L205:
	decq	N
	jg	.L201
	ALIGN_3

.L999:
	xorq	%rax, %rax

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

	ret

	EPILOGUE
