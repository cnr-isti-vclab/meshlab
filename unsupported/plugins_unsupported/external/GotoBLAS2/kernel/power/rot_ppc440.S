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
#define PRE	r8
#define	XX	r9
#define	YY	r10

#define C	f1
#define S	f2

#define STACKSIZE 32

	PROLOGUE
	PROFCODE

	addi	SP, SP, -STACKSIZE
	li	r0,   0

	stfd	f14,    0(SP)
	stfd	f15,    8(SP)
	stfd	f16,   16(SP)
	stfd	f17,   24(SP)

	slwi	INCX, INCX, BASE_SHIFT
	slwi	INCY, INCY, BASE_SHIFT

	li	PRE, 2 * 16 * SIZE

	cmpwi	cr0, N, 0
	ble-	LL(999)

	sub	X, X, INCX
	sub	Y, Y, INCY

	mr	XX, X
	mr	YY, Y

	srawi.	r0, N, 3
	mtspr	CTR,  r0
	beq-	LL(150)

	LFDUX	f0,    X,  INCX
	LFDUX	f3,    Y,  INCY
	LFDUX	f4,    X,  INCX

	FMUL	f10, C, f0
	LFDUX	f5,    Y,  INCY
	FMUL	f11, C, f3
	LFDUX	f6,    X,  INCX
	FMUL	f12, C, f4
	LFDUX	f7,    Y,  INCY
	FMUL	f13, C, f5
	LFDUX	f8,    X,  INCX

	FMADD	f10, S, f3, f10
	LFDUX	f9,    Y,  INCY
	FNMSUB	f11, S, f0, f11
	LFDUX	f0,    X,  INCX
	FMADD	f12, S, f5, f12
	LFDUX	f3,    Y,  INCY
	FNMSUB	f13, S, f4, f13
	LFDUX	f4,    X,  INCX

	bdz	LL(111)
	.align 4

LL(110):
	FMUL	f14, C, f6
	LFDUX	f5,    Y,  INCY
	FMUL	f15, C, f7
	STFDUX	f10,    XX, INCX
	FMUL	f16, C, f8
	STFDUX	f11,    YY, INCY
	FMUL	f17, C, f9
	STFDUX	f12,    XX, INCX

#ifdef PPCG4
	dcbtst	X, PRE
#endif

	FMADD	f14, S, f7, f14
	STFDUX	f13,    YY, INCY
	FNMSUB	f15, S, f6, f15
	LFDUX	f6,    X,  INCX
	FMADD	f16, S, f9, f16
	LFDUX	f7,    Y,  INCY
	FNMSUB	f17, S, f8, f17
	LFDUX	f8,    X,  INCX

	FMUL	f10, C, f0
	LFDUX	f9,    Y,  INCY
	FMUL	f11, C, f3
	STFDUX	f14,    XX, INCX
	FMUL	f12, C, f4
	STFDUX	f15,    YY, INCY
	FMUL	f13, C, f5
	STFDUX	f16,    XX, INCX

#ifdef PPCG4
	dcbtst	Y, PRE
#endif

	FMADD	f10, S, f3, f10
	STFDUX	f17,    YY, INCY
	FNMSUB	f11, S, f0, f11
	LFDUX	f0,    X,  INCX
	FMADD	f12, S, f5, f12
	LFDUX	f3,    Y,  INCY
	FNMSUB	f13, S, f4, f13
	LFDUX	f4,    X,  INCX

	FMUL	f14, C, f6
	LFDUX	f5,    Y,  INCY
	FMUL	f15, C, f7
	STFDUX	f10,    XX, INCX
	FMUL	f16, C, f8
	STFDUX	f11,    YY, INCY
	FMUL	f17, C, f9
	STFDUX	f12,    XX, INCX

#if defined(PPCG4) && defined(DOUBLE)
	dcbt	X, PRE
#endif

	FMADD	f14, S, f7, f14
	STFDUX	f13,    YY, INCY
	FNMSUB	f15, S, f6, f15
	LFDUX	f6,    X,  INCX
	FMADD	f16, S, f9, f16
	LFDUX	f7,    Y,  INCY
	FNMSUB	f17, S, f8, f17
	LFDUX	f8,    X,  INCX

	FMUL	f10, C, f0
	LFDUX	f9,    Y,  INCY
	FMUL	f11, C, f3
	STFDUX	f14,    XX, INCX
	FMUL	f12, C, f4
	STFDUX	f15,    YY, INCY
	FMUL	f13, C, f5
	STFDUX	f16,    XX, INCX

#if defined(PPCG4) && defined(DOUBLE)
	dcbtst	Y, PRE
#endif

	FMADD	f10, S, f3, f10
	STFDUX	f17,    YY, INCY
	FNMSUB	f11, S, f0, f11
	LFDUX	f0,    X,  INCX
	FMADD	f12, S, f5, f12
	LFDUX	f3,    Y,  INCY
	FNMSUB	f13, S, f4, f13
	LFDUX	f4,    X,  INCX

	bdnz	LL(110)
	.align 4

LL(111):
	FMUL	f14, C, f6
	LFDUX	f5,    Y,  INCY
	FMUL	f15, C, f7
	STFDUX	f10,    XX, INCX
	FMUL	f16, C, f8
	STFDUX	f11,    YY, INCY
	FMUL	f17, C, f9
	STFDUX	f12,    XX, INCX

	FMADD	f14, S, f7, f14
	STFDUX	f13,    YY, INCY
	FNMSUB	f15, S, f6, f15
	LFDUX	f6,    X,  INCX
	FMADD	f16, S, f9, f16
	LFDUX	f7,    Y,  INCY
	FNMSUB	f17, S, f8, f17
	LFDUX	f8,    X,  INCX

	FMUL	f10, C, f0
	LFDUX	f9,    Y,  INCY
	FMUL	f11, C, f3
	STFDUX	f14,    XX, INCX
	FMUL	f12, C, f4
	STFDUX	f15,    YY, INCY
	FMUL	f13, C, f5
	STFDUX	f16,    XX, INCX

	FMUL	f14, C, f6
	STFDUX	f17,    YY, INCY
	FMUL	f15, C, f7
	FMUL	f16, C, f8
	FMUL	f17, C, f9

	FMADD	f10, S, f3, f10
	FNMSUB	f11, S, f0, f11
	FMADD	f12, S, f5, f12
	FNMSUB	f13, S, f4, f13

	FMADD	f14, S, f7, f14
	STFDUX	f10,    XX, INCX
	FNMSUB	f15, S, f6, f15
	STFDUX	f11,    YY, INCY
	FMADD	f16, S, f9, f16
	STFDUX	f12,    XX, INCX
	FNMSUB	f17, S, f8, f17
	STFDUX	f13,    YY, INCY

	STFDUX	f14,    XX, INCX
	STFDUX	f15,    YY, INCY
	STFDUX	f16,    XX, INCX
	STFDUX	f17,    YY, INCY
	.align 4

LL(150):
	andi.	r0,  N, 7
	mtspr	CTR, r0
	beq	LL(999)
	.align 4

LL(160):
	LFDUX	f0,    X,  INCX
	LFDUX	f3,    Y,  INCY

	FMUL	f10, C, f0
	FMUL	f11, C, f3

	FMADD	f10, S, f3, f10
	FNMSUB	f11, S, f0, f11

	STFDUX	f10,    XX, INCX
	STFDUX	f11,    YY, INCY
	bdnz	LL(160)
	.align 4

LL(999):
	lfd	f14,    0(SP)
	lfd	f15,    8(SP)
	lfd	f16,   16(SP)
	lfd	f17,   24(SP)

	addi	SP, SP, STACKSIZE
	blr

	EPILOGUE
