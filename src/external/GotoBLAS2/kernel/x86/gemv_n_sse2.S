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

#ifdef PENTIUM4
#define PREFETCH	prefetcht0
#define PREFETCHW	prefetcht0
#define PREFETCHSIZE	(8 * 2)
#endif

#if defined(CORE2) || defined(PENRYN) || defined(DUNNINGTON) || defined(NEHALEM)
#define PREFETCH	prefetcht0
#define PREFETCHW	prefetcht0
#define PREFETCHSIZE	(8 * 7)
#endif

#ifdef OPTERON
#define PREFETCH	prefetchnta
#define PREFETCHW	prefetchw
#define PREFETCHSIZE	(8 * 3)
#define movsd		movlps
#endif

#ifdef BARCELONA
#define PREFETCH	prefetchnta
#define PREFETCHW	prefetchw
#define PREFETCHSIZE	(8 * 5)
#endif

#ifdef ATOM
#define PREFETCH	prefetch
#define PREFETCHW	prefetcht0
#define PREFETCHSIZE	(8 * 6)
#endif

#ifdef NANO
#define PREFETCH	prefetcht0
#define PREFETCHSIZE	(8 * 4)
#endif

#define STACKSIZE	16

#define M		 4 + STACKSIZE(%esp)
#define N		 8 + STACKSIZE(%esp)
#define ALPHA		16 + STACKSIZE(%esp)
#define A		24 + STACKSIZE(%esp)
#define STACK_LDA	28 + STACKSIZE(%esp)
#define STACK_X		32 + STACKSIZE(%esp)
#define STACK_INCX	36 + STACKSIZE(%esp)
#define Y		40 + STACKSIZE(%esp)
#define STACK_INCY	44 + STACKSIZE(%esp)
#define BUFFER		48 + STACKSIZE(%esp)
	
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

	subl	$-16 * SIZE, A

	cmpl	$0, N
	jle	.L999
	cmpl	$0, M
	jle	.L999

	movl	BUFFER, Y1
	
	pxor	%xmm7, %xmm7

	movl	M,   %eax
	addl	$16, %eax
	sarl	$4,  %eax
	ALIGN_3

.L01:
	movapd	%xmm7,  0 * SIZE(Y1)
	movapd	%xmm7,  2 * SIZE(Y1)
	movapd	%xmm7,  4 * SIZE(Y1)
	movapd	%xmm7,  6 * SIZE(Y1)
	movapd	%xmm7,  8 * SIZE(Y1)
	movapd	%xmm7, 10 * SIZE(Y1)
	movapd	%xmm7, 12 * SIZE(Y1)
	movapd	%xmm7, 14 * SIZE(Y1)
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
	addl	$16 * SIZE, Y1

	movl	A,  A1
	leal	(A1,  LDA, 2), %eax
	movl	%eax, A

#ifdef HAVE_SSE3
	movddup	(X), %xmm6
	addl	INCX, X
	movddup	(X), %xmm7
	addl	INCX, X

	movddup	ALPHA, %xmm0

	mulpd	%xmm0, %xmm6
	mulpd	%xmm0, %xmm7
#else
	movsd	(X), %xmm6
	addl	INCX, X
	movsd	(X), %xmm7
	addl	INCX, X

	movsd	ALPHA, %xmm0

	mulsd	%xmm0, %xmm6
	mulsd	%xmm0, %xmm7

	unpcklpd %xmm6, %xmm6
	unpcklpd %xmm7, %xmm7
#endif

	ALIGN_3

	movl	M,   I
	sarl	$3,  I
	jle	.L15

	movsd	 -16 * SIZE(A1), %xmm2
	movhpd	 -15 * SIZE(A1), %xmm2
	movsd	 -14 * SIZE(A1), %xmm3
	movhpd	 -13 * SIZE(A1), %xmm3

	movapd	 -16 * SIZE(Y1), %xmm0
	movapd	 -14 * SIZE(Y1), %xmm1

	movsd	 -16 * SIZE(A1, LDA), %xmm4
	movhpd	 -15 * SIZE(A1, LDA), %xmm4
	movsd	 -14 * SIZE(A1, LDA), %xmm5
	movhpd	 -13 * SIZE(A1, LDA), %xmm5

	decl	 I
	jle	 .L14
	ALIGN_3

.L13:
#ifdef PREFETCH
	PREFETCH	(PREFETCHSIZE + 0) * SIZE(A1)
#endif

	mulpd	 %xmm6, %xmm2
	addpd	 %xmm2, %xmm0
	movsd	 -12 * SIZE(A1), %xmm2
	movhpd	 -11 * SIZE(A1), %xmm2
	mulpd	 %xmm6, %xmm3
	addpd	 %xmm3, %xmm1
	movsd	 -10 * SIZE(A1), %xmm3
	movhpd	  -9 * SIZE(A1), %xmm3

	mulpd	 %xmm7, %xmm4
	addpd	 %xmm4, %xmm0
	movsd	 -12 * SIZE(A1, LDA), %xmm4
	movhpd	 -11 * SIZE(A1, LDA), %xmm4

	movapd	 %xmm0,  -16 * SIZE(Y1)
	movapd	 -12 * SIZE(Y1), %xmm0

	mulpd	 %xmm7, %xmm5
	addpd	 %xmm5, %xmm1
	movsd	 -10 * SIZE(A1, LDA), %xmm5
	movhpd	  -9 * SIZE(A1, LDA), %xmm5

	movapd	 %xmm1,  -14 * SIZE(Y1)
	movapd	 -10 * SIZE(Y1), %xmm1

#ifdef PREFETCH
	PREFETCH	(PREFETCHSIZE + 0) * SIZE(A1, LDA)
#endif

	mulpd	 %xmm6, %xmm2
	addpd	 %xmm2, %xmm0
	movsd	  -8 * SIZE(A1), %xmm2
	movhpd	  -7 * SIZE(A1), %xmm2
	mulpd	 %xmm6, %xmm3
	addpd	 %xmm3, %xmm1
	movsd	  -6 * SIZE(A1), %xmm3
	movhpd	  -5 * SIZE(A1), %xmm3

	mulpd	 %xmm7, %xmm4
	addpd	 %xmm4, %xmm0
	movsd	  -8 * SIZE(A1, LDA), %xmm4
	movhpd	  -7 * SIZE(A1, LDA), %xmm4

	movapd	 %xmm0,  -12 * SIZE(Y1)
	movapd	  -8 * SIZE(Y1), %xmm0

	mulpd	 %xmm7, %xmm5
	addpd	 %xmm5, %xmm1
	movsd	  -6 * SIZE(A1, LDA), %xmm5
	movhpd	  -5 * SIZE(A1, LDA), %xmm5

	movapd	 %xmm1,  -10 * SIZE(Y1)
	movapd	  -6 * SIZE(Y1), %xmm1

	subl	 $-8 * SIZE, A1
	subl	 $-8 * SIZE, Y1

	subl	 $1, I
	BRANCH
	jg	.L13
	ALIGN_3

.L14:
	mulpd	 %xmm6, %xmm2
	addpd	 %xmm2, %xmm0
	movsd	 -12 * SIZE(A1), %xmm2
	movhpd	 -11 * SIZE(A1), %xmm2
	mulpd	 %xmm6, %xmm3
	addpd	 %xmm3, %xmm1
	movsd	 -10 * SIZE(A1), %xmm3
	movhpd	  -9 * SIZE(A1), %xmm3

	mulpd	 %xmm7, %xmm4
	addpd	 %xmm4, %xmm0
	movsd	 -12 * SIZE(A1, LDA), %xmm4
	movhpd	 -11 * SIZE(A1, LDA), %xmm4

	movapd	 %xmm0,  -16 * SIZE(Y1)
	movapd	 -12 * SIZE(Y1), %xmm0

	mulpd	 %xmm7, %xmm5
	addpd	 %xmm5, %xmm1
	movsd	 -10 * SIZE(A1, LDA), %xmm5
	movhpd	  -9 * SIZE(A1, LDA), %xmm5

	movapd	 %xmm1,  -14 * SIZE(Y1)
	movapd	 -10 * SIZE(Y1), %xmm1

	mulpd	 %xmm6, %xmm2
	addpd	 %xmm2, %xmm0
	mulpd	 %xmm6, %xmm3
	addpd	 %xmm3, %xmm1

	mulpd	 %xmm7, %xmm4
	addpd	 %xmm4, %xmm0
	movapd	 %xmm0,  -12 * SIZE(Y1)
	mulpd	 %xmm7, %xmm5
	addpd	 %xmm5, %xmm1
	movapd	 %xmm1,  -10 * SIZE(Y1)

	subl	 $-8 * SIZE, A1
	subl	 $-8 * SIZE, Y1
	ALIGN_3

.L15:
	testl	$4, M
	je	.L16

	movsd	 -16 * SIZE(A1), %xmm2
	movhpd	 -15 * SIZE(A1), %xmm2
	movsd	 -14 * SIZE(A1), %xmm3
	movhpd	 -13 * SIZE(A1), %xmm3

	movapd	 -16 * SIZE(Y1), %xmm0
	movapd	 -14 * SIZE(Y1), %xmm1

	mulpd	 %xmm6, %xmm2
	addpd	 %xmm2, %xmm0
	mulpd	 %xmm6, %xmm3
	addpd	 %xmm3, %xmm1

	movsd	 -16 * SIZE(A1, LDA), %xmm4
	movhpd	 -15 * SIZE(A1, LDA), %xmm4
	movsd	 -14 * SIZE(A1, LDA), %xmm5
	movhpd	 -13 * SIZE(A1, LDA), %xmm5

	mulpd	 %xmm7, %xmm4
	addpd	 %xmm4, %xmm0
	mulpd	 %xmm7, %xmm5
	addpd	 %xmm5, %xmm1

	movapd	 %xmm0,  -16 * SIZE(Y1)
	movapd	 %xmm1,  -14 * SIZE(Y1)

	addl	 $4 * SIZE, A1
	addl	 $4 * SIZE, Y1
	ALIGN_3

.L16:
	testl	$2, M
	je	.L17

	movsd	 -16 * SIZE(A1), %xmm2
	movhpd	 -15 * SIZE(A1), %xmm2
	movsd	 -16 * SIZE(A1, LDA), %xmm3
	movhpd	 -15 * SIZE(A1, LDA), %xmm3

	movapd	 -16 * SIZE(Y1), %xmm0

	mulpd	 %xmm6, %xmm2
	addpd	 %xmm2, %xmm0
	mulpd	 %xmm7, %xmm3
	addpd	 %xmm3, %xmm0

	movapd	 %xmm0, -16 * SIZE(Y1)

	addl	 $2 * SIZE, A1
	addl	 $2 * SIZE, Y1
	ALIGN_3

.L17:
	testl	$1, M
	je	.L19

	movsd	 -16 * SIZE(A1), %xmm2
	movsd	 -16 * SIZE(A1, LDA), %xmm3

	movsd	 -16 * SIZE(Y1), %xmm0

	mulsd	 %xmm6, %xmm2
	addsd	 %xmm2, %xmm0
	mulsd	 %xmm7, %xmm3
	addsd	 %xmm3, %xmm0

	movsd	 %xmm0, -16 * SIZE(Y1)
	ALIGN_3

.L19:
	decl	J
	jg	.L11
	ALIGN_4

.L20:
	testl	$1, N
	jle	.L990

	movl	BUFFER, Y1
	addl	$16 * SIZE, Y1

	movl	A,  A1

#ifdef HAVE_SSE3
	movddup	(X), %xmm6
	addl	INCX, X

	movddup	ALPHA, %xmm0

	mulpd	%xmm0, %xmm6
#else
	movsd	(X), %xmm6
	addl	INCX, X

	movsd	ALPHA, %xmm0

	mulsd	%xmm0, %xmm6
	unpcklpd %xmm6, %xmm6
#endif

	ALIGN_3

	movl	M,   I
	sarl	$3,  I
	jle	.L25

	movsd	 -16 * SIZE(A1), %xmm2
	movhpd	 -15 * SIZE(A1), %xmm2
	movsd	 -14 * SIZE(A1), %xmm3
	movhpd	 -13 * SIZE(A1), %xmm3

	movapd	 -16 * SIZE(Y1), %xmm0
	movapd	 -14 * SIZE(Y1), %xmm1
	decl	 I
	jle	 .L24
	ALIGN_3

.L23:
#ifdef PREFETCH
	PREFETCH	(PREFETCHSIZE + 0) * SIZE(A1)
#endif

	mulpd	 %xmm6, %xmm2
	addpd	 %xmm2, %xmm0
	movsd	 -12 * SIZE(A1), %xmm2
	movhpd	 -11 * SIZE(A1), %xmm2

	movapd	 %xmm0,  -16 * SIZE(Y1)
	movapd	 -12 * SIZE(Y1), %xmm0

	mulpd	 %xmm6, %xmm3
	addpd	 %xmm3, %xmm1
	movsd	 -10 * SIZE(A1), %xmm3
	movhpd	  -9 * SIZE(A1), %xmm3

	movapd	 %xmm1,  -14 * SIZE(Y1)
	movapd	 -10 * SIZE(Y1), %xmm1

	mulpd	 %xmm6, %xmm2
	addpd	 %xmm2, %xmm0
	movsd	  -8 * SIZE(A1), %xmm2
	movhpd	  -7 * SIZE(A1), %xmm2

	movapd	 %xmm0,  -12 * SIZE(Y1)
	movapd	  -8 * SIZE(Y1), %xmm0

	mulpd	 %xmm6, %xmm3
	addpd	 %xmm3, %xmm1
	movsd	  -6 * SIZE(A1), %xmm3
	movhpd	  -5 * SIZE(A1), %xmm3

	movapd	 %xmm1,  -10 * SIZE(Y1)
	movapd	  -6 * SIZE(Y1), %xmm1

	subl	 $-8 * SIZE, A1
	subl	 $-8 * SIZE, Y1

	subl	 $1, I
	BRANCH
	jg	.L23
	ALIGN_3

.L24:
	mulpd	 %xmm6, %xmm2
	addpd	 %xmm2, %xmm0
	movsd	 -12 * SIZE(A1), %xmm2
	movhpd	 -11 * SIZE(A1), %xmm2
	mulpd	 %xmm6, %xmm3
	addpd	 %xmm3, %xmm1
	movsd	 -10 * SIZE(A1), %xmm3
	movhpd	  -9 * SIZE(A1), %xmm3

	movapd	 %xmm0,  -16 * SIZE(Y1)
	movapd	 -12 * SIZE(Y1), %xmm0

	movapd	 %xmm1,  -14 * SIZE(Y1)
	movapd	 -10 * SIZE(Y1), %xmm1

	mulpd	 %xmm6, %xmm2
	addpd	 %xmm2, %xmm0
	movapd	 %xmm0,  -12 * SIZE(Y1)
	mulpd	 %xmm6, %xmm3
	addpd	 %xmm3, %xmm1
	movapd	 %xmm1,  -10 * SIZE(Y1)

	subl	 $-8 * SIZE, A1
	subl	 $-8 * SIZE, Y1
	ALIGN_3

.L25:
	testl	$4, M
	je	.L26

	movsd	 -16 * SIZE(A1), %xmm2
	movhpd	 -15 * SIZE(A1), %xmm2
	movsd	 -14 * SIZE(A1), %xmm3
	movhpd	 -13 * SIZE(A1), %xmm3

	movapd	 -16 * SIZE(Y1), %xmm0
	movapd	 -14 * SIZE(Y1), %xmm1

	mulpd	 %xmm6, %xmm2
	addpd	 %xmm2, %xmm0
	mulpd	 %xmm6, %xmm3
	addpd	 %xmm3, %xmm1

	movapd	 %xmm0,  -16 * SIZE(Y1)
	movapd	 %xmm1,  -14 * SIZE(Y1)

	addl	 $4 * SIZE, A1
	addl	 $4 * SIZE, Y1
	ALIGN_3

.L26:
	testl	$2, M
	je	.L27

	movsd	 -16 * SIZE(A1), %xmm2
	movhpd	 -15 * SIZE(A1), %xmm2

	movapd	 -16 * SIZE(Y1), %xmm0

	mulpd	 %xmm6, %xmm2
	addpd	 %xmm2, %xmm0

	movapd	 %xmm0, -16 * SIZE(Y1)

	addl	 $2 * SIZE, A1
	addl	 $2 * SIZE, Y1
	ALIGN_3

.L27:
	testl	$1, M
	je	.L990

	movsd	 -16 * SIZE(A1), %xmm2
	movsd	 -16 * SIZE(Y1), %xmm0

	mulsd	 %xmm6, %xmm2
	addsd	 %xmm2, %xmm0

	movsd	 %xmm0, -16 * SIZE(Y1)
	ALIGN_3

.L990:
	movl	Y,   Y1
	movl	BUFFER, X

	movl	STACK_INCY, INCY
	sall	$BASE_SHIFT, INCY

	movl	M,   %eax
	sarl	$3,  %eax
	jle	.L994
	ALIGN_3

.L992:
	movsd	(Y1), %xmm0
	movhpd	(Y1, INCY), %xmm0

	addpd	0 * SIZE(X), %xmm0

	movlpd	%xmm0, (Y1)
	movhpd	%xmm0, (Y1, INCY)
	leal	(Y1, INCY, 2), Y1

	movsd	(Y1), %xmm0
	movhpd	(Y1, INCY), %xmm0

	addpd	2 * SIZE(X), %xmm0

	movlpd	%xmm0, (Y1)
	movhpd	%xmm0, (Y1, INCY)
	leal	(Y1, INCY, 2), Y1

	movsd	(Y1), %xmm0
	movhpd	(Y1, INCY), %xmm0

	addpd	4 * SIZE(X), %xmm0

	movlpd	%xmm0, (Y1)
	movhpd	%xmm0, (Y1, INCY)
	leal	(Y1, INCY, 2), Y1

	movsd	(Y1), %xmm0
	movhpd	(Y1, INCY), %xmm0

	addpd	6 * SIZE(X), %xmm0

	movlpd	%xmm0, (Y1)
	movhpd	%xmm0, (Y1, INCY)
	leal	(Y1, INCY, 2), Y1

	addl	$8 * SIZE, X
	decl	%eax
	jg	.L992
	ALIGN_3

.L994:
	testl	$7, M
	jle	.L999

	testl	$4, M
	jle	.L995

	movsd	(Y1), %xmm0
	movhpd	(Y1, INCY), %xmm0

	addpd	0 * SIZE(X), %xmm0

	movlpd	%xmm0, (Y1)
	movhpd	%xmm0, (Y1, INCY)
	leal	(Y1, INCY, 2), Y1

	movsd	(Y1), %xmm0
	movhpd	(Y1, INCY), %xmm0

	addpd	2 * SIZE(X), %xmm0

	movlpd	%xmm0, (Y1)
	movhpd	%xmm0, (Y1, INCY)
	leal	(Y1, INCY, 2), Y1

	addl	$4 * SIZE, X
	ALIGN_3

.L995:
	testl	$2, M
	jle	.L996

	movsd	(Y1), %xmm0
	movhpd	(Y1, INCY), %xmm0

	addpd	0 * SIZE(X), %xmm0

	movlpd	%xmm0, (Y1)
	movhpd	%xmm0, (Y1, INCY)
	leal	(Y1, INCY, 2), Y1

	addl	$2 * SIZE, X
	ALIGN_3

.L996:
	testl	$1, M
	jle	.L999

	movsd	(Y1), %xmm0

	movsd	0 * SIZE(X), %xmm4

	addsd	%xmm4, %xmm0

	movlpd	%xmm0, (Y1)
	ALIGN_3

.L999:
	popl	%ebx
	popl	%esi
	popl	%edi	
	popl	%ebp
	ret

	EPILOGUE
