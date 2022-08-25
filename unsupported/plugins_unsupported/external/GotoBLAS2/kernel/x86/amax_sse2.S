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
#define maxpd	minpd
#define maxsd	minsd
#endif

#include "l1param.h"

	PROLOGUE

	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx

	PROFCODE

#if defined(F_INTERFACE_GFORT) || defined(F_INTERFACE_G95)
	EMMS
#endif

	movl	STACK_M, M
	movl	STACK_X, X
	movl	STACK_INCX, INCX

	xorps	%xmm0, %xmm0
	leal	(, INCX, SIZE), INCX

	testl	M, M
	jle	.L999

#ifdef USE_ABS
	pcmpeqb	%xmm3, %xmm3
	psrlq	$1, %xmm3
#endif

	movsd	(X), %xmm0
	addl	INCX, X
#ifdef USE_ABS
	andps	 %xmm3, %xmm0
#endif
	unpcklpd %xmm0, %xmm0
	movaps	 %xmm0, %xmm1
	decl	M
	jle	.L999

	cmpl	$SIZE, INCX
	jne	.L40

	subl	$-16 * SIZE, X

	testl	$SIZE, X
	je	.L05

	movsd	-16 * SIZE(X), %xmm4
#ifdef USE_ABS
	andps	 %xmm3, %xmm4
#endif
	unpcklpd  %xmm4, %xmm4
	maxpd	  %xmm4, %xmm0
	addl	$SIZE, X
	decl	M
	jle	.L998
	ALIGN_3

.L05:
	movl	M,  I
	sarl	$4, I
	jle	.L15

	movaps	-16 * SIZE(X), %xmm4
	movaps	-14 * SIZE(X), %xmm5
	movaps	-12 * SIZE(X), %xmm6
	movaps	-10 * SIZE(X), %xmm7

	decl	I
	jle	.L12
	ALIGN_4
	
.L11:
#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

#ifdef USE_ABS
	andps	 %xmm3, %xmm4
#endif
	maxpd	%xmm4, %xmm0
	movaps	 -8 * SIZE(X), %xmm4

#ifdef USE_ABS
	andps	 %xmm3, %xmm5
#endif
	maxpd	%xmm5, %xmm1
	movaps	 -6 * SIZE(X), %xmm5

#ifdef USE_ABS
	andps	 %xmm3, %xmm6
#endif
	maxpd	%xmm6, %xmm0
	movaps	 -4 * SIZE(X), %xmm6

#ifdef USE_ABS
	andps	 %xmm3, %xmm7
#endif
	maxpd	%xmm7, %xmm1
	movaps	 -2 * SIZE(X), %xmm7

#if defined(PREFETCH) && !defined(FETCH128)
	PREFETCH (PREFETCHSIZE +  64) - PREOFFSET(X)
#endif

#ifdef USE_ABS
	andps	 %xmm3, %xmm4
#endif
	maxpd	%xmm4, %xmm0
	movaps	  0 * SIZE(X), %xmm4

#ifdef USE_ABS
	andps	 %xmm3, %xmm5
#endif
	maxpd	%xmm5, %xmm1
	movaps	  2 * SIZE(X), %xmm5

#ifdef USE_ABS
	andps	 %xmm3, %xmm6
#endif
	maxpd	%xmm6, %xmm0
	movaps	  4 * SIZE(X), %xmm6

#ifdef USE_ABS
	andps	 %xmm3, %xmm7
#endif
	maxpd	%xmm7, %xmm1
	movaps	  6 * SIZE(X), %xmm7

	subl	$-16 * SIZE, X
	decl	I
	jg	.L11
	ALIGN_4

.L12:
#ifdef USE_ABS
	andps	 %xmm3, %xmm4
#endif
	maxpd	%xmm4, %xmm0
	movaps	 -8 * SIZE(X), %xmm4

#ifdef USE_ABS
	andps	 %xmm3, %xmm5
#endif
	maxpd	%xmm5, %xmm1
	movaps	 -6 * SIZE(X), %xmm5

#ifdef USE_ABS
	andps	 %xmm3, %xmm6
#endif
	maxpd	%xmm6, %xmm0
	movaps	 -4 * SIZE(X), %xmm6

#ifdef USE_ABS
	andps	 %xmm3, %xmm7
#endif
	maxpd	%xmm7, %xmm1
	movaps	 -2 * SIZE(X), %xmm7

#ifdef USE_ABS
	andps	 %xmm3, %xmm4
#endif
	maxpd	%xmm4, %xmm0

#ifdef USE_ABS
	andps	 %xmm3, %xmm5
#endif
	maxpd	%xmm5, %xmm1

#ifdef USE_ABS
	andps	 %xmm3, %xmm6
#endif
	maxpd	%xmm6, %xmm0

#ifdef USE_ABS
	andps	 %xmm3, %xmm7
#endif
	maxpd	%xmm7, %xmm1

	subl	$-16 * SIZE, X
	ALIGN_4

.L15:
	testl	$8, M
	jle	.L16

	movaps	-16 * SIZE(X), %xmm4
#ifdef USE_ABS
	andps	 %xmm3, %xmm4
#endif
	maxpd	%xmm4, %xmm0

	movaps	-14 * SIZE(X), %xmm5
#ifdef USE_ABS
	andps	 %xmm3, %xmm5
#endif
	maxpd	%xmm5, %xmm1

	movaps	-12 * SIZE(X), %xmm6
#ifdef USE_ABS
	andps	 %xmm3, %xmm6
#endif
	maxpd	%xmm6, %xmm0

	movaps	-10 * SIZE(X), %xmm7
#ifdef USE_ABS
	andps	 %xmm3, %xmm7
#endif
	maxpd	%xmm7, %xmm1
	addl	$8 * SIZE, X
	ALIGN_3

.L16:
	testl	$4, M
	jle	.L17

	movaps	-16 * SIZE(X), %xmm4
#ifdef USE_ABS
	andps	 %xmm3, %xmm4
#endif
	maxpd	%xmm4, %xmm0

	movaps	-14 * SIZE(X), %xmm5
#ifdef USE_ABS
	andps	 %xmm3, %xmm5
#endif
	maxpd	%xmm5, %xmm1

	addl	$4 * SIZE, X
	ALIGN_3	

.L17:
	testl	$2, M
	jle	.L18

	movaps	-16 * SIZE(X), %xmm4
#ifdef USE_ABS
	andps	 %xmm3, %xmm4
#endif
	maxpd	%xmm4, %xmm0
	addl	$2 * SIZE, X
	ALIGN_3	
	
.L18:
	testl	$1, M
	jle	.L998

	movsd	-16 * SIZE(X), %xmm4
	unpcklpd  %xmm4, %xmm4
#ifdef USE_ABS
	andps	 %xmm3, %xmm4
#endif
	maxpd	%xmm4, %xmm1
	jmp	.L998
	ALIGN_3

.L40:
	movl	M,  I
	sarl	$4, I
	jle	.L45
	ALIGN_4
	
.L41:
#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	movsd	(X), %xmm4
	addl	INCX, X
	movhps	(X), %xmm4
	addl	INCX, X
#ifdef USE_ABS
	andps	 %xmm3, %xmm4
#endif
	maxpd	%xmm4, %xmm0

	movsd	(X), %xmm5
	addl	INCX, X
	movhps	(X), %xmm5
	addl	INCX, X
#ifdef USE_ABS
	andps	 %xmm3, %xmm5
#endif
	maxpd	%xmm5, %xmm1

	movsd	(X), %xmm6
	addl	INCX, X
	movhps	(X), %xmm6
	addl	INCX, X
#ifdef USE_ABS
	andps	 %xmm3, %xmm6
#endif
	maxpd	%xmm6, %xmm0

	movsd	(X), %xmm7
	addl	INCX, X
	movhps	(X), %xmm7
	addl	INCX, X
#ifdef USE_ABS
	andps	 %xmm3, %xmm7
#endif
	maxpd	%xmm7, %xmm1

#if defined(PREFETCH) && !defined(FETCH128)
	PREFETCH (PREFETCHSIZE +  64) - PREOFFSET(X)
#endif

	movsd	(X), %xmm4
	addl	INCX, X
	movhps	(X), %xmm4
	addl	INCX, X
#ifdef USE_ABS
	andps	 %xmm3, %xmm4
#endif
	maxpd	%xmm4, %xmm0

	movsd	(X), %xmm5
	addl	INCX, X
	movhps	(X), %xmm5
	addl	INCX, X
#ifdef USE_ABS
	andps	 %xmm3, %xmm5
#endif
	maxpd	%xmm5, %xmm1

	movsd	(X), %xmm6
	addl	INCX, X
	movhps	(X), %xmm6
	addl	INCX, X
#ifdef USE_ABS
	andps	 %xmm3, %xmm6
#endif
	maxpd	%xmm6, %xmm0

	movsd	(X), %xmm7
	addl	INCX, X
	movhps	(X), %xmm7
	addl	INCX, X
#ifdef USE_ABS
	andps	 %xmm3, %xmm7
#endif
	maxpd	%xmm7, %xmm1

	decl	I
	jg	.L41
	ALIGN_4

.L45:
	andl	$15,  M
	jle	.L998

	testl	$8, M
	je	.L46

	movsd	(X), %xmm4
	addl	INCX, X
	movhps	(X), %xmm4
	addl	INCX, X
#ifdef USE_ABS
	andps	 %xmm3, %xmm4
#endif
	maxpd	%xmm4, %xmm0

	movsd	(X), %xmm5
	addl	INCX, X
	movhps	(X), %xmm5
	addl	INCX, X
#ifdef USE_ABS
	andps	 %xmm3, %xmm5
#endif
	maxpd	%xmm5, %xmm1

	movsd	(X), %xmm6
	addl	INCX, X
	movhps	(X), %xmm6
	addl	INCX, X
#ifdef USE_ABS
	andps	 %xmm3, %xmm6
#endif
	maxpd	%xmm6, %xmm0

	movsd	(X), %xmm7
	addl	INCX, X
	movhps	(X), %xmm7
	addl	INCX, X
#ifdef USE_ABS
	andps	 %xmm3, %xmm7
#endif
	maxpd	%xmm7, %xmm1
	ALIGN_3

.L46:
	testl	$4, M
	je	.L47

	movsd	(X), %xmm4
	addl	INCX, X
	movhps	(X), %xmm4
	addl	INCX, X
#ifdef USE_ABS
	andps	 %xmm3, %xmm4
#endif
	maxpd	%xmm4, %xmm0

	movsd	(X), %xmm5
	addl	INCX, X
	movhps	(X), %xmm5
	addl	INCX, X
#ifdef USE_ABS
	andps	 %xmm3, %xmm5
#endif
	maxpd	%xmm5, %xmm1
	ALIGN_3	

.L47:
	testl	$2, M
	je	.L48

	movsd	(X), %xmm6
	addl	INCX, X
	movhps	(X), %xmm6
	addl	INCX, X
#ifdef USE_ABS
	andps	 %xmm3, %xmm6
#endif
	maxpd	%xmm6, %xmm0
	ALIGN_3
	
.L48:
	testl	$1, M
	je	.L998

	movsd	(X), %xmm7
	unpcklpd  %xmm7, %xmm7
#ifdef USE_ABS
	andps	 %xmm3, %xmm7
#endif
	maxpd	%xmm7, %xmm1
	ALIGN_4

.L998:
	maxpd	%xmm1, %xmm0
	movaps	%xmm0, %xmm1
	unpckhpd %xmm0, %xmm0
	maxsd	%xmm1, %xmm0
	ALIGN_4

.L999:
	subl	$8, %esp
	movsd	%xmm0, (%esp)
	fldl	(%esp)
	addl	$8, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret

	EPILOGUE
