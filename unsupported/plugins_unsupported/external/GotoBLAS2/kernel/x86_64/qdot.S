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

#include "l1param.h"

	PROLOGUE

	pushl	%edi
	pushl	%esi
	pushl	%ebx

	PROFCODE

	movl	STACK_N,     N
	movl	STACK_X,     X
	movl	STACK_INCX,  INCX
	movl	STACK_Y,     Y
	movl	STACK_INCY,  INCY

	sall	$BASE_SHIFT, INCX
	sall	$BASE_SHIFT, INCY

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
#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	FLD	0 * SIZE(X)
	FLD	0 * SIZE(Y)
	fmulp	%st, %st(1)
	faddp	%st,%st(1)
	FLD	1 * SIZE(X)
	FLD	1 * SIZE(Y)
	fmulp	%st, %st(1)
	faddp	%st,%st(2)

#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(Y)
#endif

	FLD	2 * SIZE(X)
	FLD	2 * SIZE(Y)
	fmulp	%st, %st(1)
	faddp	%st,%st(3)
	FLD	3 * SIZE(X)
	FLD	3 * SIZE(Y)
	fmulp	%st, %st(1)
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
	FLD	(Y)
	fmulp	%st, %st(1)
	addl	$SIZE, Y
	faddp	%st,%st(1)
	decl	%eax
	jg	.L22

	jmp	.L27
	ALIGN_3

.L14:
	movl	N, %eax
	sarl	$2,   %eax
	jle	.L30
	ALIGN_3

.L31:
	FLD	(X)
	addl	INCX, X
	FLD	(Y)
	fmulp	%st, %st(1)
	addl	INCY, Y
	faddp	%st,%st(1)

	FLD	(X)
	addl	INCX, X
	FLD	(Y)
	fmulp	%st, %st(1)
	addl	INCY, Y
	faddp	%st,%st(2)

	FLD	(X)
	addl	INCX, X
	FLD	(Y)
	fmulp	%st, %st(1)
	addl	INCY, Y
	faddp	%st,%st(3)

	FLD	(X)
	addl	INCX, X
	FLD	(Y)
	fmulp	%st, %st(1)
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
	FLD	(Y)
	fmulp	%st, %st(1)
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
