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
	
#define	M	r3
#define	N	r4
#define	A	r5
#define	LDA	r6
#define B	r7

#define AO1	r8
#define AO2	r9

#define J	r10
#define B1	r11

#define B2	r28
#define M4	r29
#define INC	r30
#define INC2	r31
	
#define c01	f0
#define c02	f1
#define c03	f2
#define c04	f3
#define c05	f4
#define c06	f5
#define c07	f6
#define c08	f7

	PROLOGUE
	PROFCODE

	stwu	r31,  -4(SP)
	stwu	r30,  -4(SP)
	stwu	r29,  -4(SP)
	stwu	r28,  -4(SP)

	slwi	LDA, LDA, ZBASE_SHIFT
	slwi	M4, M, 1 + ZBASE_SHIFT

	li	r9, -2

	and	B2, N, r9

	mullw	B2, B2, M

	slwi	B2, B2, ZBASE_SHIFT

	add	B2, B2, B

	cmpwi	cr0, M, 0
	ble-	LL(99)
	cmpwi	cr0, N, 0
	ble-	LL(99)

	subi	B2, B2, 2 * SIZE
	subi	M4, M4, 6 * SIZE

	li	INC,  1 * SIZE
	li	INC2, 2 * SIZE

	andi.	r0, A,   2 * SIZE - 1
	bne	LL(100)

	subi	A, A, 2 * SIZE
	srawi.	J,  M,  1
	ble	LL(20)
	.align 4

LL(10):
	mr	AO1, A
	add	AO2, A,   LDA
	add	A,   AO2, LDA

	sub	B1, B, M4
	addi	B, B, 8 * SIZE

	srawi.	r0,  N,  1
	mtspr	CTR, r0
	ble	LL(15)
	.align 4

LL(12):
	LFPDUX	c01,   AO1, INC2
	LFPDUX	c02,   AO1, INC2
	LFPDUX	c03,   AO2, INC2
	LFPDUX	c04,   AO2, INC2

	STFPDUX	c01,   B1, M4
	STFPDUX	c02,   B1, INC2
	STFPDUX	c03,   B1, INC2
	STFPDUX	c04,   B1, INC2
	bdnz	LL(12)
	.align 4
	
LL(15):
	andi.	r0,  N,  1
	ble	LL(19)

	LFPDUX	c01,   AO1, INC2
	LFPDUX	c02,   AO2, INC2

	STFPDUX	c01,   B2, INC2
	STFPDUX	c02,   B2, INC2
	.align 4

LL(19):
	addic.	J, J, -1
	bgt	LL(10)
	.align 4

LL(20):
	andi.	J,  M,  1
	addi	M4, M4, 4 * SIZE
	ble	LL(99)

	mr	AO1, A
	sub	B1, B, M4

	srawi.	r0,  N,  1
	mtspr	CTR, r0
	ble	LL(23)
	.align 4

LL(22):
	LFPDUX	c01,   AO1, INC2
	LFPDUX	c02,   AO1, INC2

	STFPDUX	c01,   B1, M4
	STFPDUX	c02,   B1, INC2
	bdnz	LL(22)
	.align 4
	
LL(23):
	andi.	r0,  N,  1
	ble	LL(99)

	LFPDUX	c01,   AO1, INC2

	STFPDUX	c01,   B2, INC2
	.align 4

LL(99):
	addi	SP, SP, -4

	lwzu	r28,   4(SP)
	lwzu	r29,   4(SP)
	lwzu	r30,   4(SP)
	lwzu	r31,   4(SP)

	addi	SP, SP, 4
	blr
	.align 4

LL(100):
	subi	A, A, SIZE
	srawi.	J,  M,  1
	ble	LL(120)
	.align 4

LL(110):
	mr	AO1, A
	add	AO2, A,   LDA
	add	A,   AO2, LDA

	sub	B1, B, M4
	addi	B, B, 8 * SIZE

	srawi.	r0,  N,  1
	mtspr	CTR, r0
	ble	LL(115)
	.align 4

LL(112):
	LFDUX	c01,   AO1, INC
	LFDUX	c02,   AO1, INC
	LFDUX	c03,   AO1, INC
	LFDUX	c04,   AO1, INC

	LFDUX	c05,   AO2, INC
	fsmfp	c01, c02
	LFDUX	c06,   AO2, INC
	fsmfp	c03, c04
	LFDUX	c07,   AO2, INC
	fsmfp	c05, c06
	LFDUX	c08,   AO2, INC
	fsmfp	c07, c08

	STFPDUX	c01,   B1, M4
	STFPDUX	c03,   B1, INC2
	STFPDUX	c05,   B1, INC2
	STFPDUX	c07,   B1, INC2
	bdnz	LL(112)
	.align 4
	
LL(115):
	andi.	r0,  N,  1
	ble	LL(119)

	LFDUX	c01,   AO1, INC
	LFDUX	c02,   AO1, INC
	LFDUX	c03,   AO2, INC
	LFDUX	c04,   AO2, INC

	fsmfp	c01, c02
	fsmfp	c03, c04

	STFPDUX	c01,   B2, INC2
	STFPDUX	c03,   B2, INC2
	.align 4

LL(119):
	addic.	J, J, -1
	bgt	LL(110)
	.align 4

LL(120):
	andi.	J,  M,  1
	addi	M4, M4, 4 * SIZE
	ble	LL(999)

	mr	AO1, A
	sub	B1, B, M4

	srawi.	r0,  N,  1
	mtspr	CTR, r0
	ble	LL(123)
	.align 4

LL(122):
	LFDUX	c01,   AO1, INC
	LFDUX	c02,   AO1, INC
	LFDUX	c03,   AO1, INC
	LFDUX	c04,   AO1, INC

	fsmfp	c01, c02
	fsmfp	c03, c04

	STFPDUX	c01,   B1, M4
	STFPDUX	c03,   B1, INC2
	bdnz	LL(122)
	.align 4
	
LL(123):
	andi.	r0,  N,  1
	ble	LL(999)

	LFDUX	c01,   AO1, INC
	LFDUX	c02,   AO1, INC

	fsmfp	c01, c02

	STFPDUX	c01,   B2, INC2
	.align 4

LL(999):
	addi	SP, SP, -4

	lwzu	r28,   4(SP)
	lwzu	r29,   4(SP)
	lwzu	r30,   4(SP)
	lwzu	r31,   4(SP)

	addi	SP, SP, 4
	blr




	EPILOGUE
