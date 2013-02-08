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
	.globl NAME
	.ent NAME
NAME:
	.frame	$sp, 0, $26, 0

#ifdef PROFILE
	ldgp	$gp, 0($27)
	lda	$28, _mcount
	jsr	$28, ($28), _mcount
#endif

	LD	$f10,    0($16)
	LD	$f11, SIZE($16)
#ifndef PROFILE
	.prologue 0
#else
	.prologue 1
#endif

	fabs	$f10, $f12
	fabs	$f11, $f0
	ADD	$f12, $f0, $f0
	ret
	.end NAME
	.ident	VERSION
