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
Fixed test tool
A few examples of fixed point math.
*/

#include "fixed.h"

#include <iostream>

using namespace std;
using namespace vcg::math;

typedef vcg::math::fixed<8,vcg::math::base32> fixed_base;

int main()
{
  fixed_base t;
  fixed_base a = 3.5;
  fixed_base b = 1.5;
  vcg::math::fixed<16> c = a * a;
  vcg::math::fixed<16> d = a * b;
  cout << ceil(a) << endl;
  cout << floor(a) << endl;
  cout << floor(c) << endl;
  cout << ceil(c) << endl;
  cout << (c>d) << endl;
  return 0;
}
