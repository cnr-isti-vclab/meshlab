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

#define N	ARG1	/* rdi */
#define X	ARG2	/* rsi */
#define INCX	ARG3	/* rdx */
#define Y	ARG4	/* rcx */
#define INCY	ARG5	/* r8  */

#include "l1param.h"

	PROLOGUE
	PROFCODE

	salq	$BASE_SHIFT, INCX
	salq	$BASE_SHIFT, INCY

	fldz
	fldz
	fldz
	fldz

	cmpq	$SIZE, INCX
	jne	.L14
	cmpq	$SIZE, INCY
	jne	.L14

	movq	N, %rax
	sarq	$2,   %rax
	jle	.L15
	ALIGN_3

.L16:
#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	FLD	0 * SIZE(X)
	FLD	0 * SIZE(Y)
	fmulp	%st, %st(1)
	faddp	%st,%st(1)
	FLD	1 * SIZE(X)
	FLD	1 * SIZE(Y)
	fmulp	%st, %st(1)
	faddp	%st,%st(2)

#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(Y)
#endif

	FLD	2 * SIZE(X)
	FLD	2 * SIZE(Y)
	fmulp	%st, %st(1)
	faddp	%st,%st(3)
	FLD	3 * SIZE(X)
	FLD	3 * SIZE(Y)
	fmulp	%st, %st(1)
	faddp	%st,%st(4)
	addq	$4 * SIZE, X
	addq	$4 * SIZE, Y
	decq	%rax
	jg	.L16
	ALIGN_3

.L15:
	movq	N, %rax
	andq	$3,   %rax
	jle	.L27
	ALIGN_3

.L22:
	FLD	(X)
	addq	$SIZE, X
	FLD	(Y)
	fmulp	%st, %st(1)
	addq	$SIZE, Y
	faddp	%st,%st(1)
	decq	%rax
	jg	.L22

	jmp	.L27
	ALIGN_3

.L14:
	movq	N, %rax
	sarq	$2,   %rax
	jle	.L30
	ALIGN_3

.L31:
	FLD	(X)
	addq	INCX, X
	FLD	(Y)
	fmulp	%st, %st(1)
	addq	INCY, Y
	faddp	%st,%st(1)

	FLD	(X)
	addq	INCX, X
	FLD	(Y)
	fmulp	%st, %st(1)
	addq	INCY, Y
	faddp	%st,%st(2)

	FLD	(X)
	addq	INCX, X
	FLD	(Y)
	fmulp	%st, %st(1)
	addq	INCY, Y
	faddp	%st,%st(3)

	FLD	(X)
	addq	INCX, X
	FLD	(Y)
	fmulp	%st, %st(1)
	addq	INCY, Y
	faddp	%st,%st(4)

	decq	%rax
	jg	.L31
	ALIGN_3

.L30:
	movq	N, %rax
	andq	$3,   %rax
	jle	.L27
	ALIGN_3

.L37:
	FLD	(X)
	addq	INCX, X
	FLD	(Y)
	fmulp	%st, %st(1)
	addq	INCY, Y
	faddp	%st, %st(1)
	decq	%rax
	jg	.L37
	ALIGN_3

.L27:
	faddp	%st,%st(2)
	faddp	%st,%st(2)
	faddp	%st,%st(1)
	ret

	EPILOGUE
