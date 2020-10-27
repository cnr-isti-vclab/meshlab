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

#ifdef movsd
#undef movsd
#endif

#ifdef PENTIUM3
#ifdef HAVE_SSE
#define PREFETCH	prefetcht0
#define PREFETCHW	prefetcht0
#define PREFETCHSIZE	(16 * 2)
#endif
#define movsd		movlps
#endif

#ifdef PENTIUM4
#define PREFETCH	prefetcht0
#define PREFETCHW	prefetcht0
#define PREFETCHSIZE	(16 * 4)
#endif

#if defined(CORE2) || defined(PENRYN) || defined(DUNNINGTON) || defined(NEHALEM)
#define PREFETCH	prefetcht0
#define PREFETCHW	prefetcht0
#define PREFETCHSIZE	(16 * 7)
#endif

#ifdef OPTERON
#define PREFETCH	prefetchnta
#define PREFETCHW	prefetchw
#define PREFETCHSIZE	(16 * 3)
#define movsd		movlps
#endif

#ifdef BARCELONA
#define PREFETCH	prefetchnta
#define PREFETCHW	prefetchw
#define PREFETCHSIZE	(16 * 5)
#endif

#ifdef ATOM
#define PREFETCH	prefetchnta
#define PREFETCHW	prefetcht0
#define PREFETCHSIZE	(16 * 6)
#endif

#ifdef NANO
#define PREFETCH	prefetcht0
#define PREFETCHSIZE	(16 * 4)
#endif

#define STACKSIZE	16

#define M		 4 + STACKSIZE(%esp)
#define N		 8 + STACKSIZE(%esp)
#define ALPHA		16 + STACKSIZE(%esp)
#define A		20 + STACKSIZE(%esp)
#define STACK_LDA	24 + STACKSIZE(%esp)
#define STACK_X		28 + STACKSIZE(%esp)
#define STACK_INCX	32 + STACKSIZE(%esp)
#define Y		36 + STACKSIZE(%esp)
#define STACK_INCY	40 + STACKSIZE(%esp)
#define BUFFER		44 + STACKSIZE(%esp)
	
#define I	%eax
#define J	%ebx

#define INCX	%ecx
#define INCY	J

#define A1	%esi
#define X	%edx
#define Y1	%edi
#define LDA	%ebp

	PROLOGUE

	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx

	PROFCODE

	movl	STACK_LDA,  LDA
	movl	STACK_X,    X
	movl	STACK_INCX, INCX

	leal	(,INCX, SIZE), INCX
	leal	(,LDA,  SIZE), LDA

	subl	$-32 * SIZE, A

	cmpl	$0, N
	jle	.L999
	cmpl	$0, M
	jle	.L999

	movl	BUFFER, Y1
	
	xorps	%xmm7, %xmm7

	movl	M,   %eax
	addl	$16, %eax
	sarl	$4,  %eax
	ALIGN_3

.L01:
	movaps	%xmm7,  0 * SIZE(Y1)
	movaps	%xmm7,  4 * SIZE(Y1)
	movaps	%xmm7,  8 * SIZE(Y1)
	movaps	%xmm7, 12 * SIZE(Y1)
	subl	$-16 * SIZE, Y1
	decl	%eax
	jg	.L01
	ALIGN_3

.L10:
	movl	N,  J
	sarl	$1, J
	jle	.L20
	ALIGN_3

.L11:
	movl	BUFFER, Y1
	addl	$32 * SIZE, Y1

	movl	A,  A1
	leal	(A1,  LDA, 2), %eax
	movl	%eax, A

	movss	(X), %xmm6
	addl	INCX, X
	movss	(X), %xmm7
	addl	INCX, X

	movss	ALPHA, %xmm0

	mulss	%xmm0, %xmm6
	mulss	%xmm0, %xmm7

	shufps	$0, %xmm6, %xmm6
	shufps	$0, %xmm7, %xmm7
	ALIGN_3

	movl	M,   I
	sarl	$4,  I
	jle	.L15

	movsd	 -32 * SIZE(A1), %xmm2
	movhps	 -30 * SIZE(A1), %xmm2
	movsd	 -28 * SIZE(A1), %xmm3
	movhps	 -26 * SIZE(A1), %xmm3

	movaps	 -32 * SIZE(Y1), %xmm0
	movaps	 -28 * SIZE(Y1), %xmm1

	movsd	 -32 * SIZE(A1, LDA), %xmm4
	movhps	 -30 * SIZE(A1, LDA), %xmm4
	movsd	 -28 * SIZE(A1, LDA), %xmm5
	movhps	 -26 * SIZE(A1, LDA), %xmm5

	decl	 I
	jle	 .L14
	ALIGN_3

.L13:
#ifdef PREFETCH
	PREFETCH	(PREFETCHSIZE + 0) * SIZE(A1)
#endif

	mulps	 %xmm6, %xmm2
	addps	 %xmm2, %xmm0
	movsd	 -24 * SIZE(A1), %xmm2
	movhps	 -22 * SIZE(A1), %xmm2
	mulps	 %xmm6, %xmm3
	addps	 %xmm3, %xmm1
	movsd	 -20 * SIZE(A1), %xmm3
	movhps	 -18 * SIZE(A1), %xmm3

	mulps	 %xmm7, %xmm4
	addps	 %xmm4, %xmm0
	movsd	 -24 * SIZE(A1, LDA), %xmm4
	movhps	 -22 * SIZE(A1, LDA), %xmm4

	movaps	 %xmm0,  -32 * SIZE(Y1)
	movaps	 -24 * SIZE(Y1), %xmm0

	mulps	 %xmm7, %xmm5
	addps	 %xmm5, %xmm1
	movsd	 -20 * SIZE(A1, LDA), %xmm5
	movhps	 -18 * SIZE(A1, LDA), %xmm5

	movaps	 %xmm1,  -28 * SIZE(Y1)
	movaps	 -20 * SIZE(Y1), %xmm1

#ifdef PREFETCH
	PREFETCH	(PREFETCHSIZE + 0) * SIZE(A1, LDA)
#endif

	mulps	 %xmm6, %xmm2
	addps	 %xmm2, %xmm0
	movsd	 -16 * SIZE(A1), %xmm2
	movhps	 -14 * SIZE(A1), %xmm2
	mulps	 %xmm6, %xmm3
	addps	 %xmm3, %xmm1
	movsd	 -12 * SIZE(A1), %xmm3
	movhps	 -10 * SIZE(A1), %xmm3

	mulps	 %xmm7, %xmm4
	addps	 %xmm4, %xmm0
	movsd	 -16 * SIZE(A1, LDA), %xmm4
	movhps	 -14 * SIZE(A1, LDA), %xmm4

	movaps	 %xmm0,  -24 * SIZE(Y1)
	movaps	 -16 * SIZE(Y1), %xmm0

	mulps	 %xmm7, %xmm5
	addps	 %xmm5, %xmm1
	movsd	 -12 * SIZE(A1, LDA), %xmm5
	movhps	 -10 * SIZE(A1, LDA), %xmm5

	movaps	 %xmm1,  -20 * SIZE(Y1)
	movaps	 -12 * SIZE(Y1), %xmm1

	subl	 $-16 * SIZE, A1
	subl	 $-16 * SIZE, Y1

	subl	 $1, I
	BRANCH
	jg	.L13
	ALIGN_3

.L14:
	mulps	 %xmm6, %xmm2
	addps	 %xmm2, %xmm0
	movsd	 -24 * SIZE(A1), %xmm2
	movhps	 -22 * SIZE(A1), %xmm2
	mulps	 %xmm6, %xmm3
	addps	 %xmm3, %xmm1
	movsd	 -20 * SIZE(A1), %xmm3
	movhps	 -18 * SIZE(A1), %xmm3

	mulps	 %xmm7, %xmm4
	addps	 %xmm4, %xmm0
	movsd	 -24 * SIZE(A1, LDA), %xmm4
	movhps	 -22 * SIZE(A1, LDA), %xmm4

	movaps	 %xmm0,  -32 * SIZE(Y1)
	movaps	 -24 * SIZE(Y1), %xmm0

	mulps	 %xmm7, %xmm5
	addps	 %xmm5, %xmm1
	movsd	 -20 * SIZE(A1, LDA), %xmm5
	movhps	 -18 * SIZE(A1, LDA), %xmm5

	movaps	 %xmm1,  -28 * SIZE(Y1)
	movaps	 -20 * SIZE(Y1), %xmm1

	mulps	 %xmm6, %xmm2
	addps	 %xmm2, %xmm0
	mulps	 %xmm6, %xmm3
	addps	 %xmm3, %xmm1

	mulps	 %xmm7, %xmm4
	addps	 %xmm4, %xmm0
	movaps	 %xmm0,  -24 * SIZE(Y1)
	mulps	 %xmm7, %xmm5
	addps	 %xmm5, %xmm1
	movaps	 %xmm1,  -20 * SIZE(Y1)

	subl	 $-16 * SIZE, A1
	subl	 $-16 * SIZE, Y1
	ALIGN_3

.L15:
	testl	$8, M
	je	.L16

	movsd	 -32 * SIZE(A1), %xmm2
	movhps	 -30 * SIZE(A1), %xmm2
	movsd	 -28 * SIZE(A1), %xmm3
	movhps	 -26 * SIZE(A1), %xmm3

	movaps	 -32 * SIZE(Y1), %xmm0
	movaps	 -28 * SIZE(Y1), %xmm1

	mulps	 %xmm6, %xmm2
	addps	 %xmm2, %xmm0
	mulps	 %xmm6, %xmm3
	addps	 %xmm3, %xmm1

	movsd	 -32 * SIZE(A1, LDA), %xmm4
	movhps	 -30 * SIZE(A1, LDA), %xmm4
	movsd	 -28 * SIZE(A1, LDA), %xmm5
	movhps	 -26 * SIZE(A1, LDA), %xmm5

	mulps	 %xmm7, %xmm4
	addps	 %xmm4, %xmm0
	mulps	 %xmm7, %xmm5
	addps	 %xmm5, %xmm1

	movaps	 %xmm0,  -32 * SIZE(Y1)
	movaps	 %xmm1,  -28 * SIZE(Y1)

	addl	 $8 * SIZE, A1
	addl	 $8 * SIZE, Y1
	ALIGN_3

.L16:
	testl	$4, M
	je	.L17

	movsd	 -32 * SIZE(A1), %xmm2
	movhps	 -30 * SIZE(A1), %xmm2
	movsd	 -32 * SIZE(A1, LDA), %xmm3
	movhps	 -30 * SIZE(A1, LDA), %xmm3

	movaps	 -32 * SIZE(Y1), %xmm0

	mulps	 %xmm6, %xmm2
	addps	 %xmm2, %xmm0
	mulps	 %xmm7, %xmm3
	addps	 %xmm3, %xmm0

	movaps	 %xmm0, -32 * SIZE(Y1)

	addl	 $4 * SIZE, A1
	addl	 $4 * SIZE, Y1
	ALIGN_3

.L17:
	testl	$2, M
	je	.L18

	movsd	 -32 * SIZE(A1), %xmm2
	movsd	 -32 * SIZE(A1, LDA), %xmm3

	movsd	 -32 * SIZE(Y1), %xmm0

	mulps	 %xmm6, %xmm2
	addps	 %xmm2, %xmm0
	mulps	 %xmm7, %xmm3
	addps	 %xmm3, %xmm0

	movlps	 %xmm0, -32 * SIZE(Y1)

	addl	 $2 * SIZE, A1
	addl	 $2 * SIZE, Y1
	ALIGN_3

.L18:
	testl	$1, M
	je	.L19

	movss	 -32 * SIZE(A1), %xmm2
	movss	 -32 * SIZE(A1, LDA), %xmm3

	movss	 -32 * SIZE(Y1), %xmm0

	mulss	 %xmm6, %xmm2
	addss	 %xmm2, %xmm0
	mulss	 %xmm7, %xmm3
	addss	 %xmm3, %xmm0

	movss	 %xmm0, -32 * SIZE(Y1)
	ALIGN_3

.L19:
	decl	J
	jg	.L11
	ALIGN_4

.L20:
	testl	$1, N
	jle	.L990

	movl	BUFFER, Y1
	addl	$32 * SIZE, Y1

	movl	A,  A1

	movss	(X), %xmm6
	addl	INCX, X

	movss	ALPHA, %xmm0

	mulss	%xmm0, %xmm6

	shufps	$0, %xmm6, %xmm6
	ALIGN_3

	movl	M,   I
	sarl	$4,  I
	jle	.L25

	movsd	 -32 * SIZE(A1), %xmm2
	movhps	 -30 * SIZE(A1), %xmm2
	movsd	 -28 * SIZE(A1), %xmm3
	movhps	 -26 * SIZE(A1), %xmm3

	movaps	 -32 * SIZE(Y1), %xmm0
	movaps	 -28 * SIZE(Y1), %xmm1

	decl	 I
	jle	 .L24
	ALIGN_3

.L23:
#ifdef PREFETCH
	PREFETCH	(PREFETCHSIZE + 0) * SIZE(A1)
#endif

	mulps	 %xmm6, %xmm2
	addps	 %xmm2, %xmm0
	movsd	 -24 * SIZE(A1), %xmm2
	movhps	 -22 * SIZE(A1), %xmm2

	movaps	 %xmm0,  -32 * SIZE(Y1)
	movaps	 -24 * SIZE(Y1), %xmm0

	mulps	 %xmm6, %xmm3
	addps	 %xmm3, %xmm1
	movsd	 -20 * SIZE(A1), %xmm3
	movhps	 -18 * SIZE(A1), %xmm3

	movaps	 %xmm1,  -28 * SIZE(Y1)
	movaps	 -20 * SIZE(Y1), %xmm1

	mulps	 %xmm6, %xmm2
	addps	 %xmm2, %xmm0
	movsd	 -16 * SIZE(A1), %xmm2
	movhps	 -14 * SIZE(A1), %xmm2

	movaps	 %xmm0,  -24 * SIZE(Y1)
	movaps	 -16 * SIZE(Y1), %xmm0

	mulps	 %xmm6, %xmm3
	addps	 %xmm3, %xmm1
	movsd	 -12 * SIZE(A1), %xmm3
	movhps	 -10 * SIZE(A1), %xmm3

	movaps	 %xmm1,  -20 * SIZE(Y1)
	movaps	 -12 * SIZE(Y1), %xmm1

	subl	 $-16 * SIZE, A1
	subl	 $-16 * SIZE, Y1

	subl	 $1, I
	BRANCH
	jg	.L23
	ALIGN_3

.L24:
	mulps	 %xmm6, %xmm2
	addps	 %xmm2, %xmm0
	movsd	 -24 * SIZE(A1), %xmm2
	movhps	 -22 * SIZE(A1), %xmm2
	mulps	 %xmm6, %xmm3
	addps	 %xmm3, %xmm1
	movsd	 -20 * SIZE(A1), %xmm3
	movhps	 -18 * SIZE(A1), %xmm3

	movaps	 %xmm0,  -32 * SIZE(Y1)
	movaps	 -24 * SIZE(Y1), %xmm0

	movaps	 %xmm1,  -28 * SIZE(Y1)
	movaps	 -20 * SIZE(Y1), %xmm1

	mulps	 %xmm6, %xmm2
	addps	 %xmm2, %xmm0
	movaps	 %xmm0,  -24 * SIZE(Y1)
	mulps	 %xmm6, %xmm3
	addps	 %xmm3, %xmm1
	movaps	 %xmm1,  -20 * SIZE(Y1)

	subl	 $-16 * SIZE, A1
	subl	 $-16 * SIZE, Y1
	ALIGN_3

.L25:
	testl	$8, M
	je	.L26

	movsd	 -32 * SIZE(A1), %xmm2
	movhps	 -30 * SIZE(A1), %xmm2
	movsd	 -28 * SIZE(A1), %xmm3
	movhps	 -26 * SIZE(A1), %xmm3

	movaps	 -32 * SIZE(Y1), %xmm0
	movaps	 -28 * SIZE(Y1), %xmm1

	mulps	 %xmm6, %xmm2
	addps	 %xmm2, %xmm0
	mulps	 %xmm6, %xmm3
	addps	 %xmm3, %xmm1

	movaps	 %xmm0,  -32 * SIZE(Y1)
	movaps	 %xmm1,  -28 * SIZE(Y1)

	addl	 $8 * SIZE, A1
	addl	 $8 * SIZE, Y1
	ALIGN_3

.L26:
	testl	$4, M
	je	.L27

	movsd	 -32 * SIZE(A1), %xmm2
	movhps	 -30 * SIZE(A1), %xmm2

	movaps	 -32 * SIZE(Y1), %xmm0

	mulps	 %xmm6, %xmm2
	addps	 %xmm2, %xmm0

	movaps	 %xmm0, -32 * SIZE(Y1)

	addl	 $4 * SIZE, A1
	addl	 $4 * SIZE, Y1
	ALIGN_3

.L27:
	testl	$2, M
	je	.L28

	movsd	 -32 * SIZE(A1), %xmm2
	movsd	 -32 * SIZE(Y1), %xmm0

	mulps	 %xmm6, %xmm2
	addps	 %xmm2, %xmm0

	movlps	 %xmm0, -32 * SIZE(Y1)

	addl	 $2 * SIZE, A1
	addl	 $2 * SIZE, Y1
	ALIGN_3

.L28:
	testl	$1, M
	je	.L990

	movss	 -32 * SIZE(A1), %xmm2
	movss	 -32 * SIZE(Y1), %xmm0

	mulss	 %xmm6, %xmm2
	addss	 %xmm2, %xmm0

	movss	 %xmm0, -32 * SIZE(Y1)
	ALIGN_3

.L990:
	movl	Y,   Y1
	movl	BUFFER, X

	movl	STACK_INCY, INCY
	sall	$BASE_SHIFT, INCY

	movl	M,   %eax
	sarl	$2,  %eax
	jle	.L994
	ALIGN_3

.L992:
	movss	(Y1), %xmm0
	addss	0 * SIZE(X), %xmm0
	movss	%xmm0, (Y1)
	addl	INCY, Y1

	movss	(Y1), %xmm0
	addss	1 * SIZE(X), %xmm0
	movss	%xmm0, (Y1)
	addl	INCY, Y1

	movss	(Y1), %xmm0
	addss	2 * SIZE(X), %xmm0
	movss	%xmm0, (Y1)
	addl	INCY, Y1

	movss	(Y1), %xmm0
	addss	3 * SIZE(X), %xmm0
	movss	%xmm0, (Y1)
	addl	INCY, Y1

	addl	$4 * SIZE, X
	decl	%eax
	jg	.L992
	ALIGN_3

.L994:
	testl	$2, M
	jle	.L996

	movss	(Y1), %xmm0
	addss	0 * SIZE(X), %xmm0
	movss	%xmm0, (Y1)
	addl	INCY, Y1

	movss	(Y1), %xmm0
	addss	1 * SIZE(X), %xmm0
	movss	%xmm0, (Y1)
	addl	INCY, Y1

	addl	$2 * SIZE, X
	ALIGN_3

.L996:
	testl	$1, M
	jle	.L999

	movss	(Y1), %xmm0
	addss	0 * SIZE(X), %xmm0
	movss	%xmm0, (Y1)
	ALIGN_3

.L999:
	popl	%ebx
	popl	%esi
	popl	%edi	
	popl	%ebp
	ret

	EPILOGUE
