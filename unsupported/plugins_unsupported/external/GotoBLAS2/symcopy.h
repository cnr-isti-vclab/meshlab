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

/* This implementation is completely wrong. I'll rewrite this */

#ifndef SYMCOPY_H
#define SYMCOPY_H

#if !defined(XDOUBLE) || !defined(QUAD_PRECISION)

static inline void SYMCOPY_L(BLASLONG m, FLOAT *a, BLASLONG lda, FLOAT *b){
  BLASLONG is, js;

  FLOAT *aa1, *aa2;
  FLOAT *b1, *b2;
  FLOAT *bb1, *bb2;
  FLOAT *cc1, *cc2;
  FLOAT a11, a12;
  FLOAT a21, a22;

  b1 = b;
  b2 = b;

  for (js = 0; js < m; js += 2){

    aa1 = a + 0 * lda;
    aa2 = a + 1 * lda;
    a  += 2 * lda + 2;
    
    bb1 = b1 + 0 * m;
    bb2 = b1 + 1 * m;
    b1 += 2 * m + 2;
	  
    cc1 = b2 + 0 * m;
    cc2 = b2 + 1 * m;
    b2 += 2 * m + 2;

    if (m - js >= 2){

      a11 = *(aa1 + 0);
      a21 = *(aa1 + 1);
      
      a22 = *(aa2 + 1);
      
      *(bb1 + 0) = a11;
      *(bb1 + 1) = a21;
      *(bb2 + 0) = a21;
      *(bb2 + 1) = a22;
      aa1 += 2;
      aa2 += 2;
      bb1 += 2;
      bb2 += 2;
      
      cc1 += 2 * m;
      cc2 += 2 * m;

      is = ((m - js - 2) >> 1);

      while (is > 0){
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	a12 = *(aa2 + 0);
	a22 = *(aa2 + 1);
	
	aa1 += 2;
	aa2 += 2;
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = a21;
	*(bb2 + 0) = a12;
	*(bb2 + 1) = a22;

	*(cc1 + 0) = a11;
	*(cc1 + 1) = a12;
	*(cc2 + 0) = a21;
	*(cc2 + 1) = a22;

	bb1 += 2;
	bb2 += 2;

	cc1 += 2 * m;
	cc2 += 2 * m;

	is --;
      }

      is = ((m - js - 2) & 1);

      if (is == 1){
	a11 = *(aa1 + 0);
	a12 = *(aa2 + 0);
	
	*(bb1 + 0) = a11;
	*(bb2 + 0) = a12;

	*(cc1 + 0) = a11;
	*(cc1 + 1) = a12;
      }
    }
    
    if (m - js == 1){
      a11 = *(aa1 + 0);
      *(bb1 + 0) = a11;
    }

  }
}

static inline void SYMCOPY_U(BLASLONG m, FLOAT *a, BLASLONG lda, FLOAT *b){
  BLASLONG is, js;

  FLOAT *aa1, *aa2;
  FLOAT *b1, *b2;
  FLOAT *bb1, *bb2;
  FLOAT *cc1, *cc2;
  FLOAT a11, a12;
  FLOAT a21, a22;

  b1 = b;
  b2 = b;

  for (js = 0; js < m; js += 2){

    aa1 = a + 0 * lda;
    aa2 = a + 1 * lda;
    a  += 2 * lda;
    
    bb1 = b1 + 0 * m;
    bb2 = b1 + 1 * m;
    b1 += 2 * m;
	  
    cc1 = b2 + 0 * m;
    cc2 = b2 + 1 * m;
    b2 += 2;

    if (m - js >= 2){

      for (is = 0; is < js; is += 2){
      
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	a12 = *(aa2 + 0);
	a22 = *(aa2 + 1);

	aa1 += 2;
	aa2 += 2;
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = a21;
	*(bb2 + 0) = a12;
	*(bb2 + 1) = a22;
	
	*(cc1 + 0) = a11;
	*(cc1 + 1) = a12;
	*(cc2 + 0) = a21;
	*(cc2 + 1) = a22;
	
	bb1 += 2;
	bb2 += 2;
	
	cc1 += 2 * m;
	cc2 += 2 * m;
      }

      a11 = *(aa1 + 0);
      
      a12 = *(aa2 + 0);
      a22 = *(aa2 + 1);
      
      *(bb1 + 0) = a11;
      *(bb1 + 1) = a12;
      *(bb2 + 0) = a12;
      *(bb2 + 1) = a22;
    }
    
    if (m - js == 1){
      for (is = 0; is < js; is += 2){
      
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	aa1 += 2;
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = a21;
	*(cc1 + 0) = a11;
	*(cc2 + 0) = a21;
	bb1 += 2;
	
	cc1 += 2 * m;
	cc2 += 2 * m;
      }

      a11 = *(aa1 + 0);
      *(bb1 + 0) = a11;
    }
  }
}


static inline void ZSYMCOPY_L(BLASLONG m, FLOAT *a, BLASLONG lda, FLOAT *b){
  BLASLONG is, js;

  FLOAT *aa1, *aa2;
  FLOAT *b1, *b2;
  FLOAT *bb1, *bb2;
  FLOAT *cc1, *cc2;
  FLOAT a11, a21, a31, a41;
  FLOAT a12, a22, a32, a42;

  b1 = b;
  b2 = b;

  lda *= 2;

  for (js = 0; js < m; js += 2){

    aa1 = a + 0 * lda;
    aa2 = a + 1 * lda;
    a  += 2 * lda + 4;
    
    bb1 = b1 + 0 * m;
    bb2 = b1 + 2 * m;
    b1 += 4 * m + 4;
	  
    cc1 = b2 + 0 * m;
    cc2 = b2 + 2 * m;
    b2 += 4 * m + 4;

    if (m - js >= 2){

      a11 = *(aa1 + 0);
      a21 = *(aa1 + 1);
      a31 = *(aa1 + 2);
      a41 = *(aa1 + 3);
  
      a12 = *(aa2 + 2);
      a22 = *(aa2 + 3);
      
      *(bb1 + 0) = a11;
      *(bb1 + 1) = a21;
      *(bb1 + 2) = a31;
      *(bb1 + 3) = a41;

      *(bb2 + 0) = a31;
      *(bb2 + 1) = a41;
      *(bb2 + 2) = a12;
      *(bb2 + 3) = a22;

      aa1 += 4;
      aa2 += 4;
      bb1 += 4;
      bb2 += 4;
      
      cc1 += 4 * m;
      cc2 += 4 * m;

      is = ((m - js - 2) >> 1);

      while (is > 0){
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	a31 = *(aa1 + 2);
	a41 = *(aa1 + 3);

	a12 = *(aa2 + 0);
	a22 = *(aa2 + 1);
	a32 = *(aa2 + 2);
	a42 = *(aa2 + 3);
	
	aa1 += 4;
	aa2 += 4;
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = a21;
	*(bb1 + 2) = a31;
	*(bb1 + 3) = a41;

	*(bb2 + 0) = a12;
	*(bb2 + 1) = a22;
	*(bb2 + 2) = a32;
	*(bb2 + 3) = a42;

	*(cc1 + 0) = a11;
	*(cc1 + 1) = a21;
	*(cc1 + 2) = a12;
	*(cc1 + 3) = a22;

	*(cc2 + 0) = a31;
	*(cc2 + 1) = a41;
	*(cc2 + 2) = a32;
	*(cc2 + 3) = a42;

	bb1 += 4;
	bb2 += 4;

	cc1 += 4 * m;
	cc2 += 4 * m;

	is --;
      }

      if (m & 1){
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	a12 = *(aa2 + 0);
	a22 = *(aa2 + 1);
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = a21;
	*(bb2 + 0) = a12;
	*(bb2 + 1) = a22;

	*(cc1 + 0) = a11;
	*(cc1 + 1) = a21;
	*(cc1 + 2) = a12;
	*(cc1 + 3) = a22;
      }
    }
    
    if (m - js == 1){
      a11 = *(aa1 + 0);
      a21 = *(aa1 + 1);
      *(bb1 + 0) = a11;
      *(bb1 + 1) = a21;
    }

  }
}

static inline void ZSYMCOPY_U(BLASLONG m, FLOAT *a, BLASLONG lda, FLOAT *b){
  BLASLONG is, js;

  FLOAT *aa1, *aa2;
  FLOAT *b1, *b2;
  FLOAT *bb1, *bb2;
  FLOAT *cc1, *cc2;
  FLOAT a11, a21, a31, a41;
  FLOAT a12, a22, a32, a42;

  b1 = b;
  b2 = b;

  lda *= 2;

  for (js = 0; js < m; js += 2){

    aa1 = a + 0 * lda;
    aa2 = a + 1 * lda;
    a  += 2 * lda;
    
    bb1 = b1 + 0 * m;
    bb2 = b1 + 2 * m;
    b1 += 4 * m;
	  
    cc1 = b2 + 0 * m;
    cc2 = b2 + 2 * m;
    b2 += 4;

    if (m - js >= 2){

      for (is = 0; is < js; is += 2){
      
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	a31 = *(aa1 + 2);
	a41 = *(aa1 + 3);

	a12 = *(aa2 + 0);
	a22 = *(aa2 + 1);
	a32 = *(aa2 + 2);
	a42 = *(aa2 + 3);

	aa1 += 4;
	aa2 += 4;
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = a21;
	*(bb1 + 2) = a31;
	*(bb1 + 3) = a41;

	*(bb2 + 0) = a12;
	*(bb2 + 1) = a22;
	*(bb2 + 2) = a32;
	*(bb2 + 3) = a42;
	
	*(cc1 + 0) = a11;
	*(cc1 + 1) = a21;
	*(cc1 + 2) = a12;
	*(cc1 + 3) = a22;

	*(cc2 + 0) = a31;
	*(cc2 + 1) = a41;
	*(cc2 + 2) = a32;
	*(cc2 + 3) = a42;
	
	bb1 += 4;
	bb2 += 4;
	
	cc1 += 4 * m;
	cc2 += 4 * m;
      }

      a11 = *(aa1 + 0);
      a21 = *(aa1 + 1);
      
      a12 = *(aa2 + 0);
      a22 = *(aa2 + 1);
      a32 = *(aa2 + 2);
      a42 = *(aa2 + 3);
      
      *(bb1 + 0) = a11;
      *(bb1 + 1) = a21;
      *(bb1 + 2) = a12;
      *(bb1 + 3) = a22;

      *(bb2 + 0) = a12;
      *(bb2 + 1) = a22;
      *(bb2 + 2) = a32;
      *(bb2 + 3) = a42;
    }
    
    if (m - js == 1){
      for (is = 0; is < js; is += 2){
      
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	a31 = *(aa1 + 2);
	a41 = *(aa1 + 3);
	aa1 += 4;
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = a21;
	*(bb1 + 2) = a31;
	*(bb1 + 3) = a41;

	*(cc1 + 0) = a11;
	*(cc1 + 1) = a21;
	*(cc2 + 0) = a31;
	*(cc2 + 1) = a41;
	bb1 += 4;
	
	cc1 += 4 * m;
	cc2 += 4 * m;
      }

      a11 = *(aa1 + 0);
      a21 = *(aa1 + 1);
      *(bb1 + 0) = a11;
      *(bb1 + 1) = a21;
    }
  }
}

static inline void ZHEMCOPY_L(BLASLONG m, FLOAT *a, BLASLONG lda, FLOAT *b){
  BLASLONG is, js;

  FLOAT *aa1, *aa2;
  FLOAT *b1, *b2;
  FLOAT *bb1, *bb2;
  FLOAT *cc1, *cc2;
  FLOAT a11, a21, a31, a41;
  FLOAT a12, a22, a32, a42;

  b1 = b;
  b2 = b;

  lda *= 2;

  for (js = 0; js < m; js += 2){

    aa1 = a + 0 * lda;
    aa2 = a + 1 * lda;
    a  += 2 * lda + 4;
    
    bb1 = b1 + 0 * m;
    bb2 = b1 + 2 * m;
    b1 += 4 * m + 4;
	  
    cc1 = b2 + 0 * m;
    cc2 = b2 + 2 * m;
    b2 += 4 * m + 4;

    if (m - js >= 2){

      a11 = *(aa1 + 0);
      a31 = *(aa1 + 2);
      a41 = *(aa1 + 3);
  
      a12 = *(aa2 + 2);
      
      *(bb1 + 0) = a11;
      *(bb1 + 1) = 0.;
      *(bb1 + 2) = a31;
      *(bb1 + 3) = a41;

      *(bb2 + 0) = a31;
      *(bb2 + 1) = -a41;
      *(bb2 + 2) = a12;
      *(bb2 + 3) = 0.;

      aa1 += 4;
      aa2 += 4;
      bb1 += 4;
      bb2 += 4;
      
      cc1 += 4 * m;
      cc2 += 4 * m;

      is = ((m - js - 2) >> 1);

      while (is > 0){
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	a31 = *(aa1 + 2);
	a41 = *(aa1 + 3);

	a12 = *(aa2 + 0);
	a22 = *(aa2 + 1);
	a32 = *(aa2 + 2);
	a42 = *(aa2 + 3);
	
	aa1 += 4;
	aa2 += 4;
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = a21;
	*(bb1 + 2) = a31;
	*(bb1 + 3) = a41;

	*(bb2 + 0) = a12;
	*(bb2 + 1) = a22;
	*(bb2 + 2) = a32;
	*(bb2 + 3) = a42;

	*(cc1 + 0) = a11;
	*(cc1 + 1) = -a21;
	*(cc1 + 2) = a12;
	*(cc1 + 3) = -a22;

	*(cc2 + 0) = a31;
	*(cc2 + 1) = -a41;
	*(cc2 + 2) = a32;
	*(cc2 + 3) = -a42;

	bb1 += 4;
	bb2 += 4;

	cc1 += 4 * m;
	cc2 += 4 * m;

	is --;
      }

      if (m & 1){
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	a12 = *(aa2 + 0);
	a22 = *(aa2 + 1);
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = a21;
	*(bb2 + 0) = a12;
	*(bb2 + 1) = a22;

	*(cc1 + 0) = a11;
	*(cc1 + 1) = -a21;
	*(cc1 + 2) = a12;
	*(cc1 + 3) = -a22;
      }
    }
    
    if (m - js == 1){
      a11 = *(aa1 + 0);
      *(bb1 + 0) = a11;
      *(bb1 + 1) = 0.;
    }

  }
}

static inline void ZHEMCOPY_U(BLASLONG m, FLOAT *a, BLASLONG lda, FLOAT *b){
  BLASLONG is, js;

  FLOAT *aa1, *aa2;
  FLOAT *b1, *b2;
  FLOAT *bb1, *bb2;
  FLOAT *cc1, *cc2;
  FLOAT a11, a21, a31, a41;
  FLOAT a12, a22, a32, a42;

  b1 = b;
  b2 = b;

  lda *= 2;

  for (js = 0; js < m; js += 2){

    aa1 = a + 0 * lda;
    aa2 = a + 1 * lda;
    a  += 2 * lda;
    
    bb1 = b1 + 0 * m;
    bb2 = b1 + 2 * m;
    b1 += 4 * m;
	  
    cc1 = b2 + 0 * m;
    cc2 = b2 + 2 * m;
    b2 += 4;

    if (m - js >= 2){

      for (is = 0; is < js; is += 2){
      
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	a31 = *(aa1 + 2);
	a41 = *(aa1 + 3);

	a12 = *(aa2 + 0);
	a22 = *(aa2 + 1);
	a32 = *(aa2 + 2);
	a42 = *(aa2 + 3);

	aa1 += 4;
	aa2 += 4;
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = a21;
	*(bb1 + 2) = a31;
	*(bb1 + 3) = a41;

	*(bb2 + 0) = a12;
	*(bb2 + 1) = a22;
	*(bb2 + 2) = a32;
	*(bb2 + 3) = a42;
	
	*(cc1 + 0) = a11;
	*(cc1 + 1) = -a21;
	*(cc1 + 2) = a12;
	*(cc1 + 3) = -a22;

	*(cc2 + 0) = a31;
	*(cc2 + 1) = -a41;
	*(cc2 + 2) = a32;
	*(cc2 + 3) = -a42;
	
	bb1 += 4;
	bb2 += 4;
	
	cc1 += 4 * m;
	cc2 += 4 * m;
      }

      a11 = *(aa1 + 0);
      
      a12 = *(aa2 + 0);
      a22 = *(aa2 + 1);
      a32 = *(aa2 + 2);
      
      *(bb1 + 0) = a11;
      *(bb1 + 1) = 0.;
      *(bb1 + 2) = a12;
      *(bb1 + 3) = -a22;

      *(bb2 + 0) = a12;
      *(bb2 + 1) = a22;
      *(bb2 + 2) = a32;
      *(bb2 + 3) = 0.;
    }
    
    if (m - js == 1){
      for (is = 0; is < js; is += 2){
      
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	a31 = *(aa1 + 2);
	a41 = *(aa1 + 3);
	aa1 += 4;
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = a21;
	*(bb1 + 2) = a31;
	*(bb1 + 3) = a41;

	*(cc1 + 0) = a11;
	*(cc1 + 1) = -a21;
	*(cc2 + 0) = a31;
	*(cc2 + 1) = -a41;
	bb1 += 4;
	
	cc1 += 4 * m;
	cc2 += 4 * m;
      }

      a11 = *(aa1 + 0);
      *(bb1 + 0) = a11;
      *(bb1 + 1) = 0.;
    }
  }
}


static inline void ZHEMCOPY_M(BLASLONG m, FLOAT *a, BLASLONG lda, FLOAT *b){
  BLASLONG is, js;

  FLOAT *aa1, *aa2;
  FLOAT *b1, *b2;
  FLOAT *bb1, *bb2;
  FLOAT *cc1, *cc2;
  FLOAT a11, a21, a31, a41;
  FLOAT a12, a22, a32, a42;

  b1 = b;
  b2 = b;

  lda *= 2;

  for (js = 0; js < m; js += 2){

    aa1 = a + 0 * lda;
    aa2 = a + 1 * lda;
    a  += 2 * lda + 4;
    
    bb1 = b1 + 0 * m;
    bb2 = b1 + 2 * m;
    b1 += 4 * m + 4;
	  
    cc1 = b2 + 0 * m;
    cc2 = b2 + 2 * m;
    b2 += 4 * m + 4;

    if (m - js >= 2){

      a11 = *(aa1 + 0);
      a31 = *(aa1 + 2);
      a41 = *(aa1 + 3);
  
      a12 = *(aa2 + 2);
      
      *(bb1 + 0) = a11;
      *(bb1 + 1) = 0.;
      *(bb1 + 2) = a31;
      *(bb1 + 3) = -a41;

      *(bb2 + 0) = a31;
      *(bb2 + 1) = a41;
      *(bb2 + 2) = a12;
      *(bb2 + 3) = 0.;

      aa1 += 4;
      aa2 += 4;
      bb1 += 4;
      bb2 += 4;
      
      cc1 += 4 * m;
      cc2 += 4 * m;

      is = ((m - js - 2) >> 1);

      while (is > 0){
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	a31 = *(aa1 + 2);
	a41 = *(aa1 + 3);

	a12 = *(aa2 + 0);
	a22 = *(aa2 + 1);
	a32 = *(aa2 + 2);
	a42 = *(aa2 + 3);
	
	aa1 += 4;
	aa2 += 4;
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = -a21;
	*(bb1 + 2) = a31;
	*(bb1 + 3) = -a41;

	*(bb2 + 0) = a12;
	*(bb2 + 1) = -a22;
	*(bb2 + 2) = a32;
	*(bb2 + 3) = -a42;

	*(cc1 + 0) = a11;
	*(cc1 + 1) = a21;
	*(cc1 + 2) = a12;
	*(cc1 + 3) = a22;

	*(cc2 + 0) = a31;
	*(cc2 + 1) = a41;
	*(cc2 + 2) = a32;
	*(cc2 + 3) = a42;

	bb1 += 4;
	bb2 += 4;

	cc1 += 4 * m;
	cc2 += 4 * m;

	is --;
      }

      if (m & 1){
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	a12 = *(aa2 + 0);
	a22 = *(aa2 + 1);
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = -a21;
	*(bb2 + 0) = a12;
	*(bb2 + 1) = -a22;

	*(cc1 + 0) = a11;
	*(cc1 + 1) = a21;
	*(cc1 + 2) = a12;
	*(cc1 + 3) = a22;
      }
    }
    
    if (m - js == 1){
      a11 = *(aa1 + 0);
      *(bb1 + 0) = a11;
      *(bb1 + 1) = 0.;
    }

  }
}

static inline void ZHEMCOPY_V(BLASLONG m, FLOAT *a, BLASLONG lda, FLOAT *b){
  BLASLONG is, js;

  FLOAT *aa1, *aa2;
  FLOAT *b1, *b2;
  FLOAT *bb1, *bb2;
  FLOAT *cc1, *cc2;
  FLOAT a11, a21, a31, a41;
  FLOAT a12, a22, a32, a42;

  b1 = b;
  b2 = b;

  lda *= 2;

  for (js = 0; js < m; js += 2){

    aa1 = a + 0 * lda;
    aa2 = a + 1 * lda;
    a  += 2 * lda;
    
    bb1 = b1 + 0 * m;
    bb2 = b1 + 2 * m;
    b1 += 4 * m;
	  
    cc1 = b2 + 0 * m;
    cc2 = b2 + 2 * m;
    b2 += 4;

    if (m - js >= 2){

      for (is = 0; is < js; is += 2){
      
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	a31 = *(aa1 + 2);
	a41 = *(aa1 + 3);

	a12 = *(aa2 + 0);
	a22 = *(aa2 + 1);
	a32 = *(aa2 + 2);
	a42 = *(aa2 + 3);

	aa1 += 4;
	aa2 += 4;
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = -a21;
	*(bb1 + 2) = a31;
	*(bb1 + 3) = -a41;

	*(bb2 + 0) = a12;
	*(bb2 + 1) = -a22;
	*(bb2 + 2) = a32;
	*(bb2 + 3) = -a42;
	
	*(cc1 + 0) = a11;
	*(cc1 + 1) = a21;
	*(cc1 + 2) = a12;
	*(cc1 + 3) = a22;

	*(cc2 + 0) = a31;
	*(cc2 + 1) = a41;
	*(cc2 + 2) = a32;
	*(cc2 + 3) = a42;
	
	bb1 += 4;
	bb2 += 4;
	
	cc1 += 4 * m;
	cc2 += 4 * m;
      }

      a11 = *(aa1 + 0);
      
      a12 = *(aa2 + 0);
      a22 = *(aa2 + 1);
      a32 = *(aa2 + 2);
      
      *(bb1 + 0) = a11;
      *(bb1 + 1) = 0.;
      *(bb1 + 2) = a12;
      *(bb1 + 3) = a22;

      *(bb2 + 0) = a12;
      *(bb2 + 1) = -a22;
      *(bb2 + 2) = a32;
      *(bb2 + 3) = 0.;
    }
    
    if (m - js == 1){
      for (is = 0; is < js; is += 2){
      
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	a31 = *(aa1 + 2);
	a41 = *(aa1 + 3);
	aa1 += 4;
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = -a21;
	*(bb1 + 2) = a31;
	*(bb1 + 3) = -a41;

	*(cc1 + 0) = a11;
	*(cc1 + 1) = a21;
	*(cc2 + 0) = a31;
	*(cc2 + 1) = a41;
	bb1 += 4;
	
	cc1 += 4 * m;
	cc2 += 4 * m;
      }

      a11 = *(aa1 + 0);
      *(bb1 + 0) = a11;
      *(bb1 + 1) = 0.;
    }
  }
}


static inline void TRMCOPY_NL(BLASLONG m, FLOAT *a, BLASLONG lda, FLOAT *b){
  BLASLONG is, js;

  FLOAT *aa1, *aa2;
  FLOAT *b1, *b2;
  FLOAT *bb1, *bb2;
  FLOAT *cc1, *cc2;
  FLOAT a11, a12;
  FLOAT a21, a22;

  b1 = b;
  b2 = b;

  for (js = 0; js < m; js += 2){

    aa1 = a + 0 * lda;
    aa2 = a + 1 * lda;
    a  += 2 * lda + 2;
    
    bb1 = b1 + 0 * m;
    bb2 = b1 + 1 * m;
    b1 += 2 * m + 2;
	  
    cc1 = b2 + 0 * m;
    cc2 = b2 + 1 * m;
    b2 += 2 * m + 2;

    if (m - js >= 2){

      a11 = *(aa1 + 0);
      a21 = *(aa1 + 1);
      
      a22 = *(aa2 + 1);
      
      *(bb1 + 0) = a11;
      *(bb1 + 1) = a21;
      *(bb2 + 0) = a21;
      *(bb2 + 1) = a22;
      aa1 += 2;
      aa2 += 2;
      bb1 += 2;
      bb2 += 2;
      
      cc1 += 2 * m;
      cc2 += 2 * m;

      is = ((m - js - 2) >> 1);

      while (is > 0){
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	a12 = *(aa2 + 0);
	a22 = *(aa2 + 1);
	
	aa1 += 2;
	aa2 += 2;
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = a21;
	*(bb2 + 0) = a12;
	*(bb2 + 1) = a22;

	*(cc1 + 0) = a11;
	*(cc1 + 1) = a12;
	*(cc2 + 0) = a21;
	*(cc2 + 1) = a22;

	bb1 += 2;
	bb2 += 2;

	cc1 += 2 * m;
	cc2 += 2 * m;

	is --;
      }

      is = ((m - js - 2) & 1);

      if (is == 1){
	a11 = *(aa1 + 0);
	a12 = *(aa2 + 0);
	
	*(bb1 + 0) = a11;
	*(bb2 + 0) = a12;

	*(cc1 + 0) = a11;
	*(cc1 + 1) = a12;
      }
    }
    
    if (m - js == 1){
      a11 = *(aa1 + 0);
      *(bb1 + 0) = a11;
    }

  }
}

static inline void TRMCOPY_TL(BLASLONG m, FLOAT *a, BLASLONG lda, FLOAT *b){
  BLASLONG is, js;

  FLOAT *aa1, *aa2;
  FLOAT *b1, *b2;
  FLOAT *bb1, *bb2;
  FLOAT *cc1, *cc2;
  FLOAT a11, a12;
  FLOAT a21, a22;

  b1 = b;
  b2 = b;

  for (js = 0; js < m; js += 2){

    aa1 = a + 0 * lda;
    aa2 = a + 1 * lda;
    a  += 2 * lda + 2;
    
    bb1 = b1 + 0 * m;
    bb2 = b1 + 1 * m;
    b1 += 2 * m + 2;
	  
    cc1 = b2 + 0 * m;
    cc2 = b2 + 1 * m;
    b2 += 2 * m + 2;

    if (m - js >= 2){

      a11 = *(aa1 + 0);
      a21 = *(aa1 + 1);
      
      a22 = *(aa2 + 1);
      
      *(bb1 + 0) = a11;
      *(bb1 + 1) = a21;
      *(bb2 + 0) = a21;
      *(bb2 + 1) = a22;
      aa1 += 2;
      aa2 += 2;
      bb1 += 2;
      bb2 += 2;
      
      cc1 += 2 * m;
      cc2 += 2 * m;

      is = ((m - js - 2) >> 1);

      while (is > 0){
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	a12 = *(aa2 + 0);
	a22 = *(aa2 + 1);
	
	aa1 += 2;
	aa2 += 2;
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = a21;
	*(bb2 + 0) = a12;
	*(bb2 + 1) = a22;

	*(cc1 + 0) = a11;
	*(cc1 + 1) = a12;
	*(cc2 + 0) = a21;
	*(cc2 + 1) = a22;

	bb1 += 2;
	bb2 += 2;

	cc1 += 2 * m;
	cc2 += 2 * m;

	is --;
      }

      is = ((m - js - 2) & 1);

      if (is == 1){
	a11 = *(aa1 + 0);
	a12 = *(aa2 + 0);
	
	*(bb1 + 0) = a11;
	*(bb2 + 0) = a12;

	*(cc1 + 0) = a11;
	*(cc1 + 1) = a12;
      }
    }
    
    if (m - js == 1){
      a11 = *(aa1 + 0);
      *(bb1 + 0) = a11;
    }

  }
}

static inline void TRMCOPY_NU(BLASLONG m, FLOAT *a, BLASLONG lda, FLOAT *b){
  BLASLONG is, js;

  FLOAT *aa1, *aa2;
  FLOAT *b1, *b2;
  FLOAT *bb1, *bb2;
  FLOAT *cc1, *cc2;
  FLOAT a11, a12;
  FLOAT a21, a22;

  b1 = b;
  b2 = b;

  for (js = 0; js < m; js += 2){

    aa1 = a + 0 * lda;
    aa2 = a + 1 * lda;
    a  += 2 * lda;
    
    bb1 = b1 + 0 * m;
    bb2 = b1 + 1 * m;
    b1 += 2 * m;
	  
    cc1 = b2 + 0 * m;
    cc2 = b2 + 1 * m;
    b2 += 2;

    if (m - js >= 2){

      for (is = 0; is < js; is += 2){
      
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	a12 = *(aa2 + 0);
	a22 = *(aa2 + 1);

	aa1 += 2;
	aa2 += 2;
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = a21;
	*(bb2 + 0) = a12;
	*(bb2 + 1) = a22;
	
	*(cc1 + 0) = a11;
	*(cc1 + 1) = a12;
	*(cc2 + 0) = a21;
	*(cc2 + 1) = a22;
	
	bb1 += 2;
	bb2 += 2;
	
	cc1 += 2 * m;
	cc2 += 2 * m;
      }

      a11 = *(aa1 + 0);
      
      a12 = *(aa2 + 0);
      a22 = *(aa2 + 1);
      
      *(bb1 + 0) = a11;
      *(bb1 + 1) = a12;
      *(bb2 + 0) = a12;
      *(bb2 + 1) = a22;
    }
    
    if (m - js == 1){
      for (is = 0; is < js; is += 2){
      
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	aa1 += 2;
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = a21;
	*(cc1 + 0) = a11;
	*(cc2 + 0) = a21;
	bb1 += 2;
	
	cc1 += 2 * m;
	cc2 += 2 * m;
      }

      a11 = *(aa1 + 0);
      *(bb1 + 0) = a11;
    }
  }
}

static inline void TRMCOPY_TU(BLASLONG m, FLOAT *a, BLASLONG lda, FLOAT *b){
  BLASLONG is, js;

  FLOAT *aa1, *aa2;
  FLOAT *b1, *b2;
  FLOAT *bb1, *bb2;
  FLOAT *cc1, *cc2;
  FLOAT a11, a12;
  FLOAT a21, a22;

  b1 = b;
  b2 = b;

  for (js = 0; js < m; js += 2){

    aa1 = a + 0 * lda;
    aa2 = a + 1 * lda;
    a  += 2 * lda;
    
    bb1 = b1 + 0 * m;
    bb2 = b1 + 1 * m;
    b1 += 2 * m;
	  
    cc1 = b2 + 0 * m;
    cc2 = b2 + 1 * m;
    b2 += 2;

    if (m - js >= 2){

      for (is = 0; is < js; is += 2){
      
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	a12 = *(aa2 + 0);
	a22 = *(aa2 + 1);

	aa1 += 2;
	aa2 += 2;
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = a21;
	*(bb2 + 0) = a12;
	*(bb2 + 1) = a22;
	
	*(cc1 + 0) = a11;
	*(cc1 + 1) = a12;
	*(cc2 + 0) = a21;
	*(cc2 + 1) = a22;
	
	bb1 += 2;
	bb2 += 2;
	
	cc1 += 2 * m;
	cc2 += 2 * m;
      }

      a11 = *(aa1 + 0);
      
      a12 = *(aa2 + 0);
      a22 = *(aa2 + 1);
      
      *(bb1 + 0) = a11;
      *(bb1 + 1) = a12;
      *(bb2 + 0) = a12;
      *(bb2 + 1) = a22;
    }
    
    if (m - js == 1){
      for (is = 0; is < js; is += 2){
      
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	aa1 += 2;
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = a21;
	*(cc1 + 0) = a11;
	*(cc2 + 0) = a21;
	bb1 += 2;
	
	cc1 += 2 * m;
	cc2 += 2 * m;
      }

      a11 = *(aa1 + 0);
      *(bb1 + 0) = a11;
    }
  }
}

static inline void ZTRMCOPY_NL(BLASLONG m, FLOAT *a, BLASLONG lda, FLOAT *b){
  BLASLONG is, js;

  FLOAT *aa1, *aa2;
  FLOAT *b1, *b2;
  FLOAT *bb1, *bb2;
  FLOAT *cc1, *cc2;
  FLOAT a11, a21, a31, a41;
  FLOAT a12, a22, a32, a42;

  b1 = b;
  b2 = b;

  lda *= 2;

  for (js = 0; js < m; js += 2){

    aa1 = a + 0 * lda;
    aa2 = a + 1 * lda;
    a  += 2 * lda + 4;
    
    bb1 = b1 + 0 * m;
    bb2 = b1 + 2 * m;
    b1 += 4 * m + 4;
	  
    cc1 = b2 + 0 * m;
    cc2 = b2 + 2 * m;
    b2 += 4 * m + 4;

    if (m - js >= 2){

      a11 = *(aa1 + 0);
      a21 = *(aa1 + 1);
      a31 = *(aa1 + 2);
      a41 = *(aa1 + 3);
  
      a12 = *(aa2 + 2);
      a22 = *(aa2 + 3);
      
      *(bb1 + 0) = a11;
      *(bb1 + 1) = a21;
      *(bb1 + 2) = a31;
      *(bb1 + 3) = a41;

      *(bb2 + 0) = a31;
      *(bb2 + 1) = a41;
      *(bb2 + 2) = a12;
      *(bb2 + 3) = a22;

      aa1 += 4;
      aa2 += 4;
      bb1 += 4;
      bb2 += 4;
      
      cc1 += 4 * m;
      cc2 += 4 * m;

      is = ((m - js - 2) >> 1);

      while (is > 0){
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	a31 = *(aa1 + 2);
	a41 = *(aa1 + 3);

	a12 = *(aa2 + 0);
	a22 = *(aa2 + 1);
	a32 = *(aa2 + 2);
	a42 = *(aa2 + 3);
	
	aa1 += 4;
	aa2 += 4;
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = a21;
	*(bb1 + 2) = a31;
	*(bb1 + 3) = a41;

	*(bb2 + 0) = a12;
	*(bb2 + 1) = a22;
	*(bb2 + 2) = a32;
	*(bb2 + 3) = a42;

	*(cc1 + 0) = a11;
	*(cc1 + 1) = a21;
	*(cc1 + 2) = a12;
	*(cc1 + 3) = a22;

	*(cc2 + 0) = a31;
	*(cc2 + 1) = a41;
	*(cc2 + 2) = a32;
	*(cc2 + 3) = a42;

	bb1 += 4;
	bb2 += 4;

	cc1 += 4 * m;
	cc2 += 4 * m;

	is --;
      }

      if (m & 1){
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	a12 = *(aa2 + 0);
	a22 = *(aa2 + 1);
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = a21;
	*(bb2 + 0) = a12;
	*(bb2 + 1) = a22;

	*(cc1 + 0) = a11;
	*(cc1 + 1) = a21;
	*(cc1 + 2) = a12;
	*(cc1 + 3) = a22;
      }
    }
    
    if (m - js == 1){
      a11 = *(aa1 + 0);
      a21 = *(aa1 + 1);
      *(bb1 + 0) = a11;
      *(bb1 + 1) = a21;
    }

  }
}

static inline void ZTRMCOPY_TL(BLASLONG m, FLOAT *a, BLASLONG lda, FLOAT *b){
  BLASLONG is, js;

  FLOAT *aa1, *aa2;
  FLOAT *b1, *b2;
  FLOAT *bb1, *bb2;
  FLOAT *cc1, *cc2;
  FLOAT a11, a21, a31, a41;
  FLOAT a12, a22, a32, a42;

  b1 = b;
  b2 = b;

  lda *= 2;

  for (js = 0; js < m; js += 2){

    aa1 = a + 0 * lda;
    aa2 = a + 1 * lda;
    a  += 2 * lda + 4;
    
    bb1 = b1 + 0 * m;
    bb2 = b1 + 2 * m;
    b1 += 4 * m + 4;
	  
    cc1 = b2 + 0 * m;
    cc2 = b2 + 2 * m;
    b2 += 4 * m + 4;

    if (m - js >= 2){

      a11 = *(aa1 + 0);
      a21 = *(aa1 + 1);
      a31 = *(aa1 + 2);
      a41 = *(aa1 + 3);
  
      a12 = *(aa2 + 2);
      a22 = *(aa2 + 3);
      
      *(bb1 + 0) = a11;
      *(bb1 + 1) = a21;
      *(bb1 + 2) = a31;
      *(bb1 + 3) = a41;

      *(bb2 + 0) = a31;
      *(bb2 + 1) = a41;
      *(bb2 + 2) = a12;
      *(bb2 + 3) = a22;

      aa1 += 4;
      aa2 += 4;
      bb1 += 4;
      bb2 += 4;
      
      cc1 += 4 * m;
      cc2 += 4 * m;

      is = ((m - js - 2) >> 1);

      while (is > 0){
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	a31 = *(aa1 + 2);
	a41 = *(aa1 + 3);

	a12 = *(aa2 + 0);
	a22 = *(aa2 + 1);
	a32 = *(aa2 + 2);
	a42 = *(aa2 + 3);
	
	aa1 += 4;
	aa2 += 4;
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = a21;
	*(bb1 + 2) = a31;
	*(bb1 + 3) = a41;

	*(bb2 + 0) = a12;
	*(bb2 + 1) = a22;
	*(bb2 + 2) = a32;
	*(bb2 + 3) = a42;

	*(cc1 + 0) = a11;
	*(cc1 + 1) = a21;
	*(cc1 + 2) = a12;
	*(cc1 + 3) = a22;

	*(cc2 + 0) = a31;
	*(cc2 + 1) = a41;
	*(cc2 + 2) = a32;
	*(cc2 + 3) = a42;

	bb1 += 4;
	bb2 += 4;

	cc1 += 4 * m;
	cc2 += 4 * m;

	is --;
      }

      if (m & 1){
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	a12 = *(aa2 + 0);
	a22 = *(aa2 + 1);
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = a21;
	*(bb2 + 0) = a12;
	*(bb2 + 1) = a22;

	*(cc1 + 0) = a11;
	*(cc1 + 1) = a21;
	*(cc1 + 2) = a12;
	*(cc1 + 3) = a22;
      }
    }
    
    if (m - js == 1){
      a11 = *(aa1 + 0);
      a21 = *(aa1 + 1);
      *(bb1 + 0) = a11;
      *(bb1 + 1) = a21;
    }

  }
}

static inline void ZTRMCOPY_NU(BLASLONG m, FLOAT *a, BLASLONG lda, FLOAT *b){
  BLASLONG is, js;

  FLOAT *aa1, *aa2;
  FLOAT *b1, *b2;
  FLOAT *bb1, *bb2;
  FLOAT *cc1, *cc2;
  FLOAT a11, a21, a31, a41;
  FLOAT a12, a22, a32, a42;

  b1 = b;
  b2 = b;

  lda *= 2;

  for (js = 0; js < m; js += 2){

    aa1 = a + 0 * lda;
    aa2 = a + 1 * lda;
    a  += 2 * lda;
    
    bb1 = b1 + 0 * m;
    bb2 = b1 + 2 * m;
    b1 += 4 * m;
	  
    cc1 = b2 + 0 * m;
    cc2 = b2 + 2 * m;
    b2 += 4;

    if (m - js >= 2){

      for (is = 0; is < js; is += 2){
      
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	a31 = *(aa1 + 2);
	a41 = *(aa1 + 3);

	a12 = *(aa2 + 0);
	a22 = *(aa2 + 1);
	a32 = *(aa2 + 2);
	a42 = *(aa2 + 3);

	aa1 += 4;
	aa2 += 4;
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = a21;
	*(bb1 + 2) = a31;
	*(bb1 + 3) = a41;

	*(bb2 + 0) = a12;
	*(bb2 + 1) = a22;
	*(bb2 + 2) = a32;
	*(bb2 + 3) = a42;
	
	*(cc1 + 0) = a11;
	*(cc1 + 1) = a21;
	*(cc1 + 2) = a12;
	*(cc1 + 3) = a22;

	*(cc2 + 0) = a31;
	*(cc2 + 1) = a41;
	*(cc2 + 2) = a32;
	*(cc2 + 3) = a42;
	
	bb1 += 4;
	bb2 += 4;
	
	cc1 += 4 * m;
	cc2 += 4 * m;
      }

      a11 = *(aa1 + 0);
      a21 = *(aa1 + 1);
      
      a12 = *(aa2 + 0);
      a22 = *(aa2 + 1);
      a32 = *(aa2 + 2);
      a42 = *(aa2 + 3);
      
      *(bb1 + 0) = a11;
      *(bb1 + 1) = a21;
      *(bb1 + 2) = a12;
      *(bb1 + 3) = a22;

      *(bb2 + 0) = a12;
      *(bb2 + 1) = a22;
      *(bb2 + 2) = a32;
      *(bb2 + 3) = a42;
    }
    
    if (m - js == 1){
      for (is = 0; is < js; is += 2){
      
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	a31 = *(aa1 + 2);
	a41 = *(aa1 + 3);
	aa1 += 4;
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = a21;
	*(bb1 + 2) = a31;
	*(bb1 + 3) = a41;

	*(cc1 + 0) = a11;
	*(cc1 + 1) = a21;
	*(cc2 + 0) = a31;
	*(cc2 + 1) = a41;
	bb1 += 4;
	
	cc1 += 4 * m;
	cc2 += 4 * m;
      }

      a11 = *(aa1 + 0);
      a21 = *(aa1 + 1);
      *(bb1 + 0) = a11;
      *(bb1 + 1) = a21;
    }
  }
}

static inline void ZTRMCOPY_TU(BLASLONG m, FLOAT *a, BLASLONG lda, FLOAT *b){
  BLASLONG is, js;

  FLOAT *aa1, *aa2;
  FLOAT *b1, *b2;
  FLOAT *bb1, *bb2;
  FLOAT *cc1, *cc2;
  FLOAT a11, a21, a31, a41;
  FLOAT a12, a22, a32, a42;

  b1 = b;
  b2 = b;

  lda *= 2;

  for (js = 0; js < m; js += 2){

    aa1 = a + 0 * lda;
    aa2 = a + 1 * lda;
    a  += 2 * lda;
    
    bb1 = b1 + 0 * m;
    bb2 = b1 + 2 * m;
    b1 += 4 * m;
	  
    cc1 = b2 + 0 * m;
    cc2 = b2 + 2 * m;
    b2 += 4;

    if (m - js >= 2){

      for (is = 0; is < js; is += 2){
      
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	a31 = *(aa1 + 2);
	a41 = *(aa1 + 3);

	a12 = *(aa2 + 0);
	a22 = *(aa2 + 1);
	a32 = *(aa2 + 2);
	a42 = *(aa2 + 3);

	aa1 += 4;
	aa2 += 4;
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = a21;
	*(bb1 + 2) = a31;
	*(bb1 + 3) = a41;

	*(bb2 + 0) = a12;
	*(bb2 + 1) = a22;
	*(bb2 + 2) = a32;
	*(bb2 + 3) = a42;
	
	*(cc1 + 0) = a11;
	*(cc1 + 1) = a21;
	*(cc1 + 2) = a12;
	*(cc1 + 3) = a22;

	*(cc2 + 0) = a31;
	*(cc2 + 1) = a41;
	*(cc2 + 2) = a32;
	*(cc2 + 3) = a42;
	
	bb1 += 4;
	bb2 += 4;
	
	cc1 += 4 * m;
	cc2 += 4 * m;
      }

      a11 = *(aa1 + 0);
      a21 = *(aa1 + 1);
      
      a12 = *(aa2 + 0);
      a22 = *(aa2 + 1);
      a32 = *(aa2 + 2);
      a42 = *(aa2 + 3);
      
      *(bb1 + 0) = a11;
      *(bb1 + 1) = a21;
      *(bb1 + 2) = a12;
      *(bb1 + 3) = a22;

      *(bb2 + 0) = a12;
      *(bb2 + 1) = a22;
      *(bb2 + 2) = a32;
      *(bb2 + 3) = a42;
    }
    
    if (m - js == 1){
      for (is = 0; is < js; is += 2){
      
	a11 = *(aa1 + 0);
	a21 = *(aa1 + 1);
	a31 = *(aa1 + 2);
	a41 = *(aa1 + 3);
	aa1 += 4;
	
	*(bb1 + 0) = a11;
	*(bb1 + 1) = a21;
	*(bb1 + 2) = a31;
	*(bb1 + 3) = a41;

	*(cc1 + 0) = a11;
	*(cc1 + 1) = a21;
	*(cc2 + 0) = a31;
	*(cc2 + 1) = a41;
	bb1 += 4;
	
	cc1 += 4 * m;
	cc2 += 4 * m;
      }

      a11 = *(aa1 + 0);
      a21 = *(aa1 + 1);
      *(bb1 + 0) = a11;
      *(bb1 + 1) = a21;
    }
  }
}

#endif
#endif

