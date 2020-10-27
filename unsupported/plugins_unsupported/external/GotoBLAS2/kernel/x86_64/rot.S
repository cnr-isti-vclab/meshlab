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
	
#define N	ARG1
#define X	ARG2
#define INCX	ARG3
#define Y	ARG4
#ifndef WINDOWS_ABI
#define INCY	ARG5	/* r8  */
#else
#define INCY	%r10
#endif

#define	I	%rax

#include "l1param.h"

	PROLOGUE
	PROFCODE

#ifdef WINDOWS_ABI
	movq	48(%rsp), INCY
	FLD	72(%rsp)
	FLD	56(%rsp)
#else
	FLD	24(%rsp)
	FLD	 8(%rsp)
#endif

	salq	$BASE_SHIFT, INCX
	salq	$BASE_SHIFT, INCY

	testq	N, N
	jle	.L999

	cmpq	$SIZE, INCX
	jne	.L50
	cmpq	$SIZE, INCY
	jne	.L50

	movq	N,  I
	sarq	$2, I
	jle	.L15
	ALIGN_4
	
.L10:
#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(X)
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

#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(Y)
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

	addq	$4 * SIZE, X
	addq	$4 * SIZE, Y

	decq	I
	jg	.L10
	ALIGN_4

.L15:
	movq	N,  I
	andq	$3, I
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

	addq	$SIZE, X
	addq	$SIZE, Y

	decq	I
	jg	.L16
	jmp	.L999
	ALIGN_4

.L50:
	movq	N,  I
	sarq	$2, I
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

	addq	INCX, X
	addq	INCY, Y

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

	addq	INCX, X
	addq	INCY, Y

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

	addq	INCX, X
	addq	INCY, Y

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

	addq	INCX, X
	addq	INCY, Y

	decq	I
	jg	.L51
	ALIGN_4

.L55:
	movq	N,  I
	andq	$3, I
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

	addq	INCX, X
	addq	INCY, Y

	decq	I
	jg	.L56
	ALIGN_4


.L999:
	ffreep	%st
	ffreep	%st
	ret

	EPILOGUE
