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

#ifndef __FRAME__
#define __FRAME__

#include <vcg/math/matrix44.h>
#include <vcg/math/matrix33.h>

template <class S>
struct Frame{
	S aX,aY,aZ;	 // euler angles
	vcg::Point3<S> orig; // origine del reference frame
	vcg::Matrix33<S> rot;
	S & Ang(const int & i){ return (i==0)?aX:( (i==1)? aY : aZ);}

	void angles(vcg::Matrix33<S>  mm) {
		rot = mm;
		vcg::Point3<S> x = mm.GetRow(0),y = mm.GetRow(1),z = mm.GetRow(2),xp,yp,zp;

		// rotation around   z that brings x on XZ
		float norm = vcg::math::Sqrt(x[0]*x[0]+x[1]*x[1]);
		float cosa = (norm < 0.0001)?1.0: x[0]/norm;
		aZ = vcg::math::Acos(cosa);
 		if(x[1] < 0) aZ= -aZ;

		vcg::Matrix33<S> mp;

		mp.RotateR(-aZ,vcg::Point3<S>(0,0,1));
		xp = mp * x;
		yp = mp * y;
		zp = mp * z;

		// rotation around y that brings (x su XY) su X
		norm = vcg::math::Sqrt(xp[0]*xp[0]+xp[2]*xp[2]);
		cosa = (norm < 0.0001)?1.0:xp[0]/norm;
		aY = vcg::math::Acos(cosa);
 		if(xp[2] > 0 ) aY= -aY;

		mp.RotateR(-aY,vcg::Point3<S>(0,1,0));
		xp = mp * xp;
		yp = mp * yp;
		zp = mp * zp;

		norm = vcg::math::Sqrt(yp[1]*yp[1]+yp[2]*yp[2]);
		cosa = (norm < 0.0001)?1.0:yp[1]/norm;
		aX = vcg::math::Acos(cosa);
 		if(yp[2] < 0) aX= -aX;

		mp.RotateR(-aX,vcg::Point3<S>(1,0,0));
		xp = mp * xp;
		yp = mp * yp;
		zp = mp * zp;
	}

	Frame(){}
	Frame(vcg::Matrix33<S> m, vcg::Point3<S> O){
		angles(m);
		orig = O;
	}

	void angles(vcg::Matrix44<S>  mm) {
		vcg::Matrix33<S> m;
		memcpy(&m[0][0],&mm[0][0],3*sizeof(S));
		memcpy(&m[1][0],&mm[1][0],3*sizeof(S));
		memcpy(&m[2][0],&mm[2][0],3*sizeof(S));
		angles(m);
	}

	vcg::Point3<S> Apply(vcg::Point3<S> p){

		vcg::Matrix33<S> mp; 
		mp.RotateR(aX,vcg::Point3<S>(1,0,0));
		p = mp * p;
		mp.RotateR(aY,vcg::Point3<S>(0,1,0));
		p = mp * p;
		mp.RotateR(aZ,vcg::Point3<S>(0,0,1));
		p = mp * p;

		p += orig;
		return p; 
	}

	void ToMatrix(vcg::Matrix44<S> &m){
		vcg::Matrix33<S> mp,tmp; 
		mp.RotateR(aX,vcg::Point3<S>(1,0,0));
		tmp.RotateR(aY,vcg::Point3<S>(0,1,0));
		mp = mp * tmp;
		tmp.RotateR(aZ,vcg::Point3<S>(0,0,1));
		mp = mp * tmp;
		memcpy(&m[0][0],&mp[0][0],3*sizeof(S));
		memcpy(&m[1][0],&mp[1][0],3*sizeof(S));
		memcpy(&m[2][0],&mp[2][0],3*sizeof(S));
		m[0][3] = orig[0];
		m[1][3] = orig[1];
		m[2][3] = orig[2];
	}

		Frame(vcg::Matrix44<S> m4){
		vcg::Matrix33<S> m;
		memcpy(&m[0][0],&m4[0][0],3*sizeof(S));
		memcpy(&m[1][0],&m4[1][0],3*sizeof(S));
		memcpy(&m[2][0],&m4[2][0],3*sizeof(S));

		m.Transpose();
		angles(m);
		orig[0] = m4[0][3];
		orig[1] = m4[1][3];
		orig[2] = m4[2][3];
	}

		static void BuildRotoTranslation(const vcg::Point3<S> & point, 
													const vcg::Point3<S> & norm, 
													const vcg::Point3<S> & dir, 
													const S & k, 
													const S & delta, 
													vcg::Matrix44<S> & rt){

	vcg::Matrix44<S> rt2d;
	rt2d.SetIdentity();

	// rt2d is a rototranslation around the axis parallel to z
	// and passing though (0,-1/k,0)
	rt2d[0][0] =  rt2d[1][1]  = cos(k*delta);
	rt2d[0][1] = -sin(k*delta);
	rt2d[1][0] = -rt2d[0][1];


	// taylor expansion for sinx/x and (cosx-1)/x
	 S delta2 = delta*delta;	
	 S delta3 = delta2*delta;	
	 S delta4 = delta3*delta;	
	 S delta5 = delta4*delta;	
	 S delta6 = delta5*delta;	
	 S delta7 = delta6*delta;	
	 S k2 = k*k;	
	 S k3 = k2*k;	
	 S k4 = k3*k;	
	 S k5 = k4*k;	
	 S k6 = k5*k;	

	float sinkd  = sin ( k* delta) / (-k);
	float coskd   = (1.0 - cos ( k* delta)) / (-k); -1.0 + cos (k*delta)/k;
	rt2d[0][3] = -(delta - k2 * delta3/6.0 + k4*delta5/120.0 -k6*delta7/5040);
	rt2d[1][3] = - k  * delta2/2.0 + k3*delta4/24    -k5*delta6/720; 

	// build a frame with y = norm e x = dir
	vcg::Point3<S> z = (dir ^ norm).Normalize();

	// fw (from world) brings the canonical frame on the frame (dir,norm,z, point)
	vcg::Matrix44<S> tw,fw;
	fw.SetIdentity();
	memcpy(&fw[0][0],&dir[0]	,3*sizeof(S));
	memcpy(&fw[1][0],&norm[0]	,3*sizeof(S));
	memcpy(&fw[2][0],&z[0]		,3*sizeof(S));
	fw[0][3] = - dir * point;
	fw[1][3] = - norm * point;
	fw[2][3] = - z * point;


	tw = fw;
	vcg::Transpose(tw);
	memset(&tw[3][0],0		,3*sizeof(S));
	tw[0][3] = point[0];
	tw[1][3] = point[1];
	tw[2][3] = point[2];

	 rt =  tw * rt2d * fw;// write the result
}

};

#endif