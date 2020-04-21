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

#define N	$16
#define X	$17
#define INCX	$18

#ifndef USE_MIN
#define CMPLT(a, b) cmptlt a, b
#else
#define CMPLT(a, b) cmptlt b, a
#endif

#define STACKSIZE 8 * 8

	PROLOGUE
	PROFCODE
	.frame	$sp, STACKSIZE, $26, 0

#ifdef F_INTERFACE
	ldl	N,     0(N)		# n
	ldl	INCX,  0(INCX)		# incx
#endif
	lda	$sp, -STACKSIZE($sp)
	nop
	.align 4

	cmplt	$31, N,    $2
	cmplt	$31, INCX, $3
	SXADDQ	INCX, $31, INCX
	and	$2,  $3,  $0

	sra	N, 3, $1
	fclr	$f0
	unop
	beq	$0,  $End		# if (n <= 0) or (incx <= 0) return
	.align 4

	LD	$f0,  0 * SIZE(X)
	unop
	unop
	ble	$1,  $L15
	.align 4

	fmov	$f0,  $f1
	addq	X, INCX, X
	fmov	$f0,  $f10
	lda	$1,  -1($1)

	LD	$f21,  0 * SIZE(X)
	fmov	$f0,  $f11
	addq	X, INCX, X
	fmov	$f0,  $f12

	LD	$f22,  0 * SIZE(X)
	fmov	$f0,  $f13
	addq	X, INCX, X
	fmov	$f0,  $f14

	LD	$f23,  0 * SIZE(X)
	fmov	$f0,  $f15
	addq	X, INCX, X
	fmov	$f0,  $f20

	LD	$f24,  0 * SIZE(X)
	addq	X, INCX, X
	LD	$f25,  0 * SIZE(X)
	addq	X, INCX, X
	LD	$f26,  0 * SIZE(X)
	addq	X, INCX, X
	LD	$f27,  0 * SIZE(X)
	addq	X, INCX, X

	CMPLT($f0,  $f20), $f16
	CMPLT($f1,  $f21), $f17
	CMPLT($f10, $f22), $f18
	CMPLT($f11, $f23), $f19

	ble	$1, $L13
	.align 4

$L12:
	fcmovne	$f16, $f20, $f0
	LD	$f20,  0 * SIZE(X)
	CMPLT($f12,  $f24), $f16
	addq	X, INCX, X

	fcmovne	$f17, $f21, $f1
	LD	$f21,  0 * SIZE(X)
	CMPLT($f13,  $f25), $f17
	addq	X, INCX, X

	fcmovne	$f18, $f22, $f10
	LD	$f22,  0 * SIZE(X)
	CMPLT($f14,  $f26), $f18
	addq	X, INCX, X

	fcmovne	$f19, $f23, $f11
	LD	$f23,  0 * SIZE(X)
	CMPLT($f15,  $f27), $f19
	addq	X, INCX, X

	fcmovne	$f16, $f24, $f12
	LD	$f24,  0 * SIZE(X)
	CMPLT($f0,  $f20), $f16
	addq	X, INCX, X

	fcmovne	$f17, $f25, $f13
	LD	$f25,  0 * SIZE(X)
	CMPLT($f1,  $f21), $f17
	addq	X, INCX, X

	fcmovne	$f18, $f26, $f14
	LD	$f26,  0 * SIZE(X)
	CMPLT($f10,  $f22), $f18
	addq	X, INCX, X

	fcmovne	$f19, $f27, $f15
	LD	$f27,  0 * SIZE(X)
	CMPLT($f11,  $f23), $f19
	lda	$1,   -1($1)		# i --

	addq	X, INCX, X
	unop
	unop
	bgt	$1,$L12
	.align 4

$L13:
	fcmovne	$f16, $f20, $f0
	CMPLT($f12,  $f24), $f16

	fcmovne	$f17, $f21, $f1
	CMPLT($f13,  $f25), $f17

	fcmovne	$f18, $f22, $f10
	CMPLT($f14,  $f26), $f18

	fcmovne	$f19, $f23, $f11
	CMPLT($f15,  $f27), $f19

	fcmovne	$f16, $f24, $f12
	CMPLT($f0,  $f1), $f16
	fcmovne	$f17, $f25, $f13
	CMPLT($f10,  $f11), $f17

	fcmovne	$f18, $f26, $f14
	CMPLT($f12,  $f13), $f18
	fcmovne	$f19, $f27, $f15
	CMPLT($f14,  $f15), $f19

	fcmovne	$f16, $f1, $f0
	fcmovne	$f17, $f11, $f10
	fcmovne	$f18, $f13, $f12
	fcmovne	$f19, $f15, $f14

	CMPLT($f0,  $f10), $f16
	CMPLT($f12,  $f14), $f17

	fcmovne	$f16, $f10, $f0
	fcmovne	$f17, $f14, $f12

	CMPLT($f0,  $f12), $f16
	fcmovne	$f16, $f12, $f0
	.align 4

$L15:
	and	N, 7, $1
	unop
	unop
	ble	$1,  $End
	.align 4

$L16:
	LD	$f20,  0 * SIZE(X)
	addq	X, INCX, X

	CMPLT($f0,  $f20), $f16
	fcmovne	$f16, $f20, $f0
	lda	$1,   -1($1)		# i --
	bgt	$1, $L16
	.align 4

$End:
	lda	$sp,  STACKSIZE($sp)
	ret

	EPILOGUE
