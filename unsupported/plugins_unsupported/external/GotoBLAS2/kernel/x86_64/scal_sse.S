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

	lea	(, INCX, SIZE), INCX

	pxor	%xmm1, %xmm1
	comiss	%xmm0, %xmm1
	shufps	$0, %xmm0, %xmm0

	jne	.L100		# Alpha != ZERO

/* Alpha == ZERO */
	cmpq	$SIZE, INCX
	jne	.L50

/* INCX == 1 */
	cmpq	$3, M
 	jle	.L14

	testq	$4, X		# aligned for double word?
	je	.L05

	movss	%xmm1, 0 * SIZE(X)
	addq	$SIZE, X
	decq	M
	jle	.L999
	ALIGN_3

.L05:
	testq	$8, X		# aligned for quad word?
	je	.L06

	movsd	%xmm1, 0 * SIZE(X)
	addq	$2 * SIZE, X
	subq	$2, M
	jle	.L999
	ALIGN_3

.L06:
	movq	M,  I
	sarq	$4, I
	jle	.L12
	ALIGN_4

.L11:
#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	movaps	%xmm1,  0 * SIZE(X)
	movaps	%xmm1,  4 * SIZE(X)
	movaps	%xmm1,  8 * SIZE(X)
	movaps	%xmm1, 12 * SIZE(X)
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
	movaps	%xmm1,  4 * SIZE(X)
	addq	$8 * SIZE, X
	ALIGN_3

.L13:
	testq  $4, M
	je    .L14

	movaps	%xmm1,  0 * SIZE(X)
	addq	$4 * SIZE, X
	ALIGN_3

.L14:
	testq  $2, M
	je    .L15

	movsd	%xmm1,  0 * SIZE(X)
	addq	$2 * SIZE, X
	ALIGN_3

.L15:
	testq  $1, M
	je    .L999

	movss	%xmm1,  0 * SIZE(X)
	jmp	.L999
	ALIGN_4

/* incx != 1 */
.L50:
	movq	M,  I		# rcx = n
	sarq	$3, I		# (n >> 3)
	jle	.L52
	ALIGN_4

.L51:
#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	movss	%xmm1, (X)
	addq	INCX, X
	movss	%xmm1, (X)
	addq	INCX, X
	movss	%xmm1, (X)
	addq	INCX, X
	movss	%xmm1, (X)
	addq	INCX, X
	movss	%xmm1, (X)
	addq	INCX, X
	movss	%xmm1, (X)
	addq	INCX, X
	movss	%xmm1, (X)
	addq	INCX, X
	movss	%xmm1, (X)
	addq	INCX, X

	decq	I
	jg	.L51
	ALIGN_4

.L52:
	testq	$7, M
	je	.L999

	testq	$4, M
	je	.L53

	movss	%xmm1, (X)
	addq	INCX, X
	movss	%xmm1, (X)
	addq	INCX, X
	movss	%xmm1, (X)
	addq	INCX, X
	movss	%xmm1, (X)
	addq	INCX, X
	ALIGN_3

.L53:
	testq	$2, M
	je	.L54

	movss	%xmm1, (X)
	addq	INCX, X
	movss	%xmm1, (X)
	addq	INCX, X
	ALIGN_3

.L54:
	testq	$1, M
	je	.L999

	movss	%xmm1, (X)
	jmp	.L999
	ALIGN_4

/* Alpha != ZERO */

.L100:
	cmpq	$SIZE, INCX
	jne	.L150

	subq	$-32 * SIZE, X

	cmpq	$3, M
	jle	.L116

	testq	$SIZE, X
	je	.L105

	movss	-32 * SIZE(X), %xmm1
	mulss	%xmm0, %xmm1
	movss	%xmm1, -32 * SIZE(X)
	addq	$SIZE, X
	decq	M
	jle	.L999
	ALIGN_3

.L105:
	testq	$2 * SIZE, X
	je	.L110

	movsd	-32 * SIZE(X), %xmm1
	mulps	%xmm0, %xmm1
	movsd	%xmm1, -32 * SIZE(X)
	addq	$2 * SIZE, X
	subq	$2, M
	jle	.L999
	ALIGN_3

.L110:
	movq	M,  I
	sarq	$5, I
	jle	.L113

#if defined(BARCELONA) || defined(SHANGHAI)

	movaps	%xmm0, %xmm1
	mulps	-32 * SIZE(X), %xmm1
	movaps	%xmm0, %xmm2
	mulps	-28 * SIZE(X), %xmm2
	movaps	%xmm0, %xmm3
	mulps	-24 * SIZE(X), %xmm3
	movaps	%xmm0, %xmm4
	mulps	-20 * SIZE(X), %xmm4
	movaps	%xmm0, %xmm5
	mulps	-16 * SIZE(X), %xmm5
	movaps	%xmm0, %xmm6
	mulps	-12 * SIZE(X), %xmm6
	movaps	%xmm0, %xmm7
	mulps	 -8 * SIZE(X), %xmm7
	movaps	%xmm0, %xmm8
	mulps	 -4 * SIZE(X), %xmm8

	decq	I 
	jle	.L112
	ALIGN_4

.L111:
#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	movaps	%xmm1, -32 * SIZE(X)
	movaps	%xmm2, -28 * SIZE(X)
	movaps	%xmm3, -24 * SIZE(X)
	movaps	%xmm4, -20 * SIZE(X)

	movaps	%xmm0, %xmm1
	mulps	  0 * SIZE(X), %xmm1
	movaps	%xmm0, %xmm2
	mulps	  4 * SIZE(X), %xmm2
	movaps	%xmm0, %xmm3
	mulps	  8 * SIZE(X), %xmm3
	movaps	%xmm0, %xmm4
	mulps	 12 * SIZE(X), %xmm4

#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE + 64) - PREOFFSET(X)
#endif

	movaps	%xmm5, -16 * SIZE(X)
	movaps	%xmm6, -12 * SIZE(X)
	movaps	%xmm7,  -8 * SIZE(X)
	movaps	%xmm8,  -4 * SIZE(X)

	movaps	%xmm0, %xmm5
	mulps	 16 * SIZE(X), %xmm5
	movaps	%xmm0, %xmm6
	mulps	 20 * SIZE(X), %xmm6
	movaps	%xmm0, %xmm7
	mulps	 24 * SIZE(X), %xmm7
	movaps	%xmm0, %xmm8
	mulps	 28 * SIZE(X), %xmm8

	subq	$-32 * SIZE, X
	decq	I
	jg	.L111
	ALIGN_4

.L112:
	movaps	%xmm1, -32 * SIZE(X)
	movaps	%xmm2, -28 * SIZE(X)
	movaps	%xmm3, -24 * SIZE(X)
	movaps	%xmm4, -20 * SIZE(X)

	movaps	%xmm5, -16 * SIZE(X)
	movaps	%xmm6, -12 * SIZE(X)
	movaps	%xmm7,  -8 * SIZE(X)
	movaps	%xmm8,  -4 * SIZE(X)
	
#else
	
	movaps	-32 * SIZE(X), %xmm1
	movaps	-28 * SIZE(X), %xmm2
	movaps	-24 * SIZE(X), %xmm3
	movaps	-20 * SIZE(X), %xmm4
	movaps	-16 * SIZE(X), %xmm5
	movaps	-12 * SIZE(X), %xmm6
	movaps	 -8 * SIZE(X), %xmm7
	movaps	 -4 * SIZE(X), %xmm8
	decq	I 
	jle	.L112
	ALIGN_4

.L111:
#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	mulps	%xmm0, %xmm1
	movaps	%xmm1, -32 * SIZE(X)
	movaps	 0 * SIZE(X), %xmm1
	mulps	%xmm0, %xmm2
	movaps	%xmm2, -28 * SIZE(X)
	movaps	 4 * SIZE(X), %xmm2
	mulps	%xmm0, %xmm3
	movaps	%xmm3, -24 * SIZE(X)
	movaps	 8 * SIZE(X), %xmm3
	mulps	%xmm0, %xmm4
	movaps	%xmm4, -20 * SIZE(X)
	movaps	12 * SIZE(X), %xmm4

#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE + 64) - PREOFFSET(X)
#endif

	mulps	%xmm0, %xmm5
	movaps	%xmm5, -16 * SIZE(X)
	movaps	16 * SIZE(X), %xmm5
	mulps	%xmm0, %xmm6
	movaps	%xmm6, -12 * SIZE(X)
	movaps	20 * SIZE(X), %xmm6
	mulps	%xmm0, %xmm7
	movaps	%xmm7,  -8 * SIZE(X)
	movaps	24 * SIZE(X), %xmm7
	mulps	%xmm0, %xmm8
	movaps	%xmm8,  -4 * SIZE(X)
	movaps	28 * SIZE(X), %xmm8

	subq	$-32 * SIZE, X
	decq	I
	jg	.L111
	ALIGN_4

.L112:
	mulps	%xmm0, %xmm1
	movaps	%xmm1, -32 * SIZE(X)
	mulps	%xmm0, %xmm2
	movaps	%xmm2, -28 * SIZE(X)
	mulps	%xmm0, %xmm3
	movaps	%xmm3, -24 * SIZE(X)
	mulps	%xmm0, %xmm4
	movaps	%xmm4, -20 * SIZE(X)

	mulps	%xmm0, %xmm5
	movaps	%xmm5, -16 * SIZE(X)
	mulps	%xmm0, %xmm6
	movaps	%xmm6, -12 * SIZE(X)
	mulps	%xmm0, %xmm7
	movaps	%xmm7,  -8 * SIZE(X)
	mulps	%xmm0, %xmm8
	movaps	%xmm8,  -4 * SIZE(X)

#endif

	subq	$-32 * SIZE, X
	ALIGN_3

.L113:
	testq	$31, M
	je	.L999

	testq	$16, M
	je	.L114

	movaps	-32 * SIZE(X), %xmm1
	movaps	-28 * SIZE(X), %xmm3
	movaps	-24 * SIZE(X), %xmm5
	movaps	-20 * SIZE(X), %xmm7

	mulps	%xmm0, %xmm1
	movaps	%xmm1, -32 * SIZE(X)
	mulps	%xmm0, %xmm3
	movaps	%xmm3, -28 * SIZE(X)
	mulps	%xmm0, %xmm5
	movaps	%xmm5, -24 * SIZE(X)
	mulps	%xmm0, %xmm7
	movaps	%xmm7, -20 * SIZE(X)

	addq	$16 * SIZE, X
	ALIGN_3

.L114:
	testq	$8, M
	je	.L115

	movaps	-32 * SIZE(X), %xmm1
	movaps	-28 * SIZE(X), %xmm3

	mulps	%xmm0, %xmm1
	movaps	%xmm1, -32 * SIZE(X)
	mulps	%xmm0, %xmm3
	movaps	%xmm3, -28 * SIZE(X)
	addq	$8 * SIZE, X
	ALIGN_3

.L115:
	testq	$4, M
	je	.L116

	movaps	-32 * SIZE(X), %xmm1
	mulps	%xmm0, %xmm1
	movaps	%xmm1, -32 * SIZE(X)
	addq	$4 * SIZE, X
	ALIGN_3

.L116:
	testq	$2, M
	je	.L117

	movsd	-32 * SIZE(X), %xmm1
	mulps	%xmm0, %xmm1
	movsd	%xmm1,  -32 * SIZE(X)
	addq	$2 * SIZE, X
	ALIGN_3

.L117:
	testq	$1, M
	je	.L999

	movss	-32 * SIZE(X), %xmm1
	mulss	%xmm0, %xmm1
	movss	%xmm1, -32 * SIZE(X)
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
	movss	(X), %xmm1
	addq	INCX, X
	movss	(X), %xmm2
	addq	INCX, X
	movss	(X), %xmm3
	addq	INCX, X
	movss	(X), %xmm4
	addq	INCX, X
	movss	(X), %xmm5
	addq	INCX, X
	movss	(X), %xmm6
	addq	INCX, X
	movss	(X), %xmm7
	addq	INCX, X
	movss	(X), %xmm8
	addq	INCX, X

	mulss	%xmm0, %xmm1
	mulss	%xmm0, %xmm2
	mulss	%xmm0, %xmm3
	mulss	%xmm0, %xmm4
	mulss	%xmm0, %xmm5
	mulss	%xmm0, %xmm6
	mulss	%xmm0, %xmm7
	mulss	%xmm0, %xmm8

	movss	%xmm1, (XX)
	addq	INCX, XX
	movss	%xmm2, (XX)
	addq	INCX, XX
	movss	%xmm3, (XX)
	addq	INCX, XX
	movss	%xmm4, (XX)
	addq	INCX, XX
	movss	%xmm5, (XX)
	addq	INCX, XX
	movss	%xmm6, (XX)
	addq	INCX, XX
	movss	%xmm7, (XX)
	addq	INCX, XX
	movss	%xmm8, (XX)
	addq	INCX, XX
	decq	I
	jg	.L151
	ALIGN_4

.L152:
	testq	$7, M
	je	.L999

	testq	$4, M
	je	.L153

	movss	 (X), %xmm1
	addq	INCX, X
	movss	 (X), %xmm2
	addq	INCX, X
	movss	 (X), %xmm3
	addq	INCX, X
	movss	 (X), %xmm4
	addq	INCX, X

	mulss	%xmm0, %xmm1
	mulss	%xmm0, %xmm2
	mulss	%xmm0, %xmm3
	mulss	%xmm0, %xmm4

	movss	%xmm1,  (XX)
	addq	INCX, XX
	movss	%xmm2,  (XX)
	addq	INCX, XX
	movss	%xmm3,  (XX)
	addq	INCX, XX
	movss	%xmm4,  (XX)
	addq	INCX, XX
	ALIGN_3

.L153:
	testq	$2, M
	je	.L154

	movss	 (X), %xmm1
	addq	INCX, X
	movss	 (X), %xmm2
	addq	INCX, X

	mulss	%xmm0, %xmm1
	mulss	%xmm0, %xmm2

	movss	%xmm1,  (XX)
	addq	INCX, XX
	movss	%xmm2,  (XX)
	addq	INCX, XX
	ALIGN_3

.L154:
	testq	$1, M
	je	.L999

	movss	 (X), %xmm1
	mulss	%xmm0, %xmm1
	movss	%xmm1,  (X)
	ALIGN_4

.L999:
	xorq	%rax, %rax

	RESTOREREGISTERS

	ret

	EPILOGUE
