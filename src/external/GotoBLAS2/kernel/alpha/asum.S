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
#include "version.h"

#define PREFETCHSIZE	88

#define N	$16
#define X	$17
#define INCX	$18
#define I	$19

#define s0	$f0
#define s1	$f1
#define s2	$f10
#define s3	$f11

#define a0	$f12
#define a1	$f13
#define a2	$f14
#define a3	$f15
#define a4	$f16
#define a5	$f17
#define a6	$f18
#define a7	$f19

#define t0	$f20
#define t1	$f21
#define t2	$f22
#define t3	$f23

	PROLOGUE
	PROFCODE

	fclr	s0
	unop
	fclr	t0
	ble	N,  $L999

	sra	N, 3, I
	fclr	s1
	fclr	s2
	ble	I, $L15

	LD	a0,  0 * SIZE(X)
	fclr	t1
	SXADDQ	INCX, X, X
	fclr	t2

	LD	a1,  0 * SIZE(X)
	fclr	t3
	SXADDQ	INCX, X, X
	fclr	s3

	LD	a2,  0 * SIZE(X)
	SXADDQ	INCX, X, X
	LD	a3,  0 * SIZE(X)
	SXADDQ	INCX, X, X

	LD	a4,  0 * SIZE(X)
	SXADDQ	INCX, X, X
	LD	a5,  0 * SIZE(X)
	SXADDQ	INCX, X, X

	lda	I,  -1(I)
	ble	I, $L13
	.align 4

$L12:
	ADD	s0, t0, s0
	ldl	$31, PREFETCHSIZE * 2 * SIZE(X)
	fabs	a0, t0
	lda	I,  -1(I)

	ADD	s1, t1, s1
	LD	a6,  0 * SIZE(X)
	fabs	a1, t1
	SXADDQ	INCX, X, X

	ADD	s2, t2, s2
	LD	a7,  0 * SIZE(X)
	fabs	a2, t2
	SXADDQ	INCX, X, X

	ADD	s3, t3, s3
	LD	a0,  0 * SIZE(X)
	fabs	a3, t3
	SXADDQ	INCX, X, X

	ADD	s0, t0, s0
	LD	a1,  0 * SIZE(X)
	fabs	a4, t0
	SXADDQ	INCX, X, X

	ADD	s1, t1, s1
	LD	a2,  0 * SIZE(X)
	fabs	a5, t1
	SXADDQ	INCX, X, X

	ADD	s2, t2, s2
	LD	a3,  0 * SIZE(X)
	fabs	a6, t2
	SXADDQ	INCX, X, X

	ADD	s3, t3, s3
	LD	a4,  0 * SIZE(X)
	fabs	a7, t3
	SXADDQ	INCX, X, X

	LD	a5,  0 * SIZE(X)
	unop
	SXADDQ	INCX, X, X
	bne	I, $L12
	.align 4

$L13:
	ADD	s0, t0, s0
	LD	a6,  0 * SIZE(X)
	fabs	a0, t0
	SXADDQ	INCX, X, X

	ADD	s1, t1, s1
	LD	a7,  0 * SIZE(X)
	fabs	a1, t1
	SXADDQ	INCX, X, X

	ADD	s2, t2, s2
	fabs	a2, t2
	ADD	s3, t3, s3
	fabs	a3, t3

	ADD	s0, t0, s0
	fabs	a4, t0
	ADD	s1, t1, s1
	fabs	a5, t1
	ADD	s2, t2, s2
	fabs	a6, t2
	ADD	s3, t3, s3
	fabs	a7, t3

	ADD	s1, t1, s1
	ADD	s2, t2, s2
	ADD	s3, t3, s3

	ADD	s0, s1, s0
	ADD	s2, s3, s2
	.align 4

$L15:
	and	N, 7, I
	ADD	s0, s2, s0
	unop
	ble	I, $L999
	.align 4

$L17:
	ADD	s0, t0, s0
	LD	a0,  0 * SIZE(X)
	SXADDQ	INCX, X, X
	fabs	a0, t0

	lda	I,  -1(I)
	bne	I, $L17
	.align 4

$L999:
	ADD	s0, t0, s0
	ret
	EPILOGUE
