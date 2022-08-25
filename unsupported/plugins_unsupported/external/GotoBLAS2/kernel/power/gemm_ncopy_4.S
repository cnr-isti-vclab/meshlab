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

#define STACKSIZE 32

#ifdef CELL
#define PREFETCHSIZE   16
#define PREFETCHWSIZE  72
#endif

#ifdef PPC970
#define PREFETCHSIZE   16
#define PREFETCHWSIZE  72
#endif

#ifdef PPC440
#define PREFETCHSIZE   16
#define PREFETCHWSIZE  72
#endif

#ifdef POWER4
#define PREFETCHSIZE   16
#define PREFETCHWSIZE  72
#endif

#ifdef POWER5
#define PREFETCHSIZE   16
#define PREFETCHWSIZE  72
#endif

#ifdef POWER6
#define PREFETCHSIZE   16
#define PREFETCHWSIZE  72
#endif

#ifdef PPCG4
#define PREFETCHSIZE   16
#define PREFETCHWSIZE  72
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
#else
	stw	r14,   16(SP)
	stw	r15,   20(SP)
#endif

	slwi	LDA, LDA, BASE_SHIFT

	li	PREA,  PREFETCHSIZE * SIZE
	li	PREB1, (PREFETCHWSIZE +  0) * SIZE

	cmpwi	cr0, M, 0
	ble-	LL(999)
	cmpwi	cr0, N, 0
	ble-	LL(999)

	srawi.	J,  N,  2
	ble	LL(20)
	.align 4

LL(10):
	mr	AO1, A
	add	AO2, A,   LDA
	add	AO3, AO2, LDA
	add	AO4, AO3, LDA
	add	A,   AO4, LDA

	srawi.	r0,  M,  2
	mtspr	CTR, r0
	ble	LL(15)
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

	STFD	c01,   0 * SIZE(B)
	STFD	c05,   1 * SIZE(B)
	STFD	c09,   2 * SIZE(B)
	STFD	c13,   3 * SIZE(B)

	STFD	c02,   4 * SIZE(B)
	STFD	c06,   5 * SIZE(B)
	STFD	c10,   6 * SIZE(B)
	STFD	c14,   7 * SIZE(B)

	STFD	c03,   8 * SIZE(B)
	STFD	c07,   9 * SIZE(B)
	STFD	c11,  10 * SIZE(B)
	STFD	c15,  11 * SIZE(B)

	STFD	c04,  12 * SIZE(B)
	STFD	c08,  13 * SIZE(B)
	STFD	c12,  14 * SIZE(B)
	STFD	c16,  15 * SIZE(B)

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
	addi	B,   B,   16 * SIZE
	bdnz	LL(12)
	.align 4
	
LL(15):
	andi.	r0,  M,  3
	mtspr	CTR, r0
	ble	LL(17)
	.align 4

LL(16):
	LFD	c01,   0 * SIZE(AO1)
	LFD	c05,   0 * SIZE(AO2)
	LFD	c09,   0 * SIZE(AO3)
	LFD	c13,   0 * SIZE(AO4)

	STFD	c01,   0 * SIZE(B)
	STFD	c05,   1 * SIZE(B)
	STFD	c09,   2 * SIZE(B)
	STFD	c13,   3 * SIZE(B)

	addi	AO1, AO1,  1 * SIZE
	addi	AO2, AO2,  1 * SIZE
	addi	AO3, AO3,  1 * SIZE
	addi	AO4, AO4,  1 * SIZE
	addi	B,   B,    4 * SIZE
	bdnz	LL(16)
	.align 4

LL(17):
	addic.	J, J, -1
	bgt	LL(10)
	.align 4

LL(20):
	andi.	J,  N,  2
	ble	LL(30)

	mr	AO1, A
	add	AO2, A,   LDA
	add	A,   AO2, LDA

	srawi.	r0,  M,  2
	mtspr	CTR, r0
	ble	LL(25)
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

	STFD	c01,   0 * SIZE(B)
	STFD	c05,   1 * SIZE(B)
	STFD	c02,   2 * SIZE(B)
	STFD	c06,   3 * SIZE(B)

	STFD	c03,   4 * SIZE(B)
	STFD	c07,   5 * SIZE(B)
	STFD	c04,   6 * SIZE(B)
	STFD	c08,   7 * SIZE(B)

	addi	AO1, AO1,  4 * SIZE
	addi	AO2, AO2,  4 * SIZE
	addi	B,   B,    8 * SIZE
	bdnz	LL(22)
	.align 4
	
LL(25):
	andi.	r0,  M,  3
	mtspr	CTR, r0
	ble	LL(30)
	.align 4

LL(26):
	LFD	c01,   0 * SIZE(AO1)
	LFD	c05,   0 * SIZE(AO2)

	STFD	c01,   0 * SIZE(B)
	STFD	c05,   1 * SIZE(B)

	addi	AO1, AO1,  1 * SIZE
	addi	AO2, AO2,  1 * SIZE
	addi	B,   B,    2 * SIZE
	bdnz	LL(26)
	.align 4

LL(30):
	andi.	J,  N,  1
	ble	LL(999)

	mr	AO1, A

	srawi.	r0,  M,  2
	mtspr	CTR, r0
	ble	LL(35)
	.align 4

LL(32):
	LFD	c01,   0 * SIZE(AO1)
	LFD	c02,   1 * SIZE(AO1)
	LFD	c03,   2 * SIZE(AO1)
	LFD	c04,   3 * SIZE(AO1)

	STFD	c01,   0 * SIZE(B)
	STFD	c02,   1 * SIZE(B)
	STFD	c03,   2 * SIZE(B)
	STFD	c04,   3 * SIZE(B)

	addi	AO1, AO1,  4 * SIZE
	addi	B,   B,    4 * SIZE
	bdnz	LL(32)
	.align 4
	
LL(35):
	andi.	r0,  M,  3
	mtspr	CTR, r0
	ble	LL(999)
	.align 4

LL(36):
	LFD	c01,   0 * SIZE(AO1)

	STFD	c01,   0 * SIZE(B)

	addi	AO1, AO1,  1 * SIZE
	addi	B,   B,    1 * SIZE
	bdnz	LL(36)
	.align 4

LL(999):
	li	r3, 0

	lfd	f14,    0(SP)
	lfd	f15,    8(SP)

#ifdef __64BIT__
	ld	r14,   16(SP)
	ld	r15,   24(SP)
#else
	lwz	r14,   16(SP)
	lwz	r15,   20(SP)
#endif
	addi	SP, SP, STACKSIZE

	blr
	EPILOGUE
