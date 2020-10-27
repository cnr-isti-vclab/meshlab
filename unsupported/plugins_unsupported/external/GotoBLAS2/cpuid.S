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

#if defined(__APPLE__) && defined(__i386__)

/* Quick hack for Darwin/x86 */
	
	.text
	.globl _cpuid
_cpuid:	
	pushl	%esi
	pushl	%ebx

	movl	12(%esp), %eax
	cpuid

	movl	16(%esp), %esi
	movl	%eax, (%esi)
	movl	20(%esp), %esi
	movl	%ebx, (%esi)
	movl	24(%esp), %esi
	movl	%ecx, (%esi)
	movl	28(%esp), %esi
	movl	%edx, (%esi)

	popl	%ebx
	popl	%esi
	ret

	.subsections_via_symbols

#endif
