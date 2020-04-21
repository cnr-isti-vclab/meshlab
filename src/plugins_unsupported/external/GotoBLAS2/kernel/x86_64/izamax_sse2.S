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

#define RET	%rax
#define I	ARG4
#define XX	%r10
#define	MM	%r11
	
#ifdef USE_MIN
#define maxpd	minpd
#define maxsd	minsd
#endif

#include "l1param.h"

	PROLOGUE
	PROFCODE

	SAVEREGISTERS

	pxor	%xmm0, %xmm0
	xor	RET, RET
	testq	M, M
	jle	.L999
	testq	INCX, INCX
	jle	.L999

	salq	$ZBASE_SHIFT, INCX
	movq	M, MM
	movq	X, XX

	pcmpeqb	%xmm15, %xmm15
	psrlq	$1, %xmm15

	movsd	0 * SIZE(X), %xmm0
	movsd	1 * SIZE(X), %xmm1
	addq	INCX, X
	decq	M
	andpd	 %xmm15, %xmm0
	andpd	 %xmm15, %xmm1
	addpd	 %xmm1, %xmm0
	unpcklpd %xmm0, %xmm0
	movapd	 %xmm0, %xmm1
	movapd	 %xmm0, %xmm2
	movapd	 %xmm0, %xmm3
	cmpq	$2 * SIZE, INCX
	jne	.L60

	movq	M,  I
	sarq	$3, I
	jle	.L25
	ALIGN_4

.L21:
#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	movsd	0 * SIZE(X), %xmm4
	movsd	1 * SIZE(X), %xmm5
	movhpd	2 * SIZE(X), %xmm4
	movhpd	3 * SIZE(X), %xmm5
	movsd	4 * SIZE(X), %xmm6
	movsd	5 * SIZE(X), %xmm7
	movhpd	6 * SIZE(X), %xmm6
	movhpd	7 * SIZE(X), %xmm7

	andpd	%xmm15, %xmm4
	andpd	%xmm15, %xmm5
	andpd	%xmm15, %xmm6
	andpd	%xmm15, %xmm7

	addpd	%xmm5,  %xmm4
	addpd	%xmm7,  %xmm6
	maxpd	%xmm4, %xmm0
	maxpd	%xmm6, %xmm1

#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE + 64) - PREOFFSET(X)
#endif

	movsd	 8 * SIZE(X), %xmm4
	movsd	 9 * SIZE(X), %xmm5
	movhpd	10 * SIZE(X), %xmm4
	movhpd	11 * SIZE(X), %xmm5
	movsd	12 * SIZE(X), %xmm6
	movsd	13 * SIZE(X), %xmm7
	movhpd	14 * SIZE(X), %xmm6
	movhpd	15 * SIZE(X), %xmm7

	andpd	%xmm15, %xmm4
	andpd	%xmm15, %xmm5
	andpd	%xmm15, %xmm6
	andpd	%xmm15, %xmm7

	addpd	%xmm5,  %xmm4
	addpd	%xmm7,  %xmm6
	maxpd	%xmm4, %xmm2
	maxpd	%xmm6, %xmm3

	addq	$16 * SIZE, X
	decq	I
	jg	.L21
	ALIGN_4

.L25:
	andq	$7,  M
	jle	.L30

	testq	$4, M
	je	.L26

	movsd	0 * SIZE(X), %xmm4
	movsd	1 * SIZE(X), %xmm5
	movhpd	2 * SIZE(X), %xmm4
	movhpd	3 * SIZE(X), %xmm5
	movsd	4 * SIZE(X), %xmm6
	movsd	5 * SIZE(X), %xmm7
	movhpd	6 * SIZE(X), %xmm6
	movhpd	7 * SIZE(X), %xmm7

	andpd	%xmm15, %xmm4
	andpd	%xmm15, %xmm5
	andpd	%xmm15, %xmm6
	andpd	%xmm15, %xmm7

	addpd	%xmm5,  %xmm4
	addpd	%xmm7,  %xmm6
	maxpd	%xmm4, %xmm0
	maxpd	%xmm6, %xmm1

	addq	$8 * SIZE, X
	ALIGN_3

.L26:
	testq	$2, M
	je	.L27

	movsd	0 * SIZE(X), %xmm4
	movsd	1 * SIZE(X), %xmm5
	movhpd	2 * SIZE(X), %xmm4
	movhpd	3 * SIZE(X), %xmm5
	addq	$4 * SIZE, X

	andpd	%xmm15, %xmm4
	andpd	%xmm15, %xmm5
	addpd	%xmm5,  %xmm4
	maxpd	%xmm4, %xmm0
	ALIGN_3	

.L27:
	testq	$1, M
	je	.L30

	movsd	0 * SIZE(X), %xmm4
	movsd	1 * SIZE(X), %xmm5
	andpd	%xmm15, %xmm4
	andpd	%xmm15, %xmm5
	addpd	%xmm5, %xmm4
	maxsd	%xmm4, %xmm2
	ALIGN_4

.L30:
	movq	XX, X
	movq	MM, M

	maxpd	%xmm1, %xmm0
	maxpd	%xmm3, %xmm2
	maxpd	%xmm2, %xmm0
	movapd	%xmm0, %xmm1
	unpckhpd %xmm0, %xmm0
	maxsd	%xmm1, %xmm0
	unpcklpd %xmm0, %xmm0

	movq	M,  I
	sarq	$2, I
	jle	.L35
	ALIGN_4

.L31:
#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	movsd	0 * SIZE(X), %xmm1
	movsd	1 * SIZE(X), %xmm2
	movhpd	2 * SIZE(X), %xmm1
	movhpd	3 * SIZE(X), %xmm2
	movsd	4 * SIZE(X), %xmm3
	movsd	5 * SIZE(X), %xmm4
	movhpd	6 * SIZE(X), %xmm3
	movhpd	7 * SIZE(X), %xmm4

	andpd	%xmm15, %xmm1
	andpd	%xmm15, %xmm2
	andpd	%xmm15, %xmm3
	andpd	%xmm15, %xmm4

	addpd	%xmm2,  %xmm1
	addpd	%xmm4,  %xmm3

	cmpeqpd	%xmm0, %xmm1
	cmpeqpd	%xmm0, %xmm3

	orpd	%xmm3, %xmm1
#ifndef C_SUN
	movmskpd %xmm1, %r11
#else
	.byte	0x66
	.long	0xd9500f4c
#endif
	testq	 $3, %r11
	jne	 .L33

	addq	$8 * SIZE, X
	addq	$4, RET
	decq	I
	jg	.L31
	jmp	.L35
	ALIGN_4

.L33:
	movsd	0 * SIZE(X), %xmm1
	movsd	1 * SIZE(X), %xmm2
	movsd	2 * SIZE(X), %xmm3
	movsd	3 * SIZE(X), %xmm4
	movsd	4 * SIZE(X), %xmm5
	movsd	5 * SIZE(X), %xmm6
	movsd	6 * SIZE(X), %xmm7
	movsd	7 * SIZE(X), %xmm8
	addq	$8 * SIZE, X

	andpd	%xmm15, %xmm1
	andpd	%xmm15, %xmm2
	andpd	%xmm15, %xmm3
	andpd	%xmm15, %xmm4
	andpd	%xmm15, %xmm5
	andpd	%xmm15, %xmm6
	andpd	%xmm15, %xmm7
	andpd	%xmm15, %xmm8

	addpd	%xmm2,  %xmm1
	addpd	%xmm4,  %xmm3
	addpd	%xmm6,  %xmm5
	addpd	%xmm8,  %xmm7

	incq	RET
	comisd	%xmm0, %xmm1
	je	.L999
	incq	RET
	comisd	%xmm0, %xmm3
	je	.L999
	incq	RET
	comisd	%xmm0, %xmm5
	je	.L999
	incq	RET
	comisd	%xmm0, %xmm7
	je	.L999
	ALIGN_3

.L35:
	testq	$2, M
	je	.L36

	movsd	0 * SIZE(X), %xmm1
	movsd	1 * SIZE(X), %xmm2
	movsd	2 * SIZE(X), %xmm3
	movsd	3 * SIZE(X), %xmm4
	addq	$4 * SIZE, X

	andpd	%xmm15, %xmm1
	andpd	%xmm15, %xmm2
	andpd	%xmm15, %xmm3
	andpd	%xmm15, %xmm4

	addpd	%xmm2,  %xmm1
	addpd	%xmm4,  %xmm3
	incq	RET
	comisd	%xmm0, %xmm1
	je	.L999
	incq	RET
	comisd	%xmm0, %xmm3
	je	.L999
	ALIGN_3	

.L36:
	incq	RET
	jmp	.L999
	ALIGN_3

.L60:
	movq	M,  I
	sarq	$3, I
	jle	.L65
	ALIGN_4
	
.L61:
#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	movsd	0 * SIZE(X), %xmm4
	movsd	1 * SIZE(X), %xmm5
	addq	INCX, X
	movhpd	0 * SIZE(X), %xmm4
	movhpd	1 * SIZE(X), %xmm5
	addq	INCX, X
	movsd	0 * SIZE(X), %xmm6
	movsd	1 * SIZE(X), %xmm7
	addq	INCX, X
	movhpd	0 * SIZE(X), %xmm6
	movhpd	1 * SIZE(X), %xmm7
	addq	INCX, X

	andpd	%xmm15, %xmm4
	andpd	%xmm15, %xmm5
	andpd	%xmm15, %xmm6
	andpd	%xmm15, %xmm7
	addpd	%xmm5, %xmm4
	addpd	%xmm7, %xmm6
	maxpd	%xmm4, %xmm0
	maxpd	%xmm6, %xmm1

#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE + 64) - PREOFFSET(X)
#endif

	movsd	0 * SIZE(X), %xmm4
	movsd	1 * SIZE(X), %xmm5
	addq	INCX, X
	movhpd	0 * SIZE(X), %xmm4
	movhpd	1 * SIZE(X), %xmm5
	addq	INCX, X
	movsd	0 * SIZE(X), %xmm6
	movsd	1 * SIZE(X), %xmm7
	addq	INCX, X
	movhpd	0 * SIZE(X), %xmm6
	movhpd	1 * SIZE(X), %xmm7
	addq	INCX, X

	andpd	%xmm15, %xmm4
	andpd	%xmm15, %xmm5
	andpd	%xmm15, %xmm6
	andpd	%xmm15, %xmm7
	addpd	%xmm5, %xmm4
	addpd	%xmm7, %xmm6
	maxpd	%xmm4, %xmm2
	maxpd	%xmm6, %xmm3

	decq	I
	jg	.L61
	ALIGN_4

.L65:
	andq	$7,  M
	jle	.L70

	testq	$4, M
	je	.L66

	movsd	0 * SIZE(X), %xmm4
	movsd	1 * SIZE(X), %xmm5
	addq	INCX, X
	movhpd	0 * SIZE(X), %xmm4
	movhpd	1 * SIZE(X), %xmm5
	addq	INCX, X
	movsd	0 * SIZE(X), %xmm6
	movsd	1 * SIZE(X), %xmm7
	addq	INCX, X
	movhpd	0 * SIZE(X), %xmm6
	movhpd	1 * SIZE(X), %xmm7
	addq	INCX, X

	andpd	%xmm15, %xmm4
	andpd	%xmm15, %xmm5
	andpd	%xmm15, %xmm6
	andpd	%xmm15, %xmm7
	addpd	%xmm5, %xmm4
	addpd	%xmm7, %xmm6
	maxpd	%xmm4, %xmm0
	maxpd	%xmm6, %xmm1
	ALIGN_3

.L66:
	testq	$2, M
	je	.L67

	movsd	0 * SIZE(X), %xmm4
	movsd	1 * SIZE(X), %xmm5
	addq	INCX, X
	movhpd	0 * SIZE(X), %xmm4
	movhpd	1 * SIZE(X), %xmm5
	addq	INCX, X
	andpd	%xmm15, %xmm4
	andpd	%xmm15, %xmm5
	addpd	%xmm5, %xmm4
	maxpd	%xmm4, %xmm2
	ALIGN_3	

.L67:
	testq	$1, M
	je	.L70

	movsd	0 * SIZE(X), %xmm4
	movsd	1 * SIZE(X), %xmm5
	andpd	%xmm15, %xmm4
	andpd	%xmm15, %xmm5
	addpd	%xmm5, %xmm4
	maxsd	%xmm4, %xmm3
	ALIGN_3

.L70:
	movq	XX, X
	movq	MM, M

	maxpd	%xmm1, %xmm0
	maxpd	%xmm3, %xmm2
	maxpd	%xmm2, %xmm0
	movapd	%xmm0, %xmm1
	unpckhpd %xmm0, %xmm0
	maxsd	%xmm1, %xmm0
	unpcklpd %xmm0, %xmm0

	movq	M,  I
	sarq	$2, I
	jle	.L75
	ALIGN_4

.L71:
#if defined(OPTERON) || defined(BARCELONA) || defined(SHANGHAI)
	prefetch	PREFETCHSIZE * SIZE(X)
#endif

#ifdef PENTIUM4
	prefetchnta	PREFETCHSIZE * SIZE(X)
#endif

	movsd	0 * SIZE(X), %xmm1
	movsd	1 * SIZE(X), %xmm2
	addq	INCX, X
	movhpd	0 * SIZE(X), %xmm1
	movhpd	1 * SIZE(X), %xmm2
	addq	INCX, X
	movsd	0 * SIZE(X), %xmm3
	movsd	1 * SIZE(X), %xmm4
	addq	INCX, X
	movhpd	0 * SIZE(X), %xmm3
	movhpd	1 * SIZE(X), %xmm4
	addq	INCX, X

	andpd	%xmm15, %xmm1
	andpd	%xmm15, %xmm2
	andpd	%xmm15, %xmm3
	andpd	%xmm15, %xmm4

	addpd	%xmm2,  %xmm1
	addpd	%xmm4,  %xmm3

	cmpeqpd	%xmm0, %xmm1
	cmpeqpd	%xmm0, %xmm3

	orpd	%xmm3, %xmm1
#ifndef C_SUN
	movmskpd %xmm1, %r11
#else
	.byte	0x66
	.long	0xd9500f4c
#endif
	testq	 $3, %r11
	jne	 .L73

	addq	$4, RET
	decq	I
	jg	.L71
	jmp	.L75
	ALIGN_4

.L73:
	subq	INCX, X
	movsd	0 * SIZE(X), %xmm7
	movsd	1 * SIZE(X), %xmm8
	subq	INCX, X
	movsd	0 * SIZE(X), %xmm5
	movsd	1 * SIZE(X), %xmm6
	subq	INCX, X
	movsd	0 * SIZE(X), %xmm3
	movsd	1 * SIZE(X), %xmm4
	subq	INCX, X
	movsd	0 * SIZE(X), %xmm1
	movsd	1 * SIZE(X), %xmm2

	andpd	%xmm15, %xmm1
	andpd	%xmm15, %xmm2
	andpd	%xmm15, %xmm3
	andpd	%xmm15, %xmm4
	andpd	%xmm15, %xmm5
	andpd	%xmm15, %xmm6
	andpd	%xmm15, %xmm7
	andpd	%xmm15, %xmm8

	addpd	%xmm2,  %xmm1
	addpd	%xmm4,  %xmm3
	addpd	%xmm6,  %xmm5
	addpd	%xmm8,  %xmm7

	incq	RET
	comisd	%xmm0, %xmm1
	je	.L999
	incq	RET
	comisd	%xmm0, %xmm3
	je	.L999
	incq	RET
	comisd	%xmm0, %xmm5
	je	.L999
	incq	RET
	comisd	%xmm0, %xmm7
	je	.L999
	ALIGN_3

.L75:
	testq	$2, M
	je	.L76

	movsd	0 * SIZE(X), %xmm1
	movsd	1 * SIZE(X), %xmm2
	addq	INCX, X
	movsd	0 * SIZE(X), %xmm3
	movsd	1 * SIZE(X), %xmm4
	addq	INCX, X

	andpd	%xmm15, %xmm1
	andpd	%xmm15, %xmm2
	andpd	%xmm15, %xmm3
	andpd	%xmm15, %xmm4

	addpd	%xmm2,  %xmm1
	addpd	%xmm4,  %xmm3
	incq	RET
	comisd	%xmm0, %xmm1
	je	.L999
	incq	RET
	comisd	%xmm0, %xmm3
	je	.L999
	ALIGN_3	

.L76:
	incq	RET
	ALIGN_4

.L999:
	RESTOREREGISTERS

	ret

	EPILOGUE
