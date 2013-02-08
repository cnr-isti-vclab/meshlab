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

	PROLOGUE
	PROFCODE
	.frame	$sp, 0, $26, 0

	mov	$20, $17
	mov	$21, $18
	ldq	$19,  0($sp)
	ldl	$20,  8($sp)
#ifndef PROFILE
	.prologue 0
#else
	.prologue 1
#endif

	subl	$18, 1, $1
	subl	$20, 1, $2
	ble	$16,  $SubEnd		# if n <= 0 goto $End
	or	$1, $2, $1

	sra	$16, 3, $21

	and	$16, 7, $22
	bne	$1,  $Sub
	ble	$21, $MainRemain
	.align 4

$MainLoop:
	LD	$f10,   0*SIZE($19)
	LD	$f11,   1*SIZE($19)
	LD	$f12,   2*SIZE($19)
	LD	$f13,   3*SIZE($19)
	LD	$f14,   4*SIZE($19)
	LD	$f15,   5*SIZE($19)
	LD	$f16,   6*SIZE($19)
	LD	$f17,   7*SIZE($19)

	LD	$f20,   0*SIZE($17)
	LD	$f21,   1*SIZE($17)
	LD	$f22,   2*SIZE($17)
	LD	$f23,   3*SIZE($17)
	LD	$f24,   4*SIZE($17)
	LD	$f25,   5*SIZE($17)
	LD	$f26,   6*SIZE($17)
	LD	$f27,   7*SIZE($17)

	lds	$f31,  32*SIZE($17)
	unop
	lds	$f31,  32*SIZE($19)
	subl	$21, 1, $21

	ST	$f10,   0*SIZE($17)
	ST	$f11,   1*SIZE($17)
	ST	$f12,   2*SIZE($17)
	ST	$f13,   3*SIZE($17)
	ST	$f14,   4*SIZE($17)
	ST	$f15,   5*SIZE($17)
	ST	$f16,   6*SIZE($17)
	ST	$f17,   7*SIZE($17)

	ST	$f20,   0*SIZE($19)
	ST	$f21,   1*SIZE($19)
	ST	$f22,   2*SIZE($19)
	ST	$f23,   3*SIZE($19)
	ST	$f24,   4*SIZE($19)
	ST	$f25,   5*SIZE($19)
	ST	$f26,   6*SIZE($19)
	ST	$f27,   7*SIZE($19)

	lda	$17,   8*SIZE($17)
	lda	$19,   8*SIZE($19)
	bgt	$21, $MainLoop
	.align 4

$MainRemain:
	ble	$22, $MainEnd
	.align 4

$MainRemainLoop:
	LD	$f10,   0*SIZE($19)
	LD	$f20,   0*SIZE($17)
	lda	$17,    1*SIZE($17)
	lda	$19,    1*SIZE($19)
	subl	$22, 1, $22
	ST	$f10,  -1*SIZE($17)
	ST	$f20,  -1*SIZE($19)
	bgt	$22, $MainRemainLoop
	.align 4

$MainEnd:
	clr	$0
	ret
	.align 4

$Sub:
	mov	$17, $23
	mov	$19, $24

	ble	$21, $SubRemain
	.align 4

$SubLoop:
	LD	$f10,   0*SIZE($19)
	SXADDQ	$20, $19, $19
	LD	$f11,   0*SIZE($19)
	SXADDQ	$20, $19, $19

	LD	$f12,   0*SIZE($19)
	SXADDQ	$20, $19, $19
	LD	$f13,   0*SIZE($19)
	SXADDQ	$20, $19, $19

	LD	$f14,   0*SIZE($19)
	SXADDQ	$20, $19, $19
	LD	$f15,   0*SIZE($19)
	SXADDQ	$20, $19, $19

	LD	$f16,   0*SIZE($19)
	SXADDQ	$20, $19, $19
	LD	$f17,   0*SIZE($19)
	SXADDQ	$20, $19, $19

	LD	$f20,   0*SIZE($17)
	SXADDQ	$18, $17, $17
	LD	$f21,   0*SIZE($17)
	SXADDQ	$18, $17, $17

	LD	$f22,   0*SIZE($17)
	SXADDQ	$18, $17, $17
	LD	$f23,   0*SIZE($17)
	SXADDQ	$18, $17, $17

	LD	$f24,   0*SIZE($17)
	SXADDQ	$18, $17, $17
	LD	$f25,   0*SIZE($17)
	SXADDQ	$18, $17, $17

	LD	$f26,   0*SIZE($17)
	SXADDQ	$18, $17, $17
	LD	$f27,   0*SIZE($17)
	SXADDQ	$18, $17, $17

	ST	$f10,   0*SIZE($23)
	SXADDQ	$18, $23, $23
	ST	$f11,   0*SIZE($23)
	SXADDQ	$18, $23, $23

	ST	$f12,   0*SIZE($23)
	SXADDQ	$18, $23, $23
	ST	$f13,   0*SIZE($23)
	SXADDQ	$18, $23, $23

	ST	$f14,   0*SIZE($23)
	SXADDQ	$18, $23, $23
	ST	$f15,   0*SIZE($23)
	SXADDQ	$18, $23, $23

	ST	$f16,   0*SIZE($23)
	SXADDQ	$18, $23, $23
	ST	$f17,   0*SIZE($23)
	SXADDQ	$18, $23, $23

	ST	$f20,   0*SIZE($24)
	SXADDQ	$20, $24, $24
	ST	$f21,   0*SIZE($24)
	SXADDQ	$20, $24, $24

	ST	$f22,   0*SIZE($24)
	SXADDQ	$20, $24, $24
	ST	$f23,   0*SIZE($24)
	SXADDQ	$20, $24, $24

	ST	$f24,   0*SIZE($24)
	SXADDQ	$20, $24, $24
	ST	$f25,   0*SIZE($24)
	SXADDQ	$20, $24, $24

	ST	$f26,   0*SIZE($24)
	SXADDQ	$20, $24, $24
	ST	$f27,   0*SIZE($24)
	SXADDQ	$20, $24, $24

	subl	$21, 1, $21
	bgt	$21, $SubLoop
	.align 4

$SubRemain:
	ble	$22, $SubEnd
	.align 4

$SubRemainLoop:
	LD	$f10,   0*SIZE($19)
	LD	$f20,   0*SIZE($17)

	subl	$22, 1, $22

	ST	$f10,   0*SIZE($17)
	ST	$f20,   0*SIZE($19)

	SXADDQ	$18, $17, $17
	SXADDQ	$20, $19, $19
	bgt	$22, $SubRemainLoop
	.align 4

$SubEnd:
	clr	$0
	ret
	EPILOGUE
