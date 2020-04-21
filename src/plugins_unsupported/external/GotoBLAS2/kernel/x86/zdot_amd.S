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
#define ARGS     0
	
#if !defined(DOUBLE) && !defined(XDOUBLE)
#define RESULT		 4 + STACK + ARGS(%esp)
#define STACK_N		 8 + STACK + ARGS(%esp)
#define STACK_X		12 + STACK + ARGS(%esp)
#define STACK_INCX	16 + STACK + ARGS(%esp)
#define STACK_Y		20 + STACK + ARGS(%esp)
#define STACK_INCY	24 + STACK + ARGS(%esp)
#else
#define STACK_N		 4 + STACK + ARGS(%esp)
#define STACK_X		 8 + STACK + ARGS(%esp)
#define STACK_INCX	12 + STACK + ARGS(%esp)
#define STACK_Y		16 + STACK + ARGS(%esp)
#define STACK_INCY	20 + STACK + ARGS(%esp)
#endif

	PROLOGUE

	pushl	%edi
	pushl	%esi
	pushl	%ebx

	PROFCODE

#if defined(F_INTERFACE_GFORT) || defined(F_INTERFACE_G95)
	EMMS
#endif

#define N	%ebx
#define X	%esi
#define INCX	%ecx
#define Y	%edi
#define INCY	%edx

	movl	STACK_N,     N
	movl	STACK_X,     X
	movl	STACK_INCX,  INCX
	movl	STACK_Y,     Y
	movl	STACK_INCY,  INCY

#if defined(F_INTERFACE)
	movl	(N),N
	movl	(INCX),INCX
	movl	(INCY),INCY
#endif

	testl	N, N
	jle	.L88

	fldz
	fldz
	fldz
	fldz

	addl	INCX, INCX
	addl	INCY, INCY

	leal	(, INCX, SIZE), INCX	
	leal	(, INCY, SIZE), INCY	

	cmpl	$2 * SIZE, INCX
	jne	.L14
	cmpl	$2 * SIZE, INCY
	jne	.L14

	movl	N,  %eax
	sarl	$2, %eax
	jle	.L15

	FLD	 0 * SIZE(X)
	ALIGN_3

.L16:
	FLD	 0 * SIZE(Y)
	PADDING fmul	%st(1)
	faddp	%st, %st(2)

	FMUL	 1 * SIZE(Y)
	faddp	%st, %st(2)
	FLD	 1 * SIZE(X)

	FLD	 0 * SIZE(Y)
	PADDING fmul	%st(1)
	faddp	%st, %st(4)

	FMUL	 1 * SIZE(Y)
	faddp	%st, %st(4)
	FLD	 2 * SIZE(X)

	FLD	 2 * SIZE(Y)
	PADDING fmul	%st(1)
	faddp	%st, %st(2)

	FMUL	 3 * SIZE(Y)
	faddp	%st, %st(2)
	FLD	 3 * SIZE(X)

	FLD	 2 * SIZE(Y)
	PADDING fmul	%st(1)
	faddp	%st, %st(4)

	FMUL	 3 * SIZE(Y)
	faddp	%st, %st(4)
	FLD	 4 * SIZE(X)

	FLD	 4 * SIZE(Y)
	PADDING fmul	%st(1)
	faddp	%st, %st(2)

	FMUL	 5 * SIZE(Y)
	faddp	%st, %st(2)
	FLD	 5 * SIZE(X)

	FLD	 4 * SIZE(Y)
	PADDING fmul	%st(1)
	faddp	%st, %st(4)

	FMUL	 5 * SIZE(Y)
	faddp	%st, %st(4)
	FLD	 6 * SIZE(X)

	FLD	 6 * SIZE(Y)
	PADDING fmul	%st(1)
	faddp	%st, %st(2)

	FMUL	 7 * SIZE(Y)
	faddp	%st, %st(2)
	FLD	 7 * SIZE(X)

	FLD	 6 * SIZE(Y)
	PADDING fmul	%st(1)
	faddp	%st, %st(4)

	FMUL	 7 * SIZE(Y)
	faddp	%st, %st(4)
	FLD	 8 * SIZE(X)

	prefetch	32 * SIZE(X)

	addl	$8 * SIZE, X
	addl	$8 * SIZE, Y
	decl	%eax
	jg	.L16
	ffreep	%st(0)
	ALIGN_3

.L15:
	movl	N,  %eax
	andl	$3, %eax
	jle	.L27
	ALIGN_3

.L22:
	FLD	 0 * SIZE(X)

	FLD	 0 * SIZE(Y)
	fmul	%st(1)
	faddp	%st, %st(2)

	FMUL	 1 * SIZE(Y)
	faddp	%st, %st(2)
	FLD	 1 * SIZE(X)

	FLD	 0 * SIZE(Y)
	fmul	%st(1)
	faddp	%st, %st(4)

	FMUL	 1 * SIZE(Y)
	faddp	%st, %st(4)

	addl	$2 * SIZE, X
	addl	$2 * SIZE, Y

	decl	%eax
	jg	.L22
	jmp	.L27
	ALIGN_3

.L14:
#ifdef F_INTERFACE
	testl	INCX, INCX		# if (incx < 0)
	jge	.L28

	movl	N, %eax
	decl	%eax
	imull	INCX, %eax
	subl	%eax, X
	ALIGN_3

.L28:
	testl	INCY, INCY		# if (incy < 0)
	jge	.L29

	movl	N, %eax
	decl	%eax
	imull	INCY, %eax
	subl	%eax, Y
	ALIGN_3

.L29:
#endif

	movl	N,  %eax
	sarl	$1, %eax
	jle	.L30
	ALIGN_3


.L31:
	FLD	 0 * SIZE(X)

	FLD	 0 * SIZE(Y)
	fmul	%st(1)
	faddp	%st, %st(2)

	FMUL	 1 * SIZE(Y)
	faddp	%st, %st(2)
	FLD	 1 * SIZE(X)

	FLD	 0 * SIZE(Y)
	fmul	%st(1)
	faddp	%st, %st(4)

	FMUL	 1 * SIZE(Y)
	faddp	%st, %st(4)
	addl	INCX, X

	FLD	 0 * SIZE(X)
	addl	INCY, Y

	FLD	 0 * SIZE(Y)
	fmul	%st(1)
	faddp	%st, %st(2)

	FMUL	 1 * SIZE(Y)
	faddp	%st, %st(2)
	FLD	 1 * SIZE(X)

	FLD	 0 * SIZE(Y)
	fmul	%st(1)
	faddp	%st, %st(4)

	FMUL	 1 * SIZE(Y)
	faddp	%st, %st(4)
	addl	INCX, X
	addl	INCY, Y

	decl	%eax
	jg	.L31
	ALIGN_3

.L30:
	movl	N,  %eax
	andl	$1, %eax
	jle	.L27
	ALIGN_3

.L37:
	FLD	 0 * SIZE(X)

	FLD	 0 * SIZE(Y)
	fmul	%st(1)
	faddp	%st, %st(2)

	FMUL	 1 * SIZE(Y)
	faddp	%st, %st(2)
	FLD	 1 * SIZE(X)

	FLD	 0 * SIZE(Y)
	fmul	%st(1)
	faddp	%st, %st(4)

	FMUL	 1 * SIZE(Y)
	faddp	%st, %st(4)
	ALIGN_3

.L27:
#ifndef CONJ
	fsubp	%st, %st(3)
	faddp	%st, %st(1)
#else
	faddp	%st, %st(3)
	fsubp	%st, %st(1)
#endif

#if !defined(DOUBLE) && !defined(XDOUBLE)
	subl	$2 * SIZE, %esp
	FST	1 * SIZE(%esp)
	FST	0 * SIZE(%esp)
	movl	0 * SIZE(%esp), %eax
	movl	1 * SIZE(%esp), %edx
	addl	$2 * SIZE, %esp
#else
	movl	RESULT, %eax

	FST	1 * SIZE(%eax)
	FST	0 * SIZE(%eax)
#endif

	popl	%ebx
	popl	%esi
	popl	%edi
#if defined(F_INTERFACE) && defined(F_PATHSCALE)
	ret	$0x4
#else
	ret
#endif

	ALIGN_3

.L88:
#if !defined(DOUBLE) && !defined(XDOUBLE)
	xor	%eax, %eax
	xor	%edx, %edx
#else
	movl	RESULT, %eax

	fldz
	fldz

	FST	1 * SIZE(%eax)
	FST	0 * SIZE(%eax)
#endif

	popl	%ebx
	popl	%esi
	popl	%edi
#if defined(F_INTERFACE) && defined(F_PATHSCALE)
	ret	$0x4
#else
	ret
#endif

	EPILOGUE
