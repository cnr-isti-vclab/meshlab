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

#ifndef USE_ALPHA
#define REAL_PART(a, b)  (a)
#define IMAGE_PART(a, b) (b)
#else
#define REAL_PART(a, b)  (alpha_r * (a) + alpha_i * (b))
#define IMAGE_PART(a, b) (alpha_i * (a) - alpha_r * (b))
#endif

#if defined(REAL_ONLY)
#define CMULT(a, b) (REAL_PART(a, b))
#elif defined(IMAGE_ONLY)
#define CMULT(a, b) (IMAGE_PART(a, b))
#else
#define CMULT(a, b) (REAL_PART(a, b) + IMAGE_PART(a, b))
#endif

int CNAME(BLASLONG m, BLASLONG n, FLOAT *a, BLASLONG lda, BLASLONG posX, BLASLONG posY,
#ifdef USE_ALPHA
	   FLOAT alpha_r, FLOAT alpha_i,
#endif
	   FLOAT *b){

  BLASLONG i, js, offset;

  FLOAT data01, data02;
  FLOAT *ao1, *ao2;

  lda *= 2;

  js = (n >> 1);
  while (js > 0){
    offset = posX - posY;
    
    if (offset >  0) ao1 = a + posY * 2 + (posX + 0) * lda; else ao1 = a + (posX + 0) * 2 + posY * lda;
    if (offset > -1) ao2 = a + posY * 2 + (posX + 1) * lda; else ao2 = a + (posX + 1) * 2 + posY * lda;

    i     = m;

    while (i > 0) {
      if (offset > 0) {
	data01 = CMULT(*(ao1 + 0), -*(ao1 + 1));
	data02 = CMULT(*(ao2 + 0), -*(ao2 + 1));
      } else
	if (offset < -1) {
	  data01 = CMULT(*(ao1 + 0), *(ao1 + 1));
	  data02 = CMULT(*(ao2 + 0), *(ao2 + 1));
	} else {
	  switch (offset) {
	  case  0 :
	    data01 = CMULT(*(ao1 + 0), ZERO);
	    data02 = CMULT(*(ao2 + 0), -*(ao2 + 1));
	    break;
	  case -1 :
	    data01 = CMULT(*(ao1 + 0), *(ao1 + 1));
	    data02 = CMULT(*(ao2 + 0), ZERO);
	    break;
	  }
	}
      
      if (offset >   0) ao1 += 2; else ao1 += lda;
      if (offset >  -1) ao2 += 2; else ao2 += lda;

      b[ 0] = data01;
      b[ 1] = data02;

      b += 2;

      offset --;
      i --;
    }
    
    posX += 2;
    js --;
  }

  if (n & 1) {
    offset = posX - posY;
    
    if (offset >  0) ao1 = a + posY * 2 + (posX + 0) * lda; else ao1 = a + (posX + 0) * 2 + posY * lda;

    i     = m;

    while (i > 0) {
      if (offset > 0) {
	data01 = CMULT(*(ao1 + 0), -*(ao1 + 1));
      } else
	if (offset < 0) {
	  data01 = CMULT(*(ao1 + 0), *(ao1 + 1));
	} else {
	    data01 = CMULT(*(ao1 + 0), ZERO);
	}
      
      if (offset >   0) ao1 += 2; else ao1 += lda;

      b[ 0] = data01;

      b ++;

      offset --;
      i --;
    }
  }

  return 0;
}
