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

#include <vector>
#include <vcg/math/matrix33.h>
#include <vcg/math/matrix44.h>
#include <vcg/space/point3.h>

using namespace vcg;
using namespace std;

#include "radial_distortion.h"
#include "epoch_camera.h"


/*
Epoch Camera


*/
// This function take in input a point in image space (e.g. with coords in range [0..1024]x[0..768]
// a depth value and it returns the point in absolute 3D coords
// 
void EpochCamera::DepthTo3DPoint(double x, double y, double depth, Point3d &M) const
{
    Point3d m_temp = Kinv * Point3d(x,y,1);
    
    double oldx, oldy;
    rd.ComputeOldXY(m_temp[0] / m_temp[2], m_temp[1] / m_temp[2], oldx, oldy);
    
    m_temp=Point3d(oldx,oldy,1);
    Point3d fp=t;    
    Point3d end = TRinv*m_temp;    
    Point3d dir =fp-end;
    dir.Normalize();
    M = fp-dir*depth; 
}




bool EpochCamera::Open(const char *filename)
{
  FILE *fp=fopen(filename,"rb");
  if(!fp) return false;

  fscanf(fp,"%lf %lf %lf",&(K[0][0]),&(K[0][1]),&(K[0][2]));
  fscanf(fp,"%lf %lf %lf",&(K[1][0]),&(K[1][1]),&(K[1][2]));
  fscanf(fp,"%lf %lf %lf",&(K[2][0]),&(K[2][1]),&(K[2][2]));

  k.resize(3);
  fscanf(fp,"%lf %lf %lf",&(k[0]),&(k[1]),&(k[2]));

  fscanf(fp,"%lf %lf %lf",&(R[0][0]),&(R[0][1]),&(R[0][2]));
  fscanf(fp,"%lf %lf %lf",&(R[1][0]),&(R[1][1]),&(R[1][2]));
  fscanf(fp,"%lf %lf %lf",&(R[2][0]),&(R[2][1]),&(R[2][2]));

  fscanf(fp,"%lf %lf %lf",&(t[0]),&(t[1]),&(t[2]));
  
  fscanf(fp,"%i %i",&width,&height);

  fclose(fp);
  Kinv=Inverse(K);
  
  rd.SetParameters(k);

  // TR = [R | -Rt] 4x4 matrix with upperleft a 3x3 rotation
  // on the right the rotated translation -Rt and 0001 in the 
  // lower line.

  R.transposeInPlace();
  #ifndef VCG_USE_EIGEN
  for(int i=0;i<3;++i)
    for(int j=0;j<3;++j)
          TR[i][j]= R[i][j];
	#else
	TR.corner<3,3>(Eigen::TopLeft) = R.transpose();
	#endif

  Point3d rt= R*(-t);
  
  for(int i=0;i<3;++i)
    TR[i][3]=rt[i];

  for(int j=0;j<3;++j)
    TR[3][j]=0;

  TR[3][3]=1;
  TRinv=Inverse(TR);
  return true;
}
