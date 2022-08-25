/*********************************************************************/
/*                                                                   */
/*             Optimized BLAS libraries                              */
/*                     By Kazushige Goto <kgoto@tacc.utexas.edu>     */
/*                                                                   */
/* Copyright (c) The University of Texas, 2005. All rights reserved. */
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

#define  PREFETCHSIZE 72
#define WPREFETCHSIZE 16

#define M	%i0
#define N	%i1
#define A	%i2
#define LDA	%i3
#define B	%i4

#define A1	%l0
#define A2	%l1
#define A3	%l2
#define A4	%l3

#define I	%l4
#define J	%l5

#define B1	%o0
#define B2	%o1
#define B3	%o3
#define M2	%o4

#ifdef DOUBLE
#define c01	%f0
#define c02	%f2
#define c03	%f4
#define c04	%f6
#define c05	%f8
#define c06	%f10
#define c07	%f12
#define c08	%f14
#define c09	%f16
#define c10	%f18
#define c11	%f20
#define c12	%f22
#define c13	%f24
#define c14	%f26
#define c15	%f28
#define c16	%f30
#else
#define c01	%f0
#define c02	%f1
#define c03	%f2
#define c04	%f3
#define c05	%f4
#define c06	%f5
#define c07	%f6
#define c08	%f7
#define c09	%f8
#define c10	%f9
#define c11	%f10
#define c12	%f11
#define c13	%f12
#define c14	%f13
#define c15	%f14
#define c16	%f15
#endif

	PROLOGUE
	SAVESP

	sll	M, BASE_SHIFT + 1, M2

	and	N, -2, B2
	sll	M, BASE_SHIFT, B1
	smul	B1, B2, B2
	add	B, B2, B2

	sra	M, 1, J
	cmp	J, 0
	ble,pn	%icc, .LL100
	sll	LDA, BASE_SHIFT, LDA

.LL11:
	add	A,  LDA, A2
	mov	A,  A1
	sra	N, 3, I
	cmp	I, 0

	mov	B, B1
	add	B, 4 * SIZE, B

	ble,pn	%icc, .LL13
	add	A2, LDA, A
	.align 4

.LL12:
	prefetch [A1 + (PREFETCHSIZE + 0) * SIZE], 0
	LDF	[A1 +  0 * SIZE], c01
	LDF	[A1 +  1 * SIZE], c02
	LDF	[A2 +  0 * SIZE], c09
	LDF	[A2 +  1 * SIZE], c10
	LDF	[A1 +  2 * SIZE], c03
	LDF	[A1 +  3 * SIZE], c04
	LDF	[A2 +  2 * SIZE], c11
	LDF	[A2 +  3 * SIZE], c12

	prefetch [A2 + (PREFETCHSIZE + 0) * SIZE], 0
	LDF	[A1 +  4 * SIZE], c05
	LDF	[A1 +  5 * SIZE], c06
	LDF	[A2 +  4 * SIZE], c13
	LDF	[A2 +  5 * SIZE], c14
	LDF	[A1 +  6 * SIZE], c07
	LDF	[A1 +  7 * SIZE], c08
	LDF	[A2 +  6 * SIZE], c15
	LDF	[A2 +  7 * SIZE], c16

	add	A1,  8 * SIZE, A1
	add	A2,  8 * SIZE, A2
	add	I, -1, I
	cmp	I, 0

	prefetch [B1 + (WPREFETCHSIZE + 0) * SIZE], 0
	STF	c01, [B1 +  0 * SIZE]
	STF	c02, [B1 +  1 * SIZE]
	STF	c09, [B1 +  2 * SIZE]
	STF	c10, [B1 +  3 * SIZE]
	add	B1, M2, B1

	prefetch [B1 + (WPREFETCHSIZE + 0) * SIZE], 0
	STF	c03, [B1 +  0 * SIZE]
	STF	c04, [B1 +  1 * SIZE]
	STF	c11, [B1 +  2 * SIZE]
	STF	c12, [B1 +  3 * SIZE]
	add	B1, M2, B1

	prefetch [B1 + (WPREFETCHSIZE + 0) * SIZE], 0
	STF	c05, [B1 +  0 * SIZE]
	STF	c06, [B1 +  1 * SIZE]
	STF	c13, [B1 +  2 * SIZE]
	STF	c14, [B1 +  3 * SIZE]
	add	B1, M2, B1

	prefetch [B1 + (WPREFETCHSIZE + 0) * SIZE], 0
	STF	c07, [B1 +  0 * SIZE]
	STF	c08, [B1 +  1 * SIZE]
	STF	c15, [B1 +  2 * SIZE]
	STF	c16, [B1 +  3 * SIZE]

	bg,pt	%icc, .LL12
	add	B1, M2, B1

.LL13:
	and	N, 4, I
	cmp	I, 0
	ble,pn	%icc, .LL14
	nop

	LDF	[A1 +  0 * SIZE], c01
	LDF	[A1 +  1 * SIZE], c02
	LDF	[A2 +  0 * SIZE], c03
	LDF	[A2 +  1 * SIZE], c04

	LDF	[A1 +  2 * SIZE], c05
	LDF	[A1 +  3 * SIZE], c06
	LDF	[A2 +  2 * SIZE], c07
	LDF	[A2 +  3 * SIZE], c08

	add	A1,  4 * SIZE, A1
	add	A2,  4 * SIZE, A2

	STF	c01, [B1 +  0 * SIZE]
	STF	c02, [B1 +  1 * SIZE]
	STF	c03, [B1 +  2 * SIZE]
	STF	c04, [B1 +  3 * SIZE]
	add	B1, M2, B1
	STF	c05, [B1 +  0 * SIZE]
	STF	c06, [B1 +  1 * SIZE]
	STF	c07, [B1 +  2 * SIZE]
	STF	c08, [B1 +  3 * SIZE]
	add	B1, M2, B1
	.align 4

.LL14:
	and	N, 2, I
	cmp	I, 0
	ble,pn	%icc, .LL15
	nop

	LDF	[A1 +  0 * SIZE], c01
	LDF	[A1 +  1 * SIZE], c02

	LDF	[A2 +  0 * SIZE], c03
	LDF	[A2 +  1 * SIZE], c04

	add	A1,  2 * SIZE, A1
	add	A2,  2 * SIZE, A2

	STF	c01, [B1 +  0 * SIZE]
	STF	c02, [B1 +  1 * SIZE]
	STF	c03, [B1 +  2 * SIZE]
	STF	c04, [B1 +  3 * SIZE]
	add	B1, M2, B1
	.align 4

.LL15:
	and	N, 1, I
	cmp	I, 0
	ble,pn	%icc, .LL99
	nop

	LDF	[A1 +  0 * SIZE], c01
	LDF	[A2 +  0 * SIZE], c02

	STF	c01, [B2 +  0 * SIZE]
	STF	c02, [B2 +  1 * SIZE]
	add	B2, 2 * SIZE, B2

.LL99:
	add	J, -1, J
	cmp	J, 0
	bg,pt	%icc, .LL11
	nop

.LL100:
	and	M, 1, J
	cmp	J, 0
	ble,pn	%icc, .LL999
	nop

.LL211:
	sra	N, 1, I
	cmp	I, 0

	mov	B, B1

	ble,pn	%icc, .LL215
	mov	A,  A1

.LL212:
	LDF	[A1 +  0 * SIZE], c01
	LDF	[A1 +  1 * SIZE], c02

	add	A1,  2 * SIZE, A1
	add	I, -1, I
	cmp	I, 0

	STF	c01, [B +  0 * SIZE]
	STF	c02, [B +  1 * SIZE]

	bg,pt	%icc, .LL212
	add	B,   M2, B

.LL215:
	and	N, 1, I
	cmp	I, 0
	ble,pn	%icc, .LL999
	nop

	LDF	[A1 +  0 * SIZE], c01
	STF	c01, [B2 +  0 * SIZE]

.LL999:
	return	%i7 + 8
	clr	%o0

	EPILOGUE
