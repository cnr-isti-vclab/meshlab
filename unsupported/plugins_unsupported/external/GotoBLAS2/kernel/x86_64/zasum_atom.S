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
	xorps	 %xmm13, %xmm13

	cmpq	$2 * SIZE, INCX
	jne	.L20

	addq	M, M

	testq	$SIZE, X
	je	.L05

	movsd	(X), %xmm0
	addq	$SIZE, X
	andps	%xmm15, %xmm0
	decq	M
	ALIGN_3

.L05:
	subq	$-16 * SIZE, X

	movq	M,  I
	sarq	$4, I
	jle	.L12

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
	addsd	%xmm13, %xmm3
	pshufd	$0x4e, %xmm4, %xmm12
	addsd	%xmm4, %xmm0
	movaps	  0 * SIZE(X), %xmm4

	andps	%xmm15, %xmm5
	addsd	%xmm12, %xmm1
	pshufd	$0x4e, %xmm5, %xmm13
	addsd	%xmm5, %xmm2
	movaps	  2 * SIZE(X), %xmm5

	andps	%xmm15, %xmm6
	addsd	%xmm13, %xmm3
	pshufd	$0x4e, %xmm6, %xmm12
	addsd	%xmm6, %xmm0
	movaps	  4 * SIZE(X), %xmm6

	andps	%xmm15, %xmm7
	addsd	%xmm12, %xmm1
	pshufd	$0x4e, %xmm7, %xmm13
	addsd	%xmm7, %xmm2
	movaps	  6 * SIZE(X), %xmm7

#if defined(PREFETCH) && !defined(FETCH128)
	PREFETCH (PREFETCHSIZE +  64) - PREOFFSET(X)
#endif

	andps	%xmm15, %xmm8
	addsd	%xmm13, %xmm3
	pshufd	$0x4e, %xmm8, %xmm12
	addsd	%xmm8, %xmm0
	movaps	  8 * SIZE(X), %xmm8

	andps	%xmm15, %xmm9
	addsd	%xmm12, %xmm1
	pshufd	$0x4e, %xmm9, %xmm13
	addsd	%xmm9, %xmm2
	movaps	 10 * SIZE(X), %xmm9

	andps	%xmm15, %xmm10
	addsd	%xmm13, %xmm3
	pshufd	$0x4e, %xmm10, %xmm12
	addsd	%xmm10, %xmm0
	movaps	 12 * SIZE(X), %xmm10

	andps	%xmm15, %xmm11
	addsd	%xmm12, %xmm1
	pshufd	$0x4e, %xmm11, %xmm13
	addsd	%xmm11, %xmm2
	movaps	 14 * SIZE(X), %xmm11

	subq	$-16 * SIZE, X
	decq	I
	jg	.L10
	ALIGN_4

.L11:
	andps	%xmm15, %xmm4
	addsd	%xmm13, %xmm3
	pshufd	$0x4e, %xmm4, %xmm12
	addsd	%xmm4, %xmm0

	andps	%xmm15, %xmm5
	addsd	%xmm12, %xmm1
	pshufd	$0x4e, %xmm5, %xmm13
	addsd	%xmm5, %xmm2

	andps	%xmm15, %xmm6
	addsd	%xmm13, %xmm3
	pshufd	$0x4e, %xmm6, %xmm12
	addsd	%xmm6, %xmm0

	andps	%xmm15, %xmm7
	addsd	%xmm12, %xmm1
	pshufd	$0x4e, %xmm7, %xmm13
	addsd	%xmm7, %xmm2

	andps	%xmm15, %xmm8
	addsd	%xmm13, %xmm3
	pshufd	$0x4e, %xmm8, %xmm12
	addsd	%xmm8, %xmm0

	andps	%xmm15, %xmm9
	addsd	%xmm12, %xmm1
	pshufd	$0x4e, %xmm9, %xmm13
	addsd	%xmm9, %xmm2

	andps	%xmm15, %xmm10
	addsd	%xmm13, %xmm3
	pshufd	$0x4e, %xmm10, %xmm12
	addsd	%xmm10, %xmm0

	andps	%xmm15, %xmm11
	addsd	%xmm12, %xmm1
	pshufd	$0x4e, %xmm11, %xmm13
	addsd	%xmm11, %xmm2

	addsd	%xmm13, %xmm3
	subq	$-16 * SIZE, X
	ALIGN_3

.L12:
	andq	$15,  M
	jle	.L998

	testq	$8, M
	je	.L13

	movaps	-16 * SIZE(X), %xmm4
	movaps	-14 * SIZE(X), %xmm5
	movaps	-12 * SIZE(X), %xmm6
	movaps	-10 * SIZE(X), %xmm7
	addq	$8 * SIZE, X

	andps	%xmm15, %xmm4
	pshufd	$0x4e, %xmm4, %xmm12
	addsd	%xmm4, %xmm0
	andps	%xmm15, %xmm5
	addsd	%xmm12, %xmm1
	pshufd	$0x4e, %xmm5, %xmm13
	addsd	%xmm5, %xmm2
	addsd	%xmm13, %xmm3
	andps	%xmm15, %xmm6
	pshufd	$0x4e, %xmm6, %xmm12
	addsd	%xmm6, %xmm0
	andps	%xmm15, %xmm7
	addsd	%xmm12, %xmm1
	pshufd	$0x4e, %xmm7, %xmm13
	addsd	%xmm7, %xmm2
	addsd	%xmm13, %xmm3
	ALIGN_3

.L13:
	testq	$4, M
	je	.L14

	movaps	-16 * SIZE(X), %xmm4
	movaps	-14 * SIZE(X), %xmm5
	addq	$4 * SIZE, X

	andps	%xmm15, %xmm4
	pshufd	$0x4e, %xmm4, %xmm12
	addsd	%xmm4, %xmm0
	andps	%xmm15, %xmm5
	addsd	%xmm12, %xmm1
	pshufd	$0x4e, %xmm5, %xmm13
	addsd	%xmm5, %xmm2
	addsd	%xmm13, %xmm3
	ALIGN_3	

.L14:
	testq	$2, M
	je	.L15

	movaps	-16 * SIZE(X), %xmm4
	addq	$2 * SIZE, X
	andps	%xmm15, %xmm4

	pshufd	$0x4e, %xmm4, %xmm5
	addsd	%xmm4, %xmm2
	addsd	%xmm5, %xmm3
	ALIGN_3	
	
.L15:
	testq	$1, M
	je	.L998

	movsd	-16 * SIZE(X), %xmm4
	andps	%xmm15, %xmm4
	addsd	%xmm4, %xmm0
	jmp	.L998
	ALIGN_3

.L20:
	movq	M,  I
	sarq	$2, I
	jle	.L25

	movsd	0 * SIZE(X), %xmm4
	movsd	1 * SIZE(X), %xmm5
	addq	INCX, X
	movsd	0 * SIZE(X), %xmm6
	movsd	1 * SIZE(X), %xmm7
	addq	INCX, X

	movsd	0 * SIZE(X), %xmm8
	movsd	1 * SIZE(X), %xmm9
	addq	INCX, X
	movsd	0 * SIZE(X), %xmm10
	movsd	1 * SIZE(X), %xmm11

	decq	I
	jle	.L23
	ALIGN_4
	
.L22:
	andps	%xmm15, %xmm4
	addq	INCX, X
	addsd	%xmm4,  %xmm0
	movsd	 0 * SIZE(X), %xmm4
	andps	%xmm15, %xmm5
	addsd	%xmm5,  %xmm1
	movsd	 1 * SIZE(X), %xmm5
	andps	%xmm15, %xmm6
	addq	INCX, X
	addsd	%xmm6,  %xmm2
	movsd	 0 * SIZE(X), %xmm6
	andps	%xmm15, %xmm7
	addsd	%xmm7,  %xmm3
	movsd	 1 * SIZE(X), %xmm7

	andps	%xmm15, %xmm8
	addq	INCX, X
	addsd	%xmm8,  %xmm0
	movsd	 0 * SIZE(X), %xmm8
	andps	%xmm15, %xmm9
	addsd	%xmm9,  %xmm1
	movsd	 1 * SIZE(X), %xmm9
	andps	%xmm15, %xmm10
	addq	INCX, X
	addsd	%xmm10, %xmm2
	movsd	 0 * SIZE(X), %xmm10
	andps	%xmm15, %xmm11
	addsd	%xmm11, %xmm3
	movsd	 1 * SIZE(X), %xmm11

	decq	I
	jg	.L22
	ALIGN_4

.L23:
	andps	%xmm15, %xmm4
	addq	INCX, X
	addsd	%xmm4,  %xmm0
	andps	%xmm15, %xmm5
	addsd	%xmm5,  %xmm1
	andps	%xmm15, %xmm6
	addsd	%xmm6,  %xmm2
	andps	%xmm15, %xmm7
	addsd	%xmm7,  %xmm3

	andps	%xmm15, %xmm8
	addsd	%xmm8,  %xmm0
	andps	%xmm15, %xmm9
	addsd	%xmm9,  %xmm1
	andps	%xmm15, %xmm10
	addsd	%xmm10, %xmm2
	andps	%xmm15, %xmm11
	addsd	%xmm11, %xmm3
	ALIGN_3

.L25:
	testq	$2, M
	je	.L26

	movsd	0 * SIZE(X), %xmm4
	movsd	1 * SIZE(X), %xmm5
	addq	INCX, X
	movsd	0 * SIZE(X), %xmm6
	andps	%xmm15, %xmm4
	addsd	%xmm4,  %xmm0
	movsd	1 * SIZE(X), %xmm7
	andps	%xmm15, %xmm5
	addsd	%xmm5,  %xmm1
	addq	INCX, X

	andps	%xmm15, %xmm6
	addsd	%xmm6,  %xmm2
	andps	%xmm15, %xmm7
	addsd	%xmm7,  %xmm3
	ALIGN_3	

.L26:
	testq	$1, M
	je	.L998

	movsd	0 * SIZE(X), %xmm4
	movsd	1 * SIZE(X), %xmm5
	addq	INCX, X

	andps	%xmm15, %xmm4
	andps	%xmm15, %xmm5

	addsd	%xmm4, %xmm0
	addsd	%xmm5, %xmm1
	ALIGN_3

.L998:
	addsd	%xmm1, %xmm0
	addsd	%xmm3, %xmm2
	addsd	%xmm2, %xmm0
	ALIGN_4

.L999:
	RESTOREREGISTERS
	
	ret

	EPILOGUE

