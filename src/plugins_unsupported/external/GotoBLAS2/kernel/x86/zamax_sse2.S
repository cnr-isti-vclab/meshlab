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
	
#define STACK	16
#define ARGS	 0
	
#define STACK_M		 4 + STACK + ARGS(%esp)
#define STACK_X		 8 + STACK + ARGS(%esp)
#define STACK_INCX	12 + STACK + ARGS(%esp)

#define RET	%eax
#define	M	%ebx
#define X	%ecx
#define INCX	%edx
#define I	%esi
#define MM	%ebp
#define XX	%edi
#define TEMP	%ebx
	
#ifdef USE_MIN
#define maxpd	minpd
#define maxsd	minsd
#endif

#include "l1param.h"

	PROLOGUE

	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx

	PROFCODE

	movl	STACK_M, M
	movl	STACK_X, X
	movl	STACK_INCX, INCX

	pxor	%xmm0, %xmm0
	pxor	%xmm7, %xmm7
	xor	RET, RET
	testl	M, M
	jle	.L999
	testl	INCX, INCX
	jle	.L999

	sall	$ZBASE_SHIFT, INCX
	movl	M, MM
	movl	X, XX

	cmpeqpd	%xmm7, %xmm7
	psrlq	$1, %xmm7

	movsd	0 * SIZE(XX), %xmm0
	movsd	1 * SIZE(XX), %xmm1
	addl	INCX, XX
	decl	MM
	andpd	 %xmm7, %xmm0
	andpd	 %xmm7, %xmm1
	addpd	 %xmm1, %xmm0
	unpcklpd %xmm0, %xmm0
	cmpl	$2 * SIZE, INCX
	jne	.L60

	movl	MM,  I
	sarl	$3, I
	jle	.L25
	ALIGN_4

.L21:
#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(XX)
#endif

	movsd	 0 * SIZE(XX), %xmm1
	movsd	 1 * SIZE(XX), %xmm2
	movhpd	 2 * SIZE(XX), %xmm1
	movhpd	 3 * SIZE(XX), %xmm2

	andpd	%xmm7, %xmm1
	andpd	%xmm7, %xmm2
	addpd	%xmm2, %xmm1
	maxpd	%xmm1, %xmm0

	movsd	 4 * SIZE(XX), %xmm3
	movsd	 5 * SIZE(XX), %xmm4
	movhpd	 6 * SIZE(XX), %xmm3
	movhpd	 7 * SIZE(XX), %xmm4

	andpd	%xmm7, %xmm3
	andpd	%xmm7, %xmm4
	addpd	%xmm4, %xmm3
	maxpd	%xmm3, %xmm0

#if defined(PREFETCH) && !defined(FETCH128)
	PREFETCH (PREFETCHSIZE +  64) - PREOFFSET(XX)
#endif

	movsd	 8 * SIZE(XX), %xmm1
	movsd	 9 * SIZE(XX), %xmm2
	movhpd	10 * SIZE(XX), %xmm1
	movhpd	11 * SIZE(XX), %xmm2

	andpd	%xmm7, %xmm1
	andpd	%xmm7, %xmm2
	addpd	%xmm2, %xmm1
	maxpd	%xmm1, %xmm0

	movsd	12 * SIZE(XX), %xmm3
	movsd	13 * SIZE(XX), %xmm4
	movhpd	14 * SIZE(XX), %xmm3
	movhpd	15 * SIZE(XX), %xmm4

	andpd	%xmm7, %xmm3
	andpd	%xmm7, %xmm4
	addpd	%xmm4, %xmm3
	maxpd	%xmm3, %xmm0

	addl	$16 * SIZE, XX
	decl	I
	jg	.L21
	ALIGN_4

.L25:
	andl	$7,  MM
	jle	.L30

	testl	$4, MM
	je	.L26

	movsd	 0 * SIZE(XX), %xmm1
	movsd	 1 * SIZE(XX), %xmm2
	movhpd	 2 * SIZE(XX), %xmm1
	movhpd	 3 * SIZE(XX), %xmm2

	andpd	%xmm7, %xmm1
	andpd	%xmm7, %xmm2
	addpd	%xmm2, %xmm1
	maxpd	%xmm1, %xmm0

	movsd	 4 * SIZE(XX), %xmm3
	movsd	 5 * SIZE(XX), %xmm4
	movhpd	 6 * SIZE(XX), %xmm3
	movhpd	 7 * SIZE(XX), %xmm4

	andpd	%xmm7, %xmm3
	andpd	%xmm7, %xmm4
	addpd	%xmm4, %xmm3
	maxpd	%xmm3, %xmm0
	addl	$8 * SIZE, XX
	ALIGN_3

.L26:
	testl	$2, MM
	je	.L27

	movsd	 0 * SIZE(XX), %xmm1
	movsd	 1 * SIZE(XX), %xmm2
	movhpd	 2 * SIZE(XX), %xmm1
	movhpd	 3 * SIZE(XX), %xmm2

	andpd	%xmm7, %xmm1
	andpd	%xmm7, %xmm2
	addpd	%xmm2, %xmm1
	maxpd	%xmm1, %xmm0

	addl	$4 * SIZE, XX
	ALIGN_3	

.L27:
	testl	$1, MM
	je	.L30

	movsd	0 * SIZE(XX), %xmm1
	movsd	1 * SIZE(XX), %xmm2
	andpd	%xmm7, %xmm1
	andpd	%xmm7, %xmm2
	addpd	%xmm2, %xmm1
	maxsd	%xmm1, %xmm0
	ALIGN_4

.L30:
	movapd	 %xmm0, %xmm1
	unpckhpd %xmm0, %xmm0
	maxsd	 %xmm1, %xmm0
	jmp	.L999
	ALIGN_3

.L60:
	movl	MM,  I
	sarl	$3, I
	jle	.L65
	ALIGN_4
	
.L61:
#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(XX)
#endif

	movsd	0 * SIZE(XX), %xmm1
	movsd	1 * SIZE(XX), %xmm2
	addl	INCX, XX
	movhpd	0 * SIZE(XX), %xmm1
	movhpd	1 * SIZE(XX), %xmm2
	addl	INCX, XX

	andpd	%xmm7, %xmm1
	andpd	%xmm7, %xmm2
	addpd	%xmm2, %xmm1
	maxpd	%xmm1, %xmm0

	movsd	0 * SIZE(XX), %xmm3
	movsd	1 * SIZE(XX), %xmm4
	addl	INCX, XX
	movhpd	0 * SIZE(XX), %xmm3
	movhpd	1 * SIZE(XX), %xmm4
	addl	INCX, XX

	andpd	%xmm7, %xmm3
	andpd	%xmm7, %xmm4
	addpd	%xmm4, %xmm3
	maxpd	%xmm3, %xmm0

#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +   0) - PREOFFSET(XX)
#endif

	movsd	0 * SIZE(XX), %xmm1
	movsd	1 * SIZE(XX), %xmm2
	addl	INCX, XX
	movhpd	0 * SIZE(XX), %xmm1
	movhpd	1 * SIZE(XX), %xmm2
	addl	INCX, XX

	andpd	%xmm7, %xmm1
	andpd	%xmm7, %xmm2
	addpd	%xmm2, %xmm1
	maxpd	%xmm1, %xmm0

	movsd	0 * SIZE(XX), %xmm3
	movsd	1 * SIZE(XX), %xmm4
	addl	INCX, XX
	movhpd	0 * SIZE(XX), %xmm3
	movhpd	1 * SIZE(XX), %xmm4
	addl	INCX, XX

	andpd	%xmm7, %xmm3
	andpd	%xmm7, %xmm4
	addpd	%xmm4, %xmm3
	maxpd	%xmm3, %xmm0

	decl	I
	jg	.L61
	ALIGN_4

.L65:
	andl	$7,  MM
	jle	.L70

	testl	$4, MM
	je	.L66

	movsd	0 * SIZE(XX), %xmm1
	movsd	1 * SIZE(XX), %xmm2
	addl	INCX, XX
	movhpd	0 * SIZE(XX), %xmm1
	movhpd	1 * SIZE(XX), %xmm2
	addl	INCX, XX

	andpd	%xmm7, %xmm1
	andpd	%xmm7, %xmm2
	addpd	%xmm2, %xmm1
	maxpd	%xmm1, %xmm0

	movsd	0 * SIZE(XX), %xmm3
	movsd	1 * SIZE(XX), %xmm4
	addl	INCX, XX
	movhpd	0 * SIZE(XX), %xmm3
	movhpd	1 * SIZE(XX), %xmm4
	addl	INCX, XX

	andpd	%xmm7, %xmm3
	andpd	%xmm7, %xmm4
	addpd	%xmm4, %xmm3
	maxpd	%xmm3, %xmm0
	ALIGN_3

.L66:
	testl	$2, MM
	je	.L67

	movsd	0 * SIZE(XX), %xmm1
	movsd	1 * SIZE(XX), %xmm2
	addl	INCX, XX
	movhpd	0 * SIZE(XX), %xmm1
	movhpd	1 * SIZE(XX), %xmm2
	addl	INCX, XX

	andpd	%xmm7, %xmm1
	andpd	%xmm7, %xmm2
	addpd	%xmm2, %xmm1
	maxpd	%xmm1, %xmm0
	ALIGN_3	

.L67:
	testl	$1, MM
	je	.L70

	movsd	0 * SIZE(XX), %xmm1
	movsd	1 * SIZE(XX), %xmm2
	andpd	%xmm7, %xmm1
	andpd	%xmm7, %xmm2
	addpd	%xmm2, %xmm1
	maxsd	%xmm1, %xmm0
	ALIGN_3

.L70:
	movapd	%xmm0, %xmm1
	unpckhpd %xmm0, %xmm0
	maxsd	%xmm1, %xmm0
	ALIGN_4

.L999:
	subl	$8, %esp
	movsd	%xmm0, (%esp)
	fldl	(%esp)
	addl	$8, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret

	EPILOGUE
