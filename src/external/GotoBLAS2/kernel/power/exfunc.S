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

	.machine "any"

	.globl	.rpcc                   
.rpcc:
	mftb	r3
	rlinm	r3, r3, 3, 0, 31	# ldc(scaling)
	bcr	BO_ALWAYS,CR0_LT

	.globl	.blas_lock
.blas_lock:
	cal	r7, 1(r0)
LL(0):
	l	r6, 0(r3)
	cmpi	CR0, r6, 0
	bne	LL(2)
	lwarx	r6, r0, r3
	cmpwi	CR6, r6, 0
	bne	LL(2)
	stwcx.	r7, r0, r3
	bne-	LL(0)
LL(1):
	bcr	BO_ALWAYS,CR0_LT

LL(2):
	b	LL(0)
