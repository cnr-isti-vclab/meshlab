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
#define X	ARG2	/* rsi */
#define INCX	ARG3	/* rdx */

#define I	%rax
	
#ifdef USE_MIN
#define maxsd	minsd
#endif

#include "l1param.h"

	PROLOGUE
	PROFCODE

	SAVEREGISTERS

	xorps	%xmm0, %xmm0
	salq	$ZBASE_SHIFT, INCX

	testq	M, M
	jle	.L999

	testq	INCX, INCX
	jle	.L999

	pcmpeqb	%xmm15, %xmm15
	psrlq	$1, %xmm15

	movsd	0 * SIZE(X), %xmm0
	movsd	1 * SIZE(X), %xmm4
	addq	INCX, X

	andps	%xmm15, %xmm0
	andps	%xmm15, %xmm4

	addsd	%xmm4,  %xmm0
	decq	M
	jle	.L999

	movaps	 %xmm0, %xmm1

	cmpq	$2 * SIZE, INCX
	jne	.L20

	movq	M,  I
	sarq	$2, I
	jle	.L15

	movsd	0 * SIZE(X), %xmm4
	movsd	1 * SIZE(X), %xmm5
	movsd	2 * SIZE(X), %xmm6
	movsd	3 * SIZE(X), %xmm7

	movsd	4 * SIZE(X), %xmm8
	andps	%xmm15, %xmm4
	movsd	5 * SIZE(X), %xmm9
	andps	%xmm15, %xmm5
	movsd	6 * SIZE(X), %xmm10
	addsd	%xmm4,  %xmm5
	movsd	7 * SIZE(X), %xmm11
	decq	I
	jle	.L13
	ALIGN_4
	
.L12:
#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	andps	%xmm15, %xmm6
	movsd	 8 * SIZE(X), %xmm4
	andps	%xmm15, %xmm7
	addsd	%xmm6,  %xmm7
	movsd	10 * SIZE(X), %xmm6

	maxsd	%xmm5,  %xmm0
	movsd	 9 * SIZE(X), %xmm5
	andps	%xmm15, %xmm8
	maxsd	%xmm7,  %xmm1
	movsd	11 * SIZE(X), %xmm7
	andps	%xmm15, %xmm9
	addsd	%xmm8,  %xmm9
	movsd	12 * SIZE(X), %xmm8

	andps	%xmm15, %xmm10
	andps	%xmm15, %xmm11
	addsd	%xmm10, %xmm11
	movsd	14 * SIZE(X), %xmm10

	maxsd	%xmm9,  %xmm0
	movsd	13 * SIZE(X), %xmm9
	andps	%xmm15, %xmm4
	maxsd	%xmm11, %xmm1
	movsd	15 * SIZE(X), %xmm11
	andps	%xmm15, %xmm5
	addsd	%xmm4,  %xmm5

	addq	$8 * SIZE, X
	decq	I
	jg	.L12
	ALIGN_4

.L13:
	andps	%xmm15, %xmm6
	andps	%xmm15, %xmm7
	addsd	%xmm6,  %xmm7

	maxsd	%xmm5,  %xmm0
	andps	%xmm15, %xmm8
	maxsd	%xmm7,  %xmm1
	andps	%xmm15, %xmm9
	addsd	%xmm8,  %xmm9

	andps	%xmm15, %xmm10
	andps	%xmm15, %xmm11
	addsd	%xmm10, %xmm11

	maxsd	%xmm9,  %xmm0
	maxsd	%xmm11, %xmm1

	addq	$8 * SIZE, X
	ALIGN_4

.L15:
	testq	$2, M
	jle	.L17

	movsd	0 * SIZE(X), %xmm4
	movsd	1 * SIZE(X), %xmm5
	movsd	2 * SIZE(X), %xmm6
	movsd	3 * SIZE(X), %xmm7
	addq	$4 * SIZE, X

	andps	%xmm15, %xmm4
	andps	%xmm15, %xmm5
	addsd	%xmm4,  %xmm5

	andps	%xmm15, %xmm6
	andps	%xmm15, %xmm7
	addsd	%xmm6,  %xmm7

	maxsd	%xmm5,  %xmm0
	maxsd	%xmm7,  %xmm1
	ALIGN_3	

.L17:
	testq	$1, M
	jle	.L998

	movsd	0 * SIZE(X), %xmm4
	movsd	1 * SIZE(X), %xmm5

	andps	%xmm15, %xmm4
	andps	%xmm15, %xmm5

	addsd	%xmm4,  %xmm5
	maxsd	%xmm5,  %xmm0
	jmp	.L998
	ALIGN_3

.L20:
	movq	M,  I
	sarq	$2, I
	jle	.L25

	movsd	0 * SIZE(X), %xmm4
	movsd	1 * SIZE(X), %xmm5
	addq	INCX, X
	movsd	0 * SIZE(X), %xmm6
	movsd	1 * SIZE(X), %xmm7
	addq	INCX, X

	movsd	0 * SIZE(X), %xmm8
	andps	%xmm15, %xmm4
	movsd	1 * SIZE(X), %xmm9
	addq	INCX, X
	andps	%xmm15, %xmm5
	movsd	0 * SIZE(X), %xmm10
	addsd	%xmm4,  %xmm5
	movsd	1 * SIZE(X), %xmm11
	addq	INCX, X

	decq	I
	jle	.L23
	ALIGN_4
	
.L22:
#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	andps	%xmm15, %xmm6
	movsd	 0 * SIZE(X), %xmm4
	andps	%xmm15, %xmm7
	addsd	%xmm6,  %xmm7

	maxsd	%xmm5,  %xmm0
	movsd	 1 * SIZE(X), %xmm5
	andps	%xmm15, %xmm8
	addq	INCX, X
	maxsd	%xmm7,  %xmm1
	movsd	 0 * SIZE(X), %xmm6
	andps	%xmm15, %xmm9
	movsd	 1 * SIZE(X), %xmm7
	addsd	%xmm8,  %xmm9
	addq	INCX, X

	andps	%xmm15, %xmm10
	movsd	 0 * SIZE(X), %xmm8
	andps	%xmm15, %xmm11
	addsd	%xmm10, %xmm11

	maxsd	%xmm9,  %xmm0
	movsd	 1 * SIZE(X), %xmm9
	addq	INCX, X
	andps	%xmm15, %xmm4
	movsd	 0 * SIZE(X), %xmm10
	maxsd	%xmm11, %xmm1
	movsd	 1 * SIZE(X), %xmm11
	andps	%xmm15, %xmm5
	addq	INCX, X
	addsd	%xmm4,  %xmm5

	decq	I
	jg	.L22
	ALIGN_4

.L23:
	andps	%xmm15, %xmm6
	andps	%xmm15, %xmm7
	addsd	%xmm6,  %xmm7

	maxsd	%xmm5,  %xmm0
	andps	%xmm15, %xmm8
	maxsd	%xmm7,  %xmm1
	andps	%xmm15, %xmm9
	addsd	%xmm8,  %xmm9

	andps	%xmm15, %xmm10
	andps	%xmm15, %xmm11
	addsd	%xmm10, %xmm11

	maxsd	%xmm9,  %xmm0
	maxsd	%xmm11, %xmm1
	ALIGN_4

.L25:
	testq	$2, M
	jle	.L27

	movsd	0 * SIZE(X), %xmm4
	movsd	1 * SIZE(X), %xmm5
	addq	INCX, X
	movsd	0 * SIZE(X), %xmm6
	movsd	1 * SIZE(X), %xmm7
	addq	INCX, X

	andps	%xmm15, %xmm4
	andps	%xmm15, %xmm5
	addsd	%xmm4,  %xmm5

	andps	%xmm15, %xmm6
	andps	%xmm15, %xmm7
	addsd	%xmm6,  %xmm7

	maxsd	%xmm5,  %xmm0
	maxsd	%xmm7,  %xmm1
	ALIGN_3	

.L27:
	testq	$1, M
	jle	.L998

	movsd	0 * SIZE(X), %xmm4
	movsd	1 * SIZE(X), %xmm5

	andps	%xmm15, %xmm4
	andps	%xmm15, %xmm5

	addsd	%xmm4,  %xmm5
	maxsd	%xmm5,  %xmm0
	ALIGN_3

.L998:
	maxsd	%xmm1, %xmm0
	ALIGN_4

.L999:

	RESTOREREGISTERS

	ret

	EPILOGUE
