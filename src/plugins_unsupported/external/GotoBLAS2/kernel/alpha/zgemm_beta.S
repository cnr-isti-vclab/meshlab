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

	.set noat
	.set noreorder
.text
	.align 5
	.globl CNAME
	.ent CNAME
CNAME:
	.frame	$sp, 0, $26, 0

#ifdef PROFILE
	ldgp	$gp, 0($27)
	lda	$28, _mcount
	jsr	$28, ($28), _mcount
	.prologue 1
#else
	.prologue 0
#endif

	ldq	$18,   24($sp)
	ble	$16, $End
	ldl	$19,   32($sp)
	ble	$17, $End

	addq	$19, $19, $19
	fbne	$f19,$Main
	fbne	$f20,$Main
	.align 4

$L13:
	mov	$18, $1
	lda	$17,  -1($17)
	SXADDQ	$19, $18, $18
	mov	$16, $2
	.align 4

$L12:
	ST	$f31, 0*SIZE($1)
	ST	$f31, 1*SIZE($1)
	lda	$2,  -1($2)
	lda	$1,   2*SIZE($1)
	bgt	$2, $L12
	bgt	$17,$L13
	clr	$0
	ret
	.align 4

/* Main Routine */
$Main:
	sra	$16, 1, $2		# $2 = (m >> 1)
	mov	$18, $1			# c_offset = c
	lda	$17,  -1($17)		# n --
	SXADDQ	$19, $18, $18		# c += ldc
	beq	$2, $L18

	LD	$f14, 0*SIZE($1)
	LD	$f15, 1*SIZE($1)
	LD	$f24, 2*SIZE($1)
	LD	$f25, 3*SIZE($1)
	lda	$2,    -1($2)		# $2 --
	ble	$2, $L19
	.align 4


$L23:
	MUL	$f19, $f14, $f10
	lds	$f31,  9*SIZE($1)
	MUL	$f20, $f15, $f11
	lda	$2,    -1($2)

	MUL	$f19, $f15, $f12
	LD	$f15, 5*SIZE($1)
	MUL	$f20, $f14, $f13
	LD	$f14, 4*SIZE($1)

	MUL	$f19, $f24, $f16
	unop
	MUL	$f20, $f25, $f17
	unop

	MUL	$f19, $f25, $f18
	LD	$f25, 7*SIZE($1)
	SUB	$f10, $f11, $f22
	unop

	MUL	$f20, $f24, $f21
	LD	$f24, 6*SIZE($1)
	ADD	$f12, $f13, $f23
	lda	$1,   4*SIZE($1)

	SUB	$f16, $f17, $f26
	ADD	$f18, $f21, $f27
	ST	$f22,-4*SIZE($1)
	ST	$f23,-3*SIZE($1)

	ST	$f26,-2*SIZE($1)
	ST	$f27,-1*SIZE($1)
	unop
	bgt	$2,$L23
	.align 4

$L19:
	MUL	$f19, $f14, $f10
	MUL	$f20, $f15, $f11
	MUL	$f19, $f15, $f12
	MUL	$f20, $f14, $f13

	MUL	$f19, $f24, $f16
	MUL	$f20, $f25, $f17
	MUL	$f19, $f25, $f18
	MUL	$f20, $f24, $f21

	SUB	$f10, $f11, $f22
	ADD	$f12, $f13, $f23
	SUB	$f16, $f17, $f26
	ADD	$f18, $f21, $f27
	lda	$1,   4*SIZE($1)

	ST	$f22, -4*SIZE($1)
	ST	$f23, -3*SIZE($1)
	ST	$f26, -2*SIZE($1)
	ST	$f27, -1*SIZE($1)

	blbs	$16, $L18
	bgt	$17, $Main
	clr	$0
	ret
	.align 4

$L18:
	LD	$f14, 0*SIZE($1)
	LD	$f15, 1*SIZE($1)
	MUL	$f19, $f15, $f13
	MUL	$f20, $f14, $f10

	MUL	$f19, $f14, $f12
	MUL	$f20, $f15, $f11
	ADD	$f13, $f10, $f26
	SUB	$f12, $f11, $f27

	ST	$f26, 1*SIZE($1)
	ST	$f27, 0*SIZE($1)
	lda	$1,   2*SIZE($1)
	bgt	$17, $Main
	.align 4

$End:
	clr	$0
	ret
	.ident	VERSION
	.end CNAME
