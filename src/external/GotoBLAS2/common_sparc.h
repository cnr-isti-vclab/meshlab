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

#ifndef COMMON_POWER
#define COMMON_POWER

#define MB	__asm__ __volatile__ ("nop")
#define WMB	__asm__ __volatile__ ("nop")

#ifndef ASSEMBLER

static void __inline blas_lock(volatile unsigned long *address){

  long int ret = 1;

  do {
    while (*address) {YIELDING;};

    __asm__ __volatile__(
			 "ldstub [%1], %0"
			 : "=&r"(ret)
			 : "r" (address)
			 : "memory");
  } while (ret);
}

static __inline unsigned long rpcc(void){
  unsigned long clocks;

  __asm__ __volatile__ ("rd %%tick, %0" : "=r" (clocks));

  return clocks;
};

#ifdef __64BIT__
#define RPCC64BIT
#endif

#ifndef __BIG_ENDIAN__
#define __BIG_ENDIAN__
#endif

#ifdef DOUBLE
#define GET_IMAGE(res)  __asm__ __volatile__("fmovd %%f2, %0" : "=f"(res)  : : "memory")
#else
#define GET_IMAGE(res)  __asm__ __volatile__("fmovs %%f1, %0" : "=f"(res)  : : "memory")
#endif

#define GET_IMAGE_CANCEL

#ifdef SMP
static __inline int blas_quickdivide(blasint x, blasint y){
  return x / y;
}
#endif
#endif


#ifdef ASSEMBLER

#ifndef __64BIT__
#define STACK_START 128
#define SAVESP		save	%sp,  -64, %sp
#else
#define STACK_START 2423
#define SAVESP		save	%sp, -256, %sp
#endif

#define NOP	or %g1, %g1, %g1

#ifdef DOUBLE
#define LDF	ldd
#define STF	std
#define FADD	faddd
#define FMUL	fmuld
#define FMOV	fmovd
#define FABS	fabsd
#define FSUB	fsubd
#define FCMP	fcmpd
#define FMOVG	fmovdg
#define FMOVL	fmovdl
#define FSQRT	fsqrtd
#define FDIV	fdivd
#else
#define LDF	ld
#define STF	st
#define FADD	fadds
#define FMUL	fmuls
#define FMOV	fmovs
#define FABS	fabss
#define FSUB	fsubs
#define FCMP	fcmps
#define FMOVG	fmovsg
#define FMOVL	fmovsl
#define FSQRT	fsqrts
#define FDIV	fdivs
#endif
	
#define HALT prefetch [%g0], 5

#define FMADDS(rs1, rs2, rs3, rd) \
        .word	((2 << 30) | ((rd) << 25) | ( 0x37 << 19) | ((rs1) << 14) | ((rs3) << 9) | ( 1 << 5) | (rs2))

#define FMADDD(rs1, rs2, rs3, rd) \
        .word	((2 << 30) | ((rd) << 25) | ( 0x37 << 19) | ((rs1) << 14) | ((rs3) << 9) | ( 2 << 5) | (rs2))

#define FMSUBS(rs1, rs2, rs3, rd) \
        .word	((2 << 30) | ((rd) << 25) | ( 0x37 << 19) | ((rs1) << 14) | ((rs3) << 9) | ( 5 << 5) | (rs2))

#define FMSUBD(rs1, rs2, rs3, rd) \
        .word	((2 << 30) | ((rd) << 25) | ( 0x37 << 19) | ((rs1) << 14) | ((rs3) << 9) | ( 6 << 5) | (rs2))

#define FNMSUBS(rs1, rs2, rs3, rd) \
        .word	((2 << 30) | ((rd) << 25) | ( 0x37 << 19) | ((rs1) << 14) | ((rs3) << 9) | ( 9 << 5) | (rs2))

#define FNMSUBD(rs1, rs2, rs3, rd) \
        .word	((2 << 30) | ((rd) << 25) | ( 0x37 << 19) | ((rs1) << 14) | ((rs3) << 9) | (10 << 5) | (rs2))

#define FNMADDS(rs1, rs2, rs3, rd) \
        .word	((2 << 30) | ((rd) << 25) | ( 0x37 << 19) | ((rs1) << 14) | ((rs3) << 9) | (13 << 5) | (rs2))

#define FNMADDD(rs1, rs2, rs3, rd) \
        .word	((2 << 30) | ((rd) << 25) | ( 0x37 << 19) | ((rs1) << 14) | ((rs3) << 9) | (14 << 5) | (rs2))

#define FCLRS(rd) \
        .word	((2 << 30) | ((rd) << 25) | ( 0x36 << 19) | ( 0x61 << 5))

#define FCLRD(rd) \
        .word	((2 << 30) | ((rd) << 25) | ( 0x36 << 19) | ( 0x60 << 5))

#define FONES(rd) \
        .word	((2 << 30) | ((rd) << 25) | ( 0x36 << 19) | ( 0x7f << 5))

#define FONED(rd) \
        .word	((2 << 30) | ((rd) << 25) | ( 0x36 << 19) | ( 0x7e << 5))

#ifndef DOUBLE
#define	FCLR(a)			FCLRS(a) 
#define	FONE(a)			FONES(a) 
#define	FMADD(a, b, c, d)	FMADDS(a, b, c, d) 
#define	FMSUB(a, b, c, d)	FMSUBS(a, b, c, d) 
#define	FNMADD(a, b, c, d)	FNMADDS(a, b, c, d) 
#define	FNMSUB(a, b, c, d)	FNMSUBS(a, b, c, d) 
#else
#define	FCLR(a)			FCLRD(a) 
#define	FONE(a)			FONED(a) 
#define	FMADD(a, b, c, d)	FMADDD(a, b, c, d) 
#define	FMSUB(a, b, c, d)	FMSUBD(a, b, c, d) 
#define	FNMADD(a, b, c, d)	FNMADDD(a, b, c, d) 
#define	FNMSUB(a, b, c, d)	FNMSUBD(a, b, c, d) 
#endif

#ifndef F_INTERFACE
#define REALNAME ASMNAME
#else
#define REALNAME ASMFNAME
#endif

#ifdef sparc
#define PROLOGUE \
	.section	".text"; \
	.align 32; \
	.global REALNAME;\
	.type	REALNAME, #function; \
	.proc	07; \
REALNAME:;
#define EPILOGUE \
	.size	 REALNAME, .-REALNAME
#endif

#endif

#ifdef sparc
#define SEEK_ADDRESS
#endif

#define BUFFER_SIZE	(32 << 20)

#ifndef PAGESIZE
#define PAGESIZE	( 8 << 10)
#endif
#define HUGE_PAGESIZE	( 4 << 20)

#define BASE_ADDRESS (START_ADDRESS - BUFFER_SIZE * MAX_CPU_NUMBER)

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif
#endif
