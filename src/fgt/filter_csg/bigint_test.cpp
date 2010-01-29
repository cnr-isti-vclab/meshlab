/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2010                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *   
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

/*
Bigint test tool
This program creates random bigints and operates on them.
The same operations are also performed using native 64-bits integers, to check
the correctness of the bigint computation.
If the output is correct, piping it through "uniq -u" (only showing unique lines)
should produce no output, otherwise the correct computation and the incorrect one
should be shown in this order
*/

#include <iostream>
#include "bigint.h"

using namespace vcg::math;
using namespace std;

#define tbits 8
#define mysub(x,y) (x<y? y-x : x-y)
#define log(a,b,c)     cout << c << '\t' << a << '\t' << b << '\t' << a + b << '\t' << (a < b) << '\t' << mysub(a,b) << '\t' << a * b << '\t' << (a >> c) << '\t' << (b << c) << endl

int main()
{
  uint8_t r[8];

  cout << hex;

  srand(time(NULL));
  for (int tests = 0; tests < 10000; ++tests) {
    for (int i = 0; i < 8; ++i)
      r[i] = rand();

    size_t sa = ((rand() % 7) + 1);
    size_t myshift = rand() % (sa * 8);
    bigint8 a = r[0];
    uint64_t a64 = r[0];

    for (size_t i = 1; i < sa; ++i) {
      a = (a << tbits) + r[i];
      a64 = (a64 << tbits) + r[i];
    }

    
    bigint8 b = r[sa];
    uint64_t b64 = r[sa];
    for (size_t i = sa + 1; i < 8; ++i) {
      b = (b << tbits) + r[i];
      b64 = (b64 << tbits) + r[i];
    }
    log(a64,b64,myshift);
    log(a,b,myshift);
  }

  return 0;
}
