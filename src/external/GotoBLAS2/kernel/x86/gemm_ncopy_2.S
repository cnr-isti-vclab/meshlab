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

#define M	 4 + STACK + ARGS(%esp)
#define N	 8 + STACK + ARGS(%esp)
#define A	12 + STACK + ARGS(%esp)
#define LDA	16 + STACK + ARGS(%esp)
#define B	20 + STACK + ARGS(%esp)

	PROLOGUE

	subl	$ARGS, %esp
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx

	PROFCODE

	EMMS

	movl	B, %esi			# ESI : offsetB
	movl	M, %edi

	movl	A,   %ebx		# EBX : offsetA
	movl	LDA, %edx
	leal    (%ebx, %edx, SIZE), %ebp

	addl	%edx, %edx
	subl	%edi, %edx		# edx = 2 * lda - m

	movl	N,  %eax
	sarl	$1, %eax
	movl	%eax, J
	je	.L20
	ALIGN_3

.L21:
#if 0
	movl	%edi,  %ecx		# ECX : I(Counter of M)
	andl	$-8, %ecx
	leal	(%ebx, %ecx, SIZE), %ebx
	leal	(%ebp, %ecx, SIZE), %ebp
	negl	%ecx
	ALIGN_3

.Blocking1:
	MMXLOAD		(%ebx, %ecx, SIZE), %mm0
	MMXLOAD		(%ebp, %ecx, SIZE), %mm1
	addl	 $8,   %ecx
	jl	.Blocking1

	movl	%edi,  %ecx		# ECX : I(Counter of M)
	andl	$-8, %ecx
	negl	%ecx
	leal	(%ebx, %ecx, SIZE), %ebx
	leal	(%ebp, %ecx, SIZE), %ebp
#endif

	movl	%edi, %ecx		# ECX : I(Counter of M)
	sarl	$2,   %ecx
	je	.L24
	ALIGN_3

.L25:
#ifdef HAVE_MMX
	MMXLOAD		0 * SIZE(%ebx), %mm0
	MMXLOAD		0 * SIZE(%ebp), %mm1
	MMXLOAD		1 * SIZE(%ebx), %mm2
	MMXLOAD		1 * SIZE(%ebp), %mm3

	MMXLOAD		2 * SIZE(%ebx), %mm4
	MMXLOAD		2 * SIZE(%ebp), %mm5
	MMXLOAD		3 * SIZE(%ebx), %mm6
	MMXLOAD		3 * SIZE(%ebp), %mm7

	MMXSTORE	%mm0, 0 * SIZE(%esi)
	MMXSTORE	%mm1, 1 * SIZE(%esi)
	MMXSTORE	%mm2, 2 * SIZE(%esi)
	MMXSTORE	%mm3, 3 * SIZE(%esi)

	MMXSTORE	%mm4, 4 * SIZE(%esi)
	MMXSTORE	%mm5, 5 * SIZE(%esi)
	MMXSTORE	%mm6, 6 * SIZE(%esi)
	MMXSTORE	%mm7, 7 * SIZE(%esi)
#else
	FLD	3 * SIZE(%ebp)
	FLD	3 * SIZE(%ebx)
	FLD	2 * SIZE(%ebp)
	FLD	2 * SIZE(%ebx)
	FLD	1 * SIZE(%ebp)
	FLD	1 * SIZE(%ebx)
	FLD	0 * SIZE(%ebp)
	FLD	0 * SIZE(%ebx)

	FST	0 * SIZE(%esi)
	FST	1 * SIZE(%esi)
	FST	2 * SIZE(%esi)
	FST	3 * SIZE(%esi)
	FST	4 * SIZE(%esi)
	FST	5 * SIZE(%esi)
	FST	6 * SIZE(%esi)
	FST	7 * SIZE(%esi)
#endif
	addl	$4 * SIZE, %ebx
	addl	$4 * SIZE, %ebp
	addl	$8 * SIZE, %esi
	decl	%ecx
	jne	.L25
	ALIGN_3

.L24:
	movl	%edi, %ecx
	andl	$3,   %ecx
	jle	.L30
	ALIGN_3

.L31:
#ifdef HAVE_MMX
	MMXLOAD		0 * SIZE(%ebx), %mm0
	MMXLOAD		0 * SIZE(%ebp), %mm1
	MMXSTORE	%mm0, 0 * SIZE(%esi)
	MMXSTORE	%mm1, 1 * SIZE(%esi)
#else
	FLD	0 * SIZE(%ebp)
	FLD	0 * SIZE(%ebx)
	FST	0 * SIZE(%esi)
	FST	1 * SIZE(%esi)
#endif
	addl	$1 * SIZE, %ebx
	addl	$1 * SIZE, %ebp
	addl	$2 * SIZE, %esi
	decl	%ecx
	jne	.L31
	ALIGN_3

.L30:
	leal	(%ebx, %edx, SIZE), %ebx
	leal	(%ebp, %edx, SIZE), %ebp
	decl	J
	jne	.L21
	ALIGN_3

.L20:
	movl	N, %eax
	andl	$1,%eax
	jle	.L38
	ALIGN_3

.L39:
	movl	%edi, %ecx
	sarl	$3,   %ecx
	je	.L42
	ALIGN_3

.L43:
#ifdef HAVE_MMX
	MMXLOAD		0 * SIZE(%ebx), %mm0
	MMXLOAD		1 * SIZE(%ebx), %mm1
	MMXLOAD		2 * SIZE(%ebx), %mm2
	MMXLOAD		3 * SIZE(%ebx), %mm3
	MMXLOAD		4 * SIZE(%ebx), %mm4
	MMXLOAD		5 * SIZE(%ebx), %mm5
	MMXLOAD		6 * SIZE(%ebx), %mm6
	MMXLOAD		7 * SIZE(%ebx), %mm7

	MMXSTORE	%mm0,  0 * SIZE(%esi)
	MMXSTORE	%mm1,  1 * SIZE(%esi)
	MMXSTORE	%mm2,  2 * SIZE(%esi)
	MMXSTORE	%mm3,  3 * SIZE(%esi)
	MMXSTORE	%mm4,  4 * SIZE(%esi)
	MMXSTORE	%mm5,  5 * SIZE(%esi)
	MMXSTORE	%mm6,  6 * SIZE(%esi)
	MMXSTORE	%mm7,  7 * SIZE(%esi)
#else
	FLD	 7 * SIZE(%ebx)
	FLD	 6 * SIZE(%ebx)
	FLD	 5 * SIZE(%ebx)
	FLD	 4 * SIZE(%ebx)
	FLD	 3 * SIZE(%ebx)
	FLD	 2 * SIZE(%ebx)
	FLD	 1 * SIZE(%ebx)
	FLD	 0 * SIZE(%ebx)

	FST	 0 * SIZE(%esi)
	FST	 1 * SIZE(%esi)
	FST	 2 * SIZE(%esi)
	FST	 3 * SIZE(%esi)
	FST	 4 * SIZE(%esi)
	FST	 5 * SIZE(%esi)
	FST	 6 * SIZE(%esi)
	FST	 7 * SIZE(%esi)
#endif

	addl	$8 * SIZE, %ebx
	addl	$8 * SIZE, %esi
	decl	%ecx
	jne	.L43
	ALIGN_3

.L42:
	movl	%edi, %ecx
	andl	$7,   %ecx
	jle	.L38
	ALIGN_3

.L49:
#ifdef HAVE_MMX
	MMXLOAD		0 * SIZE(%ebx), %mm0
	MMXSTORE	%mm0,  0 * SIZE(%esi)
#else
	FLD	0 * SIZE(%ebx)
	FST	0 * SIZE(%esi)
#endif
	addl	$1 * SIZE, %ebx
	addl	$1 * SIZE, %esi
	decl	%ecx
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
