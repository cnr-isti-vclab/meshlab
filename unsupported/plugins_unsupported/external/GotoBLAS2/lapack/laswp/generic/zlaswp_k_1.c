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
#include "common.h"

#ifndef MINUS
#define a2	(a1 + 2)
#else
#define a2	(a1 - 2)
#endif

int CNAME(BLASLONG n, BLASLONG k1, BLASLONG k2, FLOAT dummy1, FLOAT dummy4, 
	  FLOAT *a, BLASLONG lda, 
	  FLOAT *dummy2, BLASLONG dumy3, blasint *ipiv, BLASLONG incx){

  BLASLONG i, j, ip1, ip2;
  blasint *piv;
  FLOAT *a1;
  FLOAT *b1, *b2;
  FLOAT A1, A2, B1, B2, A3, A4, B3, B4;

  a   -= 2;
  lda *= 2;
  k1 --;

#ifndef MINUS
 ipiv += k1;
#else
  ipiv -= (k2 - 1) * incx;
#endif

  if (n  <= 0) return 0;

  j = n;
  if (j > 0) {
    do {
      piv = ipiv;

#ifndef MINUS
      a1 = a + (k1 + 1) * 2;
#else
      a1 = a + k2 * 2;
#endif

      ip1 = *piv * 2;
      piv += incx;
      ip2 = *piv * 2;
      piv += incx;
      
      b1 = a + ip1;
      b2 = a + ip2;
	  
      i = ((k2 - k1) >> 1);
      
      if (i > 0) {
	do {
#ifdef OPTERON
#ifndef MINUS
	  asm volatile("prefetchw  2 * 128(%0)\n"  : : "r"(a1));
	  asm volatile("prefetchw  2 * 128(%0)\n"  : : "r"(b1));
#else
	  asm volatile("prefetchw -2 * 128(%0)\n"  : : "r"(a1));
	  asm volatile("prefetchw -2 * 128(%0)\n"  : : "r"(b1));
#endif
#endif

#ifdef CORE2
#ifndef MINUS
	  asm volatile("prefetcht1  2 * 128(%0)\n"  : : "r"(a1));
	  asm volatile("prefetcht1  2 * 128(%0)\n"  : : "r"(b1));
	  asm volatile("prefetcht1  2 * 128(%0)\n"  : : "r"(b2));
#else
	  asm volatile("prefetcht1 -2 * 128(%0)\n"  : : "r"(a1));
	  asm volatile("prefetcht1 -2 * 128(%0)\n"  : : "r"(b1));
	  asm volatile("prefetcht1 -2 * 128(%0)\n"  : : "r"(b2));
#endif
#endif
	  A1 = *(a1 + 0);
	  A2 = *(a1 + 1);
	  A3 = *(a2 + 0);
	  A4 = *(a2 + 1);
	  B1 = *(b1 + 0);
	  B2 = *(b1 + 1);
	  B3 = *(b2 + 0);
	  B4 = *(b2 + 1);
	  
	  ip1 = *piv * 2;
	  piv += incx;
	  ip2 = *piv * 2;
	  piv += incx;
	  
	  if (b1 == a1) {
	    if (b2 == a1) {
	      *(a1 + 0) = A3;
	      *(a1 + 1) = A4;
	      *(a2 + 0) = A1;
	      *(a2 + 1) = A2;
	    } else 
	      if (b2 != a2) {
		*(a2 + 0) = B3;
		*(a2 + 1) = B4;
		*(b2 + 0) = A3;
		*(b2 + 1) = A4;
	      }
	  } else 
	    if (b1 == a2) {
	      if (b2 != a1) {
		if (b2 == a2) {
		  *(a1 + 0) = A3;
		  *(a1 + 1) = A4;
		  *(a2 + 0) = A1;
		  *(a2 + 1) = A2;
		} else {
		  *(a1 + 0) = A3;
		  *(a1 + 1) = A4;
		  *(a2 + 0) = B3;
		  *(a2 + 1) = B4;
		  *(b2 + 0) = A1;
		  *(b2 + 1) = A2;
		}
	      }
	    } else {
	      if (b2 == a1) {
		*(a1 + 0) = A3;
		*(a1 + 1) = A4;
		*(a2 + 0) = B1;
		*(a2 + 1) = B2;
		*(b1 + 0) = A1;
		*(b1 + 1) = A2;
	      } else 
		if (b2 == a2) {
		  *(a1 + 0) = B1;
		  *(a1 + 1) = B2;
		  *(b1 + 0) = A1;
		  *(b1 + 1) = A2;
		} else 
		  if (b2 == b1) {
		    *(a1 + 0) = B1;
		    *(a1 + 1) = B2;
		    *(a2 + 0) = A1;
		    *(a2 + 1) = A2;
		    *(b1 + 0) = A3;
		    *(b1 + 1) = A4;
		  } else {
		    *(a1 + 0) = B1;
		    *(a1 + 1) = B2;
		    *(a2 + 0) = B3;
		    *(a2 + 1) = B4;
		    *(b1 + 0) = A1;
		    *(b1 + 1) = A2;
		    *(b2 + 0) = A3;
		    *(b2 + 1) = A4;
		  }
	    }
	  
	  b1 = a + ip1;
	  b2 = a + ip2;
	  
#ifndef MINUS
	  a1 += 4;
#else
	  a1 -= 4;
#endif
	i --;
	} while (i > 0);
      }
      
      i = ((k2 - k1) & 1);
      
      if (i > 0) {
	A1 = *(a1 + 0);
	A2 = *(a1 + 1);
	B1 = *(b1 + 0);
	B2 = *(b1 + 1);
	*(a1 + 0) = B1;
	*(a1 + 1) = B2;
	*(b1 + 0) = A1;
	*(b1 + 1) = A2;
      }
      
      a += lda;
      
      j --;
    } while (j > 0);
  }

  return 0;
} 

