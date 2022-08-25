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

  lda *= 2;

  js = (n >> 3);
  while (js > 0){

    offset = posX - posY;
    
    if (offset >  0) ao1 = a + posY * 2 + (posX + 0) * lda; else ao1 = a + (posX + 0) * 2 + posY * lda;
    if (offset > -1) ao2 = a + posY * 2 + (posX + 1) * lda; else ao2 = a + (posX + 1) * 2 + posY * lda;
    if (offset > -2) ao3 = a + posY * 2 + (posX + 2) * lda; else ao3 = a + (posX + 2) * 2 + posY * lda;
    if (offset > -3) ao4 = a + posY * 2 + (posX + 3) * lda; else ao4 = a + (posX + 3) * 2 + posY * lda;
    if (offset > -4) ao5 = a + posY * 2 + (posX + 4) * lda; else ao5 = a + (posX + 4) * 2 + posY * lda;
    if (offset > -5) ao6 = a + posY * 2 + (posX + 5) * lda; else ao6 = a + (posX + 5) * 2 + posY * lda;
    if (offset > -6) ao7 = a + posY * 2 + (posX + 6) * lda; else ao7 = a + (posX + 6) * 2 + posY * lda;
    if (offset > -7) ao8 = a + posY * 2 + (posX + 7) * lda; else ao8 = a + (posX + 7) * 2 + posY * lda;

    i     = m;

    while (i > 0) {
      data01 = *(ao1 + 0);
      data02 = *(ao1 + 1);
      data03 = *(ao2 + 0);
      data04 = *(ao2 + 1);
      data05 = *(ao3 + 0);
      data06 = *(ao3 + 1);
      data07 = *(ao4 + 0);
      data08 = *(ao4 + 1);
      data09 = *(ao5 + 0);
      data10 = *(ao5 + 1);
      data11 = *(ao6 + 0);
      data12 = *(ao6 + 1);
      data13 = *(ao7 + 0);
      data14 = *(ao7 + 1);
      data15 = *(ao8 + 0);
      data16 = *(ao8 + 1);
      
      if (offset >   0) ao1 += 2; else ao1 += lda;
      if (offset >  -1) ao2 += 2; else ao2 += lda;
      if (offset >  -2) ao3 += 2; else ao3 += lda;
      if (offset >  -3) ao4 += 2; else ao4 += lda;
      if (offset >  -4) ao5 += 2; else ao5 += lda;
      if (offset >  -5) ao6 += 2; else ao6 += lda;
      if (offset >  -6) ao7 += 2; else ao7 += lda;
      if (offset >  -7) ao8 += 2; else ao8 += lda;

      if (offset > 0) {
	b[ 0] = data01;
	b[ 1] = -data02;
	b[ 2] = data03;
	b[ 3] = -data04;
	b[ 4] = data05;
	b[ 5] = -data06;
	b[ 6] = data07;
	b[ 7] = -data08;
	b[ 8] = data09;
	b[ 9] = -data10;
	b[10] = data11;
	b[11] = -data12;
	b[12] = data13;
	b[13] = -data14;
	b[14] = data15;
	b[15] = -data16;
      } else
	if (offset < -7) {
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
	} else {
	  switch (offset) {
	  case  0 :
	    b[ 0] = data01;
	    b[ 1] = ZERO;
	    b[ 2] = data03;
	    b[ 3] = -data04;
	    b[ 4] = data05;
	    b[ 5] = -data06;
	    b[ 6] = data07;
	    b[ 7] = -data08;
	    b[ 8] = data09;
	    b[ 9] = -data10;
	    b[10] = data11;
	    b[11] = -data12;
	    b[12] = data13;
	    b[13] = -data14;
	    b[14] = data15;
	    b[15] = -data16;
	    break;
	  case -1 :
	    b[ 0] = data01;
	    b[ 1] = data02;
	    b[ 2] = data03;
	    b[ 3] = ZERO;
	    b[ 4] = data05;
	    b[ 5] = -data06;
	    b[ 6] = data07;
	    b[ 7] = -data08;
	    b[ 8] = data09;
	    b[ 9] = -data10;
	    b[10] = data11;
	    b[11] = -data12;
	    b[12] = data13;
	    b[13] = -data14;
	    b[14] = data15;
	    b[15] = -data16;
	    break;
	  case -2 :
	    b[ 0] = data01;
	    b[ 1] = data02;
	    b[ 2] = data03;
	    b[ 3] = data04;
	    b[ 4] = data05;
	    b[ 5] = ZERO;
	    b[ 6] = data07;
	    b[ 7] = -data08;
	    b[ 8] = data09;
	    b[ 9] = -data10;
	    b[10] = data11;
	    b[11] = -data12;
	    b[12] = data13;
	    b[13] = -data14;
	    b[14] = data15;
	    b[15] = -data16;
	    break;
	  case -3 :
	    b[ 0] = data01;
	    b[ 1] = data02;
	    b[ 2] = data03;
	    b[ 3] = data04;
	    b[ 4] = data05;
	    b[ 5] = data06;
	    b[ 6] = data07;
	    b[ 7] = ZERO;
	    b[ 8] = data09;
	    b[ 9] = -data10;
	    b[10] = data11;
	    b[11] = -data12;
	    b[12] = data13;
	    b[13] = -data14;
	    b[14] = data15;
	    b[15] = -data16;
	    break;
	  case -4 :
	    b[ 0] = data01;
	    b[ 1] = data02;
	    b[ 2] = data03;
	    b[ 3] = data04;
	    b[ 4] = data05;
	    b[ 5] = data06;
	    b[ 6] = data07;
	    b[ 7] = data08;
	    b[ 8] = data09;
	    b[ 9] = ZERO;
	    b[10] = data11;
	    b[11] = -data12;
	    b[12] = data13;
	    b[13] = -data14;
	    b[14] = data15;
	    b[15] = -data16;
	    break;
	  case -5 :
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
	    b[11] = ZERO;
	    b[12] = data13;
	    b[13] = -data14;
	    b[14] = data15;
	    b[15] = -data16;
	    break;
	  case -6 :
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
	    b[13] = ZERO;
	    b[14] = data15;
	    b[15] = -data16;
	    break;
	  case -7 :
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
	    b[15] = ZERO;
	    break;
	  }
	}
  
      b += 16;

      offset --;
      i --;
    }

    posX += 8;
    js --;
  }

  if (n & 4) {
    offset = posX - posY;
    
    if (offset >  0) ao1 = a + posY * 2 + (posX + 0) * lda; else ao1 = a + (posX + 0) * 2 + posY * lda;
    if (offset > -1) ao2 = a + posY * 2 + (posX + 1) * lda; else ao2 = a + (posX + 1) * 2 + posY * lda;
    if (offset > -2) ao3 = a + posY * 2 + (posX + 2) * lda; else ao3 = a + (posX + 2) * 2 + posY * lda;
    if (offset > -3) ao4 = a + posY * 2 + (posX + 3) * lda; else ao4 = a + (posX + 3) * 2 + posY * lda;

    i     = m;

    while (i > 0) {
      data01 = *(ao1 + 0);
      data02 = *(ao1 + 1);
      data03 = *(ao2 + 0);
      data04 = *(ao2 + 1);
      data05 = *(ao3 + 0);
      data06 = *(ao3 + 1);
      data07 = *(ao4 + 0);
      data08 = *(ao4 + 1);
      
      if (offset >   0) ao1 += 2; else ao1 += lda;
      if (offset >  -1) ao2 += 2; else ao2 += lda;
      if (offset >  -2) ao3 += 2; else ao3 += lda;
      if (offset >  -3) ao4 += 2; else ao4 += lda;

      if (offset > 0) {
	b[ 0] = data01;
	b[ 1] = -data02;
	b[ 2] = data03;
	b[ 3] = -data04;
	b[ 4] = data05;
	b[ 5] = -data06;
	b[ 6] = data07;
	b[ 7] = -data08;
      } else
	if (offset < -3) {
	  b[ 0] = data01;
	  b[ 1] = data02;
	  b[ 2] = data03;
	  b[ 3] = data04;
	  b[ 4] = data05;
	  b[ 5] = data06;
	  b[ 6] = data07;
	  b[ 7] = data08;
	} else {
	  switch (offset) {
	  case  0 :
	    b[ 0] = data01;
	    b[ 1] = ZERO;
	    b[ 2] = data03;
	    b[ 3] = -data04;
	    b[ 4] = data05;
	    b[ 5] = -data06;
	    b[ 6] = data07;
	    b[ 7] = -data08;
	    break;
	  case -1 :
	    b[ 0] = data01;
	    b[ 1] = data02;
	    b[ 2] = data03;
	    b[ 3] = ZERO;
	    b[ 4] = data05;
	    b[ 5] = -data06;
	    b[ 6] = data07;
	    b[ 7] = -data08;
	    break;
	  case -2 :
	    b[ 0] = data01;
	    b[ 1] = data02;
	    b[ 2] = data03;
	    b[ 3] = data04;
	    b[ 4] = data05;
	    b[ 5] = ZERO;
	    b[ 6] = data07;
	    b[ 7] = -data08;
	    break;
	  case -3 :
	    b[ 0] = data01;
	    b[ 1] = data02;
	    b[ 2] = data03;
	    b[ 3] = data04;
	    b[ 4] = data05;
	    b[ 5] = data06;
	    b[ 6] = data07;
	    b[ 7] = ZERO;
	    break;
	  }
	}
  
      b += 8;

      offset --;
      i --;
    }

    posX += 4;
  }

  if (n & 2) {

    offset = posX - posY;
    
    if (offset >  0) ao1 = a + posY * 2 + (posX + 0) * lda; else ao1 = a + (posX + 0) * 2 + posY * lda;
    if (offset > -1) ao2 = a + posY * 2 + (posX + 1) * lda; else ao2 = a + (posX + 1) * 2 + posY * lda;

    i     = m;

    while (i > 0) {
      data01 = *(ao1 + 0);
      data02 = *(ao1 + 1);
      data03 = *(ao2 + 0);
      data04 = *(ao2 + 1);
      
      if (offset >   0) ao1 += 2; else ao1 += lda;
      if (offset >  -1) ao2 += 2; else ao2 += lda;

      if (offset > 0) {
	b[ 0] = data01;
	b[ 1] = -data02;
	b[ 2] = data03;
	b[ 3] = -data04;
      } else
	if (offset < -1) {
	  b[ 0] = data01;
	  b[ 1] = data02;
	  b[ 2] = data03;
	  b[ 3] = data04;
	} else {
	  switch (offset) {
	  case  0 :
	    b[ 0] = data01;
	    b[ 1] = ZERO;
	    b[ 2] = data03;
	    b[ 3] = -data04;
	    break;
	  case -1 :
	    b[ 0] = data01;
	    b[ 1] = data02;
	    b[ 2] = data03;
	    b[ 3] = ZERO;
	    break;
	  }
	}

      b += 4;

      offset --;
      i --;
    }

    posX += 2;
  }

  if (n & 1) {

    offset = posX - posY;
    
    if (offset >  0) ao1 = a + posY * 2 + (posX + 0) * lda; else ao1 = a + (posX + 0) * 2 + posY * lda;

    i     = m;

    while (i > 0) {
      data01 = *(ao1 + 0);
      data02 = *(ao1 + 1);
      
      if (offset >   0) ao1 += 2; else ao1 += lda;

      if (offset > 0) {
	b[ 0] = data01;
	b[ 1] = -data02;
      } else
	if (offset < 0) {
	  b[ 0] = data01;
	  b[ 1] = data02;
	} else {
	    b[ 0] = data01;
	    b[ 1] = ZERO;
	}

      b += 2;

      offset --;
      i --;
    }

  }

  return 0;
}
