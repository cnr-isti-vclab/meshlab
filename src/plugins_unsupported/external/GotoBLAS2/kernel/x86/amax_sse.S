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
	
#define STACK	16
#define ARGS	 0
	
#define STACK_M		 4 + STACK + ARGS(%esp)
#define STACK_X		 8 + STACK + ARGS(%esp)
#define STACK_INCX	12 + STACK + ARGS(%esp)

#define	M	%ebx
#define X	%ecx
#define INCX	%edx
#define I	%eax
	
#ifdef USE_MIN
#define maxps	minps
#define maxss	minss
#endif

#include "l1param.h"

	PROLOGUE

	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx

	PROFCODE

	movl	STACK_M, M
	movl	STACK_X, X
	movl	STACK_INCX, INCX

#ifdef F_INTERFACE
	movl	(M), M
	movl	(INCX), INCX
#endif

	xorps	%xmm0, %xmm0
	leal	(, INCX, SIZE), INCX

	testl	M, M
	jle	.L999

#ifdef USE_ABS
#ifndef HAVE_SSE2
	subl	$8, %esp
	movl	$0x7fffffff, (%esp)
	movss	(%esp), %xmm3
	shufps	$0, %xmm3, %xmm3
	addl	$8, %esp
#else
	pcmpeqb	%xmm3, %xmm3
	psrld	$1, %xmm3
#endif
#endif

	movss	(X), %xmm0
	shufps	$0,  %xmm0, %xmm0
#ifdef USE_ABS
	andps	%xmm3, %xmm0
#endif
	movaps	%xmm0, %xmm1
	addl	INCX, X
	decl	M
	jle	.L999

	cmpl	$SIZE, INCX
	jne	.L40

	subl	$-32 * SIZE, X

	cmpl	$3, M
	jle	.L17

	testl	$SIZE, X
	je	.L05

	movss	-32 * SIZE(X), %xmm4
	addl	$SIZE, X
	shufps	$0, %xmm4, %xmm4
#ifdef USE_ABS
	andps	%xmm3, %xmm4
#endif
	maxps	%xmm4, %xmm0

	decl	M
	ALIGN_3

.L05:
	testl	$2 * SIZE, X
	je	.L06

	movsd	-32 * SIZE(X), %xmm4
	addl	$2 * SIZE, X
	unpcklps  %xmm4, %xmm4
#ifdef USE_ABS
	andps	%xmm3, %xmm4
#endif
	maxps	%xmm4, %xmm1

	subl	$2, M
	ALIGN_3

.L06:
	movl	M,  I
	sarl	$5, I
	jle	.L15

	movaps	-32 * SIZE(X), %xmm4
	movaps	-28 * SIZE(X), %xmm5
	movaps	-24 * SIZE(X), %xmm6
	movaps	-20 * SIZE(X), %xmm7

	decl	I
	jle	.L12
	ALIGN_4
	
.L11:
#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

#ifdef USE_ABS
	andps	%xmm3, %xmm4
#endif
	maxps	%xmm4, %xmm0
	movaps	-16 * SIZE(X), %xmm4

#ifdef USE_ABS
	andps	%xmm3, %xmm5
#endif
	maxps	%xmm5, %xmm1
	movaps	-12 * SIZE(X), %xmm5

#ifdef USE_ABS
	andps	%xmm3, %xmm6
#endif
	maxps	%xmm6, %xmm0
	movaps	 -8 * SIZE(X), %xmm6

#ifdef USE_ABS
	andps	%xmm3, %xmm7
#endif
	maxps	%xmm7, %xmm1
	movaps	 -4 * SIZE(X), %xmm7

#if defined(PREFETCH) && !defined(FETCH128)
	PREFETCH (PREFETCHSIZE + 64) - PREOFFSET(X)
#endif

#ifdef USE_ABS
	andps	%xmm3, %xmm4
#endif
	maxps	%xmm4, %xmm0
	movaps	  0 * SIZE(X), %xmm4

#ifdef USE_ABS
	andps	%xmm3, %xmm5
#endif
	maxps	%xmm5, %xmm1
	movaps	  4 * SIZE(X), %xmm5

#ifdef USE_ABS
	andps	%xmm3, %xmm6
#endif
	maxps	%xmm6, %xmm0
	movaps	  8 * SIZE(X), %xmm6

#ifdef USE_ABS
	andps	%xmm3, %xmm7
#endif
	maxps	%xmm7, %xmm1
	movaps	 12 * SIZE(X), %xmm7

	subl	$-32 * SIZE, X
	decl	I
	jg	.L11
	ALIGN_4

.L12:
#ifdef USE_ABS
	andps	%xmm3, %xmm4
#endif
	maxps	%xmm4, %xmm0
	movaps	-16 * SIZE(X), %xmm4

#ifdef USE_ABS
	andps	%xmm3, %xmm5
#endif
	maxps	%xmm5, %xmm1
	movaps	-12 * SIZE(X), %xmm5

#ifdef USE_ABS
	andps	%xmm3, %xmm6
#endif
	maxps	%xmm6, %xmm0
	movaps	 -8 * SIZE(X), %xmm6

#ifdef USE_ABS
	andps	%xmm3, %xmm7
#endif
	maxps	%xmm7, %xmm1
	movaps	 -4 * SIZE(X), %xmm7

#ifdef USE_ABS
	andps	%xmm3, %xmm4
#endif
	maxps	%xmm4, %xmm0

#ifdef USE_ABS
	andps	%xmm3, %xmm5
#endif
	maxps	%xmm5, %xmm1

#ifdef USE_ABS
	andps	%xmm3, %xmm6
#endif
	maxps	%xmm6, %xmm0

#ifdef USE_ABS
	andps	%xmm3, %xmm7
#endif
	maxps	%xmm7, %xmm1

	subl	$-32 * SIZE, X
	ALIGN_3


.L15:
	testl	$16, M
	je	.L16

	movaps	-32 * SIZE(X), %xmm4
#ifdef USE_ABS
	andps	%xmm3, %xmm4
#endif
	maxps	%xmm4, %xmm0

	movaps	-28 * SIZE(X), %xmm5
#ifdef USE_ABS
	andps	%xmm3, %xmm5
#endif
	maxps	%xmm5, %xmm1

	movaps	-24 * SIZE(X), %xmm6
#ifdef USE_ABS
	andps	%xmm3, %xmm6
#endif
	maxps	%xmm6, %xmm0

	movaps	-20 * SIZE(X), %xmm7
#ifdef USE_ABS
	andps	%xmm3, %xmm7
#endif
	maxps	%xmm7, %xmm1

	addl	$16 * SIZE, X
	ALIGN_3

.L16:
	testl	$8, M
	je	.L17

	movaps	-32 * SIZE(X), %xmm4
#ifdef USE_ABS
	andps	%xmm3, %xmm4
#endif
	maxps	%xmm4, %xmm0

	movaps	-28 * SIZE(X), %xmm5
#ifdef USE_ABS
	andps	%xmm3, %xmm5
#endif
	maxps	%xmm5, %xmm1
	addl	$8 * SIZE, X
	ALIGN_3

.L17:
	testl	$4, M
	je	.L18

	movaps	-32 * SIZE(X), %xmm4
#ifdef USE_ABS
	andps	%xmm3, %xmm4
#endif
	maxps	%xmm4, %xmm0
	addl	$4 * SIZE, X
	ALIGN_3	

.L18:
	testl	$2, M
	je	.L19

	movsd	-32 * SIZE(X), %xmm4
	unpcklps %xmm4, %xmm4
#ifdef USE_ABS
	andps	%xmm3, %xmm4
#endif
	maxps	%xmm4, %xmm1
	addl	$2 * SIZE, X
	ALIGN_3
	
.L19:
	testl	$1, M
	je	.L998

	movss	-32 * SIZE(X), %xmm4
#ifdef USE_ABS
	andps	%xmm3, %xmm4
#endif
	maxss	%xmm4, %xmm0
	jmp	.L998
	ALIGN_3

.L40:
	movl	M,  I
	sarl	$3, I
	jle	.L45
	ALIGN_4
	
.L41:
	movss	(X), %xmm4
	addl	INCX, X
#ifdef USE_ABS
	andps	%xmm3, %xmm4
#endif
	maxss	%xmm4, %xmm0

	movss	(X), %xmm5
	addl	INCX, X
#ifdef USE_ABS
	andps	%xmm3, %xmm5
#endif
	maxss	%xmm5, %xmm1

	movss	(X), %xmm6
	addl	INCX, X
#ifdef USE_ABS
	andps	%xmm3, %xmm6
#endif
	maxss	%xmm6, %xmm0

	movss	(X), %xmm7
	addl	INCX, X
#ifdef USE_ABS
	andps	%xmm3, %xmm7
#endif
	maxss	%xmm7, %xmm1

	movss	(X), %xmm4
	addl	INCX, X
#ifdef USE_ABS
	andps	%xmm3, %xmm4
#endif
	maxss	%xmm4, %xmm0

	movss	(X), %xmm5
	addl	INCX, X
#ifdef USE_ABS
	andps	%xmm3, %xmm5
#endif
	maxss	%xmm5, %xmm1

	movss	(X), %xmm6
	addl	INCX, X
#ifdef USE_ABS
	andps	%xmm3, %xmm6
#endif
	maxss	%xmm6, %xmm0

	movss	(X), %xmm7
	addl	INCX, X
#ifdef USE_ABS
	andps	%xmm3, %xmm7
#endif
	maxss	%xmm7, %xmm1

	decl	I
	jg	.L41
	ALIGN_4

.L45:
	testl	$4, M
	je	.L46

	movss	(X), %xmm4
	addl	INCX, X
#ifdef USE_ABS
	andps	%xmm3, %xmm4
#endif
	maxss	%xmm4, %xmm0

	movss	(X), %xmm5
	addl	INCX, X
#ifdef USE_ABS
	andps	%xmm3, %xmm5
#endif
	maxss	%xmm5, %xmm1

	movss	(X), %xmm6
	addl	INCX, X
#ifdef USE_ABS
	andps	%xmm3, %xmm6
#endif
	maxss	%xmm6, %xmm0

	movss	(X), %xmm7
	addl	INCX, X
#ifdef USE_ABS
	andps	%xmm3, %xmm7
#endif
	maxss	%xmm7, %xmm1
	ALIGN_3	

.L46:
	testl	$2, M
	je	.L47

	movss	(X), %xmm4
	addl	INCX, X
#ifdef USE_ABS
	andps	%xmm3, %xmm4
#endif
	maxss	%xmm4, %xmm0

	movss	(X), %xmm5
	addl	INCX, X
#ifdef USE_ABS
	andps	%xmm3, %xmm5
#endif
	maxss	%xmm5, %xmm1
	ALIGN_3
	
.L47:
	testl	$1, M
	je	.L998

	movss	(X), %xmm4
	addl	INCX, X
#ifdef USE_ABS
	andps	%xmm3, %xmm4
#endif
	maxss	%xmm4, %xmm0
	ALIGN_4

.L998:
	maxps	%xmm1, %xmm0
	movaps	%xmm0, %xmm1
	movhlps %xmm0, %xmm0
	maxps	%xmm1, %xmm0
	movaps	%xmm0, %xmm1
	shufps	$1, %xmm0, %xmm0
	maxss	%xmm1, %xmm0
	ALIGN_4
	
.L999:
	RESTOREREGISTERS

	subl	$8, %esp
	movss	%xmm0, (%esp)
	flds	(%esp)
	addl	$8, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret

	EPILOGUE
