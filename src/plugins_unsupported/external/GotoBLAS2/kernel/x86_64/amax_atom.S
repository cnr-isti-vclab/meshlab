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
	leaq	(, INCX, SIZE), INCX

	testq	M, M
	jle	.L999

	testq	INCX, INCX
	jle	.L999

#ifdef USE_ABS
	pcmpeqb	%xmm15, %xmm15
	psrlq	$1, %xmm15
#endif

	movsd	(X), %xmm0
	addq	INCX, X

#ifdef USE_ABS
	andps	 %xmm15, %xmm0
#endif
	decq	M
	jle	.L999

	movaps	 %xmm0, %xmm1
	movaps	 %xmm0, %xmm2
	movaps	 %xmm0, %xmm3

	cmpq	$SIZE, INCX
	jne	.L20

	movq	M,  I
	sarq	$3, I
	jle	.L15

	movsd	0 * SIZE(X), %xmm4
	movsd	1 * SIZE(X), %xmm5
	movsd	2 * SIZE(X), %xmm6
	movsd	3 * SIZE(X), %xmm7

	movsd	4 * SIZE(X), %xmm8
	movsd	5 * SIZE(X), %xmm9
	movsd	6 * SIZE(X), %xmm10
	movsd	7 * SIZE(X), %xmm11

	decq	I
	jle	.L13
	ALIGN_4
	
.L12:
#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

#ifdef USE_ABS
	andps	%xmm15, %xmm4
#endif
	maxsd	%xmm4,  %xmm1
	movsd	 8 * SIZE(X), %xmm4

#ifdef USE_ABS
	andps	%xmm15, %xmm5
#endif
	maxsd	%xmm5,  %xmm2
	movsd	 9 * SIZE(X), %xmm5

#ifdef USE_ABS
	andps	%xmm15, %xmm6
#endif
	maxsd	%xmm6,  %xmm1
	movsd	10 * SIZE(X), %xmm6

#ifdef USE_ABS
	andps	%xmm15, %xmm7
#endif
	maxsd	%xmm7,  %xmm2
	movsd	11 * SIZE(X), %xmm7

#ifdef USE_ABS
	andps	%xmm15, %xmm8
#endif
	maxsd	%xmm8,  %xmm1
	movsd	12 * SIZE(X), %xmm8

#ifdef USE_ABS
	andps	%xmm15, %xmm9
#endif
	maxsd	%xmm9,  %xmm2
	movsd	13 * SIZE(X), %xmm9

#ifdef USE_ABS
	andps	%xmm15, %xmm10
#endif
	maxsd	%xmm10, %xmm1
	movsd	14 * SIZE(X), %xmm10

#ifdef USE_ABS
	andps	%xmm15, %xmm11
#endif
	maxsd	%xmm11, %xmm2
	movsd	15 * SIZE(X), %xmm11

	addq	$8 * SIZE, X
	decq	I
	jg	.L12
	ALIGN_4

.L13:
#ifdef USE_ABS
	andps	%xmm15, %xmm4
#endif
	maxsd	%xmm4,  %xmm0
#ifdef USE_ABS
	andps	%xmm15, %xmm5
#endif
	maxsd	%xmm5,  %xmm1
#ifdef USE_ABS
	andps	%xmm15, %xmm6
#endif
	maxsd	%xmm6,  %xmm2
#ifdef USE_ABS
	andps	%xmm15, %xmm7
#endif
	maxsd	%xmm7,  %xmm3

#ifdef USE_ABS
	andps	%xmm15, %xmm8
#endif
	maxsd	%xmm8,  %xmm0
#ifdef USE_ABS
	andps	%xmm15, %xmm9
#endif
	maxsd	%xmm9,  %xmm1
#ifdef USE_ABS
	andps	%xmm15, %xmm10
#endif
	maxsd	%xmm10, %xmm2
#ifdef USE_ABS
	andps	%xmm15, %xmm11
#endif
	maxsd	%xmm11, %xmm3

	addq	$8 * SIZE, X
	ALIGN_4

.L15:
	testq	$4, M
	jle	.L17

	movsd	0 * SIZE(X), %xmm4
	movsd	1 * SIZE(X), %xmm5
	movsd	2 * SIZE(X), %xmm6
	movsd	3 * SIZE(X), %xmm7

#ifdef USE_ABS
	andps	%xmm15, %xmm4
#endif
	maxsd	%xmm4,  %xmm0
#ifdef USE_ABS
	andps	%xmm15, %xmm5
#endif
	maxsd	%xmm5,  %xmm1
#ifdef USE_ABS
	andps	%xmm15, %xmm6
#endif
	maxsd	%xmm6,  %xmm2
#ifdef USE_ABS
	andps	%xmm15, %xmm7
#endif
	maxsd	%xmm7,  %xmm3

	addq	$4 * SIZE, X
	ALIGN_3	

.L17:
	testq	$2, M
	jle	.L18

	movsd	0 * SIZE(X), %xmm4
	movsd	1 * SIZE(X), %xmm5

#ifdef USE_ABS
	andps	 %xmm15, %xmm4
#endif
	maxsd	%xmm4, %xmm1
#ifdef USE_ABS
	andps	 %xmm15, %xmm5
#endif
	maxsd	%xmm5, %xmm2
	addq	$2 * SIZE, X
	ALIGN_3
	
.L18:
	testq	$1, M
	jle	.L998

	movsd	0 * SIZE(X), %xmm4
#ifdef USE_ABS
	andps	 %xmm15, %xmm4
#endif
	maxsd	%xmm4, %xmm3
	jmp	.L998
	ALIGN_3

.L20:
	movq	M,  I
	sarq	$3, I
	jle	.L25

	movsd	(X), %xmm4
	addq	INCX, X
	movsd	(X), %xmm5
	addq	INCX, X
	movsd	(X), %xmm6
	addq	INCX, X
	movsd	(X), %xmm7
	addq	INCX, X

	movsd	(X), %xmm8
	addq	INCX, X
	movsd	(X), %xmm9
	addq	INCX, X
	movsd	(X), %xmm10
	addq	INCX, X
	movsd	(X), %xmm11

	decq	I
	jle	.L23
	ALIGN_4
	
.L22:
#ifdef USE_ABS
	andps	%xmm15, %xmm4
#endif
	addq	INCX, X
	maxsd	%xmm4,  %xmm1
	movsd	(X), %xmm4

#ifdef USE_ABS
	andps	%xmm15, %xmm5
#endif
	addq	INCX, X
	maxsd	%xmm5,  %xmm2
	movsd	(X), %xmm5

#ifdef USE_ABS
	andps	%xmm15, %xmm6
#endif
	addq	INCX, X
	maxsd	%xmm6,  %xmm1
	movsd	(X), %xmm6

#ifdef USE_ABS
	andps	%xmm15, %xmm7
#endif
	addq	INCX, X
	maxsd	%xmm7,  %xmm2
	movsd	(X), %xmm7

#ifdef USE_ABS
	andps	%xmm15, %xmm8
#endif
	addq	INCX, X
	maxsd	%xmm8,  %xmm1
	movsd	(X), %xmm8

#ifdef USE_ABS
	andps	%xmm15, %xmm9
#endif
	addq	INCX, X
	maxsd	%xmm9,  %xmm2
	movsd	(X), %xmm9

#ifdef USE_ABS
	andps	%xmm15, %xmm10
#endif
	addq	INCX, X
	maxsd	%xmm10, %xmm1
	movsd	(X), %xmm10

#ifdef USE_ABS
	andps	%xmm15, %xmm11
#endif
	addq	INCX, X
	maxsd	%xmm11, %xmm2
	movsd	(X), %xmm11

	decq	I
	jg	.L22
	ALIGN_4

.L23:
#ifdef USE_ABS
	andps	%xmm15, %xmm4
#endif
	addq	INCX, X
	maxsd	%xmm4,  %xmm0
#ifdef USE_ABS
	andps	%xmm15, %xmm5
#endif
	maxsd	%xmm5,  %xmm1
#ifdef USE_ABS
	andps	%xmm15, %xmm6
#endif
	maxsd	%xmm6,  %xmm2
#ifdef USE_ABS
	andps	%xmm15, %xmm7
#endif
	maxsd	%xmm7,  %xmm3

#ifdef USE_ABS
	andps	%xmm15, %xmm8
#endif
	maxsd	%xmm8,  %xmm0
#ifdef USE_ABS
	andps	%xmm15, %xmm9
#endif
	maxsd	%xmm9,  %xmm1
#ifdef USE_ABS
	andps	%xmm15, %xmm10
#endif
	maxsd	%xmm10, %xmm2
#ifdef USE_ABS
	andps	%xmm15, %xmm11
#endif
	maxsd	%xmm11, %xmm3
	ALIGN_4

.L25:
	testq	$4, M
	jle	.L27

	movsd	(X), %xmm4
	addq	INCX, X
	movsd	(X), %xmm5
	addq	INCX, X
	movsd	(X), %xmm6
	addq	INCX, X
	movsd	(X), %xmm7
	addq	INCX, X

#ifdef USE_ABS
	andps	%xmm15, %xmm4
#endif
	maxsd	%xmm4,  %xmm0
#ifdef USE_ABS
	andps	%xmm15, %xmm5
#endif
	maxsd	%xmm5,  %xmm1
#ifdef USE_ABS
	andps	%xmm15, %xmm6
#endif
	maxsd	%xmm6,  %xmm2
#ifdef USE_ABS
	andps	%xmm15, %xmm7
#endif
	maxsd	%xmm7,  %xmm3
	ALIGN_3	

.L27:
	testq	$2, M
	jle	.L28

	movsd	(X), %xmm4
	addq	INCX, X
	movsd	(X), %xmm5
	addq	INCX, X

#ifdef USE_ABS
	andps	 %xmm15, %xmm4
#endif
	maxsd	%xmm4, %xmm1
#ifdef USE_ABS
	andps	 %xmm15, %xmm5
#endif
	maxsd	%xmm5, %xmm2
	ALIGN_3
	
.L28:
	testq	$1, M
	jle	.L998

	movsd	(X), %xmm4
	addq	INCX, X
#ifdef USE_ABS
	andps	 %xmm15, %xmm4
#endif
	maxsd	%xmm4, %xmm3
	ALIGN_3

.L998:
	maxsd	%xmm1, %xmm0
	maxsd	%xmm3, %xmm2
	maxsd	%xmm2, %xmm0
	ALIGN_4

.L999:

	RESTOREREGISTERS

	ret

	EPILOGUE
