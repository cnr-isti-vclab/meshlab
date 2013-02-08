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
#define maxpd	minpd
#define maxsd	minsd
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
	psrlq	$1, %xmm15
#endif

	movsd	(X), %xmm0
	addq	INCX, X
#ifdef USE_ABS
	andps	 %xmm15, %xmm0
#endif
	unpcklpd %xmm0, %xmm0
	movaps	 %xmm0, %xmm1
	movaps	 %xmm0, %xmm2
	movaps	 %xmm0, %xmm3
	decq	M
	jle	.L999

	cmpq	$SIZE, INCX
	jne	.L40

	subq	$-16 * SIZE, X

	testq	$SIZE, X
	je	.L05

	movsd	-16 * SIZE(X), %xmm4
#ifdef USE_ABS
	andps	 %xmm15, %xmm4
#endif
	unpcklpd  %xmm4, %xmm4
	maxpd	  %xmm4, %xmm3
	addq	$SIZE, X
	decq	M
	jle	.L998
	ALIGN_3

.L05:
	movq	M,  I
	sarq	$4, I
	jle	.L15

	movaps	-16 * SIZE(X), %xmm4
	movaps	-14 * SIZE(X), %xmm5
	movaps	-12 * SIZE(X), %xmm6
	movaps	-10 * SIZE(X), %xmm7

	decq	I
	jle	.L12
	ALIGN_4
	
.L11:
#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

#ifdef USE_ABS
	andps	 %xmm15, %xmm4
#endif
	maxpd	%xmm4, %xmm0
	movaps	 -8 * SIZE(X), %xmm4

#ifdef USE_ABS
	andps	 %xmm15, %xmm5
#endif
	maxpd	%xmm5, %xmm1
	movaps	 -6 * SIZE(X), %xmm5

#ifdef USE_ABS
	andps	 %xmm15, %xmm6
#endif
	maxpd	%xmm6, %xmm2
	movaps	 -4 * SIZE(X), %xmm6

#ifdef USE_ABS
	andps	 %xmm15, %xmm7
#endif
	maxpd	%xmm7, %xmm3
	movaps	 -2 * SIZE(X), %xmm7

#if defined(PREFETCH) && !defined(FETCH128)
	PREFETCH (PREFETCHSIZE +  64) - PREOFFSET(X)
#endif

#ifdef USE_ABS
	andps	 %xmm15, %xmm4
#endif
	maxpd	%xmm4, %xmm0
	movaps	  0 * SIZE(X), %xmm4

#ifdef USE_ABS
	andps	 %xmm15, %xmm5
#endif
	maxpd	%xmm5, %xmm1
	movaps	  2 * SIZE(X), %xmm5

#ifdef USE_ABS
	andps	 %xmm15, %xmm6
#endif
	maxpd	%xmm6, %xmm2
	movaps	  4 * SIZE(X), %xmm6

#ifdef USE_ABS
	andps	 %xmm15, %xmm7
#endif
	maxpd	%xmm7, %xmm3
	movaps	  6 * SIZE(X), %xmm7

	subq	$-16 * SIZE, X
	decq	I
	jg	.L11
	ALIGN_4

.L12:
#ifdef USE_ABS
	andps	 %xmm15, %xmm4
#endif
	maxpd	%xmm4, %xmm0
	movaps	 -8 * SIZE(X), %xmm4

#ifdef USE_ABS
	andps	 %xmm15, %xmm5
#endif
	maxpd	%xmm5, %xmm1
	movaps	 -6 * SIZE(X), %xmm5

#ifdef USE_ABS
	andps	 %xmm15, %xmm6
#endif
	maxpd	%xmm6, %xmm2
	movaps	 -4 * SIZE(X), %xmm6

#ifdef USE_ABS
	andps	 %xmm15, %xmm7
#endif
	maxpd	%xmm7, %xmm3
	movaps	 -2 * SIZE(X), %xmm7

#ifdef USE_ABS
	andps	 %xmm15, %xmm4
#endif
	maxpd	%xmm4, %xmm0

#ifdef USE_ABS
	andps	 %xmm15, %xmm5
#endif
	maxpd	%xmm5, %xmm1

#ifdef USE_ABS
	andps	 %xmm15, %xmm6
#endif
	maxpd	%xmm6, %xmm2

#ifdef USE_ABS
	andps	 %xmm15, %xmm7
#endif
	maxpd	%xmm7, %xmm3

	subq	$-16 * SIZE, X
	ALIGN_4

.L15:
	testq	$8, M
	jle	.L16

	movaps	-16 * SIZE(X), %xmm4
#ifdef USE_ABS
	andps	 %xmm15, %xmm4
#endif
	maxpd	%xmm4, %xmm0

	movaps	-14 * SIZE(X), %xmm5
#ifdef USE_ABS
	andps	 %xmm15, %xmm5
#endif
	maxpd	%xmm5, %xmm1

	movaps	-12 * SIZE(X), %xmm6
#ifdef USE_ABS
	andps	 %xmm15, %xmm6
#endif
	maxpd	%xmm6, %xmm2

	movaps	-10 * SIZE(X), %xmm7
#ifdef USE_ABS
	andps	 %xmm15, %xmm7
#endif
	maxpd	%xmm7, %xmm3
	addq	$8 * SIZE, X
	ALIGN_3

.L16:
	testq	$4, M
	jle	.L17

	movaps	-16 * SIZE(X), %xmm4
#ifdef USE_ABS
	andps	 %xmm15, %xmm4
#endif
	maxpd	%xmm4, %xmm0

	movaps	-14 * SIZE(X), %xmm5
#ifdef USE_ABS
	andps	 %xmm15, %xmm5
#endif
	maxpd	%xmm5, %xmm1

	addq	$4 * SIZE, X
	ALIGN_3	

.L17:
	testq	$2, M
	jle	.L18

	movaps	-16 * SIZE(X), %xmm4
#ifdef USE_ABS
	andps	 %xmm15, %xmm4
#endif
	maxpd	%xmm4, %xmm2
	addq	$2 * SIZE, X
	ALIGN_3	
	
.L18:
	testq	$1, M
	jle	.L998

	movsd	-16 * SIZE(X), %xmm4
	unpcklpd  %xmm4, %xmm4
#ifdef USE_ABS
	andps	 %xmm15, %xmm4
#endif
	maxpd	%xmm4, %xmm3
	jmp	.L998
	ALIGN_3

.L40:
	movq	M,  I
	sarq	$4, I
	jle	.L45
	ALIGN_4
	
.L41:
#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	movsd	(X), %xmm4
	addq	INCX, X
	movhps	(X), %xmm4
	addq	INCX, X
#ifdef USE_ABS
	andps	 %xmm15, %xmm4
#endif
	maxpd	%xmm4, %xmm0

	movsd	(X), %xmm5
	addq	INCX, X
	movhps	(X), %xmm5
	addq	INCX, X
#ifdef USE_ABS
	andps	 %xmm15, %xmm5
#endif
	maxpd	%xmm5, %xmm1

	movsd	(X), %xmm6
	addq	INCX, X
	movhps	(X), %xmm6
	addq	INCX, X
#ifdef USE_ABS
	andps	 %xmm15, %xmm6
#endif
	maxpd	%xmm6, %xmm2

	movsd	(X), %xmm7
	addq	INCX, X
	movhps	(X), %xmm7
	addq	INCX, X
#ifdef USE_ABS
	andps	 %xmm15, %xmm7
#endif
	maxpd	%xmm7, %xmm3

#if defined(PREFETCH) && !defined(FETCH128)
	PREFETCH (PREFETCHSIZE +  64) - PREOFFSET(X)
#endif

	movsd	(X), %xmm4
	addq	INCX, X
	movhps	(X), %xmm4
	addq	INCX, X
#ifdef USE_ABS
	andps	 %xmm15, %xmm4
#endif
	maxpd	%xmm4, %xmm0

	movsd	(X), %xmm5
	addq	INCX, X
	movhps	(X), %xmm5
	addq	INCX, X
#ifdef USE_ABS
	andps	 %xmm15, %xmm5
#endif
	maxpd	%xmm5, %xmm1

	movsd	(X), %xmm6
	addq	INCX, X
	movhps	(X), %xmm6
	addq	INCX, X
#ifdef USE_ABS
	andps	 %xmm15, %xmm6
#endif
	maxpd	%xmm6, %xmm2

	movsd	(X), %xmm7
	addq	INCX, X
	movhps	(X), %xmm7
	addq	INCX, X
#ifdef USE_ABS
	andps	 %xmm15, %xmm7
#endif
	maxpd	%xmm7, %xmm3

	decq	I
	jg	.L41
	ALIGN_4

.L45:
	andq	$15,  M
	jle	.L998

	testq	$8, M
	je	.L46

	movsd	(X), %xmm4
	addq	INCX, X
	movhps	(X), %xmm4
	addq	INCX, X
#ifdef USE_ABS
	andps	 %xmm15, %xmm4
#endif
	maxpd	%xmm4, %xmm0

	movsd	(X), %xmm5
	addq	INCX, X
	movhps	(X), %xmm5
	addq	INCX, X
#ifdef USE_ABS
	andps	 %xmm15, %xmm5
#endif
	maxpd	%xmm5, %xmm1

	movsd	(X), %xmm6
	addq	INCX, X
	movhps	(X), %xmm6
	addq	INCX, X
#ifdef USE_ABS
	andps	 %xmm15, %xmm6
#endif
	maxpd	%xmm6, %xmm2

	movsd	(X), %xmm7
	addq	INCX, X
	movhps	(X), %xmm7
	addq	INCX, X
#ifdef USE_ABS
	andps	 %xmm15, %xmm7
#endif
	maxpd	%xmm7, %xmm3
	ALIGN_3

.L46:
	testq	$4, M
	je	.L47

	movsd	(X), %xmm4
	addq	INCX, X
	movhps	(X), %xmm4
	addq	INCX, X
#ifdef USE_ABS
	andps	 %xmm15, %xmm4
#endif
	maxpd	%xmm4, %xmm0

	movsd	(X), %xmm5
	addq	INCX, X
	movhps	(X), %xmm5
	addq	INCX, X
#ifdef USE_ABS
	andps	 %xmm15, %xmm5
#endif
	maxpd	%xmm5, %xmm1
	ALIGN_3	

.L47:
	testq	$2, M
	je	.L48

	movsd	(X), %xmm6
	addq	INCX, X
	movhps	(X), %xmm6
	addq	INCX, X
#ifdef USE_ABS
	andps	 %xmm15, %xmm6
#endif
	maxpd	%xmm6, %xmm2
	ALIGN_3
	
.L48:
	testq	$1, M
	je	.L998

	movsd	(X), %xmm7
	unpcklpd  %xmm7, %xmm7
#ifdef USE_ABS
	andps	 %xmm15, %xmm7
#endif
	maxpd	%xmm7, %xmm3
	ALIGN_4

.L998:
	maxpd	%xmm1, %xmm0
	maxpd	%xmm3, %xmm2
	maxpd	%xmm2, %xmm0
	movaps	%xmm0, %xmm1
	unpckhpd %xmm0, %xmm0
	maxsd	%xmm1, %xmm0
	ALIGN_4

.L999:
	RESTOREREGISTERS

	ret

	EPILOGUE
