/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2007                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software: you can redistribute it and/or modify      *
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
#ifndef DIRT_UTILS_H
#define DIRT_UTILS_H

//Include Files
#include <QDir>
#include <QPainter>
#include <QColor>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <limits>
#include <common/meshmodel.h>
#include <common/interfaces.h>
#include<vector>
#include<vcg/simplex/vertex/base.h>
#include<vcg/space/index/base.h>
#include<vcg/simplex/face/base.h>
#include<vcg/complex/complex.h>
#include <vcg/space/point3.h>
#include <vcg/space/intersection2.h>
#include <vcg/complex/allocate.h>
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/closest.h>
#include <vcg/simplex/face/distance.h>
#include <vcg/complex/algorithms/geodesic.h>
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/update/flag.h>
#include <vcg/complex/algorithms/update/normal.h>
#include <vcg/complex/algorithms/point_sampling.h>
#include <vcg/complex/algorithms/create/resampler.h>
#include <vcg/complex/algorithms/clustering.h>
#include <vcg/simplex/face/distance.h>
#include <vcg/space/distance3.h>
#include <vcg/space/index/grid_static_ptr.h>
#include <vcg/space/intersection3.h>
#include "particle.h"

using namespace vcg;
using namespace tri;


typedef GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType > MetroMeshFaceGrid;
typedef GridStaticPtr<CMeshO::VertexType, CMeshO::ScalarType > MetroMeshVertexGrid;
typedef FaceTmark<CMeshO> MarkerFace;

#define PI 3.14159265
#define EPSILON 0.0001


bool PFDist(CMeshO::FacePointer face,CMeshO::CoordType &p,float dist,Point3f &closest){

	Point3f clos[3];
	float distv[3];
	Point3f clos_proj;
	float distproj;

	vcg::Triangle3<float> t(face->P(0),face->P(1),face->P(2));

	///find distance on the plane
	vcg::Plane3<float> plane;
	plane.Init(t.P(0),t.P(1),t.P(2));
	clos_proj=plane.Projection(p);

	///control if inside/outside
	Point3f n=(t.P(1)-t.P(0))^(t.P(2)-t.P(0));
	Point3f n0=(t.P(0)-clos_proj)^(t.P(1)-clos_proj);
	Point3f n1=(t.P(1)-clos_proj)^(t.P(2)-clos_proj);
	Point3f n2=(t.P(2)-clos_proj)^(t.P(0)-clos_proj);
	distproj=(clos_proj-p).Norm();
	if (((n*n0)>=0)&&((n*n1)>=0)&&((n*n2)>=0))
	{
		closest=clos_proj;
		dist=distproj;
		return true;
	}
	
	return false;





};

/**
@def Generate random barycentric coordinates

@return a triple of barycentric coordinates
*/
CMeshO::CoordType RandomBaricentric(){

	CMeshO::CoordType interp;
	static math::MarsenneTwisterRNG rnd;
	interp[1] = rnd.generate01();
	interp[2] = rnd.generate01();

	if(interp[1] + interp[2] > 1.0){
		interp[1] = 1.0 - interp[1];
		interp[2] = 1.0 - interp[2];
	}

	assert(interp[1] + interp[2] <= 1.0);
	interp[0]=1.0-(interp[1] + interp[2]);
	return interp;
};
/**
@def This funcion calculate the cartesian coordinates of a point given from its barycentric coordinates

@param Point3f bc       - barycentric coordinates of the point
@param FacePointer f    - pointer to the face

@return cartesian coordinates of the point
*/



CMeshO::CoordType fromBarCoords(Point3f bc,CMeshO::FacePointer f){

	CMeshO::CoordType p;
	Point3f p0=f->P(0);
	Point3f p1=f->P(1);
	Point3f p2=f->P(2);
	p=f->P(0)*bc[0]+f->P(1)*bc[1]+f->P(2)*bc[2];
	return p;
};

CMeshO::CoordType getBaricenter(CMeshO::FacePointer f){
	Point3f bc;
	bc[0]=0.33f;
	bc[1]=0.33f;
	bc[2]=1-bc[0]-bc[1];
	CMeshO::CoordType pc=fromBarCoords(bc,f);
	return pc;
};

/**
@def Given a Face and the index of an edge this function return a random point in the triangle
defined by the two points of the edge e and the center of the face.
@param FacePointer f - a pointer to the face
@param int         e - index of an edge

@return a point in the face f near the edge e
*/

CMeshO::CoordType GetSafePosition(CMeshO::CoordType p,CMeshO::FacePointer f){
	CMeshO::CoordType safe_p;
	Point3f bc;
	bc[0]=0.33f;
	bc[1]=0.33f;
	bc[2]=1-bc[0]-bc[1];
	CMeshO::CoordType pc=fromBarCoords(bc,f);
	Ray3<float> ray=Ray3<float>(p,pc);
	ray.Normalize();
	Line3f line;
	Point3f p1=pc-p;
	safe_p=p+p1*0.05;
	return safe_p;
};


/**
@def Verify if a point lies on a face

@param Point3f p   - Coordinates of the point
@param FacePointer f - Pointer to the face

@return true if point p is on face f, false elsewhere.
*/
bool IsOnFace(Point3f p, CMeshO::FacePointer f){
	//Compute vectors
	Point3f a=f->V(0)->P();
	Point3f b=f->V(2)->P();
	Point3f c=f->V(1)->P();


	Point3f v0 = c-a;
	Point3f v1 = b-a;
	Point3f v2 = p-a;

	// Compute dot products
	float dot00 = v0.dot(v0);
	float dot01 = v0.dot(v1);
	float dot02 = v0.dot(v2);
	float dot11 = v1.dot(v1);
	float dot12 = v1.dot(v2);

	// Compute barycentric coordinates
	float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// Check if point is in triangle
	if(math::Abs(u)<0) u=0;
	if(math::Abs(v)<0) v=0;
	return (u >= 0) && (v >= 0) && (u + v <=1);
};


float GetRemainingTime(float t,Point3f pi,Point3f pm,Point3f pf){
	float time;

	float d1=Distance(pi,pm);
	float d2=Distance(pi,pf);
	float d=d2-d1;

	if(d>0) time=t-t*d/d2;

	else return 0;
};


//To delete?
CMeshO::CoordType ComputeVelocity(CMeshO::CoordType vi,CMeshO::CoordType ac,float t){

	CMeshO::CoordType n_vel;

	n_vel[0]=vi[0]+ac[0]*t;
	n_vel[1]=vi[1]+ac[1]*t;
	n_vel[2]=vi[2]+ac[2]*t;

	return n_vel;
};
//To delete?
float UpdateVelocity(CMeshO::CoordType pi,CMeshO::CoordType pf,CMeshO::CoordType v,float m,CMeshO::FacePointer &face,CMeshO::CoordType force){

	CMeshO::CoordType new_vel;
	float new_v;
	Point3f n= face->cN();
	float a=n[0]*force[0]+n[1]*force[1]+n[2]*force[2];

	Point3f f;

	f[0]=force[0]-a*n[0];
	f[1]=force[1]-a*n[1];
	f[2]=force[2]-a*n[2];



	new_vel[0]=sqrt(pow(v[0],2)+2*(f[0]/m)*(pf[0]-pi[0]) );
	new_vel[1]=sqrt(pow(v[1],2)+2*(f[1]/m)*(pf[1]-pi[1]) );
	new_vel[2]=sqrt(pow(v[2],2)+2*(f[2]/m)*(pf[2]-pi[2]) );

	new_v=sqrt(pow(new_vel[0],2)+pow(new_vel[1],2)+pow(new_vel[2],2));

	return new_v;
};



/**
@def
*/
Point3f GetVelocityComponents(float v,CMeshO::FacePointer face){

	//xy
	//yz
	//zx

	Point3f n=face->cN();
	n.Normalize();
	Point3f acc;

	float g=1;
	//XY

	Point3f vel;

	Point3f axis_x=Point3f(1,0,0);
	Point3f axis_y=Point3f(0,1,0);
	Point3f axis_z=Point3f(0,0,1);

	Point3f nx=Point3f(n[0],0,0);
	Point3f ny=Point3f(0,n[1],0);
	Point3f nz=Point3f(0,0,n[2]);
	float alpha=90-(acos(axis_x.dot(nx))*(180/PI));
	float beta =90-(acos(axis_y.dot(ny))*(180/PI));
	float gamma=90-(acos(axis_z.dot(nz))*(180/PI));

	vel[0]=v*cos(alpha*PI/180);
	vel[1]=v*cos(beta*PI/180);
	vel[2]=v*cos(gamma*PI/180);

	acc[0]=g*sin(alpha*PI/180);
	acc[1]=g*sin(beta*PI/180);
	acc[2]=g*sin(gamma*PI/180);

	return vel;




};

/**
@def Simulate the movement of a point, affected by a force "dir" on a face.

@param CoordType p   - coordinates of the point
@param CoordType v   - velocity of the particle
@param float     m   - mass of the particle
@param FaceType face - pointer to the face
@param CoordType dir - direction of the force
@param float 	   l   - length of the movement
@param float     t   - time step

@return new coordinates of the point
*/
CMeshO::CoordType StepForward(CMeshO::CoordType p,float v,float m,CMeshO::FacePointer &face,CMeshO::CoordType dir,float l,float t=1){
	Point3f new_pos;
	Point3f n= face->N();
	float a=n[0]*dir[0]+n[1]*dir[1]+n[2]*dir[2];

	Point3f f;
	GetVelocityComponents(v,face);
	//Point3f vel=GetVelocityComponents(v,face);
	Point3f vel;


	//Compute force component along the face
	f[0]=dir[0]-a*n[0];
	f[1]=dir[1]-a*n[1];
	f[2]=dir[2]-a*n[2];


	
/*
	float angle=0;
	//Vx
	angle = asin(f[0]/m);
	vel[0]=v*cos(angle);
	//Vy
	angle = asin(f[1]/m);
	vel[1]=v*cos(angle);
	//Vz
	angle = asin(f[2]/m);
	vel[2]=v*cos(angle);;
	*/

	vel[0]=0;
	vel[1]=0;
	vel[2]=0;
	new_pos[0]=p[0]+(vel[0]*t+0.5*(f[0]/m)*pow(t,2))*l;
	new_pos[1]=p[1]+(vel[1]*t+0.5*(f[1]/m)*pow(t,2))*l;
	new_pos[2]=p[2]+(vel[2]*t+0.5*(f[2]/m)*pow(t,2))*l;


	return new_pos;

};

void DrawDust(MeshModel *base_mesh,MeshModel *cloud_mesh){
	if(base_mesh->cm.HasPerWedgeTexCoord() && base_mesh->cm.textures.size()>0){
		QImage img;
		QFileInfo text_file=QFileInfo(base_mesh->cm.textures[0].c_str());
		img.load(base_mesh->cm.textures[0].c_str());

		QPainter painter(&img);
		float w=img.width();
		float h=img.height();
		painter.setPen(Qt::black);
		painter.setBrush(Qt::SolidPattern);
		base_mesh->updateDataMask(MeshModel::MM_WEDGTEXCOORD);
		CMeshO::PerVertexAttributeHandle<Particle<CMeshO> > ph= tri::Allocator<CMeshO>::GetPerVertexAttribute<Particle<CMeshO> > (cloud_mesh->cm,std::string("ParticleInfo"));

		CMeshO::VertexIterator vi;
		for(vi=cloud_mesh->cm.vert.begin();vi!=cloud_mesh->cm.vert.end();++vi){
			CMeshO::FacePointer f=ph[vi].face;

			TexCoord2f t0=f->WT(0);
			TexCoord2f t1=f->WT(1);
			TexCoord2f t2=f->WT(2);



			Point2f p0=Point2f(t0.U()*w,h-t0.V()*h);
			Point2f p1=Point2f(t1.U()*w,h-t1.V()*h);
			Point2f p2=Point2f(t2.U()*w,h-t2.V()*h);


			//QPolygonF polygon;
			//polygon.append(QPointF(p0[0],p0[1]));
			//polygon.append(QPointF(p1[0],p1[1]));
			//polygon.append(QPointF(p2[0],p2[1]));

			Point3f bc;
			Point2f dbc;
			InterpolationParameters(*f,vi->P(),bc);
			dbc=p0*bc[0]+p1*bc[1]+p2*bc[2];
			painter.drawPoint(dbc[0],dbc[1]);
		}
		QString path=QDir::currentPath()+"/dirt_texture.png";
		img.save(path,"PNG");
		base_mesh->cm.textures.clear();
		base_mesh->cm.textures.push_back(path.toStdString());

	}


};


void ColorizeMesh(MeshModel* m){
	CMeshO::FaceIterator fi;
	float base_color=255;
	float s_color;
	for(fi = m->cm.face.begin(); fi != m->cm.face.end(); ++fi){
		s_color=(*fi).Q();
		if(s_color>255) s_color=255;
		(*fi).C()=Color4b(base_color-s_color,base_color-s_color, base_color-s_color, 0);
	}


	/*float base_color=255;
	float s_color;
	std::pair<float,float> minmax = tri::Stat<CMeshO>::ComputePerFaceQualityMinMax(m->cm);
	CMeshO::FaceIterator fi;
	for(fi = m->cm.face.begin(); fi != m->cm.face.end(); ++fi){
	s_color=base_color*(1-(((*fi).Q()-minmax.first)/(minmax.second-minmax.first)));
	(*fi).C()=Color4b(s_color, s_color, s_color, 0);
	}
	*/
};


/**
@def Compute the intersection of the segment from p1 to p2 and the face f

@param CoordType p1 - position of the first point
@param Coordtype p2 - position of the second poin
@param Facepointer f - pointer to the face
@param CoordType int_point - intersection point this is a return parameter for the function.
@param FacePointer face - pointer to the new face

@return the intersection edge index if there is an intersection -1 elsewhere
Step
*/

int ComputeIntersection(CMeshO::CoordType p1,CMeshO::CoordType p2,CMeshO::FacePointer &f,CMeshO::FacePointer &new_f,CMeshO::CoordType &int_point){
	
	CMeshO::CoordType v0=f->V(0)->P();
	CMeshO::CoordType v1=f->V(1)->P();
	CMeshO::CoordType v2=f->V(2)->P();
	
	float dist[3];
	Point3f int_points[3];
	dist[0]=PSDist(p2,v0,v1,int_points[0]);
	dist[1]=PSDist(p2,v1,v2,int_points[1]);
	dist[2]=PSDist(p2,v2,v0,int_points[2]);
	
	int edge=-1;
	if(dist[0]<dist[1]){
		if(dist[0]<dist[2]) edge=0;
		else edge=2;
	}else{
		if(dist[1]<dist[2]) edge=1;
		else edge=2;
	}
	
	CMeshO::VertexType* v;
	if(Distance(int_points[edge],f->V(edge)->P())<Distance(int_points[edge],f->V((edge+1) % 3)->P())) v=f->V(edge);
	else v=f->V((edge+1) % 3);
	vcg::face::Pos<CMeshO::FaceType> p(f,edge,v);
	new_f=f->FFp(edge);
	if(new_f==f) return -1;
	
	
	
	
	if(Distance(int_points[edge],v->P())<EPSILON){
	p.FlipF();
	CMeshO::FacePointer tmp_f=p.F();
	
	int n_face=0;
	while(tmp_f!=f){
			p.FlipE();
			p.FlipF();
			tmp_f=p.F();
			n_face++;
			}
	if(n_face!=0){
	int r=(rand()%(n_face-1))+2;

	for(int i=0;i<r;i++){
		p.FlipE();
		p.FlipF();
	}
	new_f=p.F();
	}
	}	

	/*
		//Near a vertex
		p.FlipE();
		p.FlipF();
		CMeshO::FacePointer tmp_f=p.F();
		float min_dist=-1;
			float min_dist;
			Point3f prj;
			vcg::Triangle3<float> triangle(tmp_f->P(0),tmp_f->P(1),tmp_f->P(2));
			float dist;
			TrianglePointDistance<float>(triangle,p2,dist,prj);
			if(min_dist==-1){
				min_dist=dist;
				new_f=tmp_f;
				}
				else{
					if(dist<min_dist){
					min_dist=dist;
					new_f=tmp_f;
					}
			}
			//if(IsOnFace(prj,tmp_f)){
			//	new_f=tmp_f;
			//	tmp_f->C()=Color4b::Black;
			//	}
			p.FlipE();
			p.FlipF();
			tmp_f=p.F();
		
		}
		if(i%2!=0)i++;
		for(int j=0;j<=i/2;j++){
			p.FlipE();
			p.FlipF();
		}
		new_f=p.F();
	}
	*/
	


	int_point=GetSafePosition(int_points[edge],new_f);
	return edge;
};






/**
@def Compute the Normal Dust Amount Function per face of a Mesh m

@param m MeshModel
@param u CoordType dust direction
@param k float
@param s float

@return nothing
*/
void ComputeNormalDustAmount(MeshModel* m,CMeshO::CoordType u,float k,float s){

	CMeshO::FaceIterator fi;
	float d;
	for(fi=m->cm.face.begin();fi!=m->cm.face.end();++fi){
		d=k/s+(1+k/s)*pow(fi->N().dot(u),s);
		fi->Q()=d;
	}

};

/**
@def This function compute the Surface Exposure per face of a Mesh m

@param  MeshModel* m - Pointer to the new mesh
@param int r - scaling factor
@param int n_ray - number of rays emitted

@return nothing
*/
void ComputeSurfaceExposure(MeshModel* m,int r,int n_ray){

	CMeshO::PerFaceAttributeHandle<float> eh=vcg::tri::Allocator<CMeshO>::AddPerFaceAttribute<float>(m->cm,std::string("exposure"));

	float dh=1.2;
	float exp=0;
	float di=0;
	float xi=0;

	CMeshO::FacePointer face;
	CMeshO::CoordType p_c;
	MetroMeshFaceGrid f_grid;
	f_grid.Set(m->cm.face.begin(),m->cm.face.end());
	MarkerFace markerFunctor;
	markerFunctor.SetMesh(&(m->cm));
	RayTriangleIntersectionFunctor<false> RSectFunct;
	CMeshO::FaceIterator fi;
	for(fi=m->cm.face.begin();fi!=m->cm.face.end();++fi){
		xi=0;
		eh[fi]=0;

		for(int i=0;i<n_ray;i++){
			//For every f_face  get the central point
			p_c=fromBarCoords(RandomBaricentric(),&*fi);
			//Create a ray with p_c as origin and direction N
			p_c=p_c+NormalizedNormal(*fi)*0.1;
			Ray3<float> ray=Ray3<float>(p_c,fi->N());
			di=0;
			face=0;
			face=f_grid.DoRay<RayTriangleIntersectionFunctor<false>,MarkerFace>(RSectFunct,markerFunctor,ray,1000,di);
			
			if(di!=0){
				xi=xi+(dh/(dh-di));
				//face->C()=Color4b::Black;
				//fi->C()=Color4b::Red;
			}
		}
		exp=1-(xi/n_ray);
		eh[fi]=exp;

	}
};


void ComputeParticlesFallsPosition(MeshModel* cloud_mesh,MeshModel* base_mesh,CMeshO::CoordType dir){
	CMeshO::VertexIterator vi;
	
	MetroMeshFaceGrid f_grid;
	f_grid.Set(base_mesh->cm.face.begin(),base_mesh->cm.face.end());
	MarkerFace markerFunctor;
	markerFunctor.SetMesh(&(base_mesh->cm));
	RayTriangleIntersectionFunctor<false> RSectFunct;
	CMeshO::PerVertexAttributeHandle<Particle<CMeshO> > ph= tri::Allocator<CMeshO>::GetPerVertexAttribute<Particle<CMeshO> >(cloud_mesh->cm,"ParticleInfo");
	
	
	std::vector<CMeshO::VertexPointer> ToDelVec;


	for(vi=cloud_mesh->cm.vert.begin();vi!=cloud_mesh->cm.vert.end();++vi){
		Particle<CMeshO> info=ph[vi];
		if((*vi).IsS()){
			Point3f p_c=vi->P()+info.face->N().normalized()*0.1;
		Ray3<float> ray=Ray3<float>(p_c,dir);
		float di;	
		CMeshO::FacePointer new_f=f_grid.DoRay<RayTriangleIntersectionFunctor<false>,MarkerFace>(RSectFunct,markerFunctor,ray,base_mesh->cm.bbox.Diag(),di);
		
		if(new_f!=0){
				info.face=new_f;
				float min_dist;
				Point3f prj;
				vcg::Triangle3<float> triangle(new_f->P(0),new_f->P(1),new_f->P(2));
				float dist;
				TrianglePointDistance<float>(triangle,vi->P(),dist,prj);
				vi->ClearS();
		}else{
			ToDelVec.push_back(&*vi);
			}
		}
	}

	for(int i=0;i<ToDelVec.size();i++){
		if(!ToDelVec[i]->IsD()) Allocator<CMeshO>::DeleteVertex(cloud_mesh->cm,*ToDelVec[i]);
	}
};



/**
@def This funcion

@param
@param
@param

@return ?
*/
bool GenerateParticles(MeshModel* m,std::vector<CMeshO::CoordType> &cpv,std::vector< Particle<CMeshO> > &dpv,int d,float threshold){

	//Handler
	CMeshO::PerFaceAttributeHandle<float> eh=vcg::tri::Allocator<CMeshO>::GetPerFaceAttribute<float>(m->cm,std::string("exposure"));

	CMeshO::FaceIterator fi;
	CMeshO::CoordType p;
	cpv.clear();
	dpv.clear();
	float r=1;
	float a0=0;
	float a=0;
	float a1=0;
	int n_dust=0;

	for(fi=m->cm.face.begin();fi!=m->cm.face.end();++fi){
		a1=a0+r*eh[fi];
		if(a1<0)
			a=0;
		if(a1>1)
			a=1;
		if(a1>=0 && a1<=1)
			a=a1;


		if(eh[fi]==1) a=1;
		else a=0;

		n_dust=(int)d*fi->Q()*a;

		for(int i=0;i<n_dust;i++){
			p=RandomBaricentric();
			CMeshO::CoordType n_p;
			n_p=fi->P(0)*p[0]+fi->P(1)*p[1]+fi->P(2)*p[2];
			cpv.push_back(n_p);
			Particle<CMeshO> part;
			part.face=&(*fi);
			part.bar_coord=p;
			dpv.push_back(part);
		}

		fi->Q()=n_dust;

	}


	return true;
};


/**

*/
void associateParticles(MeshModel* b_m,MeshModel* c_m,float m,float v){

	MetroMeshFaceGrid   unifGridFace;
	Point3f closestPt;
	CMeshO::PerVertexAttributeHandle<Particle<CMeshO> > ph= tri::Allocator<CMeshO>::AddPerVertexAttribute<Particle<CMeshO> > (c_m->cm,std::string("ParticleInfo"));
	unifGridFace.Set(b_m->cm.face.begin(),b_m->cm.face.end());
	MarkerFace markerFunctor;
	markerFunctor.SetMesh(&(b_m->cm));
	float dist=1;
	float dist_upper_bound=dist;
	CMeshO::VertexIterator vi;
	vcg::face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
	for(vi=c_m->cm.vert.begin();vi!=c_m->cm.vert.end();++vi){
		Particle<CMeshO> part;
		part.face=unifGridFace.GetClosest(PDistFunct,markerFunctor,vi->P(),dist_upper_bound,dist,closestPt);
		part.face->Q()=part.face->Q()+1;
		part.mass=m;
		part.vel=v;
		ph[vi]=part;
	}



};


/**
@def This function initialize the mesh m in order to respect some prerequisites of the filter

@param MeshModel* m - Pointer to the Mesh

@return nothing
*/
void prepareMesh(MeshModel* m){



	m->updateDataMask(MeshModel::MM_FACEFACETOPO);
	m->updateDataMask(MeshModel::MM_FACEMARK);
	m->updateDataMask(MeshModel::MM_FACECOLOR);
	m->updateDataMask(MeshModel::MM_VERTQUALITY);
	m->updateDataMask(MeshModel::MM_FACEQUALITY);
	m->updateDataMask(MeshModel::MM_FACENORMAL);

	tri::UnMarkAll(m->cm);


	//clean Mesh
	tri::Allocator<CMeshO>::CompactFaceVector(m->cm);
	tri::Clean<CMeshO>::RemoveUnreferencedVertex(m->cm);
	tri::Clean<CMeshO>::RemoveDuplicateVertex(m->cm);
	tri::Allocator<CMeshO>::CompactVertexVector(m->cm);

	tri::UpdateFlags<CMeshO>::FaceClear(m->cm);

	//update Mesh
	m->cm.vert.EnableVFAdjacency();
	m->cm.face.EnableVFAdjacency();
	tri::UpdateTopology<CMeshO>::FaceFace(m->cm);
	tri::UpdateTopology<CMeshO>::VertexFace(m->cm);
	tri::UpdateNormals<CMeshO>::PerFaceNormalized(m->cm);
	tri::UpdateFlags<CMeshO>::FaceProjection(m->cm);

};

void MoveParticle(Particle<CMeshO> &info,CMeshO::VertexPointer p,float l,int t,Point3f dir){
	float time=t;
	float velocity;
	float mass;
	Point3f new_pos;
	Point3f current_pos;
	Point3f int_pos;
	CMeshO::FacePointer current_face=info.face;
	CMeshO::FacePointer new_face;
	new_face=current_face;
	current_pos=p->P();
	velocity=info.vel;
	mass=info.mass;
	new_pos=StepForward(current_pos,velocity,mass,current_face,dir,l,time);
	if(!IsOnFace(new_pos,current_face)){
		int edge=ComputeIntersection(current_pos,new_pos,current_face,new_face,int_pos);
		if(edge!=-1){
				new_pos=int_pos;
			current_face=new_face;	
			Point3f n = current_face->N();
			if(acos(n.dot(dir)/(n.Norm()*dir.Norm()))<(PI/2))
				current_face->C()=Color4b::Green;
				p->SetS();
			}else{
			new_pos=int_pos;
			current_face=new_face;	
			current_face->C()=Color4b::Blue;
			}
	}
	
	/*
	int i=0;
	while(!IsOnFace(new_pos,current_face)){
	if(i<5){
		ComputeIntersection(current_pos,new_pos,current_face,new_face,int_pos);
		current_pos=int_pos;
		current_face->C()=Color4b::Green;
		current_face=new_face;
		//time=time/2;
		new_pos=StepForward(current_pos,velocity,mass,current_face,dir,l,time);
	}else{
		//current_face->C()=Comp::Green;
		ComputeIntersection(current_pos,new_pos,current_face,new_face,int_pos);
		new_pos=int_pos;
		current_face=new_face;
		}
	i++;
	}
	current_face->Q()=current_face->Q()+1;
	//current_face->C()=Color4b::Red;
	p->P()=new_pos;
	info.vel=velocity;
	info.face=current_face;
	*/
	current_face->Q()=current_face->Q()+1;
	p->P()=new_pos;
	info.vel=velocity;
	info.face=current_face;
	

};




/**
@def This function compute the repulsion beetwen particles

@param MeshModel* c_m - cloud of points
@param int k          - max number of particle to repulse

@return nothing
*/
void ComputeRepulsion(MeshModel *c_m,int k){
	CMeshO::PerVertexAttributeHandle<Particle<CMeshO> > ph = Allocator<CMeshO>::GetPerVertexAttribute<Particle<CMeshO> >(c_m->cm,"ParticleInfo");
	MetroMeshVertexGrid v_grid;
	std::vector<Point3f> v_points;
	std::vector<CMeshO::VertexPointer> vp;
	std::vector<float> distances;

	v_grid.Set(c_m->cm.vert.begin(),c_m->cm.vert.end());

	Point3f bc ;

	CMeshO::VertexIterator vi;

	for(vi=c_m->cm.vert.begin();vi!=c_m->cm.vert.end();++vi){
		vcg::tri::GetKClosestVertex(c_m->cm,v_grid,k,vi->P(),0.0001f,vp,distances,v_points);
		for(int i=0;i<v_points.size();i++){
			Point3f dir = fromBarCoords(RandomBaricentric(),ph[vp[i]].face);
			Ray3<float> ray=Ray3<float>(vi->P(),dir);
			//MoveParticle(ph[vp[i]],vp[i],0.01f,1,ray.Direction());
			Point3f n_p=dir-vi->P();
			vi->P()=vi->P()+n_p*0.1;
		}
	}
};


/**
@def This function simulate the movement of the cloud mesh, it requires that every point is associated with a Particle data structure

@param MeshModel cloud  - Mesh of points
@param Point3f   force  - Direction of the force
@param float     l      - Lenght of the  movementstep
@param float     t   - Time Step

@return nothing
*/
void MoveCloudMeshForward(MeshModel *cloud,MeshModel *base,Point3f force,float l,float t,int r_step){

	CMeshO::PerVertexAttributeHandle<Particle<CMeshO> > ph = Allocator<CMeshO>::GetPerVertexAttribute<Particle<CMeshO> >(cloud->cm,"ParticleInfo");
	CMeshO::VertexIterator vi;
	
	for(vi=cloud->cm.vert.begin();vi!=cloud->cm.vert.end();++vi)
		if(!vi->IsD())MoveParticle(ph[vi],&*vi,l,t,force);
		
	
		ComputeParticlesFallsPosition(cloud,base,force);

	//for(int i=0;i<r_step;i++)
	//	ComputeRepulsion(cloud,10);
	
};





#endif // DIRT_UTILS_H
