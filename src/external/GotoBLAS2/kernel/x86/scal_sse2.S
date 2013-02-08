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

#define STACK_M	 	 4 + STACK + ARGS(%esp)
#define STACK_ALPHA	16 + STACK + ARGS(%esp)
#define STACK_X		24 + STACK + ARGS(%esp)
#define STACK_INCX	28 + STACK + ARGS(%esp)

#define M	%ebx
#define X	%ecx
#define INCX	%edx
#define I	%esi
#define XX	%edi

#include "l1param.h"

	PROLOGUE
	PROFCODE

	pushl	%edi
	pushl	%esi
	pushl	%ebx

	movl	STACK_M,     M
	movl	STACK_X,     X
	movl	STACK_INCX,  INCX

	movsd	STACK_ALPHA, %xmm0

	testl	M, M
	jle	.L999

	leal	(, INCX, SIZE), INCX

	xorps	%xmm1, %xmm1
	comisd	%xmm0, %xmm1
	jne	.L100		# Alpha != ZERO

/* Alpha == ZERO */
	cmpl	$SIZE, INCX
	jne	.L50

/* INCX == 1 */
	testl	$15, X		# aligned for quad word?
	je	.L05

	movsd	%xmm1, 0 * SIZE(X)
	addl	$SIZE, X
	decl	M
	jle	.L999
	ALIGN_3
.L05:

/* Aligned Mode */
	movl	M,  I	# rcx = n
	sarl	$4, I
	jle	.L12
	ALIGN_4

.L11:
#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	movaps	%xmm1,  0 * SIZE(X)
	movaps	%xmm1,  2 * SIZE(X)
	movaps	%xmm1,  4 * SIZE(X)
	movaps	%xmm1,  6 * SIZE(X)

#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE + 64) - PREOFFSET(X)
#endif

	movaps	%xmm1,  8 * SIZE(X)
	movaps	%xmm1, 10 * SIZE(X)
	movaps	%xmm1, 12 * SIZE(X)
	movaps	%xmm1, 14 * SIZE(X)

	addl	$16 * SIZE, X
	decl	I
	jg	.L11
	ALIGN_4

.L12:
	testl	$15, M
	je	.L999
	testl	$8, M
	je	.L13

	movaps	%xmm1,  0 * SIZE(X)
	movaps	%xmm1,  2 * SIZE(X)
	movaps	%xmm1,  4 * SIZE(X)
	movaps	%xmm1,  6 * SIZE(X)
	addl	$8 * SIZE, X
	ALIGN_3

.L13:
	testl  $4, M
	je    .L14

	movaps	%xmm1,  0 * SIZE(X)
	movaps	%xmm1,  2 * SIZE(X)
	addl	$4 * SIZE, X
	ALIGN_3

.L14:
	testl  $2, M
	je    .L15

	movaps	%xmm1,  0 * SIZE(X)
	addl	$2 * SIZE, X
	ALIGN_3

.L15:
	testl  $1, M
	je    .L999

	movsd	%xmm1,  0 * SIZE(X)
	jmp	.L999
	ALIGN_4

.L50:
	movl	M,  I
	sarl	$3, I
	jle	.L52
	ALIGN_4

.L51:
#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	movsd	%xmm1, (X)
	addl	INCX, X
	movsd	%xmm1, (X)
	addl	INCX, X
	movsd	%xmm1, (X)
	addl	INCX, X
	movsd	%xmm1, (X)
	addl	INCX, X
	movsd	%xmm1, (X)
	addl	INCX, X
	movsd	%xmm1, (X)
	addl	INCX, X
	movsd	%xmm1, (X)
	addl	INCX, X
	movsd	%xmm1, (X)
	addl	INCX, X

	decl	I
	jg	.L51
	ALIGN_4

.L52:
	testl	$7, M
	je	.L999

	testl	$4, M
	je	.L53

	movsd	%xmm1, (X)
	addl	INCX, X
	movsd	%xmm1, (X)
	addl	INCX, X
	movsd	%xmm1, (X)
	addl	INCX, X
	movsd	%xmm1, (X)
	addl	INCX, X
	ALIGN_3

.L53:
	testl	$2, M
	je	.L54

	movsd	%xmm1, (X)
	addl	INCX, X
	movsd	%xmm1, (X)
	addl	INCX, X
	ALIGN_3

.L54:
	testl	$1, M
	je	.L999

	movsd	%xmm1, (X)
	jmp	.L999
	ALIGN_4

/* Alpha != ZERO */

.L100:
	unpcklpd %xmm0, %xmm0

	cmpl	$SIZE, INCX
	jne	.L150

	testl	$SIZE, X
	je	.L105

	movsd	0 * SIZE(X), %xmm1
	mulsd	%xmm0, %xmm1
	movsd	%xmm1, 0 * SIZE(X)
	addl	$SIZE, X
	decl	M
	jle	.L999
	ALIGN_3
.L105:
	subl	$-16 * SIZE, X

	movl	M,  I		# rcx = n
	sarl	$4, I
	jle	.L113

#if defined(BARCELONA)

	movaps  %xmm0, %xmm1
	mulpd	-16 * SIZE(X), %xmm1
	movaps  %xmm0, %xmm2
	mulpd	-14 * SIZE(X), %xmm2
	movaps  %xmm0, %xmm3
	mulpd	-12 * SIZE(X), %xmm3
	movaps  %xmm0, %xmm4
	mulpd	-10 * SIZE(X), %xmm4

	decl	I 
	jle	.L112
	ALIGN_4

.L111:
#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	movaps	%xmm1, -16 * SIZE(X)
	movaps  %xmm0, %xmm1
	mulpd	 -8 * SIZE(X), %xmm1

	movaps	%xmm2, -14 * SIZE(X)
	movaps  %xmm0, %xmm2
	mulpd	 -6 * SIZE(X), %xmm2

	movaps	%xmm3, -12 * SIZE(X)
	movaps  %xmm0, %xmm3
	mulpd	 -4 * SIZE(X), %xmm3

	movaps	%xmm4, -10 * SIZE(X)
	movaps  %xmm0, %xmm4
	mulpd	 -2 * SIZE(X), %xmm4

#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE + 64) - PREOFFSET(X)
#endif

	movaps	%xmm1,  -8 * SIZE(X)
	movaps  %xmm0, %xmm1
	mulpd	  0 * SIZE(X), %xmm1

	movaps	%xmm2,  -6 * SIZE(X)
	movaps  %xmm0, %xmm2
	mulpd	  2 * SIZE(X), %xmm2

	movaps	%xmm3,  -4 * SIZE(X)
	movaps  %xmm0, %xmm3
	mulpd	  4 * SIZE(X), %xmm3

	movaps	%xmm4,  -2 * SIZE(X)
	movaps  %xmm0, %xmm4
	mulpd	  6 * SIZE(X), %xmm4

	subl	$-16 * SIZE, X
	decl	I
	jg	.L111
	ALIGN_4

.L112:
	movaps	%xmm1, -16 * SIZE(X)
	movaps  %xmm0, %xmm1
	mulpd	 -8 * SIZE(X), %xmm1

	movaps	%xmm2, -14 * SIZE(X)
	movaps  %xmm0, %xmm2
	mulpd	 -6 * SIZE(X), %xmm2

	movaps	%xmm3, -12 * SIZE(X)
	movaps  %xmm0, %xmm3
	mulpd	 -4 * SIZE(X), %xmm3

	movaps	%xmm4, -10 * SIZE(X)
	movaps  %xmm0, %xmm4
	mulpd	 -2 * SIZE(X), %xmm4

	movaps	%xmm1,  -8 * SIZE(X)
	movaps	%xmm2,  -6 * SIZE(X)
	movaps	%xmm3,  -4 * SIZE(X)
	movaps	%xmm4,  -2 * SIZE(X)

#else
	movaps	-16 * SIZE(X), %xmm1
	movaps	-14 * SIZE(X), %xmm2
	movaps	-12 * SIZE(X), %xmm3
	movaps	-10 * SIZE(X), %xmm4

	decl	I 
	jle	.L112
	ALIGN_4

.L111:
#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	mulpd	%xmm0, %xmm1
	movaps	%xmm1, -16 * SIZE(X)
	movaps	 -8 * SIZE(X), %xmm1

	mulpd	%xmm0, %xmm2
	movaps	%xmm2, -14 * SIZE(X)
	movaps	 -6 * SIZE(X), %xmm2

	mulpd	%xmm0, %xmm3
	movaps	%xmm3, -12 * SIZE(X)
	movaps	 -4 * SIZE(X), %xmm3

	mulpd	%xmm0, %xmm4
	movaps	%xmm4, -10 * SIZE(X)
	movaps	 -2 * SIZE(X), %xmm4

#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE + 64) - PREOFFSET(X)
#endif

	mulpd	%xmm0, %xmm1
	movaps	%xmm1,  -8 * SIZE(X)
	movaps	  0 * SIZE(X), %xmm1

	mulpd	%xmm0, %xmm2
	movaps	%xmm2,  -6 * SIZE(X)
	movaps	  2 * SIZE(X), %xmm2

	mulpd	%xmm0, %xmm3
	movaps	%xmm3,  -4 * SIZE(X)
	movaps	  4 * SIZE(X), %xmm3

	mulpd	%xmm0, %xmm4
	movaps	%xmm4,  -2 * SIZE(X)
	movaps	  6 * SIZE(X), %xmm4

	subl	$-16 * SIZE, X
	decl	I
	jg	.L111
	ALIGN_4

.L112:
	mulpd	%xmm0, %xmm1
	movaps	%xmm1, -16 * SIZE(X)
	movaps	 -8 * SIZE(X), %xmm1

	mulpd	%xmm0, %xmm2
	movaps	%xmm2, -14 * SIZE(X)
	movaps	 -6 * SIZE(X), %xmm2

	mulpd	%xmm0, %xmm3
	movaps	%xmm3, -12 * SIZE(X)
	movaps	 -4 * SIZE(X), %xmm3

	mulpd	%xmm0, %xmm4
	movaps	%xmm4, -10 * SIZE(X)
	movaps	 -2 * SIZE(X), %xmm4

	mulpd	%xmm0, %xmm1
	movaps	%xmm1,  -8 * SIZE(X)
	mulpd	%xmm0, %xmm2
	movaps	%xmm2,  -6 * SIZE(X)
	mulpd	%xmm0, %xmm3
	movaps	%xmm3,  -4 * SIZE(X)
	mulpd	%xmm0, %xmm4
	movaps	%xmm4,  -2 * SIZE(X)
#endif

	subl	$-16 * SIZE, X
	ALIGN_3

.L113:
	testl	$15, M
	je	.L999

	testl	$8, M
	je	.L114

	movaps	-16 * SIZE(X), %xmm1
	movaps	-14 * SIZE(X), %xmm2
	movaps	-12 * SIZE(X), %xmm3
	movaps	-10 * SIZE(X), %xmm4

	mulpd	%xmm0, %xmm1
	movaps	%xmm1, -16 * SIZE(X)
	mulpd	%xmm0, %xmm2
	movaps	%xmm2, -14 * SIZE(X)
	mulpd	%xmm0, %xmm3
	movaps	%xmm3, -12 * SIZE(X)
	mulpd	%xmm0, %xmm4
	movaps	%xmm4, -10 * SIZE(X)
	addl	$8 * SIZE, X
	ALIGN_3

.L114:
	testl	$4, M
	je	.L115

	movaps	-16 * SIZE(X), %xmm1
	movaps	-14 * SIZE(X), %xmm2

	mulpd	%xmm0, %xmm1
	movaps	%xmm1, -16 * SIZE(X)
	mulpd	%xmm0, %xmm2
	movaps	%xmm2, -14 * SIZE(X)
	addl	$4 * SIZE, X
	ALIGN_3

.L115:
	testl	$2, M
	je	.L116

	movaps	-16 * SIZE(X), %xmm1
	mulpd	%xmm0, %xmm1
	movaps	%xmm1, -16 * SIZE(X)
	addl	$2 * SIZE, X
	ALIGN_3

.L116:
	testl	$1, M
	je	.L999

	movsd	-16 * SIZE(X), %xmm1
	mulsd	%xmm0, %xmm1
	movsd	%xmm1, -16 * SIZE(X)
	jmp	.L999
	ALIGN_3

/* incx != 1 */

.L150:
	movl	X, XX
	movl	M,  I
	sarl	$2, I
	jle	.L152
	ALIGN_4

.L151:
#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE + 64) - PREOFFSET(X)
#endif
	movsd	(X), %xmm1
	addl	INCX, X
	movsd	(X), %xmm2
	addl	INCX, X
	movsd	(X), %xmm3
	addl	INCX, X
	movsd	(X), %xmm4
	addl	INCX, X

	mulsd	%xmm0, %xmm1
	mulsd	%xmm0, %xmm2
	mulsd	%xmm0, %xmm3
	mulsd	%xmm0, %xmm4

	movsd	%xmm1, (XX)
	addl	INCX, XX
	movsd	%xmm2, (XX)
	addl	INCX, XX
	movsd	%xmm3, (XX)
	addl	INCX, XX
	movsd	%xmm4, (XX)
	addl	INCX, XX

	decl	I
	jg	.L151
	ALIGN_4

.L152:
	testl	$2, M
	je	.L154

	movsd	 (X), %xmm1
	addl	INCX, X
	movsd	 (X), %xmm2
	addl	INCX, X

	mulsd	%xmm0, %xmm1
	mulsd	%xmm0, %xmm2

	movsd	%xmm1,  (XX)
	addl	INCX, XX
	movsd	%xmm2,  (XX)
	addl	INCX, XX
	ALIGN_3

.L154:
	testl	$1, M
	je	.L999

	movsd	 (X), %xmm1
	mulsd	%xmm0, %xmm1
	movsd	%xmm1,  (X)
	ALIGN_4

.L999:
	xorl	%eax, %eax

	popl	%ebx
	popl	%esi
	popl	%edi
	ret

	EPILOGUE
