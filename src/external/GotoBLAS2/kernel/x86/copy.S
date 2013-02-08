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
#define ARGS	 0
	
#define M	 4 + STACK + ARGS(%esp)
#define X	 8 + STACK + ARGS(%esp)
#define INCX	12 + STACK + ARGS(%esp)
#define Y	16 + STACK + ARGS(%esp)
#define INCY	20 + STACK + ARGS(%esp)
	
	PROLOGUE

	pushl	%edi
	pushl	%esi
	pushl	%ebx

	PROFCODE

#if defined(F_INTERFACE_GFORT) || defined(F_INTERFACE_G95)
	EMMS
#endif

	movl	M,    %ebx
	movl	X,    %ecx
	movl	INCX, %esi
	movl	Y,    %edx
	movl	INCY, %edi

	testl	%ebx, %ebx	# if m == 0 goto End
	jle	.L999

#if SIZE > 8
	sall	$BASE_SHIFT, %esi
	sall	$BASE_SHIFT, %edi
#else
	leal	(, %esi, SIZE), %esi
	leal	(, %edi, SIZE), %edi
#endif

	cmpl	$SIZE, %esi	# if incx != 1
	jne	.L100
	cmpl	$SIZE, %edi	# if incy != 1
	jne	.L100

	movl	%ebx, %eax	# i = m
	sarl	$3,   %eax
	jle	.L20
	ALIGN_2

.L11:
	FLD	7 * SIZE(%ecx)	
	FLD	6 * SIZE(%ecx)	
	FLD	5 * SIZE(%ecx)	
	FLD	4 * SIZE(%ecx)	
	FLD	3 * SIZE(%ecx)	
	FLD	2 * SIZE(%ecx)	
	FLD	1 * SIZE(%ecx)	
	FLD	0 * SIZE(%ecx)	

	FST	0 * SIZE(%edx)
	FST	1 * SIZE(%edx)
	FST	2 * SIZE(%edx)
	FST	3 * SIZE(%edx)
	FST	4 * SIZE(%edx)
	FST	5 * SIZE(%edx)
	FST	6 * SIZE(%edx)
	FST	7 * SIZE(%edx)

	addl	$8 * SIZE, %ecx
	addl	$8 * SIZE, %edx
	decl	%eax
	jg	.L11
	ALIGN_2

.L20:
	movl	%ebx, %eax	# i = m
	andl	$7,   %eax
	jle	.L99
	ALIGN_2

.L21:
	FLD	(%ecx)
	FST	(%edx)
	addl	$SIZE, %ecx
	addl	$SIZE, %edx
	decl	%eax
	jg	.L21

.L99:
	xorl	%eax,%eax
	popl	%ebx
	popl	%esi
	popl	%edi
	ret
	ALIGN_3

.L100:
	movl	%ebx, %eax
	sarl	$3,   %eax
	jle	.L120
	ALIGN_2

.L111:
	FLD	(%ecx)
	addl	%esi, %ecx
	FLD	(%ecx)
	addl	%esi, %ecx
	FLD	(%ecx)
	addl	%esi, %ecx
	FLD	(%ecx)
	addl	%esi, %ecx
	FLD	(%ecx)
	addl	%esi, %ecx
	FLD	(%ecx)
	addl	%esi, %ecx
	FLD	(%ecx)
	addl	%esi, %ecx
	FLD	(%ecx)
	addl	%esi, %ecx

	fxch	%st(7)
	FST	(%edx)
	addl	%edi, %edx

	fxch	%st(5)
	FST	(%edx)
	addl	%edi, %edx

	fxch	%st(3)
	FST	(%edx)
	addl	%edi, %edx

	fxch	%st(1)
	FST	(%edx)
	addl	%edi, %edx

	FST	(%edx)
	addl	%edi, %edx

	FST	(%edx)
	addl	%edi, %edx

	FST	(%edx)
	addl	%edi, %edx

	FST	(%edx)
	addl	%edi, %edx

	decl	%eax
	jg	.L111

.L120:
	movl	%ebx, %eax
	andl	$7,   %eax
	jle	.L999
	ALIGN_2

.L121:
	FLD	(%ecx)
	FST	(%edx)
	addl	%esi, %ecx
	addl	%edi, %edx
	decl	%eax
	jg	.L121

.L999:
	xorl	%eax,%eax
	popl	%ebx
	popl	%esi
	popl	%edi
	ret

	EPILOGUE
