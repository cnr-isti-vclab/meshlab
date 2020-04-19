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
	
#define XX	$9
#define YY	$10

#define C	$f17
#define S	$f18

#define I	$2
#define TEMP	$3

#define a1	$f4
#define a2	$f5
#define a3	$f6
#define a4	$f7

#define b1	$f8
#define b2	$f9
#define b3	$f10
#define b4	$f11

#define t1	$f0
#define t2	$f1
#define t3	$f2
#define t4	$f3

	PROLOGUE
	
	dsll	INCX, INCX, BASE_SHIFT
	li	TEMP, SIZE

	blez	N, .L999
	dsll	INCY, INCY, BASE_SHIFT

	bne	INCX, TEMP, .L20
	dsra	I, N, 2

	bne	INCY, TEMP, .L20
	NOP

	blez	I, .L15
	daddiu	I, I, -1

	LD	a1,  0 * SIZE(X)
	LD	b1,  0 * SIZE(Y)
	LD	a2,  1 * SIZE(X)
	LD	b2,  1 * SIZE(Y)

	LD	a3,  2 * SIZE(X)
	LD	b3,  2 * SIZE(Y)
	MUL	t1, S, b1

	LD	a4,  3 * SIZE(X)
	MUL	t2, C, b1
	LD	b4,  3 * SIZE(Y)
	MUL	t3, S, b2

	blez	I, .L13
	MUL	t4, C, b2
	.align 3

.L12:
	MADD	t1, t1, C, a1
	LD	b1,  4 * SIZE(Y)
	NMSUB	t2, t2, S, a1
	LD	a1,  4 * SIZE(X)
	MADD	t3, t3, C, a2
	LD	b2,  5 * SIZE(Y)
	NMSUB	t4, t4, S, a2
	LD	a2,  5 * SIZE(X)

	ST	t1,  0 * SIZE(X)
	MUL	t1, S, b3
	ST	t2,  0 * SIZE(Y)
	MUL	t2, C, b3
	ST	t3,  1 * SIZE(X)
	MUL	t3, S, b4
	ST	t4,  1 * SIZE(Y)
	MUL	t4, C, b4


	MADD	t1, t1, C, a3
	LD	b3,  6 * SIZE(Y)
	NMSUB	t2, t2, S, a3
	LD	a3,  6 * SIZE(X)
	MADD	t3, t3, C, a4
	LD	b4,  7 * SIZE(Y)
	NMSUB	t4, t4, S, a4
	LD	a4,  7 * SIZE(X)

	ST	t1,  2 * SIZE(X)
	MUL	t1, S, b1
	ST	t2,  2 * SIZE(Y)
	MUL	t2, C, b1
	ST	t3,  3 * SIZE(X)
	MUL	t3, S, b2
	ST	t4,  3 * SIZE(Y)
	MUL	t4, C, b2

	daddiu	I, I, -1
	daddiu	X, X, 4 * SIZE

	bgtz	I, .L12
	daddiu	Y, Y, 4 * SIZE
	.align 3

.L13:
	MADD	t1, t1, C, a1
	NMSUB	t2, t2, S, a1
	MADD	t3, t3, C, a2
	NMSUB	t4, t4, S, a2

	ST	t1,  0 * SIZE(X)
	MUL	t1, S, b3
	ST	t2,  0 * SIZE(Y)
	MUL	t2, C, b3
	ST	t3,  1 * SIZE(X)
	MUL	t3, S, b4
	ST	t4,  1 * SIZE(Y)
	MUL	t4, C, b4

	MADD	t1, t1, C, a3
	NMSUB	t2, t2, S, a3
	MADD	t3, t3, C, a4
	daddiu	X, X, 4 * SIZE
	NMSUB	t4, t4, S, a4
	daddiu	Y, Y, 4 * SIZE

	ST	t1, -2 * SIZE(X)
	ST	t2, -2 * SIZE(Y)
	ST	t3, -1 * SIZE(X)
	ST	t4, -1 * SIZE(Y)
	.align 3

.L15:
	andi	I,  N, 3

	blez	I, .L999
	NOP
	.align	3

.L16:
	LD	a1,  0 * SIZE(X)
	LD	b1,  0 * SIZE(Y)

	MUL	t1, S, b1
	MUL	t2, C, b1

	MADD	t1, t1, C, a1
	NMSUB	t2, t2, S, a1

	ST	t1,  0 * SIZE(X)
	ST	t2,  0 * SIZE(Y)

	daddiu I, I, -1

	daddiu	X, X, SIZE
	daddiu	Y, Y, SIZE

	bgtz	I, .L16
	NOP
	j	.L999
	NOP
	.align 3

.L20:
	move	XX, X
	move	YY, Y

	blez	I, .L25
	daddiu	I, I, -1

	LD	a1,  0 * SIZE(X)
	dadd	X, X, INCX
	LD	b1,  0 * SIZE(Y)
	dadd	Y, Y, INCY

	LD	a2,  0 * SIZE(X)
	dadd	X, X, INCX
	LD	b2,  0 * SIZE(Y)
	dadd	Y, Y, INCY

	LD	a3,  0 * SIZE(X)
	dadd	X, X, INCX
	LD	b3,  0 * SIZE(Y)
	dadd	Y, Y, INCY

	MUL	t1, S, b1

	LD	a4,  0 * SIZE(X)
	dadd	X, X, INCX
	MUL	t2, C, b1
	LD	b4,  0 * SIZE(Y)
	dadd	Y, Y, INCY

	MUL	t3, S, b2
	blez	I, .L23
	MUL	t4, C, b2
	.align 3

.L22:
	MADD	t1, t1, C, a1
	LD	b1,  0 * SIZE(Y)
	dadd	Y, Y, INCY
	NMSUB	t2, t2, S, a1
	LD	a1,  0 * SIZE(X)
	dadd	X, X, INCX
	MADD	t3, t3, C, a2
	LD	b2,  0 * SIZE(Y)
	dadd	Y, Y, INCY
	NMSUB	t4, t4, S, a2
	LD	a2,  0 * SIZE(X)
	dadd	X, X, INCX

	ST	t1,  0 * SIZE(XX)
	dadd	XX, XX, INCX
	MUL	t1, S, b3
	ST	t2,  0 * SIZE(YY)
	dadd	YY, YY, INCY
	MUL	t2, C, b3
	ST	t3,  0 * SIZE(XX)
	dadd	XX, XX, INCX
	MUL	t3, S, b4
	ST	t4,  0 * SIZE(YY)
	dadd	YY, YY, INCY
	MUL	t4, C, b4

	MADD	t1, t1, C, a3
	LD	b3,  0 * SIZE(Y)
	dadd	Y, Y, INCY
	NMSUB	t2, t2, S, a3
	LD	a3,  0 * SIZE(X)
	dadd	X, X, INCX
	MADD	t3, t3, C, a4
	LD	b4,  0 * SIZE(Y)
	dadd	Y, Y, INCY
	NMSUB	t4, t4, S, a4
	LD	a4,  0 * SIZE(X)
	dadd	X, X, INCX

	ST	t1,  0 * SIZE(XX)
	dadd	XX, XX, INCX
	MUL	t1, S, b1
	ST	t2,  0 * SIZE(YY)
	dadd	YY, YY, INCY
	MUL	t2, C, b1
	ST	t3,  0 * SIZE(XX)
	dadd	XX, XX, INCX
	MUL	t3, S, b2
	ST	t4,  0 * SIZE(YY)
	MUL	t4, C, b2
	daddiu	I, I, -1

	bgtz	I, .L22
	dadd	YY, YY, INCY
	.align 3

.L23:
	MADD	t1, t1, C, a1
	NMSUB	t2, t2, S, a1
	MADD	t3, t3, C, a2
	NMSUB	t4, t4, S, a2

	ST	t1,  0 * SIZE(XX)
	dadd	XX, XX, INCX
	MUL	t1, S, b3
	ST	t2,  0 * SIZE(YY)
	dadd	YY, YY, INCY
	MUL	t2, C, b3
	ST	t3,  0 * SIZE(XX)
	dadd	XX, XX, INCX
	MUL	t3, S, b4
	ST	t4,  0 * SIZE(YY)
	dadd	YY, YY, INCY
	MUL	t4, C, b4

	MADD	t1, t1, C, a3
	NMSUB	t2, t2, S, a3
	MADD	t3, t3, C, a4
	NMSUB	t4, t4, S, a4

	ST	t1,  0 * SIZE(XX)
	dadd	XX, XX, INCX
	ST	t2,  0 * SIZE(YY)
	dadd	YY, YY, INCY
	ST	t3,  0 * SIZE(XX)
	dadd	XX, XX, INCX
	ST	t4,  0 * SIZE(YY)
	dadd	YY, YY, INCY
	.align 3

.L25:
	andi	I,  N, 3

	blez	I, .L999
	NOP
	.align	3

.L26:
	LD	a1,  0 * SIZE(X)
	LD	b1,  0 * SIZE(Y)

	MUL	t1, S, b1
	MUL	t2, C, b1

	MADD	t1, t1, C, a1
	daddiu	I, I, -1
	NMSUB	t2, t2, S, a1

	ST	t1,  0 * SIZE(X)
	ST	t2,  0 * SIZE(Y)

	dadd	X, X, INCX
	bgtz	I, .L26
	dadd	Y, Y, INCY
	.align 3

.L999:
	j	$31
	NOP

	EPILOGUE
