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

	.prologue
	.body
	ld1	r14 = [r32]
	ld1	r15 = [r33]
	;;
	adds	r16 = -32, r14		// a1 = a - 32
	adds	r17 = -32, r15		// b1 = b - 32
	;;
	cmp4.ge	p6, p7 = 96, r14	// if (a > 96)
	cmp4.ge	p8, p9 = 96, r15	// if (b > 96)
	;;
	(p7) mov r14 = r16
	(p9) mov r15 = r17
	;;
	cmp4.eq p6, p7 = r15, r14
	mov r8 = 1
	;; 
	(p7) mov r8 = 0
	br.ret.sptk.many b0

	EPILOGUE

