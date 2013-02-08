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

#ifdef ATOM
#define PREFETCH	prefetchnta
#define PREFETCHW	prefetcht0
#define PREFETCHSIZE	(8 * 6)
#endif

#define STACKSIZE	16

#define M		 4 + STACKSIZE(%esp)
#define N		 8 + STACKSIZE(%esp)
#define ALPHA_R		16 + STACKSIZE(%esp)
#define ALPHA_I		24 + STACKSIZE(%esp)
#define A		32 + STACKSIZE(%esp)
#define STACK_LDA	36 + STACKSIZE(%esp)
#define STACK_X		40 + STACKSIZE(%esp)
#define STACK_INCX	44 + STACKSIZE(%esp)
#define Y		48 + STACKSIZE(%esp)
#define STACK_INCY	52 + STACKSIZE(%esp)
#define BUFFER		56 + STACKSIZE(%esp)
	
#define I	%eax
#define J	%ebx

#define INCX	J
#define INCY	%ecx

#define A1	%esi
#define X	%edx
#define Y1	%edi
#define LDA	%ebp

#if !defined(CONJ) && !defined(XCONJ)
#define ADD1	   addsd
#define ADD2	   addsd
#define ADD3	   subsd
#define ADD4	   addsd
#endif

#if  defined(CONJ) && !defined(XCONJ)
#define ADD1	   addsd
#define ADD2	   addsd
#define ADD3	   addsd
#define ADD4	   subsd
#endif

#if !defined(CONJ) &&  defined(XCONJ)
#define ADD1	   addsd
#define ADD2	   subsd
#define ADD3	   addsd
#define ADD4	   addsd
#endif

#if  defined(CONJ) &&  defined(XCONJ)
#define ADD1	   addsd
#define ADD2	   subsd
#define ADD3	   subsd
#define ADD4	   subsd
#endif

	PROLOGUE

	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx

	PROFCODE

	movl	STACK_LDA,  LDA
	movl	STACK_X,    X
	movl	STACK_INCX, INCX
	movl	STACK_INCY, INCY

	sall	$ZBASE_SHIFT, INCX
	sall	$ZBASE_SHIFT, INCY
	sall	$ZBASE_SHIFT, LDA

	subl	$-16 * SIZE, A

	cmpl	$0, N
	jle	.L999
	cmpl	$0, M
	jle	.L999

	movl	BUFFER, Y1
	
	movl	M,  I
	sarl	$2, I
	jle	.L05
	ALIGN_4

.L02:
	movsd	0 * SIZE(X), %xmm0
	movhpd	1 * SIZE(X), %xmm0
	addl	INCX, X

	movsd	0 * SIZE(X), %xmm1
	movhpd	1 * SIZE(X), %xmm1
	addl	INCX, X

	movsd	0 * SIZE(X), %xmm2
	movhpd	1 * SIZE(X), %xmm2
	addl	INCX, X

	movsd	0 * SIZE(X), %xmm3
	movhpd	1 * SIZE(X), %xmm3
	addl	INCX, X

	movapd	%xmm0, 0 * SIZE(Y1)
	movapd	%xmm1, 2 * SIZE(Y1)
	movapd	%xmm2, 4 * SIZE(Y1)
	movapd	%xmm3, 6 * SIZE(Y1)

	addl	$8 * SIZE, Y1
	decl	I
	jg	.L02
	ALIGN_4

.L05:
	movl	M,  I
	andl	$3, I
	jle	.L10
	ALIGN_2

.L06:
	movsd	0 * SIZE(X), %xmm0
	movhpd	1 * SIZE(X), %xmm0
	addl	INCX, X

	movapd	%xmm0, 0 * SIZE(Y1)
	addl	$2 * SIZE, Y1
	decl	I
	jg	.L06
	ALIGN_4

.L10:
	movl	Y, Y1

	movl	N,  J
	ALIGN_3

.L11:	
	movl	BUFFER, X
	addl	$16 * SIZE, X

	movl	A, A1
	addl	LDA, A

	xorps	%xmm0, %xmm0
	xorps	%xmm1, %xmm1

	movsd	-16 * SIZE(X), %xmm2
	movsd	-15 * SIZE(X), %xmm3

	movl	M,   I
	sarl	$2,  I
	jle	.L15

	movsd	-16 * SIZE(A1), %xmm4
	movsd	-15 * SIZE(A1), %xmm5

	movapd	%xmm4, %xmm6
	mulsd	%xmm2, %xmm4
	mulsd	%xmm3, %xmm6

	decl	I
	jle	.L13
	ALIGN_4

.L12:
#ifdef PREFETCH
	PREFETCH PREFETCHSIZE * SIZE(A1)
#endif

	movapd	%xmm5, %xmm7
	mulsd	%xmm3, %xmm5
	movsd	-13 * SIZE(X), %xmm3
	ADD1	%xmm4, %xmm0
	movsd	-14 * SIZE(A1), %xmm4
	mulsd	%xmm2, %xmm7
	movsd	-14 * SIZE(X), %xmm2
	ADD2	%xmm6, %xmm1

	movapd	%xmm4, %xmm6
	mulsd	%xmm2, %xmm4
	ADD3	%xmm5, %xmm0
	movsd	-13 * SIZE(A1), %xmm5
	mulsd	%xmm3, %xmm6
	ADD4	%xmm7, %xmm1

	movapd	%xmm5, %xmm7
	mulsd	%xmm3, %xmm5
	movsd	-11 * SIZE(X), %xmm3
	ADD1	%xmm4, %xmm0
	movsd	-12 * SIZE(A1), %xmm4
	mulsd	%xmm2, %xmm7
	movsd	-12 * SIZE(X), %xmm2
	ADD2	%xmm6, %xmm1

	movapd	%xmm4, %xmm6
	mulsd	%xmm2, %xmm4
	ADD3	%xmm5, %xmm0
	movsd	-11 * SIZE(A1), %xmm5
	mulsd	%xmm3, %xmm6
	ADD4	%xmm7, %xmm1

	movapd	%xmm5, %xmm7
	mulsd	%xmm3, %xmm5
	movsd	 -9 * SIZE(X), %xmm3
	ADD1	%xmm4, %xmm0
	movsd	-10 * SIZE(A1), %xmm4
	mulsd	%xmm2, %xmm7
	movsd	-10 * SIZE(X), %xmm2
	ADD2	%xmm6, %xmm1

	movapd	%xmm4, %xmm6
	mulsd	%xmm2, %xmm4
	ADD3	%xmm5, %xmm0
	movsd	 -9 * SIZE(A1), %xmm5
	mulsd	%xmm3, %xmm6
	ADD4	%xmm7, %xmm1

	movapd	%xmm5, %xmm7
	mulsd	%xmm3, %xmm5
	movsd	 -7 * SIZE(X), %xmm3
	ADD1	%xmm4, %xmm0
	movsd	 -8 * SIZE(A1), %xmm4
	mulsd	%xmm2, %xmm7
	movsd	 -8 * SIZE(X), %xmm2
	ADD2	%xmm6, %xmm1

	movapd	%xmm4, %xmm6
	mulsd	%xmm2, %xmm4
	ADD3	%xmm5, %xmm0
	movsd	 -7 * SIZE(A1), %xmm5
	mulsd	%xmm3, %xmm6
	ADD4	%xmm7, %xmm1

	addl	$8 * SIZE, A1
	addl	$8 * SIZE, X

	decl	I
	jg	.L12
	ALIGN_4

.L13:
	movapd	%xmm5, %xmm7
	mulsd	%xmm3, %xmm5
	movsd	-13 * SIZE(X), %xmm3
	ADD1	%xmm4, %xmm0
	movsd	-14 * SIZE(A1), %xmm4
	mulsd	%xmm2, %xmm7
	movsd	-14 * SIZE(X), %xmm2
	ADD2	%xmm6, %xmm1

	movapd	%xmm4, %xmm6
	mulsd	%xmm2, %xmm4
	ADD3	%xmm5, %xmm0
	movsd	-13 * SIZE(A1), %xmm5
	mulsd	%xmm3, %xmm6
	ADD4	%xmm7, %xmm1

	movapd	%xmm5, %xmm7
	mulsd	%xmm3, %xmm5
	movsd	-11 * SIZE(X), %xmm3
	ADD1	%xmm4, %xmm0
	movsd	-12 * SIZE(A1), %xmm4
	mulsd	%xmm2, %xmm7
	movsd	-12 * SIZE(X), %xmm2
	ADD2	%xmm6, %xmm1

	movapd	%xmm4, %xmm6
	mulsd	%xmm2, %xmm4
	ADD3	%xmm5, %xmm0
	movsd	-11 * SIZE(A1), %xmm5
	mulsd	%xmm3, %xmm6
	ADD4	%xmm7, %xmm1

	movapd	%xmm5, %xmm7
	mulsd	%xmm3, %xmm5
	movsd	 -9 * SIZE(X), %xmm3
	ADD1	%xmm4, %xmm0
	movsd	-10 * SIZE(A1), %xmm4
	mulsd	%xmm2, %xmm7
	movsd	-10 * SIZE(X), %xmm2
	ADD2	%xmm6, %xmm1

	movapd	%xmm4, %xmm6
	mulsd	%xmm2, %xmm4
	ADD3	%xmm5, %xmm0
	movsd	 -9 * SIZE(A1), %xmm5
	mulsd	%xmm3, %xmm6
	ADD4	%xmm7, %xmm1

	movapd	%xmm5, %xmm7
	mulsd	%xmm3, %xmm5
	movsd	 -7 * SIZE(X), %xmm3
	ADD1	%xmm4, %xmm0
	mulsd	%xmm2, %xmm7
	movsd	 -8 * SIZE(X), %xmm2
	ADD2	%xmm6, %xmm1

	ADD3	%xmm5, %xmm0
	ADD4	%xmm7, %xmm1

	addl	$8 * SIZE, A1
	addl	$8 * SIZE, X
	ALIGN_4

.L15:
	testl	$2, M
	jle	.L17

	movsd	-16 * SIZE(A1), %xmm4
	movsd	-15 * SIZE(A1), %xmm5

	movapd	%xmm4, %xmm6
	mulsd	%xmm2, %xmm4
	mulsd	%xmm3, %xmm6

	movapd	%xmm5, %xmm7
	mulsd	%xmm3, %xmm5
	movsd	-13 * SIZE(X), %xmm3
	ADD1	%xmm4, %xmm0
	movsd	-14 * SIZE(A1), %xmm4
	mulsd	%xmm2, %xmm7
	movsd	-14 * SIZE(X), %xmm2
	ADD2	%xmm6, %xmm1

	movapd	%xmm4, %xmm6
	mulsd	%xmm2, %xmm4
	ADD3	%xmm5, %xmm0
	movsd	-13 * SIZE(A1), %xmm5
	mulsd	%xmm3, %xmm6
	ADD4	%xmm7, %xmm1

	movapd	%xmm5, %xmm7
	mulsd	%xmm3, %xmm5
	movsd	-11 * SIZE(X), %xmm3
	ADD1	%xmm4, %xmm0
	mulsd	%xmm2, %xmm7
	movsd	-12 * SIZE(X), %xmm2
	ADD2	%xmm6, %xmm1

	ADD3	%xmm5, %xmm0
	ADD4	%xmm7, %xmm1

	addl	$4 * SIZE, A1
	ALIGN_4

.L17:
	testl	$1, M
	jle	.L18

	movsd	-16 * SIZE(A1), %xmm4
	movsd	-15 * SIZE(A1), %xmm5

	movapd	%xmm4, %xmm6
	mulsd	%xmm2, %xmm4
	mulsd	%xmm3, %xmm6

	movapd	%xmm5, %xmm7
	mulsd	%xmm3, %xmm5
	ADD1	%xmm4, %xmm0
	mulsd	%xmm2, %xmm7
	ADD2	%xmm6, %xmm1

	ADD3	%xmm5, %xmm0
	ADD4	%xmm7, %xmm1
	ALIGN_4

.L18:
	movsd	 0 * SIZE(Y1), %xmm4
	movapd	%xmm0, %xmm2
	mulsd	ALPHA_R, %xmm0
	movsd	 1 * SIZE(Y1), %xmm5
	movapd	%xmm1, %xmm3
	mulsd	ALPHA_R, %xmm1

	mulsd	ALPHA_I, %xmm2
	mulsd	ALPHA_I, %xmm3

	addsd	%xmm2, %xmm1
	subsd	%xmm3, %xmm0
	addsd	%xmm4, %xmm0
	addsd	%xmm5, %xmm1

	movlpd	%xmm0,  0 * SIZE(Y1)
	movlpd	%xmm1,  1 * SIZE(Y1)

	addl	INCY, Y1

	decl	J
	jg	.L11
	ALIGN_4
	
.L999:
	popl	%ebx
	popl	%esi
	popl	%edi	
	popl	%ebp
	ret

	EPILOGUE
