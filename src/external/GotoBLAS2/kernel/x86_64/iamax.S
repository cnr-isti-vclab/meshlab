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
#define X	ARG2
#define INCX	ARG3

#define RET	%rax
#define I	ARG4
#define NUM	%r10

#ifndef USE_MIN
#define FMOV	fcmovbe
#define IMOV	cmovnbe
#else
#define FMOV	fcmovnbe
#define IMOV	cmovb
#endif

#include "l1param.h"

	PROLOGUE
	PROFCODE

	salq	$BASE_SHIFT, INCX

	fldz
	xorq	RET, RET

	testq	M, M
	jle	.L999
	testq	INCX, INCX
	jle	.L999

	ffreep	%st
	movq	$2, NUM
	movq	$1, RET

	FLD	(X)
#ifdef USE_ABS
	fabs	
#endif
	addq	INCX, X
	decq	M
	jle	.L999

	cmpq	$SIZE, INCX
	jne	.L40

	movq	M, I
	sarq	$3,   I
	jle	.L20
	ALIGN_4
	
.L10:
#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	FLD	0 * SIZE(X)
#ifdef USE_ABS
	fabs	
#endif
	fcomi
	FMOV	%st(1), %st(0)
	IMOV	NUM, RET
	fxch	%st(1)
	ffreep	%st
	incq	NUM

	FLD	1 * SIZE(X)
#ifdef USE_ABS
	fabs	
#endif
	fcomi
	FMOV	%st(1), %st(0)
	IMOV	NUM, RET
	fxch	%st(1)
	ffreep	%st
	incq	NUM

	FLD	2 * SIZE(X)
#ifdef USE_ABS
	fabs	
#endif
	fcomi
	FMOV	%st(1), %st(0)
	IMOV	NUM, RET
	fxch	%st(1)
	ffreep	%st
	incq	NUM

	FLD	3 * SIZE(X)
#ifdef USE_ABS
	fabs	
#endif
	fcomi
	FMOV	%st(1), %st(0)
	IMOV	NUM, RET
	fxch	%st(1)
	ffreep	%st
	incq	NUM

	FLD	4 * SIZE(X)
#ifdef USE_ABS
	fabs	
#endif
	fcomi
	FMOV	%st(1), %st(0)
	IMOV	NUM, RET
	fxch	%st(1)
	ffreep	%st
	incq	NUM

	FLD	5 * SIZE(X)
#ifdef USE_ABS
	fabs	
#endif
	fcomi
	FMOV	%st(1), %st(0)
	IMOV	NUM, RET
	fxch	%st(1)
	ffreep	%st
	incq	NUM

	FLD	6 * SIZE(X)
#ifdef USE_ABS
	fabs	
#endif
	fcomi
	FMOV	%st(1), %st(0)
	IMOV	NUM, RET
	fxch	%st(1)
	ffreep	%st
	incq	NUM

	FLD	7 * SIZE(X)
#ifdef USE_ABS
	fabs	
#endif
	fcomi
	FMOV	%st(1), %st(0)
	IMOV	NUM, RET
	fxch	%st(1)
	ffreep	%st
	incq	NUM

	addq	$8 * SIZE, X

	decq	I
	jg	.L10
	ALIGN_4

.L20:
	movq	M,  I
	andq	$7, I
	jle	.L999
	ALIGN_4


.L21:
	FLD	0 * SIZE(X)
#ifdef USE_ABS
	fabs	
#endif
	fcomi
	FMOV	%st(1), %st(0)
	IMOV	NUM, RET
	fxch	%st(1)
	ffreep	%st

	addq	$1 * SIZE, X
	incq	NUM
	decq	I
	jg	.L21
	jmp	.L999
	ALIGN_4

.L40:
	movq	M, I
	sarq	$3,   I
	jle	.L60
	ALIGN_4
	
.L50:
	FLD	0 * SIZE(X)
	addq	INCX, X
#ifdef USE_ABS
	fabs	
#endif
	fcomi
	FMOV	%st(1), %st(0)
	IMOV	NUM, RET
	fxch	%st(1)
	ffreep	%st
	incq	NUM

	FLD	0 * SIZE(X)
	addq	INCX, X
#ifdef USE_ABS
	fabs	
#endif
	fcomi
	FMOV	%st(1), %st(0)
	IMOV	NUM, RET
	fxch	%st(1)
	ffreep	%st
	incq	NUM

	FLD	0 * SIZE(X)
	addq	INCX, X
#ifdef USE_ABS
	fabs	
#endif
	fcomi
	FMOV	%st(1), %st(0)
	IMOV	NUM, RET
	fxch	%st(1)
	ffreep	%st
	incq	NUM

	FLD	0 * SIZE(X)
	addq	INCX, X
#ifdef USE_ABS
	fabs	
#endif
	fcomi
	FMOV	%st(1), %st(0)
	IMOV	NUM, RET
	fxch	%st(1)
	ffreep	%st
	incq	NUM

	FLD	0 * SIZE(X)
	addq	INCX, X
#ifdef USE_ABS
	fabs	
#endif
	fcomi
	FMOV	%st(1), %st(0)
	IMOV	NUM, RET
	fxch	%st(1)
	ffreep	%st
	incq	NUM

	FLD	0 * SIZE(X)
	addq	INCX, X
#ifdef USE_ABS
	fabs	
#endif
	fcomi
	FMOV	%st(1), %st(0)
	IMOV	NUM, RET
	fxch	%st(1)
	ffreep	%st
	incq	NUM

	FLD	0 * SIZE(X)
	addq	INCX, X
#ifdef USE_ABS
	fabs	
#endif
	fcomi
	FMOV	%st(1), %st(0)
	IMOV	NUM, RET
	fxch	%st(1)
	ffreep	%st
	incq	NUM

	FLD	0 * SIZE(X)
	addq	INCX, X
#ifdef USE_ABS
	fabs	
#endif
	fcomi
	FMOV	%st(1), %st(0)
	IMOV	NUM, RET
	fxch	%st(1)
	ffreep	%st
	incq	NUM

	decq	I
	jg	.L50
	ALIGN_4

.L60:
	movq	M,  I
	andq	$7, I
	jle	.L999
	ALIGN_4


.L61:
	FLD	0 * SIZE(X)
#ifdef USE_ABS
	fabs	
#endif
	fcomi
	FMOV	%st(1), %st(0)
	IMOV	NUM, RET
	fxch	%st(1)
	ffreep	%st
	incq	NUM

	addq	INCX, X
	decq	I
	jg	.L61
	ALIGN_4

.L999:
	ffreep	%st
	ret

	EPILOGUE
