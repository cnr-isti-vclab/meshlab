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

	lbz	r3,  0(r3)
	lbz	r4,  0(r4)

	cmplwi	cr0, r3, 96
	cmplwi	cr6, r4, 96
	addi	r0, r3, -32
	addi	r11,r4, -32

	ble-	cr0, LL(2)
#ifdef __64BIT__
	rldicl	r3, r0, 0, 56
#else
	rlwinm  r3, r0, 0, 0xff
#endif
LL(2):
	ble-	cr6, LL(3)
#ifdef __64BIT__
	rldicl	r4, r11, 0, 56
#else
	rlwinm  r4, r11, 0, 0xff
#endif
LL(3):
	xor	r3, r3, r4
	subfic	r0, r3, 0
	adde	r3, r0, r3
	blr

	EPILOGUE
