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

  FLOAT data01, data02, data03, data04, data05, data06, data07, data08;
  FLOAT data09, data10, data11, data12, data13, data14, data15, data16;
  FLOAT data17, data18, data19, data20, data21, data22, data23, data24;
  FLOAT data25, data26, data27, data28, data29, data30, data31, data32;
  FLOAT *ao1, *ao2, *ao3, *ao4;

  lda += lda;

  js = (n >> 2);
  if (js > 0){
    do {
      X = posX;

      if (posX <= posY) {
	ao1 = a + posY * 2 + (posX + 0) * lda;
	ao2 = a + posY * 2 + (posX + 1) * lda;
	ao3 = a + posY * 2 + (posX + 2) * lda;
	ao4 = a + posY * 2 + (posX + 3) * lda;
      } else {
	ao1 = a + posX * 2 + (posY + 0) * lda;
	ao2 = a + posX * 2 + (posY + 1) * lda;
	ao3 = a + posX * 2 + (posY + 2) * lda;
	ao4 = a + posX * 2 + (posY + 3) * lda;
      }

      i = (m >> 2);
      if (i > 0) {
	do {
	  if (X > posY) {
	    data01 = *(ao1 + 0);
	    data02 = *(ao1 + 1);
	    data03 = *(ao1 + 2);
	    data04 = *(ao1 + 3);
	    data05 = *(ao1 + 4);
	    data06 = *(ao1 + 5);
	    data07 = *(ao1 + 6);
	    data08 = *(ao1 + 7);
	    
	    data09 = *(ao2 + 0);
	    data10 = *(ao2 + 1);
	    data11 = *(ao2 + 2);
	    data12 = *(ao2 + 3);
	    data13 = *(ao2 + 4);
	    data14 = *(ao2 + 5);
	    data15 = *(ao2 + 6);
	    data16 = *(ao2 + 7);
	    
	    data17 = *(ao3 + 0);
	    data18 = *(ao3 + 1);
	    data19 = *(ao3 + 2);
	    data20 = *(ao3 + 3);
	    data21 = *(ao3 + 4);
	    data22 = *(ao3 + 5);
	    data23 = *(ao3 + 6);
	    data24 = *(ao3 + 7);
	    
	    data25 = *(ao4 + 0);
	    data26 = *(ao4 + 1);
	    data27 = *(ao4 + 2);
	    data28 = *(ao4 + 3);
	    data29 = *(ao4 + 4);
	    data30 = *(ao4 + 5);
	    data31 = *(ao4 + 6);
	    data32 = *(ao4 + 7);
	  
	    b[ 0] = data01;
	    b[ 1] = data02;
	    b[ 2] = data09;
	    b[ 3] = data10;
	    b[ 4] = data17;
	    b[ 5] = data18;
	    b[ 6] = data25;
	    b[ 7] = data26;
	    
	    b[ 8] = data03;
	    b[ 9] = data04;
	    b[10] = data11;
	    b[11] = data12;
	    b[12] = data19;
	    b[13] = data20;
	    b[14] = data27;
	    b[15] = data28;
	    
	    b[16] = data05;
	    b[17] = data06;
	    b[18] = data13;
	    b[19] = data14;
	    b[20] = data21;
	    b[21] = data22;
	    b[22] = data29;
	    b[23] = data30;
	    
	    b[24] = data07;
	    b[25] = data08;
	    b[26] = data15;
	    b[27] = data16;
	    b[28] = data23;
	    b[29] = data24;
	    b[30] = data31;
	    b[31] = data32;
	      
	    ao1 += 8;
	    ao2 += 8;
	    ao3 += 8;
	    ao4 += 8;
	    b += 32;

	  } else 
	    if (X < posY) {
	      ao1 += 4 * lda;
	      ao2 += 4 * lda;
	      ao3 += 4 * lda;
	      ao4 += 4 * lda;
	      b += 32;

	    } else {
#ifdef UNIT
	      data03 = *(ao1 + 2);
	      data04 = *(ao1 + 3);
	      data05 = *(ao1 + 4);
	      data06 = *(ao1 + 5);
	      data07 = *(ao1 + 6);
	      data08 = *(ao1 + 7);
	      
	      data13 = *(ao2 + 4);
	      data14 = *(ao2 + 5);
	      data15 = *(ao2 + 6);
	      data16 = *(ao2 + 7);
	    
	      data23 = *(ao3 + 6);
	      data24 = *(ao3 + 7);
	      
	      b[ 0] = ONE;
	      b[ 1] = ZERO;
	      b[ 2] = ZERO;
	      b[ 3] = ZERO;
	      b[ 4] = ZERO;
	      b[ 5] = ZERO;
	      b[ 6] = ZERO;
	      b[ 7] = ZERO;
	      
	      b[ 8] = data03;
	      b[ 9] = data04;
	      b[10] = ONE;
	      b[11] = ZERO;
	      b[12] = ZERO;
	      b[13] = ZERO;
	      b[14] = ZERO;
	      b[15] = ZERO;
	      
	      b[16] = data05;
	      b[17] = data06;
	      b[18] = data13;
	      b[19] = data14;
	      b[20] = ONE;
	      b[21] = ZERO;
	      b[22] = ZERO;
	      b[23] = ZERO;
	      
	      b[24] = data07;
	      b[25] = data08;
	      b[26] = data15;
	      b[27] = data16;
	      b[28] = data23;
	      b[29] = data24;
	      b[30] = ONE;
	      b[31] = ZERO;
#else
	      data01 = *(ao1 + 0);
	      data02 = *(ao1 + 1);
	      data03 = *(ao1 + 2);
	      data04 = *(ao1 + 3);
	      data05 = *(ao1 + 4);
	      data06 = *(ao1 + 5);
	      data07 = *(ao1 + 6);
	      data08 = *(ao1 + 7);
	      
	      data11 = *(ao2 + 2);
	      data12 = *(ao2 + 3);
	      data13 = *(ao2 + 4);
	      data14 = *(ao2 + 5);
	      data15 = *(ao2 + 6);
	      data16 = *(ao2 + 7);
	    
	      data21 = *(ao3 + 4);
	      data22 = *(ao3 + 5);
	      data23 = *(ao3 + 6);
	      data24 = *(ao3 + 7);
	      
	      data31 = *(ao4 + 6);
	      data32 = *(ao4 + 7);
	      
	      b[ 0] = data01;
	      b[ 1] = data02;
	      b[ 2] = ZERO;
	      b[ 3] = ZERO;
	      b[ 4] = ZERO;
	      b[ 5] = ZERO;
	      b[ 6] = ZERO;
	      b[ 7] = ZERO;
	      
	      b[ 8] = data03;
	      b[ 9] = data04;
	      b[10] = data11;
	      b[11] = data12;
	      b[12] = ZERO;
	      b[13] = ZERO;
	      b[14] = ZERO;
	      b[15] = ZERO;
	      
	      b[16] = data05;
	      b[17] = data06;
	      b[18] = data13;
	      b[19] = data14;
	      b[20] = data21;
	      b[21] = data22;
	      b[22] = ZERO;
	      b[23] = ZERO;
	      
	      b[24] = data07;
	      b[25] = data08;
	      b[26] = data15;
	      b[27] = data16;
	      b[28] = data23;
	      b[29] = data24;
	      b[30] = data31;
	      b[31] = data32;
#endif
	      ao1 += 8;
	      ao2 += 8;
	      ao3 += 8;
	      ao4 += 8;
	      b += 32;
	    }

	  X += 4;
	  i --;
	} while (i > 0);
      }

      i = (m & 3);
      if (i) {
      
	if (X > posY) {

	  if (m & 2) {
	    data01 = *(ao1 + 0);
	    data02 = *(ao1 + 1);
	    data03 = *(ao1 + 2);
	    data04 = *(ao1 + 3);
	    
	    data09 = *(ao2 + 0);
	    data10 = *(ao2 + 1);
	    data11 = *(ao2 + 2);
	    data12 = *(ao2 + 3);
	    
	    data17 = *(ao3 + 0);
	    data18 = *(ao3 + 1);
	    data19 = *(ao3 + 2);
	    data20 = *(ao3 + 3);
	    
	    data25 = *(ao4 + 0);
	    data26 = *(ao4 + 1);
	    data27 = *(ao4 + 2);
	    data28 = *(ao4 + 3);
	  
	    b[ 0] = data01;
	    b[ 1] = data02;
	    b[ 2] = data09;
	    b[ 3] = data10;
	    b[ 4] = data17;
	    b[ 5] = data18;
	    b[ 6] = data25;
	    b[ 7] = data26;
	    
	    b[ 8] = data03;
	    b[ 9] = data04;
	    b[10] = data11;
	    b[11] = data12;
	    b[12] = data19;
	    b[13] = data20;
	    b[14] = data27;
	    b[15] = data28;
	    
	    ao1 += 4;
	    ao2 += 4;
	    ao3 += 4;
	    ao4 += 4;
	    b += 16;
	  }
	  
	  if (m & 1) {
	    data01 = *(ao1 + 0);
	    data02 = *(ao1 + 1);
	    data09 = *(ao2 + 0);
	    data10 = *(ao2 + 1);

	    data17 = *(ao3 + 0);
	    data18 = *(ao3 + 1);
	    data25 = *(ao4 + 0);
	    data26 = *(ao4 + 1);
	  
	    b[ 0] = data01;
	    b[ 1] = data02;
	    b[ 2] = data09;
	    b[ 3] = data10;
	    b[ 4] = data17;
	    b[ 5] = data18;
	    b[ 6] = data25;
	    b[ 7] = data26;
	    
	    ao1 += 2;
	    ao2 += 2;
	    ao3 += 2;
	    ao4 += 2;
	    b += 8;
	  }
	  
	} else 
	  if (X < posY) {
	    if (m & 2) {
	      ao1 += 2 * lda;
	      ao2 += 2 * lda;
	      b += 16;
	    }
	    
	    if (m & 1) {
	      ao1 += lda;
	      b += 8;
	    }
	    
	  } else {
#ifdef UNIT

	    if (i >= 2) {
	      data03 = *(ao1 + 2);
	      data04 = *(ao1 + 3);
	    }

	    if (i >= 3) {
	      data05 = *(ao1 + 4);
	      data06 = *(ao1 + 5);
	      data13 = *(ao2 + 4);
	      data14 = *(ao2 + 5);
	    }
	    
	    b[ 0] = ONE;
	    b[ 1] = ZERO;
	    b[ 2] = ZERO;
	    b[ 3] = ZERO;
	    b[ 4] = ZERO;
	    b[ 5] = ZERO;
	    b[ 6] = ZERO;
	    b[ 7] = ZERO;
	    b += 8;

	    if (i >= 2) {
	      b[ 0] = data03;
	      b[ 1] = data04;
	      b[ 2] = ONE;
	      b[ 3] = ZERO;
	      b[ 4] = ZERO;
	      b[ 5] = ZERO;
	      b[ 6] = ZERO;
	      b[ 7] = ZERO;
	      b += 8;
	    }
	    
	    if (i >= 3) {
	      b[ 0] = data05;
	      b[ 1] = data06;
	      b[ 2] = data13;
	      b[ 3] = data14;
	      b[ 4] = ONE;
	      b[ 5] = ZERO;
	      b[ 6] = ZERO;
	      b[ 7] = ZERO;
	      b += 8;
	    }
#else
	    data01 = *(ao1 + 0);
	    data02 = *(ao1 + 1);

	    if (i >= 2) {
	      data03 = *(ao1 + 2);
	      data04 = *(ao1 + 3);
	      data11 = *(ao2 + 2);
	      data12 = *(ao2 + 3);
	    }

	    if (i >= 3) {
	      data05 = *(ao1 + 4);
	      data06 = *(ao1 + 5);
	      data13 = *(ao2 + 4);
	      data14 = *(ao2 + 5);
	      data21 = *(ao3 + 4);
	      data22 = *(ao3 + 5);
	    }
	    
	    b[ 0] = data01;
	    b[ 1] = data02;
	    b[ 2] = ZERO;
	    b[ 3] = ZERO;
	    b[ 4] = ZERO;
	    b[ 5] = ZERO;
	    b[ 6] = ZERO;
	    b[ 7] = ZERO;
	    b += 8;

	    if (i >= 2) {
	      b[ 0] = data03;
	      b[ 1] = data04;
	      b[ 2] = data11;
	      b[ 3] = data12;
	      b[ 4] = ZERO;
	      b[ 5] = ZERO;
	      b[ 6] = ZERO;
	      b[ 7] = ZERO;
	      b += 8;
	    }
	    
	    if (i >= 3) {
	      b[ 0] = data05;
	      b[ 1] = data06;
	      b[ 2] = data13;
	      b[ 3] = data14;
	      b[ 4] = data21;
	      b[ 5] = data22;
	      b[ 6] = ZERO;
	      b[ 7] = ZERO;
	      b += 8;
	    }
#endif
	  }
     }

      posY += 4;
      js --;
    } while (js > 0);
  } /* End of main loop */


  if (n & 2){
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
	    
	    data09 = *(ao2 + 0);
	    data10 = *(ao2 + 1);
	    data11 = *(ao2 + 2);
	    data12 = *(ao2 + 3);
	    
	    b[ 0] = data01;
	    b[ 1] = data02;
	    b[ 2] = data09;
	    b[ 3] = data10;
	    b[ 4] = data03;
	    b[ 5] = data04;
	    b[ 6] = data11;
	    b[ 7] = data12;

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
	      
	      data11 = *(ao2 + 2);
	      data12 = *(ao2 + 3);
	      
	      b[ 0] = data01;
	      b[ 1] = data02;
	      b[ 2] = ZERO;
	      b[ 3] = ZERO;
	      b[ 4] = data03;
	      b[ 5] = data04;
	      b[ 6] = data11;
	      b[ 7] = data12;
#endif
	      ao1 += 4;
	      ao2 += 4;
	      
	      b += 8;
	    }

	  X += 2;
	  i --;
	} while (i > 0);
      }

      i = (m & 1);
      if (i) {
      
	if (X > posY) {
	    data01 = *(ao1 + 0);
	    data02 = *(ao1 + 1);
	    data09 = *(ao2 + 0);
	    data10 = *(ao2 + 1);
	    
	    b[ 0] = data01;
	    b[ 1] = data02;
	    b[ 2] = data09;
	    b[ 3] = data10;

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
	    b[ 2] = ZERO;
	    b[ 3] = ZERO;
#else
	    data01 = *(ao1 + 0);
	    data02 = *(ao1 + 1);

	    b[ 0] = data01;
	    b[ 1] = data02;
	    b[ 2] = ZERO;
	    b[ 3] = ZERO;
#endif
	    b += 2;
	  }
      }
      posY += 2;
  }

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
	    ao1 += 2;
	    b += 2;
	  } else
	    if (X < posY) {
	      ao1 += lda;
	      b += 2;
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
	      ao1 += lda;
	      b += 2;
	    }
	  
	  X ++;
	  i --;
	} while (i > 0);
      }

      posY += 1;
  }

  return 0;
}
