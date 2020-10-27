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

#include "version.h"

	.set noat
	.set noreorder
.text
	.align 5
	.globl lsame_
	.ent lsame_
lsame_:
	.frame $sp,0,$26,0
#ifdef PROFILE
	ldgp	$gp, 0($27)
	lda	$28, _mcount
	jsr	$28, ($28), _mcount
	.prologue 1
#else
	.prologue 0
#endif

	ldq_u	$5,  0($16)
	ldq_u	$6,  0($17)
	extbl	$5,  $16, $5
	extbl	$6,  $17, $6

	subl	$5,  96,  $1
	subl	$6,  96,  $2
	subl	$5,  32,  $3
	subl	$6,  32,  $4

	cmovgt	$1,  $3,  $5
	cmovgt	$2,  $4,  $6
	cmpeq	$5,  $6,  $0
	.align 4

$End:
	ret
	.end lsame_
	.ident VERSION
