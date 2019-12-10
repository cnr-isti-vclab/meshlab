/* Templates for defines setup by configure.

Copyright 2000, 2001, 2002 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

/* Define one (and only one) of these for the CPU host.
   Only hosts that are going to be tested for need to be in this list,
   not everything that can possibly be selected.
 */
#undef HAVE_HOST_CPU_alpha
#undef HAVE_HOST_CPU_alphaev5
#undef HAVE_HOST_CPU_alphaev6
#undef HAVE_HOST_CPU_alphaev67

#undef HAVE_HOST_CPU_m68k
#undef HAVE_HOST_CPU_m68000
#undef HAVE_HOST_CPU_m68010
#undef HAVE_HOST_CPU_m68020
#undef HAVE_HOST_CPU_m68030
#undef HAVE_HOST_CPU_m68040
#undef HAVE_HOST_CPU_m68060
#undef HAVE_HOST_CPU_m68302
#undef HAVE_HOST_CPU_m68360

#undef HAVE_HOST_CPU_powerpc604
#undef HAVE_HOST_CPU_powerpc604e
#undef HAVE_HOST_CPU_powerpc750
#undef HAVE_HOST_CPU_powerpc7400

#undef HAVE_HOST_CPU_sparc
#undef HAVE_HOST_CPU_sparcv8
#undef HAVE_HOST_CPU_supersparc
#undef HAVE_HOST_CPU_sparclite
#undef HAVE_HOST_CPU_microsparc
#undef HAVE_HOST_CPU_ultrasparc1
#undef HAVE_HOST_CPU_ultrasparc2
#undef HAVE_HOST_CPU_sparc64

#undef HAVE_HOST_CPU_hppa1_0
#undef HAVE_HOST_CPU_hppa1_1
#undef HAVE_HOST_CPU_hppa2_0n
#undef HAVE_HOST_CPU_hppa2_0w

#undef HAVE_HOST_CPU_i386
#undef HAVE_HOST_CPU_i486
#undef HAVE_HOST_CPU_i586
#undef HAVE_HOST_CPU_i686
#undef HAVE_HOST_CPU_pentium
#undef HAVE_HOST_CPU_pentiummmx
#undef HAVE_HOST_CPU_pentiumpro
#undef HAVE_HOST_CPU_pentium2
#undef HAVE_HOST_CPU_pentium3
#undef HAVE_HOST_CPU_k5
#undef HAVE_HOST_CPU_k6
#undef HAVE_HOST_CPU_k62
#undef HAVE_HOST_CPU_k63
#undef HAVE_HOST_CPU_athlon

/* a dummy to make autoheader happy */
#undef HAVE_HOST_CPU_

/* Define one (and only one) of these for the CPU host family.
   Only hosts that are going to be tested for need to be in this list,
   not everything that can possibly be selected.
 */
#undef HAVE_HOST_CPU_FAMILY_power
#undef HAVE_HOST_CPU_FAMILY_powerpc
#define HAVE_HOST_CPU_FAMILY_x86	1

/* Define if we have native implementation of function. 
   (use just one of the three following defines)
*/

#undef HAVE_NATIVE_mpn_add
#undef HAVE_NATIVE_mpn_add_1
#undef HAVE_NATIVE_mpn_addmul_2
#undef HAVE_NATIVE_mpn_addmul_3
#undef HAVE_NATIVE_mpn_addmul_4
#undef HAVE_NATIVE_mpn_addsub_n
#undef HAVE_NATIVE_mpn_addsub_nc
#undef HAVE_NATIVE_mpn_and_n
#undef HAVE_NATIVE_mpn_andn_n
#undef HAVE_NATIVE_mpn_bdivmod
#undef HAVE_NATIVE_mpn_cmp
#undef HAVE_NATIVE_mpn_com_n
#undef HAVE_NATIVE_mpn_divrem
#undef HAVE_NATIVE_mpn_divrem_2
#undef HAVE_NATIVE_mpn_divrem_newton
#undef HAVE_NATIVE_mpn_divrem_classic
#undef HAVE_NATIVE_mpn_dump
#undef HAVE_NATIVE_mpn_gcd
#undef HAVE_NATIVE_mpn_gcd_1
#undef HAVE_NATIVE_mpn_gcd_finda
#undef HAVE_NATIVE_mpn_gcdext
#undef HAVE_NATIVE_mpn_get_str
#undef HAVE_NATIVE_mpn_invert_limb
#undef HAVE_NATIVE_mpn_ior_n
#undef HAVE_NATIVE_mpn_iorn_n
#undef HAVE_NATIVE_mpn_mul
#undef HAVE_NATIVE_mpn_mul_2
#undef HAVE_NATIVE_mpn_mul_3
#undef HAVE_NATIVE_mpn_mul_4
#undef HAVE_NATIVE_mpn_mul_n
#undef HAVE_NATIVE_mpn_nand_n
#undef HAVE_NATIVE_mpn_nior_n
#undef HAVE_NATIVE_mpn_perfect_square_p
#undef HAVE_NATIVE_mpn_preinv_mod_1
#undef HAVE_NATIVE_mpn_random2
#undef HAVE_NATIVE_mpn_random
#undef HAVE_NATIVE_mpn_rawrandom
#undef HAVE_NATIVE_mpn_scan0
#undef HAVE_NATIVE_mpn_scan1
#undef HAVE_NATIVE_mpn_set_str
#undef HAVE_NATIVE_mpn_sqrtrem
#undef HAVE_NATIVE_mpn_sqr_diagonal
#undef HAVE_NATIVE_mpn_sub
#undef HAVE_NATIVE_mpn_sub_1
#undef HAVE_NATIVE_mpn_udiv_w_sdiv
#undef HAVE_NATIVE_mpn_xor_n
#undef HAVE_NATIVE_mpn_xnor_n

#undef HAVE_NATIVE_mpn_add_n
#undef HAVE_NATIVE_mpn_add_nc
#undef HAVE_NATIVE_mpn_sub_n
#undef HAVE_NATIVE_mpn_sub_nc

#undef HAVE_NATIVE_mpn_addmul_1
#undef HAVE_NATIVE_mpn_addmul_1c
#undef HAVE_NATIVE_mpn_submul_1
#undef HAVE_NATIVE_mpn_submul_1c

#undef HAVE_NATIVE_mpn_copyd
#undef HAVE_NATIVE_mpn_copyi

#undef HAVE_NATIVE_mpn_divexact_1
#undef HAVE_NATIVE_mpn_divexact_by3c
#undef HAVE_NATIVE_mpn_divrem_1
#undef HAVE_NATIVE_mpn_divrem_1c

#undef HAVE_NATIVE_mpn_hamdist
#undef HAVE_NATIVE_mpn_popcount

#undef HAVE_NATIVE_mpn_lshift
#undef HAVE_NATIVE_mpn_rshift

#undef HAVE_NATIVE_mpn_mod_1
#undef HAVE_NATIVE_mpn_mod_1c
#undef HAVE_NATIVE_mpn_modexact_1_odd
#undef HAVE_NATIVE_mpn_modexact_1c_odd

#undef HAVE_NATIVE_mpn_mul_1
#undef HAVE_NATIVE_mpn_mul_1c
#undef HAVE_NATIVE_mpn_mul_basecase
#undef HAVE_NATIVE_mpn_sqr_basecase

#undef HAVE_NATIVE_mpn_umul_ppmm
#undef HAVE_NATIVE_mpn_udiv_qrnnd

/* For the generic C code */

#define HAVE_NATIVE_mpn_add_n			1
#define HAVE_NATIVE_mpn_sub_n			1

/* a dummy to make autoheader happy */
#undef HAVE_NATIVE_


/* The gmp-mparam.h to update when tuning. */
#undef GMP_MPARAM_H_SUGGEST

/* Define if you have the `alarm' function. */
#undef HAVE_ALARM

/* Define if alloca() works (via gmp-impl.h). */
#define HAVE_ALLOCA			1

/* Define if you have <alloca.h> and it should be used (not on Ultrix). */
#undef HAVE_ALLOCA_H

/* Define if the compiler accepts gcc style __attribute__ ((const)) */
#undef HAVE_ATTRIBUTE_CONST

/* Define if the compiler accepts gcc style __attribute__ ((malloc)) */
#undef HAVE_ATTRIBUTE_MALLOC

/* Define if the compiler accepts gcc style __attribute__ ((mode (XX))) */
#undef HAVE_ATTRIBUTE_MODE

/* Define if the compiler accepts gcc style __attribute__ ((noreturn)) */
#undef HAVE_ATTRIBUTE_NORETURN

/* Define if tests/libtests has calling conventions checking for the CPU */
#undef HAVE_CALLING_CONVENTIONS

/* Define if you have the `clock' function. */
#define HAVE_CLOCK			1

/* Define if you have the `clock_gettime' function. */
#undef HAVE_CLOCK_GETTIME

/* Define if you have the `cputime' function. */
#undef HAVE_CPUTIME

/* Define to 1 if you have the declaration of `fgetc', and to 0 if you don't.
   */
#define HAVE_DECL_FGETC		1

/* Define to 1 if you have the declaration of `fscanf', and to 0 if you don't.
   */
#define HAVE_DECL_FSCANF	1

/* Define to 1 if you have the declaration of `optarg', and to 0 if you don't.
   */
#define HAVE_DECL_OPTARG	0

/* Define to 1 if you have the declaration of `ungetc', and to 0 if you don't.
   */
#define HAVE_DECL_UNGETC	1

/* Define to 1 if you have the declaration of `vfprintf', and to 0 if you
   don't. */
#define HAVE_DECL_VFPRINTF	1

/* Define if denormalized floats work. */
#define HAVE_DENORMS		1

/* Define if you have the <dlfcn.h> header file. */
#undef HAVE_DLFCN_H

/* Define one (and only one) of the following for the format of a `double'.
   If your format is not among these choices, or you don't know what it is,
   then leave all of them undefined.
   "IEEE_LITTLE_SWAPPED" means little endian, but with the two 4-byte halves
   swapped, as used by ARM CPUs in little endian mode.  */
#undef HAVE_DOUBLE_IEEE_BIG_ENDIAN
#define HAVE_DOUBLE_IEEE_LITTLE_ENDIAN	1
#undef HAVE_DOUBLE_IEEE_LITTLE_SWAPPED
#undef HAVE_DOUBLE_VAX_D
#undef HAVE_DOUBLE_VAX_G
#undef HAVE_DOUBLE_CRAY_CFP

/* Define if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H		1

/* Define if you have the <fpu_control.h> header file. */
#undef HAVE_FPU_CONTROL_H

/* Define if you have the `getpagesize' function. */
#undef HAVE_GETPAGESIZE

/* Define if you have the `getrusage' function. */
#undef HAVE_GETRUSAGE

/* Define if you have the `gettimeofday' function. */
#undef HAVE_GETTIMEOFDAY

/* Define if 0/0, 1/0, -1/0 and sqrt(-1) work to generate NaN/infinities. */
#define HAVE_INFS			1

/* Define if the system has the type `intmax_t'. */
#undef HAVE_INTMAX_T

/* Define if you have the <inttypes.h> header file. */
#undef HAVE_INTTYPES_H

/* Define one (just one) of the following for the endiannes of `mp_limb_t'.
   If the endianness is not a simple big or little, or you don't know what
   it is, then leave both of these undefined. */
#undef HAVE_LIMB_BIG_ENDIAN
#define HAVE_LIMB_LITTLE_ENDIAN		1

#define HAVE_STD__LOCALE 1

/* Define if you have the `localeconv' function. */
#define HAVE_LOCALECONV		1

/* Define if you have the <locale.h> header file. */
#define HAVE_LOCALE_H		1

/* Define if the system has the type `long double'. */
#define HAVE_LONG_DOUBLE	1

/* Define if the system has the type `long long'. */
#define HAVE_LONG_LONG		1

/* Define if you have the `lrand48' function. */
#undef HAVE_LRAND48

/* Define if you have the <memory.h> header file. */
#define HAVE_MEMORY_H		1

/* Define if you have the `memset' function. */
#define HAVE_MEMSET			1

/* Define if you have the `mmap' function. */
#undef HAVE_MMAP

/* Define if you have the `mprotect' function. */
#undef HAVE_MPROTECT

/* Define if you have the `obstack_vprintf' function. */
#undef HAVE_OBSTACK_VPRINTF

/* Define if you have the `popen' function. */
#undef HAVE_POPEN

/* Define if you have the `processor_info' function. */
#undef HAVE_PROCESSOR_INFO

/* Define if the system has the type `ptrdiff_t'. */
#define HAVE_PTRDIFF_T		1

/* Define if the system has the type `quad_t'. */
#undef HAVE_QUAD_T

#define HAVE_RAISE			1

/* Define if you have the `read_real_time' function. */
#undef HAVE_READ_REAL_TIME

/* Define if you have the `sigaction' function. */
#undef HAVE_SIGACTION

/* Define if you have the `sigaltstack' function. */
#undef HAVE_SIGALTSTACK

/* Define if you have the `sigstack' function. */
#undef HAVE_SIGSTACK

/* Tune directory speed_cyclecounter, undef=none, 1=32bits, 2=64bits) */
#define HAVE_SPEED_CYCLECOUNTER	2

/* Define if the system has the type `stack_t'. */
#undef HAVE_STACK_T

/* Define if <stdarg.h> exists and works */
#define HAVE_STDARG			1

/* Define if you have the <stdint.h> header file. */
#undef HAVE_STDINT_H

/* Define if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H		1

/* Define if you have the `strcasecmp' function. */
#undef HAVE_STRCASECMP

/* Define if you have the `strchr' function. */
#define HAVE_STRCHR			1

/* Define if cpp supports the ANSI # stringizing operator. */
#define HAVE_STRINGIZE		1

/* Define if you have the <strings.h> header file. */
#undef HAVE_STRINGS_H

/* Define if you have the <string.h> header file. */
#define HAVE_STRING_H		1

/* Define if you have the `strnlen' function. */
#define HAVE_STRNLEN        1

/* Define if you have the `strtoul' function. */
#define HAVE_STRTOUL		1

/* Define if you have the `sysconf' function. */
#undef HAVE_SYSCONF

/* Define if you have the `sysctl' function. */
#undef HAVE_SYSCTL

/* Define if you have the `sysctlbyname' function. */
#undef HAVE_SYSCTLBYNAME

/* Define if you have the `syssgi' function. */
#undef HAVE_SYSSGI

/* Define if you have the <sys/mman.h> header file. */
#undef HAVE_SYS_MMAN_H

/* Define if you have the <sys/param.h> header file. */
#undef HAVE_SYS_PARAM_H

/* Define if you have the <sys/processor.h> header file. */
#undef HAVE_SYS_PROCESSOR_H

/* Define if you have the <sys/resource.h> header file. */
#undef HAVE_SYS_RESOURCE_H

/* Define if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H		1

/* Define if you have the <sys/sysctl.h> header file. */
#undef HAVE_SYS_SYSCTL_H

/* Define if you have the <sys/syssgi.h> header file. */
#undef HAVE_SYS_SYSSGI_H

/* Define if you have the <sys/systemcfg.h> header file. */
#undef HAVE_SYS_SYSTEMCFG_H

/* Define if you have the <sys/times.h> header file. */
#undef HAVE_SYS_TIMES_H

/* Define if you have the <sys/time.h> header file. */
#undef HAVE_SYS_TIME_H

/* Define if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H	1

/* Define if you have the `times' function. */
#undef HAVE_TIMES

/* Define if you have the <unistd.h> header file. */
#undef HAVE_UNISTD_H

/* Define if you have vsnprintf and it works properly. */
#undef HAVE_VSNPRINTF

/* Assembler local label prefix */
#undef LSYM_PREFIX

/* Define if you have the `fesetround' function via the <fenv.h> header file.
   */
#undef MPFR_HAVE_FESETROUND

#define HAVE_SSTREAM 1

/* Name of package */
#define PACKAGE			"mpir"

/* Define if compiler has function prototypes */
#define PROTOTYPES			1

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE	void

/* The size of a `unsigned long', as computed by sizeof. */
#define SIZEOF_UNSIGNED_LONG	4

/* Define if sscanf requires writable inputs */
#undef SSCANF_WRITABLE_INPUT

/* Define if you have the ANSI C header files. */
#define STDC_HEADERS		1

/* Define if you can safely include both <sys/time.h> and <time.h>. */
#undef TIME_WITH_SYS_TIME

/* Maximum size the tune program can test for KARATSUBA_SQR_THRESHOLD */
#define TUNE_KARATSUBA_SQR_MAX	67

/* ./configure --enable-assert option, to enable some ASSERT()s */
#undef WANT_ASSERT

/* ./configure --enable-fft option, to enable FFTs for multiplication */
#define WANT_FFT			1

/* Define to 1 if --enable-profiling=gprof */
#undef WANT_PROFILING_GPROF

/* Define to 1 if --enable-profiling=prof */
#undef WANT_PROFILING_PROF

/* --enable-alloca=yes */
#define WANT_TMP_ALLOCA     1

/* --enable-alloca=debug */
#undef WANT_TMP_DEBUG

/* --enable-alloca=malloc-notreentrant */
#undef WANT_TMP_NOTREENTRANT

/* --enable-alloca=malloc-reentrant */
#undef WANT_TMP_REENTRANT

/* Define if your processor stores words with the most significant byte first
   (like Motorola and SPARC, unlike Intel and VAX). */
#undef WORDS_BIGENDIAN

/* Define if `lex' declares `yytext' as a `char *' by default, not a `char[]'.
   */
#undef YYTEXT_POINTER

/* Define as `__inline' if that's what the C compiler calls it, or to nothing
   if it is not supported. */
#ifndef __cplusplus
#define inline	__inline
#endif

/* Define to empty if the keyword `volatile' does not work. Warning: valid
   code using `volatile' can become incorrect without. Disable with care. */
#undef volatile

#ifdef _MSC_VER
#define access _access
#define strcasecmp _stricmp
#define strncasecmp	_strnicmp
#define alloca _alloca
#define HAVE_STRCASECMP		1
#define HAVE_STRNCASECMP	1
#define va_copy(d, s) (d) = (s)
#endif
