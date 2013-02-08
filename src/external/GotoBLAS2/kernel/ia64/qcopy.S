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

#define N	r32
#define X1	r33
#define INCX	r34
#define Y1	r35
#define INCY	r36

#define PREX	r2
#define PREY	r3

#define I	r14
#define J	r15

#define X2	r16
#define Y2	r17
#define INCX2	r18
#define INCY2	r19
#define INCX8	r20
#define INCY8	r21
#define PR	r30
#define ARLC	r31

#define PREFETCH_SIZE (8 * 16)

	PROLOGUE
	.prologue
	PROFCODE
	{ .mmi
	shladd	INCX = INCX, BASE_SHIFT, r0
	shladd	INCY = INCY, BASE_SHIFT, r0
	.save ar.lc, ARLC
	mov	ARLC = ar.lc
	}
	{ .mib
	cmp.lt	p0, p6 = r0, N
	shr	I = N, 4
	(p6) br.ret.sptk.many b0
	}
	;;
	.body
	{ .mmi
	sub	r8 = X1, Y1
	mov	r9 = 0xf0
	mov	PR = pr
	}
	{ .mmi
	shladd	INCX2 = INCX, 1, r0
	shladd	INCY2 = INCY, 1, r0
	and	J = 15, N
	}
	;;
	{ .mmi
	shladd	INCX8 = INCX, 3, r0
	shladd	INCY8 = INCY, 3, r0
	mov	pr.rot = 0
	}
	{ .mmi
	and	r8 = r9, r8
	cmp.eq	p9,  p0 = r0, J
	adds	I = -1, I
	}
	;;
	{ .mmi
	add	X2 = X1, INCX
	add	Y2 = Y1, INCY
	mov	ar.ec = 4
	}
	{ .mmb
	cmp.gt	p6, p0 = 127, r8
	cmp.eq	p16, p0 = r0, r0
	(p6) br.cond.dpnt     .L20
	}
	;;
	{ .mmi
	adds	PREX  = (PREFETCH_SIZE + 0) * SIZE, X1
	adds	PREY  = (PREFETCH_SIZE + 2) * SIZE, Y1
	mov	ar.lc = I
	}
	{ .mib
	cmp.eq	p8 ,p0  =   -1, I
	tbit.z	p0, p12 = N, 3
	(p8) br.cond.dpnt  .L15
	}
	;;
	.align 16

.L12:
	{ .mmi
	(p19) STFD	[Y1] = f35
	(p19) STFD	[Y2] = f39
	(p19) add   Y1 = INCY2, Y1
	}
	{ .mmi
	(p17) LDFD	f81  = [X1], INCX2
	(p17) LDFD	f85  = [X2], INCX2
	(p19) add   Y2 = INCY2, Y2
	}
	;;
	{ .mmi
	(p19) STFD	[Y1] = f43
	(p19) STFD	[Y2] = f47
	(p19) add   Y1 = INCY2, Y1
	}
	{ .mmi
	(p17) LDFD	f89  = [X1], INCX2
	(p17) LDFD	f93  = [X2], INCX2
	(p19) add   Y2 = INCY2, Y2
	}
	;;
	{ .mmi
	(p19) STFD	[Y1] = f51
	(p19) STFD	[Y2] = f55
	(p19) add   Y1 = INCY2, Y1
	}
	{ .mmi
	(p16) LDFD	f32  = [X1], INCX2
	(p16) LDFD	f36  = [X2], INCX2
	(p19) add   Y2 = INCY2, Y2
	}
	;;
	{ .mmi
	(p19) STFD	[Y1] = f59
	(p19) STFD	[Y2] = f63
	(p19) add   Y1 = INCY2, Y1
	}
	{ .mmi
	lfetch.fault.nt1	[PREX], INCX8
	lfetch.fault.excl.nt1	[PREY], INCY8
	(p19) add   Y2 = INCY2, Y2
	}
	;;
	{ .mmi
	(p16) LDFD	f40  = [X1], INCX2
	(p16) LDFD	f44  = [X2], INCX2
	nop	__LINE__
	}
	;;
	{ .mmi
	(p19) STFD	[Y1] = f67
	(p19) STFD	[Y2] = f71
	(p19) add   Y1 = INCY2, Y1
	}
	{ .mmi
	(p16) LDFD	f48  = [X1], INCX2
	(p16) LDFD	f52  = [X2], INCX2
	(p19) add   Y2 = INCY2, Y2
	}
	;;
	{ .mmi
	(p19) STFD	[Y1] = f75
	(p19) STFD	[Y2] = f79
	(p19) add   Y1 = INCY2, Y1
	}
	{ .mmi
	(p16) LDFD	f56  = [X1], INCX2
	(p16) LDFD	f60  = [X2], INCX2
	(p19) add   Y2 = INCY2, Y2
	}
	;;
	{ .mmi
	(p19) STFD	[Y1] = f83
	(p19) STFD	[Y2] = f87
	(p19) add   Y1 = INCY2, Y1
	}
	{ .mmi
	lfetch.fault.nt1	[PREX], INCX8
	lfetch.fault.excl.nt1	[PREY], INCY8
	(p19) add   Y2 = INCY2, Y2
	}
	;;
	{ .mmi
	(p19) STFD	[Y1] = f91
	(p19) STFD	[Y2] = f95
	(p19) add   Y1 = INCY2, Y1
	}
	{ .mmi
 	(p16) LDFD	f64  = [X1], INCX2
	(p16) LDFD	f68  = [X2], INCX2
	(p19) add   Y2 = INCY2, Y2
	}
	;;
	{ .mmb
 	(p16) LDFD	f72  = [X1], INCX2
	(p16) LDFD	f76  = [X2], INCX2
	br.ctop.sptk.few .L12
	}
	;;
	.align 32

.L15:
	{ .mmi
	(p12) LDFD	f48 = [X1], INCX2
	(p12) LDFD	f49 = [X2], INCX2
	mov	ar.lc = ARLC
	}
	;;
	{ .mmi
	(p12) LDFD	f50 = [X1], INCX2
	(p12) LDFD	f51 = [X2], INCX2
	mov	pr = PR, -65474
	}
	;;
	{ .mmb
	(p12) LDFD	f52 = [X1], INCX2
	(p12) LDFD	f53 = [X2], INCX2
	(p9) br.ret.sptk.many b0
	}
	;;
	{ .mmi
	(p12) LDFD	f54 = [X1], INCX2
	(p12) LDFD	f55 = [X2], INCX2
	tbit.z	p0, p13 = N, 2
	}
	;;
	{ .mmi
	(p13) LDFD	f56 = [X1], INCX2
	(p13) LDFD	f57 = [X2], INCX2
	tbit.z	p0, p14 = N, 1
	}
	;;
	{ .mmi
	(p13) LDFD	f58 = [X1], INCX2
	(p13) LDFD	f59 = [X2], INCX2
	tbit.z	p0, p15 = N, 0
	}
	;;
	{ .mmi
	(p12) STFD	[Y1] = f48
	(p12) STFD	[Y2] = f49
	(p12) add Y1 = INCY2, Y1
	}
	{ .mmi
	(p14) LDFD	f60 = [X1], INCX2
	(p14) LDFD	f61 = [X2], INCX2
	(p12) add Y2 = INCY2, Y2
	}
	;;
	{ .mmi
	(p12) STFD	[Y1] = f50
	(p12) STFD	[Y2] = f51
	(p12) add Y1 = INCY2, Y1
	}
	{ .mmi
	(p15) LDFD	f62 = [X1]
	nop   __LINE__
	(p12) add Y2 = INCY2, Y2
	}
	;;
	{ .mmi
	(p12) STFD	[Y1] = f52
	(p12) STFD	[Y2] = f53
	(p12) add Y1 = INCY2, Y1
	}
	{ .mmi
	nop	__LINE__
	nop	__LINE__
	(p12) add Y2 = INCY2, Y2
	}
	;;
	{ .mmi
	(p12) STFD	[Y1] = f54
	(p12) STFD	[Y2] = f55
	(p12) add Y1 = INCY2, Y1
	}
	{ .mmi
	nop	__LINE__
	nop	__LINE__
	(p12) add Y2 = INCY2, Y2
	}
	;;
	{ .mmi
	(p13) STFD	[Y1] = f56
	(p13) STFD	[Y2] = f57
	(p13) add Y1 = INCY2, Y1
	}
	{ .mmi
	nop	__LINE__
	nop	__LINE__
	(p13) add Y2 = INCY2, Y2
	}
	;;
	{ .mmi
	(p13) STFD	[Y1] = f58
	(p13) STFD	[Y2] = f59
	(p13) add Y1 = INCY2, Y1
	}
	{ .mmi
	nop	__LINE__
	nop	__LINE__
	(p13) add Y2 = INCY2, Y2
	}
	;;
	{ .mmi
	(p14) STFD	[Y1] = f60
	(p14) STFD	[Y2] = f61
	(p14) add Y1 = INCY2, Y1
	}
	;;
	{ .mmb
	(p15) STFD	[Y1] = f62
	nop	__LINE__
	br.ret.sptk.many b0
	}
	;;
	.align 16

.L20:
	{ .mmi
	adds	PREX  = (PREFETCH_SIZE +  0) * SIZE, X1
	adds	PREY  = (PREFETCH_SIZE + 10) * SIZE, Y1
	mov	ar.lc = I
	}
	{ .mib
	cmp.eq	p8 ,p0  =   -1, I
	tbit.z	p0, p12 = N, 3
	(p8) br.cond.dpnt  .L25
	}
	;;
	.align 16

.L22:
	{ .mmi
	(p19) STFD	[Y1] = f67
	(p19) STFD	[Y2] = f71
	(p19) add   Y1 = INCY2, Y1
	}
	{ .mmi
	(p17) LDFD	f81  = [X1], INCX2
	(p17) LDFD	f85  = [X2], INCX2
	(p19) add   Y2 = INCY2, Y2
	}
	;;
	{ .mmi
	(p19) STFD	[Y1] = f75
	(p19) STFD	[Y2] = f79
	(p19) add   Y1 = INCY2, Y1
	}
	{ .mmi
	(p17) LDFD	f89  = [X1], INCX2
	(p17) LDFD	f93  = [X2], INCX2
	(p19) add   Y2 = INCY2, Y2
	}
	;;
	{ .mmi
	(p19) STFD	[Y1] = f83
	(p19) STFD	[Y2] = f87
	(p19) add   Y1 = INCY2, Y1
	}
	{ .mmi
	(p16) LDFD	f32  = [X1], INCX2
	(p16) LDFD	f36  = [X2], INCX2
	(p19) add   Y2 = INCY2, Y2
	}
	;;
	{ .mmi
	(p19) STFD	[Y1] = f91
	(p19) STFD	[Y2] = f95
	(p19) add   Y1 = INCY2, Y1
	}
	{ .mmi
	lfetch.fault.nt1	[PREX], INCX8
	lfetch.fault.excl.nt1	[PREY], INCY8
	(p19) add   Y2 = INCY2, Y2
	}
	;;
	{ .mmi
	(p16) LDFD	f40  = [X1], INCX2
	(p16) LDFD	f44  = [X2], INCX2
	nop	__LINE__
	}
	;;
	{ .mmi
	(p18) STFD	[Y1] = f34
	(p18) STFD	[Y2] = f38
	(p18) add   Y1 = INCY2, Y1
	}
	{ .mmi
	(p16) LDFD	f48  = [X1], INCX2
	(p16) LDFD	f52  = [X2], INCX2
	(p18) add   Y2 = INCY2, Y2
	}
	;;
	{ .mmi
	(p18) STFD	[Y1] = f42
	(p18) STFD	[Y2] = f46
	(p18) add   Y1 = INCY2, Y1
	}
	{ .mmi
	(p16) LDFD	f56  = [X1], INCX2
	(p16) LDFD	f60  = [X2], INCX2
	(p18) add   Y2 = INCY2, Y2
	}
	;;
	{ .mmi
	(p18) STFD	[Y1] = f50
	(p18) STFD	[Y2] = f54
	(p18) add   Y1 = INCY2, Y1
	}
	{ .mmi
	lfetch.fault.nt1	[PREX], INCX8
	lfetch.fault.excl.nt1	[PREY], INCY8
	(p18) add   Y2 = INCY2, Y2
	}
	;;
	{ .mmi
	(p18) STFD	[Y1] = f58
	(p18) STFD	[Y2] = f62
	(p18) add   Y1 = INCY2, Y1
	}
	{ .mmi
 	(p16) LDFD	f64  = [X1], INCX2
	(p16) LDFD	f68  = [X2], INCX2
	(p18) add   Y2 = INCY2, Y2
	}
	;;
	{ .mmb
 	(p16) LDFD	f72  = [X1], INCX2
	(p16) LDFD	f76  = [X2], INCX2
	br.ctop.sptk.few .L22
	}
	;;
	.align 32

.L25:
	{ .mmi
	(p12) LDFD	f48 = [X1], INCX2
	(p12) LDFD	f49 = [X2], INCX2
	mov	ar.lc = ARLC
	}
	;;
	{ .mmi
	(p12) LDFD	f50 = [X1], INCX2
	(p12) LDFD	f51 = [X2], INCX2
	mov	pr = PR, -65474
	}
	;;
	{ .mmb
	(p12) LDFD	f52 = [X1], INCX2
	(p12) LDFD	f53 = [X2], INCX2
	(p9) br.ret.sptk.many b0
	}
	;;
	{ .mmi
	(p12) LDFD	f54 = [X1], INCX2
	(p12) LDFD	f55 = [X2], INCX2
	tbit.z	p0, p13 = N, 2
	}
	;;
	{ .mmi
	(p13) LDFD	f56 = [X1], INCX2
	(p13) LDFD	f57 = [X2], INCX2
	tbit.z	p0, p14 = N, 1
	}
	;;
	{ .mmi
	(p13) LDFD	f58 = [X1], INCX2
	(p13) LDFD	f59 = [X2], INCX2
	tbit.z	p0, p15 = N, 0
	}
	;;
	{ .mmi
	(p12) STFD	[Y1] = f48
	(p12) STFD	[Y2] = f49
	(p12) add Y1 = INCY2, Y1
	}
	{ .mmi
	(p14) LDFD	f60 = [X1], INCX2
	(p14) LDFD	f61 = [X2], INCX2
	(p12) add Y2 = INCY2, Y2
	}
	;;
	{ .mmi
	(p12) STFD	[Y1] = f50
	(p12) STFD	[Y2] = f51
	(p12) add Y1 = INCY2, Y1
	}
	{ .mmi
	(p15) LDFD	f62 = [X1]
	nop   __LINE__
	(p12) add Y2 = INCY2, Y2
	}
	;;
	{ .mmi
	(p12) STFD	[Y1] = f52
	(p12) STFD	[Y2] = f53
	(p12) add Y1 = INCY2, Y1
	}
	{ .mmi
	nop	__LINE__
	nop	__LINE__
	(p12) add Y2 = INCY2, Y2
	}
	;;
	{ .mmi
	(p12) STFD	[Y1] = f54
	(p12) STFD	[Y2] = f55
	(p12) add Y1 = INCY2, Y1
	}
	{ .mmi
	nop	__LINE__
	nop	__LINE__
	(p12) add Y2 = INCY2, Y2
	}
	;;
	{ .mmi
	(p13) STFD	[Y1] = f56
	(p13) STFD	[Y2] = f57
	(p13) add Y1 = INCY2, Y1
	}
	{ .mmi
	nop	__LINE__
	nop	__LINE__
	(p13) add Y2 = INCY2, Y2
	}
	;;
	{ .mmi
	(p13) STFD	[Y1] = f58
	(p13) STFD	[Y2] = f59
	(p13) add Y1 = INCY2, Y1
	}
	{ .mmi
	nop	__LINE__
	nop	__LINE__
	(p13) add Y2 = INCY2, Y2
	}
	;;
	{ .mmi
	(p14) STFD	[Y1] = f60
	(p14) STFD	[Y2] = f61
	(p14) add Y1 = INCY2, Y1
	}
	;;
	{ .mmb
	(p15) STFD	[Y1] = f62
	nop	__LINE__
	br.ret.sptk.many b0
	}
	;;

	EPILOGUE

