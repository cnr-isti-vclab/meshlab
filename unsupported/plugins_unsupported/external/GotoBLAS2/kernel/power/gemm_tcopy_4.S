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
#define AO3	r10
#define AO4	r11

#define J	r12

#define PREA	r14
#define PREB1	r15
#define B1	r16
#define B2	r17
#define B3	r18
#define M4	r19
	
#define c01	f0
#define c02	f1
#define c03	f2
#define c04	f3
#define c05	f4
#define c06	f5
#define c07	f6
#define c08	f7
#define c09	f8
#define c10	f9
#define c11	f10
#define c12	f11
#define c13	f12
#define c14	f13
#define c15	f14
#define c16	f15

#define STACKSIZE 64

#ifdef CELL
#define PREFETCHSIZE   16
#define PREFETCHWSIZE  48
#endif

#ifdef PPC970
#define PREFETCHSIZE   16
#define PREFETCHWSIZE  48
#endif

#ifdef PPC440
#define PREFETCHSIZE   16
#define PREFETCHWSIZE  48
#endif

#ifdef POWER4
#define PREFETCHSIZE   16
#define PREFETCHWSIZE  48
#endif

#ifdef POWER5
#define PREFETCHSIZE   16
#define PREFETCHWSIZE  48
#endif

#ifdef POWER6
#define PREFETCHSIZE   16
#define PREFETCHWSIZE  48
#endif

#ifdef PPCG4
#define PREFETCHSIZE   16
#define PREFETCHWSIZE  48
#endif

	PROLOGUE
	PROFCODE

	addi	SP, SP, -STACKSIZE
	li	r0, 0

	stfd	f14,    0(SP)
	stfd	f15,    8(SP)

#ifdef __64BIT__
	std	r14,   16(SP)
	std	r15,   24(SP)
	std	r16,   32(SP)
	std	r17,   40(SP)
	std	r18,   48(SP)
	std	r19,   56(SP)
#else
	stw	r14,   16(SP)
	stw	r15,   20(SP)
	stw	r16,   24(SP)
	stw	r17,   28(SP)
	stw	r18,   32(SP)
	stw	r19,   36(SP)
#endif

	slwi	LDA, LDA, BASE_SHIFT
	slwi	M4, M, 2 + BASE_SHIFT

	li	PREA,  -4
	li	PREB1, -2

	and	B2, N, PREA
	and	B3, N, PREB1

	mullw	B2, B2, M
	mullw	B3, B3, M

	slwi	B2, B2, BASE_SHIFT
	slwi	B3, B3, BASE_SHIFT

	add	B2, B2, B
	add	B3, B3, B

	li	PREA,  PREFETCHSIZE * SIZE
	li	PREB1, (PREFETCHWSIZE +  0) * SIZE

	cmpwi	cr0, M, 0
	ble-	LL(999)
	cmpwi	cr0, N, 0
	ble-	LL(999)

	srawi.	J,  M,  2
	ble	LL(20)
	.align 4

LL(10):
	mr	AO1, A
	add	AO2, A,   LDA
	add	AO3, AO2, LDA
	add	AO4, AO3, LDA
	add	A,   AO4, LDA

	mr	B1, B
	addi	B, B, 16 * SIZE

	srawi.	r0,  N,  2
	mtspr	CTR, r0
	ble	LL(13)
	.align 4

LL(12):
	LFD	c01,   0 * SIZE(AO1)
	LFD	c02,   1 * SIZE(AO1)
	LFD	c03,   2 * SIZE(AO1)
	LFD	c04,   3 * SIZE(AO1)

	LFD	c05,   0 * SIZE(AO2)
	LFD	c06,   1 * SIZE(AO2)
	LFD	c07,   2 * SIZE(AO2)
	LFD	c08,   3 * SIZE(AO2)

	LFD	c09,   0 * SIZE(AO3)
	LFD	c10,   1 * SIZE(AO3)
	LFD	c11,   2 * SIZE(AO3)
	LFD	c12,   3 * SIZE(AO3)

	LFD	c13,   0 * SIZE(AO4)
	LFD	c14,   1 * SIZE(AO4)
	LFD	c15,   2 * SIZE(AO4)
	LFD	c16,   3 * SIZE(AO4)

	STFD	c01,   0 * SIZE(B1)
	STFD	c02,   1 * SIZE(B1)
	STFD	c03,   2 * SIZE(B1)
	STFD	c04,   3 * SIZE(B1)

	STFD	c05,   4 * SIZE(B1)
	STFD	c06,   5 * SIZE(B1)
	STFD	c07,   6 * SIZE(B1)
	STFD	c08,   7 * SIZE(B1)

	STFD	c09,   8 * SIZE(B1)
	STFD	c10,   9 * SIZE(B1)
	STFD	c11,  10 * SIZE(B1)
	STFD	c12,  11 * SIZE(B1)

	STFD	c13,  12 * SIZE(B1)
	STFD	c14,  13 * SIZE(B1)
	STFD	c15,  14 * SIZE(B1)
	STFD	c16,  15 * SIZE(B1)

#ifdef POWER6
	dcbtst	PREA, AO1
	dcbtst	PREA, AO2
	dcbtst	PREA, AO3
	dcbtst	PREA, AO4
#else
	dcbt	PREA, AO1
	dcbt	PREA, AO2
	dcbt	PREA, AO3
	dcbt	PREA, AO4
#endif
	
	dcbtst	PREB1, B

	addi	AO1, AO1,  4 * SIZE
	addi	AO2, AO2,  4 * SIZE
	addi	AO3, AO3,  4 * SIZE
	addi	AO4, AO4,  4 * SIZE
	add	B1,  B1,   M4
	bdnz	LL(12)
	.align 4
	
LL(13):
	andi.	r0,  N,  2
	ble	LL(14)

	LFD	c01,   0 * SIZE(AO1)
	LFD	c02,   1 * SIZE(AO1)
	LFD	c03,   0 * SIZE(AO2)
	LFD	c04,   1 * SIZE(AO2)

	LFD	c05,   0 * SIZE(AO3)
	LFD	c06,   1 * SIZE(AO3)
	LFD	c07,   0 * SIZE(AO4)
	LFD	c08,   1 * SIZE(AO4)

	STFD	c01,   0 * SIZE(B2)
	STFD	c02,   1 * SIZE(B2)
	STFD	c03,   2 * SIZE(B2)
	STFD	c04,   3 * SIZE(B2)

	STFD	c05,   4 * SIZE(B2)
	STFD	c06,   5 * SIZE(B2)
	STFD	c07,   6 * SIZE(B2)
	STFD	c08,   7 * SIZE(B2)

	addi	AO1, AO1,  2 * SIZE
	addi	AO2, AO2,  2 * SIZE
	addi	AO3, AO3,  2 * SIZE
	addi	AO4, AO4,  2 * SIZE
	addi	B2,  B2,   8 * SIZE
	.align 4

LL(14):
	andi.	r0,  N,  1
	ble	LL(17)

	LFD	c01,   0 * SIZE(AO1)
	LFD	c02,   0 * SIZE(AO2)
	LFD	c03,   0 * SIZE(AO3)
	LFD	c04,   0 * SIZE(AO4)

	STFD	c01,   0 * SIZE(B3)
	STFD	c02,   1 * SIZE(B3)
	STFD	c03,   2 * SIZE(B3)
	STFD	c04,   3 * SIZE(B3)

	addi	B3,  B3,  4 * SIZE
	.align 4

LL(17):
	addic.	J, J, -1
	bgt	LL(10)
	.align 4

LL(20):
	andi.	J,  M,  2
	ble	LL(30)

	mr	AO1, A
	add	AO2, A,   LDA
	add	A,   AO2, LDA

	mr	B1, B
	addi	B, B, 8 * SIZE

	srawi.	r0,  N,  2
	mtspr	CTR, r0
	ble	LL(23)
	.align 4

LL(22):
	LFD	c01,   0 * SIZE(AO1)
	LFD	c02,   1 * SIZE(AO1)
	LFD	c03,   2 * SIZE(AO1)
	LFD	c04,   3 * SIZE(AO1)

	LFD	c05,   0 * SIZE(AO2)
	LFD	c06,   1 * SIZE(AO2)
	LFD	c07,   2 * SIZE(AO2)
	LFD	c08,   3 * SIZE(AO2)

	STFD	c01,   0 * SIZE(B1)
	STFD	c02,   1 * SIZE(B1)
	STFD	c03,   2 * SIZE(B1)
	STFD	c04,   3 * SIZE(B1)

	STFD	c05,   4 * SIZE(B1)
	STFD	c06,   5 * SIZE(B1)
	STFD	c07,   6 * SIZE(B1)
	STFD	c08,   7 * SIZE(B1)

	addi	AO1, AO1,  4 * SIZE
	addi	AO2, AO2,  4 * SIZE
	add	B1,   B1, M4
	bdnz	LL(22)
	.align 4
	
LL(23):
	andi.	r0,  N,  2
	ble	LL(24)

	LFD	c01,   0 * SIZE(AO1)
	LFD	c02,   1 * SIZE(AO1)
	LFD	c03,   0 * SIZE(AO2)
	LFD	c04,   1 * SIZE(AO2)

	STFD	c01,   0 * SIZE(B2)
	STFD	c02,   1 * SIZE(B2)
	STFD	c03,   2 * SIZE(B2)
	STFD	c04,   3 * SIZE(B2)

	addi	AO1, AO1,  2 * SIZE
	addi	AO2, AO2,  2 * SIZE
	addi	B2,  B2,   4 * SIZE
	.align 4

LL(24):
	andi.	r0,  N,  1
	ble	LL(30)

	LFD	c01,   0 * SIZE(AO1)
	LFD	c02,   0 * SIZE(AO2)

	STFD	c01,   0 * SIZE(B3)
	STFD	c02,   1 * SIZE(B3)

	addi	B3,  B3,  2 * SIZE
	.align 4

LL(30):
	andi.	J,  M,  1
	ble	LL(999)

	mr	AO1, A

	mr	B1, B

	srawi.	r0,  N,  2
	mtspr	CTR, r0
	ble	LL(33)
	.align 4

LL(32):
	LFD	c01,   0 * SIZE(AO1)
	LFD	c02,   1 * SIZE(AO1)
	LFD	c03,   2 * SIZE(AO1)
	LFD	c04,   3 * SIZE(AO1)

	STFD	c01,   0 * SIZE(B1)
	STFD	c02,   1 * SIZE(B1)
	STFD	c03,   2 * SIZE(B1)
	STFD	c04,   3 * SIZE(B1)

	addi	AO1, AO1,  4 * SIZE
	add	B1,   B1,  M4
	bdnz	LL(32)
	.align 4
	
LL(33):
	andi.	r0,  N,  2
	ble	LL(34)

	LFD	c01,   0 * SIZE(AO1)
	LFD	c02,   1 * SIZE(AO1)

	STFD	c01,   0 * SIZE(B2)
	STFD	c02,   1 * SIZE(B2)

	addi	AO1, AO1,  2 * SIZE
	addi	B2,  B2,   2 * SIZE
	.align 4

LL(34):
	andi.	r0,  N,  1
	ble	LL(999)

	LFD	c01,   0 * SIZE(AO1)
	STFD	c01,   0 * SIZE(B3)
	.align 4

LL(999):
	li	r3, 0

	lfd	f14,    0(SP)
	lfd	f15,    8(SP)

#ifdef __64BIT__
	ld	r14,   16(SP)
	ld	r15,   24(SP)
	ld	r16,   32(SP)
	ld	r17,   40(SP)
	ld	r18,   48(SP)
	ld	r19,   56(SP)
#else
	lwz	r14,   16(SP)
	lwz	r15,   20(SP)
	lwz	r16,   24(SP)
	lwz	r17,   28(SP)
	lwz	r18,   32(SP)
	lwz	r19,   36(SP)
#endif
	addi	SP, SP, STACKSIZE

	blr
	EPILOGUE
