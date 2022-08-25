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
#define STACK_X		20 + STACK + ARGS(%esp)
#define STACK_INCX	24 + STACK + ARGS(%esp)

#define M	%ebx
#define X	%ecx
#define INCX	%edx
#define I	%esi
#define XX	%edi

#include "l1param.h"

	PROLOGUE

	pushl	%edi
	pushl	%esi
	pushl	%ebx

	PROFCODE

	movl	STACK_M,     M
	movl	STACK_X,     X
	movl	STACK_INCX,  INCX

	lea	(, INCX, SIZE), INCX

	movss	STACK_ALPHA, %xmm0

	testl	M, M
	jle	.L999

	xorps	%xmm1, %xmm1
	comiss	%xmm0, %xmm1
	shufps	$0, %xmm0, %xmm0

	jne	.L100

/* Alpha == ZERO */
	cmpl	$SIZE, INCX
	jne	.L50

/* INCX == 1 */
	cmpl	$3, M
 	jle	.L14

	testl	$4, X		# aligned for double word?
	je	.L05

	movss	%xmm1, 0 * SIZE(X)
	addl	$SIZE, X
	decl	M
	jle	.L999
	ALIGN_3

.L05:
	testl	$8, X		# aligned for quad word?
	je	.L06

	movsd	%xmm1, 0 * SIZE(X)
	addl	$2 * SIZE, X
	subl	$2, M
	jle	.L999
	ALIGN_3

.L06:
	movl	M,  I
	sarl	$4, I
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
	movaps	%xmm1,  4 * SIZE(X)
	addl	$8 * SIZE, X
	ALIGN_3

.L13:
	testl  $4, M
	je    .L14

	movaps	%xmm1,  0 * SIZE(X)
	addl	$4 * SIZE, X
	ALIGN_3

.L14:
	testl  $2, M
	je    .L15

	movsd	%xmm1,  0 * SIZE(X)
	addl	$2 * SIZE, X
	ALIGN_3

.L15:
	testl  $1, M
	je    .L999

	movss	%xmm1,  0 * SIZE(X)
	jmp	.L999
	ALIGN_4

/* incx != 1 */
.L50:
	movl	M,  I		# rcx = n
	sarl	$3, I		# (n >> 3)
	jle	.L52
	ALIGN_4

.L51:
#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	movss	%xmm1, (X)
	addl	INCX, X
	movss	%xmm1, (X)
	addl	INCX, X
	movss	%xmm1, (X)
	addl	INCX, X
	movss	%xmm1, (X)
	addl	INCX, X
	movss	%xmm1, (X)
	addl	INCX, X
	movss	%xmm1, (X)
	addl	INCX, X
	movss	%xmm1, (X)
	addl	INCX, X
	movss	%xmm1, (X)
	addl	INCX, X

	decl	I
	jg	.L51
	ALIGN_4

.L52:
	testl	$7, M
	je	.L999

	testl	$4, M
	je	.L53

	movss	%xmm1, (X)
	addl	INCX, X
	movss	%xmm1, (X)
	addl	INCX, X
	movss	%xmm1, (X)
	addl	INCX, X
	movss	%xmm1, (X)
	addl	INCX, X
	ALIGN_3

.L53:
	testl	$2, M
	je	.L54

	movss	%xmm1, (X)
	addl	INCX, X
	movss	%xmm1, (X)
	addl	INCX, X
	ALIGN_3

.L54:
	testl	$1, M
	je	.L999

	movss	%xmm1, (X)
	jmp	.L999
	ALIGN_4

/* Alpha != ZERO */

.L100:
	cmpl	$SIZE, INCX
	jne	.L150

	subl	$-32 * SIZE, X

	cmpl	$3, M
	jle	.L116

	testl	$SIZE, X
	je	.L105

	movss	-32 * SIZE(X), %xmm1
	mulss	%xmm0, %xmm1
	movss	%xmm1, -32 * SIZE(X)
	addl	$SIZE, X
	decl	M
	jle	.L999
	ALIGN_3

.L105:
	testl	$2 * SIZE, X
	je	.L110

	movsd	-32 * SIZE(X), %xmm1
	mulps	%xmm0, %xmm1
	movsd	%xmm1, -32 * SIZE(X)
	addl	$2 * SIZE, X
	subl	$2, M
	jle	.L999
	ALIGN_3

.L110:
	movl	M,  I
	sarl	$5, I
	jle	.L113

#if defined(BARCELONA)

	movaps	%xmm0, %xmm1
	mulps	-32 * SIZE(X), %xmm1
	movaps	%xmm0, %xmm2
	mulps	-28 * SIZE(X), %xmm2
	movaps	%xmm0, %xmm3
	mulps	-24 * SIZE(X), %xmm3
	movaps	%xmm0, %xmm4
	mulps	-20 * SIZE(X), %xmm4

	decl	I 
	jle	.L112
	ALIGN_4

.L111:
#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	movaps	%xmm1, -32 * SIZE(X)
	movaps	%xmm0, %xmm1
	mulps	-16 * SIZE(X), %xmm1

	movaps	%xmm2, -28 * SIZE(X)
	movaps	%xmm0, %xmm2
	mulps	-12 * SIZE(X), %xmm2

	movaps	%xmm3, -24 * SIZE(X)
	movaps	%xmm0, %xmm3
	mulps	 -8 * SIZE(X), %xmm3

	movaps	%xmm4, -20 * SIZE(X)
	movaps	%xmm0, %xmm4
	mulps	 -4 * SIZE(X), %xmm4

#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE + 64) - PREOFFSET(X)
#endif

	movaps	%xmm1, -16 * SIZE(X)
	movaps	%xmm0, %xmm1
	mulps	  0 * SIZE(X), %xmm1

	movaps	%xmm2, -12 * SIZE(X)
	movaps	%xmm0, %xmm2
	mulps	  4 * SIZE(X), %xmm2

	movaps	%xmm3,  -8 * SIZE(X)
	movaps	%xmm0, %xmm3
	mulps	  8 * SIZE(X), %xmm3

	movaps	%xmm4,  -4 * SIZE(X)
	movaps	%xmm0, %xmm4
	mulps	 12 * SIZE(X), %xmm4

	subl	$-32 * SIZE, X
	decl	I
	jg	.L111
	ALIGN_4

.L112:
	movaps	%xmm1, -32 * SIZE(X)
	movaps	%xmm0, %xmm1
	mulps	-16 * SIZE(X), %xmm1

	movaps	%xmm2, -28 * SIZE(X)
	movaps	%xmm0, %xmm2
	mulps	-12 * SIZE(X), %xmm2

	movaps	%xmm3, -24 * SIZE(X)
	movaps	%xmm0, %xmm3
	mulps	 -8 * SIZE(X), %xmm3

	movaps	%xmm4, -20 * SIZE(X)
	movaps	%xmm0, %xmm4
	mulps	 -4 * SIZE(X), %xmm4

	movaps	%xmm1, -16 * SIZE(X)
	movaps	%xmm2, -12 * SIZE(X)
	movaps	%xmm3,  -8 * SIZE(X)
	movaps	%xmm4,  -4 * SIZE(X)

#else
	
	movaps	-32 * SIZE(X), %xmm1
	movaps	-28 * SIZE(X), %xmm2
	movaps	-24 * SIZE(X), %xmm3
	movaps	-20 * SIZE(X), %xmm4

	decl	I 
	jle	.L112
	ALIGN_4

.L111:
#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	mulps	%xmm0, %xmm1
	movaps	%xmm1, -32 * SIZE(X)
	movaps	-16 * SIZE(X), %xmm1

	mulps	%xmm0, %xmm2
	movaps	%xmm2, -28 * SIZE(X)
	movaps	-12 * SIZE(X), %xmm2

	mulps	%xmm0, %xmm3
	movaps	%xmm3, -24 * SIZE(X)
	movaps	 -8 * SIZE(X), %xmm3

	mulps	%xmm0, %xmm4
	movaps	%xmm4, -20 * SIZE(X)
	movaps	 -4 * SIZE(X), %xmm4

#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE + 64) - PREOFFSET(X)
#endif

	mulps	%xmm0, %xmm1
	movaps	%xmm1, -16 * SIZE(X)
	movaps	  0 * SIZE(X), %xmm1

	mulps	%xmm0, %xmm2
	movaps	%xmm2, -12 * SIZE(X)
	movaps	  4 * SIZE(X), %xmm2

	mulps	%xmm0, %xmm3
	movaps	%xmm3,  -8 * SIZE(X)
	movaps	  8 * SIZE(X), %xmm3

	mulps	%xmm0, %xmm4
	movaps	%xmm4,  -4 * SIZE(X)
	movaps	 12 * SIZE(X), %xmm4

	subl	$-32 * SIZE, X
	decl	I
	jg	.L111
	ALIGN_4

.L112:
	mulps	%xmm0, %xmm1
	movaps	%xmm1, -32 * SIZE(X)
	movaps	-16 * SIZE(X), %xmm1

	mulps	%xmm0, %xmm2
	movaps	%xmm2, -28 * SIZE(X)
	movaps	-12 * SIZE(X), %xmm2

	mulps	%xmm0, %xmm3
	movaps	%xmm3, -24 * SIZE(X)
	movaps	 -8 * SIZE(X), %xmm3

	mulps	%xmm0, %xmm4
	movaps	%xmm4, -20 * SIZE(X)
	movaps	 -4 * SIZE(X), %xmm4

	mulps	%xmm0, %xmm1
	movaps	%xmm1, -16 * SIZE(X)
	mulps	%xmm0, %xmm2
	movaps	%xmm2, -12 * SIZE(X)
	mulps	%xmm0, %xmm3
	movaps	%xmm3,  -8 * SIZE(X)
	mulps	%xmm0, %xmm4
	movaps	%xmm4,  -4 * SIZE(X)

#endif

	subl	$-32 * SIZE, X
	ALIGN_3

.L113:
	testl	$31, M
	je	.L999

	testl	$16, M
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

	addl	$16 * SIZE, X
	ALIGN_3

.L114:
	testl	$8, M
	je	.L115

	movaps	-32 * SIZE(X), %xmm1
	movaps	-28 * SIZE(X), %xmm3

	mulps	%xmm0, %xmm1
	movaps	%xmm1, -32 * SIZE(X)
	mulps	%xmm0, %xmm3
	movaps	%xmm3, -28 * SIZE(X)
	addl	$8 * SIZE, X
	ALIGN_3

.L115:
	testl	$4, M
	je	.L116

	movaps	-32 * SIZE(X), %xmm1
	mulps	%xmm0, %xmm1
	movaps	%xmm1, -32 * SIZE(X)
	addl	$4 * SIZE, X
	ALIGN_3

.L116:
	testl	$2, M
	je	.L117

	movsd	-32 * SIZE(X), %xmm1
	mulps	%xmm0, %xmm1
	movsd	%xmm1,  -32 * SIZE(X)
	addl	$2 * SIZE, X
	ALIGN_3

.L117:
	testl	$1, M
	je	.L999

	movss	-32 * SIZE(X), %xmm1
	mulss	%xmm0, %xmm1
	movss	%xmm1, -32 * SIZE(X)
	jmp	.L999
	ALIGN_3

/* incx != 1 */

.L150:
	movl	X, XX
	movl	M,  I		# rcx = n
	sarl	$3, I		# (n >> 3)
	jle	.L152
	ALIGN_4

.L151:
	movss	(X), %xmm1
	addl	INCX, X
	movss	(X), %xmm2
	addl	INCX, X
	movss	(X), %xmm3
	addl	INCX, X
	movss	(X), %xmm4
	addl	INCX, X

	mulss	%xmm0, %xmm1
	mulss	%xmm0, %xmm2
	mulss	%xmm0, %xmm3
	mulss	%xmm0, %xmm4

	movss	%xmm1, (XX)
	addl	INCX, XX
	movss	%xmm2, (XX)
	addl	INCX, XX
	movss	%xmm3, (XX)
	addl	INCX, XX
	movss	%xmm4, (XX)
	addl	INCX, XX

	movss	(X), %xmm1
	addl	INCX, X
	movss	(X), %xmm2
	addl	INCX, X
	movss	(X), %xmm3
	addl	INCX, X
	movss	(X), %xmm4
	addl	INCX, X

	mulss	%xmm0, %xmm1
	mulss	%xmm0, %xmm2
	mulss	%xmm0, %xmm3
	mulss	%xmm0, %xmm4

	movss	%xmm1, (XX)
	addl	INCX, XX
	movss	%xmm2, (XX)
	addl	INCX, XX
	movss	%xmm3, (XX)
	addl	INCX, XX
	movss	%xmm4, (XX)
	addl	INCX, XX

	decl	I
	jg	.L151
	ALIGN_4

.L152:
	testl	$7, M
	je	.L999

	testl	$4, M
	je	.L153

	movss	 (X), %xmm1
	addl	INCX, X
	movss	 (X), %xmm2
	addl	INCX, X
	movss	 (X), %xmm3
	addl	INCX, X
	movss	 (X), %xmm4
	addl	INCX, X

	mulss	%xmm0, %xmm1
	mulss	%xmm0, %xmm2
	mulss	%xmm0, %xmm3
	mulss	%xmm0, %xmm4

	movss	%xmm1,  (XX)
	addl	INCX, XX
	movss	%xmm2,  (XX)
	addl	INCX, XX
	movss	%xmm3,  (XX)
	addl	INCX, XX
	movss	%xmm4,  (XX)
	addl	INCX, XX
	ALIGN_3

.L153:
	testl	$2, M
	je	.L154

	movss	 (X), %xmm1
	addl	INCX, X
	movss	 (X), %xmm2
	addl	INCX, X

	mulss	%xmm0, %xmm1
	mulss	%xmm0, %xmm2

	movss	%xmm1,  (XX)
	addl	INCX, XX
	movss	%xmm2,  (XX)
	addl	INCX, XX
	ALIGN_3

.L154:
	testl	$1, M
	je	.L999

	movss	 (X), %xmm1
	mulss	%xmm0, %xmm1
	movss	%xmm1,  (X)
	ALIGN_4

.L999:
	xorl	%eax, %eax

	popl	%ebx
	popl	%esi
	popl	%edi
	ret

	EPILOGUE
