/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

#include <common/meshmodel.h>

namespace vcg
{
class Arc3DCamera
{
public:
  Matrix33d K;      // parametri intriseci camera
  Matrix33d Kundist;      // parametri intriseci camera non distorta
  Matrix33d Kinv; 
  Matrix33d Kinvundist;

  std::vector<double> k;
  Matrix33d R;
  Matrix44d TR; // [R | -Rt] e.g. la matrice in cui
  Matrix44d TRinv;
  Point3d t;
  int width, height;

  RadialDistortion rd;

  Point3m DepthTo3DPoint(double x, double y, double depth, Point3d &M) const;

  bool Open(const char * filename);
};

}