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
#define	C	r10
#define	LDC	r11
#define	J	r5
#define PRE	r6
#define	CO1	r7

#define ALPHA	f31

#define STACKSIZE 32

	PROLOGUE
	PROFCODE

	addi	SP, SP, -STACKSIZE
	li	r0, 0

	stfd	f14,    0(SP)
	stfd	f15,    8(SP)
	stfd	f31,   16(SP)
	stw	r0,    24(SP)

#ifdef linux
#ifndef __64BIT__
	lwz	LDC,      8 + STACKSIZE(SP)
#else
	ld	C,      112 + STACKSIZE(SP)
	ld	LDC,    120 + STACKSIZE(SP)
#endif
#endif

#if defined(_AIX) || defined(__APPLE__)
#ifdef __64BIT__
	ld	C,      112 + STACKSIZE(SP)
	ld	LDC,    120 + STACKSIZE(SP)
#else
#ifdef DOUBLE
	lwz	C,       60 + STACKSIZE(SP)
	lwz	LDC,     64 + STACKSIZE(SP)
#else
	lwz	C,       56 + STACKSIZE(SP)
	lwz	LDC,     60 + STACKSIZE(SP)
#endif
#endif
#endif

	slwi	LDC, LDC, BASE_SHIFT

	fmr	ALPHA, f1
	lfs	f0,    24(SP)
	
	cmpwi	cr0, M, 0
	ble-	LL(999)
	cmpwi	cr0, N, 0
	ble-	LL(999)

	mr	J, N
	fcmpu	cr7, f1, f0
	bne	cr7, LL(20)
	.align 4

LL(10):
	mr	CO1, C
	add	C,  C, LDC
	addi	PRE, 0, 32 * SIZE

	srawi.	r0,  M,  4
	mtspr	CTR, r0
	ble	LL(15)
	.align 4

LL(12):
	STFD	f0,   0 * SIZE(CO1)
	STFD	f0,   1 * SIZE(CO1)
	STFD	f0,   2 * SIZE(CO1)
	STFD	f0,   3 * SIZE(CO1)
	STFD	f0,   4 * SIZE(CO1)
	STFD	f0,   5 * SIZE(CO1)
	STFD	f0,   6 * SIZE(CO1)
	STFD	f0,   7 * SIZE(CO1)
	STFD	f0,   8 * SIZE(CO1)
	STFD	f0,   9 * SIZE(CO1)
	STFD	f0,  10 * SIZE(CO1)
	STFD	f0,  11 * SIZE(CO1)
	STFD	f0,  12 * SIZE(CO1)
	STFD	f0,  13 * SIZE(CO1)
	STFD	f0,  14 * SIZE(CO1)
	STFD	f0,  15 * SIZE(CO1)

	dcbst	PRE, CO1
	addi	CO1, CO1,  16 * SIZE
	bdnz	LL(12)
	.align 4
	
LL(15):
	andi.	r0,  M,  15
	mtspr	CTR, r0
	beq	LL(19)
	.align 4

LL(16):
	STFD	f0,  0 * SIZE(CO1)
	addi	CO1, CO1, 1 * SIZE
	bdnz	LL(16)
	.align 4

LL(19):
	addic.	J,  J,  -1
	bgt	LL(10)
	b	LL(999)
	.align 4

LL(20):
	mr	CO1, C
	add	C,  C, LDC
	addi	PRE, 0, 16 * SIZE

	srawi.	r0,  M,  4
	mtspr	CTR, r0
	ble	LL(25)
	.align 4

LL(22):
	LFD	f0,    0 * SIZE(CO1)
	LFD	f1,    1 * SIZE(CO1)
	LFD	f2,    2 * SIZE(CO1)
	LFD	f3,    3 * SIZE(CO1)
	LFD	f4,    4 * SIZE(CO1)
	LFD	f5,    5 * SIZE(CO1)
	LFD	f6,    6 * SIZE(CO1)
	LFD	f7,    7 * SIZE(CO1)

	LFD	f8,    8 * SIZE(CO1)
	LFD	f9,    9 * SIZE(CO1)
	LFD	f10,  10 * SIZE(CO1)
	LFD	f11,  11 * SIZE(CO1)
	LFD	f12,  12 * SIZE(CO1)
	LFD	f13,  13 * SIZE(CO1)
	LFD	f14,  14 * SIZE(CO1)
	LFD	f15,  15 * SIZE(CO1)

	FMUL	f0,   ALPHA, f0
	FMUL	f1,   ALPHA, f1
	FMUL	f2,   ALPHA, f2
	FMUL	f3,   ALPHA, f3
	FMUL	f4,   ALPHA, f4
	FMUL	f5,   ALPHA, f5
	FMUL	f6,   ALPHA, f6
	FMUL	f7,   ALPHA, f7

	FMUL	f8,   ALPHA, f8
	FMUL	f9,   ALPHA, f9
	FMUL	f10,  ALPHA, f10
	FMUL	f11,  ALPHA, f11
	FMUL	f12,  ALPHA, f12
	FMUL	f13,  ALPHA, f13
	FMUL	f14,  ALPHA, f14
	FMUL	f15,  ALPHA, f15

	STFD	f0,    0 * SIZE(CO1)
	STFD	f1,    1 * SIZE(CO1)
	STFD	f2,    2 * SIZE(CO1)
	STFD	f3,    3 * SIZE(CO1)
	STFD	f4,    4 * SIZE(CO1)
	STFD	f5,    5 * SIZE(CO1)
	STFD	f6,    6 * SIZE(CO1)
	STFD	f7,    7 * SIZE(CO1)

	STFD	f8,    8 * SIZE(CO1)
	STFD	f9,    9 * SIZE(CO1)
	STFD	f10,  10 * SIZE(CO1)
	STFD	f11,  11 * SIZE(CO1)
	STFD	f12,  12 * SIZE(CO1)
	STFD	f13,  13 * SIZE(CO1)
	STFD	f14,  14 * SIZE(CO1)
	STFD	f15,  15 * SIZE(CO1)

	addi	CO1, CO1,  16 * SIZE
	dcbtst	PRE, CO1
	bdnz	LL(22)
	.align 4
	
LL(25):
	andi.	r0,  M,  15
	mtspr	CTR, r0
	ble	LL(29)
	.align 4

LL(26):
	LFD	f0,  0 * SIZE(CO1)
	FMUL	f0, f0, ALPHA
	STFD	f0,  0 * SIZE(CO1)
	addi	CO1, CO1, 1 * SIZE
	bdnz	LL(26)
	.align 4

LL(29):
	addic.	J,  J,  -1
	bgt	LL(20)
	.align 4

LL(999):
	li	r3, 0

	lfd	f14,    0(SP)
	lfd	f15,    8(SP)
	lfd	f31,   16(SP)
	addi	SP, SP, STACKSIZE

	blr
	EPILOGUE
