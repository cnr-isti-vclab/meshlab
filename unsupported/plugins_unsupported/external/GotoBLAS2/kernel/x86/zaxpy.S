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

#define STACK	12
#define ARGS	 0
	
#define STACK_M		 4 + STACK + ARGS(%esp)
#ifdef DOUBLE
#define STACK_ALPHA_R	16 + STACK + ARGS(%esp)
#define STACK_ALPHA_I	24 + STACK + ARGS(%esp)
#define STACK_X		32 + STACK + ARGS(%esp)
#define STACK_INCX	36 + STACK + ARGS(%esp)
#define STACK_Y		40 + STACK + ARGS(%esp)
#define STACK_INCY	44 + STACK + ARGS(%esp)
#else
#define STACK_ALPHA_R	16 + STACK + ARGS(%esp)
#define STACK_ALPHA_I	20 + STACK + ARGS(%esp)
#define STACK_X		24 + STACK + ARGS(%esp)
#define STACK_INCX	28 + STACK + ARGS(%esp)
#define STACK_Y		32 + STACK + ARGS(%esp)
#define STACK_INCY	36 + STACK + ARGS(%esp)
#endif

#define M	%ebx
#define X	%esi
#define INCX	%ecx
#define Y	%edi
#define INCY	%edx

#ifndef CONJ
#define ADD1	fsubrp
#define ADD2	faddp
#else
#define ADD1	faddp
#define ADD2	fsubrp
#endif

	PROLOGUE

	pushl	%edi
	pushl	%esi
	pushl	%ebx

	PROFCODE

#if defined(F_INTERFACE_GFORT) || defined(F_INTERFACE_G95)
	EMMS
#endif

	FLD	STACK_ALPHA_I
	FLD	STACK_ALPHA_R

	movl	STACK_M,    M
	movl	STACK_X,    X
	movl	STACK_INCX, INCX
	movl	STACK_Y,    Y
	movl	STACK_INCY, INCY

	addl	INCX, INCX
	addl	INCY, INCY

	leal	(, INCX, SIZE), INCX
	leal	(, INCY, SIZE), INCY

	testl	M, M
	jle	.L40

	cmpl	$2 * SIZE, INCX
	jne	.L14
	cmpl	$2 * SIZE, INCY
	jne	.L14

	movl	M,  %eax
	sarl	$2, %eax
	jle	.L15
	ALIGN_3

.L16:
	FLD	0 * SIZE(X)
	fmul	%st(1), %st
	FLD	1 * SIZE(X)
	fmul	%st(3), %st
	ADD1	%st, %st(1)
	FADD	0 * SIZE(Y)
	FST	0 * SIZE(Y)

	FLD	0 * SIZE(X)
	fmul	%st(2), %st
	FLD	1 * SIZE(X)
	fmul	%st(2), %st
	ADD2	%st, %st(1)
	FADD	1 * SIZE(Y)
	FST	1 * SIZE(Y)

	FLD	2 * SIZE(X)
	fmul	%st(1), %st
	FLD	3 * SIZE(X)
	fmul	%st(3), %st
	ADD1	%st, %st(1)
	FADD	2 * SIZE(Y)
	FST	2 * SIZE(Y)

	FLD	2 * SIZE(X)
	fmul	%st(2), %st
	FLD	3 * SIZE(X)
	fmul	%st(2), %st
	ADD2	%st, %st(1)
	FADD	3 * SIZE(Y)
	FST	3 * SIZE(Y)

	FLD	4 * SIZE(X)
	fmul	%st(1), %st
	FLD	5 * SIZE(X)
	fmul	%st(3), %st
	ADD1	%st, %st(1)
	FADD	4 * SIZE(Y)
	FST	4 * SIZE(Y)

	FLD	4 * SIZE(X)
	fmul	%st(2), %st
	FLD	5 * SIZE(X)
	fmul	%st(2), %st
	ADD2	%st, %st(1)
	FADD	5 * SIZE(Y)
	FST	5 * SIZE(Y)

	FLD	6 * SIZE(X)
	fmul	%st(1), %st
	FLD	7 * SIZE(X)
	fmul	%st(3), %st
	ADD1	%st, %st(1)
	FADD	6 * SIZE(Y)
	FST	6 * SIZE(Y)

	FLD	6 * SIZE(X)
	fmul	%st(2), %st
	FLD	7 * SIZE(X)
	fmul	%st(2), %st
	ADD2	%st, %st(1)
	FADD	7 * SIZE(Y)
	FST	7 * SIZE(Y)

#ifdef HAVE_3DNOW
	prefetch	20 * SIZE(X)
	prefetchw	20 * SIZE(Y)
#endif

	addl	$8 * SIZE, X
	addl	$8 * SIZE, Y
	decl	%eax
	jg	.L16
	ALIGN_3

.L15:
	movl	M,  %eax
	andl	$3, %eax
	jle	.L40
	ALIGN_3

.L22:
	FLD	0 * SIZE(X)
	fmul	%st(1), %st
	FLD	1 * SIZE(X)
	fmul	%st(3), %st
	ADD1	%st, %st(1)
	FADD	0 * SIZE(Y)
	FST	0 * SIZE(Y)

	FLD	0 * SIZE(X)
	fmul	%st(2), %st
	FLD	1 * SIZE(X)
	fmul	%st(2), %st
	ADD2	%st, %st(1)
	FADD	1 * SIZE(Y)
	FST	1 * SIZE(Y)

	addl	$2 * SIZE, X
	addl	$2 * SIZE, Y
	decl	%eax
	jg	.L22
	jmp	.L40
	ALIGN_3

.L14:
	movl	M, %eax
	sarl	$2,   %eax
	jle	.L28
	ALIGN_3

.L29:
	FLD	0 * SIZE(X)
	fmul	%st(1), %st
	FLD	1 * SIZE(X)
	fmul	%st(3), %st
	ADD1	%st, %st(1)
	FADD	0 * SIZE(Y)
	FST	0 * SIZE(Y)

	FLD	0 * SIZE(X)
	fmul	%st(2), %st
	FLD	1 * SIZE(X)
	fmul	%st(2), %st
	ADD2	%st, %st(1)
	FADD	1 * SIZE(Y)
	FST	1 * SIZE(Y)

	addl	INCX, X
	addl	INCY, Y

	FLD	0 * SIZE(X)
	fmul	%st(1), %st
	FLD	1 * SIZE(X)
	fmul	%st(3), %st
	ADD1	%st, %st(1)
	FADD	0 * SIZE(Y)
	FST	0 * SIZE(Y)

	FLD	0 * SIZE(X)
	fmul	%st(2), %st
	FLD	1 * SIZE(X)
	fmul	%st(2), %st
	ADD2	%st, %st(1)
	FADD	1 * SIZE(Y)
	FST	1 * SIZE(Y)

	addl	INCX, X
	addl	INCY, Y

	FLD	0 * SIZE(X)
	fmul	%st(1), %st
	FLD	1 * SIZE(X)
	fmul	%st(3), %st
	ADD1	%st, %st(1)
	FADD	0 * SIZE(Y)
	FST	0 * SIZE(Y)

	FLD	0 * SIZE(X)
	fmul	%st(2), %st
	FLD	1 * SIZE(X)
	fmul	%st(2), %st
	ADD2	%st, %st(1)
	FADD	1 * SIZE(Y)
	FST	1 * SIZE(Y)

	addl	INCX, X
	addl	INCY, Y

	FLD	0 * SIZE(X)
	fmul	%st(1), %st
	FLD	1 * SIZE(X)
	fmul	%st(3), %st
	ADD1	%st, %st(1)
	FADD	0 * SIZE(Y)
	FST	0 * SIZE(Y)

	FLD	0 * SIZE(X)
	fmul	%st(2), %st
	FLD	1 * SIZE(X)
	fmul	%st(2), %st
	ADD2	%st, %st(1)
	FADD	1 * SIZE(Y)
	FST	1 * SIZE(Y)

	addl	INCX, X
	addl	INCY, Y

	decl	%eax
	jg	.L29
	ALIGN_3

.L28:
	movl	M,  %eax
	andl	$3, %eax
	jle	.L40
	ALIGN_3

.L35:
	FLD	0 * SIZE(X)
	fmul	%st(1), %st
	FLD	1 * SIZE(X)
	fmul	%st(3), %st
	ADD1	%st, %st(1)
	FADD	0 * SIZE(Y)
	FST	0 * SIZE(Y)

	FLD	0 * SIZE(X)
	fmul	%st(2), %st
	FLD	1 * SIZE(X)
	fmul	%st(2), %st
	ADD2	%st, %st(1)
	FADD	1 * SIZE(Y)
	FST	1 * SIZE(Y)

	addl	INCX, X
	addl	INCY, Y

	decl	%eax
	jg	.L35
	ALIGN_3

.L40:
	ffreep	%st(0)
	ffreep	%st(0)
	xorl	%eax,%eax
	popl	%ebx
	popl	%esi
	popl	%edi
	ret

	EPILOGUE
