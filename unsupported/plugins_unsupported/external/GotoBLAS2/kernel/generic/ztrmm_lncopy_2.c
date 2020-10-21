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

int CNAME(BLASLONG m, BLASLONG n, FLOAT *a, BLASLONG lda, BLASLONG posX, BLASLONG posY, FLOAT *b){

  BLASLONG i, js;
  BLASLONG X;

  FLOAT data01, data02, data03, data04;
  FLOAT data05, data06, data07, data08;
  FLOAT *ao1, *ao2;

  lda += lda;

  js = (n >> 1);

  if (js > 0){
    do {
      X = posX;

      if (posX <= posY) {
	ao1 = a + posY * 2 + (posX + 0) * lda;
	ao2 = a + posY * 2 + (posX + 1) * lda;
      } else {
	ao1 = a + posX * 2 + (posY + 0) * lda;
	ao2 = a + posX * 2 + (posY + 1) * lda;
      }

      i = (m >> 1);
      if (i > 0) {
	do {
	  if (X > posY) {
	    data01 = *(ao1 + 0);
	    data02 = *(ao1 + 1);
	    data03 = *(ao1 + 2);
	    data04 = *(ao1 + 3);
	    data05 = *(ao2 + 0);
	    data06 = *(ao2 + 1);
	    data07 = *(ao2 + 2);
	    data08 = *(ao2 + 3);
	  
	    b[ 0] = data01;
	    b[ 1] = data02;
	    b[ 2] = data05;
	    b[ 3] = data06;
	    b[ 4] = data03;
	    b[ 5] = data04;
	    b[ 6] = data07;
	    b[ 7] = data08;

	    ao1 += 4;
	    ao2 += 4;
	    b += 8;
	  } else 
	    if (X < posY) {
	      ao1 += 2 * lda;
	      ao2 += 2 * lda;
	      b += 8;

	    } else {
#ifdef UNIT
	      data03 = *(ao1 + 2);
	      data04 = *(ao1 + 3);

	      b[ 0] = ONE;
	      b[ 1] = ZERO;
	      b[ 2] = ZERO;
	      b[ 3] = ZERO;
	      b[ 4] = data03;
	      b[ 5] = data04;
	      b[ 6] = ONE;
	      b[ 7] = ZERO;
#else
	      data01 = *(ao1 + 0);
	      data02 = *(ao1 + 1);
	      data03 = *(ao1 + 2);
	      data04 = *(ao1 + 3);
	      data07 = *(ao2 + 2);
	      data08 = *(ao2 + 3);

	      b[ 0] = data01;
	      b[ 1] = data02;
	      b[ 2] = ZERO;
	      b[ 3] = ZERO;
	      b[ 4] = data03;
	      b[ 5] = data04;
	      b[ 6] = data07;
	      b[ 7] = data08;
#endif
	      ao1 += 4;
	      ao2 += 4;
	      b += 8;
	    }

	  X += 2;
	  i --;
	} while (i > 0);
      }

      if (m & 1) {
      
	if (X > posY) {
	  data01 = *(ao1 + 0);
	  data02 = *(ao1 + 1);
	  data03 = *(ao2 + 0);
	  data04 = *(ao2 + 1);
	    
	  b[ 0] = data01;
	  b[ 1] = data02;
	  b[ 2] = data03;
	  b[ 3] = data04;

	  ao1 += 2;
	  ao2 += 2;
	  b += 4;
	} else 
	  if (X < posY) {
	    ao1 += lda;
	    b += 4;
	  } else {
#ifdef UNIT
	    b[ 0] = ONE;
	    b[ 1] = ZERO;
	    b[ 0] = ZERO;
	    b[ 1] = ZERO;
#else
	    data01 = *(ao1 + 0);
	    data02 = *(ao1 + 1);
	    b[ 0] = data01;
	    b[ 1] = data02;
	    b[ 2] = ZERO;
	    b[ 3] = ZERO;
#endif

	    b += 4;
	  }
      }

      posY += 2;
      js --;
    } while (js > 0);
  } /* End of main loop */


  if (n & 1){
      X = posX;

      if (posX <= posY) {
	ao1 = a + posY * 2 + (posX + 0) * lda;
      } else {
	ao1 = a + posX * 2 + (posY + 0) * lda;
      }

      i = m;
      if (i > 0) {
	do {

	  if (X > posY) {
	    data01 = *(ao1 + 0);
	    data02 = *(ao1 + 1);
	    b[ 0] = data01;
	    b[ 1] = data02;
	    b += 2;
	    ao1 += 2;
	  } else 
	    if (X < posY) {
	      b += 2;
	      ao1 += lda;
	    } else {
#ifdef UNIT
	      b[ 0] = ONE;
	      b[ 1] = ZERO;
#else
	      data01 = *(ao1 + 0);
	      data02 = *(ao1 + 1);

	      b[ 0] = data01;
	      b[ 1] = data02;
#endif
	      b += 2;
	      ao1 += 2;
	    }

	  X ++;
	  i --;
	} while (i > 0);
      }

      posY += 1;
  }
  return 0;
}
