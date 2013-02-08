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

  BLASLONG i, js, offset;

  FLOAT data01, data02, data03, data04;
  FLOAT *ao1, *ao2, *ao3, *ao4;

  js = (n >> 2);
  while (js > 0){

    offset = posX - posY;
    
    if (offset >  0) ao1 = a + posX + 0 + posY * lda; else ao1 = a + posY + (posX + 0) * lda;
    if (offset > -1) ao2 = a + posX + 1 + posY * lda; else ao2 = a + posY + (posX + 1) * lda;
    if (offset > -2) ao3 = a + posX + 2 + posY * lda; else ao3 = a + posY + (posX + 2) * lda;
    if (offset > -3) ao4 = a + posX + 3 + posY * lda; else ao4 = a + posY + (posX + 3) * lda;

    i     = m;

    while (i > 0) {
      data01 = *(ao1 + 0);
      data02 = *(ao2 + 0);
      data03 = *(ao3 + 0);
      data04 = *(ao4 + 0);
      
      if (offset >   0) ao1 += lda; else ao1 ++;
      if (offset >  -1) ao2 += lda; else ao2 ++;
      if (offset >  -2) ao3 += lda; else ao3 ++;
      if (offset >  -3) ao4 += lda; else ao4 ++;

      b[ 0] = data01;
      b[ 1] = data02;
      b[ 2] = data03;
      b[ 3] = data04;

      b += 4;

      offset --;
      i --;
    }

    posX += 4;
    js --;
  }

  if (n & 2) {

    offset = posX - posY;
    
    if (offset >  0) ao1 = a + posX + 0 + posY * lda; else ao1 = a + posY + (posX + 0) * lda;
    if (offset > -1) ao2 = a + posX + 1 + posY * lda; else ao2 = a + posY + (posX + 1) * lda;

    i     = m;

    while (i > 0) {
      data01 = *(ao1 + 0);
      data02 = *(ao2 + 0);
      
      if (offset >   0) ao1 += lda; else ao1 ++;
      if (offset >  -1) ao2 += lda; else ao2 ++;

      b[ 0] = data01;
      b[ 1] = data02;

      b += 2;

      offset --;
      i --;
    }

    posX += 2;
  }

  if (n & 1) {

    offset = posX - posY;
    
    if (offset >  0) ao1 = a + posX + 0 + posY * lda; else ao1 = a + posY + (posX + 0) * lda;

    i     = m;

    while (i > 0) {
      data01 = *(ao1 + 0);
      
      if (offset >   0) ao1 += lda; else ao1 ++;

      b[ 0] = data01;

      b ++;

      offset --;
      i --;
    }
  }

  return 0;
}
