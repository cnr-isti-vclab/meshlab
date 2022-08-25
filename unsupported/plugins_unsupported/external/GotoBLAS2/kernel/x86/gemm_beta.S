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
#define ARGS	 0
	
#define M	 4 + STACK + ARGS(%esp)
#define N	 8 + STACK + ARGS(%esp)
#ifdef DOUBLE
#define BETA	16 + STACK + ARGS(%esp)
#define C	40 + STACK + ARGS(%esp)
#define LDC	44 + STACK + ARGS(%esp)
#else
#define BETA	16 + STACK + ARGS(%esp)
#define C	36 + STACK + ARGS(%esp)
#define LDC	40 + STACK + ARGS(%esp)
#endif

	PROLOGUE

	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx

	PROFCODE

	movl	M, %esi			# m
	movl	N, %ecx			# n
	FLD	BETA			# beta

	movl	C,   %edi		# C
	movl	LDC, %ebp		# ldc

	testl	%esi, %esi		# if n <= 0 goto End
	jle	.L999
	testl	%ecx, %ecx		# if m <= 0 goto End
	jle	.L999

	ftst
	fnstsw	%ax
	andb	$68, %ah
	je	.L201
	ALIGN_4

.L101:
	movl	 %edi, %eax			# c_offset = c
	leal	(%edi, %ebp, SIZE), %edi	# c += ldc
	movl	%esi, %edx
	sarl	$3,   %edx
	jle	.L103
	ALIGN_4

.L102:
#ifdef HAS_PREFETCH
#ifndef ATHLON
	prefetchnta	12 * SIZE(%eax)
	prefetchnta	16 * SIZE(%eax)
#else
	prefetchw	32 * SIZE(%eax)
#endif
#endif

	FSTU	0 * SIZE(%eax)
	FSTU	1 * SIZE(%eax)
	FSTU	2 * SIZE(%eax)
	FSTU	3 * SIZE(%eax)
	FSTU	4 * SIZE(%eax)
	FSTU	5 * SIZE(%eax)
	FSTU	6 * SIZE(%eax)
	FSTU	7 * SIZE(%eax)
	addl	$8 * SIZE, %eax
	decl	%edx
	jg	.L102
	ALIGN_4

.L103:
	movl	%esi, %edx
	andl	$7,   %edx
	jle	.L105
	ALIGN_4

.L104:
	FSTU	0 * SIZE(%eax)
	addl	$SIZE, %eax
	decl	%edx
	jg	.L104
	ALIGN_4

.L105:
	decl	%ecx
	jg	.L101
	jmp	.L999
	ALIGN_3


.L201:
	movl	 %edi, %eax			# c_offset = c
	leal	(%edi, %ebp, SIZE), %edi	# c += ldc
	movl	%esi, %edx
	sarl	$3,   %edx
	jle	.L203
	ALIGN_4

.L202:
#ifdef HAS_PREFETCH
#ifndef ATHLON
	prefetchnta	16 * SIZE(%eax)
	prefetchnta	20 * SIZE(%eax)
#else
	prefetchw	32 * SIZE(%eax)
#endif
#endif

	FLD	0 * SIZE(%eax)
	fmul	%st(1),%st
	FST	0 * SIZE(%eax)

	FLD	1 * SIZE(%eax)
	fmul	%st(1),%st
	FST	1 * SIZE(%eax)

	FLD	2 * SIZE(%eax)
	fmul	%st(1),%st
	FST	2 * SIZE(%eax)

	FLD	3 * SIZE(%eax)
	fmul	%st(1),%st
	FST	3 * SIZE(%eax)

	FLD	4 * SIZE(%eax)
	fmul	%st(1),%st
	FST	4 * SIZE(%eax)
	
	FLD	5 * SIZE(%eax)
	fmul	%st(1),%st
	FST	5 * SIZE(%eax)

	FLD	6 * SIZE(%eax)
	fmul	%st(1),%st
	FST	6 * SIZE(%eax)

	FLD	7 * SIZE(%eax)
	fmul	%st(1),%st
	FST	7 * SIZE(%eax)

	addl	$8 * SIZE, %eax
	decl	%edx
	jg	.L202
	ALIGN_4

.L203:
	movl	%esi, %edx
	andl	$7,   %edx
	jle	.L205
	ALIGN_4

.L204:
	FLD	0 * SIZE(%eax)
	fmul	%st(1), %st
	FST	0 * SIZE(%eax)
	addl	$SIZE, %eax
	decl	%edx
	jg	.L204
	ALIGN_4

.L205:
	decl	%ecx
	jg	.L201
	ALIGN_3

.L999:
#ifndef C_SUN
	ffreep	%st(0)
#else
	.byte	0xdf
	.byte	0xc0
#endif
	xorl	%eax,%eax
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret

	EPILOGUE
