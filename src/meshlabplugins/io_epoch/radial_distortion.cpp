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

#include "radial_distortion.h"
#include <cmath>

using namespace std;


void RadialDistortion::SetParameters(vector<double>& k, double max, int resolution)
{ 
   k_ = k;
   max_ = max;
   resolution_ = resolution;

   SetupLookupTable(max_, resolution_);
}

void RadialDistortion::forward_map(double x1, double y1, double* x2, double* y2) const
{
   ComputeNewXY(x1-ocx_, y1-ocy_, *x2, *y2);
   *x2 += ncx_;
   *y2 += ncy_;
}


void RadialDistortion::inverse_map(double x2, double y2, double* x1, double* y1) const
{
   ComputeOldXY(x2-ncx_, y2-ncy_, *x1, *y1);
   *x1 += ocx_;
   *y1 += ocy_;
}

//----------------------------------------------------------------------------
// Relative w.r.t. center !
void RadialDistortion::ComputeNewXY(double xo, double yo, double& xn, double& yn) const
{
   double r = (xo*xo) + (yo*yo);  // r is r squared
   double f = 1.0;
   for (int i = 0; i < static_cast<int>(k_.size()); i++) f += (k_[i] * pow(r, i+1));
   xn = f*xo;
   yn = f*yo;
}


void RadialDistortion::ComputeOldXY(double xn, double yn, double& xo, double& yo) const
{
   double rn = sqrt((xn*xn) + (yn*yn));  //// r is r squared
   map<double, double>::const_iterator next = lookup_.upper_bound(rn);
   map<double, double>::const_iterator prev = next;  prev--;

   // The compiler will optimise this.   
   double a = (*prev).first;
   double b = (*next).first;
   double c = rn;
   double d = (*prev).second;
   double e = (*next).second;
   double f = ((e-d)/(b-a))*(c-a) + d;
   
   xo = f*xn;
   yo = f*yn;
}


void RadialDistortion::SetupLookupTable(double max, int resolution)
{
   lookup_.clear();
   double incr = max/(double)resolution;
   double value = 0.0;
   double old=-1.0;
   while (value < max)
   {
      double f = 1.0;
      for (int i = 0; i < static_cast<int>(k_.size()); i++) 
         f += (k_[i] * pow(value*value, i+1));
      if ((f*value)>old) 
      {
	      //cout << "R = " << value << " ,Rd = " << f*value << " ,f = " << f << endl; 
	      lookup_[f*value] = 1.0/f;
	      old=f*value;
	      value += incr;
      }
      else {
	  //vstDebug(100)<<"vstUndoRadialDistortion::SetupLookupTable:warning RADIAL DISTORTION FOLDING BACK at "<<value<<endl;
	break;
      }
   }
}

