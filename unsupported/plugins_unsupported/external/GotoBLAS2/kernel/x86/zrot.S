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
#define STACK_C		24 + STACK + ARGS(%esp)
#ifdef XDOUBLE
#define STACK_S		40 + STACK + ARGS(%esp)
#elif defined DOUBLE
#define STACK_S		32 + STACK + ARGS(%esp)
#else
#define STACK_S		28 + STACK + ARGS(%esp)
#endif

#define N	%ebx
#define X	%esi
#define INCX	%ecx
#define Y	%edi
#define INCY	%edx

#define	I	%eax

#ifdef PENTIUM4
#define PREFETCH	prefetcht0
#define PREFETCH_SIZE	144
#endif

#ifdef OPTERON
#define PREFETCH	prefetchw
#define PREFETCH_SIZE	144
#endif

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

	FLD	STACK_S
	FLD	STACK_C

	sall	$ZBASE_SHIFT, INCX
	sall	$ZBASE_SHIFT, INCY

	testl	N, N
	jle	.L999

	cmpl	$2 * SIZE, INCX
	jne	.L50
	cmpl	$2 * SIZE, INCY
	jne	.L50

	movl	N,  I
	sarl	$1, I
	jle	.L15
	ALIGN_4
	
.L10:
#ifdef PENTIUM4
	PREFETCH	(PREFETCH_SIZE + 0) * SIZE(X)
#endif
#ifdef OPTERON
	PREFETCH	(PREFETCH_SIZE + 0) * SIZE(X)
#endif

	FLD	0 * SIZE(X)
	FLD	0 * SIZE(Y)

	fld	%st(1)
	fmul	%st(3), %st

	fld	%st(1)
	fmul	%st(5), %st

	faddp	%st, %st(1)
	FST	0 * SIZE(X)

	fmul	%st(2), %st
	fxch	%st(1)
	fmul	%st(3), %st

	fsubrp	%st, %st(1)
	FST	0 * SIZE(Y)

	FLD	1 * SIZE(X)
	FLD	1 * SIZE(Y)

	fld	%st(1)
	fmul	%st(3), %st

	fld	%st(1)
	fmul	%st(5), %st

	faddp	%st, %st(1)
	FST	1 * SIZE(X)

	fmul	%st(2), %st
	fxch	%st(1)
	fmul	%st(3), %st

	fsubrp	%st, %st(1)
	FST	1 * SIZE(Y)

#ifdef PENTIUM4
	PREFETCH	(PREFETCH_SIZE + 0) * SIZE(Y)
#endif
#ifdef OPTERON
	PREFETCH	(PREFETCH_SIZE + 0) * SIZE(Y)
#endif

	FLD	2 * SIZE(X)
	FLD	2 * SIZE(Y)

	fld	%st(1)
	fmul	%st(3), %st

	fld	%st(1)
	fmul	%st(5), %st

	faddp	%st, %st(1)
	FST	2 * SIZE(X)

	fmul	%st(2), %st
	fxch	%st(1)
	fmul	%st(3), %st

	fsubrp	%st, %st(1)
	FST	2 * SIZE(Y)

	FLD	3 * SIZE(X)
	FLD	3 * SIZE(Y)

	fld	%st(1)
	fmul	%st(3), %st

	fld	%st(1)
	fmul	%st(5), %st

	faddp	%st, %st(1)
	FST	3 * SIZE(X)

	fmul	%st(2), %st
	fxch	%st(1)
	fmul	%st(3), %st

	fsubrp	%st, %st(1)
	FST	3 * SIZE(Y)

	addl	$4 * SIZE, X
	addl	$4 * SIZE, Y

	decl	I
	jg	.L10
	ALIGN_4

.L15:
	movl	N,  I
	andl	$1, I
	jle	.L999
	ALIGN_4

.L16:
	FLD	0 * SIZE(X)
	FLD	0 * SIZE(Y)

	fld	%st(1)
	fmul	%st(3), %st

	fld	%st(1)
	fmul	%st(5), %st

	faddp	%st, %st(1)
	FST	0 * SIZE(X)

	fmul	%st(2), %st
	fxch	%st(1)
	fmul	%st(3), %st

	fsubrp	%st, %st(1)
	FST	0 * SIZE(Y)

	FLD	1 * SIZE(X)
	FLD	1 * SIZE(Y)

	fld	%st(1)
	fmul	%st(3), %st

	fld	%st(1)
	fmul	%st(5), %st

	faddp	%st, %st(1)
	FST	1 * SIZE(X)

	fmul	%st(2), %st
	fxch	%st(1)
	fmul	%st(3), %st

	fsubrp	%st, %st(1)
	FST	1 * SIZE(Y)
	jmp	.L999
	ALIGN_4

.L50:
	movl	N,  I
	sarl	$1, I
	jle	.L55
	ALIGN_4
	
.L51:
	FLD	0 * SIZE(X)
	FLD	0 * SIZE(Y)

	fld	%st(1)
	fmul	%st(3), %st

	fld	%st(1)
	fmul	%st(5), %st

	faddp	%st, %st(1)
	FST	0 * SIZE(X)

	fmul	%st(2), %st
	fxch	%st(1)
	fmul	%st(3), %st

	fsubrp	%st, %st(1)
	FST	0 * SIZE(Y)

	FLD	1 * SIZE(X)
	FLD	1 * SIZE(Y)

	fld	%st(1)
	fmul	%st(3), %st

	fld	%st(1)
	fmul	%st(5), %st

	faddp	%st, %st(1)
	FST	1 * SIZE(X)

	fmul	%st(2), %st
	fxch	%st(1)
	fmul	%st(3), %st

	fsubrp	%st, %st(1)
	FST	1 * SIZE(Y)

	addl	INCX, X
	addl	INCY, Y

	FLD	0 * SIZE(X)
	FLD	0 * SIZE(Y)

	fld	%st(1)
	fmul	%st(3), %st

	fld	%st(1)
	fmul	%st(5), %st

	faddp	%st, %st(1)
	FST	0 * SIZE(X)

	fmul	%st(2), %st
	fxch	%st(1)
	fmul	%st(3), %st

	fsubrp	%st, %st(1)
	FST	0 * SIZE(Y)

	FLD	1 * SIZE(X)
	FLD	1 * SIZE(Y)

	fld	%st(1)
	fmul	%st(3), %st

	fld	%st(1)
	fmul	%st(5), %st

	faddp	%st, %st(1)
	FST	1 * SIZE(X)

	fmul	%st(2), %st
	fxch	%st(1)
	fmul	%st(3), %st

	fsubrp	%st, %st(1)
	FST	1 * SIZE(Y)

	addl	INCX, X
	addl	INCY, Y

	decl	I
	jg	.L51
	ALIGN_4

.L55:
	movl	N,  I
	andl	$1, I
	jle	.L999
	ALIGN_4

.L56:
	FLD	0 * SIZE(X)
	FLD	0 * SIZE(Y)

	fld	%st(1)
	fmul	%st(3), %st

	fld	%st(1)
	fmul	%st(5), %st

	faddp	%st, %st(1)
	FST	0 * SIZE(X)

	fmul	%st(2), %st
	fxch	%st(1)
	fmul	%st(3), %st

	fsubrp	%st, %st(1)
	FST	0 * SIZE(Y)

	FLD	1 * SIZE(X)
	FLD	1 * SIZE(Y)

	fld	%st(1)
	fmul	%st(3), %st

	fld	%st(1)
	fmul	%st(5), %st

	faddp	%st, %st(1)
	FST	1 * SIZE(X)

	fmul	%st(2), %st
	fxch	%st(1)
	fmul	%st(3), %st

	fsubrp	%st, %st(1)
	FST	1 * SIZE(Y)
	ALIGN_4

.L999:
	ffreep	%st(0)
	ffreep	%st(0)

	popl	%ebx
	popl	%esi
	popl	%edi
	ret

	EPILOGUE
