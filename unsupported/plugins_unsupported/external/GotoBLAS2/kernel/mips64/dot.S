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

#define N	$4
#define	X	$5
#define INCX	$6
#define Y	$7
#define INCY	$8
	
#define I	$2
#define TEMP	$3

#define a1	$f2
#define a2	$f3
#define a3	$f4
#define a4	$f5
#define b1	$f6
#define b2	$f7
#define b3	$f8
#define b4	$f9

#define s1	$f0
#define s2	$f1

	PROLOGUE
	
#ifdef F_INTERFACE
	LDINT	N,     0(N)
	LDINT	INCX,  0(INCX)
	LDINT	INCY,  0(INCY)
#endif

	MTC	$0,  s1
	MTC	$0,  s2

	dsll	INCX, INCX, BASE_SHIFT
	li	TEMP, SIZE

	blez	N, .L999
	dsll	INCY, INCY, BASE_SHIFT

	bne	INCX, TEMP, .L20
	dsra	I, N, 3

	bne	INCY, TEMP, .L20
	NOP

	blez	I, .L15
	NOP

	LD	a1,  0 * SIZE(X)
	LD	b1,  0 * SIZE(Y)

	LD	a2,  1 * SIZE(X)
	LD	b2,  1 * SIZE(Y)

	LD	a3,  2 * SIZE(X)
	LD	b3,  2 * SIZE(Y)

	LD	a4,  3 * SIZE(X)
	daddiu	I, I, -1

	blez	I, .L13
	LD	b4,  3 * SIZE(Y)
	.align 3

.L12:
	MADD	s1, s1, a1, b1
	LD	a1,  4 * SIZE(X)
	LD	b1,  4 * SIZE(Y)

	MADD	s2, s2, a2, b2
	LD	a2,  5 * SIZE(X)
	LD	b2,  5 * SIZE(Y)

	MADD	s1, s1, a3, b3
	LD	a3,  6 * SIZE(X)
	LD	b3,  6 * SIZE(Y)

	MADD	s2, s2, a4, b4
	LD	a4,  7 * SIZE(X)
	LD	b4,  7 * SIZE(Y)

	MADD	s1, s1, a1, b1
	LD	a1,  8 * SIZE(X)
	LD	b1,  8 * SIZE(Y)

	MADD	s2, s2, a2, b2
	LD	a2,  9 * SIZE(X)
	LD	b2,  9 * SIZE(Y)

	MADD	s1, s1, a3, b3
	LD	a3, 10 * SIZE(X)
	LD	b3, 10 * SIZE(Y)

	MADD	s2, s2, a4, b4
	LD	a4, 11 * SIZE(X)
	LD	b4, 11 * SIZE(Y)

	daddiu	I, I, -1
	daddiu	X, X, 8 * SIZE

	bgtz	I, .L12
	daddiu	Y, Y, 8 * SIZE
	.align 3

.L13:
	MADD	s1, s1, a1, b1
	LD	a1,  4 * SIZE(X)
	LD	b1,  4 * SIZE(Y)

	MADD	s2, s2, a2, b2
	LD	a2,  5 * SIZE(X)
	LD	b2,  5 * SIZE(Y)

	MADD	s1, s1, a3, b3
	LD	a3,  6 * SIZE(X)
	LD	b3,  6 * SIZE(Y)

	MADD	s2, s2, a4, b4
	LD	a4,  7 * SIZE(X)
	LD	b4,  7 * SIZE(Y)

	MADD	s1, s1, a1, b1
	daddiu	X, X, 8 * SIZE
	MADD	s2, s2, a2, b2
	daddiu	Y, Y, 8 * SIZE

	MADD	s1, s1, a3, b3
	MADD	s2, s2, a4, b4
	.align 3

.L15:
	andi	I,  N, 7

	blez	I, .L999
	NOP
	.align	3

.L16:
	LD	a1,  0 * SIZE(X)
	LD	b1,  0 * SIZE(Y)

	MADD	s1, s1, a1, b1

	daddiu	I, I, -1

	daddiu	X, X, SIZE
	daddiu	Y, Y, SIZE

	bgtz	I, .L16
	NOP
	j	.L999
	NOP
	.align 3

.L20:
#ifdef F_INTERFACE
	bgez	INCX, .L21
	daddiu	TEMP, N, -1

	mult	TEMP, INCX

	mflo	TEMP
	dsub	X, X, TEMP
	.align 3

.L21:
	bgez	INCY, .L22
	daddiu	TEMP, N, -1

	mult	TEMP, INCY

	mflo	TEMP
	dsub	Y, Y, TEMP
	.align 3

.L22:
#endif
	blez	I, .L25
	NOP
	.align 3

.L23:
	LD	a1,  0 * SIZE(X)
	dadd	X, X, INCX
	LD	b1,  0 * SIZE(Y)
	dadd	Y, Y, INCY

	MADD	s1, s1, a1, b1

	LD	a1,  0 * SIZE(X)
	dadd	X, X, INCX
	LD	b1,  0 * SIZE(Y)
	dadd	Y, Y, INCY

	MADD	s2, s2, a1, b1

	LD	a1,  0 * SIZE(X)
	dadd	X, X, INCX
	LD	b1,  0 * SIZE(Y)
	dadd	Y, Y, INCY

	MADD	s1, s1, a1, b1

	LD	a1,  0 * SIZE(X)
	dadd	X, X, INCX
	LD	b1,  0 * SIZE(Y)
	dadd	Y, Y, INCY

	MADD	s2, s2, a1, b1

	LD	a1,  0 * SIZE(X)
	dadd	X, X, INCX
	LD	b1,  0 * SIZE(Y)
	dadd	Y, Y, INCY

	MADD	s1, s1, a1, b1

	LD	a1,  0 * SIZE(X)
	dadd	X, X, INCX
	LD	b1,  0 * SIZE(Y)
	dadd	Y, Y, INCY

	MADD	s2, s2, a1, b1

	LD	a1,  0 * SIZE(X)
	dadd	X, X, INCX
	LD	b1,  0 * SIZE(Y)
	dadd	Y, Y, INCY

	MADD	s1, s1, a1, b1

	LD	a1,  0 * SIZE(X)
	dadd	X, X, INCX
	LD	b1,  0 * SIZE(Y)
	dadd	Y, Y, INCY

	daddiu	I, I, -1

	bgtz	I, .L23
	MADD	s2, s2, a1, b1
	.align 3

.L25:
	andi	I,  N, 7

	blez	I, .L999
	NOP
	.align	3

.L26:
	LD	a1,  0 * SIZE(X)
	dadd	X, X, INCX
	LD	b1,  0 * SIZE(Y)
	dadd	Y, Y, INCY

	daddiu	I, I, -1

	bgtz	I, .L26
	MADD	s1, s1, a1, b1
	.align 3

.L999:
	j	$31
	ADD	s1, s1, s2

	EPILOGUE
