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
	
#ifdef USE_MIN
#define maxps	minps
#define maxss	minss
#endif
	
#include "l1param.h"

	PROLOGUE
	PROFCODE

	SAVEREGISTERS

	xorps	%xmm0, %xmm0
	leaq	(, INCX, SIZE), INCX

	testq	M, M
	jle	.L999

#ifdef USE_ABS
	pcmpeqb	%xmm15, %xmm15
	psrld	$1,    %xmm15
#endif

	movss	(X), %xmm0
	shufps	$0,  %xmm0, %xmm0
#ifdef USE_ABS
	andps	%xmm15, %xmm0
#endif
	movaps	%xmm0, %xmm1
	movaps	%xmm0, %xmm2
	movaps	%xmm0, %xmm3
	addq	INCX, X
	decq	M
	jle	.L999

	cmpq	$SIZE, INCX
	jne	.L40

	subq	$-32 * SIZE, X

	cmpq	$3, M
	jle	.L17

	testq	$SIZE, X
	je	.L05

	movss	-32 * SIZE(X), %xmm1
	shufps	$0, %xmm1, %xmm1
#ifdef USE_ABS
	andps	%xmm15, %xmm1
#endif
	decq	M
	addq	$SIZE, X
	ALIGN_3

.L05:
	testq	$2 * SIZE, X
	je	.L06

	movsd	-32 * SIZE(X), %xmm2
	unpcklps  %xmm2, %xmm2
#ifdef USE_ABS
	andps	%xmm15, %xmm2
#endif
	subq	$2, M
	addq	$2 * SIZE, X
	ALIGN_3

.L06:
	movq	M,  I
	sarq	$5, I
	jle	.L15

	movaps	-32 * SIZE(X), %xmm4
	movaps	-28 * SIZE(X), %xmm5
	movaps	-24 * SIZE(X), %xmm6
	movaps	-20 * SIZE(X), %xmm7

	decq	I
	jle	.L12
	ALIGN_4
	
.L11:
#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

#ifdef USE_ABS
	andps	%xmm15, %xmm4
#endif
	maxps	%xmm4, %xmm0
	movaps	-16 * SIZE(X), %xmm4

#ifdef USE_ABS
	andps	%xmm15, %xmm5
#endif
	maxps	%xmm5, %xmm1
	movaps	-12 * SIZE(X), %xmm5

#ifdef USE_ABS
	andps	%xmm15, %xmm6
#endif
	maxps	%xmm6, %xmm2
	movaps	 -8 * SIZE(X), %xmm6

#ifdef USE_ABS
	andps	%xmm15, %xmm7
#endif
	maxps	%xmm7, %xmm3
	movaps	 -4 * SIZE(X), %xmm7

#if defined(PREFETCH) && !defined(FETCH128)
	PREFETCH (PREFETCHSIZE + 64) - PREOFFSET(X)
#endif

#ifdef USE_ABS
	andps	%xmm15, %xmm4
#endif
	maxps	%xmm4, %xmm0
	movaps	  0 * SIZE(X), %xmm4

#ifdef USE_ABS
	andps	%xmm15, %xmm5
#endif
	maxps	%xmm5, %xmm1
	movaps	  4 * SIZE(X), %xmm5

#ifdef USE_ABS
	andps	%xmm15, %xmm6
#endif
	maxps	%xmm6, %xmm2
	movaps	  8 * SIZE(X), %xmm6

#ifdef USE_ABS
	andps	%xmm15, %xmm7
#endif
	maxps	%xmm7, %xmm3
	movaps	 12 * SIZE(X), %xmm7

	subq	$-32 * SIZE, X
	decq	I
	jg	.L11
	ALIGN_4

.L12:
#ifdef USE_ABS
	andps	%xmm15, %xmm4
#endif
	maxps	%xmm4, %xmm0
	movaps	-16 * SIZE(X), %xmm4

#ifdef USE_ABS
	andps	%xmm15, %xmm5
#endif
	maxps	%xmm5, %xmm1
	movaps	-12 * SIZE(X), %xmm5

#ifdef USE_ABS
	andps	%xmm15, %xmm6
#endif
	maxps	%xmm6, %xmm2
	movaps	 -8 * SIZE(X), %xmm6

#ifdef USE_ABS
	andps	%xmm15, %xmm7
#endif
	maxps	%xmm7, %xmm3
	movaps	 -4 * SIZE(X), %xmm7

#ifdef USE_ABS
	andps	%xmm15, %xmm4
#endif
	maxps	%xmm4, %xmm0

#ifdef USE_ABS
	andps	%xmm15, %xmm5
#endif
	maxps	%xmm5, %xmm1

#ifdef USE_ABS
	andps	%xmm15, %xmm6
#endif
	maxps	%xmm6, %xmm2

#ifdef USE_ABS
	andps	%xmm15, %xmm7
#endif
	maxps	%xmm7, %xmm3

	subq	$-32 * SIZE, X
	ALIGN_3


.L15:
	testq	$16, M
	je	.L16

	movaps	-32 * SIZE(X), %xmm4
#ifdef USE_ABS
	andps	%xmm15, %xmm4
#endif
	maxps	%xmm4, %xmm0

	movaps	-28 * SIZE(X), %xmm5
#ifdef USE_ABS
	andps	%xmm15, %xmm5
#endif
	maxps	%xmm5, %xmm1

	movaps	-24 * SIZE(X), %xmm6
#ifdef USE_ABS
	andps	%xmm15, %xmm6
#endif
	maxps	%xmm6, %xmm2

	movaps	-20 * SIZE(X), %xmm7
#ifdef USE_ABS
	andps	%xmm15, %xmm7
#endif
	maxps	%xmm7, %xmm3

	addq	$16 * SIZE, X
	ALIGN_3

.L16:
	testq	$8, M
	je	.L17

	movaps	-32 * SIZE(X), %xmm4
#ifdef USE_ABS
	andps	%xmm15, %xmm4
#endif
	maxps	%xmm4, %xmm0

	movaps	-28 * SIZE(X), %xmm5
#ifdef USE_ABS
	andps	%xmm15, %xmm5
#endif
	maxps	%xmm5, %xmm1
	addq	$8 * SIZE, X
	ALIGN_3

.L17:
	testq	$4, M
	je	.L18

	movaps	-32 * SIZE(X), %xmm4
#ifdef USE_ABS
	andps	%xmm15, %xmm4
#endif
	maxps	%xmm4, %xmm2
	addq	$4 * SIZE, X
	ALIGN_3	

.L18:
	testq	$2, M
	je	.L19

	movsd	-32 * SIZE(X), %xmm4
	unpcklps %xmm4, %xmm4
#ifdef USE_ABS
	andps	%xmm15, %xmm4
#endif
	maxps	%xmm4, %xmm3
	addq	$2 * SIZE, X
	ALIGN_3
	
.L19:
	testq	$1, M
	je	.L998

	movss	-32 * SIZE(X), %xmm4
#ifdef USE_ABS
	andps	%xmm15, %xmm4
#endif
	maxss	%xmm4, %xmm0
	jmp	.L998
	ALIGN_3

.L40:
	movq	M,  I
	sarq	$3, I
	jle	.L45
	ALIGN_4
	
.L41:
	movss	(X), %xmm4
	addq	INCX, X
#ifdef USE_ABS
	andps	%xmm15, %xmm4
#endif
	maxss	%xmm4, %xmm0

	movss	(X), %xmm5
	addq	INCX, X
#ifdef USE_ABS
	andps	%xmm15, %xmm5
#endif
	maxss	%xmm5, %xmm1

	movss	(X), %xmm6
	addq	INCX, X
#ifdef USE_ABS
	andps	%xmm15, %xmm6
#endif
	maxss	%xmm6, %xmm2

	movss	(X), %xmm7
	addq	INCX, X
#ifdef USE_ABS
	andps	%xmm15, %xmm7
#endif
	maxss	%xmm7, %xmm3

	movss	(X), %xmm4
	addq	INCX, X
#ifdef USE_ABS
	andps	%xmm15, %xmm4
#endif
	maxss	%xmm4, %xmm0

	movss	(X), %xmm5
	addq	INCX, X
#ifdef USE_ABS
	andps	%xmm15, %xmm5
#endif
	maxss	%xmm5, %xmm1

	movss	(X), %xmm6
	addq	INCX, X
#ifdef USE_ABS
	andps	%xmm15, %xmm6
#endif
	maxss	%xmm6, %xmm2

	movss	(X), %xmm7
	addq	INCX, X
#ifdef USE_ABS
	andps	%xmm15, %xmm7
#endif
	maxss	%xmm7, %xmm3

	decq	I
	jg	.L41
	ALIGN_4

.L45:
	testq	$4, M
	je	.L46

	movss	(X), %xmm4
	addq	INCX, X
#ifdef USE_ABS
	andps	%xmm15, %xmm4
#endif
	maxss	%xmm4, %xmm0

	movss	(X), %xmm5
	addq	INCX, X
#ifdef USE_ABS
	andps	%xmm15, %xmm5
#endif
	maxss	%xmm5, %xmm1

	movss	(X), %xmm6
	addq	INCX, X
#ifdef USE_ABS
	andps	%xmm15, %xmm6
#endif
	maxss	%xmm6, %xmm2

	movss	(X), %xmm7
	addq	INCX, X
#ifdef USE_ABS
	andps	%xmm15, %xmm7
#endif
	maxss	%xmm7, %xmm3
	ALIGN_3	

.L46:
	testq	$2, M
	je	.L47

	movss	(X), %xmm4
	addq	INCX, X
#ifdef USE_ABS
	andps	%xmm15, %xmm4
#endif
	maxss	%xmm4, %xmm0

	movss	(X), %xmm5
	addq	INCX, X
#ifdef USE_ABS
	andps	%xmm15, %xmm5
#endif
	maxss	%xmm5, %xmm1
	ALIGN_3
	
.L47:
	testq	$1, M
	je	.L998

	movss	(X), %xmm4
	addq	INCX, X
#ifdef USE_ABS
	andps	%xmm15, %xmm4
#endif
	maxss	%xmm4, %xmm2
	ALIGN_4

.L998:
	maxps	%xmm1, %xmm0
	maxps	%xmm3, %xmm2
	maxps	%xmm2, %xmm0
	movaps	%xmm0, %xmm1
	movhlps %xmm0, %xmm0
	maxps	%xmm1, %xmm0
	movaps	%xmm0, %xmm1
	shufps	$1, %xmm0, %xmm0
	maxss	%xmm1, %xmm0
	ALIGN_4

.L999:
	RESTOREREGISTERS

	ret

	EPILOGUE
