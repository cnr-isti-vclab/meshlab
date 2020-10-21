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
	xorps	%xmm1, %xmm1

	testl	M, M
	jle	.L999
	testl	INCX, INCX
	jle	.L999

	pcmpeqb	%xmm3, %xmm3
	psrlq	$1,    %xmm3

	sall	$BASE_SHIFT, INCX

	subl	$-16 * SIZE, X

	cmpl	$SIZE, INCX
	jne	.L40

	testl	$SIZE, X
	je	.L05

	movsd	-16 * SIZE(X), %xmm0
	addl	$SIZE, X

	andps	%xmm3, %xmm0
	subl	$1, M
	jle	.L999
	ALIGN_3

.L05:
	movl	M,  I
	sarl	$4, I
	jle	.L20

	movaps	-16 * SIZE(X), %xmm4
	movaps	-14 * SIZE(X), %xmm5
	movaps	-12 * SIZE(X), %xmm6
	movaps	-10 * SIZE(X), %xmm7

	decl	I
	jle	.L11
	ALIGN_4
	
.L10:
#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	andps	%xmm3, %xmm4
	addpd	%xmm4, %xmm0
	movaps	 -8 * SIZE(X), %xmm4

	andps	%xmm3, %xmm5
	addpd	%xmm5, %xmm1
	movaps	 -6 * SIZE(X), %xmm5

	andps	%xmm3, %xmm6
	addpd	%xmm6, %xmm0
	movaps	 -4 * SIZE(X), %xmm6

	andps	%xmm3, %xmm7
	addpd	%xmm7, %xmm1
	movaps	 -2 * SIZE(X), %xmm7

#if defined(PREFETCH) && !defined(FETCH128)
	PREFETCH (PREFETCHSIZE +  64) - PREOFFSET(X)
#endif

	andps	%xmm3, %xmm4
	addpd	%xmm4, %xmm0
	movaps	  0 * SIZE(X), %xmm4

	andps	%xmm3, %xmm5
	addpd	%xmm5, %xmm1
	movaps	  2 * SIZE(X), %xmm5

	andps	%xmm3, %xmm6
	addpd	%xmm6, %xmm0
	movaps	  4 * SIZE(X), %xmm6

	andps	%xmm3, %xmm7
	addpd	%xmm7, %xmm1
	movaps	  6 * SIZE(X), %xmm7

	subl	$-16 * SIZE, X
	decl	I
	jg	.L10
	ALIGN_4

.L11:
	andps	%xmm3, %xmm4
	addpd	%xmm4, %xmm0
	movaps	 -8 * SIZE(X), %xmm4

	andps	%xmm3, %xmm5
	addpd	%xmm5, %xmm1
	movaps	 -6 * SIZE(X), %xmm5

	andps	%xmm3, %xmm6
	addpd	%xmm6, %xmm0
	movaps	 -4 * SIZE(X), %xmm6

	andps	%xmm3, %xmm7
	addpd	%xmm7, %xmm1
	movaps	 -2 * SIZE(X), %xmm7

	andps	%xmm3, %xmm4
	addpd	%xmm4, %xmm0
	andps	%xmm3, %xmm5
	addpd	%xmm5, %xmm1
	andps	%xmm3, %xmm6
	addpd	%xmm6, %xmm0
	andps	%xmm3, %xmm7
	addpd	%xmm7, %xmm1

	subl	$-16 * SIZE, X
	ALIGN_3

.L20:
	andl	$15,  M
	jle	.L999

	testl	$8, M
	je	.L21

	movaps	-16 * SIZE(X), %xmm4
	movaps	-14 * SIZE(X), %xmm5
	movaps	-12 * SIZE(X), %xmm6
	movaps	-10 * SIZE(X), %xmm7

	andps	%xmm3, %xmm4
	addpd	%xmm4, %xmm0
	andps	%xmm3, %xmm5
	addpd	%xmm5, %xmm1
	andps	%xmm3, %xmm6
	addpd	%xmm6, %xmm0
	andps	%xmm3, %xmm7
	addpd	%xmm7, %xmm1
	addl	$8 * SIZE, X
	ALIGN_3

.L21:
	testl	$4, M
	je	.L22

	movaps	-16 * SIZE(X), %xmm4
	movaps	-14 * SIZE(X), %xmm5

	andps	%xmm3, %xmm4
	addpd	%xmm4, %xmm0
	andps	%xmm3, %xmm5
	addpd	%xmm5, %xmm1

	addl	$4 * SIZE, X
	ALIGN_3	

.L22:
	testl	$2, M
	je	.L23

	movaps	-16 * SIZE(X), %xmm4
	andps	%xmm3, %xmm4
	addpd	%xmm4, %xmm0
	addl	$2 * SIZE, X
	
.L23:
	testl	$1, M
	je	.L999

#ifdef movsd
	xorps	%xmm4, %xmm4
#endif
	movsd	-16 * SIZE(X), %xmm4
	andps	%xmm3, %xmm4
	addsd	%xmm4, %xmm1
	jmp	.L999
	ALIGN_3

.L40:
	movl	M,  I
	sarl	$3, I
	jle	.L60
	ALIGN_4
	
.L50:
	movsd	-16 * SIZE(X), %xmm4
	addl	INCX, X
	movhps	-16 * SIZE(X), %xmm4
	addl	INCX, X
	andps	%xmm3, %xmm4
	addpd	%xmm4, %xmm0

	movsd	-16 * SIZE(X), %xmm5
	addl	INCX, X
	movhps	-16 * SIZE(X), %xmm5
	addl	INCX, X
	andps	%xmm3, %xmm5
	addpd	%xmm5, %xmm1

	movsd	-16 * SIZE(X), %xmm6
	addl	INCX, X
	movhps	-16 * SIZE(X), %xmm6
	addl	INCX, X
	andps	%xmm3, %xmm6
	addpd	%xmm6, %xmm0

	movsd	-16 * SIZE(X), %xmm7
	addl	INCX, X
	movhps	-16 * SIZE(X), %xmm7
	addl	INCX, X
	andps	%xmm3, %xmm7
	addpd	%xmm7, %xmm1

	decl	I
	jg	.L50
	ALIGN_4

.L60:
#ifdef movsd
	xorps	%xmm4, %xmm4
#endif
	andl	$7,  M
	jle	.L999
	ALIGN_4

.L61:
	movsd	-16 * SIZE(X), %xmm4
	andps	%xmm3, %xmm4
	addsd	%xmm4, %xmm0
	addl	INCX, X
	decl	M
	jg	.L61
	ALIGN_4

.L999:
	addpd	%xmm1, %xmm0

#ifndef HAVE_SSE3
	movaps	%xmm0,  %xmm1
	unpckhpd %xmm0, %xmm0
	addsd	 %xmm1, %xmm0
#else
	haddpd	%xmm0, %xmm0
#endif

	movsd	%xmm0, STACK_M
	fldl	STACK_M
	popl	%ebx
	popl	%esi
	ret

	EPILOGUE

