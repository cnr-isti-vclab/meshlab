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
	
#define N	r3
#define X	r4
#define INCX	r5	
#define Y	r6
#define INCY	r7
#define PREA	r8

#define INCXM1	r9
#define INCYM1	r10

#define STACKSIZE 16

	PROLOGUE
	PROFCODE

	addi	SP, SP, -STACKSIZE
	li	r0,   0

	stfd	f14,    0(SP)
	stfd	f15,    8(SP)

	slwi	INCX, INCX, ZBASE_SHIFT
	slwi	INCY, INCY, ZBASE_SHIFT

	subi	INCXM1, INCX, SIZE
	subi	INCYM1, INCY, SIZE

#ifdef L1_DUALFETCH
	li	PREA, (L1_PREFETCHSIZE) / 2
#else
	li	PREA, (L1_PREFETCHSIZE) 
#endif

	cmpwi	cr0, N, 0
	ble-	LL(999)

	cmpwi	cr0, INCX, 2 * SIZE
	bne-	cr0, LL(100)
	cmpwi	cr0, INCY, 2 * SIZE
	bne-	cr0, LL(100)

	srawi.	r0, N, 3
	mtspr	CTR, r0
	beq-	cr0, LL(50)
	.align 4

LL(10):
	LFD	f0,    0 * SIZE(X)
	LFD	f1,    1 * SIZE(X)
	LFD	f2,    2 * SIZE(X)
	LFD	f3,    3 * SIZE(X)

	STFD	f0,    0 * SIZE(Y)
	STFD	f1,    1 * SIZE(Y)
	STFD	f2,    2 * SIZE(Y)
	STFD	f3,    3 * SIZE(Y)

	LFD	f4,    4 * SIZE(X)
	LFD	f5,    5 * SIZE(X)
	LFD	f6,    6 * SIZE(X)
	LFD	f7,    7 * SIZE(X)

	STFD	f4,    4 * SIZE(Y)
	STFD	f5,    5 * SIZE(Y)
	STFD	f6,    6 * SIZE(Y)
	STFD	f7,    7 * SIZE(Y)

	LFD	f8,    8 * SIZE(X)
	LFD	f9,    9 * SIZE(X)
	LFD	f10,  10 * SIZE(X)
	LFD	f11,  11 * SIZE(X)

	STFD	f8,    8 * SIZE(Y)
	STFD	f9,    9 * SIZE(Y)
	STFD	f10,  10 * SIZE(Y)
	STFD	f11,  11 * SIZE(Y)

	LFD	f12,  12 * SIZE(X)
	LFD	f13,  13 * SIZE(X)
	LFD	f14,  14 * SIZE(X)
	LFD	f15,  15 * SIZE(X)

	STFD	f12,  12 * SIZE(Y)
	STFD	f13,  13 * SIZE(Y)
	STFD	f14,  14 * SIZE(Y)
	STFD	f15,  15 * SIZE(Y)

#ifndef POWER6
	dcbtst	Y, PREA
#ifdef	L1_DUALFETCH
	dcbt	X, PREA
#endif
#endif
	addi	X, X, 16 * SIZE
	addi	Y, Y, 16 * SIZE

#ifdef POWER6
	dcbtst	Y, PREA
	L1_PREFETCH	X, PREA
#endif

	bdnz	LL(10)
	.align 4

LL(50):
	andi.	r0,  N, 7
	mtspr	CTR, r0
	beq	LL(999)
	.align 4

LL(60):
	LFD	f8,  0 * SIZE(X)
	LFD	f9,  1 * SIZE(X)
	addi	X, X,  2 * SIZE

	STFD	f8, 0 * SIZE(Y)
	STFD	f9, 1 * SIZE(Y)
	addi	Y, Y,  2 * SIZE
	bdnz	LL(60)
	b	LL(999)
	.align 4

LL(100):
	sub	X, X, INCXM1
	sub	Y, Y, INCYM1

	srawi.	r0, N, 3
	mtspr	CTR,  r0
	beq-	LL(150)
	.align 4

LL(110):
	LFDX	f0,    X, INCXM1
	LFDUX	f1,    X, INCX
	LFDX	f2,    X, INCXM1
	LFDUX	f3,    X, INCX

	LFDX	f4,    X, INCXM1
	LFDUX	f5,    X, INCX
	LFDX	f6,    X, INCXM1
	LFDUX	f7,    X, INCX

	LFDX	f8,    X, INCXM1
	LFDUX	f9,    X, INCX
	LFDX	f10,   X, INCXM1
	LFDUX	f11,   X, INCX

	LFDX	f12,   X, INCXM1
	LFDUX	f13,   X, INCX
	LFDX	f14,   X, INCXM1
	LFDUX	f15,   X, INCX

	STFDX	f0,    Y, INCYM1
	STFDUX	f1,    Y, INCY
	STFDX	f2,    Y, INCYM1
	STFDUX	f3,    Y, INCY

	STFDX	f4,    Y, INCYM1
	STFDUX	f5,    Y, INCY
	STFDX	f6,    Y, INCYM1
	STFDUX	f7,    Y, INCY

	STFDX	f8,    Y, INCYM1
	STFDUX	f9,    Y, INCY
	STFDX	f10,   Y, INCYM1
	STFDUX	f11,   Y, INCY

	STFDX	f12,   Y, INCYM1
	STFDUX	f13,   Y, INCY
	STFDX	f14,   Y, INCYM1
	STFDUX	f15,   Y, INCY
	bdnz	LL(110)
	.align 4

LL(150):
	andi.	r0,  N, 7
	mtspr	CTR, r0
	beq	LL(999)
	.align 4

LL(160):
	LFDX	f8,    X, INCXM1
	LFDUX	f9,    X, INCX

	STFDX	f8,    Y, INCYM1
	STFDUX	f9,    Y, INCY
	bdnz	LL(160)
	.align 4

LL(999):
	lfd	f14,    0(SP)
	lfd	f15,    8(SP)
	addi	SP, SP, STACKSIZE
	blr

	EPILOGUE
