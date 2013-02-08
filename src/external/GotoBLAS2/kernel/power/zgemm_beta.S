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

#define ALPHA_R	f30
#define ALPHA_I	f31

#define STACKSIZE 32

	PROLOGUE
	PROFCODE

	addi	SP, SP, -STACKSIZE
	li	r0, 0

	stfd	f30,    0(SP)
	stfd	f31,    8(SP)
	stw	r0,    16(SP)

#ifdef linux
#ifndef __64BIT__
	lwz	LDC,      8 + STACKSIZE(SP)
#else
	ld	C,      120 + STACKSIZE(SP)
	ld	LDC,    128 + STACKSIZE(SP)
#endif
#endif

#if defined(_AIX) || defined(__APPLE__)
#ifdef __64BIT__
	ld	C,      120 + STACKSIZE(SP)
	ld	LDC,    128 + STACKSIZE(SP)
#else
#ifdef DOUBLE
	lwz	C,       68 + STACKSIZE(SP)
	lwz	LDC,     72 + STACKSIZE(SP)
#else
	lwz	C,       60 + STACKSIZE(SP)
	lwz	LDC,     64 + STACKSIZE(SP)
#endif
#endif
#endif

	
	slwi	LDC, LDC, ZBASE_SHIFT

	lfs	f0,    16(SP)
	
	fmr	ALPHA_R, f1
	fmr	ALPHA_I, f2

	cmpwi	cr0, M, 0
	ble-	LL(999)
	cmpwi	cr0, N, 0
	ble-	LL(999)

	mr	J, N
	fcmpu	cr7, f1, f0
	bne	cr7, LL(20)
	fcmpu	cr7, f2, f0
	bne	cr7, LL(20)
	.align 4

LL(10):
	mr	CO1, C
	add	C,  C, LDC
	addi	PRE, 0, 32 * SIZE

	srawi.	r0,  M,  3
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
	andi.	r0,  M,  7
	mtspr	CTR, r0
	beq	LL(19)
	.align 4

LL(16):
	STFD	f0,  0 * SIZE(CO1)
	STFD	f0,  1 * SIZE(CO1)
	addi	CO1, CO1, 2 * SIZE
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

	srawi.	r0,  M,  2
	mtspr	CTR, r0
	ble	LL(25)
	.align 4

LL(22):
	LFD	f3,  0 * SIZE(CO1)
	LFD	f4,  1 * SIZE(CO1)
	LFD	f5,  2 * SIZE(CO1)
	LFD	f6,  3 * SIZE(CO1)
	LFD	f7,  4 * SIZE(CO1)
	LFD	f8,  5 * SIZE(CO1)
	LFD	f9,  6 * SIZE(CO1)
	LFD	f10, 7 * SIZE(CO1)

	FMUL	f0,  ALPHA_I, f4
	FMUL	f4,  ALPHA_R, f4
	FMUL	f11, ALPHA_I, f6
	FMUL	f6,  ALPHA_R, f6

	FMUL	f12, ALPHA_I, f8
	FMUL	f8,  ALPHA_R, f8
	FMUL	f13, ALPHA_I, f10
	FMUL	f10, ALPHA_R, f10

	FMADD	f4,  ALPHA_I, f3, f4
	FMSUB	f3,  ALPHA_R, f3, f0
	FMADD	f6,  ALPHA_I, f5, f6
	FMSUB	f5,  ALPHA_R, f5, f11

	FMADD	f8,  ALPHA_I, f7, f8
	FMSUB	f7,  ALPHA_R, f7, f12
	FMADD	f10, ALPHA_I, f9, f10
	FMSUB	f9,  ALPHA_R, f9, f13

	STFD	f3,  0 * SIZE(CO1)
	STFD	f4,  1 * SIZE(CO1)
	STFD	f5,  2 * SIZE(CO1)
	STFD	f6,  3 * SIZE(CO1)
	STFD	f7,  4 * SIZE(CO1)
	STFD	f8,  5 * SIZE(CO1)
	STFD	f9,  6 * SIZE(CO1)
	STFD	f10, 7 * SIZE(CO1)

	addi	CO1, CO1,  8 * SIZE
	dcbtst	PRE, CO1
	bdnz	LL(22)
	.align 4
	
LL(25):
	andi.	r0,  M,  3
	mtspr	CTR, r0
	ble	LL(29)
	.align 4

LL(26):
	LFD	f0,  0 * SIZE(CO1)
	LFD	f1,  1 * SIZE(CO1)

	FMUL	f5, ALPHA_I, f1
	FMUL	f1, ALPHA_R, f1
	FMADD	f1, ALPHA_I, f0, f1
	FMSUB	f0, ALPHA_R, f0, f5

	STFD	f0,  0 * SIZE(CO1)
	STFD	f1,  1 * SIZE(CO1)

	addi	CO1, CO1, 2 * SIZE
	bdnz	LL(26)
	.align 4

LL(29):
	addic.	J,  J,  -1
	bgt	LL(20)
	.align 4

LL(999):
	li	r3, 0
	lfd	f30,    0(SP)
	lfd	f31,    8(SP)
	addi	SP, SP, STACKSIZE

	blr
	EPILOGUE
