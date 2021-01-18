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
#define XX	$19
	
#ifndef USE_MIN
#define CMPLT(a, b) cmptlt a, b
#else
#define CMPLT(a, b) cmptlt b, a
#endif

#define STACKSIZE 8 * 8

	PROLOGUE
	PROFCODE

	clr	$0
	mov	X, XX
	.align 4

	cmplt	$31, N,    $2
	cmplt	$31, INCX, $3
	SXADDQ	INCX, $31, INCX
	and	$2,  $3,  $2

	sra	N, 3, $1
	fclr	$f0
	unop
	beq	$2,  $End		# if (n <= 0) or (incx <= 0) return
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
	ble	$1,  $L20
	.align 4

$L16:
	LD	$f20,  0 * SIZE(X)
	addq	X, INCX, X

	CMPLT($f0,  $f20), $f16
	fcmovne	$f16, $f20, $f0
	lda	$1,   -1($1)		# i --
	bgt	$1, $L16
	.align 4

$L20:
	sra	N, 3, $1
	ble	$1,  $L40
	.align 4

	LD	$f10,  0 * SIZE(XX)
	addq	XX, INCX, XX
	LD	$f11,  0 * SIZE(XX)
	addq	XX, INCX, XX

	LD	$f12,  0 * SIZE(XX)
	addq	XX, INCX, XX
	LD	$f13,  0 * SIZE(XX)
	addq	XX, INCX, XX

	LD	$f14,  0 * SIZE(XX)
	addq	XX, INCX, XX
	LD	$f15,  0 * SIZE(XX)
	addq	XX, INCX, XX

	LD	$f16,  0 * SIZE(XX)
	addq	XX, INCX, XX
	LD	$f17,  0 * SIZE(XX)
	addq	XX, INCX, XX

	cmpteq	$f0, $f10, $f20
	cmpteq	$f0, $f11, $f21
	cmpteq	$f0, $f12, $f22
	cmpteq	$f0, $f13, $f23

	lda	$1,  -1($1)
	ble	$1, $L23
	.align 4

$L22:
	LD	$f10,  0 * SIZE(XX)
	cmpteq	$f0, $f14, $f24
	lda	$0,    1($0)
	addq	XX, INCX, XX
	fbne	$f20,  $End

	LD	$f11,  0 * SIZE(XX)
	cmpteq	$f0, $f15, $f25
	lda	$0,    1($0)
	addq	XX, INCX, XX
	fbne	$f21,  $End

	LD	$f12,  0 * SIZE(XX)
	cmpteq	$f0, $f16, $f26
	lda	$0,    1($0)
 	addq	XX, INCX, XX
	fbne	$f22,  $End

	LD	$f13,  0 * SIZE(XX)
	cmpteq	$f0, $f17, $f27
	lda	$0,    1($0)
	addq	XX, INCX, XX
	fbne	$f23,  $End

	LD	$f14,  0 * SIZE(XX)
	cmpteq	$f0, $f10, $f20
	lda	$0,    1($0)
	addq	XX, INCX, XX
	fbne	$f24, $End

	LD	$f15,  0 * SIZE(XX)
	cmpteq	$f0, $f11, $f21
	lda	$0,    1($0)
	addq	XX, INCX, XX
	fbne	$f25, $End

	LD	$f16,  0 * SIZE(XX)
	lda	$1,   -1($1)		# i --
	cmpteq	$f0, $f12, $f22
	lda	$0,    1($0)
	addq	XX, INCX, XX
	fbne	$f26, $End

	LD	$f17,  0 * SIZE(XX)
	cmpteq	$f0, $f13, $f23
	lda	$0,    1($0)
	addq	XX, INCX, XX
	fbne	$f27, $End

	bgt	$1,  $L22
	.align 4

$L23:
	lda	$0,    1($0)
	cmpteq	$f0, $f14, $f24
	unop
	fbne	$f20,  $End

	lda	$0,    1($0)
	cmpteq	$f0, $f15, $f25
	unop
	fbne	$f21,  $End

	lda	$0,    1($0)
	cmpteq	$f0, $f16, $f26
	unop
	fbne	$f22,  $End

	lda	$0,    1($0)
	cmpteq	$f0, $f17, $f27
	unop
	fbne	$f23,  $End

	lda	$0,    1($0)
	fbne	$f24, $End
	lda	$0,    1($0)
	fbne	$f25, $End
	lda	$0,    1($0)
	fbne	$f26, $End
	lda	$0,    1($0)
	fbne	$f27, $End
	.align 4

$L40:
	LD	$f20,  0 * SIZE(XX)
	addq	XX, INCX, XX

	cmpteq	$f0, $f20, $f29

	lda	$0,    1($0)
	fbne	$f29,  $End
	br	$31, $L40
	.align 4

$End:
	ret

	EPILOGUE
