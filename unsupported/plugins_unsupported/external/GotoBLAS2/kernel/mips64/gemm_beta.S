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

#define M	$4
#define	N	$5
#define C	$6
#define LDC	$7

#define I	$2
#define J	$3

#define CO1	$8

#define a1	$f0
#define a2	$f1
#define a3	$f2
#define a4	$f3
#define b1	$f4
#define b2	$f5
#define b3	$f6
#define b4	$f7

#define FZERO	$f8
#define ALPHA	$f15

	PROLOGUE
	
	LDARG	C,     0($sp)
	MTC	$0,  FZERO
	LDARG	LDC,   8($sp)

	dsll	LDC, LDC, BASE_SHIFT

	move	J, N
	blez	J, .L999
	nop
	.align 3

.L10:
	move	CO1, C
	dsra	I,  M, 3

	blez	I, .L15
	daddu	C,   C, LDC

	LD	a1,  0 * SIZE(CO1)
	LD	a2,  1 * SIZE(CO1)
	LD	a3,  2 * SIZE(CO1)
	LD	a4,  3 * SIZE(CO1)

	MUL	b1, ALPHA, a1
	LD	a1,  4 * SIZE(CO1)

	MUL	b2, ALPHA, a2
	daddiu	I, I, -1

	blez	I, .L13
	LD	a2,  5 * SIZE(CO1)
	.align 3

.L12:
	MUL	b3, ALPHA, a3
	LD	a3,  6 * SIZE(CO1)

	ST	b1,  0 * SIZE(CO1)

	MUL	b4, ALPHA, a4
	LD	a4,  7 * SIZE(CO1)

	ST	b2,  1 * SIZE(CO1)

	MUL	b1, ALPHA, a1
	LD	a1,  8 * SIZE(CO1)

	ST	b3,  2 * SIZE(CO1)

	MUL	b2, ALPHA, a2
	LD	a2,  9 * SIZE(CO1)

	ST	b4,  3 * SIZE(CO1)

	MUL	b3, ALPHA, a3
	LD	a3, 10 * SIZE(CO1)

	ST	b1,  4 * SIZE(CO1)

	MUL	b4, ALPHA, a4
	LD	a4, 11 * SIZE(CO1)

	ST	b2,  5 * SIZE(CO1)

	MUL	b1, ALPHA, a1
	LD	a1, 12 * SIZE(CO1)

	ST	b3,  6 * SIZE(CO1)

	MUL	b2, ALPHA, a2
	LD	a2, 13 * SIZE(CO1)

	ST	b4,  7 * SIZE(CO1)
	daddiu	I, I, -1

	bgtz	I, .L12
	daddiu	CO1, CO1, 8 * SIZE
	.align 3

.L13:
	MUL	b3, ALPHA, a3
	LD	a3,  6 * SIZE(CO1)

	ST	b1,  0 * SIZE(CO1)

	MUL	b4, ALPHA, a4
	LD	a4,  7 * SIZE(CO1)

	ST	b2,  1 * SIZE(CO1)

	MUL	b1, ALPHA, a1

	ST	b3,  2 * SIZE(CO1)

	MUL	b2, ALPHA, a2

	ST	b4,  3 * SIZE(CO1)

	MUL	b3, ALPHA, a3

	ST	b1,  4 * SIZE(CO1)

	MUL	b4, ALPHA, a4

	ST	b2,  5 * SIZE(CO1)
	ST	b3,  6 * SIZE(CO1)
	ST	b4,  7 * SIZE(CO1)

	daddiu	CO1, CO1, 8 * SIZE
	.align 3

.L15:
	andi	I,  M, 7
	daddiu	J, J, -1

	blez	I, .L18
	NOP
	.align	3

.L16:
	LD	a1,  0 * SIZE(CO1)
	daddiu	I, I, -1

	MUL	b1, ALPHA, a1
	daddiu	CO1, CO1, 1 * SIZE

	bgtz	I, .L16
	ST	b1, -1 * SIZE(CO1)
	.align 3

.L18:
	bgtz	J, .L10
	NOP
	.align 3

.L999:
	j	$31
	NOP

	EPILOGUE
