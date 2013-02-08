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

#define M	ARG1
#define X	ARG4
#define INCX	ARG5

#define I	%rax

#include "l1param.h"

	PROLOGUE
	PROFCODE

	FLD	 8(%rsp)

	ftst
	fnstsw	%ax
	andb	$68, %ah
	je	.L300

/* Alpha == ZERO */
	cmpq	$1, INCX
	jne	.L104

	movq	M,  I
	sarq	$3, I
	jle	.L102
	ALIGN_4

.L101:
	fld	%st
	FST	0 * SIZE(X)
	fld	%st
	FST	1 * SIZE(X)
	fld	%st
	FST	2 * SIZE(X)
	fld	%st
	FST	3 * SIZE(X)
	fld	%st
	FST	4 * SIZE(X)
	fld	%st
	FST	5 * SIZE(X)
	fld	%st
	FST	6 * SIZE(X)
	fld	%st
	FST	7 * SIZE(X)

	addq	$8 * SIZE, X
	decq	I
	jg	.L101
	ALIGN_4

.L102:
	movq	M,  I
	andq	$7, I
	jle	.L999
	ALIGN_4

.L103:
	fld	%st
	FST	0 * SIZE(X)

	addq	$SIZE, X
	decq	I
	jg	.L103
	jmp	.L999
	ALIGN_4

.L104:
	salq	$BASE_SHIFT, INCX

	movq	M,  I
	sarq	$3, I
	jle	.L106
	ALIGN_4

.L105:
	fld	%st
	FST	0 * SIZE(X)
	addq	INCX, X
	fld	%st
	FST	0 * SIZE(X)
	addq	INCX, X
	fld	%st
	FST	0 * SIZE(X)
	addq	INCX, X
	fld	%st
	FST	0 * SIZE(X)
	addq	INCX, X
	fld	%st
	FST	0 * SIZE(X)
	addq	INCX, X
	fld	%st
	FST	0 * SIZE(X)
	addq	INCX, X
	fld	%st
	FST	0 * SIZE(X)
	addq	INCX, X
	fld	%st
	FST	0 * SIZE(X)
	addq	INCX, X

	decq	I
	jg	.L105
	ALIGN_4

.L106:
	movq	M,  I
	andq	$7, I
	jle	.L999
	ALIGN_4

.L107:
	fld	%st
	FST	0 * SIZE(X)
	addq	INCX, X
	decq	I
	jg	.L107
	jmp	.L999
	ALIGN_4

/* Alpha != ZERO */

.L300:
	cmpq	$1,INCX
	jne	.L304

	movq	M,  I
	sarq	$3, I
	jle	.L302
	ALIGN_4

.L301:
#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	FLD	0 * SIZE(X)
	fmul	%st(1), %st
	FST	0 * SIZE(X)

	FLD	1 * SIZE(X)
	fmul	%st(1), %st
	FST	1 * SIZE(X)

	FLD	2 * SIZE(X)
	fmul	%st(1), %st
	FST	2 * SIZE(X)

	FLD	3 * SIZE(X)
	fmul	%st(1), %st
	FST	3 * SIZE(X)

	FLD	4 * SIZE(X)
	fmul	%st(1), %st
	FST	4 * SIZE(X)

	FLD	5 * SIZE(X)
	fmul	%st(1), %st
	FST	5 * SIZE(X)

	FLD	6 * SIZE(X)
	fmul	%st(1), %st
	FST	6 * SIZE(X)

	FLD	7 * SIZE(X)
	fmul	%st(1), %st
	FST	7 * SIZE(X)

	addq	$8 * SIZE, X
	decq	I
	jg	.L301
	ALIGN_4

.L302:
	movq	M,  I
	andq	$7, I
	jle	.L999
	ALIGN_4

.L303:
	FLD	0 * SIZE(X)
	fmul	%st(1), %st
	FST	0 * SIZE(X)
	addq	$SIZE, X
	decq	I
	jg	.L303
	jmp	.L999
	ALIGN_4

.L304:
	salq	$BASE_SHIFT, INCX

	movq	M,  I
	sarq	$3, I
	jle	.L306
	ALIGN_4

.L305:
	FLD	0 * SIZE(X)
	fmul	%st(1), %st
	FST	0 * SIZE(X)
	addq	INCX, X

	FLD	0 * SIZE(X)
	fmul	%st(1), %st
	FST	0 * SIZE(X)
	addq	INCX, X

	FLD	0 * SIZE(X)
	fmul	%st(1), %st
	FST	0 * SIZE(X)
	addq	INCX, X

	FLD	0 * SIZE(X)
	fmul	%st(1), %st
	FST	0 * SIZE(X)
	addq	INCX, X

	FLD	0 * SIZE(X)
	fmul	%st(1), %st
	FST	0 * SIZE(X)
	addq	INCX, X

	FLD	0 * SIZE(X)
	fmul	%st(1), %st
	FST	0 * SIZE(X)
	addq	INCX, X

	FLD	0 * SIZE(X)
	fmul	%st(1), %st
	FST	0 * SIZE(X)
	addq	INCX, X

	FLD	0 * SIZE(X)
	fmul	%st(1), %st
	FST	0 * SIZE(X)
	addq	INCX, X

	decq	I
	jg	.L305
	ALIGN_4

.L306:
	movq	M,  I
	andq	$7, I
	jle	.L999
	ALIGN_4

.L307:
	FLD	0 * SIZE(X)
	fmul	%st(1), %st
	FST	0 * SIZE(X)
	addq	INCX, X
	decq	I
	jg	.L307
	ALIGN_4

.L999:
	ffreep	%st(0)
	ret

	EPILOGUE
