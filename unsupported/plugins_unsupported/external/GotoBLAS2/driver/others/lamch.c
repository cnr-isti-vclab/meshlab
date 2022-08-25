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

#include "common.h"

#if 0
static FLOAT hdata[] __attribute__((aligned(128))) = {
#ifdef XDOUBLE
  +0x1.0000000000000000P-00064L,
  +0x1.0000000000000000P-16382L,
  +0x1.0000000000000000P+00001L,
  +0x1.0000000000000000P-00063L,
  +0x1.0000000000000000P+00006L,
  +0x1.0000000000000000P+00000L,
  -0x1.ffe8000000000000P+00013L,
  +0x1.0000000000000000P-16382L,
  +0x1.0004000000000000P+00014L,
  +0x1.fffffffffffffffeP+16383L,
#elif defined DOUBLE
  +0x1.0000000000000P-0053,
  +0x1.0000000000000P-1022,
  +0x1.0000000000000P+0001,
  +0x1.0000000000000P-0052,
  +0x1.a800000000000P+0005,
  +0x1.0000000000000P+0000,
  -0x1.fe80000000000P+0009,
  +0x1.0000000000000P-1022,
  +0x1.0000000000000P+0010,
  +0x1.fffffffffffffP+1023,
#else
  +0x1.000000P-024f,
  +0x1.000000P-126f,
  +0x1.000000P+001f,
  +0x1.000000P-023f,
  +0x1.800000P+004f,
  +0x1.000000P+000f,
  -0x1.f40000P+006f,
  +0x1.000000P-126f,
  +0x1.000000P+007f,
  +0x1.fffffeP+127f,
#endif
};

#endif

static unsigned int idata[] __attribute__((aligned(128))) = {

#if   defined XDOUBLE
#ifndef __BIG_ENDIAN__
  0x00000000, 0x80000000, 0x00003fbf, 0x00000000,
  0x00000000, 0x80000000, 0x00000001, 0x00000000,
  0x00000000, 0x80000000, 0x00004000, 0x00000000,
  0x00000000, 0x80000000, 0x00003fc0, 0x00000000,
  0x00000000, 0x80000000, 0x00004005, 0x00000000,
  0x00000000, 0x80000000, 0x00003fff, 0x00000000,
  0x00000000, 0xff400000, 0x0000c00c, 0x00000000,
  0x00000000, 0x80000000, 0x00000001, 0x00000000,
  0x00000000, 0x80200000, 0x0000400d, 0x00000000,
  0xffffffff, 0xffffffff, 0x00007ffe, 0x00000000,
#else
  0x00000000, 0x00003fbf, 0x80000000, 0x00000000,
  0x00000000, 0x00000001, 0x80000000, 0x00000000,
  0x00000000, 0x00004000, 0x80000000, 0x00000000,
  0x00000000, 0x00003fc0, 0x80000000, 0x00000000,
  0x00000000, 0x00004005, 0x80000000, 0x00000000,
  0x00000000, 0x00003fff, 0x80000000, 0x00000000,
  0x00000000, 0x0000c00c, 0xff400000, 0x00000000,
  0x00000000, 0x00000001, 0x80000000, 0x00000000,
  0x00000000, 0x0000400d, 0x80200000, 0x00000000,
  0x00000000, 0x00007ffe, 0xffffffff, 0xffffffff,

#endif
#elif defined DOUBLE
#ifndef __BIG_ENDIAN__
  0x00000000, 0x3ca00000,
  0x00000000, 0x00100000,
  0x00000000, 0x40000000,
  0x00000000, 0x3cb00000,
  0x00000000, 0x404a8000,
  0x00000000, 0x3ff00000,
  0x00000000, 0xc08fe800,
  0x00000000, 0x00100000,
  0x00000000, 0x40900000,
  0xffffffff, 0x7fefffff,
#else
  0x3ca00000, 0x00000000,
  0x00100000, 0x00000000,
  0x40000000, 0x00000000,
  0x3cb00000, 0x00000000,
  0x404a8000, 0x00000000,
  0x3ff00000, 0x00000000,
  0xc08fe800, 0x00000000,
  0x00100000, 0x00000000,
  0x40900000, 0x00000000,
  0x7fefffff, 0xffffffff,
#endif
#else

  0x33800000,
  0x00800000,
  0x40000000,
  0x34000000,
  0x41c00000,
  0x3f800000,
  0xc2fa0000,
  0x00800000,
  0x43000000,
  0x7f7fffff,

#endif
};


#ifdef NEED_F2CCONV
double
#else
FLOAT
#endif
NAME(char *P){
  
  char p = *P;
  int pos;
  FLOAT *hdata = (FLOAT *)idata;

  TOUPPER(p);

  switch (p) {
  case 'E':
    pos = 0;
    break;
  case 'S':
    pos = 1;
    break;
  case 'B':
    pos = 2;
    break;
  case 'P':
    pos = 3;
    break;
  case 'N':
    pos = 4;
    break;
  case 'R':
    pos = 5;
    break;
  case 'M':
    pos = 6;
    break;
  case 'U':
    pos = 7;
    break;
  case 'L':
    pos = 8;
    break;
  case 'O':
    pos = 9;
    break;
  default:
    pos = 0;
    break;
  }

 return hdata[pos];

}
