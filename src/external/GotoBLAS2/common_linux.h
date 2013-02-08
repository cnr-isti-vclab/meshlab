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

#ifndef COMMON_LINUX_H
#define COMMON_LINUX_H

#ifndef ASSEMBLER

#include <syscall.h>

extern long int syscall (long int __sysno, ...);

#ifndef MPOL_PREFERRED
#define MPOL_PREFERRED 1
#endif

#ifndef MPOL_INTERLEAVE
#define MPOL_INTERLEAVE 3
#endif

#if defined(ARCH_IA64) && defined(__ECC)
#ifndef __NR_mbind
#define __NR_mbind			1259
#endif
#ifndef __NR_get_mempolicy
#define __NR_get_mempolicy		1260
#endif
#ifndef __NR_set_mempolicy
#define __NR_set_mempolicy		1261
#endif
#endif

static inline int my_mbind(void *addr, unsigned long len, int mode,
			   unsigned long *nodemask, unsigned long maxnode,
			   unsigned flags) {
  
  return syscall(SYS_mbind, addr, len, mode, nodemask, maxnode, flags);
}

static inline int my_set_mempolicy(int mode, const unsigned long *addr, unsigned long flag) {

  return syscall(SYS_set_mempolicy, mode, addr, flag);
}

static inline int my_gettid(void) { return syscall(SYS_gettid); }

#endif
#endif
