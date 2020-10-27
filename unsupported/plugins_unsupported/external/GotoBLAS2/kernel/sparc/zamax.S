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
#define t5	%f16
#define t6	%f18
#define t7	%f20
#define t8	%f22

#define a1	%f24
#define a2	%f26
#define a3	%f28
#define a4	%f30
#define a5	%f32
#define a6	%f34
#define a7	%f36
#define a8	%f38
#else
#define c1	%f0
#define c2	%f1
#define c3	%f2
#define c4	%f3
#define t1	%f4
#define t2	%f5
#define t3	%f6
#define t4	%f7
#define t5	%f8
#define t6	%f9
#define t7	%f10
#define t8	%f11

#define a1	%f12
#define a2	%f13
#define a3	%f14
#define a4	%f15
#define a5	%f16
#define a6	%f17
#define a7	%f18
#define a8	%f19
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
	sll	INCX, ZBASE_SHIFT, INCX

	LDF	[X + 0 * SIZE], c1
	LDF	[X + 1 * SIZE], c2
	add	N, -1, N
	FABS	c1, c1
	add	X, INCX, X
	FABS	c2, c2
	cmp	N, 0
	ble	.LL20
	FADD	c1, c2, c1

	FMOV	c1, c2
	FMOV	c1, c3
	FMOV	c1, c4

	cmp	INCX, 2 * SIZE
	bne	.LL50
	nop

	sra	N, 2, I
	cmp	I, 0
	ble,pn	%icc, .LL15
	nop

	LDF	[X +  0 * SIZE], a1
	LDF	[X +  1 * SIZE], a2
	LDF	[X +  2 * SIZE], a3
	LDF	[X +  3 * SIZE], a4

	LDF	[X +  4 * SIZE], a5
	add	I, -1, I
	LDF	[X +  5 * SIZE], a6
	cmp	I, 0
	LDF	[X +  6 * SIZE], a7
	LDF	[X +  7 * SIZE], a8

	ble,pt	%icc, .LL12
	add	X, 8 * SIZE, X

#define PREFETCHSIZE 40

.LL11:
	prefetch [X + PREFETCHSIZE * SIZE], 0

	FABS	a1, t1
	LDF	[X +  0 * SIZE], a1
	FABS	a2, t2
	LDF	[X +  1 * SIZE], a2
	FABS	a3, t3
	LDF	[X +  2 * SIZE], a3
	FABS	a4, t4
	LDF	[X +  3 * SIZE], a4

	FABS	a5, t5
	LDF	[X +  4 * SIZE], a5
	FABS	a6, t6
	LDF	[X +  5 * SIZE], a6
	FABS	a7, t7
	LDF	[X +  6 * SIZE], a7
	FABS	a8, t8
	LDF	[X +  7 * SIZE], a8

	FADD	t1, t2, t1
	FADD	t3, t4, t3
	FADD	t5, t6, t5
	FADD	t7, t8, t7

	FCMP	%fcc0, t1, c1
	FCMP	%fcc1, t3, c2
	FCMP	%fcc2, t5, c3
	FCMP	%fcc3, t7, c4

	FCMOV	%fcc0, t1, c1
	add	I, -1, I
	FCMOV	%fcc1, t3, c2
	cmp	I, 0
	FCMOV	%fcc2, t5, c3
	FCMOV	%fcc3, t7, c4

	bg,pt	%icc, .LL11
	add	X, 8 * SIZE, X

.LL12:
	FABS	a1, t1
	FABS	a2, t2
	FABS	a3, t3
	FABS	a4, t4

	FABS	a5, t5
	FABS	a6, t6
	FABS	a7, t7
	FABS	a8, t8

	FADD	t1, t2, t1
	FADD	t3, t4, t3
	FADD	t5, t6, t5
	FADD	t7, t8, t7

	FCMP	%fcc0, t1, c1
	FCMP	%fcc1, t3, c2
	FCMP	%fcc2, t5, c3
	FCMP	%fcc3, t7, c4

	FCMOV	%fcc0, t1, c1
	FCMOV	%fcc1, t3, c2
	FCMOV	%fcc2, t5, c3
	FCMOV	%fcc3, t7, c4

.LL15:
	and	N, 3, I
	cmp	I,  0
	ble,a,pn %icc, .LL19
	nop

.LL16:
	LDF	[X +  0 * SIZE], a1
	LDF	[X +  1 * SIZE], a2

	FABS	a1, t1
	FABS	a2, t2
	FADD	t1, t2, t1
	FCMP	%fcc0, t1, c1
	FCMOV	%fcc0, t1, c1
	add	I, -1, I
	cmp	I, 0
	bg,pt	%icc, .LL16
	add	X, 2 * SIZE, X

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
	sra	N, 2, I
	cmp	I, 0
	ble,pn	%icc, .LL55
	nop

	LDF	[X +  0 * SIZE], a1
	LDF	[X +  1 * SIZE], a2
	add	X, INCX, X
	LDF	[X +  0 * SIZE], a3
	LDF	[X +  1 * SIZE], a4
	add	X, INCX, X
	LDF	[X +  0 * SIZE], a5
	LDF	[X +  1 * SIZE], a6
	add	X, INCX, X
	add	I, -1, I
	LDF	[X +  0 * SIZE], a7
	cmp	I, 0
	LDF	[X +  1 * SIZE], a8
	ble,pt	%icc, .LL52
	add	X, INCX, X

.LL51:
	FABS	a1, t1
	LDF	[X +  0 * SIZE], a1
	FABS	a2, t2
	LDF	[X +  1 * SIZE], a2
	add	X, INCX, X
	FABS	a3, t3
	LDF	[X +  0 * SIZE], a3
	FABS	a4, t4
	LDF	[X +  1 * SIZE], a4
	add	X, INCX, X

	FABS	a5, t5
	LDF	[X +  0 * SIZE], a5
	FABS	a6, t6
	LDF	[X +  1 * SIZE], a6
	add	X, INCX, X
	FABS	a7, t7
	LDF	[X +  0 * SIZE], a7
	FABS	a8, t8
	LDF	[X +  1 * SIZE], a8

	FADD	t1, t2, t1
	FADD	t3, t4, t3
	FADD	t5, t6, t5
	FADD	t7, t8, t7

	FCMP	%fcc0, t1, c1
	FCMP	%fcc1, t3, c2
	FCMP	%fcc2, t5, c3
	FCMP	%fcc3, t7, c4

	FCMOV	%fcc0, t1, c1
	add	I, -1, I
	FCMOV	%fcc1, t3, c2
	cmp	I, 0
	FCMOV	%fcc2, t5, c3
	FCMOV	%fcc3, t7, c4


	bg,pt	%icc, .LL51
	add	X, INCX, X

.LL52:
	FABS	a1, t1
	FABS	a2, t2
	FABS	a3, t3
	FABS	a4, t4

	FABS	a5, t5
	FABS	a6, t6
	FABS	a7, t7
	FABS	a8, t8

	FADD	t1, t2, t1
	FADD	t3, t4, t3
	FADD	t5, t6, t5
	FADD	t7, t8, t7

	FCMP	%fcc0, t1, c1
	FCMP	%fcc1, t3, c2
	FCMP	%fcc2, t5, c3
	FCMP	%fcc3, t7, c4

	FCMOV	%fcc0, t1, c1
	FCMOV	%fcc1, t3, c2
	FCMOV	%fcc2, t5, c3
	FCMOV	%fcc3, t7, c4

.LL55:
	and	N, 3, I
	cmp	I,  0
	ble,a,pn %icc, .LL59
	nop

.LL56:
	LDF	[X +  0 * SIZE], a1
	LDF	[X +  1 * SIZE], a2

	FABS	a1, t1
	add	I, -1, I
	FABS	a2, t2
	cmp	I, 0
	FADD	t1, t2, t1
	FCMP	%fcc0, t1, c1
	FCMOV	%fcc0, t1, c1

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
