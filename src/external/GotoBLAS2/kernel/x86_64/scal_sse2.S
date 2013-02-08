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

#ifndef WINDOWS_ABI
#define M	ARG1
#define X	ARG4
#define INCX	ARG5
#else
#define M	ARG1
#define X	ARG2
#define INCX	ARG3
#endif

#define XX	%r10
#define I	%rax

#include "l1param.h"

	PROLOGUE
	PROFCODE

#ifdef WINDOWS_ABI
	movq	40(%rsp), X
	movq	48(%rsp), INCX

	movaps	%xmm3, %xmm0	
#endif

	SAVEREGISTERS
	
	testq	M, M
	jle	.L999

	leaq	(, INCX, SIZE), INCX

	xorps	%xmm1, %xmm1
	comisd	%xmm0, %xmm1
	jne	.L100		# Alpha != ZERO

/* Alpha == ZERO */
	cmpq	$SIZE, INCX
	jne	.L50

/* INCX == 1 */
	testq	$15, X		# aligned for quad word?
	je	.L05

	movsd	%xmm1, 0 * SIZE(X)
	addq	$SIZE, X
	decq	M
	jle	.L999
	ALIGN_3
.L05:

/* Aligned Mode */
	movq	M,  I	# rcx = n
	sarq	$4, I
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

	addq	$16 * SIZE, X
	decq	I
	jg	.L11
	ALIGN_4

.L12:
	testq	$15, M
	je	.L999
	testq	$8, M
	je	.L13

	movaps	%xmm1,  0 * SIZE(X)
	movaps	%xmm1,  2 * SIZE(X)
	movaps	%xmm1,  4 * SIZE(X)
	movaps	%xmm1,  6 * SIZE(X)
	addq	$8 * SIZE, X
	ALIGN_3

.L13:
	testq  $4, M
	je    .L14

	movaps	%xmm1,  0 * SIZE(X)
	movaps	%xmm1,  2 * SIZE(X)
	addq	$4 * SIZE, X
	ALIGN_3

.L14:
	testq  $2, M
	je    .L15

	movaps	%xmm1,  0 * SIZE(X)
	addq	$2 * SIZE, X
	ALIGN_3

.L15:
	testq  $1, M
	je    .L999

	movsd	%xmm1,  0 * SIZE(X)
	jmp	.L999
	ALIGN_4

.L50:
	movq	M,  I
	sarq	$3, I
	jle	.L52
	ALIGN_4

.L51:
#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	movsd	%xmm1, (X)
	addq	INCX, X
	movsd	%xmm1, (X)
	addq	INCX, X
	movsd	%xmm1, (X)
	addq	INCX, X
	movsd	%xmm1, (X)
	addq	INCX, X
	movsd	%xmm1, (X)
	addq	INCX, X
	movsd	%xmm1, (X)
	addq	INCX, X
	movsd	%xmm1, (X)
	addq	INCX, X
	movsd	%xmm1, (X)
	addq	INCX, X

	decq	I
	jg	.L51
	ALIGN_4

.L52:
	testq	$7, M
	je	.L999

	testq	$4, M
	je	.L53

	movsd	%xmm1, (X)
	addq	INCX, X
	movsd	%xmm1, (X)
	addq	INCX, X
	movsd	%xmm1, (X)
	addq	INCX, X
	movsd	%xmm1, (X)
	addq	INCX, X
	ALIGN_3

.L53:
	testq	$2, M
	je	.L54

	movsd	%xmm1, (X)
	addq	INCX, X
	movsd	%xmm1, (X)
	addq	INCX, X
	ALIGN_3

.L54:
	testq	$1, M
	je	.L999

	movsd	%xmm1, (X)
	jmp	.L999
	ALIGN_4

/* Alpha != ZERO */

.L100:
	unpcklpd %xmm0, %xmm0

	cmpq	$SIZE, INCX
	jne	.L150

	testq	$SIZE, X
	je	.L105

	movsd	0 * SIZE(X), %xmm1
	mulsd	%xmm0, %xmm1
	movsd	%xmm1, 0 * SIZE(X)
	addq	$SIZE, X
	decq	M
	jle	.L999
	ALIGN_3
.L105:
	subq	$-16 * SIZE, X

	movq	M,  I		# rcx = n
	sarq	$4, I
	jle	.L113

#if defined(BARCELONA) || defined(SHANGHAI)

	movaps  %xmm0, %xmm1
	mulpd	-16 * SIZE(X), %xmm1
	movaps  %xmm0, %xmm2
	mulpd	-14 * SIZE(X), %xmm2
	movaps  %xmm0, %xmm3
	mulpd	-12 * SIZE(X), %xmm3
	movaps  %xmm0, %xmm4
	mulpd	-10 * SIZE(X), %xmm4
	movaps  %xmm0, %xmm5
	mulpd	 -8 * SIZE(X), %xmm5
	movaps  %xmm0, %xmm6
	mulpd	 -6 * SIZE(X), %xmm6
	movaps  %xmm0, %xmm7
	mulpd	 -4 * SIZE(X), %xmm7
	movaps  %xmm0, %xmm8
	mulpd	 -2 * SIZE(X), %xmm8

	decq	I 
	jle	.L112
	ALIGN_4

.L111:
#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	movaps	%xmm1, -16 * SIZE(X)
	movaps	%xmm2, -14 * SIZE(X)
	movaps	%xmm3, -12 * SIZE(X)
	movaps	%xmm4, -10 * SIZE(X)

	movaps  %xmm0, %xmm1
	mulpd	  0 * SIZE(X), %xmm1
	movaps  %xmm0, %xmm2
	mulpd	  2 * SIZE(X), %xmm2
	movaps  %xmm0, %xmm3
	mulpd	  4 * SIZE(X), %xmm3
	movaps  %xmm0, %xmm4
	mulpd	  6 * SIZE(X), %xmm4

#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE + 64) - PREOFFSET(X)
#endif

	movaps	%xmm5,  -8 * SIZE(X)
	movaps	%xmm6,  -6 * SIZE(X)
	movaps	%xmm7,  -4 * SIZE(X)
	movaps	%xmm8,  -2 * SIZE(X)

	movaps  %xmm0, %xmm5
	mulpd	  8 * SIZE(X), %xmm5
	movaps  %xmm0, %xmm6
	mulpd	 10 * SIZE(X), %xmm6
	movaps  %xmm0, %xmm7
	mulpd	 12 * SIZE(X), %xmm7
	movaps  %xmm0, %xmm8
	mulpd	 14 * SIZE(X), %xmm8

	subq	$-16 * SIZE, X
	decq	I
	jg	.L111
	ALIGN_4

.L112:
	movaps	%xmm1, -16 * SIZE(X)
	movaps	%xmm2, -14 * SIZE(X)
	movaps	%xmm3, -12 * SIZE(X)
	movaps	%xmm4, -10 * SIZE(X)
	movaps	%xmm5,  -8 * SIZE(X)
	movaps	%xmm6,  -6 * SIZE(X)
	movaps	%xmm7,  -4 * SIZE(X)
	movaps	%xmm8,  -2 * SIZE(X)

#else
	movaps	-16 * SIZE(X), %xmm1
	movaps	-14 * SIZE(X), %xmm2
	movaps	-12 * SIZE(X), %xmm3
	movaps	-10 * SIZE(X), %xmm4
	movaps	 -8 * SIZE(X), %xmm5
	movaps	 -6 * SIZE(X), %xmm6
	movaps	 -4 * SIZE(X), %xmm7
	movaps	 -2 * SIZE(X), %xmm8

	decq	I 
	jle	.L112
	ALIGN_4

.L111:
#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	mulpd	%xmm0, %xmm1
	movaps	%xmm1, -16 * SIZE(X)
	movaps	 0 * SIZE(X), %xmm1
	mulpd	%xmm0, %xmm2
	movaps	%xmm2, -14 * SIZE(X)
	movaps	 2 * SIZE(X), %xmm2

	mulpd	%xmm0, %xmm3
	movaps	%xmm3, -12 * SIZE(X)
	movaps	 4  * SIZE(X), %xmm3
	mulpd	%xmm0, %xmm4
	movaps	%xmm4, -10 * SIZE(X)
	movaps	 6 * SIZE(X), %xmm4

#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE + 64) - PREOFFSET(X)
#endif

	mulpd	%xmm0, %xmm5
	movaps	%xmm5, -8 * SIZE(X)
	movaps	 8 * SIZE(X), %xmm5
	mulpd	%xmm0, %xmm6
	movaps	%xmm6, -6 * SIZE(X)
	movaps	10 * SIZE(X), %xmm6

	mulpd	%xmm0, %xmm7
	movaps	%xmm7, -4 * SIZE(X)
	movaps	12 * SIZE(X), %xmm7
	mulpd	%xmm0, %xmm8
	movaps	%xmm8, -2 * SIZE(X)
	movaps	14 * SIZE(X), %xmm8

	subq	$-16 * SIZE, X
	decq	I
	jg	.L111
	ALIGN_4

.L112:
	mulpd	%xmm0, %xmm1
	movaps	%xmm1, -16 * SIZE(X)
	mulpd	%xmm0, %xmm2
	movaps	%xmm2, -14 * SIZE(X)
	mulpd	%xmm0, %xmm3
	movaps	%xmm3, -12 * SIZE(X)
	mulpd	%xmm0, %xmm4
	movaps	%xmm4, -10 * SIZE(X)

	mulpd	%xmm0, %xmm5
	movaps	%xmm5,  -8 * SIZE(X)
	mulpd	%xmm0, %xmm6
	movaps	%xmm6,  -6 * SIZE(X)
	mulpd	%xmm0, %xmm7
	movaps	%xmm7,  -4 * SIZE(X)
	mulpd	%xmm0, %xmm8
	movaps	%xmm8,  -2 * SIZE(X)
#endif

	subq	$-16 * SIZE, X
	ALIGN_3

.L113:
	testq	$15, M
	je	.L999

	testq	$8, M
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
	addq	$8 * SIZE, X
	ALIGN_3

.L114:
	testq	$4, M
	je	.L115

	movaps	-16 * SIZE(X), %xmm1
	movaps	-14 * SIZE(X), %xmm2

	mulpd	%xmm0, %xmm1
	movaps	%xmm1, -16 * SIZE(X)
	mulpd	%xmm0, %xmm2
	movaps	%xmm2, -14 * SIZE(X)
	addq	$4 * SIZE, X
	ALIGN_3

.L115:
	testq	$2, M
	je	.L116

	movaps	-16 * SIZE(X), %xmm1
	mulpd	%xmm0, %xmm1
	movaps	%xmm1, -16 * SIZE(X)
	addq	$2 * SIZE, X
	ALIGN_3

.L116:
	testq	$1, M
	je	.L999

	movsd	-16 * SIZE(X), %xmm1
	mulsd	%xmm0, %xmm1
	movsd	%xmm1, -16 * SIZE(X)
	jmp	.L999
	ALIGN_3

/* incx != 1 */

.L150:
	movq	X, XX
	movq	M,  I		# rcx = n
	sarq	$3, I		# (n >> 3)
	jle	.L152
	ALIGN_4

.L151:
#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE + 64) - PREOFFSET(X)
#endif
	movsd	(X), %xmm1
	addq	INCX, X
	movsd	(X), %xmm2
	addq	INCX, X
	movsd	(X), %xmm3
	addq	INCX, X
	movsd	(X), %xmm4
	addq	INCX, X
	movsd	(X), %xmm5
	addq	INCX, X
	movsd	(X), %xmm6
	addq	INCX, X
	movsd	(X), %xmm7
	addq	INCX, X
	movsd	(X), %xmm8
	addq	INCX, X

	mulsd	%xmm0, %xmm1
	mulsd	%xmm0, %xmm2
	mulsd	%xmm0, %xmm3
	mulsd	%xmm0, %xmm4
	mulsd	%xmm0, %xmm5
	mulsd	%xmm0, %xmm6
	mulsd	%xmm0, %xmm7
	mulsd	%xmm0, %xmm8

	movsd	%xmm1, (XX)
	addq	INCX, XX
	movsd	%xmm2, (XX)
	addq	INCX, XX
	movsd	%xmm3, (XX)
	addq	INCX, XX
	movsd	%xmm4, (XX)
	addq	INCX, XX
	movsd	%xmm5, (XX)
	addq	INCX, XX
	movsd	%xmm6, (XX)
	addq	INCX, XX
	movsd	%xmm7, (XX)
	addq	INCX, XX
	movsd	%xmm8, (XX)
	addq	INCX, XX
	decq	I
	jg	.L151
	ALIGN_4

.L152:
	testq	$7, M
	je	.L999

	testq	$4, M
	je	.L153

	movsd	 (X), %xmm1
	addq	INCX, X
	movsd	 (X), %xmm2
	addq	INCX, X
	movsd	 (X), %xmm3
	addq	INCX, X
	movsd	 (X), %xmm4
	addq	INCX, X

	mulsd	%xmm0, %xmm1
	mulsd	%xmm0, %xmm2
	mulsd	%xmm0, %xmm3
	mulsd	%xmm0, %xmm4

	movsd	%xmm1,  (XX)
	addq	INCX, XX
	movsd	%xmm2,  (XX)
	addq	INCX, XX
	movsd	%xmm3,  (XX)
	addq	INCX, XX
	movsd	%xmm4,  (XX)
	addq	INCX, XX
	ALIGN_3

.L153:
	testq	$2, M
	je	.L154

	movsd	 (X), %xmm1
	addq	INCX, X
	movsd	 (X), %xmm2
	addq	INCX, X

	mulsd	%xmm0, %xmm1
	mulsd	%xmm0, %xmm2

	movsd	%xmm1,  (XX)
	addq	INCX, XX
	movsd	%xmm2,  (XX)
	addq	INCX, XX
	ALIGN_3

.L154:
	testq	$1, M
	je	.L999

	movsd	 (X), %xmm1
	mulsd	%xmm0, %xmm1
	movsd	%xmm1,  (X)
	ALIGN_4

.L999:
	xorq	%rax, %rax

	RESTOREREGISTERS

	ret

	EPILOGUE
