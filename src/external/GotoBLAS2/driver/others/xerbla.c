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

#include <stdio.h>
#include <stdlib.h>
#include "common.h"

#if defined(OS_WINDOWS) && (defined(__MINGW32__) || defined(__MINGW64__))
#include <conio.h>
#undef  printf
#define printf	_cprintf
#endif

#ifdef __ELF__
int __xerbla(char *message, blasint *info, blasint length){
 
  printf(" ** On entry to %6s parameter number %2d had an illegal value\n",
	  message, *info);

  return 0;
}

int BLASFUNC(xerbla)(char *, blasint *, blasint) __attribute__ ((weak, alias ("__xerbla")));

#else

int BLASFUNC(xerbla)(char *message, blasint *info, blasint length){
 
  printf(" ** On entry to %6s parameter number %2d had an illegal value\n",
	  message, *info);

  return 0;
}

#endif
