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
#include "l2param.h"

#define STACKSIZE	80
#define P		4096
	
#define ALPHA_R		 8 + STACKSIZE(%rsp)
#define ALPHA_I		24 + STACKSIZE(%rsp)
#define OLD_INCX	40 + STACKSIZE(%rsp)
#define OLD_Y		48 + STACKSIZE(%rsp)
#define OLD_INCY	56 + STACKSIZE(%rsp)
#define BUFFER		64 + STACKSIZE(%rsp)

#define NLDA	        56            (%rsp)
#define IS		64	      (%rsp)

#define M	  %rdi
#define N	  %rsi
#define A	  %rcx
#define LDA	  %r8
#define X	  %r9
#define INCX	  %rdx
#define Y	  %rbp
#define INCY	  %r10

#define TEMP	%rax
#define I	%rax
#define J	%r11
#define A1	%r12
#define XP	%r15
#define X1	%r13
#define Y1	%r14
#define MIN_M	%rbx
	

	PROLOGUE
	PROFCODE

	subq	$STACKSIZE, %rsp
	movq	%rbx,  0(%rsp)
	movq	%rbp,  8(%rsp)
	movq	%r12, 16(%rsp)
	movq	%r13, 24(%rsp)
	movq	%r14, 32(%rsp)
	movq	%r15, 40(%rsp)

	movq	OLD_INCX,   INCX
	movq	OLD_Y,      Y
	movq	OLD_INCY,   INCY

	FLD	ALPHA_I
	FLD	ALPHA_R

	salq	$ZBASE_SHIFT, INCX
	salq	$ZBASE_SHIFT, INCY

	movq	$0, IS

	test	M, M
	jle	.L79			# goto END
	test	N, N
	jle	.L79			# goto END

	movq	N,    %rax
	imulq	LDA,  %rax
	movq	$P,   NLDA
	subq	%rax, NLDA
	salq	$ZBASE_SHIFT, NLDA

	salq	$ZBASE_SHIFT, LDA
	ALIGN_2

.L32:
	movq	$P,   %rax
	movq	M,    MIN_M
	subq	IS  , MIN_M
	cmpq	%rax, MIN_M
	cmovg	%rax, MIN_M

	movq	IS, X1
	salq	$ZBASE_SHIFT, X1
	leaq	(X,X1, 1), X1

	movq	X1, XP

	cmpq	$2 * SIZE, INCX
	je	.L34

	movq	BUFFER, X1
	movq	X1, XP

	movq	MIN_M, I
	sarq	$1, I
	jle	.L35
	ALIGN_3

.L36:
	FLD	0 * SIZE(X)
	FLD	1 * SIZE(X)
	addq	INCX,X		# x += incx
	FLD	0 * SIZE(X)
	FLD	1 * SIZE(X)
	addq	INCX,X		# x += incx

	FST	3 * SIZE(X1)
	FST	2 * SIZE(X1)
	FST	1 * SIZE(X1)
	FST	0 * SIZE(X1)

	addq	$4 * SIZE, X1		# xp += 4
	decq	I
	jg	.L36
	ALIGN_3

.L35:
	movq	MIN_M, I
	andq	$1,I
	jle	.L34

	FLD	0 * SIZE(X)
	FLD	1 * SIZE(X)
	addq	INCX,X		# x += incx
	FST	1 * SIZE(X1)
	FST	0 * SIZE(X1)
	ALIGN_3

/* Main Routine */

.L34:
	movq	Y, Y1			# coffset = y

	movq	N, J
	ALIGN_2

.L61:
	movq	A, A1			# a_offset = a
	fldz				# ct1 = ZERO
	fldz				# ct1 = ZERO

	addq	LDA, A
	fldz				# ct1 = ZERO
	fldz				# ct1 = ZERO

	movq	XP, X1

	FLD	(X1)			#  bt1 = *(b_offset + 0)

	movq	MIN_M, I
	sarq	$1,    I
	jle	.L64
	ALIGN_3

.L65:
	FLD	 0 * SIZE(A1)		# at1  = *(a_offset + 0)
	fmul	%st(1)			# at1 *= bt1
	faddp	%st, %st(2)		# ct1 += at1

	FLD	 1 * SIZE(A1)		# bt1 *= *(a_offset + 1)
	fmulp	 %st, %st(1)
#ifndef CONJ
	faddp	%st, %st(2)		# ct2 += bt1
#else
	fsubrp	%st, %st(2)		# ct2 -= bt1
#endif
	FLD	 1 * SIZE(X1)		# bt1  = *(b_offset + 1)

	FLD	 0 * SIZE(A1)		# at1  = *(a_offset + 0)
	fmul	%st(1)			# at1 *= bt1
	faddp	%st, %st(4)		# ct3 += at1

	FLD	 1 * SIZE(A1)		# bt1 *= *(a_offset + 1)
	fmulp	 %st, %st(1)
	faddp	%st, %st(4)		# ct4 += bt1
	FLD	 2 * SIZE(X1)		# bt1  = *(b_offset + 1)

	FLD	 2 * SIZE(A1)		# at1  = *(a_offset + 0)
	fmul	%st(1)			# at1 *= bt1
	faddp	%st, %st(2)		# ct1 += at1

	FLD	 3 * SIZE(A1)		# bt1 *= *(a_offset + 1)
	fmulp	 %st, %st(1)
#ifndef CONJ
	faddp	%st, %st(2)		# ct2 += bt1
#else
	fsubrp	%st, %st(2)		# ct2 -= bt1
#endif
	FLD	 3 * SIZE(X1)		# bt1  = *(b_offset + 1)

	FLD	 2 * SIZE(A1)		# at1  = *(a_offset + 0)
	fmul	%st(1)			# at1 *= bt1
	faddp	%st, %st(4)		# ct3 += at1

	FLD	 3 * SIZE(A1)		# bt1 *= *(a_offset + 1)
	fmulp	 %st, %st(1)
	faddp	%st, %st(4)		# ct4 += bt1
	FLD	 4 * SIZE(X1)		# bt1  = *(b_offset + 1)

	addq	$4 * SIZE, X1
	addq	$4 * SIZE, A1
	decq	I
	jg	.L65
	ALIGN_3

.L64:
	movq	MIN_M, I
	andq	$1, I
	jle	.L70
	ALIGN_3

.L71:
	FLD	 0 * SIZE(A1)		# at1  = *(a_offset + 0)
	fmul	%st(1)			# at1 *= bt1
	faddp	%st, %st(2)		# ct1 += at1

	FLD	 1 * SIZE(A1)		# bt1 *= *(a_offset + 1)
	fmulp	 %st, %st(1)
#ifndef CONJ
	faddp	%st, %st(2)		# ct2 += bt1
#else
	fsubrp	%st, %st(2)		# ct2 -= bt1
#endif
	FLD	 1 * SIZE(X1)		# bt1  = *(b_offset + 1)

	FLD	 0 * SIZE(A1)		# at1  = *(a_offset + 0)
	fmul	%st(1)			# at1 *= bt1
	faddp	%st, %st(4)		# ct3 += at1

	FLD	 1 * SIZE(A1)		# bt1 *= *(a_offset + 1)
	fmulp	 %st, %st(1)
	faddp	%st, %st(4)		# ct4 += bt1
	fldz
	ALIGN_3

.L70:
	ffreep	%st(0)

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
	fmul	%st(4)
	fld	%st(2)
	fmul	%st(4)
	fsubp	%st, %st(1)

	FLD	0 * SIZE(Y1)
	faddp	 %st, %st(1)
	FST	0 * SIZE(Y1)

	fmul	%st(2)
	fxch	%st(1)
	fmul	%st(3)
	faddp	%st, %st(1)

	FLD	1 * SIZE(Y1)
	faddp	 %st, %st(1)
	FST	1 * SIZE(Y1)
	addq	INCY, Y1

	decq	J
	jg	.L61
	ALIGN_3

.L60:
	addq	NLDA, A

	addq	$P, IS
	cmpq	M,  IS
	jl	.L32
	ALIGN_3

.L79:
	ffreep	%st
	ffreep	%st

	movq	  0(%rsp), %rbx
	movq	  8(%rsp), %rbp
	movq	 16(%rsp), %r12
	movq	 24(%rsp), %r13
	movq	 32(%rsp), %r14
	movq	 40(%rsp), %r15
	addq	$STACKSIZE, %rsp
	ret
	EPILOGUE
