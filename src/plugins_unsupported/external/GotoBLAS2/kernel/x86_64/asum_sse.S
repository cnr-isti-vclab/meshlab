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
	
#include "l1param.h"

	PROLOGUE
	PROFCODE

	SAVEREGISTERS
	
	xorps	%xmm0, %xmm0
	testq	M, M
	jle	.L999
	testq	INCX, INCX
	jle	.L999

	xorps	%xmm1, %xmm1
	xorps	%xmm2, %xmm2
	xorps	%xmm3, %xmm3

	pcmpeqb	%xmm15, %xmm15
	psrld	$1, %xmm15

	leaq	(, INCX, SIZE), INCX

	cmpq	$SIZE, INCX
	jne	.L100

	subq	$-32 * SIZE, X

	cmpq	$3, M
	jle	.L18

	testq	$4, X
	je	.L05
	movss	-32 * SIZE(X), %xmm0
	andps	%xmm15, %xmm0
	addq	$SIZE, X
	decq	M
	jle	.L998
	ALIGN_3

.L05:
	testq	$8, X
	je	.L10

	movsd	-32 * SIZE(X), %xmm1
	andps	%xmm15, %xmm1
	addq	$2 * SIZE, X
	subq	$2, M
	jle	.L998
	ALIGN_3

.L10:
	movq	M,  I
	sarq	$5, I
	jle	.L14

	movaps	-32 * SIZE(X), %xmm4
	movaps	-28 * SIZE(X), %xmm5
	movaps	-24 * SIZE(X), %xmm6
	movaps	-20 * SIZE(X), %xmm7

	movaps	-16 * SIZE(X), %xmm8
	movaps	-12 * SIZE(X), %xmm9
	movaps	 -8 * SIZE(X), %xmm10
	movaps	 -4 * SIZE(X), %xmm11
	decq	I
	jle	.L12
	ALIGN_3
	
.L11:
#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	andps	%xmm15, %xmm4
	addps	%xmm4,  %xmm0
	movaps	  0 * SIZE(X), %xmm4

	andps	%xmm15, %xmm5
	addps	%xmm5,  %xmm1
	movaps	  4 * SIZE(X), %xmm5

	andps	%xmm15, %xmm6
	addps	%xmm6,  %xmm2
	movaps	  8 * SIZE(X), %xmm6

	andps	%xmm15, %xmm7
	addps	%xmm7,  %xmm3
	movaps	 12 * SIZE(X), %xmm7

#if defined(PREFETCH) && !defined(FETCH128)
	PREFETCH (PREFETCHSIZE +  64) - PREOFFSET(X)
#endif

	andps	%xmm15, %xmm8
	addps	%xmm8,  %xmm0
	movaps	 16 * SIZE(X), %xmm8

	andps	%xmm15, %xmm9
	addps	%xmm9,  %xmm1
	movaps	 20 * SIZE(X), %xmm9

	andps	%xmm15, %xmm10
	addps	%xmm10, %xmm2
	movaps	 24 * SIZE(X), %xmm10

	andps	%xmm15, %xmm11
	addps	%xmm11, %xmm3
	movaps	 28 * SIZE(X), %xmm11

	subq	$-32 * SIZE, X
	decq	I
	jg	.L11
	ALIGN_3

.L12:
	andps	%xmm15, %xmm4
	addps	%xmm4,  %xmm0
	andps	%xmm15, %xmm5
	addps	%xmm5,  %xmm1

	andps	%xmm15, %xmm6
	addps	%xmm6,  %xmm2
	andps	%xmm15, %xmm7
	addps	%xmm7,  %xmm3

	andps	%xmm15, %xmm8
	addps	%xmm8,  %xmm0
	andps	%xmm15, %xmm9
	addps	%xmm9,  %xmm1

	andps	%xmm15, %xmm10
	addps	%xmm10, %xmm2
	andps	%xmm15, %xmm11
	addps	%xmm11, %xmm3

	subq	$-32 * SIZE, X
	ALIGN_3

.L14:
	testq	$16, M
	je	.L16

	movaps	-32 * SIZE(X), %xmm4
	andps	%xmm15, %xmm4
	addps	%xmm4, %xmm0

	movaps	-28 * SIZE(X), %xmm5
	andps	%xmm15, %xmm5
	addps	%xmm5, %xmm1

	movaps	-24 * SIZE(X), %xmm4
	andps	%xmm15, %xmm4
	addps	%xmm4, %xmm0

	movaps	-20 * SIZE(X), %xmm5
	andps	%xmm15, %xmm5
	addps	%xmm5, %xmm1

	addq	$16 * SIZE, X
	ALIGN_3

.L16:
	testq	$8, M
	je	.L17

	movaps	-32 * SIZE(X), %xmm4
	andps	%xmm15, %xmm4
	addps	%xmm4, %xmm0

	movaps	-28 * SIZE(X), %xmm5
	andps	%xmm15, %xmm5
	addps	%xmm5, %xmm1

	addq	$8 * SIZE, X
	ALIGN_3

.L17:
	testq	$4, M
	je	.L18

	movaps	-32 * SIZE(X), %xmm6
	andps	%xmm15, %xmm6
	addps	%xmm6, %xmm2
	addq	$4 * SIZE, X
	ALIGN_3

.L18:
	testq	$2, M
	je	.L19

#ifdef movsd
	xorps	%xmm7, %xmm7
#endif
	movsd	-32 * SIZE(X), %xmm7
	andps	%xmm15, %xmm7
	addps	%xmm7, %xmm3
	addq	$2 * SIZE, X
	ALIGN_3

.L19:
	testq	$1, M
	je	.L998

	movss	-32 * SIZE(X), %xmm6
	andps	%xmm15, %xmm6
	addps	%xmm6, %xmm2
	jmp	.L998
	ALIGN_4

.L100:
	movq	M,  I
	sarq	$3, I
	jle	.L105
	ALIGN_4
	
.L101:
	movss	0 * SIZE(X), %xmm4
	addq	INCX, X
	andps	%xmm15, %xmm4
	addss	%xmm4, %xmm0

	movss	0 * SIZE(X), %xmm5
	addq	INCX, X
	andps	%xmm15, %xmm5
	addss	%xmm5, %xmm1

	movss	0 * SIZE(X), %xmm6
	addq	INCX, X
	andps	%xmm15, %xmm6
	addss	%xmm6, %xmm2

	movss	0 * SIZE(X), %xmm7
	addq	INCX, X
	andps	%xmm15, %xmm7
	addss	%xmm7, %xmm3

	movss	0 * SIZE(X), %xmm8
	addq	INCX, X
	andps	%xmm15, %xmm8
	addss	%xmm8, %xmm0

	movss	0 * SIZE(X), %xmm4
	addq	INCX, X
	andps	%xmm15, %xmm4
	addss	%xmm4, %xmm1

	movss	0 * SIZE(X), %xmm5
	addq	INCX, X
	andps	%xmm15, %xmm5
	addss	%xmm5, %xmm2

	movss	0 * SIZE(X), %xmm6
	addq	INCX, X
	andps	%xmm15, %xmm6
	addss	%xmm6, %xmm3

	decq	I
	jg	.L101
	ALIGN_4

.L105:
	andq	$7,  M
	jle	.L998
	ALIGN_4

.L106:
	movss	0 * SIZE(X), %xmm4
	andps	%xmm15, %xmm4
	addps	%xmm4, %xmm0
	addq	INCX, X
	decq	M
	jg	.L106
	ALIGN_4

.L998:
	addps	%xmm1, %xmm0
	addps	%xmm3, %xmm2
	addps	%xmm2, %xmm0

#ifndef HAVE_SSE3
	movhlps	%xmm0, %xmm1
	addps	%xmm1, %xmm0
	
	movaps	%xmm0, %xmm1
	shufps  $1, %xmm0, %xmm0
	addss	 %xmm1, %xmm0
#else
	haddps	%xmm0, %xmm0
	haddps	%xmm0, %xmm0
#endif
	ALIGN_4

.L999:
	RESTOREREGISTERS
	
	ret

	EPILOGUE
