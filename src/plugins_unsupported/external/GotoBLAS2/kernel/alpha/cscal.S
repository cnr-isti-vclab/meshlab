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

	.set noat
	.set noreorder

#define ASSEMBLER

#include "common.h"
#include "version.h"

	.globl NAME
	.ent   NAME

NAME:
#ifdef PROFILE
	ldgp	$gp, 0($27)
	lda	$28, _mcount
	jsr	$28, ($28), _mcount
#endif

#ifndef C_INTERFACE
	ldl	$16,   0($16)		# n
	mov	$18,  $20		# Store Address
	ldl	$19,   0($19)		# incx
	nop

	LD	$f1,   0($17)		# alpha
#else
	mov	$18,  $20		# Store Address
	fmov	$f17, $f1		# alpha
#endif

#ifndef PROFILE
	.prologue 0
#else
	.prologue 1
#endif

	sra	$16,  1, $21		# 4-unrolling
	ble	$16,  $End

	lda	$23,  -1($19)
	ble	$19,  $End

	bgt	$23,  $INC_NOT_1
	.align 4

	ble	$21,  $Sub
	lda	$21,  -1($21)
	LD	$f10,   0*SIZE($18)
	LD	$f11,   1*SIZE($18)

	LD	$f12,   2*SIZE($18)
	LD	$f13,   3*SIZE($18)
	lda	$18,    4*SIZE($18)
	ble	$21,  $MainRemain
	.align 4

$MainLoop:
	MUL	$f10, $f1,  $f20
	LD	$f10,   0*SIZE($18)
	MUL	$f11, $f1,  $f21
	LD	$f11,   1*SIZE($18)

	MUL	$f12, $f1,  $f22
	LD	$f12,   2*SIZE($18)
	MUL	$f13, $f1,  $f23
	LD	$f13,   3*SIZE($18)

	lda	$18,    4*SIZE($18)
	lda	$21,   -1($21)

	ST	$f20,   0*SIZE($20)
	ST	$f21,   1*SIZE($20)
	ST	$f22,   2*SIZE($20)
	ST	$f23,   3*SIZE($20)
	lda	$20,    4*SIZE($20)

	bgt	$21, $MainLoop
	.align 4

$MainRemain:
	MUL	$f10, $f1,  $f20
	MUL	$f11, $f1,  $f21
	MUL	$f12, $f1,  $f22
	MUL	$f13, $f1,  $f23

	ST	$f20,   0*SIZE($20)
	ST	$f21,   1*SIZE($20)
	ST	$f22,   2*SIZE($20)
	ST	$f23,   3*SIZE($20)
	lda	$20,    4*SIZE($20)
	.align 4

$Sub:
	blbc	$16, $End
	LD	$f10,      0*SIZE($18)
	LD	$f11,      1*SIZE($18)
	MUL	$f10, $f1,  $f20
	MUL	$f11, $f1,  $f21
	ST	$f20,      0*SIZE($20)
	ST	$f21,      1*SIZE($20)
	.align 4

$End:
	ret
	.align 4

$INC_NOT_1:
	addl	$19, $19, $19
	ble	$21,  $INC_Sub
	lda	$21,  -1($21)

	LD	$f10,   0*SIZE($18)
	LD	$f11,   1*SIZE($18)
	SXADDQ	$19, $18, $18

	LD	$f12,   0*SIZE($18)
	LD	$f13,   1*SIZE($18)
	SXADDQ	$19, $18, $18
	ble	$21,  $INC_MainRemain
	.align 4

$INC_MainLoop:
	MUL	$f10, $f1,  $f20
	LD	$f10,   0*SIZE($18)
	MUL	$f11, $f1,  $f21
	LD	$f11,   1*SIZE($18)

	SXADDQ	$19, $18, $18

	MUL	$f12, $f1,  $f22
	LD	$f12,   0*SIZE($18)
	MUL	$f13, $f1,  $f23
	LD	$f13,   1*SIZE($18)

	SXADDQ	$19, $18, $18

	ST	$f20,   0*SIZE($20)
	lda	$21,   -1($21)
	ST	$f21,   1*SIZE($20)
	SXADDQ	$19, $20, $20

	ST	$f22,   0*SIZE($20)
	ST	$f23,   1*SIZE($20)
	SXADDQ	$19, $20, $20
	unop
	bgt	$21, $INC_MainLoop
	.align 4

$INC_MainRemain:
	MUL	$f10, $f1,  $f20
	MUL	$f11, $f1,  $f21
	MUL	$f12, $f1,  $f22
	MUL	$f13, $f1,  $f23

	ST	$f20,   0*SIZE($20)
	ST	$f21,   1*SIZE($20)
	SXADDQ	$19, $20, $20

	ST	$f22,   0*SIZE($20)
	ST	$f23,   1*SIZE($20)
	SXADDQ	$19, $20, $20
	.align 4

$INC_Sub:
	blbc	$16, $INC_End

	LD	$f10,   0*SIZE($18)
	LD	$f11,   1*SIZE($18)
	MUL	$f10, $f1,  $f20
	MUL	$f11, $f1,  $f21

	ST	$f20,   0*SIZE($20)
	ST	$f21,   1*SIZE($20)
	.align 4

$INC_End:
	ret
	.end NAME
	.ident VERSION
