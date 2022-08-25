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
	
#define I	$3
#define TEMP	$7

#define a1	$f4
#define a2	$f5
#define a3	$f6
#define a4	$f7
#define a5	$f8
#define a6	$f9
#define a7	$f10
#define a8	$f11

#define t1	$f12
#define t2	$f13
#define t3	$f14
#define t4	$f15

#define s1	$f0
#define s2	$f1
#define s3	$f2
#define s4	$f3

#define	x1	$2
#define	x2	$8
#define	x3	$9
#define	x4	$10
	
	PROLOGUE
	
#ifdef F_INTERFACE
	LDINT	N,     0(N)
	LDINT	INCX,  0(INCX)
#endif

	blez	N, .L999
	li	x1, 0

	blez	INCX, .L999
	dsll	INCX, INCX, BASE_SHIFT

	LD	s1,  0 * SIZE(X)
	daddiu	N, N, -1

	blez	N, .L999
	li	x1, 1

	daddu	X, X, INCX
	MOV	s2, s1
	li	x2, 1

	MOV	s3, s1
	dsra	I, N, 3
	MOV	s4, s1
	li	x3, 1

	li	TEMP, 2

	blez	I, .L15
	li	x4, 1

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
	daddiu	I, I, -1

	blez	I, .L13
	daddu	X, X, INCX
	.align 3

.L12:
	CMPLT	$fcc0, a1, s1
	LD	a7,  0 * SIZE(X)
	CMPLT	$fcc1, a2, s2
	daddu	X, X, INCX
	CMPLT	$fcc2, a3, s3
	LD	a8,  0 * SIZE(X)
	CMPLT	$fcc3, a4, s4
	daddu	X, X, INCX

	CMOVT	s1, a1, $fcc0
	movt	x1, TEMP, $fcc0
	CMOVT	s2, a2, $fcc1
	movt	x2, TEMP, $fcc1

	LD	a1,  0 * SIZE(X)
	daddu	X, X, INCX
	LD	a2,  0 * SIZE(X)
	daddu	X, X, INCX

	CMOVT	s3, a3, $fcc2
	movt	x3, TEMP, $fcc2
	CMOVT	s4, a4, $fcc3
	movt	x4, TEMP, $fcc3

	daddiu	TEMP, TEMP, 4
	daddiu	I, I, -1

	CMPLT	$fcc0, a5, s1
	LD	a3,  0 * SIZE(X)
	CMPLT	$fcc1, a6, s2
	daddu	X, X, INCX

	CMPLT	$fcc2, a7, s3
	LD	a4,  0 * SIZE(X)
	CMPLT	$fcc3, a8, s4
	daddu	X, X, INCX

	CMOVT	s1, a5, $fcc0
	movt	x1, TEMP, $fcc0
	CMOVT	s2, a6, $fcc1
	movt	x2, TEMP, $fcc1

	CMOVT	s3, a7, $fcc2
	movt	x3, TEMP, $fcc2
	CMOVT	s4, a8, $fcc3
	movt	x4, TEMP, $fcc3

	LD	a5,  0 * SIZE(X)
	daddu	X, X, INCX
	LD	a6,  0 * SIZE(X)
	daddu	X, X, INCX

	bgtz	I, .L12
	daddiu	TEMP, TEMP, 4
	.align 3

.L13:
	CMPLT	$fcc0, a1, s1
	LD	a7,  0 * SIZE(X)
	CMPLT	$fcc1, a2, s2
	daddu	X, X, INCX
	CMPLT	$fcc2, a3, s3
	LD	a8,  0 * SIZE(X)
	CMPLT	$fcc3, a4, s4
	daddu	X, X, INCX

	CMOVT	s1, a1, $fcc0
	movt	x1, TEMP, $fcc0
	CMOVT	s2, a2, $fcc1
	movt	x2, TEMP, $fcc1

	CMOVT	s3, a3, $fcc2
	movt	x3, TEMP, $fcc2
	CMOVT	s4, a4, $fcc3
	movt	x4, TEMP, $fcc3

	CMPLT	$fcc0, a5, s1
	daddiu	TEMP, TEMP, 4
	CMPLT	$fcc1, a6, s2
	NOP

	CMPLT	$fcc2, a7, s3
	CMPLT	$fcc3, a8, s4

	CMOVT	s1, a5, $fcc0
	movt	x1, TEMP, $fcc0
	CMOVT	s2, a6, $fcc1
	movt	x2, TEMP, $fcc1

	CMOVT	s3, a7, $fcc2
	movt	x3, TEMP, $fcc2
	CMOVT	s4, a8, $fcc3
	movt	x4, TEMP, $fcc3

	daddiu	TEMP, TEMP, 4
	daddiu	x2, x2, 1
	daddiu	x3, x3, 2
	daddiu	x4, x4, 3
	.align 3

.L15:
	andi	I,  N, 7
	blez	I, .L998
	NOP
	.align	3

.L16:
	LD	a1,  0 * SIZE(X)
	daddu	X, X, INCX
	daddiu	I, I, -1

	CMPLT	$fcc0, a1, s1
	NOP
	CMOVT	s1, a1, $fcc0
	movt	x1, TEMP, $fcc0

	bgtz	I, .L16
	daddiu	TEMP, TEMP, 1
	.align 3

.L998:
	CMPLT	$fcc0, s2, s1
	CMPLT	$fcc1, s4, s3

	CMOVT	s1, s2, $fcc0
	movt	x1, x2, $fcc0
	CMOVT	s3, s4, $fcc1
	movt	x3, x4, $fcc1

	CMPLT	$fcc0, s3, s1
	CMOVT	s1, s3, $fcc0
	movt	x1, x3, $fcc0
	.align 3

.L999:
	j	$31
	NOP

	EPILOGUE
