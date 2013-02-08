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
#define ARGS     0

#define STACK_M		 4 + STACK + ARGS(%esp)
#define STACK_X		 8 + STACK + ARGS(%esp)
#define STACK_INCX	12 + STACK + ARGS(%esp)

#define I	%eax
#define M	%ecx
#define X	%esi
#define INCX	%ebx

#include "l1param.h"

	PROLOGUE
	PROFCODE

	pushl	%esi
	pushl	%ebx

	movl	STACK_M, M
	movl	STACK_X, X
	movl	STACK_INCX, INCX

	xorps	%xmm0, %xmm0
	testl	M, M
	jle	.L999
	testl	INCX, INCX
	jle	.L999

	xorps	%xmm1, %xmm1

#ifdef HAVE_SSE2
	pcmpeqb	%xmm3, %xmm3
	psrld	$1, %xmm3
#else
	movl	$0x7fffffff, STACK_M
	movss	STACK_M, %xmm3
	shufps	$0, %xmm3, %xmm3
#endif

	leal	(, INCX, SIZE), INCX

	cmpl	$SIZE, INCX
	jne	.L100

	subl	$-32 * SIZE, X

	cmpl	$3, M
	jle	.L18

	testl	$4, X
	je	.L05
	movss	-32 * SIZE(X), %xmm0
	andps	%xmm3, %xmm0
	addl	$SIZE, X
	decl	M
	jle	.L998
	ALIGN_3

.L05:
	testl	$8, X
	je	.L10

	movsd	-32 * SIZE(X), %xmm1
	andps	%xmm3, %xmm1
	addl	$2 * SIZE, X
	subl	$2, M
	jle	.L998
	ALIGN_3

.L10:
	movl	M,  I
	sarl	$5, I
	jle	.L14

	movaps	-32 * SIZE(X), %xmm4
	movaps	-28 * SIZE(X), %xmm5
	movaps	-24 * SIZE(X), %xmm6
	movaps	-20 * SIZE(X), %xmm7

	decl	I
	jle	.L12
	ALIGN_3
	
.L11:
#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	andps	%xmm3, %xmm4
	addps	%xmm4, %xmm0
	movaps	-16 * SIZE(X), %xmm4

	andps	%xmm3, %xmm5
	addps	%xmm5, %xmm1
	movaps	-12 * SIZE(X), %xmm5

	andps	%xmm3, %xmm6
	addps	%xmm6, %xmm0
	movaps	 -8 * SIZE(X), %xmm6

	andps	%xmm3, %xmm7
	addps	%xmm7, %xmm1
	movaps	 -4 * SIZE(X), %xmm7

#if defined(PREFETCH) && !defined(FETCH128)
	PREFETCH (PREFETCHSIZE +  64) - PREOFFSET(X)
#endif

	andps	%xmm3, %xmm4
	addps	%xmm4, %xmm0
	movaps	  0 * SIZE(X), %xmm4

	andps	%xmm3, %xmm5
	addps	%xmm5, %xmm1
	movaps	  4 * SIZE(X), %xmm5

	andps	%xmm3, %xmm6
	addps	%xmm6, %xmm0
	movaps	  8 * SIZE(X), %xmm6

	andps	%xmm3, %xmm7
	addps	%xmm7, %xmm1
	movaps	 12 * SIZE(X), %xmm7

	subl	$-32 * SIZE, X
	decl	I
	jg	.L11
	ALIGN_3

.L12:
	andps	%xmm3, %xmm4
	addps	%xmm4, %xmm0
	movaps	-16 * SIZE(X), %xmm4

	andps	%xmm3, %xmm5
	addps	%xmm5, %xmm1
	movaps	-12 * SIZE(X), %xmm5

	andps	%xmm3, %xmm6
	addps	%xmm6, %xmm0
	movaps	 -8 * SIZE(X), %xmm6

	andps	%xmm3, %xmm7
	addps	%xmm7, %xmm1
	movaps	 -4 * SIZE(X), %xmm7

	andps	%xmm3, %xmm4
	addps	%xmm4, %xmm0

	andps	%xmm3, %xmm5
	addps	%xmm5, %xmm1

	andps	%xmm3, %xmm6
	addps	%xmm6, %xmm0

	andps	%xmm3, %xmm7
	addps	%xmm7, %xmm1

	subl	$-32 * SIZE, X
	ALIGN_3

.L14:
	testl	$16, M
	je	.L16

	movaps	-32 * SIZE(X), %xmm4
	andps	%xmm3, %xmm4
	addps	%xmm4, %xmm0

	movaps	-28 * SIZE(X), %xmm5
	andps	%xmm3, %xmm5
	addps	%xmm5, %xmm1

	movaps	-24 * SIZE(X), %xmm6
	andps	%xmm3, %xmm6
	addps	%xmm6, %xmm0

	movaps	-20 * SIZE(X), %xmm7
	andps	%xmm3, %xmm7
	addps	%xmm7, %xmm1

	addl	$16 * SIZE, X
	ALIGN_3

.L16:
	testl	$8, M
	je	.L17

	movaps	-32 * SIZE(X), %xmm4
	andps	%xmm3, %xmm4
	addps	%xmm4, %xmm0

	movaps	-28 * SIZE(X), %xmm5
	andps	%xmm3, %xmm5
	addps	%xmm5, %xmm1

	addl	$8 * SIZE, X
	ALIGN_3

.L17:
	testl	$4, M
	je	.L18

	movaps	-32 * SIZE(X), %xmm4
	andps	%xmm3, %xmm4
	addps	%xmm4, %xmm0
	addl	$4 * SIZE, X
	ALIGN_3

.L18:
	testl	$2, M
	je	.L19

#ifdef movsd
	xorps	%xmm4, %xmm4
#endif
	movsd	-32 * SIZE(X), %xmm4
	andps	%xmm3, %xmm4
	addps	%xmm4, %xmm1
	addl	$2 * SIZE, X
	ALIGN_3

.L19:
	testl	$1, M
	je	.L998

	movss	-32 * SIZE(X), %xmm4
	andps	%xmm3, %xmm4
	addps	%xmm4, %xmm0
	jmp	.L998
	ALIGN_4

.L100:
	movl	M,  I
	sarl	$3, I
	jle	.L105
	ALIGN_4
	
.L101:
	movss	(X), %xmm4
	addl	INCX, X
	andps	%xmm3, %xmm4
	addss	%xmm4, %xmm0

	movss	(X), %xmm5
	addl	INCX, X
	andps	%xmm3, %xmm5
	addss	%xmm5, %xmm1

	movss	(X), %xmm6
	addl	INCX, X
	andps	%xmm3, %xmm6
	addss	%xmm6, %xmm0

	movss	(X), %xmm7
	addl	INCX, X
	andps	%xmm3, %xmm7
	addss	%xmm7, %xmm1

	movss	(X), %xmm4
	addl	INCX, X
	andps	%xmm3, %xmm4
	addss	%xmm4, %xmm0

	movss	(X), %xmm5
	addl	INCX, X
	andps	%xmm3, %xmm5
	addss	%xmm5, %xmm1

	movss	(X), %xmm6
	addl	INCX, X
	andps	%xmm3, %xmm6
	addss	%xmm6, %xmm0

	movss	(X), %xmm7
	addl	INCX, X
	andps	%xmm3, %xmm7
	addss	%xmm7, %xmm1

	decl	I
	jg	.L101
	ALIGN_4

.L105:
	andl	$7,  M
	jle	.L998
	ALIGN_4

.L106:
	movss	(X),   %xmm4
	andps	%xmm3, %xmm4
	addss	%xmm4, %xmm0
	addl	INCX, X
	decl	M
	jg	.L106
	ALIGN_4

.L998:
	addps	%xmm1, %xmm0

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
	movss	%xmm0, STACK_M
	flds	STACK_M

	popl	%ebx
	popl	%esi

	ret

	EPILOGUE
