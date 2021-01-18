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

#define PREFETCHSIZE 12

#define a2	(a1 + 1)
#define a4	(a3 + 1)

int CNAME(BLASLONG n, BLASLONG k1, BLASLONG k2, FLOAT *a, BLASLONG lda, blasint *ipiv, FLOAT *buffer){

  BLASLONG i, j, ip1, ip2;
  blasint *piv;
  FLOAT *a1, *a3;
  FLOAT *b1, *b2, *b3, *b4;
  FLOAT A1, A2, B1, B2, A3, A4, B3, B4;

  a--;
  k1 --;

 ipiv += k1;

  if (n  <= 0) return 0;
      
  j = (n >> 1);
  if (j > 0) {
    do {
      piv = ipiv;
      
      a1 = a + k1 + 1;
      a3 = a1 + 1 * lda;
      
      ip1 = *(piv + 0);
      ip2 = *(piv + 1);
      piv += 2;
      
      b1 = a + ip1;
      b2 = a + ip2;
      
      b3 = b1 + 1 * lda;
      b4 = b2 + 1 * lda;
      
      i = ((k2 - k1) >> 1);
      
      if (i > 0) {
	do {

#ifdef __GNUC__
	  __builtin_prefetch(a1 + PREFETCHSIZE, 0, 0);
	  __builtin_prefetch(a3 + PREFETCHSIZE, 0, 0);
#endif

	  A1 = *a1;
	  A2 = *a2;
	  A3 = *a3;
	  A4 = *a4;
	  
	  B1 = *b1;
	  B2 = *b2;
	  B3 = *b3;
	  B4 = *b4;
	  
	  ip1 = *(piv + 0);
	  ip2 = *(piv + 1);
	  piv += 2;
	  
	  if (b1 == a1) {
	    if (b2 == a2) {
	      *(buffer + 0) = A1;
	      *(buffer + 1) = A3;
	      *(buffer + 2) = A2;
	      *(buffer + 3) = A4;
	    } else {
	      *(buffer + 0) = A1;
	      *(buffer + 1) = A3;
	      *(buffer + 2) = B2;
	      *(buffer + 3) = B4;
	      
	      *b2 = A2;
	      *b4 = A4;
	    }
	} else 
	  if (b1 == a2) {
	    if (b2 == a2) {
	      *(buffer + 0) = A2;
	      *(buffer + 1) = A4;
	      *(buffer + 2) = A1;
	      *(buffer + 3) = A3;
	    } else {
	      *(buffer + 0) = A2;
	      *(buffer + 1) = A4;
	      *(buffer + 2) = B2;
	      *(buffer + 3) = B4;
	      *b2 = A1;
	      *b4 = A3;
	    }
	  } else {
	    if (b2 == a2) {
	      *(buffer + 0) = B1;
	      *(buffer + 1) = B3;
	      *(buffer + 2) = A2;
	      *(buffer + 3) = A4;
	      *b1 = A1;
	      *b3 = A3;
	    } else 
	      if (b2 == b1) {
		*(buffer + 0) = B1;
		*(buffer + 1) = B3;
		*(buffer + 2) = A1;
		*(buffer + 3) = A3;
		*b1 = A2;
		*b3 = A4;
	      } else {
		*(buffer + 0) = B1;
		*(buffer + 1) = B3;
		*(buffer + 2) = B2;
		*(buffer + 3) = B4;
		*b1 = A1;
		*b2 = A2;
		*b3 = A3;
		*b4 = A4;
	      }
	  }
	  
	  buffer += 4;
	  
	  b1 = a + ip1;
	  b2 = a + ip2;
	  
	  b3 = b1 + 1 * lda;
	  b4 = b2 + 1 * lda;
	  
	  a1 += 2;
	  a3 += 2;
	  
	  i --;
	} while (i > 0);
      }
      
      i = ((k2 - k1) & 1);
      
      if (i > 0) {
	A1 = *a1;
	B1 = *b1;
	A3 = *a3;
	B3 = *b3;
	
	if (a1 == b1) {
	  *(buffer + 0) = A1;
	  *(buffer + 1) = A3;
	} else {
	  *(buffer + 0) = B1;
	  *(buffer + 1) = B3;
	  *b1 = A1;
	  *b3 = A3;
	}
	buffer += 2;
      }
      
      a += 2 * lda;
      j --;
    } while (j > 0);
  }
  
  if (n & 1) {
    piv = ipiv;

    a1 = a + k1 + 1;
    
    ip1 = *(piv + 0);
    ip2 = *(piv + 1);
    piv += 2;
    
    b1 = a + ip1;
    b2 = a + ip2;
    
    i = ((k2 - k1) >> 1);
    
    if (i > 0) {
      do {
	A1 = *a1;
	A2 = *a2;
	B1 = *b1;
	B2 = *b2;
	
	ip1 = *(piv + 0);
	ip2 = *(piv + 1);
	piv += 2;
	
	if (b1 == a1) {
	    if (b2 == a2) {
	      *(buffer + 0) = A1;
	      *(buffer + 1) = A2;
	    } else {
	      *(buffer + 0) = A1;
	      *(buffer + 1) = B2;

	      *b2 = A2;
	    }
	} else 
	  if (b1 == a2) {
	      if (b2 == a2) {
		*(buffer + 0) = A2;
		*(buffer + 1) = A1;
	      } else {
		*(buffer + 0) = A2;
		*(buffer + 1) = B2;
		*b2 = A1;
	      }
	  } else {
	      if (b2 == a2) {
		*(buffer + 0) = B1;
		*(buffer + 1) = A2;
		*b1 = A1;
	      } else 
		if (b2 == b1) {
		  *(buffer + 0) = B1;
		  *(buffer + 1) = A1;
		  *b1 = A2;
		} else {
		  *(buffer + 0) = B1;
		  *(buffer + 1) = B2;
		  *b1 = A1;
		  *b2 = A2;
		}
	  }
	
	buffer += 2;

	b1 = a + ip1;
	b2 = a + ip2;
	
	a1 += 2;

	i --;
      } while (i > 0);
    }
    
    i = ((k2 - k1) & 1);
    
    if (i > 0) {
      A1 = *a1;
      B1 = *b1;

      if (a1 == b1) {
	*(buffer + 0) = A1;
      } else {
	*(buffer + 0) = B1;
	*b1 = A1;
      }
    }
  }

  return 0;
} 

