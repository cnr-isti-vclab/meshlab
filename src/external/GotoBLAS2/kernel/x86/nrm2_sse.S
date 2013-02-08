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
	PROFCODE

	pushl	%esi
	pushl	%ebx

	movl	STACK_M,    M
	movl	STACK_X,    X
	movl	STACK_INCX, INCX

	pxor	%xmm0, %xmm0
	testl	M, M
	jle	.L999
	pxor	%xmm1, %xmm1
	testl	INCX, INCX
	jle	.L999

	leal	(, INCX, SIZE), INCX
	cmpl	$SIZE, INCX
	jne	.L40

	subl	$-32 * SIZE, X

	testl	$SIZE, X
	je	.L05
	
	movss	-32 * SIZE(X), %xmm0
	cvtss2sd %xmm0, %xmm0
	mulsd	 %xmm0, %xmm0

	addl	INCX, X
	decl	M
	jle	.L998
	ALIGN_3

.L05:
	movl	M,  I
	sarl	$4, I
	jle	.L13
	
	movsd	-32 * SIZE(X), %xmm4
	movsd	-30 * SIZE(X), %xmm5
	movsd	-28 * SIZE(X), %xmm6
	movsd	-26 * SIZE(X), %xmm7

	decl	I
	jle	.L12
	ALIGN_3

.L10:
#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	cvtps2pd %xmm4, %xmm2
	movsd	-24 * SIZE(X), %xmm4
	mulpd	 %xmm2, %xmm2
	addpd	 %xmm2, %xmm0

	cvtps2pd %xmm5, %xmm3
	movsd	-22 * SIZE(X), %xmm5
	mulpd	 %xmm3, %xmm3
	addpd	 %xmm3, %xmm1

	cvtps2pd %xmm6, %xmm2
	movsd	-20 * SIZE(X), %xmm6
	mulpd	 %xmm2, %xmm2
	addpd	 %xmm2, %xmm0

	cvtps2pd %xmm7, %xmm3
	movsd	-18 * SIZE(X), %xmm7
	mulpd	 %xmm3, %xmm3
	addpd	 %xmm3, %xmm1

	cvtps2pd %xmm4, %xmm2
	movsd	-16 * SIZE(X), %xmm4
	mulpd	 %xmm2, %xmm2
	addpd	 %xmm2, %xmm0

	cvtps2pd %xmm5, %xmm3
	movsd	-14 * SIZE(X), %xmm5
	mulpd	 %xmm3, %xmm3
	addpd	 %xmm3, %xmm1

	cvtps2pd %xmm6, %xmm2
	movsd	-12 * SIZE(X), %xmm6
	mulpd	 %xmm2, %xmm2
	addpd	 %xmm2, %xmm0

	cvtps2pd %xmm7, %xmm3
	movsd	-10 * SIZE(X), %xmm7
	mulpd	 %xmm3, %xmm3
	addpd	 %xmm3, %xmm1

	subl	$-16 * SIZE, X
	decl	I
	jg	.L10
	ALIGN_3

.L12:
	cvtps2pd %xmm4, %xmm2
	movsd	-24 * SIZE(X), %xmm4
	mulpd	 %xmm2, %xmm2
	addpd	 %xmm2, %xmm0

	cvtps2pd %xmm5, %xmm3
	movsd	-22 * SIZE(X), %xmm5
	mulpd	 %xmm3, %xmm3
	addpd	 %xmm3, %xmm1

	cvtps2pd %xmm6, %xmm2
	movsd	-20 * SIZE(X), %xmm6
	mulpd	 %xmm2, %xmm2
	addpd	 %xmm2, %xmm0

	cvtps2pd %xmm7, %xmm3
	movsd	-18 * SIZE(X), %xmm7
	mulpd	 %xmm3, %xmm3
	addpd	 %xmm3, %xmm1

	cvtps2pd %xmm4, %xmm2
	mulpd	 %xmm2, %xmm2
	addpd	 %xmm2, %xmm0

	cvtps2pd %xmm5, %xmm3
	mulpd	 %xmm3, %xmm3
	addpd	 %xmm3, %xmm1

	cvtps2pd %xmm6, %xmm2
	mulpd	 %xmm2, %xmm2
	addpd	 %xmm2, %xmm0

	cvtps2pd %xmm7, %xmm3
	mulpd	 %xmm3, %xmm3
	addpd	 %xmm3, %xmm1

	subl	$-16 * SIZE, X
	ALIGN_4

.L13:
	testl	$8,  M
	je	.L14

	movsd	-32 * SIZE(X), %xmm4

	cvtps2pd %xmm4, %xmm2
	mulpd	 %xmm2, %xmm2
	addpd	 %xmm2, %xmm0

	movsd	-30 * SIZE(X), %xmm5

	cvtps2pd %xmm5, %xmm3
	mulpd	 %xmm3, %xmm3
	addpd	 %xmm3, %xmm1

	movsd	-28 * SIZE(X), %xmm6

	cvtps2pd %xmm6, %xmm2
	mulpd	 %xmm2, %xmm2
	addpd	 %xmm2, %xmm0

	movsd	-26 * SIZE(X), %xmm7

	cvtps2pd %xmm7, %xmm3
	mulpd	 %xmm3, %xmm3
	addpd	 %xmm3, %xmm1

	addl	$8 * SIZE, X
	ALIGN_3

.L14:
	testl	$4,  M
	je	.L15

	movsd	-32 * SIZE(X), %xmm4
	cvtps2pd %xmm4, %xmm2
	mulpd	 %xmm2, %xmm2
	addpd	 %xmm2, %xmm0

	movsd	-30 * SIZE(X), %xmm5
	cvtps2pd %xmm5, %xmm3
	mulpd	 %xmm3, %xmm3
	addpd	 %xmm3, %xmm1
	addl	$4 * SIZE, X
	ALIGN_3

.L15:
	testl	$2,  M
	je	.L16

	movsd	-32 * SIZE(X), %xmm4
	cvtps2pd %xmm4, %xmm2
	mulpd	 %xmm2, %xmm2
	addpd	 %xmm2, %xmm0
	addl	$2 * SIZE, X
	ALIGN_3

.L16:
	testl	$1,  M
	je	.L998

	movss	-32 * SIZE(X), %xmm4
	cvtss2sd %xmm4, %xmm2
	mulsd	 %xmm2, %xmm2
	addsd	 %xmm2, %xmm1
	jmp	.L998
	ALIGN_4

.L40:
	movl	M,  I
	sarl	$3, I
	jle	.L44
	ALIGN_4
	
.L41:
	movss	(X), %xmm4
	addl	INCX, X

	cvtss2sd %xmm4, %xmm2
	mulsd	 %xmm2, %xmm2
	addsd	 %xmm2, %xmm0

	movss	(X), %xmm5
	addl	INCX, X

	cvtss2sd %xmm5, %xmm2
	mulsd	 %xmm2, %xmm2
	addsd	 %xmm2, %xmm1

	movss	(X), %xmm6
	addl	INCX, X

	cvtss2sd %xmm6, %xmm2
	mulsd	 %xmm2, %xmm2
	addsd	 %xmm2, %xmm0

	movss	(X), %xmm7
	addl	INCX, X

	cvtss2sd %xmm7, %xmm2
	mulsd	 %xmm2, %xmm2
	addsd	 %xmm2, %xmm1

	movss	(X), %xmm4
	addl	INCX, X

	cvtss2sd %xmm4, %xmm2
	mulsd	 %xmm2, %xmm2
	addsd	 %xmm2, %xmm0

	movss	(X), %xmm5
	addl	INCX, X

	cvtss2sd %xmm5, %xmm2
	mulsd	 %xmm2, %xmm2
	addsd	 %xmm2, %xmm1

	movss	(X), %xmm6
	addl	INCX, X

	cvtss2sd %xmm6, %xmm2
	mulsd	 %xmm2, %xmm2
	addsd	 %xmm2, %xmm0

	movss	(X), %xmm7
	addl	INCX, X

	cvtss2sd %xmm7, %xmm2
	mulsd	 %xmm2, %xmm2
	addsd	 %xmm2, %xmm1

	decl	I
	jg	.L41
	ALIGN_3

.L44:
	testl	$4,  M
	je	.L45

	movss	(X), %xmm4
	addl	INCX, X

	cvtss2sd %xmm4, %xmm2
	mulsd	 %xmm2, %xmm2
	addsd	 %xmm2, %xmm0

	movss	(X), %xmm5
	addl	INCX, X

	cvtss2sd %xmm5, %xmm2
	mulsd	 %xmm2, %xmm2
	addsd	 %xmm2, %xmm1

	movss	(X), %xmm6
	addl	INCX, X

	cvtss2sd %xmm6, %xmm2
	mulsd	 %xmm2, %xmm2
	addsd	 %xmm2, %xmm0

	movss	(X), %xmm7
	addl	INCX, X

	cvtss2sd %xmm7, %xmm2
	mulsd	 %xmm2, %xmm2
	addsd	 %xmm2, %xmm1
	ALIGN_3

.L45:
	testl	$2,  M
	je	.L46

	movss	(X), %xmm4
	addl	INCX, X

	cvtss2sd %xmm4, %xmm2
	mulsd	 %xmm2, %xmm2
	addsd	 %xmm2, %xmm0

	movss	(X), %xmm5
	addl	INCX, X

	cvtss2sd %xmm5, %xmm2
	mulsd	 %xmm2, %xmm2
	addsd	 %xmm2, %xmm1
	ALIGN_3

.L46:
	testl	$1,  M
	je	.L998

	movss	(X), %xmm4
	cvtss2sd %xmm4, %xmm2
	mulsd	 %xmm2, %xmm2
	addsd	 %xmm2, %xmm0
	ALIGN_4

.L998:
	addpd	%xmm1, %xmm0

#ifndef HAVE_SSE3
	movapd	%xmm0, %xmm1
	unpckhpd	%xmm0, %xmm0
	addsd	%xmm1, %xmm0
#else
	haddpd	%xmm0, %xmm0
#endif
	ALIGN_4

.L999:
	sqrtsd	%xmm0,  %xmm0

	cvtsd2ss %xmm0, %xmm0

	movss	 %xmm0, STACK_M
	flds	 STACK_M

	popl	%ebx
	popl	%esi
	ret

	EPILOGUE
