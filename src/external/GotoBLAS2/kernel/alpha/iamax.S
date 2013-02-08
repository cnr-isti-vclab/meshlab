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

#define STACKSIZE 6 * 8

	PROLOGUE
	PROFCODE
	.frame	$sp, STACKSIZE, $26, 0

#ifdef F_INTERFACE
	ldl	N,     0(N)		# n
	ldl	INCX,  0(INCX)		# incx
#endif
	lda	$sp, -STACKSIZE($sp)
	mov	X, XX
	.align 4

	stt	$f2,   0($sp)
	fclr	$f16
	cmplt	$31, N,    $2
	unop

	stt	$f3,   8($sp)
	fclr	$f17
	cmplt	$31, INCX, $3
	unop

	stt	$f4,  16($sp)
	fclr	$f18
	SXADDQ	INCX, $31, INCX
	unop

	stt	$f5,  24($sp)
	fclr	$f19
	and	$2,  $3,  $2
	clr	$0

	stt	$f6,  32($sp)
	fclr	$f0
	sra	N, 3, $1
	beq	$2,  $End		# if (n <= 0) or (incx <= 0) return
	.align 4

	LD	$f20,  0 * SIZE(X)
	unop
	fabs	$f20, $f0
	ble	$1,  $L15
	.align 4

	fabs	$f20, $f1
	unop
	addq	X, INCX, X
	unop

	LD	$f21,  0 * SIZE(X)
	fabs	$f20, $f2
	addq	X, INCX, X
	unop

	LD	$f22,  0 * SIZE(X)
	fabs	$f20, $f3
	addq	X, INCX, X
	unop

	LD	$f23,  0 * SIZE(X)
	fabs	$f20, $f4
	addq	X, INCX, X
	unop

	LD	$f24,  0 * SIZE(X)
	addq	X, INCX, X
	fabs	$f20, $f5
	unop

	LD	$f25,  0 * SIZE(X)
	fabs	$f20, $f6
	addq	X, INCX, X
	unop

	LD	$f26,  0 * SIZE(X)
	fabs	$f20, $f28
	addq	X, INCX, X
	lda	$1,  -1($1)

	LD	$f27,  0 * SIZE(X)
	unop
	addq	X, INCX, X
	ble	$1, $L13
	.align 4

$L12:
	fcmovne	$f16, $f12, $f4
	unop
	fabs	$f20, $f29
	ldl	$31, 56 * SIZE(X)

	fcmovne	$f17, $f13, $f5
	LD	$f20,  0 * SIZE(X)
	fabs	$f21, $f30
	addq	X, INCX, X

	fcmovne	$f18, $f14, $f6
	LD	$f21,  0 * SIZE(X)
	fabs	$f22, $f10
	addq	X, INCX, X

	fcmovne	$f19, $f15, $f28
	LD	$f22,  0 * SIZE(X)
	fabs	$f23, $f11
	addq	X, INCX, X

	fabs	$f24, $f12
	LD	$f23,  0 * SIZE(X)
	CMPLT($f0,  $f29),  $f16
	addq	X, INCX, X

	fabs	$f25, $f13
	LD	$f24,  0 * SIZE(X)
	CMPLT($f1,  $f30),  $f17
	addq	X, INCX, X

	fabs	$f26, $f14
	LD	$f25,  0 * SIZE(X)
	CMPLT($f2,  $f10), $f18
	addq	X, INCX, X

	fabs	$f27, $f15
	LD	$f26,  0 * SIZE(X)
	CMPLT($f3,  $f11), $f19
	addq	X, INCX, X

	fcmovne	$f16, $f29,  $f0
	LD	$f27,  0 * SIZE(X)
	CMPLT($f4,  $f12), $f16
	addq	X, INCX, X

	fcmovne	$f17, $f30,  $f1
	unop
	CMPLT($f5,  $f13), $f17
	lda	$1,   -1($1)		# i --

	fcmovne	$f18, $f10, $f2
	unop
	CMPLT($f6,  $f14), $f18
	unop

	fcmovne	$f19, $f11, $f3
	unop
	CMPLT($f28,  $f15), $f19
	bgt	$1,$L12
	.align 4

$L13:
	fcmovne	$f16, $f12, $f4
	fabs	$f20, $f29
	fcmovne	$f17, $f13, $f5
	fabs	$f21, $f30

	fcmovne	$f18, $f14, $f6
	fabs	$f22, $f10
	fcmovne	$f19, $f15, $f28
	fabs	$f23, $f11

	fabs	$f24, $f12
	CMPLT($f0,  $f29),  $f16
	fabs	$f25, $f13
	CMPLT($f1,  $f30),  $f17

	fabs	$f26, $f14
	CMPLT($f2,  $f10), $f18
	fabs	$f27, $f15
	CMPLT($f3,  $f11), $f19

	fcmovne	$f16, $f29,  $f0
	CMPLT($f4,  $f12), $f16
	fcmovne	$f17, $f30,  $f1
	CMPLT($f5,  $f13), $f17

	fcmovne	$f18, $f10, $f2
	CMPLT($f6,  $f14), $f18
	fcmovne	$f19, $f11, $f3
	CMPLT($f28,  $f15), $f19

	fcmovne	$f16, $f12, $f4
	CMPLT($f0,  $f1), $f16
	fcmovne	$f17, $f13, $f5
	CMPLT($f2,  $f3), $f17

	fcmovne	$f18, $f14, $f6
	CMPLT($f4,  $f5), $f18
	fcmovne	$f19, $f15, $f28
	CMPLT($f6,  $f28), $f19

	fcmovne	$f16, $f1, $f0
	fcmovne	$f17, $f3, $f2
	fcmovne	$f18, $f5, $f4
	fcmovne	$f19, $f28, $f6

	CMPLT($f0,  $f2), $f16
	CMPLT($f4,  $f6), $f17

	fcmovne	$f16, $f2, $f0
	fcmovne	$f17, $f6, $f4

	CMPLT($f0,  $f4), $f16
	fcmovne	$f16, $f4, $f0
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

	fabs	$f20, $f29
	CMPLT($f0,  $f29), $f16
	fcmovne	$f16, $f29, $f0

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

	fabs	$f10, $f18
	fabs	$f11, $f19
	fabs	$f12, $f20
	fabs	$f13, $f21

	lda	$1,  -1($1)
	ble	$1, $L23
	.align 4

$L22:
	LD	$f10,  0 * SIZE(XX)
	fabs	$f14, $f22
	addq	XX, INCX, XX
	cmpteq	$f0, $f18, $f2	

	LD	$f11,  0 * SIZE(XX)
	fabs	$f15, $f23
	addq	XX, INCX, XX
	cmpteq	$f0, $f19, $f3

	LD	$f12,  0 * SIZE(XX)
	fabs	$f16, $f24
	addq	XX, INCX, XX
	cmpteq	$f0, $f20, $f4

	LD	$f13,  0 * SIZE(XX)
	fabs	$f17, $f25
	addq	XX, INCX, XX
	cmpteq	$f0, $f21, $f5

	LD	$f14,  0 * SIZE(XX)
	lda	$1,   -1($1)		# i --
	cmpteq	$f0, $f22, $f26
	addq	XX, INCX, XX

	lda	$0,    1($0)
	fbne	$f2,  $End

	LD	$f15,  0 * SIZE(XX)
	cmpteq	$f0, $f23, $f27
	lda	$0,    1($0)
	fbne	$f3,  $End

	addq	XX, INCX, XX
	cmpteq	$f0, $f24, $f28
	lda	$0,    1($0)
	fbne	$f4,  $End

	LD	$f16,  0 * SIZE(XX)
	cmpteq	$f0, $f25, $f29
	lda	$0,    1($0)
	fbne	$f5,  $End

	addq	XX, INCX, XX
	lda	$0,    1($0)
	fabs	$f10, $f18
	fbne	$f26, $End

	LD	$f17,  0 * SIZE(XX)
	lda	$0,    1($0)
	fabs	$f11, $f19
	fbne	$f27, $End

	addq	XX, INCX, XX
	lda	$0,    1($0)
	fabs	$f12, $f20
	fbne	$f28, $End

	lda	$0,    1($0)
	fabs	$f13, $f21
	fbne	$f29, $End
	bgt	$1,  $L22
	.align 4

$L23:
	fabs	$f14, $f22
	cmpteq	$f0, $f18, $f2	
	fabs	$f15, $f23
	cmpteq	$f0, $f19, $f3

	fabs	$f16, $f24
	cmpteq	$f0, $f20, $f4
	fabs	$f17, $f25
	cmpteq	$f0, $f21, $f5

	cmpteq	$f0, $f22, $f26
	lda	$0,    1($0)
	unop
	fbne	$f2,  $End

	cmpteq	$f0, $f23, $f27
	lda	$0,    1($0)
	unop
	fbne	$f3,  $End

	cmpteq	$f0, $f24, $f28
	lda	$0,    1($0)
	unop
	fbne	$f4,  $End

	cmpteq	$f0, $f25, $f29
	lda	$0,    1($0)
	unop
	fbne	$f5,  $End

	lda	$0,    1($0)
	fbne	$f26, $End
	lda	$0,    1($0)
	fbne	$f27, $End
	lda	$0,    1($0)
	fbne	$f28, $End
	lda	$0,    1($0)
	fbne	$f29, $End
	.align 4

$L40:
	LD	$f20,  0 * SIZE(XX)
	addq	XX, INCX, XX

	fabs	$f20, $f25
	cmpteq	$f0, $f25, $f29

	lda	$0,    1($0)
	fbne	$f29,  $End
	br	$31, $L40
	.align 4

$End:
	ldt	$f2,   0($sp)
	ldt	$f3,   8($sp)
	ldt	$f4,  16($sp)
	ldt	$f5,  24($sp)

	ldt	$f6,  32($sp)
	lda	$sp,  STACKSIZE($sp)
	ret

	EPILOGUE
