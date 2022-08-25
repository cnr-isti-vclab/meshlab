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

	testl	$SIZE, Y
	je	.L10

	movsd	0 * SIZE(X), %xmm0
	mulsd	0 * SIZE(Y), %xmm0
	addl	$1 * SIZE, X
	addl	$1 * SIZE, Y
	decl	N
	ALIGN_2

.L10:
	movl	N,  %eax
	sarl	$4, %eax
	jle	.L24

	movlpd	 0 * SIZE(X), %xmm4
	movhpd	 1 * SIZE(X), %xmm4
	movlpd	 2 * SIZE(X), %xmm5
	movhpd	 3 * SIZE(X), %xmm5
	movlpd	 4 * SIZE(X), %xmm6
	movhpd	 5 * SIZE(X), %xmm6
	movlpd	 6 * SIZE(X), %xmm7
	movhpd	 7 * SIZE(X), %xmm7

	mulpd	 0 * SIZE(Y), %xmm4
	mulpd	 2 * SIZE(Y), %xmm5
	mulpd	 4 * SIZE(Y), %xmm6
	mulpd	 6 * SIZE(Y), %xmm7
	decl	%eax
	jle	.L22

	ALIGN_3

.L21:
	prefetch	(PREFETCHSIZE  + 0) * SIZE(Y)

	addpd	%xmm4,  %xmm0
	movlpd	 8 * SIZE(X), %xmm4
	movhpd	 9 * SIZE(X), %xmm4
	addpd	%xmm5,  %xmm1
	movlpd	10 * SIZE(X), %xmm5
	movhpd	11 * SIZE(X), %xmm5
	addpd	%xmm6,  %xmm2
	movlpd	12 * SIZE(X), %xmm6
	movhpd	13 * SIZE(X), %xmm6
	addpd	%xmm7,  %xmm3
	movlpd	14 * SIZE(X), %xmm7
	movhpd	15 * SIZE(X), %xmm7

	mulpd	 8 * SIZE(Y), %xmm4
	mulpd	10 * SIZE(Y), %xmm5
	mulpd	12 * SIZE(Y), %xmm6
	mulpd	14 * SIZE(Y), %xmm7

	prefetch	(PREFETCHSIZE  + 8) * SIZE(Y)

	addpd	%xmm4,  %xmm0
	movlpd	16 * SIZE(X), %xmm4
	movhpd	17 * SIZE(X), %xmm4
	addpd	%xmm5,  %xmm1
	movlpd	18 * SIZE(X), %xmm5
	movhpd	19 * SIZE(X), %xmm5
	addpd	%xmm6,  %xmm2
	movlpd	20 * SIZE(X), %xmm6
	movhpd	21 * SIZE(X), %xmm6
	addpd	%xmm7,  %xmm3
	movlpd	22 * SIZE(X), %xmm7
	movhpd	23 * SIZE(X), %xmm7

	mulpd	16 * SIZE(Y), %xmm4
	mulpd	18 * SIZE(Y), %xmm5
	mulpd	20 * SIZE(Y), %xmm6
	mulpd	22 * SIZE(Y), %xmm7

	addl	$16 * SIZE, X
	addl	$16 * SIZE, Y

	decl	%eax
	jg	.L21
	ALIGN_3

.L22:
	addpd	%xmm4,  %xmm0
	movlpd	 8 * SIZE(X), %xmm4
	movhpd	 9 * SIZE(X), %xmm4
	addpd	%xmm5,  %xmm1
	movlpd	10 * SIZE(X), %xmm5
	movhpd	11 * SIZE(X), %xmm5
	addpd	%xmm6,  %xmm2
	movlpd	12 * SIZE(X), %xmm6
	movhpd	13 * SIZE(X), %xmm6
	addpd	%xmm7,  %xmm3
	movlpd	14 * SIZE(X), %xmm7
	movhpd	15 * SIZE(X), %xmm7

	mulpd	 8 * SIZE(Y), %xmm4
	mulpd	10 * SIZE(Y), %xmm5
	mulpd	12 * SIZE(Y), %xmm6
	mulpd	14 * SIZE(Y), %xmm7

	addpd	%xmm4,  %xmm0
	addpd	%xmm5,  %xmm1
	addpd	%xmm6,  %xmm2
	addpd	%xmm7,  %xmm3

	addl	$16 * SIZE, X
	addl	$16 * SIZE, Y
	ALIGN_3

.L24:
	testl	$15, N
	jle	.L999

	testl	$8, N
	jle	.L25

	movlpd	 0 * SIZE(X), %xmm4
	movhpd	 1 * SIZE(X), %xmm4
	movlpd	 2 * SIZE(X), %xmm5
	movhpd	 3 * SIZE(X), %xmm5
	movlpd	 4 * SIZE(X), %xmm6
	movhpd	 5 * SIZE(X), %xmm6
	movlpd	 6 * SIZE(X), %xmm7
	movhpd	 7 * SIZE(X), %xmm7

	mulpd	 0 * SIZE(Y), %xmm4
	mulpd	 2 * SIZE(Y), %xmm5
	mulpd	 4 * SIZE(Y), %xmm6
	mulpd	 6 * SIZE(Y), %xmm7

	addpd	%xmm4,  %xmm0
	addpd	%xmm5,  %xmm1
	addpd	%xmm6,  %xmm2
	addpd	%xmm7,  %xmm3

	addl	$8 * SIZE, X
	addl	$8 * SIZE, Y
	ALIGN_3

.L25:
	testl	$4, N
	jle	.L26

	movlpd	 0 * SIZE(X), %xmm4
	movhpd	 1 * SIZE(X), %xmm4
	movlpd	 2 * SIZE(X), %xmm5
	movhpd	 3 * SIZE(X), %xmm5
	mulpd	 0 * SIZE(Y), %xmm4
	mulpd	 2 * SIZE(Y), %xmm5

	addpd	%xmm4,  %xmm0
	addpd	%xmm5,  %xmm1

	addl	$4 * SIZE, X
	addl	$4 * SIZE, Y
	ALIGN_3

.L26:
	testl	$2, N
	jle	.L27

	movlpd	 0 * SIZE(X), %xmm4
	movhpd	 1 * SIZE(X), %xmm4
	mulpd	 0 * SIZE(Y), %xmm4
	addpd	%xmm4,  %xmm0

	addl	$2 * SIZE, X
	addl	$2 * SIZE, Y
	ALIGN_3

.L27:
	testl	$1, N
	jle	.L999

	movsd	 0 * SIZE(X), %xmm4
	mulsd	 0 * SIZE(Y), %xmm4
	addsd	%xmm4,  %xmm0
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
	movsd	0 * SIZE(X), %xmm4
	addl	INCX, X
	mulsd	0 * SIZE(Y), %xmm4
	addl	INCY, Y
	movsd	0 * SIZE(X), %xmm5
	addl	INCX, X
	mulsd	0 * SIZE(Y), %xmm5
	addl	INCY, Y
	movsd	0 * SIZE(X), %xmm6
	addl	INCX, X
	mulsd	0 * SIZE(Y), %xmm6
	addl	INCY, Y
	movsd	0 * SIZE(X), %xmm7
	addl	INCX, X
	mulsd	0 * SIZE(Y), %xmm7
	addl	INCY, Y

	addsd	%xmm4, %xmm0
	addsd	%xmm5, %xmm1
	addsd	%xmm6, %xmm2
	addsd	%xmm7, %xmm3

	decl	%eax
	jg	.L53
	ALIGN_3

.L55:
	movl	N, %eax
	andl	$3,   %eax
	jle	.L999
	ALIGN_3

.L56:
	movsd	0 * SIZE(X), %xmm4
	addl	INCX, X
	mulsd	0 * SIZE(Y), %xmm4
	addl	INCY, Y
	addsd	%xmm4, %xmm0
	decl	%eax
	jg	.L56
	ALIGN_3

.L999:
	addpd	%xmm1, %xmm0
	addpd	%xmm3, %xmm2
	addpd	%xmm2, %xmm0

#if !defined(HAVE_SSE3) || defined(__INTERIX)
	movapd	%xmm0, %xmm1
	unpckhpd	%xmm0, %xmm0
	addsd	%xmm1, %xmm0
#else
	haddpd	%xmm0, %xmm0
#endif

	movsd	%xmm0, STACK_N
	fldl	STACK_N

	popl	%ebx
	popl	%esi
	popl	%edi
	ret

	EPILOGUE
