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

int CNAME(BLASLONG m, BLASLONG n, FLOAT *a, BLASLONG lda, FLOAT *b){

  BLASLONG i, j;

  FLOAT *aoffset;
  FLOAT *aoffset1, *aoffset2;
  FLOAT *boffset;

  FLOAT ctemp01, ctemp02, ctemp03, ctemp04;
  FLOAT ctemp05, ctemp06, ctemp07, ctemp08;
  FLOAT ctemp09, ctemp10, ctemp11, ctemp12;
  FLOAT ctemp13, ctemp14, ctemp15, ctemp16;
  FLOAT ctemp17, ctemp18, ctemp19, ctemp20;
  FLOAT ctemp21, ctemp22, ctemp23, ctemp24;
  FLOAT ctemp25, ctemp26, ctemp27, ctemp28;
  FLOAT ctemp29, ctemp30, ctemp31, ctemp32;

  aoffset   = a;
  boffset   = b;

#if 0
  fprintf(stderr, "m = %d n = %d\n", m, n);
#endif

  j = (n >> 4);
  if (j > 0){
    do{
      aoffset1  = aoffset;
      aoffset2  = aoffset + lda;
      aoffset += 16;
      
      i = (m >> 1);
      if (i > 0){
	do{
	  ctemp01 = *(aoffset1 +  0);
	  ctemp02 = *(aoffset1 +  1);
	  ctemp03 = *(aoffset1 +  2);
	  ctemp04 = *(aoffset1 +  3);
	  ctemp05 = *(aoffset1 +  4);
	  ctemp06 = *(aoffset1 +  5);
	  ctemp07 = *(aoffset1 +  6);
	  ctemp08 = *(aoffset1 +  7);
	  ctemp09 = *(aoffset1 +  8);
	  ctemp10 = *(aoffset1 +  9);
	  ctemp11 = *(aoffset1 + 10);
	  ctemp12 = *(aoffset1 + 11);
	  ctemp13 = *(aoffset1 + 12);
	  ctemp14 = *(aoffset1 + 13);
	  ctemp15 = *(aoffset1 + 14);
	  ctemp16 = *(aoffset1 + 15);

	  ctemp17 = *(aoffset2 +  0);
	  ctemp18 = *(aoffset2 +  1);
	  ctemp19 = *(aoffset2 +  2);
	  ctemp20 = *(aoffset2 +  3);
	  ctemp21 = *(aoffset2 +  4);
	  ctemp22 = *(aoffset2 +  5);
	  ctemp23 = *(aoffset2 +  6);
	  ctemp24 = *(aoffset2 +  7);
	  ctemp25 = *(aoffset2 +  8);
	  ctemp26 = *(aoffset2 +  9);
	  ctemp27 = *(aoffset2 + 10);
	  ctemp28 = *(aoffset2 + 11);
	  ctemp29 = *(aoffset2 + 12);
	  ctemp30 = *(aoffset2 + 13);
	  ctemp31 = *(aoffset2 + 14);
	  ctemp32 = *(aoffset2 + 15);

	  *(boffset +  0) = -ctemp01;
	  *(boffset +  1) = -ctemp02;
	  *(boffset +  2) = -ctemp03;
	  *(boffset +  3) = -ctemp04;
	  *(boffset +  4) = -ctemp05;
	  *(boffset +  5) = -ctemp06;
	  *(boffset +  6) = -ctemp07;
	  *(boffset +  7) = -ctemp08;
	  
	  *(boffset +  8) = -ctemp09;
	  *(boffset +  9) = -ctemp10;
	  *(boffset + 10) = -ctemp11;
	  *(boffset + 11) = -ctemp12;
	  *(boffset + 12) = -ctemp13;
	  *(boffset + 13) = -ctemp14;
	  *(boffset + 14) = -ctemp15;
	  *(boffset + 15) = -ctemp16;
	  
	  *(boffset + 16) = -ctemp17;
	  *(boffset + 17) = -ctemp18;
	  *(boffset + 18) = -ctemp19;
	  *(boffset + 19) = -ctemp20;
	  *(boffset + 20) = -ctemp21;
	  *(boffset + 21) = -ctemp22;
	  *(boffset + 22) = -ctemp23;
	  *(boffset + 23) = -ctemp24;

	  *(boffset + 24) = -ctemp25;
	  *(boffset + 25) = -ctemp26;
	  *(boffset + 26) = -ctemp27;
	  *(boffset + 27) = -ctemp28;
	  *(boffset + 28) = -ctemp29;
	  *(boffset + 29) = -ctemp30;
	  *(boffset + 30) = -ctemp31;
	  *(boffset + 31) = -ctemp32;

	  aoffset1 +=  2 * lda;
	  aoffset2 +=  2 * lda;
	  boffset   += 32;

	  i --;
	}while(i > 0);
      }

      if (m & 1){
	ctemp01 = *(aoffset1 +  0);
	ctemp02 = *(aoffset1 +  1);
	ctemp03 = *(aoffset1 +  2);
	ctemp04 = *(aoffset1 +  3);
	ctemp05 = *(aoffset1 +  4);
	ctemp06 = *(aoffset1 +  5);
	ctemp07 = *(aoffset1 +  6);
	ctemp08 = *(aoffset1 +  7);
	ctemp09 = *(aoffset1 +  8);
	ctemp10 = *(aoffset1 +  9);
	ctemp11 = *(aoffset1 + 10);
	ctemp12 = *(aoffset1 + 11);
	ctemp13 = *(aoffset1 + 12);
	ctemp14 = *(aoffset1 + 13);
	ctemp15 = *(aoffset1 + 14);
	ctemp16 = *(aoffset1 + 15);
	
	*(boffset +  0) = -ctemp01;
	*(boffset +  1) = -ctemp02;
	*(boffset +  2) = -ctemp03;
	*(boffset +  3) = -ctemp04;
	*(boffset +  4) = -ctemp05;
	*(boffset +  5) = -ctemp06;
	*(boffset +  6) = -ctemp07;
	*(boffset +  7) = -ctemp08;
	
	*(boffset +  8) = -ctemp09;
	*(boffset +  9) = -ctemp10;
	*(boffset + 10) = -ctemp11;
	*(boffset + 11) = -ctemp12;
	*(boffset + 12) = -ctemp13;
	*(boffset + 13) = -ctemp14;
	*(boffset + 14) = -ctemp15;
	*(boffset + 15) = -ctemp16;

	boffset   += 16;
      }

      j--;
    }while(j > 0);
  } /* end of if(j > 0) */

  if (n & 8){
    aoffset1  = aoffset;
    aoffset2  = aoffset + lda;
    aoffset += 8;
    
    i = (m >> 1);
    if (i > 0){
      do{
	ctemp01 = *(aoffset1 +  0);
	ctemp02 = *(aoffset1 +  1);
	ctemp03 = *(aoffset1 +  2);
	ctemp04 = *(aoffset1 +  3);
	ctemp05 = *(aoffset1 +  4);
	ctemp06 = *(aoffset1 +  5);
	ctemp07 = *(aoffset1 +  6);
	ctemp08 = *(aoffset1 +  7);
	  
	ctemp09 = *(aoffset2 +  0);
	ctemp10 = *(aoffset2 +  1);
	ctemp11 = *(aoffset2 +  2);
	ctemp12 = *(aoffset2 +  3);
	ctemp13 = *(aoffset2 +  4);
	ctemp14 = *(aoffset2 +  5);
	ctemp15 = *(aoffset2 +  6);
	ctemp16 = *(aoffset2 +  7);

	*(boffset +  0) = -ctemp01;
	*(boffset +  1) = -ctemp02;
	*(boffset +  2) = -ctemp03;
	*(boffset +  3) = -ctemp04;
	*(boffset +  4) = -ctemp05;
	*(boffset +  5) = -ctemp06;
	*(boffset +  6) = -ctemp07;
	*(boffset +  7) = -ctemp08;
	  
	*(boffset +  8) = -ctemp09;
	*(boffset +  9) = -ctemp10;
	*(boffset + 10) = -ctemp11;
	*(boffset + 11) = -ctemp12;
	*(boffset + 12) = -ctemp13;
	*(boffset + 13) = -ctemp14;
	*(boffset + 14) = -ctemp15;
	*(boffset + 15) = -ctemp16;
	
	aoffset1 +=  2 * lda;
	aoffset2 +=  2 * lda;
	boffset   += 16;
	
	i --;
      }while(i > 0);
    }
    
    if (m & 1){
      ctemp01 = *(aoffset1 +  0);
      ctemp02 = *(aoffset1 +  1);
      ctemp03 = *(aoffset1 +  2);
      ctemp04 = *(aoffset1 +  3);
      ctemp05 = *(aoffset1 +  4);
      ctemp06 = *(aoffset1 +  5);
      ctemp07 = *(aoffset1 +  6);
      ctemp08 = *(aoffset1 +  7);
	
      *(boffset +  0) = -ctemp01;
      *(boffset +  1) = -ctemp02;
      *(boffset +  2) = -ctemp03;
      *(boffset +  3) = -ctemp04;
      *(boffset +  4) = -ctemp05;
      *(boffset +  5) = -ctemp06;
      *(boffset +  6) = -ctemp07;
      *(boffset +  7) = -ctemp08;
      
      boffset   += 8;
    }
  }

  if (n & 4){
    aoffset1  = aoffset;
    aoffset2  = aoffset + lda;
    aoffset += 4;
    
    i = (m >> 1);
    if (i > 0){
      do{
	ctemp01 = *(aoffset1 +  0);
	ctemp02 = *(aoffset1 +  1);
	ctemp03 = *(aoffset1 +  2);
	ctemp04 = *(aoffset1 +  3);

	ctemp05 = *(aoffset2 +  0);
	ctemp06 = *(aoffset2 +  1);
	ctemp07 = *(aoffset2 +  2);
	ctemp08 = *(aoffset2 +  3);

	*(boffset +  0) = -ctemp01;
	*(boffset +  1) = -ctemp02;
	*(boffset +  2) = -ctemp03;
	*(boffset +  3) = -ctemp04;
	*(boffset +  4) = -ctemp05;
	*(boffset +  5) = -ctemp06;
	*(boffset +  6) = -ctemp07;
	*(boffset +  7) = -ctemp08;
	  
	aoffset1 +=  2 * lda;
	aoffset2 +=  2 * lda;
	boffset   += 8;
	
	i --;
      }while(i > 0);
    }
    
    if (m & 1){
      ctemp01 = *(aoffset1 +  0);
      ctemp02 = *(aoffset1 +  1);
      ctemp03 = *(aoffset1 +  2);
      ctemp04 = *(aoffset1 +  3);

      *(boffset +  0) = -ctemp01;
      *(boffset +  1) = -ctemp02;
      *(boffset +  2) = -ctemp03;
      *(boffset +  3) = -ctemp04;
      
      boffset   += 4;
    }
  }

  if (n & 2){
    aoffset1  = aoffset;
    aoffset2  = aoffset + lda;
    aoffset += 2;
    
    i = (m >> 1);
    if (i > 0){
      do{
	ctemp01 = *(aoffset1 +  0);
	ctemp02 = *(aoffset1 +  1);
	ctemp03 = *(aoffset2 +  0);
	ctemp04 = *(aoffset2 +  1);

	*(boffset +  0) = -ctemp01;
	*(boffset +  1) = -ctemp02;
	*(boffset +  2) = -ctemp03;
	*(boffset +  3) = -ctemp04;
	  
	aoffset1 +=  2 * lda;
	aoffset2 +=  2 * lda;
	boffset   += 4;
	
	i --;
      }while(i > 0);
    }
    
    if (m & 1){
      ctemp01 = *(aoffset1 +  0);
      ctemp02 = *(aoffset1 +  1);

      *(boffset +  0) = -ctemp01;
      *(boffset +  1) = -ctemp02;
      boffset   += 2;
    }
  }

  if (n & 1){
    aoffset1  = aoffset;
    aoffset2  = aoffset + lda;
    
    i = (m >> 1);
    if (i > 0){
      do{
	ctemp01 = *(aoffset1 +  0);
	ctemp02 = *(aoffset2 +  0);

	*(boffset +  0) = -ctemp01;
	*(boffset +  1) = -ctemp02;

	aoffset1 +=  2 * lda;
	aoffset2 +=  2 * lda;
	boffset   += 2;
	
	i --;
      }while(i > 0);
    }
    
    if (m & 1){
      ctemp01 = *(aoffset1 +  0);
      *(boffset +  0) = -ctemp01;
      boffset   += 1;
    }
  }

  return 0;
}
