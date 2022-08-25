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

#define I	%rax

#include "l1param.h"

	PROLOGUE
	PROFCODE

	fldz
	testq	M, M
	jle	.L999
	testq	INCX, INCX
	jle	.L999

	salq	$BASE_SHIFT, INCX

	fldz
	fldz
	fldz
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

	addq	$8 * SIZE, X

	faddp	%st, %st(7)
	faddp	%st, %st(5)
	faddp	%st, %st(3)
	faddp	%st, %st(1)

	decq	I
	jg	.L10
	ALIGN_4

.L20:
	andq	$7,  M
	jle	.L998
	ALIGN_4

.L21:
	FLD	(X)
	fabs
	faddp	%st,%st(1)
	addq	$1 * SIZE, X
	decq	M
	jg	.L21
	jmp	.L998
	ALIGN_4

.L40:
	movq	M, I
	sarq	$3,   I
	jle	.L60
	ALIGN_4
	
.L50:
	FLD	(X)
	addq	INCX, X
	fabs
	FLD	(X)
	addq	INCX, X
	fabs
	FLD	(X)
	addq	INCX, X
	fabs
	FLD	(X)
	addq	INCX, X
	fabs

	faddp	%st, %st(7)
	faddp	%st, %st(5)
	faddp	%st, %st(3)
	faddp	%st, %st(1)

	FLD	(X)
	addq	INCX, X
	fabs
	FLD	(X)
	addq	INCX, X
	fabs
	FLD	(X)
	addq	INCX, X
	fabs
	FLD	(X)
	addq	INCX, X
	fabs

	faddp	%st, %st(7)
	faddp	%st, %st(5)
	faddp	%st, %st(3)
	faddp	%st, %st(1)

	decq	I
	jg	.L50
	ALIGN_4

.L60:
	andq	$7,  M
	jle	.L998
	ALIGN_4


.L61:
	FLD	(X)
	addq	INCX, X
	fabs
	faddp	%st,%st(1)
	decq	M
	jg	.L61
	ALIGN_4

.L998:
	faddp	%st,%st(2)
	faddp	%st,%st(1)
	faddp	%st,%st(1)
	ALIGN_4

.L999:
	ret

	EPILOGUE
