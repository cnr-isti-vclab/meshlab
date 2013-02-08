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
	psrlq	$1, %xmm15

	salq	$ZBASE_SHIFT, INCX

	cmpq	$2 * SIZE, INCX
	jne	.L40

	subq	$-16 * SIZE, X
	addq	M, M

	testq	$SIZE, X
	je	.L05

#ifdef movsd
	xorps	%xmm0, %xmm0
#endif
	movsd	-16 * SIZE(X), %xmm0
	addq	$SIZE, X

	andps	%xmm15, %xmm0
	subq	$1, M
	jle	.L999
	ALIGN_3

.L05:
	movq	M,  I
	sarq	$4, I
	jle	.L20

	movaps	-16 * SIZE(X), %xmm4
	movaps	-14 * SIZE(X), %xmm5
	movaps	-12 * SIZE(X), %xmm6
	movaps	-10 * SIZE(X), %xmm7

	movaps	 -8 * SIZE(X), %xmm8
	movaps	 -6 * SIZE(X), %xmm9
	movaps	 -4 * SIZE(X), %xmm10
	movaps	 -2 * SIZE(X), %xmm11

	decq	I
	jle	.L11
	ALIGN_4
	
.L10:
#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	andps	%xmm15, %xmm4
	addpd	%xmm4,  %xmm0
	movaps	  0 * SIZE(X), %xmm4

	andps	%xmm15, %xmm5
	addpd	%xmm5,  %xmm1
	movaps	  2 * SIZE(X), %xmm5

	andps	%xmm15, %xmm6
	addpd	%xmm6,  %xmm2
	movaps	  4 * SIZE(X), %xmm6

	andps	%xmm15, %xmm7
	addpd	%xmm7,  %xmm3
	movaps	  6 * SIZE(X), %xmm7

#if defined(PREFETCH) && !defined(FETCH128)
	PREFETCH (PREFETCHSIZE +  64) - PREOFFSET(X)
#endif

	andps	%xmm15, %xmm8
	addpd	%xmm8,  %xmm0
	movaps	  8 * SIZE(X), %xmm8

	andps	%xmm15, %xmm9
	addpd	%xmm9,  %xmm1
	movaps	 10 * SIZE(X), %xmm9

	andps	%xmm15, %xmm10
	addpd	%xmm10, %xmm2
	movaps	 12 * SIZE(X), %xmm10

	andps	%xmm15, %xmm11
	addpd	%xmm11, %xmm3
	movaps	 14 * SIZE(X), %xmm11

	subq	$-16 * SIZE, X
	decq	I
	jg	.L10
	ALIGN_4

.L11:
	andps	%xmm15, %xmm4
	andps	%xmm15, %xmm5
	andps	%xmm15, %xmm6
	andps	%xmm15, %xmm7

	addpd	%xmm4,  %xmm0
	addpd	%xmm5,  %xmm1
	addpd	%xmm6,  %xmm2
	addpd	%xmm7,  %xmm3

	andps	%xmm15, %xmm8
	andps	%xmm15, %xmm9
	andps	%xmm15, %xmm10
	andps	%xmm15, %xmm11

	addpd	%xmm8,  %xmm0
	addpd	%xmm9,  %xmm1
	addpd	%xmm10, %xmm2
	addpd	%xmm11, %xmm3

	subq	$-16 * SIZE, X
	ALIGN_3

.L20:
	andq	$15,  M
	jle	.L998

	testq	$8, M
	je	.L21

	movaps	-16 * SIZE(X), %xmm4
	movaps	-14 * SIZE(X), %xmm5
	movaps	-12 * SIZE(X), %xmm6
	movaps	-10 * SIZE(X), %xmm7

	andps	%xmm15, %xmm4
	andps	%xmm15, %xmm5
	andps	%xmm15, %xmm6
	andps	%xmm15, %xmm7

	addpd	%xmm4, %xmm0
	addpd	%xmm5, %xmm1
	addpd	%xmm6, %xmm2
	addpd	%xmm7, %xmm3
	addq	$8 * SIZE, X
	ALIGN_3

.L21:
	testq	$4, M
	je	.L22

	movaps	-16 * SIZE(X), %xmm4
	movaps	-14 * SIZE(X), %xmm5

	andps	%xmm15, %xmm4
	andps	%xmm15, %xmm5
	addpd	%xmm4, %xmm0
	addpd	%xmm5, %xmm1

	addq	$4 * SIZE, X
	ALIGN_3	

.L22:
	testq	$2, M
	je	.L23

	movaps	-16 * SIZE(X), %xmm6
	andps	%xmm15, %xmm6
	addpd	%xmm6, %xmm3
	addq	$2 * SIZE, X
	
.L23:
	testq	$1, M
	je	.L998

#ifdef movsd
	xorps	%xmm4, %xmm4
#endif
	movsd	-16 * SIZE(X), %xmm4
	andps	%xmm15, %xmm4
	addsd	%xmm4, %xmm0
	jmp	.L998
	ALIGN_3


.L40:
	movq	M,  I
	sarq	$2, I
	jle	.L60
	ALIGN_4
	
.L50:
#if defined(OPTERON) || defined(BARCELONA) || defined(SHANGHAI)
	prefetcht0	PREFETCHSIZE * SIZE(X)
#endif

#ifdef PENTIUM4
	prefetchnta	PREFETCHSIZE * SIZE(X)
#endif

	movsd	0 * SIZE(X), %xmm4
	movhpd	1 * SIZE(X), %xmm4
	addq	INCX, X
	andpd	%xmm15, %xmm4
	addpd	%xmm4, %xmm0

	movsd	0 * SIZE(X), %xmm5
	movhpd	1 * SIZE(X), %xmm5
	addq	INCX, X
	andpd	%xmm15, %xmm5
	addpd	%xmm5, %xmm1

	movsd	0 * SIZE(X), %xmm6
	movhpd	1 * SIZE(X), %xmm6
	addq	INCX, X
	andpd	%xmm15, %xmm6
	addpd	%xmm6, %xmm2

	movsd	0 * SIZE(X), %xmm7
	movhpd	1 * SIZE(X), %xmm7
	addq	INCX, X
	andpd	%xmm15, %xmm7
	addpd	%xmm7, %xmm3

	decq	I
	jg	.L50
	ALIGN_4

.L60:
	andq	$3,  M
	jle	.L998
	ALIGN_4


.L61:
	movsd	0 * SIZE(X), %xmm4
	movhpd	1 * SIZE(X), %xmm4
	andpd	%xmm15, %xmm4
	addpd	%xmm4, %xmm0
	addq	INCX, X
	decq	M
	jg	.L61
	ALIGN_4

.L998:
	addpd	%xmm1, %xmm0
	addpd	%xmm3, %xmm2
	addpd	%xmm2, %xmm0

#ifndef HAVE_SSE3
	movhlps	%xmm0, %xmm1
	addsd	%xmm1, %xmm0
#else
	haddpd	%xmm0, %xmm0
#endif
	ALIGN_4

.L999:
	RESTOREREGISTERS
	
	ret

	EPILOGUE
