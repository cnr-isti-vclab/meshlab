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
#define maxps	minps
#define maxss	minss
#endif
	
#ifndef HAVE_SSE2
#define pxor	xorps
#define movsd	movlps
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

#ifdef USE_ABS
#ifndef HAVE_SSE2
	subl	$8, %esp
	movl	$0x7fffffff, (%esp)
	movss	(%esp), %xmm7
	shufps	$0, %xmm7, %xmm7
	addl	$8, %esp
#else
	cmpeqps	%xmm7, %xmm7
	psrld	$1, %xmm7		/* Generate USE_ABS */
#endif
#endif

	movss	0 * SIZE(XX), %xmm0
	movss	1 * SIZE(XX), %xmm1
	addl	INCX, XX
	decl	MM

#ifdef USE_ABS
	andps	%xmm7, %xmm0
	andps	%xmm7, %xmm1
#endif
	addps	%xmm1,  %xmm0
	shufps	$0, %xmm0, %xmm0
	cmpl	$2 * SIZE, INCX
	jne	.L70

.L30:
	movl	MM,  I
	sarl	$3, I
	jle	.L35
	ALIGN_4
	
.L31:
#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(XX)
#endif

	movsd	 0 * SIZE(XX), %xmm1
	movhps	 2 * SIZE(XX), %xmm1
	movsd	 4 * SIZE(XX), %xmm2
	movhps	 6 * SIZE(XX), %xmm2

	movaps	%xmm1, %xmm3

	shufps	$0x88, %xmm2, %xmm1
	shufps	$0xdd, %xmm2, %xmm3

#ifdef USE_ABS
	andps	%xmm7, %xmm1
	andps	%xmm7, %xmm3
	addps	%xmm3,  %xmm1
	maxps	%xmm1,  %xmm0
#endif

	movsd	 8 * SIZE(XX), %xmm1
	movhps	10 * SIZE(XX), %xmm1
	movsd	12 * SIZE(XX), %xmm2
	movhps	14 * SIZE(XX), %xmm2

	movaps	%xmm1, %xmm3

	shufps	$0x88, %xmm2, %xmm1
	shufps	$0xdd, %xmm2, %xmm3

#ifdef USE_ABS
	andps	%xmm7, %xmm1
	andps	%xmm7, %xmm3
#endif

	addps	%xmm3,  %xmm1
	maxps	%xmm1,  %xmm0

	addl	$16 * SIZE, XX
	decl	I
	jg	.L31
	ALIGN_4

.L35:
	andl	$7,  MM
	jle	.L40

	testl	$4, MM
	je	.L36

	movsd	 0 * SIZE(XX), %xmm1
	movhps	 2 * SIZE(XX), %xmm1
	movsd	 4 * SIZE(XX), %xmm2
	movhps	 6 * SIZE(XX), %xmm2

	movaps	%xmm1, %xmm3

	shufps	$0x88, %xmm2, %xmm1
	shufps	$0xdd, %xmm2, %xmm3

#ifdef USE_ABS
	andps	%xmm7, %xmm1
	andps	%xmm7, %xmm3
#endif
	addps	%xmm3,  %xmm1
	maxps	%xmm1,  %xmm0

	addl	$8 * SIZE, XX
	ALIGN_3

.L36:
	testl	$2, MM
	je	.L37

	movss	0 * SIZE(XX), %xmm1
	movss	1 * SIZE(XX), %xmm2
	movss	2 * SIZE(XX), %xmm3
	movss	3 * SIZE(XX), %xmm4
#ifdef USE_ABS
	andps	%xmm7, %xmm1
	andps	%xmm7, %xmm2
	andps	%xmm7, %xmm3
	andps	%xmm7, %xmm4
#endif
	addps	%xmm2,  %xmm1
	addps	%xmm4,  %xmm3
	maxss	%xmm1,  %xmm0
	maxss	%xmm3,  %xmm0
	addl	$4 * SIZE, XX
	ALIGN_3	

.L37:
	testl	$1, MM
	je	.L40

	movss	0 * SIZE(XX), %xmm1
	movss	1 * SIZE(XX), %xmm2
#ifdef USE_ABS
	andps	%xmm7, %xmm1
	andps	%xmm7, %xmm2
#endif
	addps	%xmm2,  %xmm1
	maxss	%xmm1,  %xmm0
	ALIGN_4

.L40:
	movaps	%xmm0, %xmm1
	movhlps %xmm0, %xmm0
	maxps	%xmm1, %xmm0
	movaps	%xmm0, %xmm1
	shufps	$1, %xmm0, %xmm0
	maxss	%xmm1, %xmm0
	jmp	.L999
	ALIGN_4

.L70:
	movl	MM,  I
	sarl	$3, I
	jle	.L75
	ALIGN_4
	
.L71:
#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(XX)
#endif

	movsd	0 * SIZE(XX), %xmm1
	addl	INCX, XX
	movhps	0 * SIZE(XX), %xmm1
	addl	INCX, XX
	movsd	0 * SIZE(XX), %xmm2
	addl	INCX, XX
	movhps	0 * SIZE(XX), %xmm2
	addl	INCX, XX

	movaps	%xmm1, %xmm3

	shufps	$0x88, %xmm2, %xmm1
	shufps	$0xdd, %xmm2, %xmm3

#ifdef USE_ABS
	andps	%xmm7, %xmm1
	andps	%xmm7, %xmm3
#endif
	addps	%xmm3,  %xmm1
	maxps	%xmm1,  %xmm0

	movsd	0 * SIZE(XX), %xmm1
	addl	INCX, XX
	movhps	0 * SIZE(XX), %xmm1
	addl	INCX, XX
	movsd	0 * SIZE(XX), %xmm2
	addl	INCX, XX
	movhps	0 * SIZE(XX), %xmm2
	addl	INCX, XX

	movaps	%xmm1, %xmm3

	shufps	$0x88, %xmm2, %xmm1
	shufps	$0xdd, %xmm2, %xmm3

#ifdef USE_ABS
	andps	%xmm7, %xmm1
	andps	%xmm7, %xmm3
#endif
	addps	%xmm3,  %xmm1
	maxps	%xmm1,  %xmm0
	decl	I
	jg	.L71
	ALIGN_4

.L75:
	andl	$7,  MM
	jle	.L80

	testl	$4, MM
	je	.L76

	movsd	0 * SIZE(XX), %xmm1
	addl	INCX, XX
	movhps	0 * SIZE(XX), %xmm1
	addl	INCX, XX
	movsd	0 * SIZE(XX), %xmm2
	addl	INCX, XX
	movhps	0 * SIZE(XX), %xmm2
	addl	INCX, XX

	movaps	%xmm1, %xmm3

	shufps	$0x88, %xmm2, %xmm1
	shufps	$0xdd, %xmm2, %xmm3

#ifdef USE_ABS
	andps	%xmm7, %xmm1
	andps	%xmm7, %xmm3
#endif
	addps	%xmm3,  %xmm1
	maxps	%xmm1,  %xmm0
	ALIGN_3	

.L76:
	testl	$2, MM
	je	.L77

	movss	0 * SIZE(XX), %xmm1
	movss	1 * SIZE(XX), %xmm2
	addl	INCX, XX
	movss	0 * SIZE(XX), %xmm3
	movss	1 * SIZE(XX), %xmm4
	addl	INCX, XX

#ifdef USE_ABS
	andps	%xmm7, %xmm1
	andps	%xmm7, %xmm2
	andps	%xmm7, %xmm3
	andps	%xmm7, %xmm4
#endif
	addps	%xmm2,  %xmm1
	addps	%xmm4,  %xmm3
	maxss	%xmm1,  %xmm0
	maxss	%xmm3,  %xmm0
	ALIGN_3
	
.L77:
	testl	$1, MM
	je	.L80

	movss	0 * SIZE(XX), %xmm1
	movss	1 * SIZE(XX), %xmm2
#ifdef USE_ABS
	andps	%xmm7, %xmm1
	andps	%xmm7, %xmm2
#endif
	addps	%xmm2,  %xmm1
	maxss	%xmm1,  %xmm0
	ALIGN_4

.L80:
	movaps	%xmm0, %xmm1
	movhlps %xmm0, %xmm0
	maxps	%xmm1, %xmm0
	movaps	%xmm0, %xmm1
	shufps	$1, %xmm0, %xmm0
	maxss	%xmm1, %xmm0
	ALIGN_4

.L999:
	subl	$8, %esp
	movss	%xmm0, (%esp)
	flds	(%esp)
	addl	$8, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret

	EPILOGUE
