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
#define ARGS     0
	
#define N	 4 + STACK + ARGS(%esp)
#ifdef XDOUBLE
#define X	48 + STACK + ARGS(%esp)
#define INCX	52 + STACK + ARGS(%esp)
#define Y	56 + STACK + ARGS(%esp)
#define INCY	60 + STACK + ARGS(%esp)
#elif defined(DOUBLE)
#define X	32 + STACK + ARGS(%esp)
#define INCX	36 + STACK + ARGS(%esp)
#define Y	40 + STACK + ARGS(%esp)
#define INCY	44 + STACK + ARGS(%esp)
#else
#define X	24 + STACK + ARGS(%esp)
#define INCX	28 + STACK + ARGS(%esp)
#define Y	32 + STACK + ARGS(%esp)
#define INCY	36 + STACK + ARGS(%esp)
#endif

	PROLOGUE

	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx

	PROFCODE

#if defined(F_INTERFACE_GFORT) || defined(F_INTERFACE_G95)
	EMMS
#endif

	movl	N,    %edx
	movl	X,    %esi
	movl	Y,    %edi
	movl	INCX, %ebx
	movl	INCY, %ecx

	sall	$ZBASE_SHIFT, %ebx
	sall	$ZBASE_SHIFT, %ecx

	cmpl	$2 * SIZE, %ebx
	jne	.L14
	cmpl	$2 * SIZE, %ecx
	jne	.L14

	movl	%edx, %eax
	sarl	$1,   %eax
	jle	.L15
	ALIGN_3

.L16:
#if defined(DOUBLE) || defined(XDOUBLE)
	FLD	3 * SIZE(%esi)
	FLD	2 * SIZE(%esi)
	FLD	1 * SIZE(%esi)
	FLD	0 * SIZE(%esi)
	FLD	3 * SIZE(%edi)
	FLD	2 * SIZE(%edi)
	FLD	1 * SIZE(%edi)
	FLD	0 * SIZE(%edi)

	FST	0 * SIZE(%esi)
	FST	1 * SIZE(%esi)
	FST	2 * SIZE(%esi)
	FST	3 * SIZE(%esi)
	FST	0 * SIZE(%edi)
	FST	1 * SIZE(%edi)
	FST	2 * SIZE(%edi)
	FST	3 * SIZE(%edi)
#else
	fldl	2 * SIZE(%esi)
	fldl	0 * SIZE(%esi)
	fldl	2 * SIZE(%edi)
	fldl	0 * SIZE(%edi)

	fstpl	0 * SIZE(%esi)
	fstpl	2 * SIZE(%esi)
	fstpl	0 * SIZE(%edi)
	fstpl	2 * SIZE(%edi)
#endif
	addl	$4 * SIZE, %esi
	addl	$4 * SIZE, %edi
	decl	%eax
	jg	.L16
	ALIGN_3

.L15:
	movl	%edx, %eax
	andl	$1,   %eax
	jle	.L27
	ALIGN_3

.L22:
#if defined(DOUBLE) || defined(XDOUBLE)
	FLD	1 * SIZE(%esi)
	FLD	0 * SIZE(%esi)
	FLD	1 * SIZE(%edi)
	FLD	0 * SIZE(%edi)
	FST	0 * SIZE(%esi)
	FST	1 * SIZE(%esi)
	FST	0 * SIZE(%edi)
	FST	1 * SIZE(%edi)
#else
	fldl	0 * SIZE(%esi)
	fldl	0 * SIZE(%edi)
	fstpl	0 * SIZE(%esi)
	fstpl	0 * SIZE(%edi)
#endif

	jmp	.L27
	ALIGN_3

/* INCX != 1 or INCY != 1 */

.L14:
	movl	%edx, %eax
	sarl	$1,   %eax
	jle	.L28
	ALIGN_2

.L29:
#if defined(DOUBLE) || defined(XDOUBLE)
	FLD	1 * SIZE(%esi)
	FLD	0 * SIZE(%esi)
	addl	%ebx, %esi
	FLD	1 * SIZE(%esi)
	FLD	0 * SIZE(%esi)

	FLD	1 * SIZE(%edi)
	FLD	0 * SIZE(%edi)
	addl	%ecx, %edi
	FLD	1 * SIZE(%edi)
	FLD	0 * SIZE(%edi)

	FST	0 * SIZE(%esi)
	FST	1 * SIZE(%esi)
	subl	%ebx, %esi
	FST	0 * SIZE(%esi)
	FST	1 * SIZE(%esi)
	leal	(%esi, %ebx, 2), %esi

	FST	0 * SIZE(%edi)
	FST	1 * SIZE(%edi)
	subl	%ecx, %edi
	FST	0 * SIZE(%edi)
	FST	1 * SIZE(%edi)
	leal	(%edi, %ecx, 2), %edi
#else
	fldl	0 * SIZE(%esi)
	addl	%ebx, %esi
	fldl	0 * SIZE(%esi)

	fldl	0 * SIZE(%edi)
	addl	%ecx, %edi
	fldl	0 * SIZE(%edi)

	fstpl	0 * SIZE(%esi)
	subl	%ebx, %esi
	fstpl	0 * SIZE(%esi)
	leal	(%esi, %ebx, 2), %esi

	fstpl	0 * SIZE(%edi)
	subl	%ecx, %edi
	fstpl	0 * SIZE(%edi)
	leal	(%edi, %ecx, 2), %edi
#endif

	decl	%eax
	jg	.L29
	ALIGN_3

.L28:
	movl	%edx, %eax
	andl	$1,   %eax
	jle	.L27
	ALIGN_3

.L35:
#if defined(DOUBLE) || defined(XDOUBLE)
	FLD	1 * SIZE(%esi)
	FLD	0 * SIZE(%esi)
	FLD	1 * SIZE(%edi)
	FLD	0 * SIZE(%edi)
	FST	0 * SIZE(%esi)
	FST	1 * SIZE(%esi)
	FST	0 * SIZE(%edi)
	FST	1 * SIZE(%edi)
#else
	fldl	0 * SIZE(%esi)
	fldl	0 * SIZE(%edi)
	fstpl	0 * SIZE(%esi)
	fstpl	0 * SIZE(%edi)
#endif
	ALIGN_3

.L27:
	xorl	%eax,%eax
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret

	EPILOGUE
