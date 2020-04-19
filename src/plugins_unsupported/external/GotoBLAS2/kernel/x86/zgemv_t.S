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

#ifdef PENTIUM
#define P 88
#endif

#ifndef P
#define P 400
#endif

#define STACK	16
#define ARGS	24
	
#define NLDA	  0 + STACK(%esp)
#define XP	  4 + STACK(%esp)
#define MIN_M	  8 + STACK(%esp)
#define J	 12 + STACK(%esp)
#define IS	 16 + STACK(%esp)

#define M	 4 + STACK + ARGS(%esp)
#define N	 8 + STACK + ARGS(%esp)
#define K	12 + STACK + ARGS(%esp)
#ifdef DOUBLE
#define ALPHA_R	16 + STACK + ARGS(%esp)
#define ALPHA_I	24 + STACK + ARGS(%esp)
#define A	32 + STACK + ARGS(%esp)
#define LDA	36 + STACK + ARGS(%esp)
#define X	40 + STACK + ARGS(%esp)
#define INCX	44 + STACK + ARGS(%esp)
#define Y	48 + STACK + ARGS(%esp)
#define INCY	52 + STACK + ARGS(%esp)
#define BUFFER	56 + STACK + ARGS(%esp)
#else
#define ALPHA_R	16 + STACK + ARGS(%esp)
#define ALPHA_I	20 + STACK + ARGS(%esp)
#define A	24 + STACK + ARGS(%esp)
#define LDA	28 + STACK + ARGS(%esp)
#define X	32 + STACK + ARGS(%esp)
#define INCX	36 + STACK + ARGS(%esp)
#define Y	40 + STACK + ARGS(%esp)
#define INCY	44 + STACK + ARGS(%esp)
#define BUFFER	48 + STACK + ARGS(%esp)
#endif

	PROLOGUE

	subl	$ARGS,	%esp
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx

	PROFCODE

	FLD	ALPHA_I
	FLD	ALPHA_R

	movl	X, %edi			# X

	movl	$0, IS

	movl	M, %ebx
	movl	N, %ecx
	testl	%ebx, %ebx
	jle	.L79

	testl	%ecx, %ecx
	jle	.L79

	movl	INCX, %esi
	addl	%esi, %esi
	leal	(,%esi,SIZE), %esi
	movl	%esi, INCX

	movl	INCY, %esi
	addl	%esi, %esi
	leal	(, %esi, SIZE), %esi
	movl	%esi, INCY

	movl	LDA,  %ebx

	movl	N,    %eax
	imull	%ebx, %eax
	movl	$P,   %esi
	subl	%eax, %esi
	leal	(, %esi, SIZE), %esi
	addl	%esi, %esi
	movl	%esi, NLDA

	leal	(,%ebx,SIZE), %esi
	addl	%esi, %esi
	movl	%esi, LDA
	ALIGN_2

.L32:
	movl	IS,   %esi

	movl	$P,   %edx
	movl	M,    %eax
	subl	%esi, %eax
	cmpl	%edx, %eax
#ifdef PENTIUM
	jle	.L33
	movl	%edx, %eax
.L33:
#else
	cmovg	%edx, %eax
#endif
	movl	%eax, MIN_M

	movl	IS,   %ecx
	addl	%ecx, %ecx
	leal	(%edi,%ecx,SIZE), %ecx		# xp = x + is
	movl	INCX, %ebx
	movl	%ecx, XP
	cmpl	$2 * SIZE, %ebx
	je	.L34

	movl	BUFFER, %esi
	movl	MIN_M, %eax
	movl	%esi, XP
	sarl	$1, %eax
	jle	.L35

	ALIGN_3

.L36:
	FLD	0 * SIZE(%edi)
	FLD	1 * SIZE(%edi)
	addl	%ebx,%edi		# x += incx
	FLD	0 * SIZE(%edi)
	FLD	1 * SIZE(%edi)
	addl	%ebx,%edi		# x += incx

	FST	3 * SIZE(%esi)
	FST	2 * SIZE(%esi)
	FST	1 * SIZE(%esi)
	FST	0 * SIZE(%esi)

	addl	$4 * SIZE, %esi		# xp += 4
	decl	%eax
	jg	.L36
	ALIGN_3

.L35:
	movl	MIN_M, %eax
	andl	$1,%eax
	jle	.L34

	FLD	0 * SIZE(%edi)
	FLD	1 * SIZE(%edi)
	addl	%ebx,%edi		# x += incx
	FST	1 * SIZE(%esi)
	FST	0 * SIZE(%esi)
	ALIGN_3

/* Main Routine */

.L34:
	movl	Y, %ebp			# coffset = y

	movl	N, %ecx
	testl	%ecx, %ecx
	jle	.L60
	ALIGN_2

.L61:
	movl	A, %ebx			# a_offset = a
	fldz				# ct1 = ZERO
	movl	LDA, %edx
	fldz				# ct1 = ZERO

	addl	%ebx, %edx
	fldz				# ct1 = ZERO
	movl	%edx, A
	fldz				# ct1 = ZERO

	movl	XP, %esi

	FLD	(%esi)			#  bt1 = *(b_offset + 0)

	movl	MIN_M, %eax
	sarl	$1,    %eax
	jle	.L64
	ALIGN_3

#define PRESIZE 8

.L65:
#ifdef HAS_PREFETCH
       prefetcht0	PRESIZE * SIZE(%ebx)
       prefetcht0	PRESIZE * SIZE(%esi)
#endif

	FLD	 0 * SIZE(%ebx)		# at1  = *(a_offset + 0)
	fmul	%st(1), %st			# at1 *= bt1
	faddp	%st, %st(2)		# ct1 += at1

	FMUL	 1 * SIZE(%ebx)		# bt1 *= *(a_offset + 1)
#ifndef CONJ
	faddp	%st, %st(2)		# ct2 += bt1
#else
	fsubrp	%st, %st(2)		# ct2 -= bt1
#endif
	FLD	 1 * SIZE(%esi)		# bt1  = *(b_offset + 1)

	FLD	 0 * SIZE(%ebx)		# at1  = *(a_offset + 0)
	fmul	%st(1), %st		# at1 *= bt1
	faddp	%st, %st(4)		# ct3 += at1

	FMUL	 1 * SIZE(%ebx)		# bt1 *= *(a_offset + 1)
	faddp	%st, %st(4)		# ct4 += bt1
	FLD	 2 * SIZE(%esi)		# bt1  = *(b_offset + 1)

	FLD	 2 * SIZE(%ebx)		# at1  = *(a_offset + 0)
	fmul	%st(1), %st		# at1 *= bt1
	faddp	%st, %st(2)		# ct1 += at1

	FMUL	 3 * SIZE(%ebx)		# bt1 *= *(a_offset + 1)
#ifndef CONJ
	faddp	%st, %st(2)		# ct2 += bt1
#else
	fsubrp	%st, %st(2)		# ct2 -= bt1
#endif
	FLD	 3 * SIZE(%esi)		# bt1  = *(b_offset + 1)

	FLD	 2 * SIZE(%ebx)		# at1  = *(a_offset + 0)
	fmul	%st(1), %st		# at1 *= bt1
	faddp	%st, %st(4)		# ct3 += at1

	FMUL	 3 * SIZE(%ebx)		# bt1 *= *(a_offset + 1)
	faddp	%st, %st(4)		# ct4 += bt1
	FLD	 4 * SIZE(%esi)		# bt1  = *(b_offset + 1)

	addl	$4 * SIZE, %esi
	addl	$4 * SIZE, %ebx
	decl	%eax
	jg	.L65
	ALIGN_3

.L64:
	movl	MIN_M, %eax
	andl	$1, %eax
	jle	.L70
	ALIGN_3

.L71:
	FLD	 0 * SIZE(%ebx)		# at1  = *(a_offset + 0)
	fmul	%st(1), %st		# at1 *= bt1
	faddp	%st, %st(2)		# ct1 += at1

	FMUL	 1 * SIZE(%ebx)		# bt1 *= *(a_offset + 1)
#ifndef CONJ
	faddp	%st, %st(2)		# ct2 += bt1
#else
	fsubrp	%st, %st(2)		# ct2 -= bt1
#endif
	FLD	 1 * SIZE(%esi)		# bt1  = *(b_offset + 1)

	FLD	 0 * SIZE(%ebx)		# at1  = *(a_offset + 0)
	fmul	%st(1), %st		# at1 *= bt1
	faddp	%st, %st(4)		# ct3 += at1

	FMUL	 1 * SIZE(%ebx)		# bt1 *= *(a_offset + 1)
	faddp	%st, %st(4)		# ct4 += bt1
	fldz
	ALIGN_3

.L70:
#ifndef C_SUN
	ffreep	%st(0)
#else
	.byte	0xdf
	.byte	0xc0
#endif

#ifndef XCONJ
#ifndef CONJ
	fsubp	%st, %st(3)
	faddp	%st, %st(1)
#else
	faddp	%st, %st(3)
	faddp	%st, %st(1)
#endif
#else
#ifndef CONJ
	faddp	%st, %st(3)
	fsubp	%st, %st(1)
#else
	fsubp	%st, %st(3)
	fsubp	%st, %st(1)
#endif
#endif

	fld	%st(0)		# ct4 = ct2
	fmul	%st(4), %st
	fld	%st(2)
	fmul	%st(4), %st
	fsubp	%st, %st(1)

	FADD	0 * SIZE(%ebp)
	FST	0 * SIZE(%ebp)

	fmul	%st(2), %st
	fxch	%st(1)
	fmul	%st(3), %st
	faddp	%st, %st(1)

	FADD	1 * SIZE(%ebp)
	FST	1 * SIZE(%ebp)
	addl	INCY, %ebp

	decl	%ecx
	jg	.L61
	ALIGN_3

.L60:
	movl	A, %ebx
	addl	NLDA, %ebx
	movl	%ebx, A

	addl	$P,  IS
	movl	M, %esi
	cmpl	%esi, IS
	jl	.L32
	ALIGN_3

.L79:
#ifndef C_SUN
	ffreep	%st(0)
	ffreep	%st(0)
#else
	.byte	0xdf
	.byte	0xc0
	.byte	0xdf
	.byte	0xc0
#endif

	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	addl	$ARGS, %esp
	ret

	EPILOGUE

