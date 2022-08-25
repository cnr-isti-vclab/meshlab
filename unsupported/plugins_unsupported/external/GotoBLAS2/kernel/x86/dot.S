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
	
#define STACK_N		 4 + STACK + ARGS(%esp)
#define STACK_X		 8 + STACK + ARGS(%esp)
#define STACK_INCX	12 + STACK + ARGS(%esp)
#define STACK_Y		16 + STACK + ARGS(%esp)
#define STACK_INCY	20 + STACK + ARGS(%esp)

#define N	%ebx
#define X	%esi
#define INCX	%ecx
#define Y	%edi
#define INCY	%edx


	PROLOGUE

	pushl	%edi
	pushl	%esi
	pushl	%ebx

	PROFCODE

#if defined(F_INTERFACE_GFORT) || defined(F_INTERFACE_G95)
	EMMS
#endif

	movl	STACK_N,     N
	movl	STACK_X,     X
	movl	STACK_INCX,  INCX
	movl	STACK_Y,     Y
	movl	STACK_INCY,  INCY

#ifdef F_INTERFACE
	movl	(N),N
	movl	(INCX),INCX
	movl	(INCY),INCY
#endif

	leal	(, INCX, SIZE), INCX	
	leal	(, INCY, SIZE), INCY	

	fldz
	fldz
	fldz
	fldz

	cmpl	$SIZE, INCX
	jne	.L14
	cmpl	$SIZE, INCY
	jne	.L14

	movl	N, %eax
	sarl	$2,   %eax
	jle	.L15
	ALIGN_3

.L16:
	FLD	0 * SIZE(X)
	FMUL	0 * SIZE(Y)
	faddp	%st,%st(1)
	FLD	1 * SIZE(X)
	FMUL	1 * SIZE(Y)
	faddp	%st,%st(2)
	FLD	2 * SIZE(X)
	FMUL	2 * SIZE(Y)
	faddp	%st,%st(3)
	FLD	3 * SIZE(X)
	FMUL	3 * SIZE(Y)
	faddp	%st,%st(4)
	addl	$4 * SIZE, X
	addl	$4 * SIZE, Y
	decl	%eax
	jg	.L16
	ALIGN_3

.L15:
	movl	N, %eax
	andl	$3,   %eax
	jle	.L27
	ALIGN_3

.L22:
	FLD	(X)
	addl	$SIZE, X
	FMUL	(Y)
	addl	$SIZE, Y
	faddp	%st,%st(1)
	decl	%eax
	jg	.L22

	jmp	.L27
	ALIGN_3

.L14:
#ifdef F_INTERFACE
	testl	INCX, INCX
	jge	.L28

	movl	N, %eax
	decl	%eax
	imull	INCX, %eax
	subl	%eax, X
	ALIGN_3

.L28:
	testl	INCY, INCY
	jge	.L29

	movl	N, %eax
	decl	%eax
	imull	INCY, %eax
	subl	%eax, Y
	ALIGN_3
.L29:
#endif
	movl	N, %eax
	sarl	$2,   %eax
	jle	.L30
	ALIGN_3

.L31:
	FLD	(X)
	addl	INCX, X
	FMUL	(Y)
	addl	INCY, Y
	faddp	%st,%st(1)

	FLD	(X)
	addl	INCX, X
	FMUL	(Y)
	addl	INCY, Y
	faddp	%st,%st(2)

	FLD	(X)
	addl	INCX, X
	FMUL	(Y)
	addl	INCY, Y
	faddp	%st,%st(3)

	FLD	(X)
	addl	INCX, X
	FMUL	(Y)
	addl	INCY, Y
	faddp	%st,%st(4)

	decl	%eax
	jg	.L31
	ALIGN_3

.L30:
	movl	N, %eax
	andl	$3,   %eax
	jle	.L27
	ALIGN_3

.L37:
	FLD	(X)
	addl	INCX, X
	FMUL	(Y)
	addl	INCY, Y
	faddp	%st, %st(1)
	decl	%eax
	jg	.L37
	ALIGN_3

.L27:
	faddp	%st,%st(2)
	faddp	%st,%st(2)
	faddp	%st,%st(1)

	popl	%ebx
	popl	%esi
	popl	%edi
	ret

	EPILOGUE
