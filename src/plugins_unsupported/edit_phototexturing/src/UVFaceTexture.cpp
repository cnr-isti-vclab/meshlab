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
#include <QtGui>
#include <vcg/math/matrix44.h>

#include <src/UVFaceTexture.h>

#define EPSILON 1e-10

void UVFaceTexture::getBarycentricCoordsForUV(double eu,double ev,double& a,double& b,double& c,double& d){
	//qDebug()<<"eu: " << eu << "ev: "<< ev ;
	//qDebug() << "u[0]: " << u[0] << "u[1]: " << u[1] << "u[2]: " << u[2]<< "v[0]: " << v[0]<< "v[1]: " << v[1]<< "v[2]: " << v[2];


	/*original*/
	double A;
	double B;
	double C;
	double D;
	double E;
	double F;

	A = u[0]-u[2];
	B = u[1]-u[2];
	C = u[2]-eu;
	D = v[0]-v[2];
	E = v[1]-v[2];
	F = v[2]-ev;


	a=(B*F-C*E)/(A*E-B*D);
	b=(A*F-C*D)/(B*D-A*E);
	c=1.0-a-b;


	if ((a>=-EPSILON && a<= (1.0+EPSILON)) &&(b>=-EPSILON && b<=(1.0+EPSILON)) &&(c>=-EPSILON && c<=(1.0+EPSILON))){
		d=0.0;
	}else{
		d=1.0;
	}

	//qDebug()<< "d: "<<d<<"\tA: "<<A<< "\tB: "<<B<< "\tC: "<<C<< "\tD: "<<D<< "\tE: "<<E<< "\tF: "<<F<< "\ta: "<<a<< "\tb: "<<b<< "\tc: "<<c;

}

void UVFaceTexture::getUVatBarycentricCoords(double &eu,double &ev,double a,double b,double c){
	eu = a*u[0]+ b*u[1]+c*u[2];
	ev = a*v[0]+ b*v[1]+c*v[2];
}


bool UVFaceTexture::intersection(double *s,double *b){
	double tmp_s[2];
	double tmp_b[2];
/*
	tmp_s[0] = u[0];
	tmp_s[1] = v[0];
	tmp_b[0] = u[1]-u[0];
	tmp_b[1] = v[1]-v[0];

	if(intersectionOfTwoLines(s,b,tmp_s,tmp_b)){
		return true;
	}
	tmp_s[0] = u[1];
	tmp_s[1] = v[1];
	tmp_b[0] = u[2]-u[1];
	tmp_b[1] = v[2]-v[1];
	if(intersectionOfTwoLines(s,b,tmp_s,tmp_b)){
		return true;
	}
	tmp_s[0] = u[2];
	tmp_s[1] = v[2];
	tmp_b[0] = u[0]-u[2];
	tmp_b[1] = v[0]-v[2];
	if(intersectionOfTwoLines(s,b,tmp_s,tmp_b)){
		return true;
	}
*/

	tmp_s[0] = u[0];
	tmp_s[1] = v[0];
	tmp_b[0] = u[1];
	tmp_b[1] = v[1];

	if(intersectionOfTwoLines(s,b,tmp_s,tmp_b)){
		return true;
	}
	tmp_s[0] = u[1];
	tmp_s[1] = v[1];
	tmp_b[0] = u[2];
	tmp_b[1] = v[2];
	if(intersectionOfTwoLines(s,b,tmp_s,tmp_b)){
		return true;
	}
	tmp_s[0] = u[2];
	tmp_s[1] = v[2];
	tmp_b[0] = u[0];
	tmp_b[1] = v[0];
	if(intersectionOfTwoLines(s,b,tmp_s,tmp_b)){
		return true;
	}
	return false;

}

bool UVFaceTexture::intersectionOfTwoLines(double *p1,double *p2,double *p3, double *p4){

	double ua= (((p4[0]-p3[0])*(p1[1]-p3[1]))-((p4[1]-p3[1])*(p1[0]-p3[0])))/(((p4[1]-p3[1])*(p2[0]-p1[0]))-((p4[0]-p3[0])*(p2[1]-p1[1])));
	double ub= (((p2[0]-p1[0])*(p1[1]-p3[1]))-((p2[1]-p1[1])*(p1[0]-p3[0])))/(((p4[1]-p3[1])*(p2[0]-p1[0]))-((p4[0]-p3[0])*(p2[1]-p1[1])));

	if((ua>= -EPSILON && ua<= 1.0+EPSILON)&&(ub>= -EPSILON && ub<= 1.0+EPSILON)){
		return true;
	}
	return false;
}


bool UVFaceTexture::isInside(double x, double y, double w, double h){
	//qDebug()<<"isInside:" << x << y << w <<h;
	int i;
	for(i=0;i<3;i++){
		if ((u[i]>=x && u[i]<=x+w)&& (v[i]>=y && v[i]<=y+h)){
			return true;
		}
	}
	
	if(isInside(x,y)){
		return true;
	}
	if(isInside(x+w,y)){
		return true;
	}
	if(isInside(x+w,y+h)){
		return true;
	}
	if(isInside(x,y+h)){
		return true;
	}
	
	
	double tmp_s[2];
	double tmp_b[2];

	tmp_s[0]=x;
	tmp_s[1]=y;
	tmp_b[0]=x+w;
	tmp_b[1]=y;

	if (intersection(tmp_s,tmp_b)){
		return true;
	}
	tmp_s[0]=x+w;
	tmp_s[1]=y;
	tmp_b[0]=x+w;
	tmp_b[1]=y+h;

	if (intersection(tmp_s,tmp_b)){
		return true;
	}
	tmp_s[0]=x+w;
	tmp_s[1]=y+h;
	tmp_b[0]=x;
	tmp_b[1]=y+h;

	if (intersection(tmp_s,tmp_b)){
		return true;
	}
	tmp_s[0]=x;
	tmp_s[1]=y+h;
	tmp_b[0]=x;
	tmp_b[1]=y;

	if (intersection(tmp_s,tmp_b)){
		return true;
	}

	return false;


}
bool UVFaceTexture::isInside(double x, double y){
	double a,b,c,d;
	getBarycentricCoordsForUV(x,y,a,b,c,d);
	if (d==0.0){
		return true;
	}
	return false;
}
