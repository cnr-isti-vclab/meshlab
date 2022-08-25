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

	PROLOGUE
	PROFCODE

	movl	 4(%esp), %eax
	movl	 8(%esp), %edx
	movb	  (%eax), %al		# a = *A
	movb	  (%edx), %dl		# b = *B

	andl	$255, %eax
	andl	$255, %edx

	subl	$65,  %eax
	subl	$65,  %edx

#ifndef HAVE_CMOV
	movl	%eax, %ecx
	subl	$32,  %ecx
	jle	.L1
	movl	%ecx, %eax
.L1:

	movl	%edx, %ecx
	subl	$32,  %ecx
	jle	.L2
	movl	%ecx, %edx
.L2:
	subl	%eax, %edx
	movl	$0,   %eax
	movl	$1,   %edx
	jne	.L3
	movl	%edx, %eax
.L3:
#else
	movl	%eax, %ecx
	subl	$32,  %ecx
	cmovg	%ecx, %eax

	movl	%edx, %ecx
	subl	$32,  %ecx
	cmovg	%ecx, %edx

	subl	%eax, %edx
	movl	$0,   %eax
	movl	$1,   %edx
	cmove	%edx, %eax
#endif
	ret

	EPILOGUE
