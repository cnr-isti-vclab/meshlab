/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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

#ifndef __UTILS_WATERMARKPLUGIN_H
#define __UTILS_WATERMARKPLUGIN_H

#define RANGE_MARCHIO			 1.0		// I valori della TabellaMarchio vanno da -RANGE_MARCHIO a +RANGE_MARCHIO (distribuiti UNIFORMEMENTE)
#define RAD_TO_DEG		57.295779513082320876798154814105		// Radianti*RAD_TO_DEG = Gradi
#define DEG_TO_RAD		0.017453292519943295769236907684886		// Gradi*DEG_TO_RAD = Radianti
#define PHIEXCLUSION_DEGREE		8.0		// Esclude dalla marchiature le coordinate polari con phi troppo vicine a 180.0 o a 0.0

#include "qstring.h"
#include "math.h"
#include <vector>

class utilsWatermark
{
public:
  utilsWatermark(void){}
  ~utilsWatermark(void){}
  static unsigned int ComputeSeed(QString string_code);
  static std::vector< std::vector<double> > CreateWatermarkTable(unsigned int seed);
  static void sph2cartesian(double R, double theta, double phi, float &x, float &y, float &z);
  static void cartesian2sph(double x, double y, double z, double &R, double &theta, double &phi);
  static double round(double x);
  static double thresholdRo(double muRhoH0, double varRhoH0,
                            double muRhoH1, double varRhoH1,
                            double Pf, double &Pm);
};

#endif
