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

#define N	%i0
#define X	%i1
#define INCX	%i2
#define I	%i3

#ifdef DOUBLE
#define c1	%f0
#define c2	%f2
#define c3	%f4
#define c4	%f6
#define t1	%f8
#define t2	%f10
#define t3	%f12
#define t4	%f14

#define a1	%f16
#define a2	%f18
#define a3	%f20
#define a4	%f22
#define a5	%f24
#define a6	%f26
#define a7	%f28
#define a8	%f30
#else
#define c1	%f0
#define c2	%f1
#define c3	%f2
#define c4	%f3
#define t1	%f4
#define t2	%f5
#define t3	%f6
#define t4	%f7

#define a1	%f8
#define a2	%f9
#define a3	%f10
#define a4	%f11
#define a5	%f12
#define a6	%f13
#define a7	%f14
#define a8	%f15
#endif

#ifndef USE_MIN
#define FCMOV	FMOVG
#else
#define FCMOV	FMOVL
#endif

	PROLOGUE
	SAVESP

	FCLR(0)

	cmp	N, 0
	ble	.LL20
	nop

	cmp	INCX, 0
	ble	.LL20
	sll	INCX, BASE_SHIFT, INCX

	add	N, -1, N
	LDF	[X], c1
	add	X, INCX, X
	cmp	N, 0
	ble	.LL20
	nop

	FMOV	c1, c2
	FMOV	c1, c3
	FMOV	c1, c4

	cmp	INCX, SIZE
	bne	.LL50
	nop

	sra	N, 3, I
	cmp	I, 0
	ble,pn	%icc, .LL15
	nop

	LDF	[X +  0 * SIZE], a1
	LDF	[X +  1 * SIZE], a2
	LDF	[X +  2 * SIZE], a3
	LDF	[X +  3 * SIZE], a4

	LDF	[X +  4 * SIZE], a5
	LDF	[X +  5 * SIZE], a6
	LDF	[X +  6 * SIZE], a7
	LDF	[X +  7 * SIZE], a8
	add	X, 8 * SIZE, X

	add	I, -1, I
	cmp	I, 0
	ble,pt	%icc, .LL12
	nop	

#define PREFETCHSIZE 40

.LL11:
	FCMP	%fcc0, a1, c1
	FCMP	%fcc1, a2, c2
	FCMP	%fcc2, a3, c3
	FCMP	%fcc3, a4, c4

	FCMOV	%fcc0, a1, c1
	LDF	[X +  0 * SIZE], a1
	FCMOV	%fcc1, a2, c2
	LDF	[X +  1 * SIZE], a2
	FCMOV	%fcc2, a3, c3
	LDF	[X +  2 * SIZE], a3
	FCMOV	%fcc3, a4, c4
	LDF	[X +  3 * SIZE], a4

	FCMP	%fcc0, a5, c1
	FCMP	%fcc1, a6, c2
	FCMP	%fcc2, a7, c3
	FCMP	%fcc3, a8, c4

	FCMOV	%fcc0, a5, c1
	LDF	[X +  4 * SIZE], a5
	add	I, -1, I
	FCMOV	%fcc1, a6, c2
	LDF	[X +  5 * SIZE], a6
	cmp	I, 0
	FCMOV	%fcc2, a7, c3
	LDF	[X +  6 * SIZE], a7
	FCMOV	%fcc3, a8, c4
	LDF	[X +  7 * SIZE], a8

	bg,pt	%icc, .LL11
	add	X, 8 * SIZE, X

.LL12:
	FCMP	%fcc0, a1, c1
	FCMP	%fcc1, a2, c2
	FCMP	%fcc2, a3, c3
	FCMP	%fcc3, a4, c4

	FCMOV	%fcc0, a1, c1
	FCMOV	%fcc1, a2, c2
	FCMOV	%fcc2, a3, c3
	FCMOV	%fcc3, a4, c4

	FCMP	%fcc0, a5, c1
	FCMP	%fcc1, a6, c2
	FCMP	%fcc2, a7, c3
	FCMP	%fcc3, a8, c4

	FCMOV	%fcc0, a5, c1
	FCMOV	%fcc1, a6, c2
	FCMOV	%fcc2, a7, c3
	FCMOV	%fcc3, a8, c4

.LL15:
	and	N, 7, I
	cmp	I,  0
	ble,a,pn %icc, .LL19
	nop

.LL16:
	LDF	[X +  0 * SIZE], a1
	FCMP	%fcc0, a1, c1
	FCMOV	%fcc0, a1, c1
	add	I, -1, I
	cmp	I, 0
	bg,pt	%icc, .LL16
	add	X, 1 * SIZE, X

.LL19:
	FCMP	%fcc0, c2, c1
	FCMP	%fcc1, c4, c3
	FCMOV	%fcc0, c2, c1
	FCMOV	%fcc1, c4, c3
	FCMP	%fcc0, c3, c1
	FCMOV	%fcc0, c3, c1

.LL20:
	return	%i7 + 8
	clr	%g0

.LL50:
	sra	N, 3, I
	cmp	I, 0
	ble,pn	%icc, .LL55
	nop

	LDF	[X +  0 * SIZE], a1
	add	X, INCX, X
	LDF	[X +  0 * SIZE], a2
	add	X, INCX, X
	LDF	[X +  0 * SIZE], a3
	add	X, INCX, X
	LDF	[X +  0 * SIZE], a4
	add	X, INCX, X
	LDF	[X +  0 * SIZE], a5
	add	X, INCX, X
	LDF	[X +  0 * SIZE], a6
	add	X, INCX, X
	add	I, -1, I
	LDF	[X +  0 * SIZE], a7
	cmp	I, 0
	add	X, INCX, X
	LDF	[X +  0 * SIZE], a8

	ble,pt	%icc, .LL52
	add	X, INCX, X

.LL51:
	FCMP	%fcc0, a1, c1
	FCMP	%fcc1, a2, c2
	FCMP	%fcc2, a3, c3
	FCMP	%fcc3, a4, c4

	FCMOV	%fcc0, a1, c1
	LDF	[X +  0 * SIZE], a1
	add	X, INCX, X
	FCMOV	%fcc1, a2, c2
	LDF	[X +  0 * SIZE], a2
	add	X, INCX, X
	FCMOV	%fcc2, a3, c3
	LDF	[X +  0 * SIZE], a3
	add	X, INCX, X
	FCMOV	%fcc3, a4, c4
	LDF	[X +  0 * SIZE], a4
	add	X, INCX, X

	FCMP	%fcc0, a5, c1
	add	I, -1, I
	FCMP	%fcc1, a6, c2
	cmp	I, 0
	FCMP	%fcc2, a7, c3
	FCMP	%fcc3, a8, c4

	FCMOV	%fcc0, a5, c1
	LDF	[X +  0 * SIZE], a5
	add	X, INCX, X
	FCMOV	%fcc1, a6, c2
	LDF	[X +  0 * SIZE], a6
	add	X, INCX, X
	FCMOV	%fcc2, a7, c3
	LDF	[X +  0 * SIZE], a7
	add	X, INCX, X
	FCMOV	%fcc3, a8, c4
	LDF	[X +  0 * SIZE], a8

	bg,pt	%icc, .LL51
	add	X, INCX, X

.LL52:
	FCMP	%fcc0, a1, c1
	FCMP	%fcc1, a2, c2
	FCMP	%fcc2, a3, c3
	FCMP	%fcc3, a4, c4

	FCMOV	%fcc0, a1, c1
	FCMOV	%fcc1, a2, c2
	FCMOV	%fcc2, a3, c3
	FCMOV	%fcc3, a4, c4

	FCMP	%fcc0, a5, c1
	FCMP	%fcc1, a6, c2
	FCMP	%fcc2, a7, c3
	FCMP	%fcc3, a8, c4

	FCMOV	%fcc0, a5, c1
	FCMOV	%fcc1, a6, c2
	FCMOV	%fcc2, a7, c3
	FCMOV	%fcc3, a8, c4

.LL55:
	and	N, 7, I
	cmp	I,  0
	ble,a,pn %icc, .LL59
	nop

.LL56:
	LDF	[X +  0 * SIZE], a1
	FCMP	%fcc0, a1, c1
	FCMOV	%fcc0, a1, c1
	add	I, -1, I
	cmp	I, 0
	bg,pt	%icc, .LL56
	add	X, INCX, X

.LL59:
	FCMP	%fcc0, c2, c1
	FCMP	%fcc1, c4, c3
	FCMOV	%fcc0, c2, c1
	FCMOV	%fcc1, c4, c3
	FCMP	%fcc0, c3, c1
	FCMOV	%fcc0, c3, c1

	return	%i7 + 8
	clr	%o0

	EPILOGUE
