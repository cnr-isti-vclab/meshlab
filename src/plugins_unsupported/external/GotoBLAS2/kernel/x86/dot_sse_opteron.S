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

#define STACK	12
#define ARGS     0
	
#define STACK_N		 4 + STACK + ARGS(%esp)
#define STACK_X		 8 + STACK + ARGS(%esp)
#define STACK_INCX	12 + STACK + ARGS(%esp)
#define STACK_Y		16 + STACK + ARGS(%esp)
#define STACK_INCY	20 + STACK + ARGS(%esp)

#define N	%ecx
#define X	%esi
#define INCX	%ebx
#define Y	%edi
#define INCY	%edx

#define PREFETCHSIZE 84

	PROLOGUE

	pushl	%edi
	pushl	%esi
	pushl	%ebx

	PROFCODE

	movl	STACK_N,     N
	movl	STACK_X,     X
	movl	STACK_INCX,  INCX
	movl	STACK_Y,     Y
	movl	STACK_INCY,  INCY

#ifdef F_INTERFACE
	movl	(N),   N		# N
	movl	(INCX),INCX		# INCX
	movl	(INCY),INCY		# INCY
#endif

	leal	(, INCX, SIZE), INCX	
	leal	(, INCY, SIZE), INCY	

	pxor	%xmm0, %xmm0
	pxor	%xmm1, %xmm1
	pxor	%xmm2, %xmm2
	pxor	%xmm3, %xmm3

	cmpl	$0, N
	jle	.L999

	cmpl	$SIZE, INCX
	jne	.L50
	cmpl	$SIZE, INCY
	jne	.L50

	cmpl	$3, N
	jle	.L27

	testl	$SIZE, Y
	je	.L05

	movss	0 * SIZE(X), %xmm0
	mulss	0 * SIZE(Y), %xmm0
	addl	$1 * SIZE, X
	addl	$1 * SIZE, Y
	decl	N
	ALIGN_2

.L05:
	testl	$2 * SIZE, Y
	je	.L10

	movss	 0 * SIZE(X), %xmm4
	movss	 1 * SIZE(X), %xmm5

	mulss	 0 * SIZE(Y), %xmm4
	mulss	 1 * SIZE(Y), %xmm5

	addss	%xmm4,  %xmm1
	addss	%xmm5,  %xmm2

	addl	$2 * SIZE, X
	addl	$2 * SIZE, Y
	subl	$2, N
	jle	.L999
	ALIGN_2

.L10:
	movl	N,  %eax
	sarl	$5, %eax
	jle	.L24

	movlps	 0 * SIZE(X), %xmm4
	movhps	 2 * SIZE(X), %xmm4
	movlps	 4 * SIZE(X), %xmm5
	movhps	 6 * SIZE(X), %xmm5
	movlps	 8 * SIZE(X), %xmm6
	movhps	10 * SIZE(X), %xmm6
	movlps	12 * SIZE(X), %xmm7
	movhps	14 * SIZE(X), %xmm7

	mulps	 0 * SIZE(Y), %xmm4
	mulps	 4 * SIZE(Y), %xmm5
	mulps	 8 * SIZE(Y), %xmm6
	mulps	12 * SIZE(Y), %xmm7
	decl	%eax
	jle	.L22

	ALIGN_3

.L21:
	prefetch	(PREFETCHSIZE  + 0) * SIZE(Y)

	addps	%xmm4,  %xmm0
	movlps	16 * SIZE(X), %xmm4
	movhps	18 * SIZE(X), %xmm4
	addps	%xmm5,  %xmm1
	movlps	20 * SIZE(X), %xmm5
	movhps	22 * SIZE(X), %xmm5
	addps	%xmm6,  %xmm2
	movlps	24 * SIZE(X), %xmm6
	movhps	26 * SIZE(X), %xmm6
	addps	%xmm7,  %xmm3
	movlps	28 * SIZE(X), %xmm7
	movhps	30 * SIZE(X), %xmm7

	mulps	16 * SIZE(Y), %xmm4
	mulps	20 * SIZE(Y), %xmm5
	mulps	24 * SIZE(Y), %xmm6
	mulps	28 * SIZE(Y), %xmm7

	prefetch	(PREFETCHSIZE  + 16) * SIZE(Y)

	addps	%xmm4,  %xmm0
	movlps	32 * SIZE(X), %xmm4
	movhps	34 * SIZE(X), %xmm4
	addps	%xmm5,  %xmm1
	movlps	36 * SIZE(X), %xmm5
	movhps	38 * SIZE(X), %xmm5
	addps	%xmm6,  %xmm2
	movlps	40 * SIZE(X), %xmm6
	movhps	42 * SIZE(X), %xmm6
	addps	%xmm7,  %xmm3
	movlps	44 * SIZE(X), %xmm7
	movhps	46 * SIZE(X), %xmm7

	mulps	32 * SIZE(Y), %xmm4
	mulps	36 * SIZE(Y), %xmm5
	mulps	40 * SIZE(Y), %xmm6
	mulps	44 * SIZE(Y), %xmm7

	addl	$32 * SIZE, X
	addl	$32 * SIZE, Y

	decl	%eax
	jg	.L21
	ALIGN_3

.L22:
	addps	%xmm4,  %xmm0
	movlps	16 * SIZE(X), %xmm4
	movhps	18 * SIZE(X), %xmm4
	addps	%xmm5,  %xmm1
	movlps	20 * SIZE(X), %xmm5
	movhps	22 * SIZE(X), %xmm5
	addps	%xmm6,  %xmm2
	movlps	24 * SIZE(X), %xmm6
	movhps	26 * SIZE(X), %xmm6
	addps	%xmm7,  %xmm3
	movlps	28 * SIZE(X), %xmm7
	movhps	30 * SIZE(X), %xmm7

	mulps	16 * SIZE(Y), %xmm4
	mulps	20 * SIZE(Y), %xmm5
	mulps	24 * SIZE(Y), %xmm6
	mulps	28 * SIZE(Y), %xmm7

	addps	%xmm4,  %xmm0
	addps	%xmm5,  %xmm1
	addps	%xmm6,  %xmm2
	addps	%xmm7,  %xmm3

	addl	$32 * SIZE, X
	addl	$32 * SIZE, Y
	ALIGN_3

.L24:
	testl	$31, N
	jle	.L999

	testl	$16, N
	jle	.L25

	movlps	 0 * SIZE(X), %xmm4
	movhps	 2 * SIZE(X), %xmm4
	movlps	 4 * SIZE(X), %xmm5
	movhps	 6 * SIZE(X), %xmm5
	movlps	 8 * SIZE(X), %xmm6
	movhps	10 * SIZE(X), %xmm6
	movlps	12 * SIZE(X), %xmm7
	movhps	14 * SIZE(X), %xmm7

	mulps	 0 * SIZE(Y), %xmm4
	mulps	 4 * SIZE(Y), %xmm5
	mulps	 8 * SIZE(Y), %xmm6
	mulps	12 * SIZE(Y), %xmm7

	addps	%xmm4,  %xmm0
	addps	%xmm5,  %xmm1
	addps	%xmm6,  %xmm2
	addps	%xmm7,  %xmm3

	addl	$16 * SIZE, X
	addl	$16 * SIZE, Y
	ALIGN_3

.L25:
	testl	$8, N
	jle	.L26

	movlps	 0 * SIZE(X), %xmm4
	movhps	 2 * SIZE(X), %xmm4
	movlps	 4 * SIZE(X), %xmm5
	movhps	 6 * SIZE(X), %xmm5
	mulps	 0 * SIZE(Y), %xmm4
	mulps	 4 * SIZE(Y), %xmm5

	addps	%xmm4,  %xmm0
	addps	%xmm5,  %xmm1

	addl	$8 * SIZE, X
	addl	$8 * SIZE, Y
	ALIGN_3

.L26:
	testl	$4, N
	jle	.L27

	movlps	 0 * SIZE(X), %xmm4
	movhps	 2 * SIZE(X), %xmm4
	mulps	 0 * SIZE(Y), %xmm4
	addps	%xmm4,  %xmm0

	addl	$4 * SIZE, X
	addl	$4 * SIZE, Y
	ALIGN_3

.L27:
	testl	$2, N
	jle	.L28

	movss	 0 * SIZE(X), %xmm4
	movss	 1 * SIZE(X), %xmm5

	mulss	 0 * SIZE(Y), %xmm4
	mulss	 1 * SIZE(Y), %xmm5

	addss	%xmm4,  %xmm0
	addss	%xmm5,  %xmm1

	addl	$2 * SIZE, X
	addl	$2 * SIZE, Y
	ALIGN_3


.L28:
	testl	$1, N
	jle	.L999

	movss	 0 * SIZE(X), %xmm4
	mulss	 0 * SIZE(Y), %xmm4
	addss	%xmm4,  %xmm0
	jmp	.L999
	ALIGN_3


.L50:
#ifdef F_INTERFACE
	testl	INCX, INCX
	jge	.L51

	movl	N, %eax
	decl	%eax
	imull	INCX, %eax
	subl	%eax, X
	ALIGN_3

.L51:
	testl	INCY, INCY
	jge	.L52

	movl	N, %eax
	decl	%eax
	imull	INCY, %eax
	subl	%eax, Y
	ALIGN_3
.L52:
#endif

	movl	N,  %eax
	sarl	$2, %eax
	jle	.L55
	ALIGN_3

.L53:
	movss	0 * SIZE(X), %xmm4
	addl	INCX, X
	mulss	0 * SIZE(Y), %xmm4
	addl	INCY, Y
	movss	0 * SIZE(X), %xmm5
	addl	INCX, X
	mulss	0 * SIZE(Y), %xmm5
	addl	INCY, Y
	movss	0 * SIZE(X), %xmm6
	addl	INCX, X
	mulss	0 * SIZE(Y), %xmm6
	addl	INCY, Y
	movss	0 * SIZE(X), %xmm7
	addl	INCX, X
	mulss	0 * SIZE(Y), %xmm7
	addl	INCY, Y

	addss	%xmm4, %xmm0
	addss	%xmm5, %xmm1
	addss	%xmm6, %xmm2
	addss	%xmm7, %xmm3

	decl	%eax
	jg	.L53
	ALIGN_3

.L55:
	movl	N, %eax
	andl	$3,   %eax
	jle	.L999
	ALIGN_3

.L56:
	movss	0 * SIZE(X), %xmm4
	addl	INCX, X
	mulss	0 * SIZE(Y), %xmm4
	addl	INCY, Y
	addss	%xmm4, %xmm0
	decl	%eax
	jg	.L56
	ALIGN_3

.L999:
	addps	%xmm1, %xmm0
	addps	%xmm3, %xmm2
	addps	%xmm2, %xmm0

#if !defined(HAVE_SSE3) || defined(__INTERIX)
	movhlps	%xmm0, %xmm1
	addps	%xmm1, %xmm0
	
	pshufd	$1, %xmm0, %xmm1
	addss	 %xmm1, %xmm0
#else
	haddps	%xmm0, %xmm0
	haddps	%xmm0, %xmm0
#endif

	movss	%xmm0, STACK_N
	flds	STACK_N

	popl	%ebx
	popl	%esi
	popl	%edi
	ret

	EPILOGUE
