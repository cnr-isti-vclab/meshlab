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

#define M	ARG1	/* rdi */
#define X	ARG4	/* rsi */
#define INCX	ARG5	/* rdx */
#define Y	ARG6	/* rcx */
#define INCY	ARG2	/* r8  */

#ifndef CONJ
#define ADD1	fsubrp
#define ADD2	faddp
#else
#define ADD1	faddp
#define ADD2	fsubrp
#endif

#define ALPHA_R	  8(%rsp)
#define ALPHA_I	 24(%rsp)

#include "l1param.h"

	PROLOGUE
	PROFCODE

	FLD	ALPHA_I
	FLD	ALPHA_R

	movq	40(%rsp), INCY

	salq	$ZBASE_SHIFT, INCX
	salq	$ZBASE_SHIFT, INCY
	
	testq	M, M
	jle	.L40
	
	cmpq	$2 * SIZE, INCX
	jne	.L14
	cmpq	$2 * SIZE, INCY
	jne	.L14

	movq	M,  %rax
	sarq	$2, %rax
	jle	.L15
	ALIGN_3

.L16:
#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	FLD	0 * SIZE(X)
	fmul	%st(1), %st
	FLD	1 * SIZE(X)
	fmul	%st(3), %st
	ADD1	%st, %st(1)
	FLD	0 * SIZE(Y)
	faddp	%st, %st(1)
	FST	0 * SIZE(Y)

	FLD	0 * SIZE(X)
	fmul	%st(2), %st
	FLD	1 * SIZE(X)
	fmul	%st(2), %st
	ADD2	%st, %st(1)
	FLD	1 * SIZE(Y)
	faddp	%st, %st(1)
	FST	1 * SIZE(Y)

	FLD	2 * SIZE(X)
	fmul	%st(1), %st
	FLD	3 * SIZE(X)
	fmul	%st(3), %st
	ADD1	%st, %st(1)
	FLD	2 * SIZE(Y)
	faddp	%st, %st(1)
	FST	2 * SIZE(Y)

	FLD	2 * SIZE(X)
	fmul	%st(2), %st
	FLD	3 * SIZE(X)
	fmul	%st(2), %st
	ADD2	%st, %st(1)
	FLD	3 * SIZE(Y)
	faddp	%st, %st(1)
	FST	3 * SIZE(Y)

#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(Y)
#endif

	FLD	4 * SIZE(X)
	fmul	%st(1), %st
	FLD	5 * SIZE(X)
	fmul	%st(3), %st
	ADD1	%st, %st(1)
	FLD	4 * SIZE(Y)
	faddp	%st, %st(1)
	FST	4 * SIZE(Y)

	FLD	4 * SIZE(X)
	fmul	%st(2), %st
	FLD	5 * SIZE(X)
	fmul	%st(2), %st
	ADD2	%st, %st(1)
	FLD	5 * SIZE(Y)
	faddp	%st, %st(1)
	FST	5 * SIZE(Y)

	FLD	6 * SIZE(X)
	fmul	%st(1), %st
	FLD	7 * SIZE(X)
	fmul	%st(3), %st
	ADD1	%st, %st(1)
	FLD	6 * SIZE(Y)
	faddp	%st, %st(1)
	FST	6 * SIZE(Y)

	FLD	6 * SIZE(X)
	fmul	%st(2), %st
	FLD	7 * SIZE(X)
	fmul	%st(2), %st
	ADD2	%st, %st(1)
	FLD	7 * SIZE(Y)
	faddp	%st, %st(1)
	FST	7 * SIZE(Y)

	addq	$8 * SIZE, X
	addq	$8 * SIZE, Y
	decq	%rax
	jg	.L16
	ALIGN_3

.L15:
	movq	M,  %rax
	andq	$3, %rax
	jle	.L40
	ALIGN_3

.L22:
	FLD	0 * SIZE(X)
	fmul	%st(1), %st
	FLD	1 * SIZE(X)
	fmul	%st(3), %st
	ADD1	%st, %st(1)
	FLD	0 * SIZE(Y)
	faddp	%st, %st(1)
	FST	0 * SIZE(Y)

	FLD	0 * SIZE(X)
	fmul	%st(2), %st
	FLD	1 * SIZE(X)
	fmul	%st(2), %st
	ADD2	%st, %st(1)
	FLD	1 * SIZE(Y)
	faddp	%st, %st(1)
	FST	1 * SIZE(Y)

	addq	$2 * SIZE, X
	addq	$2 * SIZE, Y
	decq	%rax
	jg	.L22
	jmp	.L40
	ALIGN_3

.L14:
	movq	M, %rax
	sarq	$2,   %rax
	jle	.L28
	ALIGN_3

.L29:
	FLD	0 * SIZE(X)
	fmul	%st(1), %st
	FLD	1 * SIZE(X)
	fmul	%st(3), %st
	ADD1	%st, %st(1)
	FLD	0 * SIZE(Y)
	faddp	%st, %st(1)
	FST	0 * SIZE(Y)

	FLD	0 * SIZE(X)
	fmul	%st(2), %st
	FLD	1 * SIZE(X)
	fmul	%st(2), %st
	ADD2	%st, %st(1)
	FLD	1 * SIZE(Y)
	faddp	%st, %st(1)
	FST	1 * SIZE(Y)

	addq	INCX, X
	addq	INCY, Y

	FLD	0 * SIZE(X)
	fmul	%st(1), %st
	FLD	1 * SIZE(X)
	fmul	%st(3), %st
	ADD1	%st, %st(1)
	FLD	0 * SIZE(Y)
	faddp	%st, %st(1)
	FST	0 * SIZE(Y)

	FLD	0 * SIZE(X)
	fmul	%st(2), %st
	FLD	1 * SIZE(X)
	fmul	%st(2), %st
	ADD2	%st, %st(1)
	FLD	1 * SIZE(Y)
	faddp	%st, %st(1)
	FST	1 * SIZE(Y)

	addq	INCX, X
	addq	INCY, Y

	FLD	0 * SIZE(X)
	fmul	%st(1), %st
	FLD	1 * SIZE(X)
	fmul	%st(3), %st
	ADD1	%st, %st(1)
	FLD	0 * SIZE(Y)
	faddp	%st, %st(1)
	FST	0 * SIZE(Y)

	FLD	0 * SIZE(X)
	fmul	%st(2), %st
	FLD	1 * SIZE(X)
	fmul	%st(2), %st
	ADD2	%st, %st(1)
	FLD	1 * SIZE(Y)
	faddp	%st, %st(1)
	FST	1 * SIZE(Y)

	addq	INCX, X
	addq	INCY, Y

	FLD	0 * SIZE(X)
	fmul	%st(1), %st
	FLD	1 * SIZE(X)
	fmul	%st(3), %st
	ADD1	%st, %st(1)
	FLD	0 * SIZE(Y)
	faddp	%st, %st(1)
	FST	0 * SIZE(Y)

	FLD	0 * SIZE(X)
	fmul	%st(2), %st
	FLD	1 * SIZE(X)
	fmul	%st(2), %st
	ADD2	%st, %st(1)
	FLD	1 * SIZE(Y)
	faddp	%st, %st(1)
	FST	1 * SIZE(Y)

	addq	INCX, X
	addq	INCY, Y

	decq	%rax
	jg	.L29
	ALIGN_3

.L28:
	movq	M,  %rax
	andq	$3, %rax
	jle	.L40
	ALIGN_3

.L35:
	FLD	0 * SIZE(X)
	fmul	%st(1), %st
	FLD	1 * SIZE(X)
	fmul	%st(3), %st
	ADD1	%st, %st(1)
	FLD	0 * SIZE(Y)
	faddp	%st, %st(1)
	FST	0 * SIZE(Y)

	FLD	0 * SIZE(X)
	fmul	%st(2), %st
	FLD	1 * SIZE(X)
	fmul	%st(2), %st
	ADD2	%st, %st(1)
	FLD	1 * SIZE(Y)
	faddp	%st, %st(1)
	FST	1 * SIZE(Y)

	addq	INCX, X
	addq	INCY, Y

	decq	%rax
	jg	.L35
	ALIGN_3

.L40:
	ffreep	%st(0)
	ffreep	%st(0)
	ret
	
	EPILOGUE
