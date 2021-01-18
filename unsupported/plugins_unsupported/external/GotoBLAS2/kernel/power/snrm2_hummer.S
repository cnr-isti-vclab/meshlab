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

#define INCX2	r6
#define X2	r7

#define C1	f1
#define C2	f0
#define C3	f2
#define C4	f3
#define C5	f4
#define C6	f5
#define C7	f6
#define C8	f7

#define A1	f8
#define A2	f9
#define A3	f10
#define A4	f11
#define A5	f12
#define A6	f13
#define A7	f14
#define A8	f15

#define A9	f16
#define A10	f17
#define A11	f18
#define A12	f19
#define A13	f20
#define A14	f21
#define A15	f22
#define A16	f23

	PROLOGUE
	PROFCODE

	li	r10, -16

	stfpdux	f14, SP, r10
	stfpdux	f15, SP, r10
	stfpdux	f16, SP, r10
	stfpdux	f17, SP, r10

	stfpdux	f18, SP, r10
	stfpdux	f19, SP, r10
	stfpdux	f20, SP, r10
	stfpdux	f21, SP, r10

	stfpdux	f22, SP, r10
	stfpdux	f23, SP, r10

	li	r10,   0
	stwu	r10,   -4(SP)
	stwu	r10,   -4(SP)
	stwu	r10,   -4(SP)
	stwu	r10,   -4(SP)

#ifdef F_INTERFACE
	LDINT	N,    0(N)
	LDINT	INCX, 0(INCX)
#endif

	lfpdx	C1, SP, r10		# Zero clear

	slwi	INCX,  INCX, BASE_SHIFT
	add	INCX2, INCX, INCX

	fpmr	C2, C1
	fpmr	C3, C1
	fpmr	C4, C1

	fpmr	C5, C1
	fpmr	C6, C1
	fpmr	C7, C1
	fpmr	C8, C1

	cmpwi	cr0, N, 0
	ble	LL(99)
	cmpwi	cr0, INCX, 0
	ble	LL(99)

	cmpwi	cr0, INCX, SIZE
	bne	LL(100)

	andi.	r0, X, 2 * SIZE - 1
	beq	LL(05)

	LFD	C1, 0(X)
	addi	X, X, 1 * SIZE
	addi	N, N, -1
	cmpwi	cr0, N, 0
	fmul	C1, C1, C1
	ble	LL(998)
	.align 4

LL(05):
	srawi.	r0, N, 5
	sub	X, X, INCX2
	mtspr	CTR,  r0
	beq-	LL(15)

	LFPDUX	A1,    X, INCX2
	LFPDUX	A2,    X, INCX2
	LFPDUX	A3,    X, INCX2
	LFPDUX	A4,    X, INCX2
	LFPDUX	A5,    X, INCX2
	LFPDUX	A6,    X, INCX2
	LFPDUX	A7,    X, INCX2
	LFPDUX	A8,    X, INCX2

	LFPDUX	A9,    X, INCX2
	LFPDUX	A10,   X, INCX2
	LFPDUX	A11,   X, INCX2
	LFPDUX	A12,   X, INCX2
	LFPDUX	A13,   X, INCX2
	LFPDUX	A14,   X, INCX2
	LFPDUX	A15,   X, INCX2
	LFPDUX	A16,   X, INCX2
	bdz	LL(13)
	.align 4

LL(12):
	fpmadd	C1, A1,  A1,  C1
	LFPDUX	A1,    X, INCX2
	fpmadd	C2, A2,  A2,  C2
	LFPDUX	A2,    X, INCX2
	fpmadd	C3, A3,  A3,  C3
	LFPDUX	A3,    X, INCX2
	fpmadd	C4, A4,  A4,  C4
	LFPDUX	A4,    X, INCX2

	fpmadd	C5, A5,  A5,  C5
	LFPDUX	A5,    X, INCX2
	fpmadd	C6, A6,  A6,  C6
	LFPDUX	A6,    X, INCX2
	fpmadd	C7, A7,  A7,  C7
	LFPDUX	A7,    X, INCX2
	fpmadd	C8, A8,  A8,  C8
	LFPDUX	A8,    X, INCX2

	fpmadd	C1, A9,  A9,  C1
	LFPDUX	A9,    X, INCX2
	fpmadd	C2, A10, A10, C2
	LFPDUX	A10,   X, INCX2
	fpmadd	C3, A11, A11, C3
	LFPDUX	A11,   X, INCX2
	fpmadd	C4, A12, A12, C4
	LFPDUX	A12,   X, INCX2

	fpmadd	C5, A13, A13, C5
	LFPDUX	A13,   X, INCX2
	fpmadd	C6, A14, A14, C6
	LFPDUX	A14,   X, INCX2
	fpmadd	C7, A15, A15, C7
	LFPDUX	A15,   X, INCX2
	fpmadd	C8, A16, A16, C8
	LFPDUX	A16,   X, INCX2

	bdnz	LL(12)
	.align 4

LL(13):
	fpmadd	C1, A1,  A1,  C1
	fpmadd	C2, A2,  A2,  C2
	fpmadd	C3, A3,  A3,  C3
	fpmadd	C4, A4,  A4,  C4

	fpmadd	C5, A5,  A5,  C5
	fpmadd	C6, A6,  A6,  C6
	fpmadd	C7, A7,  A7,  C7
	fpmadd	C8, A8,  A8,  C8

	fpmadd	C1, A9,  A9,  C1
	fpmadd	C2, A10, A10, C2
	fpmadd	C3, A11, A11, C3
	fpmadd	C4, A12, A12, C4

	fpmadd	C5, A13, A13, C5
	fpmadd	C6, A14, A14, C6
	fpmadd	C7, A15, A15, C7
	fpmadd	C8, A16, A16, C8
	.align 4

LL(15):
	andi.	r0,  N, 31
	beq	LL(98)

	andi.	r0,  N, 16
	beq	LL(16)

	LFPDUX	A1,   X, INCX2
	LFPDUX	A2,   X, INCX2
	LFPDUX	A3,   X, INCX2
	LFPDUX	A4,   X, INCX2
	LFPDUX	A5,   X, INCX2
	LFPDUX	A6,   X, INCX2
	LFPDUX	A7,   X, INCX2
	LFPDUX	A8,   X, INCX2

	fpmadd	C1, A1, A1, C1
	fpmadd	C2, A2, A2, C2
	fpmadd	C3, A3, A3, C3
	fpmadd	C4, A4, A4, C4

	fpmadd	C5, A5, A5, C5
	fpmadd	C6, A6, A6, C6
	fpmadd	C7, A7, A7, C7
	fpmadd	C8, A8, A8, C8
	.align 4

LL(16):
	andi.	r0,  N, 8
	beq	LL(17)

	LFPDUX	A1,    X, INCX2
	LFPDUX	A2,    X, INCX2
	LFPDUX	A3,    X, INCX2
	LFPDUX	A4,    X, INCX2

	fpmadd	C1, A1, A1, C1
	fpmadd	C2, A2, A2, C2
	fpmadd	C3, A3, A3, C3
	fpmadd	C4, A4, A4, C4
	.align 4

LL(17):
	andi.	r0,  N, 4
	beq	LL(18)

	LFPDUX	A1,    X, INCX2
	LFPDUX	A2,    X, INCX2
	fpmadd	C1, A1, A1, C1
	fpmadd	C2, A2, A2, C2
	.align 4

LL(18):
	andi.	r0,  N, 2
	beq	LL(19)

	LFPDUX	A1,    X, INCX2
	fpmadd	C3, A1, A1, C3
	.align 4

LL(19):
	andi.	r0,  N, 1
	beq	LL(98)

	LFDX	A1,    X, INCX2
	fmadd	C4, A1, A1, C4
	.align 4

LL(98):
	fpadd	C1,  C1,  C5
	lis	r3, 0x3f00
	fpadd	C2,  C2,  C6
	lis	r4, 0x4040
	fpadd	C3,  C3,  C7
	stw	r3, 4(SP)
	fpadd	C4,  C4,  C8
	stw	r4, 8(SP)

	fpadd	C1,  C1,  C2
	fpadd	C3,  C3,  C4
	lfs	f10, 4(SP)

	fpadd	C1,  C1,  C3
	lfs	f11, 4(SP)
	lfs	f12, 8(SP)

	fsmtp	C2, C1
	fadd	C1, C2, C1

	fcmpu	cr0, f10, C1
	beq	cr0, LL(99)

#ifndef HUMMER_EMULATOR
	frsqrte f9, f1
	li	r10, 16

	fmul	f2,  f1,  f9
	lfpdux	f23, SP, r10
	fmul	f3,  f9,  f11
	lfpdux	f22, SP, r10
	fnmsub	f4,  f2,  f9, f12
	lfpdux	f21, SP, r10
	fmul	f9,  f3,  f4
	lfpdux	f20, SP, r10
	fadd	f13, f11, f11
	lfpdux	f19, SP, r10
	fmul	f12, f1,  f9
	lfpdux	f18, SP, r10
	fmul	f11, f12, f11
	lfpdux	f17, SP, r10
	fnmsub	f1,  f12, f9, f13
	lfpdux	f16, SP, r10
	lfpdux	f15, SP, r10
	lfpdux	f14, SP, r10
	addi	SP, SP,  16
	fmadd	f1,  f11, f1, f12
	blr
#else
	fsqrt	f1, f1
	li	r10, 16

	lfpdux	f23, SP, r10
	lfpdux	f22, SP, r10
	lfpdux	f21, SP, r10
	lfpdux	f20, SP, r10
	lfpdux	f19, SP, r10
	lfpdux	f18, SP, r10
	lfpdux	f17, SP, r10
	lfpdux	f16, SP, r10
	lfpdux	f15, SP, r10
	lfpdux	f14, SP, r10
	addi	SP, SP,  16
	blr
#endif

	.align 4

LL(99):
	li	r10, 16
	lfpdux	f23, SP, r10
	lfpdux	f22, SP, r10
	lfpdux	f21, SP, r10
	lfpdux	f20, SP, r10
	lfpdux	f19, SP, r10
	lfpdux	f18, SP, r10
	lfpdux	f17, SP, r10
	lfpdux	f16, SP, r10
	lfpdux	f15, SP, r10
	lfpdux	f14, SP, r10
	addi	SP, SP,  16
	blr
	.align 4

LL(100):
	sub	X2, X, INCX
	sub	X,  X, INCX2

	srawi.	r0, N, 4
	mtspr	CTR,  r0
	beq-	LL(115)


	LFDUX	A1,    X,  INCX2
	LFDUX	A2,    X2, INCX2
	LFDUX	A3,    X,  INCX2
	LFDUX	A4,    X2, INCX2

	LFDUX	A5,    X,  INCX2
	LFDUX	A6,    X2, INCX2
	LFDUX	A7,    X,  INCX2
	LFDUX	A8,    X2, INCX2

	LFDUX	A9,    X,  INCX2
	LFDUX	A10,   X2, INCX2
	LFDUX	A11,   X,  INCX2
	LFDUX	A12,   X2, INCX2

	LFDUX	A13,   X,  INCX2
	LFDUX	A14,   X2, INCX2
	LFDUX	A15,   X,  INCX2
	LFDUX	A16,   X2, INCX2
	bdz	LL(113)
	.align 4

LL(112):
	fmadd	C1, A1, A1, C1
	LFDUX	A1,    X,  INCX2
	fmadd	C2, A2, A2, C2
	LFDUX	A2,    X2, INCX2
	fmadd	C3, A3, A3, C3
	LFDUX	A3,    X,  INCX2
	fmadd	C4, A4, A4, C4
	LFDUX	A4,    X2, INCX2

	fmadd	C5, A5, A5, C5
	LFDUX	A5,    X,  INCX2
	fmadd	C6, A6, A6, C6
	LFDUX	A6,    X2, INCX2
	fmadd	C7, A7, A7, C7
	LFDUX	A7,    X,  INCX2
	fmadd	C8, A8, A8, C8
	LFDUX	A8,    X2, INCX2

	fmadd	C1, A9,  A9,  C1
	LFDUX	A9,    X,  INCX2
	fmadd	C2, A10, A10, C2
	LFDUX	A10,   X2, INCX2
	fmadd	C3, A11, A11, C3
	LFDUX	A11,   X,  INCX2
	fmadd	C4, A12, A12, C4
	LFDUX	A12,   X2, INCX2

	fmadd	C5, A13, A13, C5
	LFDUX	A13,   X,  INCX2
	fmadd	C6, A14, A14, C6
	LFDUX	A14,   X2, INCX2
	fmadd	C7, A15, A15, C7
	LFDUX	A15,   X,  INCX2
	fmadd	C8, A16, A16, C8
	LFDUX	A16,   X2, INCX2

	bdnz	LL(112)
	.align 4

LL(113):
	fmadd	C1, A1, A1, C1
	fmadd	C2, A2, A2, C2
	fmadd	C3, A3, A3, C3
	fmadd	C4, A4, A4, C4

	fmadd	C5, A5, A5, C5
	fmadd	C6, A6, A6, C6
	fmadd	C7, A7, A7, C7
	fmadd	C8, A8, A8, C8

	fmadd	C1, A9,  A9,  C1
	fmadd	C2, A10, A10, C2
	fmadd	C3, A11, A11, C3
	fmadd	C4, A12, A12, C4

	fmadd	C5, A13, A13, C5
	fmadd	C6, A14, A14, C6
	fmadd	C7, A15, A15, C7
	fmadd	C8, A16, A16, C8
	.align 4

LL(115):
	andi.	r0,  N, 15
	beq	LL(998)
	andi.	r0,  N, 8
	beq	LL(116)

	LFDUX	A1,    X,  INCX2
	LFDUX	A2,    X2, INCX2
	LFDUX	A3,    X,  INCX2
	LFDUX	A4,    X2, INCX2

	LFDUX	A5,    X,  INCX2
	LFDUX	A6,    X2, INCX2
	LFDUX	A7,    X,  INCX2
	LFDUX	A8,    X2, INCX2

	fmadd	C1, A1, A1, C1
	fmadd	C2, A2, A2, C2
	fmadd	C3, A3, A3, C3
	fmadd	C4, A4, A4, C4

	fmadd	C5, A5, A5, C5
	fmadd	C6, A6, A6, C6
	fmadd	C7, A7, A7, C7
	fmadd	C8, A8, A8, C8
	.align 4

LL(116):
	andi.	r0,  N, 4
	beq	LL(117)

	LFDUX	A1,    X,  INCX2
	LFDUX	A2,    X2, INCX2
	LFDUX	A3,    X,  INCX2
	LFDUX	A4,    X2, INCX2

	fmadd	C1, A1, A1, C1
	fmadd	C2, A2, A2, C2
	fmadd	C3, A3, A3, C3
	fmadd	C4, A4, A4, C4
	.align 4

LL(117):
	andi.	r0,  N, 2
	beq	LL(118)

	LFDUX	A1,    X,  INCX2
	LFDUX	A2,    X2, INCX2

	fmadd	C1, A1, A1, C1
	fmadd	C2, A2, A2, C2
	.align 4

LL(118):
	andi.	r0,  N, 1
	beq	LL(998)

	LFDX	A1,    X, INCX2
	fmadd	C1, A1, A1, C1
	.align 4

LL(998):
	fadd	C1,  C1,  C5
	lis	r3, 0x3f00
	fadd	C2,  C2,  C6
	lis	r4, 0x4040
	fadd	C3,  C3,  C7
	stw	r3, 4(SP)
	fadd	C4,  C4,  C8
	stw	r4, 8(SP)

	fadd	C1,  C1,  C2
	lfs	f10, 0(SP)
	fadd	C3,  C3,  C4
	lfs	f11, 4(SP)
	lfs	f12, 8(SP)

	fadd	C1,  C1,  C3

	fcmpu	cr0, f10, C1
	beq	cr0, LL(999)

#ifndef HUMMER_EMULATOR
	frsqrte	f9, f1
	li	r10, 16

	fmul	f2,  f1,  f9
	lfpdux	f23, SP, r10
	fmul	f3,  f9,  f11
	lfpdux	f22, SP, r10
	fnmsub	f4,  f2,  f9, f12
	lfpdux	f21, SP, r10
	fmul	f9,  f3,  f4
	lfpdux	f20, SP, r10
	fadd	f13, f11, f11
	lfpdux	f19, SP, r10
	fmul	f12, f1,  f9
	lfpdux	f18, SP, r10
	fmul	f11, f12, f11
	lfpdux	f17, SP, r10
	fnmsub	f1,  f12, f9, f13
	lfpdux	f16, SP, r10
	lfpdux	f15, SP, r10
	lfpdux	f14, SP, r10
	addi	SP, SP,  16
	fmadd	f1,  f11, f1, f12
	blr
#else
	fsqrt	f1, f1

	li	r10, 16
	lfpdux	f23, SP, r10
	lfpdux	f22, SP, r10
	lfpdux	f21, SP, r10
	lfpdux	f20, SP, r10
	lfpdux	f19, SP, r10
	lfpdux	f18, SP, r10
	lfpdux	f17, SP, r10
	lfpdux	f16, SP, r10
	lfpdux	f15, SP, r10
	lfpdux	f14, SP, r10
	addi	SP, SP,  16
	blr
#endif
	.align 4

LL(999):
	li	r10, 16

	lfpdux	f23, SP, r10
	lfpdux	f22, SP, r10
	lfpdux	f21, SP, r10
	lfpdux	f20, SP, r10
	lfpdux	f19, SP, r10
	lfpdux	f18, SP, r10
	lfpdux	f17, SP, r10
	lfpdux	f16, SP, r10
	lfpdux	f15, SP, r10
	lfpdux	f14, SP, r10
	addi	SP, SP,  16
	blr

	EPILOGUE
