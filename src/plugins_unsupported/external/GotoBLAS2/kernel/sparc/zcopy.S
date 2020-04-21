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
#define Y	%i3
#define INCY	%i4
#define I	%i5

#ifdef DOUBLE
#define a1	%f0
#define a2	%f2
#define a3	%f4
#define a4	%f6
#define a5	%f8
#define a6	%f10
#define a7	%f12
#define a8	%f14
#else
#define a1	%f0
#define a2	%f1
#define a3	%f2
#define a4	%f3
#define a5	%f4
#define a6	%f5
#define a7	%f6
#define a8	%f7
#endif

	PROLOGUE
	SAVESP

	sll	INCX, ZBASE_SHIFT, INCX
	sll	INCY, ZBASE_SHIFT, INCY

	cmp	INCX, 2 * SIZE
	bne	.LL50
	nop
	cmp	INCY, 2 * SIZE
	bne	.LL50
	nop

	sra	N, 2, I
	cmp	I, 0
	ble,pn	%icc, .LL15
	nop

#define PREFETCHSIZE 32

.LL11:
	prefetch [X  + PREFETCHSIZE * SIZE], 0
	prefetch [Y  + PREFETCHSIZE * SIZE], 0

	LDF	[X +  0 * SIZE], a1
	LDF	[X +  1 * SIZE], a2
	LDF	[X +  2 * SIZE], a3
	LDF	[X +  3 * SIZE], a4
	LDF	[X +  4 * SIZE], a5
	LDF	[X +  5 * SIZE], a6
	LDF	[X +  6 * SIZE], a7
	LDF	[X +  7 * SIZE], a8

	STF	a1, [Y +  0 * SIZE]
	add	I, -1, I
	STF	a2, [Y +  1 * SIZE]
	cmp	I, 0
	STF	a3, [Y +  2 * SIZE]
	add	X, 8 * SIZE, X
	STF	a4, [Y +  3 * SIZE]
	STF	a5, [Y +  4 * SIZE]
	STF	a6, [Y +  5 * SIZE]
	STF	a7, [Y +  6 * SIZE]
	STF	a8, [Y +  7 * SIZE]

	bg,pt	%icc, .LL11
	add	Y, 8 * SIZE, Y

.LL15:
	and	N, 3, I
	cmp	I,  0
	ble,a,pn %icc, .LL19
	nop

.LL16:
	LDF	[X +  0 * SIZE], a1
	LDF	[X +  1 * SIZE], a2
	add	I, -1, I
	cmp	I, 0
	STF	a1, [Y +  0 * SIZE]
	add	X, 2 * SIZE, X
	STF	a2, [Y +  1 * SIZE]
	bg,pt	%icc, .LL16
	add	Y, 2 * SIZE, Y

.LL19:
	return	%i7 + 8
	clr	%g0

.LL50:
	sra	N, 2, I
	cmp	I, 0
	ble,pn	%icc, .LL55
	nop

.LL51:
	LDF	[X +  0 * SIZE], a1
	LDF	[X +  1 * SIZE], a2
	add	X, INCX, X
	LDF	[X +  0 * SIZE], a3
	LDF	[X +  1 * SIZE], a4
	add	X, INCX, X
	LDF	[X +  0 * SIZE], a5
	LDF	[X +  1 * SIZE], a6
	add	X, INCX, X
	LDF	[X +  0 * SIZE], a7
	LDF	[X +  1 * SIZE], a8
	add	X, INCX, X

	STF	a1, [Y +  0 * SIZE]
	add	I, -1, I
	STF	a2, [Y +  1 * SIZE]
	add	Y, INCY, Y
	cmp	I, 0
	STF	a3, [Y +  0 * SIZE]
	STF	a4, [Y +  1 * SIZE]
	add	Y, INCY, Y
	STF	a5, [Y +  0 * SIZE]
	STF	a6, [Y +  1 * SIZE]
	add	Y, INCY, Y
	STF	a7, [Y +  0 * SIZE]
	STF	a8, [Y +  1 * SIZE]

	bg,pt	%icc, .LL51
	add	Y, INCY, Y

.LL55:
	and	N, 3, I
	cmp	I,  0
	ble,a,pn %icc, .LL59
	nop

.LL56:
	LDF	[X +  0 * SIZE], a1
	LDF	[X +  1 * SIZE], a2
	add	I, -1, I
	cmp	I, 0
	add	X, INCX, X
	STF	a1, [Y +  0 * SIZE]
	STF	a2, [Y +  1 * SIZE]
	bg,pt	%icc, .LL56
	add	Y, INCY, Y

.LL59:
	return	%i7 + 8
	clr	%o0

	EPILOGUE
