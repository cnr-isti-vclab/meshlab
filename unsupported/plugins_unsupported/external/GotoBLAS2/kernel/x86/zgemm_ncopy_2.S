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
	
#define STACK_M	 4 + STACK + ARGS(%esp)
#define STACK_N	 8 + STACK + ARGS(%esp)
#define STACK_A	12 + STACK + ARGS(%esp)
#define STACK_LDA	16 + STACK + ARGS(%esp)
#define STACK_B	20 + STACK + ARGS(%esp)

#define I	%eax
#define J	%ecx
#define LDA	%edx
#define A	%edi
#define A1	%ebx
#define A2	%ebp
#define B	%esi

	PROLOGUE

	subl	$ARGS, %esp
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx

	PROFCODE

	EMMS

	movl	STACK_A,   A
	movl	STACK_LDA, LDA
	movl	STACK_B, B
	sall	$ZBASE_SHIFT, LDA

	movl	STACK_N,  J
	sarl	$1, J
	je	.L20
	ALIGN_3

.L21:
	movl	A, A1
	leal    (A1, LDA), A2
	leal	(A,  LDA, 2), A

	movl	STACK_M,   I
	sarl	$1,  I
	je	.L24
	ALIGN_3

.L25:
#ifdef HAVE_MMX
	MMXLOAD		0 * SIZE(A1), %mm0
	MMXLOAD		1 * SIZE(A1), %mm1
	MMXLOAD		0 * SIZE(A2), %mm2
	MMXLOAD		1 * SIZE(A2), %mm3

	MMXLOAD		2 * SIZE(A1), %mm4
	MMXLOAD		3 * SIZE(A1), %mm5
	MMXLOAD		2 * SIZE(A2), %mm6
	MMXLOAD		3 * SIZE(A2), %mm7

	MMXSTORE	%mm0, 0 * SIZE(B)
	MMXSTORE	%mm1, 1 * SIZE(B)
	MMXSTORE	%mm2, 2 * SIZE(B)
	MMXSTORE	%mm3, 3 * SIZE(B)

	MMXSTORE	%mm4, 4 * SIZE(B)
	MMXSTORE	%mm5, 5 * SIZE(B)
	MMXSTORE	%mm6, 6 * SIZE(B)
	MMXSTORE	%mm7, 7 * SIZE(B)
#else
	FLD	3 * SIZE(A2)
	FLD	2 * SIZE(A2)
	FLD	3 * SIZE(A1)
	FLD	2 * SIZE(A1)
	FLD	1 * SIZE(A2)
	FLD	0 * SIZE(A2)
	FLD	1 * SIZE(A1)
	FLD	0 * SIZE(A1)

	FST	0 * SIZE(B)
	FST	1 * SIZE(B)
	FST	2 * SIZE(B)
	FST	3 * SIZE(B)
	FST	4 * SIZE(B)
	FST	5 * SIZE(B)
	FST	6 * SIZE(B)
	FST	7 * SIZE(B)
#endif
	addl	$4 * SIZE, A1
	addl	$4 * SIZE, A2
	addl	$8 * SIZE, B
	decl	I
	jne	.L25
	ALIGN_3

.L24:
	movl	STACK_M,  I
	andl	$1, I
	jle	.L30
	ALIGN_3

.L31:
#ifdef HAVE_MMX
	MMXLOAD		0 * SIZE(A1), %mm0
	MMXLOAD		1 * SIZE(A1), %mm1
	MMXLOAD		0 * SIZE(A2), %mm2
	MMXLOAD		1 * SIZE(A2), %mm3
	MMXSTORE	%mm0, 0 * SIZE(B)
	MMXSTORE	%mm1, 1 * SIZE(B)
	MMXSTORE	%mm2, 2 * SIZE(B)
	MMXSTORE	%mm3, 3 * SIZE(B)
#else
	FLD	1 * SIZE(A2)
	FLD	0 * SIZE(A2)
	FLD	1 * SIZE(A1)
	FLD	0 * SIZE(A1)
	FST	0 * SIZE(B)
	FST	1 * SIZE(B)
	FST	2 * SIZE(B)
	FST	3 * SIZE(B)
#endif
	addl	$2 * SIZE, A1
	addl	$2 * SIZE, A2
	addl	$4 * SIZE, B
	decl	I
	jne	.L31
	ALIGN_3

.L30:
	decl	J
	jne	.L21
	ALIGN_3

.L20:
	movl	A, A1
	movl	STACK_N, J
	andl	$1, J
	jle	.L38
	ALIGN_3

.L39:
	movl	STACK_M,  I
	sarl	$2, I
	je	.L42
	ALIGN_3

.L43:
#ifdef HAVE_MMX
	MMXLOAD		0 * SIZE(A1), %mm0
	MMXLOAD		1 * SIZE(A1), %mm1
	MMXLOAD		2 * SIZE(A1), %mm2
	MMXLOAD		3 * SIZE(A1), %mm3
	MMXLOAD		4 * SIZE(A1), %mm4
	MMXLOAD		5 * SIZE(A1), %mm5
	MMXLOAD		6 * SIZE(A1), %mm6
	MMXLOAD		7 * SIZE(A1), %mm7

	MMXSTORE	%mm0,  0 * SIZE(B)
	MMXSTORE	%mm1,  1 * SIZE(B)
	MMXSTORE	%mm2,  2 * SIZE(B)
	MMXSTORE	%mm3,  3 * SIZE(B)
	MMXSTORE	%mm4,  4 * SIZE(B)
	MMXSTORE	%mm5,  5 * SIZE(B)
	MMXSTORE	%mm6,  6 * SIZE(B)
	MMXSTORE	%mm7,  7 * SIZE(B)
#else
	FLD	 7 * SIZE(A1)
	FLD	 6 * SIZE(A1)
	FLD	 5 * SIZE(A1)
	FLD	 4 * SIZE(A1)
	FLD	 3 * SIZE(A1)
	FLD	 2 * SIZE(A1)
	FLD	 1 * SIZE(A1)
	FLD	 0 * SIZE(A1)

	FST	 0 * SIZE(B)
	FST	 1 * SIZE(B)
	FST	 2 * SIZE(B)
	FST	 3 * SIZE(B)
	FST	 4 * SIZE(B)
	FST	 5 * SIZE(B)
	FST	 6 * SIZE(B)
	FST	 7 * SIZE(B)
#endif

	addl	$8 * SIZE, A1
	addl	$8 * SIZE, B
	decl	I
	jne	.L43
	ALIGN_3

.L42:
	movl	STACK_M,  I
	andl	$3, I
	jle	.L38
	ALIGN_3

.L49:
#ifdef HAVE_MMX
	MMXLOAD		0 * SIZE(A1), %mm0
	MMXLOAD		1 * SIZE(A1), %mm1
	MMXSTORE	%mm0,  0 * SIZE(B)
	MMXSTORE	%mm1,  1 * SIZE(B)
#else
	FLD	1 * SIZE(A1)
	FLD	0 * SIZE(A1)
	FST	0 * SIZE(B)
	FST	1 * SIZE(B)
#endif
	addl	$2 * SIZE, A1
	addl	$2 * SIZE, B
	decl	I
	jne	.L49
	ALIGN_3

.L38:
	EMMS

	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	addl	$ARGS, %esp
	ret

	EPILOGUE
