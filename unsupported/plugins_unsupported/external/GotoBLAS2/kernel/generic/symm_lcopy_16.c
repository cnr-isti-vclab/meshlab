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

  FLOAT data01, data02, data03, data04, data05, data06, data07, data08;
  FLOAT data09, data10, data11, data12, data13, data14, data15, data16;
  FLOAT *ao1, *ao2,  *ao3,  *ao4,  *ao5,  *ao6,  *ao7,  *ao8;
  FLOAT *ao9, *ao10, *ao11, *ao12, *ao13, *ao14, *ao15, *ao16;

  js = (n >> 4);
  while (js > 0){

    offset = posX - posY;
    
    if (offset >   0) ao1  = a + posX +  0 + posY * lda; else ao1  = a + posY + (posX +  0) * lda;
    if (offset >  -1) ao2  = a + posX +  1 + posY * lda; else ao2  = a + posY + (posX +  1) * lda;
    if (offset >  -2) ao3  = a + posX +  2 + posY * lda; else ao3  = a + posY + (posX +  2) * lda;
    if (offset >  -3) ao4  = a + posX +  3 + posY * lda; else ao4  = a + posY + (posX +  3) * lda;
    if (offset >  -4) ao5  = a + posX +  4 + posY * lda; else ao5  = a + posY + (posX +  4) * lda;
    if (offset >  -5) ao6  = a + posX +  5 + posY * lda; else ao6  = a + posY + (posX +  5) * lda;
    if (offset >  -6) ao7  = a + posX +  6 + posY * lda; else ao7  = a + posY + (posX +  6) * lda;
    if (offset >  -7) ao8  = a + posX +  7 + posY * lda; else ao8  = a + posY + (posX +  7) * lda;
    if (offset >  -8) ao9  = a + posX +  8 + posY * lda; else ao9  = a + posY + (posX +  8) * lda;
    if (offset >  -9) ao10 = a + posX +  9 + posY * lda; else ao10 = a + posY + (posX +  9) * lda;
    if (offset > -10) ao11 = a + posX + 10 + posY * lda; else ao11 = a + posY + (posX + 10) * lda;
    if (offset > -11) ao12 = a + posX + 11 + posY * lda; else ao12 = a + posY + (posX + 11) * lda;
    if (offset > -12) ao13 = a + posX + 12 + posY * lda; else ao13 = a + posY + (posX + 12) * lda;
    if (offset > -13) ao14 = a + posX + 13 + posY * lda; else ao14 = a + posY + (posX + 13) * lda;
    if (offset > -14) ao15 = a + posX + 14 + posY * lda; else ao15 = a + posY + (posX + 14) * lda;
    if (offset > -15) ao16 = a + posX + 15 + posY * lda; else ao16 = a + posY + (posX + 15) * lda;

    i     = m;

    while (i > 0) {
      data01 = *(ao1  + 0);
      data02 = *(ao2  + 0);
      data03 = *(ao3  + 0);
      data04 = *(ao4  + 0);
      data05 = *(ao5  + 0);
      data06 = *(ao6  + 0);
      data07 = *(ao7  + 0);
      data08 = *(ao8  + 0);
      data09 = *(ao9  + 0);
      data10 = *(ao10 + 0);
      data11 = *(ao11 + 0);
      data12 = *(ao12 + 0);
      data13 = *(ao13 + 0);
      data14 = *(ao14 + 0);
      data15 = *(ao15 + 0);
      data16 = *(ao16 + 0);

      if (offset >   0) ao1  += lda; else ao1 ++;
      if (offset >  -1) ao2  += lda; else ao2 ++;
      if (offset >  -2) ao3  += lda; else ao3 ++;
      if (offset >  -3) ao4  += lda; else ao4 ++;
      if (offset >  -4) ao5  += lda; else ao5 ++;
      if (offset >  -5) ao6  += lda; else ao6 ++;
      if (offset >  -6) ao7  += lda; else ao7 ++;
      if (offset >  -7) ao8  += lda; else ao8 ++;
      if (offset >  -8) ao9  += lda; else ao9 ++;
      if (offset >  -9) ao10 += lda; else ao10 ++;
      if (offset > -10) ao11 += lda; else ao11 ++;
      if (offset > -11) ao12 += lda; else ao12 ++;
      if (offset > -12) ao13 += lda; else ao13 ++;
      if (offset > -13) ao14 += lda; else ao14 ++;
      if (offset > -14) ao15 += lda; else ao15 ++;
      if (offset > -15) ao16 += lda; else ao16 ++;
      
      b[ 0] = data01;
      b[ 1] = data02;
      b[ 2] = data03;
      b[ 3] = data04;
      b[ 4] = data05;
      b[ 5] = data06;
      b[ 6] = data07;
      b[ 7] = data08;
      b[ 8] = data09;
      b[ 9] = data10;
      b[10] = data11;
      b[11] = data12;
      b[12] = data13;
      b[13] = data14;
      b[14] = data15;
      b[15] = data16;

      b += 16;

      offset --;
      i --;
    }

    posX += 16;
    js --;
  }

  if (n & 8) {

    offset = posX - posY;
    
    if (offset >  0) ao1 = a + posX + 0 + posY * lda; else ao1 = a + posY + (posX + 0) * lda;
    if (offset > -1) ao2 = a + posX + 1 + posY * lda; else ao2 = a + posY + (posX + 1) * lda;
    if (offset > -2) ao3 = a + posX + 2 + posY * lda; else ao3 = a + posY + (posX + 2) * lda;
    if (offset > -3) ao4 = a + posX + 3 + posY * lda; else ao4 = a + posY + (posX + 3) * lda;
    if (offset > -4) ao5 = a + posX + 4 + posY * lda; else ao5 = a + posY + (posX + 4) * lda;
    if (offset > -5) ao6 = a + posX + 5 + posY * lda; else ao6 = a + posY + (posX + 5) * lda;
    if (offset > -6) ao7 = a + posX + 6 + posY * lda; else ao7 = a + posY + (posX + 6) * lda;
    if (offset > -7) ao8 = a + posX + 7 + posY * lda; else ao8 = a + posY + (posX + 7) * lda;

    i     = m;

    while (i > 0) {
      data01 = *(ao1 + 0);
      data02 = *(ao2 + 0);
      data03 = *(ao3 + 0);
      data04 = *(ao4 + 0);
      data05 = *(ao5 + 0);
      data06 = *(ao6 + 0);
      data07 = *(ao7 + 0);
      data08 = *(ao8 + 0);
      
      if (offset >   0) ao1 += lda; else ao1 ++;
      if (offset >  -1) ao2 += lda; else ao2 ++;
      if (offset >  -2) ao3 += lda; else ao3 ++;
      if (offset >  -3) ao4 += lda; else ao4 ++;
      if (offset >  -4) ao5 += lda; else ao5 ++;
      if (offset >  -5) ao6 += lda; else ao6 ++;
      if (offset >  -6) ao7 += lda; else ao7 ++;
      if (offset >  -7) ao8 += lda; else ao8 ++;

      b[ 0] = data01;
      b[ 1] = data02;
      b[ 2] = data03;
      b[ 3] = data04;
      b[ 4] = data05;
      b[ 5] = data06;
      b[ 6] = data07;
      b[ 7] = data08;

      b += 8;

      offset --;
      i --;
    }

    posX += 8;
  }

  if (n & 4) {
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
