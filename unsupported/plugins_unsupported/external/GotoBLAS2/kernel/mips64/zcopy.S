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

#define a1	$f0
#define a2	$f1
#define a3	$f2
#define a4	$f3
#define a5	$f4
#define a6	$f5
#define a7	$f6
#define a8	$f7

	PROLOGUE
	
#ifdef F_INTERFACE
	LDINT	N,     0(N)
	LDINT	INCX,  0(INCX)
	LDINT	INCY,  0(INCY)
#endif

	li	TEMP, 2 * SIZE
	NOP

	blez	N, .L999
	dsll	INCX, INCX, ZBASE_SHIFT

	bne	INCX, TEMP, .L20
	dsll	INCY, INCY, ZBASE_SHIFT

	bne	INCY, TEMP, .L20
	dsra	I, N, 2

	blez	I, .L15
	daddiu	I, I, -1

	LD	a1,  0 * SIZE(X)
	LD	a2,  1 * SIZE(X)
	LD	a3,  2 * SIZE(X)
	LD	a4,  3 * SIZE(X)
	LD	a5,  4 * SIZE(X)
	LD	a6,  5 * SIZE(X)
	LD	a7,  6 * SIZE(X)
	LD	a8,  7 * SIZE(X)

	blez	I, .L13
	NOP
	.align 3

.L12:
	ST	a1,  0 * SIZE(Y)
	LD	a1,  8 * SIZE(X)

	ST	a2,  1 * SIZE(Y)
	LD	a2,  9 * SIZE(X)

	ST	a3,  2 * SIZE(Y)
	LD	a3, 10 * SIZE(X)

	ST	a4,  3 * SIZE(Y)
	LD	a4, 11 * SIZE(X)

	ST	a5,  4 * SIZE(Y)
	LD	a5, 12 * SIZE(X)

	ST	a6,  5 * SIZE(Y)
	LD	a6, 13 * SIZE(X)

	ST	a7,  6 * SIZE(Y)
	LD	a7, 14 * SIZE(X)

	ST	a8,  7 * SIZE(Y)
	LD	a8, 15 * SIZE(X)

	daddiu	I, I, -1
	daddiu	X, X, 8 * SIZE

	bgtz	I, .L12
	daddiu	Y, Y, 8 * SIZE
	.align 3

.L13:
	ST	a1,  0 * SIZE(Y)
	ST	a2,  1 * SIZE(Y)
	ST	a3,  2 * SIZE(Y)
	ST	a4,  3 * SIZE(Y)
	ST	a5,  4 * SIZE(Y)
	ST	a6,  5 * SIZE(Y)
	ST	a7,  6 * SIZE(Y)
	ST	a8,  7 * SIZE(Y)

	daddiu	X, X, 8 * SIZE
	daddiu	Y, Y, 8 * SIZE
	.align 3

.L15:
	andi	I,  N, 3

	blez	I, .L999
	NOP
	.align	3

.L16:
	LD	a1,  0 * SIZE(X)
	LD	a2,  1 * SIZE(X)

	daddiu	X, X, 2 * SIZE
	daddiu	Y, Y, 2 * SIZE

	ST	a1, -2 * SIZE(Y)
	daddiu	I, I, -1

	bgtz	I, .L16
	ST	a2, -1 * SIZE(Y)

	j	$31
	NOP
	.align 3

.L20:
	dsra	I, N, 2

	blez	I, .L25
	daddiu	I, I, -1

	LD	a1,  0 * SIZE(X)
	LD	a2,  1 * SIZE(X)
	daddu	X, X, INCX
	LD	a3,  0 * SIZE(X)
	LD	a4,  1 * SIZE(X)
	daddu	X, X, INCX
	LD	a5,  0 * SIZE(X)
	LD	a6,  1 * SIZE(X)
	daddu	X, X, INCX
	LD	a7,  0 * SIZE(X)
	LD	a8,  1 * SIZE(X)

	blez	I, .L23
	daddu	X, X, INCX
	.align 3

.L22:
	ST	a1,  0 * SIZE(Y)
	LD	a1,  0 * SIZE(X)

	ST	a2,  1 * SIZE(Y)
	daddu	Y, Y, INCY
	LD	a2,  1 * SIZE(X)
	daddu	X, X, INCX

	ST	a3,  0 * SIZE(Y)
	LD	a3,  0 * SIZE(X)

	ST	a4,  1 * SIZE(Y)
	daddu	Y, Y, INCY
	LD	a4,  1 * SIZE(X)
	daddu	X, X, INCX

	ST	a5,  0 * SIZE(Y)
	LD	a5,  0 * SIZE(X)

	ST	a6,  1 * SIZE(Y)
	daddu	Y, Y, INCY
	LD	a6,  1 * SIZE(X)
	daddu	X, X, INCX

	ST	a7,  0 * SIZE(Y)
	LD	a7,  0 * SIZE(X)

	ST	a8,  1 * SIZE(Y)
	daddu	Y, Y, INCY
	LD	a8,  1 * SIZE(X)

	daddiu	I, I, -1

	bgtz	I, .L22
	daddu	X, X, INCX
	.align 3

.L23:
	ST	a1,  0 * SIZE(Y)
	ST	a2,  1 * SIZE(Y)
	daddu	Y, Y, INCY
	ST	a3,  0 * SIZE(Y)
	ST	a4,  1 * SIZE(Y)
	daddu	Y, Y, INCY
	ST	a5,  0 * SIZE(Y)
	ST	a6,  1 * SIZE(Y)
	daddu	Y, Y, INCY
	ST	a7,  0 * SIZE(Y)
	ST	a8,  1 * SIZE(Y)
	daddu	Y, Y, INCY
	.align 3

.L25:
	andi	I,  N, 3

	blez	I, .L999
	NOP
	.align	3

.L26:
	LD	a1,  0 * SIZE(X)
	LD	a2,  1 * SIZE(X)
	daddu	X, X, INCX

	daddiu	I, I, -1
	ST	a1,  0 * SIZE(Y)
	ST	a2,  1 * SIZE(Y)

	bgtz	I, .L26
	daddu	Y, Y, INCY
	.align 3

.L999:
	j	$31
	NOP

	EPILOGUE
