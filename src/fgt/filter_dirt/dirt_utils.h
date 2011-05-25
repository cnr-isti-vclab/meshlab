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



/**

*/

bool CheckFallPosition(CMeshO::FacePointer f,Point3f g,float a){
				Point3f n=f->N();
				if(a>1) return false; 
				if(acos(n.dot(g)/(n.Norm()*g.Norm()))<((PI/2)*(1-a))) return true;
				return false;
};

float GetElapsedTime(CMeshO::CoordType p1,CMeshO::CoordType p2, CMeshO::CoordType p3, float t,float l){
	float d1= Distance(p1,p2);
	float d2= Distance(p2,p3);
	if(d1+d2==0) return 0;
	float remainig_time=(d1/(d1+d2))*t;
};

void ComputeParticlesVelocityAndTime(CMeshO::CoordType o_p,CMeshO::CoordType n_p,CMeshO::FacePointer f,CMeshO::CoordType dir,float l,float m,float o_v,float &t,float &n_v){

	Point3f n=f->N();
	float b=n[0]*dir[0]+n[1]*dir[1]+n[2]*dir[2];
	float distance=Distance(o_p,n_p);
	Point3f force;
	force[0]=dir[0]-b*n[0];
	force[1]=dir[1]-b*n[1];
	force[2]=dir[2]-b*n[2];
	if(force.Norm()==0){
		t=1;
		n_v=0;
		return;
	}
	float acceleration=(force/m).Norm();
	float velocity=math::Sqrt(pow(o_v,2)+(2*acceleration*distance));
	float time;
	time=(velocity-o_v)/acceleration;

	t=time;
	n_v=velocity;
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
@def Given a Face and a point that lies on a edge of that face return a position slightly more internal to avoid 
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
	safe_p=p+p1*0.02;
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
	Point3f vel;
	//Compute force component along the face
	f[0]=dir[0]-a*n[0];
	f[1]=dir[1]-a*n[1];
	f[2]=dir[2]-a*n[2];
	
	if(f.Norm()!=0){
		vel=f/f.Norm();
		vel=vel*v;
	}else{
		vel[0]=0;
		vel[1]=0;
		vel[2]=0;
	}
	new_pos=p+(vel*t+(f/m)*pow(t,2)*0.5)*l;
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
	float color;
	float dirtiness;
	for(fi = m->cm.face.begin(); fi != m->cm.face.end(); ++fi){
		dirtiness=fi->Q();
		if(dirtiness==0){
			fi->C()=Color4b(255,255,255,0);
		}else{
		if(dirtiness>255) fi->C()=Color4b(0,0,0,0);
		else fi->C()=Color4b(255-dirtiness,255-dirtiness,255-dirtiness,0);
		}
	}

	tri::UpdateColor<CMeshO>::VertexFromFace(m->cm);
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
				ph[vi].face=new_f;
				float min_dist;
				Point3f prj;
				vcg::Triangle3<float> triangle(new_f->P(0),new_f->P(1),new_f->P(2));
				float dist;
				float u;
				float v;
				float t;
				IntersectionRayTriangle<float>(ray,new_f->P(0),new_f->P(1),new_f->P(2),t,u,v);
				Point3f bc(1-u-v,u,v);
				vi->P()=fromBarCoords(bc,new_f);
				vi->ClearS();
				new_f->C()=Color4b::Red;	
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
bool GenerateParticles(MeshModel* m,std::vector<CMeshO::CoordType> &cpv,/*std::vector< Particle<CMeshO> > &dpv,*/int d,float threshold){

	//Handler
	CMeshO::PerFaceAttributeHandle<float> eh=vcg::tri::Allocator<CMeshO>::GetPerFaceAttribute<float>(m->cm,std::string("exposure"));

	CMeshO::FaceIterator fi;
	CMeshO::CoordType p;
	cpv.clear();
	//dpv.clear();
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
			/*Particle<CMeshO> part;
			part.face=&(*fi);
			part.bar_coord=p;
			dpv.push_back(part);
			*/
		}

		fi->Q()=n_dust;

	}


	return true;
};


/**

*/
void associateParticles(MeshModel* b_m,MeshModel* c_m,float &m,float &v){
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
		Particle<CMeshO>* part = new Particle<CMeshO>();
		part->face=unifGridFace.GetClosest(PDistFunct,markerFunctor,vi->P(),dist_upper_bound,dist,closestPt);
		part->face->Q()=part->face->Q()+1;
		part->mass=m;
		part->velocity=v;
		ph[vi]=*part;
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

	CMeshO::FaceIterator fi;
	for(fi=m->cm.face.begin();fi!=m->cm.face.end();++fi){
		fi->Q()=0;
	}
};

/**
@def This function move a particle over the mesh
*/
void MoveParticle(Particle<CMeshO> &info,CMeshO::VertexPointer p,float l,int t,Point3f dir,Point3f g,float a){
	if(CheckFallPosition(info.face,g,a)){
		p->SetS();
		return;
	}
	float time=t;
	
	Point3f new_pos;
	Point3f current_pos;
	Point3f int_pos;
	CMeshO::FacePointer current_face=info.face;
	CMeshO::FacePointer new_face;
	new_face=current_face;
	current_pos=p->P();
	new_pos=StepForward(current_pos,info.velocity,info.mass,current_face,g+dir,l,time);
	while(!IsOnFace(new_pos,current_face)){
		int edge=ComputeIntersection(current_pos,new_pos,current_face,new_face,int_pos);
		if(edge!=-1){
			Point3f n = new_face->N();
			if(CheckFallPosition(new_face,g,a))  p->SetS();
			float new_velocity=0;
			//ComputeParticlesVelocityAndTime(current_pos,int_pos,info.face,dir,l,info.mass,info.velocity,elapsed_time,new_velocity);
			info.velocity=new_velocity;
			float elapsed_time=GetElapsedTime(current_pos,int_pos,new_pos,time,l);
			time=time-elapsed_time;
			current_pos=int_pos;
			current_face->Q()+=elapsed_time*5;
			current_face=new_face;	
			new_pos=int_pos;
			if(time>0){
				if(p->IsS()) break;
				new_pos=StepForward(current_pos,info.velocity,info.mass,current_face,g+dir,l,time);
			}
			current_face->C()=Color4b::Green;//Just Debug!!!!
		}else{
			//We are on a border
			new_pos=int_pos;
			current_face=new_face;	
			p->SetS();
			break;
		}
	}
	//current_face->Q()+=1;
	//Point3f n = ->N();
	//if(acos(n.dot(dir)/(n.Norm()*dir.Norm()))<(m_angle)) p->SetS();
	//float new_velocity;
	//float elapsed_time;
	//ComputeParticlesVelocityAndTime(current_pos,int_pos,info.face,dir,l,info.mass,info.velocity,elapsed_time,new_velocity);
	//current_face->Q()=current_face->Q()+1;
	p->P()=new_pos;
	//info.velocity=new_velocity;
	info.face=current_face;
};




/**
@def This function compute the repulsion beetwen particles

@param MeshModel* c_m - cloud of points
@param int k          - max number of particle to repulse

@return nothing
*/
void ComputeRepulsion(MeshModel* b_m,MeshModel *c_m,int k,float l,Point3f g,float a){
	CMeshO::PerVertexAttributeHandle<Particle<CMeshO> > ph = Allocator<CMeshO>::GetPerVertexAttribute<Particle<CMeshO> >(c_m->cm,"ParticleInfo");
	MetroMeshVertexGrid v_grid;
	std::vector<Point3<float>> v_points;
	std::vector<CMeshO::VertexPointer> vp;
	std::vector<float> distances;

	v_grid.Set(c_m->cm.vert.begin(),c_m->cm.vert.end(),b_m->cm.bbox);
	
	Point3f bc ;
	CMeshO::VertexIterator vi;
	
	for(vi=c_m->cm.vert.begin();vi!=c_m->cm.vert.end();++vi){
		vcg::tri::GetKClosestVertex(c_m->cm,v_grid,k,vi->P(),EPSILON,vp,distances,v_points);
		for(int i=0;i<vp.size();i++){CMeshO::VertexPointer v = vp[i];
			if(v->P()!=vi->P() && !v->IsD() && !vi->IsD()){
			Ray3<float> ray(vi->P(),fromBarCoords(RandomBaricentric(),ph[vp[i]].face));
			ray.Normalize();
			Point3f dir=ray.Direction(); 
			dir.Normalize();
			MoveParticle(ph[vp[i]],vp[i],0.01,1,dir,g,a);
					}
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
void MoveCloudMeshForward(MeshModel *cloud,MeshModel *base,Point3f g,Point3f force,float l,float a,float t,int r_step){

	CMeshO::PerVertexAttributeHandle<Particle<CMeshO> > ph = Allocator<CMeshO>::GetPerVertexAttribute<Particle<CMeshO> >(cloud->cm,"ParticleInfo");
	CMeshO::VertexIterator vi;
		for(vi=cloud->cm.vert.begin();vi!=cloud->cm.vert.end();++vi)
			if(!vi->IsD()) MoveParticle(ph[vi],&*vi,l,t,force,g,a);
			
		
			//Handle falls Particle
	ComputeParticlesFallsPosition(cloud,base,g);
	//Compute Particles Repulsion
	for(int i=0;i<r_step;i++)
		ComputeRepulsion(base,cloud,50,l,g,a);
};


#endif // DIRT_UTILS_H
