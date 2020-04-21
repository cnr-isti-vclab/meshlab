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

#define STACK_ALPHA_I	40 + STACKSIZE(%rsp)
#define STACK_C		80 + STACKSIZE(%rsp)
#define STACK_LDC	88 + STACKSIZE(%rsp)

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
	movsd	STACK_ALPHA_I, %xmm1
#endif

	pxor	%xmm15, %xmm15

	movq	STACK_C,   C
	movq	STACK_LDC, LDC

	testq	M, M
	jle	.L999
	testq	N, N
	jle	.L999

	salq	$ZBASE_SHIFT, LDC

#ifdef DOUBLE
	ucomisd	%xmm15, %xmm0
	jne	.L71
	ucomisd	%xmm15, %xmm1
	jne	.L71
#else
	ucomiss	%xmm15, %xmm0
	jne	.L71
	ucomiss	%xmm15, %xmm1
	jne	.L71
#endif
	ALIGN_2

.L53:
	movq	C, C1		# c_offset1 = c_offset
	addq	LDC, C		# c_offset += ldc

	movq	M,  I
	sarq	$2, I
	jle	.L56
	ALIGN_2

.L57:
#ifdef OPTERON
	prefetchw	64 * SIZE(C1)
#endif

	MOVSD	%xmm0, 0 * SIZE(C1)		# c_offset1
	MOVSD	%xmm0, 1 * SIZE(C1)
	MOVSD	%xmm0, 2 * SIZE(C1)
	MOVSD	%xmm0, 3 * SIZE(C1)
	MOVSD	%xmm0, 4 * SIZE(C1)
	MOVSD	%xmm0, 5 * SIZE(C1)
	MOVSD	%xmm0, 6 * SIZE(C1)
	MOVSD	%xmm0, 7 * SIZE(C1)
	addq	$8 * SIZE, C1		# c_offset1 += 8
	decq	I			# i--
	jg	.L57
	ALIGN_2

.L56:
	movq	M,  I
	andq	$3, I
	jle	.L62
	ALIGN_2

.L63:
	MOVSD	%xmm0, 0 * SIZE(C1)
	MOVSD	%xmm0, 1 * SIZE(C1)
	addq	$2 * SIZE,C1
	decq	I
	jg	.L63
	ALIGN_2

.L62:
	decq	N			# j --
	jg	.L53
	jmp	.L999
	ALIGN_3

.L71:
	movq	C, C1
	addq	LDC, C		# c_offset += ldc

	movq	M,  I
	sarq	$1, I
	jle	.L84
	ALIGN_3

.L85:
#ifdef OPTERON
	prefetchw	16 * SIZE(C1)
#endif

	MOVSD	0 * SIZE(C1), %xmm2
	MOVSD	1 * SIZE(C1), %xmm3
	MOVSD	0 * SIZE(C1), %xmm4
	MOVSD	1 * SIZE(C1), %xmm5

	MOVSD	2 * SIZE(C1), %xmm6
	MOVSD	3 * SIZE(C1), %xmm7
	MOVSD	2 * SIZE(C1), %xmm8
	MOVSD	3 * SIZE(C1), %xmm9

	MULSD	%xmm0, %xmm2
	MULSD	%xmm1, %xmm3
	MULSD	%xmm1, %xmm4
	MULSD	%xmm0, %xmm5

	MULSD	%xmm0, %xmm6
	MULSD	%xmm1, %xmm7
	MULSD	%xmm1, %xmm8
	MULSD	%xmm0, %xmm9

	SUBSD	%xmm3, %xmm2
	ADDPD	%xmm5, %xmm4
	SUBSD	%xmm7, %xmm6
	ADDPD	%xmm9, %xmm8

	MOVSD	%xmm2, 0 * SIZE(C1)
	MOVSD	%xmm4, 1 * SIZE(C1)
	MOVSD	%xmm6, 2 * SIZE(C1)
	MOVSD	%xmm8, 3 * SIZE(C1)
	addq	$4 * SIZE, C1
	decq	I
	jg	.L85
	ALIGN_3

.L84:
	testq	$1, M
	jle	.L74
	ALIGN_3

.L75:
	prefetchnta	80 * SIZE(C1)

	MOVSD	0 * SIZE(C1), %xmm2
	MULSD	%xmm0, %xmm2
	MOVSD	1 * SIZE(C1), %xmm3
	MULSD	%xmm1, %xmm3
	MOVSD	0 * SIZE(C1), %xmm4
	MULSD	%xmm1, %xmm4
	MOVSD	1 * SIZE(C1), %xmm5
	MULSD	%xmm0, %xmm5

	SUBSD	%xmm3, %xmm2
	ADDPD	%xmm5, %xmm4

	MOVSD	%xmm2, 0 * SIZE(C1)
	MOVSD	%xmm4, 1 * SIZE(C1)
	ALIGN_2

.L74:
	decq	N
	jg	.L71
	ALIGN_2

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

	ret

	EPILOGUE
