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
	
#define STACK	 8
#define ARGS	 0
	
#define STACK_M		 4 + STACK + ARGS(%esp)
#define STACK_X	 	 8 + STACK + ARGS(%esp)
#define STACK_INCX	12 + STACK + ARGS(%esp)

	PROLOGUE

#define M	%ebx
#define INCX	%esi
#define X	%ecx
#define I	%edx

#ifndef USE_MIN
#define FMOV	fcmovbe
#else
#define FMOV	fcmovnbe
#endif

#include "l1param.h"

	pushl	%esi
	pushl	%ebx

	PROFCODE

	movl	STACK_M,    M
	movl	STACK_INCX, INCX
	movl	STACK_X,    X

#ifdef F_INTERFACE
	movl	(M), M
	movl	(INCX), INCX
#endif

#if defined(F_INTERFACE_GFORT) || defined(F_INTERFACE_G95)
	EMMS
#endif

	sall	$BASE_SHIFT, INCX

	fldz

	testl	M, M
	jle	.L999
	testl	INCX, INCX
	jle	.L999

	fstp	%st(0)

	FLD	(X)
#ifdef USE_ABS
	fabs	
#endif
	addl	INCX, X
	decl	M
	jle	.L999

	cmpl	$SIZE, INCX
	jne	.L40

	movl	M, I
	sarl	$3,   I
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
	fcomi	%st(1), %st
	FMOV	%st(1), %st(0)
	fstp	%st(1)

	FLD	1 * SIZE(X)
#ifdef USE_ABS
	fabs	
#endif
	fcomi	%st(1), %st
	FMOV	%st(1), %st(0)
	fstp	%st(1)

	FLD	2 * SIZE(X)
#ifdef USE_ABS
	fabs	
#endif
	fcomi	%st(1), %st
	FMOV	%st(1), %st(0)
	fstp	%st(1)

	FLD	3 * SIZE(X)
#ifdef USE_ABS
	fabs	
#endif
	fcomi	%st(1), %st
	FMOV	%st(1), %st(0)
	fstp	%st(1)

	FLD	4 * SIZE(X)
#ifdef USE_ABS
	fabs	
#endif
	fcomi	%st(1), %st
	FMOV	%st(1), %st(0)
	fstp	%st(1)

	FLD	5 * SIZE(X)
#ifdef USE_ABS
	fabs	
#endif
	fcomi	%st(1), %st
	FMOV	%st(1), %st(0)
	fstp	%st(1)

	FLD	6 * SIZE(X)
#ifdef USE_ABS
	fabs	
#endif
	fcomi	%st(1), %st
	FMOV	%st(1), %st(0)
	fstp	%st(1)

	FLD	7 * SIZE(X)
#ifdef USE_ABS
	fabs	
#endif
	fcomi	%st(1), %st
	FMOV	%st(1), %st(0)
	fstp	%st(1)

	addl	$8 * SIZE, X

	decl	I
	jg	.L10
	ALIGN_4

.L20:
	movl	M,  I
	andl	$7, I
	jle	.L999
	ALIGN_4


.L21:
	FLD	0 * SIZE(X)
#ifdef USE_ABS
	fabs	
#endif
	fcomi	%st(1), %st
	FMOV	%st(1), %st(0)
	fstp	%st(1)

	addl	$1 * SIZE, X
	decl	I
	jg	.L21
	jmp	.L999
	ALIGN_4

.L40:
	movl	M, I
	sarl	$3,   I
	jle	.L60
	ALIGN_4
	
.L50:
	FLD	0 * SIZE(X)
	addl	INCX, X
#ifdef USE_ABS
	fabs	
#endif
	fcomi	%st(1), %st
	FMOV	%st(1), %st(0)
	fstp	%st(1)

	FLD	0 * SIZE(X)
	addl	INCX, X
#ifdef USE_ABS
	fabs	
#endif
	fcomi	%st(1), %st
	FMOV	%st(1), %st(0)
	fstp	%st(1)

	FLD	0 * SIZE(X)
	addl	INCX, X
#ifdef USE_ABS
	fabs	
#endif
	fcomi	%st(1), %st
	FMOV	%st(1), %st(0)
	fstp	%st(1)

	FLD	0 * SIZE(X)
	addl	INCX, X
#ifdef USE_ABS
	fabs	
#endif
	fcomi	%st(1), %st
	FMOV	%st(1), %st(0)
	fstp	%st(1)

	FLD	0 * SIZE(X)
	addl	INCX, X
#ifdef USE_ABS
	fabs	
#endif
	fcomi	%st(1), %st
	FMOV	%st(1), %st(0)
	fstp	%st(1)

	FLD	0 * SIZE(X)
	addl	INCX, X
#ifdef USE_ABS
	fabs	
#endif
	fcomi	%st(1), %st
	FMOV	%st(1), %st(0)
	fstp	%st(1)

	FLD	0 * SIZE(X)
	addl	INCX, X
#ifdef USE_ABS
	fabs	
#endif
	fcomi	%st(1), %st
	FMOV	%st(1), %st(0)
	fstp	%st(1)

	FLD	0 * SIZE(X)
	addl	INCX, X
#ifdef USE_ABS
	fabs	
#endif
	fcomi	%st(1), %st
	FMOV	%st(1), %st(0)
	fstp	%st(1)

	decl	I
	jg	.L50
	ALIGN_4

.L60:
	movl	M,  I
	andl	$7, I
	jle	.L999
	ALIGN_4


.L61:
	FLD	0 * SIZE(X)
#ifdef USE_ABS
	fabs	
#endif
	fcomi	%st(1), %st
	FMOV	%st(1), %st(0)
	fstp	%st(1)

	addl	INCX, X
	decl	I
	jg	.L61
	ALIGN_4

.L999:
	popl	%ebx
	popl	%esi
	ret

	EPILOGUE
