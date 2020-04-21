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
	
#define I	$2
#define TEMP	$3

#define a1	$f6
#define a2	$f7
#define a3	$f8
#define a4	$f9
#define a5	$f10
#define a6	$f11
#define a7	$f12
#define a8	$f13

#define s1	$f0
#define s2	$f1

#define t1	$f2
#define t2	$f3
#define t3	$f4
#define t4	$f5


	PROLOGUE
	
#ifdef F_INTERFACE
	LDINT	N,     0(N)
	LDINT	INCX,  0(INCX)
#endif

	dmtc1	$0,  s1
	li	TEMP, SIZE

	blez	N, .L999
	mov.d	s2, s1

	blez	INCX, .L999
	dsll	INCX, INCX, BASE_SHIFT

	bne	INCX, TEMP, .L20
	dsra	I, N, 3

	blez	I, .L15
	NOP

	LD	a1,  0 * SIZE(X)
	LD	a2,  1 * SIZE(X)
	LD	a3,  2 * SIZE(X)
	LD	a4,  3 * SIZE(X)

	LD	a5,  4 * SIZE(X)
	daddiu	I, I, -1

	cvt.d.s	t1, a1
	LD	a6,  5 * SIZE(X)
	cvt.d.s	t2, a2
	LD	a7,  6 * SIZE(X)
	cvt.d.s	t3, a3
	LD	a8,  7 * SIZE(X)

	blez	I, .L13
	cvt.d.s	t4, a4
	.align 3

.L12:
	madd.d	s1, s1, t1, t1
	LD	a1,  8 * SIZE(X)

	cvt.d.s	t1, a5
	NOP

	madd.d	s2, s2, t2, t2
	LD	a2,  9 * SIZE(X)

	cvt.d.s	t2, a6
	NOP

	madd.d	s1, s1, t3, t3
	LD	a3, 10 * SIZE(X)

	cvt.d.s	t3, a7
	NOP

	madd.d	s2, s2, t4, t4
	LD	a4, 11 * SIZE(X)

	cvt.d.s	t4, a8
	NOP

	madd.d	s1, s1, t1, t1
	LD	a5, 12 * SIZE(X)

	cvt.d.s	t1, a1
	NOP

	madd.d	s2, s2, t2, t2
	LD	a6, 13 * SIZE(X)

	cvt.d.s	t2, a2
	daddiu	I, I, -1

	madd.d	s1, s1, t3, t3
	LD	a7, 14 * SIZE(X)

	cvt.d.s	t3, a3
	daddiu	X, X, 8 * SIZE

	madd.d	s2, s2, t4, t4
	LD	a8,  7 * SIZE(X)

	bgtz	I, .L12
	cvt.d.s	t4, a4
	.align 3

.L13:
	madd.d	s1, s1, t1, t1
	cvt.d.s	t1, a5

	madd.d	s2, s2, t2, t2
	cvt.d.s	t2, a6

	madd.d	s1, s1, t3, t3
	cvt.d.s	t3, a7

	madd.d	s2, s2, t4, t4
	cvt.d.s	t4, a8

	madd.d	s1, s1, t1, t1
	madd.d	s2, s2, t2, t2
	madd.d	s1, s1, t3, t3
	madd.d	s2, s2, t4, t4

	daddiu	X, X, 8 * SIZE
	.align 3

.L15:
	andi	I,  N, 7

	blez	I, .L999
	NOP
	.align	3

.L16:
	LD	a1,  0 * SIZE(X)
	daddiu	I, I, -1

	cvt.d.s	t1, a1
	
	madd.d	s1, s1, t1, t1

	bgtz	I, .L16
	daddiu	X, X, SIZE

	j	.L999
	NOP
	.align 3

.L20:
	blez	I, .L25
	NOP

	LD	a1,  0 * SIZE(X)
	daddu	X, X, INCX

	LD	a2,  0 * SIZE(X)
	daddu	X, X, INCX

	LD	a3,  0 * SIZE(X)
	daddu	X, X, INCX

	LD	a4,  0 * SIZE(X)
	daddu	X, X, INCX

	LD	a5,  0 * SIZE(X)
	daddu	X, X, INCX

	LD	a6,  0 * SIZE(X)
	daddu	X, X, INCX

	LD	a7,  0 * SIZE(X)
	daddu	X, X, INCX

	LD	a8,  0 * SIZE(X)
	daddiu	I, I, -1

	cvt.d.s	t1, a1
	cvt.d.s	t2, a2
	cvt.d.s	t3, a3
	cvt.d.s	t4, a4

	blez	I, .L24
	daddu	X, X, INCX
	.align 3

.L23:
	madd.d	s1, s1, t1, t1
	LD	a1,  0 * SIZE(X)

	cvt.d.s	t1, a5
	daddu	X, X, INCX

	madd.d	s2, s2, t2, t2
	LD	a2,  0 * SIZE(X)

	cvt.d.s	t2, a6
	daddu	X, X, INCX

	madd.d	s1, s1, t3, t3
	LD	a3,  0 * SIZE(X)

	cvt.d.s	t3, a7
	daddu	X, X, INCX

	madd.d	s2, s2, t4, t4
	LD	a4,  0 * SIZE(X)

	cvt.d.s	t4, a8
	daddu	X, X, INCX

	madd.d	s1, s1, t1, t1
	LD	a5,  0 * SIZE(X)

	cvt.d.s	t1, a1
	daddu	X, X, INCX

	madd.d	s2, s2, t2, t2
	LD	a6,  0 * SIZE(X)

	cvt.d.s	t2, a2
	daddu	X, X, INCX

	madd.d	s1, s1, t3, t3
	LD	a7,  0 * SIZE(X)

	cvt.d.s	t3, a3
	daddu	X, X, INCX

	madd.d	s2, s2, t4, t4
	LD	a8,  0 * SIZE(X)

	cvt.d.s	t4, a4
	daddiu	I, I, -1

	bgtz	I, .L23
	daddu	X, X, INCX
	.align 3

.L24:
	madd.d	s1, s1, t1, t1
	cvt.d.s	t1, a5

	madd.d	s2, s2, t2, t2
	cvt.d.s	t2, a6

	madd.d	s1, s1, t3, t3
	cvt.d.s	t3, a7

	madd.d	s2, s2, t4, t4
	cvt.d.s	t4, a8

	madd.d	s1, s1, t1, t1
	madd.d	s2, s2, t2, t2
	madd.d	s1, s1, t3, t3
	madd.d	s2, s2, t4, t4
	.align 3

.L25:
	andi	I,  N, 7

	blez	I, .L999
	NOP
	.align	3

.L26:
	LD	a1,  0 * SIZE(X)
	daddiu	I, I, -1

	cvt.d.s	t1, a1
	
	daddu	X, X, INCX

	bgtz	I, .L26
	madd.d	s1, s1, t1, t1
	.align 3

.L999:
	add.d	s1, s1, s2

	sqrt.d	s1, s1

	j	$31
	cvt.s.d	s1, s1
	
	EPILOGUE
