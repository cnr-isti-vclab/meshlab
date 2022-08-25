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
#define X	ARG4
#define INCX	ARG5

#define I	%rax

#include "l1param.h"

	PROLOGUE
	PROFCODE

	salq	$ZBASE_SHIFT, INCX

	FLD	 8(%rsp)
	FLD	24(%rsp)

	testq	N, N
	jle	.L999

	fld	%st(1)
	fabs
	fld	%st(1)
	fabs
	faddp	%st, %st(1)

	fldz
	fcomip	%st(1), %st
	ffreep	%st
	jne	.L30

	EMMS

	pxor	%mm0, %mm0

	cmpq	$2 * SIZE, INCX
	jne	.L20

	movq	N,  I
	sarq	$2, I
	jle	.L15
	ALIGN_4

.L12:
#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	movq	%mm0,   0(X)
	movq	%mm0,   8(X)
	movq	%mm0,  16(X)
	movq	%mm0,  24(X)
	movq	%mm0,  32(X)
	movq	%mm0,  40(X)
	movq	%mm0,  48(X)
	movq	%mm0,  56(X)
	movq	%mm0,  64(X)
	movq	%mm0,  72(X)
	movq	%mm0,  80(X)
	movq	%mm0,  88(X)
	movq	%mm0,  96(X)
	movq	%mm0, 104(X)
	movq	%mm0, 112(X)
	movq	%mm0, 120(X)
	addq	$8 * SIZE, X
	decq	I
	jg	.L12
	ALIGN_3

.L15:
	movq	N,  I
	andq	$3, I
	jle	.L18
	ALIGN_2

.L16:
	movq	%mm0,   0(X)
	movq	%mm0,   8(X)
	movq	%mm0,  16(X)
	movq	%mm0,  24(X)

	addq	$2 * SIZE, X
	decq	I
	jg	.L16

.L18:
	EMMS

	ret
	ALIGN_2

.L20:
	movq	N,  I
	sarq	$2, I
	jle	.L25
	ALIGN_3

.L22:
	movq	%mm0,   0(X)
	movq	%mm0,   8(X)
	movq	%mm0,  16(X)
	movq	%mm0,  24(X)
	addq	INCX, X

	movq	%mm0,   0(X)
	movq	%mm0,   8(X)
	movq	%mm0,  16(X)
	movq	%mm0,  24(X)
	addq	INCX, X

	movq	%mm0,   0(X)
	movq	%mm0,   8(X)
	movq	%mm0,  16(X)
	movq	%mm0,  24(X)
	addq	INCX, X

	movq	%mm0,   0(X)
	movq	%mm0,   8(X)
	movq	%mm0,  16(X)
	movq	%mm0,  24(X)
	addq	INCX, X

	decq	I
	jg	.L22
	ALIGN_3

.L25:
	movq	N,  I
	andq	$3, I
	jle	.L28
	ALIGN_3

.L26:
	movq	%mm0,   0(X)
	movq	%mm0,   8(X)
	movq	%mm0,  16(X)
	movq	%mm0,  24(X)
	addq	INCX, X

	decq	I
	jg	.L26

.L28:
	EMMS

	ret
	ALIGN_3

.L30:
	movq	N, I
	ALIGN_2

.L32:
#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	FLD	0 * SIZE(X)
	fmul	%st(1),%st
	FLD	1 * SIZE(X)
	fmul	%st(3),%st
	faddp	%st,%st(1)

	FLD	0 * SIZE(X)
	fmul	%st(3),%st
	FLD	1 * SIZE(X)
	fmul	%st(3),%st
	fsubrp	%st,%st(1)

	FST	0 * SIZE(X)
	FST	1 * SIZE(X)
	addq	INCX, X
	decq	I
	jg	.L32
	ALIGN_2

.L999:
	ffreep	%st
	ffreep	%st

	ret

	EPILOGUE
