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

#ifndef CPUID_H
#define CPUID_H

#define VENDOR_INTEL      1
#define VENDOR_UMC        2
#define VENDOR_AMD        3
#define VENDOR_CYRIX      4
#define VENDOR_NEXGEN     5
#define VENDOR_CENTAUR    6
#define VENDOR_RISE       7
#define VENDOR_SIS	  8
#define VENDOR_TRANSMETA  9
#define VENDOR_NSC	 10
#define VENDOR_UNKNOWN   99

#define BITMASK(a, b, c) ((((a) >> (b)) & (c)))

#define FAMILY_80486  4
#define FAMILY_P5     5
#define FAMILY_P6     6
#define FAMILY_PM     7
#define FAMILY_IA64   8

#if defined(__i386__) || defined(__x86_64__)
#define GET_EXFAMILY  1
#define GET_EXMODEL   2
#define GET_TYPE      3
#define GET_FAMILY    4
#define GET_MODEL     5
#define GET_APICID    6
#define GET_LCOUNT    7
#define GET_CHUNKS    8
#define GET_STEPPING  9
#define GET_BLANDID  10
#define GET_FEATURE  11
#define GET_NUMSHARE 12
#define GET_NUMCORES 13
#endif

#ifdef __ia64__
#define GET_ARCHREV   1
#define GET_FAMILY    2
#define GET_MODEL     3
#define GET_REVISION  4
#define GET_NUMBER    5
#endif

#define CORE_UNKNOWN     0
#define CORE_80486       1
#define CORE_P5          2
#define CORE_P6          3
#define CORE_KATMAI      4
#define CORE_COPPERMINE  5
#define CORE_NORTHWOOD   6
#define CORE_PRESCOTT    7
#define CORE_BANIAS      8
#define CORE_ATHLON      9
#define CORE_OPTERON    10
#define CORE_BARCELONA  11
#define CORE_VIAC3      12
#define CORE_YONAH	13
#define CORE_CORE2	14
#define CORE_PENRYN	15
#define CORE_DUNNINGTON	16
#define CORE_NEHALEM	17
#define CORE_ATOM	18
#define CORE_NANO	19

#define HAVE_SSE      (1 <<  0)
#define HAVE_SSE2     (1 <<  1)
#define HAVE_SSE3     (1 <<  2)
#define HAVE_SSSE3    (1 <<  3)
#define HAVE_SSE4_1   (1 <<  4)
#define HAVE_SSE4_2   (1 <<  5)
#define HAVE_SSE4A    (1 <<  6)
#define HAVE_SSE5     (1 <<  7)
#define HAVE_MMX      (1 <<  8)
#define HAVE_3DNOW    (1 <<  9)
#define HAVE_3DNOWEX  (1 << 10)
#define HAVE_CMOV     (1 << 11)
#define HAVE_PSE      (1 << 12)
#define HAVE_CFLUSH   (1 << 13)
#define HAVE_HIT      (1 << 14)
#define HAVE_MISALIGNSSE (1 << 15)
#define HAVE_128BITFPU   (1 << 16)
#define HAVE_FASTMOVU    (1 << 17)

#define CACHE_INFO_L1_I     1
#define CACHE_INFO_L1_D     2
#define CACHE_INFO_L2       3
#define CACHE_INFO_L3       4
#define CACHE_INFO_L1_ITB   5
#define CACHE_INFO_L1_DTB   6
#define CACHE_INFO_L1_LITB  7
#define CACHE_INFO_L1_LDTB  8
#define CACHE_INFO_L2_ITB   9
#define CACHE_INFO_L2_DTB  10
#define CACHE_INFO_L2_LITB 11
#define CACHE_INFO_L2_LDTB 12

typedef struct {
  int size;
  int associative;
  int linesize;
  int shared;
} cache_info_t;

#define CPUTYPE_UNKNOWN			 0
#define CPUTYPE_INTEL_UNKNOWN		 1
#define CPUTYPE_UMC_UNKNOWN		 2
#define CPUTYPE_AMD_UNKNOWN		 3
#define CPUTYPE_CYRIX_UNKNOWN		 4
#define CPUTYPE_NEXGEN_UNKNOWN		 5
#define CPUTYPE_CENTAUR_UNKNOWN		 6
#define CPUTYPE_RISE_UNKNOWN		 7
#define CPUTYPE_SIS_UNKNOWN		 8
#define CPUTYPE_TRANSMETA_UNKNOWN	 9
#define CPUTYPE_NSC_UNKNOWN		10

#define CPUTYPE_80386			11
#define CPUTYPE_80486			12
#define CPUTYPE_PENTIUM			13
#define CPUTYPE_PENTIUM2		14
#define CPUTYPE_PENTIUM3		15
#define CPUTYPE_PENTIUMM		16
#define CPUTYPE_PENTIUM4		17
#define CPUTYPE_CORE2			18
#define CPUTYPE_PENRYN			19
#define CPUTYPE_DUNNINGTON		20
#define CPUTYPE_NEHALEM			21
#define CPUTYPE_ATOM			22
#define CPUTYPE_ITANIUM			23
#define CPUTYPE_ITANIUM2		24
#define CPUTYPE_AMD5X86			25
#define CPUTYPE_AMDK6			26
#define CPUTYPE_ATHLON			27
#define CPUTYPE_DURON			28
#define CPUTYPE_OPTERON			29
#define CPUTYPE_BARCELONA		30
#define CPUTYPE_SHANGHAI		31
#define CPUTYPE_ISTANBUL		32
#define CPUTYPE_CYRIX5X86		33
#define CPUTYPE_CYRIXM1			34
#define CPUTYPE_CYRIXM2			35
#define CPUTYPE_NEXGENNX586		36
#define CPUTYPE_CENTAURC6		37
#define CPUTYPE_RISEMP6			38
#define CPUTYPE_SYS55X			39
#define CPUTYPE_CRUSOETM3X		40
#define CPUTYPE_NSGEODE			41
#define CPUTYPE_VIAC3			42
#define CPUTYPE_NANO			43
#endif
