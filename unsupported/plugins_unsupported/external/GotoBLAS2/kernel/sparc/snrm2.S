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

	PROLOGUE
	SAVESP

	FCLR(0)

	FMOV	c1, c2
	FMOV	c1, c3
	FMOV	c1, c4
	FMOV	c1, t1
	FMOV	c1, t2
	FMOV	c1, t3
	FMOV	c1, t4

	cmp	INCX, 0
	ble	.LL20
	sll	INCX, BASE_SHIFT, INCX

	cmp	N, 0
	ble	.LL20
	nop

	cmp	INCX, SIZE
	bne	.LL50
	nop

	sra	N, 3, I
	cmp	I, 0
	ble,pn	%icc, .LL15
	nop

	ld	[X +  0 * SIZE], a1
	add	I, -1, I
	ld	[X +  1 * SIZE], a2
	cmp	I, 0
	ld	[X +  2 * SIZE], a3
	ld	[X +  3 * SIZE], a4
	ld	[X +  4 * SIZE], a5
	ld	[X +  5 * SIZE], a6
	ld	[X +  6 * SIZE], a7
	ld	[X +  7 * SIZE], a8

	ble,pt	%icc, .LL12
	add	X, 8 * SIZE, X

#define PREFETCHSIZE 40

.LL11:
	faddd	c1, t1, c1
	fsmuld	a1, a1, t1
	prefetch [X  + PREFETCHSIZE * SIZE], 0

	faddd	c2, t2, c2
	add	I, -1, I
	fsmuld	a2, a2, t2
	ld	[X +  0 * SIZE], a1

	faddd	c3, t3, c3
	cmp	I, 0
	fsmuld	a3, a3, t3
	ld	[X +  1 * SIZE], a2

	faddd	c4, t4, c4
	fsmuld	a4, a4, t4
	ld	[X +  2 * SIZE], a3

	faddd	c1, t1, c1
	fsmuld	a5, a5, t1
	ld	[X +  3 * SIZE], a4

	faddd	c2, t2, c2
	fsmuld	a6, a6, t2
	ld	[X +  4 * SIZE], a5

	faddd	c3, t3, c3
	fsmuld	a7, a7, t3
	ld	[X +  5 * SIZE], a6

	faddd	c4, t4, c4
	ld	[X +  6 * SIZE], a7
	fsmuld	a8, a8, t4
	add	X, 8 * SIZE, X

	bg,pt	%icc, .LL11
	ld	[X -  1 * SIZE], a8

.LL12:
	faddd	c1, t1, c1
	fsmuld	a1, a1, t1
	faddd	c2, t2, c2
	fsmuld	a2, a2, t2

	faddd	c3, t3, c3
	fsmuld	a3, a3, t3
	faddd	c4, t4, c4
	fsmuld	a4, a4, t4

	faddd	c1, t1, c1
	fsmuld	a5, a5, t1
	faddd	c2, t2, c2
	fsmuld	a6, a6, t2

	faddd	c3, t3, c3
	fsmuld	a7, a7, t3
	faddd	c4, t4, c4
	fsmuld	a8, a8, t4

.LL15:
	and	N, 7, I
	cmp	I,  0
	ble,a,pn %icc, .LL19
	nop

.LL16:
	ld	[X +  0 * SIZE], a1

	add	I, -1, I
	cmp	I, 0
	faddd	c1, t1, c1
	fsmuld	a1, a1, t1

	bg,pt	%icc, .LL16
	add	X, 1 * SIZE, X

.LL19:
	faddd	c1, t1, c1
	faddd	c2, t2, c2
	faddd	c3, t3, c3
	faddd	c4, t4, c4

	faddd	c1, c2, c1
	faddd	c3, c4, c3
	faddd	c1, c3, c1

	fsqrtd	c1, c1

#if !defined(NEED_F2CCONV) || !defined(F_INTERFACE_F2C)
	fdtos	c1, c1
#endif
.LL20:

	return	%i7 + 8
	clr	%g0

.LL50:
	sra	N, 3, I
	cmp	I, 0
	ble,pn	%icc, .LL55
	nop

	ld	[X +  0 * SIZE], a1
	add	X, INCX, X
	ld	[X +  0 * SIZE], a2
	add	X, INCX, X
	ld	[X +  0 * SIZE], a3
	add	X, INCX, X
	ld	[X +  0 * SIZE], a4
	add	X, INCX, X
	ld	[X +  0 * SIZE], a5
	add	X, INCX, X
	ld	[X +  0 * SIZE], a6
	add	X, INCX, X
	add	I, -1, I
	ld	[X +  0 * SIZE], a7
	cmp	I, 0
	add	X, INCX, X
	ld	[X +  0 * SIZE], a8

	ble,pt	%icc, .LL52
	add	X, INCX, X

.LL51:
	faddd	c1, t1, c1
	add	I, -1, I
	fsmuld	a1, a1, t1
	ld	[X +  0 * SIZE], a1
	add	X, INCX, X

	faddd	c2, t2, c2
	cmp	I, 0
	fsmuld	a2, a2, t2
	ld	[X +  0 * SIZE], a2
	add	X, INCX, X

	faddd	c3, t3, c3
	fsmuld	a3, a3, t3
	ld	[X +  0 * SIZE], a3
	add	X, INCX, X

	faddd	c4, t4, c4
	fsmuld	a4, a4, t4
	ld	[X +  0 * SIZE], a4
	add	X, INCX, X

	faddd	c1, t1, c1
	fsmuld	a5, a5, t1
	ld	[X +  0 * SIZE], a5
	add	X, INCX, X

	faddd	c2, t2, c2
	fsmuld	a6, a6, t2
	ld	[X +  0 * SIZE], a6
	add	X, INCX, X

	faddd	c3, t3, c3
	fsmuld	a7, a7, t3
	ld	[X +  0 * SIZE], a7
	add	X, INCX, X

	faddd	c4, t4, c4
	fsmuld	a8, a8, t4
	ld	[X +  0 * SIZE], a8
	bg,pt	%icc, .LL51
	add	X, INCX, X

.LL52:
	faddd	c1, t1, c1
	fsmuld	a1, a1, t1
	faddd	c2, t2, c2
	fsmuld	a2, a2, t2

	faddd	c3, t3, c3
	fsmuld	a3, a3, t3
	faddd	c4, t4, c4
	fsmuld	a4, a4, t4

	faddd	c1, t1, c1
	fsmuld	a5, a5, t1
	faddd	c2, t2, c2
	fsmuld	a6, a6, t2

	faddd	c3, t3, c3
	fsmuld	a7, a7, t3
	faddd	c4, t4, c4
	fsmuld	a8, a8, t4

.LL55:
	and	N, 7, I
	cmp	I,  0
	ble,a,pn %icc, .LL59
	nop

.LL56:
	ld	[X +  0 * SIZE], a1
	add	I, -1, I
	cmp	I, 0
	faddd	c1, t1, c1
	fsmuld	a1, a1, t1
	bg,pt	%icc, .LL56
	add	X, INCX, X

.LL59:
	faddd	c1, t1, c1
	faddd	c2, t2, c2
	faddd	c3, t3, c3
	faddd	c4, t4, c4

	faddd	c1, c2, c1
	faddd	c3, c4, c3
	faddd	c1, c3, c1

	fsqrtd	c1, c1

#if !defined(NEED_F2CCONV) || !defined(F_INTERFACE_F2C)
	fdtos	c1, c1
#endif

	return	%i7 + 8
	clr	%o0

	EPILOGUE
