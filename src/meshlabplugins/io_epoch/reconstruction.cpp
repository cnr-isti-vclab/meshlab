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

// A camera consists of K, R, t and the radial distortion parameters.
// We have an UndoRadialDistortion member (undoraddist_) which is
// initialized with the distortion parameters.

// Furthermore we have an SVD with the P-matrix (R,t, no K). This is used
// to compute the point at infinity (direction of a line) from a point in
// the image.

// We use vnl (of VXL: www.vxl.org) for the math but any other library will
// do.

void vstRadialEuclideanCamera::RecomputeSvd()
{
  vnl_matrix_fixed<double, 3, 4> P;
  P.update(R_.transpose(), 0, 0);
  P.set_column(3, -R_.transpose() * T_);
  Psvd_ = vstSvd<double>(P);
}

// from depth and 2D point to a 3D point:

void vstRadialEuclideanCamera::DepthTo3DPoint(const vstPoint2D &m, double depth, vstPoint3D &M) const
{
    vnl_vector_fixed<double, 3> m_temp = Kinv_ * m.GetVector();

    double x, y;
    undoraddist_.ComputeOldXY(m_temp(0) / m_temp(2), m_temp(1) / m_temp(2), x, y);
    m_temp(0) = x;
    m_temp(1) = y;
    m_temp(2) = 1;

    vstPoint3D fp(T_);
    vnl_vector_fixed<double, 4> end(Psvd_.solve(m_temp));
    vstLine3D l(fp, vstPoint3D(end));
    vnl_vector_fixed<double, 4> dir(l.GetPointInfinite().GetVector());
    dir.normalize();

    M.SetVector(fp.GetVector() + depth * dir);
    if (IsBehindCamera(M))
      M.SetVector(fp.GetVector() - depth * dir);
}



// Undo Radial Distortion: This class has two maps to map undistorted to
// distorted coordinates and vice versa.

#include <vector>
#include <map>
// using namespace std;
//#include <vil/vil_warp.h>

class vstUndoRadialDistortion // : public vil_warp_mapping
{
public:
  vstUndoRadialDistortion() { }
  vstUndoRadialDistortion(const vstUndoRadialDistortion &obj):
      k_(obj.k_), ocx_(obj.ocx_), ocy_(obj.ocy_), ncx_(obj.ncx_), ncy_(obj.ncy_), lookup_(obj.lookup_), max_(obj.max_), resolution_(obj.resolution_) {}
  
  void SetParameters(std::vector<double>& k, double max = 2000, int resolution = 10000);
  
  std::vector<double> GetParameters() { return k_; };
  
  void ComputeNewXY(double xo, double yo, double& xn, double& yn) const;
  void ComputeOldXY(double xn, double yn, double& xo, double& yo) const;
  
  // for vil_warp, which doesn't work
  void forward_map(double x1, double y1, double* x2, double* y2) const;
  void inverse_map(double x2, double y2, double* x1, double* y1) const;
  
protected:
  std::vector<double> k_;
  double ocx_;
  double ocy_;
  double ncx_;
  double ncy_;
  
  void SetupLookupTable(double max, int resolution);
  
  std::map<double, double>  lookup_;
  
  double max_;
  int resolution_;
};



#include <cmath>

using namespace std;


void vstUndoRadialDistortion::SetParameters(vector<double>& k, double max, int resolution)
{ 
   k_ = k;
   max_ = max;
   resolution_ = resolution;

   SetupLookupTable(max_, resolution_);
}

void vstUndoRadialDistortion::forward_map(double x1, double y1, double* x2, double* y2) const
{
   ComputeNewXY(x1-ocx_, y1-ocy_, *x2, *y2);
   *x2 += ncx_;
   *y2 += ncy_;
}


void vstUndoRadialDistortion::inverse_map(double x2, double y2, double* x1, double* y1) const
{
   ComputeOldXY(x2-ncx_, y2-ncy_, *x1, *y1);
   *x1 += ocx_;
   *y1 += ocy_;
}

//----------------------------------------------------------------------------
// Relative w.r.t. center !
void vstUndoRadialDistortion::ComputeNewXY(double xo, double yo, double& xn, double& yn) const
{
   double r = (xo*xo) + (yo*yo);  // r is r squared
   double f = 1.0;
   for (int i = 0; i < k_.size(); i++) f += (k_[i] * pow(r, i+1));
   xn = f*xo;
   yn = f*yo;
}


void vstUndoRadialDistortion::ComputeOldXY(double xn, double yn, double& xo, double& yo) const
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


void vstUndoRadialDistortion::SetupLookupTable(double max, int resolution)
{
   lookup_.clear();
   double incr = max/(double)resolution;
   double value = 0.0;
   double old=-1.0;
   while (value < max)
   {
      double f = 1.0;
      for (int i = 0; i < k_.size(); i++) 
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

