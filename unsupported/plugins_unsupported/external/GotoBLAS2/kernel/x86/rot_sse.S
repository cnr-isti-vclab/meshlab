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
#define STACK_C		24 + STACK + ARGS(%esp)
#define STACK_S		28 + STACK + ARGS(%esp)

#define N	%ebx
#define X	%esi
#define INCX	%ecx
#define Y	%edi
#define INCY	%edx

#define	I	%eax

#define C	%xmm6
#define S	%xmm7

#include "l1param.h"

	PROLOGUE
	PROFCODE

	pushl	%edi
	pushl	%esi
	pushl	%ebx

	movl	STACK_N,     N
	movl	STACK_X,     X
	movl	STACK_INCX,  INCX
	movl	STACK_Y,     Y
	movl	STACK_INCY,  INCY

	leal	(, INCX, SIZE), INCX	
	leal	(, INCY, SIZE), INCY	

	movss	STACK_C, C
	movss	STACK_S, S

	shufps	$0x0, C, C
	shufps	$0x0, S, S

	cmpl	$0, N
	jle	.L999

	cmpl	$SIZE, INCX
	jne	.L50
	cmpl	$SIZE, INCY
	jne	.L50

	testl	$SIZE, X
	je	.L05

	movss	0 * SIZE(Y), %xmm1
	movss	0 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulss	C, %xmm0
	mulss	S, %xmm1

	mulss	C, %xmm2
	mulss	S, %xmm3

	addss	%xmm1, %xmm0
	subss	%xmm3, %xmm2

	movss	%xmm0, 0 * SIZE(X)
	movss	%xmm2, 0 * SIZE(Y)

	addl	$1 * SIZE, X
	addl	$1 * SIZE, Y
	decl	N
	jle	.L999

.L05:
	testl	$2 * SIZE, X
	je	.L10

	cmpl	$1, N
	je	.L17

#ifdef movsd
	xorps	%xmm0, %xmm0
	xorps	%xmm1, %xmm1
#endif

	movsd	0 * SIZE(Y), %xmm1
	movsd	0 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movlps	%xmm0, 0 * SIZE(X)
	movlps	%xmm2, 0 * SIZE(Y)

	addl	$2 * SIZE, X
	addl	$2 * SIZE, Y
	subl	$2, N
	jle	.L999
	ALIGN_2

.L10:
	testl	$3 * SIZE, Y
	jne	.L20

	movl	N,  I
	sarl	$5, I
	jle	.L14
	ALIGN_3

.L11:
#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	movsd	0 * SIZE(Y), %xmm1
	movhps	2 * SIZE(Y), %xmm1
	movaps	0 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0, 0 * SIZE(X)
	movlps	%xmm2, 0 * SIZE(Y)
	movhps	%xmm2, 2 * SIZE(Y)

	movsd	4 * SIZE(Y), %xmm1
	movhps	6 * SIZE(Y), %xmm1
	movaps	4 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(Y)
#endif

	movaps	%xmm0, 4 * SIZE(X)
	movlps	%xmm2, 4 * SIZE(Y)
	movhps	%xmm2, 6 * SIZE(Y)

	movsd	 8 * SIZE(Y), %xmm1
	movhps	10 * SIZE(Y), %xmm1
	movaps	 8 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0,  8 * SIZE(X)
	movlps	%xmm2,  8 * SIZE(Y)
	movhps	%xmm2, 10 * SIZE(Y)

	movsd	12 * SIZE(Y), %xmm1
	movhps	14 * SIZE(Y), %xmm1
	movaps	12 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0, 12 * SIZE(X)
	movlps	%xmm2, 12 * SIZE(Y)
	movhps	%xmm2, 14 * SIZE(Y)

#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE + 64) - PREOFFSET(X)
#endif

	movsd	16 * SIZE(Y), %xmm1
	movhps	18 * SIZE(Y), %xmm1
	movaps	16 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0, 16 * SIZE(X)
	movlps	%xmm2, 16 * SIZE(Y)
	movhps	%xmm2, 18 * SIZE(Y)

	movsd	20 * SIZE(Y), %xmm1
	movhps	22 * SIZE(Y), %xmm1
	movaps	20 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0, 20 * SIZE(X)
	movlps	%xmm2, 20 * SIZE(Y)
	movhps	%xmm2, 22 * SIZE(Y)

#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE + 64) - PREOFFSET(Y)
#endif

	movsd	24 * SIZE(Y), %xmm1
	movhps	26 * SIZE(Y), %xmm1
	movaps	24 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0, 24 * SIZE(X)
	movlps	%xmm2, 24 * SIZE(Y)
	movhps	%xmm2, 26 * SIZE(Y)

	movsd	28 * SIZE(Y), %xmm1
	movhps	30 * SIZE(Y), %xmm1
	movaps	28 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0, 28 * SIZE(X)
	movlps	%xmm2, 28 * SIZE(Y)
	movhps	%xmm2, 30 * SIZE(Y)

	addl	$32 * SIZE, X
	addl	$32 * SIZE, Y

	decl	I
	jg	.L11
	ALIGN_3

.L14:
	testl	$31, N
	jle	.L999

	testl	$16, N
	jle	.L15

	movsd	0 * SIZE(Y), %xmm1
	movhps	2 * SIZE(Y), %xmm1
	movaps	0 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0, 0 * SIZE(X)
	movlps	%xmm2, 0 * SIZE(Y)
	movhps	%xmm2, 2 * SIZE(Y)

	movsd	4 * SIZE(Y), %xmm1
	movhps	6 * SIZE(Y), %xmm1
	movaps	4 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0, 4 * SIZE(X)
	movlps	%xmm2, 4 * SIZE(Y)
	movhps	%xmm2, 6 * SIZE(Y)

	movsd	 8 * SIZE(Y), %xmm1
	movhps	10 * SIZE(Y), %xmm1
	movaps	 8 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0,  8 * SIZE(X)
	movlps	%xmm2,  8 * SIZE(Y)
	movhps	%xmm2, 10 * SIZE(Y)

	movsd	12 * SIZE(Y), %xmm1
	movhps	14 * SIZE(Y), %xmm1
	movaps	12 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0, 12 * SIZE(X)
	movlps	%xmm2, 12 * SIZE(Y)
	movhps	%xmm2, 14 * SIZE(Y)
	
	addl	$16 * SIZE, X
	addl	$16 * SIZE, Y
	ALIGN_3

.L15:
	testl	$8, N
	jle	.L16

	movsd	0 * SIZE(Y), %xmm1
	movhps	2 * SIZE(Y), %xmm1
	movaps	0 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0, 0 * SIZE(X)
	movlps	%xmm2, 0 * SIZE(Y)
	movhps	%xmm2, 2 * SIZE(Y)

	movsd	4 * SIZE(Y), %xmm1
	movhps	6 * SIZE(Y), %xmm1
	movaps	4 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0, 4 * SIZE(X)
	movlps	%xmm2, 4 * SIZE(Y)
	movhps	%xmm2, 6 * SIZE(Y)

	addl	$8 * SIZE, X
	addl	$8 * SIZE, Y
	ALIGN_3

.L16:
	testl	$4, N
	jle	.L17

	movsd	0 * SIZE(Y), %xmm1
	movhps	2 * SIZE(Y), %xmm1
	movaps	0 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0, 0 * SIZE(X)
	movlps	%xmm2, 0 * SIZE(Y)
	movhps	%xmm2, 2 * SIZE(Y)

	addl	$4 * SIZE, X
	addl	$4 * SIZE, Y
	ALIGN_3

.L17:
	testl	$2, N
	jle	.L18

#ifdef movsd
	xorps	%xmm0, %xmm0
	xorps	%xmm1, %xmm1
#endif

	movsd	0 * SIZE(Y), %xmm1
	movsd	0 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movlps	%xmm0, 0 * SIZE(X)
	movlps	%xmm2, 0 * SIZE(Y)

	addl	$2 * SIZE, X
	addl	$2 * SIZE, Y
	ALIGN_3

.L18:
	testl	$1, N
	jle	.L999

	movss	0 * SIZE(Y), %xmm1
	movss	0 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulss	C, %xmm0
	mulss	S, %xmm1

	mulss	C, %xmm2
	mulss	S, %xmm3

	addss	%xmm1, %xmm0
	subss	%xmm3, %xmm2

	movss	%xmm0, 0 * SIZE(X)
	movss	%xmm2, 0 * SIZE(Y)
	jmp	.L999
	ALIGN_3

.L20:
	movl	N,  I
	sarl	$5, I
	jle	.L24
	ALIGN_3

.L21:
#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	movsd	0 * SIZE(Y), %xmm1
	movhps	2 * SIZE(Y), %xmm1
	movaps	0 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0, 0 * SIZE(X)
	movlps	%xmm2, 0 * SIZE(Y)
	movhps	%xmm2, 2 * SIZE(Y)

	movsd	4 * SIZE(Y), %xmm1
	movhps	6 * SIZE(Y), %xmm1
	movaps	4 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0, 4 * SIZE(X)
	movlps	%xmm2, 4 * SIZE(Y)
	movhps	%xmm2, 6 * SIZE(Y)

#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(Y)
#endif

	movsd	 8 * SIZE(Y), %xmm1
	movhps	10 * SIZE(Y), %xmm1
	movaps	 8 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0,  8 * SIZE(X)
	movlps	%xmm2,  8 * SIZE(Y)
	movhps	%xmm2, 10 * SIZE(Y)

	movsd	12 * SIZE(Y), %xmm1
	movhps	14 * SIZE(Y), %xmm1
	movaps	12 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0, 12 * SIZE(X)
	movlps	%xmm2, 12 * SIZE(Y)
	movhps	%xmm2, 14 * SIZE(Y)

#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE + 64) - PREOFFSET(X)
#endif

	movsd	16 * SIZE(Y), %xmm1
	movhps	18 * SIZE(Y), %xmm1
	movaps	16 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0, 16 * SIZE(X)
	movlps	%xmm2, 16 * SIZE(Y)
	movhps	%xmm2, 18 * SIZE(Y)

	movsd	20 * SIZE(Y), %xmm1
	movhps	22 * SIZE(Y), %xmm1
	movaps	20 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0, 20 * SIZE(X)
	movlps	%xmm2, 20 * SIZE(Y)
	movhps	%xmm2, 22 * SIZE(Y)

#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE + 64) - PREOFFSET(Y)
#endif

	movsd	24 * SIZE(Y), %xmm1
	movhps	26 * SIZE(Y), %xmm1
	movaps	24 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0, 24 * SIZE(X)
	movlps	%xmm2, 24 * SIZE(Y)
	movhps	%xmm2, 26 * SIZE(Y)

	movsd	28 * SIZE(Y), %xmm1
	movhps	30 * SIZE(Y), %xmm1
	movaps	28 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0, 28 * SIZE(X)
	movlps	%xmm2, 28 * SIZE(Y)
	movhps	%xmm2, 30 * SIZE(Y)

	addl	$32 * SIZE, X
	addl	$32 * SIZE, Y
	decl	I
	jg	.L21
	ALIGN_3

.L24:
	testl	$31, N
	jle	.L999

	testl	$16, N
	jle	.L25

	movsd	0 * SIZE(Y), %xmm1
	movhps	2 * SIZE(Y), %xmm1
	movaps	0 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0, 0 * SIZE(X)
	movlps	%xmm2, 0 * SIZE(Y)
	movhps	%xmm2, 2 * SIZE(Y)

	movsd	4 * SIZE(Y), %xmm1
	movhps	6 * SIZE(Y), %xmm1
	movaps	4 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0, 4 * SIZE(X)
	movlps	%xmm2, 4 * SIZE(Y)
	movhps	%xmm2, 6 * SIZE(Y)

	movsd	 8 * SIZE(Y), %xmm1
	movhps	10 * SIZE(Y), %xmm1
	movaps	 8 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0,  8 * SIZE(X)
	movlps	%xmm2,  8 * SIZE(Y)
	movhps	%xmm2, 10 * SIZE(Y)

	movsd	12 * SIZE(Y), %xmm1
	movhps	14 * SIZE(Y), %xmm1
	movaps	12 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0, 12 * SIZE(X)
	movlps	%xmm2, 12 * SIZE(Y)
	movhps	%xmm2, 14 * SIZE(Y)

	addl	$16 * SIZE, X
	addl	$16 * SIZE, Y
	ALIGN_3

.L25:
	testl	$8, N
	jle	.L26

	movsd	0 * SIZE(Y), %xmm1
	movhps	2 * SIZE(Y), %xmm1
	movaps	0 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0, 0 * SIZE(X)
	movlps	%xmm2, 0 * SIZE(Y)
	movhps	%xmm2, 2 * SIZE(Y)

	movsd	4 * SIZE(Y), %xmm1
	movhps	6 * SIZE(Y), %xmm1
	movaps	4 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0, 4 * SIZE(X)
	movlps	%xmm2, 4 * SIZE(Y)
	movhps	%xmm2, 6 * SIZE(Y)

	addl	$8 * SIZE, X
	addl	$8 * SIZE, Y
	ALIGN_3


.L26:
	testl	$4, N
	jle	.L27

	movsd	0 * SIZE(Y), %xmm1
	movhps	2 * SIZE(Y), %xmm1
	movaps	0 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movaps	%xmm0, 0 * SIZE(X)
	movlps	%xmm2, 0 * SIZE(Y)
	movhps	%xmm2, 2 * SIZE(Y)

	addl	$4 * SIZE, X
	addl	$4 * SIZE, Y
	ALIGN_3

.L27:
	testl	$2, N
	jle	.L28

#ifdef movsd
	xorps	%xmm0, %xmm0
	xorps	%xmm1, %xmm1
#endif

	movsd	0 * SIZE(Y), %xmm1
	movsd	0 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulps	C, %xmm0
	mulps	S, %xmm1

	mulps	C, %xmm2
	mulps	S, %xmm3

	addps	%xmm1, %xmm0
	subps	%xmm3, %xmm2

	movlps	%xmm0, 0 * SIZE(X)
	movlps	%xmm2, 0 * SIZE(Y)

	addl	$2 * SIZE, X
	addl	$2 * SIZE, Y
	ALIGN_3

.L28:
	testl	$1, N
	jle	.L999

	movss	0 * SIZE(Y), %xmm1
	movss	0 * SIZE(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulss	C, %xmm0
	mulss	S, %xmm1

	mulss	C, %xmm2
	mulss	S, %xmm3

	addss	%xmm1, %xmm0
	subss	%xmm3, %xmm2

	movss	%xmm0, 0 * SIZE(X)
	movss	%xmm2, 0 * SIZE(Y)
	jmp	.L999
	ALIGN_3

.L50:
	movl	N,  I
	sarl	$2, I
	jle	.L55
	ALIGN_3

.L53:
	movss	(Y), %xmm1
	movss	(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulss	C, %xmm0
	mulss	S, %xmm1

	mulss	C, %xmm2
	mulss	S, %xmm3

	addss	%xmm1, %xmm0
	subss	%xmm3, %xmm2

	movss	%xmm0, (X)
	movss	%xmm2, (Y)

	addl	INCX, X
	addl	INCY, Y

	movss	(Y), %xmm1
	movss	(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulss	C, %xmm0
	mulss	S, %xmm1

	mulss	C, %xmm2
	mulss	S, %xmm3

	addss	%xmm1, %xmm0
	subss	%xmm3, %xmm2

	movss	%xmm0, (X)
	movss	%xmm2, (Y)

	addl	INCX, X
	addl	INCY, Y

	movss	(Y), %xmm1
	movss	(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulss	C, %xmm0
	mulss	S, %xmm1

	mulss	C, %xmm2
	mulss	S, %xmm3

	addss	%xmm1, %xmm0
	subss	%xmm3, %xmm2

	movss	%xmm0, (X)
	movss	%xmm2, (Y)

	addl	INCX, X
	addl	INCY, Y

	movss	(Y), %xmm1
	movss	(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulss	C, %xmm0
	mulss	S, %xmm1

	mulss	C, %xmm2
	mulss	S, %xmm3

	addss	%xmm1, %xmm0
	subss	%xmm3, %xmm2

	movss	%xmm0, (X)
	movss	%xmm2, (Y)

	addl	INCX, X
	addl	INCY, Y

	decl	I
	jg	.L53
	ALIGN_3

.L55:
	movl	N,  I
	andl	$3, I
	jle	.L999
	ALIGN_3

.L56:
	movss	(Y), %xmm1
	movss	(X), %xmm0

	movaps	%xmm1, %xmm2
	movaps	%xmm0, %xmm3

	mulss	C, %xmm0
	mulss	S, %xmm1

	mulss	C, %xmm2
	mulss	S, %xmm3

	addss	%xmm1, %xmm0
	subss	%xmm3, %xmm2

	movss	%xmm0, (X)
	movss	%xmm2, (Y)

	addl	INCX, X
	addl	INCY, Y

	decl	I
	jg	.L56
	ALIGN_3

.L999:
	popl	%ebx
	popl	%esi
	popl	%edi

	ret

	EPILOGUE
