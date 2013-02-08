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
#define STACK_X		 8 + STACK + ARGS(%esp)
#define STACK_INCX	12 + STACK + ARGS(%esp)

#define M	%edx
#define X	%ecx
#define INCX	%esi
	
#define I	%eax

#include "l1param.h"

	PROLOGUE

	pushl	%esi
	pushl	%ebx

	PROFCODE

#if defined(F_INTERFACE_GFORT) || defined(F_INTERFACE_G95)
	EMMS
#endif

	movl	STACK_M,    M
	movl	STACK_X,    X
	movl	STACK_INCX, INCX

#ifdef F_INTERFACE
	movl	(M),    M
	movl	(INCX), INCX
#endif

	fldz
	testl	M,  M
	jle	.L999
	testl	INCX, INCX
	jle	.L999

	sall	$ZBASE_SHIFT, INCX

	fldz
	fldz
	fldz
	cmpl	$SIZE * 2, INCX
	jne	.L40

	movl	M,  I
	sarl	$2, I
	jle	.L20
	ALIGN_4
	
.L10:
#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	FLD	0 * SIZE(X)
	fabs
	FLD	1 * SIZE(X)
	fabs
	FLD	2 * SIZE(X)
	fabs
	FLD	3 * SIZE(X)
	fabs

	faddp	%st, %st(7)
	faddp	%st, %st(5)
	faddp	%st, %st(3)
	faddp	%st, %st(1)

	FLD	4 * SIZE(X)
	fabs
	FLD	5 * SIZE(X)
	fabs
	FLD	6 * SIZE(X)
	fabs
	FLD	7 * SIZE(X)
	fabs

	addl	$8 * SIZE, X

	faddp	%st, %st(7)
	faddp	%st, %st(5)
	faddp	%st, %st(3)
	faddp	%st, %st(1)

	decl	I
	jg	.L10
	ALIGN_4

.L20:
	movl	M,  I
	andl	$3, I
	jle	.L998
	ALIGN_4


.L21:
	FLD	0 * SIZE(X)
	fabs
	FLD	1 * SIZE(X)
	fabs
	faddp	%st,%st(3)
	faddp	%st,%st(1)
	addl	$2 * SIZE, X
	decl	I
	jg	.L21
	jmp	.L998
	ALIGN_4

.L40:
	movl	M,  I
	sarl	$2, I
	jle	.L60
	ALIGN_4
	
.L50:
	FLD	0 * SIZE(X)
	fabs
	FLD	1 * SIZE(X)
	addl	INCX, X
	fabs
	FLD	0 * SIZE(X)
	fabs
	FLD	1 * SIZE(X)
	addl	INCX, X
	fabs

	faddp	%st, %st(7)
	faddp	%st, %st(5)
	faddp	%st, %st(3)
	faddp	%st, %st(1)

	FLD	0 * SIZE(X)
	fabs
	FLD	1 * SIZE(X)
	addl	INCX, X
	fabs
	FLD	0 * SIZE(X)
	fabs
	FLD	1 * SIZE(X)
	addl	INCX, X
	fabs

	faddp	%st, %st(7)
	faddp	%st, %st(5)
	faddp	%st, %st(3)
	faddp	%st, %st(1)

	decl	I
	jg	.L50
	ALIGN_4

.L60:
	movl	M,  I
	andl	$3, I
	jle	.L998
	ALIGN_4


.L61:
	FLD	0 * SIZE(X)
	fabs
	FLD	1 * SIZE(X)
	addl	INCX, X
	fabs
	faddp	%st,%st(3)
	faddp	%st,%st(1)
	decl	I
	jg	.L61
	ALIGN_4

.L998:
	faddp	%st,%st(2)
	faddp	%st,%st(1)
	faddp	%st,%st(1)
	ALIGN_4

.L999:
	popl	%ebx
	popl	%esi
	ret

	EPILOGUE
