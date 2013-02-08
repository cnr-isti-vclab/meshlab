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

#define N	ARG1	/* rdi */
#define X	ARG2	/* rsi */
#define INCX	ARG3	/* rdx */
#define Y	ARG4	/* rcx */
#ifndef WINDOWS_ABI
#define INCY	ARG5	/* r8  */
#define FLAG	ARG6
#else
#define INCY	%r10
#define FLAG	%r11
#endif
		
#include "l1param.h"

	PROLOGUE
	PROFCODE

#ifdef WINDOWS_ABI
	movq	40(%rsp), INCY
#endif

	EMMS

	salq	$ZBASE_SHIFT, INCX
	salq	$ZBASE_SHIFT, INCY

	testq	N, N	# if m == 0 goto End
	jle	.L999

	cmpq	$2 * SIZE, INCX	# if incx != 1
	jne	.L100
	cmpq	$2 * SIZE, INCY	# if incy != 1
	jne	.L100

	movq	N, %rax	# i = m
	sarq	$2,   %rax
	jle	.L20
	ALIGN_2

.L11:
#ifdef XDOUBLE

#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	movq	  0(X), %mm0
	movq	%mm0,   0(Y)

	movq	  8(X), %mm1
	movq	%mm1,   8(Y)

	movq	 16(X), %mm2
	movq	%mm2,  16(Y)

	movq	 24(X), %mm3
	movq	%mm3,  24(Y)

#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(Y)
#endif

	movq	 32(X), %mm4
	movq	%mm4,  32(Y)

	movq	 40(X), %mm5
	movq	%mm5,  40(Y)

	movq	 48(X), %mm6
	movq	%mm6,  48(Y)

	movq	 56(X), %mm7
	movq	%mm7,  56(Y)

#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE + 64) - PREOFFSET(X)
#endif

	movq	 64(X), %mm0
	movq	%mm0,  64(Y)

	movq	 72(X), %mm1
	movq	%mm1,  72(Y)

	movq	 80(X), %mm2
	movq	%mm2,  80(Y)

	movq	 88(X), %mm3
	movq	%mm3,  88(Y)

#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE + 64) - PREOFFSET(Y)
#endif

	movq	 96(X), %mm4
	movq	%mm4,  96(Y)

	movq	104(X), %mm5
	movq	%mm5, 104(Y)

	movq	112(X), %mm6
	movq	%mm6, 112(Y)

	movq	120(X), %mm7
	movq	%mm7, 120(Y)
#elif defined(DOUBLE)

#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	movq	 0 * SIZE(X), %mm0
	movq	 1 * SIZE(X), %mm1

	movq	%mm0,  0 * SIZE(Y)
	movq	%mm1,  1 * SIZE(Y)

	movq	 2 * SIZE(X), %mm2
	movq	 3 * SIZE(X), %mm3

	movq	%mm2,  2 * SIZE(Y)
	movq	%mm3,  3 * SIZE(Y)

	movq	 4 * SIZE(X), %mm4
	movq	 5 * SIZE(X), %mm5

#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(Y)
#endif

	movq	%mm4,  4 * SIZE(Y)
	movq	%mm5,  5 * SIZE(Y)

	movq	 6 * SIZE(X), %mm6
	movq	 7 * SIZE(X), %mm7

	movq	%mm6,  6 * SIZE(Y)
	movq	%mm7,  7 * SIZE(Y)
#else
	movq	 0 * SIZE(X), %mm0
	movq	 2 * SIZE(X), %mm2

#ifdef PREFETCH
	PREFETCH (PREFETCHSIZE +  0) - PREOFFSET(X)
#endif

	movq	%mm0,  0 * SIZE(Y)
	movq	%mm2,  2 * SIZE(Y)

	movq	 4 * SIZE(X), %mm4
	movq	 6 * SIZE(X), %mm6

#ifdef PREFETCHW
	PREFETCHW (PREFETCHSIZE +  0) - PREOFFSET(Y)
#endif

	movq	%mm4,  4 * SIZE(Y)
	movq	%mm6,  6 * SIZE(Y)
#endif
	addq	$8 * SIZE, X
	addq	$8 * SIZE, Y
	decq	%rax
	jg	.L11
	ALIGN_2

.L20:
	movq	N, %rax	# i = m
	andq	$3,   %rax
	jle	.L99
	ALIGN_2

.L21:
#ifdef XDOUBLE
	movq	  0(X), %mm0
	movq	%mm0,   0(Y)
	movq	  8(X), %mm1
	movq	%mm1,   8(Y)
	movq	 16(X), %mm2
	movq	%mm2,  16(Y)
	movq	 24(X), %mm3
	movq	%mm3,  24(Y)
#elif defined(DOUBLE)
	movq	 0 * SIZE(X), %mm0
	movq	%mm0,  0 * SIZE(Y)
	movq	 1 * SIZE(X), %mm1
	movq	%mm1,  1 * SIZE(Y)
#else
	movq	 0 * SIZE(X), %mm0
	movq	%mm0,  0 * SIZE(Y)
#endif

	addq	$2 * SIZE, X
	addq	$2 * SIZE, Y
	decq	%rax
	jg	.L21

.L99:
	xorq	%rax,%rax
	EMMS
	ret
	ALIGN_3

.L100:
	movq	N, %rax
	sarq	$2,   %rax
	jle	.L120
	ALIGN_2

.L111:
#ifdef XDOUBLE
	movq	  0(X), %mm0
	movq	%mm0,   0(Y)
	movq	  8(X), %mm1
	movq	%mm1,   8(Y)
	movq	 16(X), %mm2
	movq	%mm2,  16(Y)
	movq	 24(X), %mm3
	movq	%mm3,  24(Y)
	addq	INCX, X
	addq	INCY, Y

	movq	  0(X), %mm0
	movq	%mm0,   0(Y)
	movq	  8(X), %mm1
	movq	%mm1,   8(Y)
	movq	 16(X), %mm2
	movq	%mm2,  16(Y)
	movq	 24(X), %mm3
	movq	%mm3,  24(Y)
	addq	INCX, X
	addq	INCY, Y

	movq	  0(X), %mm0
	movq	%mm0,   0(Y)
	movq	  8(X), %mm1
	movq	%mm1,   8(Y)
	movq	 16(X), %mm2
	movq	%mm2,  16(Y)
	movq	 24(X), %mm3
	movq	%mm3,  24(Y)
	addq	INCX, X
	addq	INCY, Y

	movq	  0(X), %mm0
	movq	%mm0,   0(Y)
	movq	  8(X), %mm1
	movq	%mm1,   8(Y)
	movq	 16(X), %mm2
	movq	%mm2,  16(Y)
	movq	 24(X), %mm3
	movq	%mm3,  24(Y)
	addq	INCX, X
	addq	INCY, Y
#elif defined(DOUBLE)
	movq	 0 * SIZE(X), %mm0
	movq	%mm0,  0 * SIZE(Y)
	movq	 1 * SIZE(X), %mm1
	movq	%mm1,  1 * SIZE(Y)
	addq	INCX, X
	addq	INCY, Y

	movq	 0 * SIZE(X), %mm2
	movq	%mm2,  0 * SIZE(Y)
	movq	 1 * SIZE(X), %mm3
	movq	%mm3,  1 * SIZE(Y)
	addq	INCX, X
	addq	INCY, Y

	movq	 0 * SIZE(X), %mm4
	movq	%mm4,  0 * SIZE(Y)
	movq	 1 * SIZE(X), %mm5
	movq	%mm5,  1 * SIZE(Y)
	addq	INCX, X
	addq	INCY, Y

	movq	 0 * SIZE(X), %mm6
	movq	%mm6,  0 * SIZE(Y)
	movq	 1 * SIZE(X), %mm7
	movq	%mm7,  1 * SIZE(Y)
	addq	INCX, X
	addq	INCY, Y
#else
	movq	 0 * SIZE(X), %mm0
	movq	%mm0,  0 * SIZE(Y)
	addq	INCX, X
	addq	INCY, Y

	movq	 0 * SIZE(X), %mm2
	movq	%mm2,  0 * SIZE(Y)
	addq	INCX, X
	addq	INCY, Y

	movq	 0 * SIZE(X), %mm4
	movq	%mm4,  0 * SIZE(Y)
	addq	INCX, X
	addq	INCY, Y

	movq	 0 * SIZE(X), %mm6
	movq	%mm6,  0 * SIZE(Y)
	addq	INCX, X
	addq	INCY, Y
#endif

	decq	%rax
	jg	.L111

.L120:
	movq	N, %rax
	andq	$3,   %rax
	jle	.L999
	ALIGN_2

.L121:
#ifdef XDOUBLE
	movq	  0(X), %mm0
	movq	%mm0,   0(Y)
	movq	  8(X), %mm1
	movq	%mm1,   8(Y)
	movq	 16(X), %mm2
	movq	%mm2,  16(Y)
	movq	 24(X), %mm3
	movq	%mm3,  24(Y)
	addq	INCX, X
	addq	INCY, Y
#elif defined(DOUBLE)
	movq	 0 * SIZE(X), %mm0
	movq	%mm0,  0 * SIZE(Y)
	movq	 1 * SIZE(X), %mm1
	movq	%mm1,  1 * SIZE(Y)
	addq	INCX, X
	addq	INCY, Y
#else
	movq	 0 * SIZE(X), %mm0
	movq	%mm0,  0 * SIZE(Y)
	addq	INCX, X
	addq	INCY, Y
#endif

	decq	%rax
	jg	.L121

.L999:
	xorq	%rax,%rax
	EMMS
	ret

	EPILOGUE

