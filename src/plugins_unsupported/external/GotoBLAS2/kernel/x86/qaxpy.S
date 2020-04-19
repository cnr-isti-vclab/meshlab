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
#define STACK_ALPHA	16 + STACK + ARGS(%esp)
#define STACK_X		32 + STACK + ARGS(%esp)
#define STACK_INCX	36 + STACK + ARGS(%esp)
#define STACK_Y		40 + STACK + ARGS(%esp)
#define STACK_INCY	44 + STACK + ARGS(%esp)

#define M	%ebx
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

	FLD	STACK_ALPHA

	movl	STACK_M,    M
	movl	STACK_X,    X
	movl	STACK_INCX, INCX
	movl	STACK_Y,    Y
	movl	STACK_INCY, INCY

	sall	$BASE_SHIFT, INCX
	sall	$BASE_SHIFT, INCY

	testl	M, M
	jle	.L40

	cmpl	$SIZE, INCX
	jne	.L14
	cmpl	$SIZE, INCY
	jne	.L14

	movl	M,  %eax
	sarl	$3, %eax
	jle	.L15
	ALIGN_3

#define PRESIZE 33

.L16:
#ifdef HAS_PREFETCH
	prefetcht0	PRESIZE * SIZE(X)
#endif

	FLD	0 * SIZE(X)
	fmul	%st(1),%st
	FLD	0 * SIZE(Y)
	faddp	%st, %st(1)
	FST	0 * SIZE(Y)

	FLD	1 * SIZE(X)
	fmul	%st(1),%st
	FLD	1 * SIZE(Y)
	faddp	%st, %st(1)
	FST	1 * SIZE(Y)

	FLD	2 * SIZE(X)
	fmul	%st(1),%st
	FLD	2 * SIZE(Y)
	faddp	%st, %st(1)
	FST	2 * SIZE(Y)

	FLD	3 * SIZE(X)
	fmul	%st(1),%st
	FLD	3 * SIZE(Y)
	faddp	%st, %st(1)
	FST	3 * SIZE(Y)

#ifdef HAS_PREFETCH
	prefetcht0	(4 + PRESIZE) * SIZE(X)
#endif

	FLD	4 * SIZE(X)
	fmul	%st(1),%st
	FLD	4 * SIZE(Y)
	faddp	%st, %st(1)
	FST	4 * SIZE(Y)

	FLD	5 * SIZE(X)
	fmul	%st(1),%st
	FLD	5 * SIZE(Y)
	faddp	%st, %st(1)
	FST	5 * SIZE(Y)

	FLD	6 * SIZE(X)
	fmul	%st(1),%st
	FLD	6 * SIZE(Y)
	faddp	%st, %st(1)
	FST	6 * SIZE(Y)

	FLD	7 * SIZE(X)
	fmul	%st(1),%st
	FLD	7 * SIZE(Y)
	faddp	%st, %st(1)
	FST	7 * SIZE(Y)

#ifdef HAVE_3DNOW
	prefetchw	24 * SIZE(Y)
#endif
	
	addl	$8 * SIZE, X
	addl	$8 * SIZE, Y
	decl	%eax
	jg	.L16
	ALIGN_3

.L15:
	movl	M, %eax
	andl	$7,   %eax
	jle	.L40
	ALIGN_3

.L22:
	FLD	0 * SIZE(X)
	fmul	%st(1),%st
	FLD	0 * SIZE(Y)
	faddp	%st, %st(1)
	FST	0 * SIZE(Y)
	addl	$SIZE, X
	addl	$SIZE, Y
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
	FLD	(X)
	fmul	%st(1),%st
	FLD	(Y)
	faddp	%st, %st(1)
	FST	(Y)
	addl	INCX, X
	addl	INCY, Y

	FLD	(X)
	fmul	%st(1),%st
	FLD	(Y)
	faddp	%st, %st(1)
	FST	(Y)
	addl	INCX, X
	addl	INCY, Y

	FLD	(X)
	fmul	%st(1),%st
	FLD	(Y)
	faddp	%st, %st(1)
	FST	(Y)
	addl	INCX, X
	addl	INCY, Y

	FLD	(X)
	fmul	%st(1),%st
	FLD	(Y)
	faddp	%st, %st(1)
	FST	(Y)
	addl	INCX, X
	addl	INCY, Y

	decl	%eax
	jg	.L29
	ALIGN_3

.L28:
	movl	M, %eax
	andl	$3,   %eax
	jle	.L40
	ALIGN_3

.L35:
	FLD	(X)
	fmul	%st(1),%st
	FLD	(Y)
	faddp	%st, %st(1)
	FST	(Y)
	addl	INCX, X
	addl	INCY, Y

	decl	%eax
	jg	.L35
	ALIGN_3

.L40:
	ffreep	%st(0)
	xorl	%eax,%eax

	popl	%ebx
	popl	%esi
	popl	%edi
	ret

	EPILOGUE
