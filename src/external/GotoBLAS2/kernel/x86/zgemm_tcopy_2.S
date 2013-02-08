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

#define STACK	16
#define ARGS	 8
	
#define J	 0 + STACK(%esp)
#define BOFFSET2 4 + STACK(%esp)

#define STACK_M	 4 + STACK + ARGS(%esp)
#define STACK_N	 8 + STACK + ARGS(%esp)
#define STACK_A	12 + STACK + ARGS(%esp)
#define STACK_LDA	16 + STACK + ARGS(%esp)
#define STACK_B	20 + STACK + ARGS(%esp)

	PROLOGUE

	subl	$ARGS, %esp
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx

	PROFCODE
	
#define A	%ebp
#define A1	%edx
#define LDA	%ecx
#define B	%edi
#define I	%ebx
#define B1	%eax
#define M4	%esi

	EMMS

	movl	STACK_A, A
	movl	STACK_B, B

	movl	STACK_M,   %ebx
	movl	STACK_N,   %eax
	movl	STACK_LDA, LDA
	sall	$ZBASE_SHIFT, LDA

	andl	$-2,  %eax
	addl	%eax, %eax
	imull	%ebx, %eax		# m * ( n & ~1)
	leal	(B, %eax, SIZE), %eax	# boffset2 = b + m * (n & ~1)
	movl	%eax, BOFFSET2

	movl	STACK_M, M4
	sall	$ZBASE_SHIFT + 1, M4

	testl	%ebx,  %ebx		# if !(m & 1) goto L28
	movl	%ebx, J
	jle	.L999
	ALIGN_4

.L39:
	movl	A, A1
	addl	LDA, A
	movl	B, B1
	addl	$4 * SIZE, B

	movl	STACK_N,   I
	sarl	$1, I
	jle	.L32
	ALIGN_4

.L36:
#ifdef HAVE_MMX
	MMXLOAD		0 * SIZE(A1), %mm0
	MMXLOAD		1 * SIZE(A1), %mm1
	MMXLOAD		2 * SIZE(A1), %mm2
	MMXLOAD		3 * SIZE(A1), %mm3

	MMXSTORE	%mm0, 0 * SIZE(B1)
	MMXSTORE	%mm1, 1 * SIZE(B1)
	MMXSTORE	%mm2, 2 * SIZE(B1)
	MMXSTORE	%mm3, 3 * SIZE(B1)
#else
	FLD	3 * SIZE(A1)
	FLD	2 * SIZE(A1)
	FLD	1 * SIZE(A1)
	FLD	0 * SIZE(A1)

	FST	0 * SIZE(B1)
	FST	1 * SIZE(B1)
	FST	2 * SIZE(B1)
	FST	3 * SIZE(B1)
#endif
	addl	$4 * SIZE, A1
	addl	M4, B1
	decl	I
	jne	.L36
	ALIGN_4

.L32:
	movl	STACK_N,   I
	andl	$1, I
	jle	.L99
	ALIGN_4

	movl	BOFFSET2, B1

#ifdef HAVE_MMX
	MMXLOAD		0 * SIZE(A1), %mm0
	MMXLOAD		1 * SIZE(A1), %mm1

	MMXSTORE	%mm0, 0 * SIZE(B1)
	MMXSTORE	%mm1, 1 * SIZE(B1)
#else
	FLD	1 * SIZE(A1)
	FLD	0 * SIZE(A1)

	FST	0 * SIZE(B1)
	FST	1 * SIZE(B1)
#endif
	addl	$2 * SIZE, BOFFSET2
	ALIGN_4

.L99:
	decl	J
	jne	.L39
	ALIGN_4

.L999:
	EMMS

	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	addl	$ARGS,%esp
	ret

	EPILOGUE
