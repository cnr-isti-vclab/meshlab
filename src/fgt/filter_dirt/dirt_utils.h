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
#ifndef DIRT_UTILS_H
#define DIRT_UTILS_H

//Include Files
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
#include<vcg/complex/trimesh/base.h>
#include <vcg/space/point3.h>
#include <vcg/space/intersection2.h>
#include <vcg/complex/trimesh/allocate.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/closest.h>
#include <vcg/simplex/face/distance.h>
#include <vcg/complex/trimesh/geodesic.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/flag.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/point_sampling.h>
#include <vcg/complex/trimesh/create/resampler.h>
#include <vcg/complex/trimesh/clustering.h>
#include <vcg/simplex/face/distance.h>
#include <vcg/space/index/grid_static_ptr.h>
#include <vcg/space/intersection3.h>
#include "particle.h"
using namespace vcg;
using namespace tri;

typedef GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType > MetroMeshFaceGrid;
typedef GridStaticPtr<CMeshO::VertexType, CMeshO::ScalarType > MetroMeshVertexGrid;
typedef FaceTmark<CMeshO> MarkerFace;

/**
@def Generate random barycentric coordinates
@return a triple of barycentric coordinates
*/
CMeshO::CoordType RandomBaricentric(){
    CMeshO::CoordType interp;
    static math::MarsenneTwisterRNG rnd;
    interp[1] = rnd.generate01();
    interp[2] = rnd.generate01();
        if(interp[1] + interp[2] > 1.0)
        {
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




/**

  */
CMeshO::CoordType GetSafePosition(CMeshO::FacePointer f,int edge){

    Point3f p0=f->P(0);
    Point3f p1=f->P(1);
    Point3f p2=f->P(2);

    Point3f bc;

    bc[0]=0.33f;
    bc[1]=0.33f;
    bc[2]=1-bc[0]-bc[1];

    CMeshO::CoordType pc=fromBarCoords(bc,f);
    CMeshO::CoordType safe_p;

    switch(edge){
    case 0:{
            bc=RandomBaricentric();
            safe_p=p0*bc[0]+p1*bc[1]+pc*bc[2];
            break;
        }
    case 1:{
            bc=RandomBaricentric();
            safe_p=p1*bc[0]+p2*bc[1]+pc*bc[2];
            break;
        }
    case 2:{
            bc=RandomBaricentric();
            safe_p=p2*bc[0]+p0*bc[1]+pc*bc[2];
            break;
        }

    }
    return safe_p;

};


/**
@def Verify if a point lies on a face

@param Point3f p   - Coordinates of the point
@param FacePointer f - Pointer to the face

@return true if point p is on face f, false elsewhere.
*/
bool IsOnFace(Point3f p, CMeshO::FacePointer f){
    float EPSILON=0.00001;
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
    if(math::Abs(u)<EPSILON) u=0;
    if(math::Abs(v)<EPSILON) v=0;
    return (u >= 0) && (v >= 0) && (u + v <=1+EPSILON);
};





float getElapsedTime(CMeshO::CoordType pi,CMeshO::CoordType pf,Point3f vi,Point3f a){

    float t1=0;
    float t2=0;
    float s=pi[0]-pf[0];
    float d=pow(vi[0],2)- 2*a[0]*s;
    if(d<0) d=0;
    t1=(-vi[0]+sqrt(d))/a[0];
    t2=(-vi[0]-sqrt(d))/a[0];
    if(t1>0) return t1;
    if(t2>0) return t2;
    return 0;

};

float getElapsedTime(CMeshO::CoordType vi,CMeshO::CoordType vf,CMeshO::FacePointer face,float m,CMeshO::CoordType force){

    float t=0;
    float v_initial=sqrt(pow(vi[0],2)+pow(vi[1],2)+pow(vi[2],2));
    float v_final=sqrt(pow(vf[0],2)+pow(vf[1],2)+pow(vf[2],2));
    Point3f n= face->N();
    float a=n[0]*force[0]+n[1]*force[1]+n[2]*force[2];



    Point3f f;

    f[0]=force[0]-a*n[0];
    f[1]=force[1]-a*n[1];
    f[2]=force[2]-a*n[2];

    float acceleration=sqrt(pow(f[0]/m,2)+pow(f[1]/m,2)+pow(f[2]/m,2));
    t=(v_final-v_initial)/acceleration;


    return t;
};


bool IsOnEdge(CMeshO::CoordType p,CMeshO::FacePointer f){
    if(f==0) return false;
    float bc[3];
    //f->C()=Color4b::Green;
    InterpolationParameters(*f,f->N(),p,bc[0],bc[1],bc[2]);
    if(bc[0]==0.0f || bc[1]==0.0f || bc[2]==0.0f) return true;
    return false;
};


CMeshO::CoordType ComputeVelocity(CMeshO::CoordType vi,CMeshO::CoordType ac,float t){

    CMeshO::CoordType n_vel;

    n_vel[0]=vi[0]+ac[0]*t;
    n_vel[1]=vi[1]+ac[1]*t;
    n_vel[2]=vi[2]+ac[2]*t;

    return n_vel;
};
CMeshO::CoordType UpdateVelocity(CMeshO::CoordType pf,CMeshO::CoordType pi,CMeshO::CoordType v,float m,CMeshO::FacePointer &face,CMeshO::CoordType force){

    CMeshO::CoordType new_vel;
    Point3f n= face->cN();
    float a=n[0]*force[0]+n[1]*force[1]+n[2]*force[2];
    //float b=n[0]*vel[0]+n[1]*vel[1]+n[2]*vel[2];

    Point3f f;
    //Point3f v;

    f[0]=force[0]-a*n[0];
    f[1]=force[1]-a*n[1];
    f[2]=force[2]-a*n[2];



    new_vel[0]=sqrt(pow(v[0],2)+2*(f[0]/m)*(pf[0]-pi[0]) );
    new_vel[1]=sqrt(pow(v[1],2)+2*(f[1]/m)*(pf[1]-pi[1]) );
    new_vel[2]=sqrt(pow(v[2],2)+2*(f[2]/m)*(pf[2]-pi[2]) );


    return new_vel;
};
CMeshO::CoordType ComputeAcceleration(float m,CMeshO::FacePointer face,CMeshO::CoordType dir){
    CMeshO::CoordType acc;

    Point3f n= face->N();
    float a=n[0]*dir[0]+n[1]*dir[1]+n[2]*dir[2];

    acc[0]=dir[0]-a*n[0];
    acc[1]=dir[1]-a*n[1];
    acc[2]=dir[2]-a*n[2];

    return acc;
};


/**
@def
*/
CMeshO::CoordType GetVelocityComponents(float v,float l,CMeshO::FacePointer face){

    Point3f dir = Point3f(0,-1,0);
    Point3f n=face->cN();
    //n.Normalize();
    float a = dir.dot(n);
    Point3f vel;

    vel[0]=dir[0]-n[0]*a;
    vel[1]=dir[1]-n[1]*a;
    vel[2]=dir[2]-n[2]*a;


    Point3f axis_x=Point3f(1,0,0);
    Point3f axis_y=Point3f(0,1,0);
    Point3f axis_z=Point3f(0,0,1);

    Point3f nx=Point3f(n[0],0,0);
    Point3f ny=Point3f(0,n[1],0);
    Point3f nz=Point3f(0,0,n[2]);
    //float alpha=90-(acos(axis_x.dot(nx))*(180/PI));
    //float beta =90-(acos(axis_y.dot(ny))*(180/PI));
    //float gamma=90-(acos(axis_z.dot(nz))*(180/PI));

    //vel[0]=v*cos(alpha*PI/180)*l;
    //vel[1]=v*cos(beta*PI/180)*l;
    //vel[2]=v*cos(gamma*PI/180)*l;

    vel[0]=vel[0]*l*v;
    vel[1]=vel[1]*l*v;
    vel[2]=vel[2]*l*v;

    return vel;




};


/**
  @def Simulate the movement of a point, affected by a force "dir" on a face.

  @param CoordType p   - coordinates of the point
  @param CoordType v   - velocity of the particle
  @param float     m   - mass of the particle
  @param FaceType face - pointer to the face
  @param CoordType dir - direction of the force
  @param float     t   - time step

  @return new coordinates of the point
*/
CMeshO::CoordType StepForward(CMeshO::CoordType p,float v,float m,CMeshO::FacePointer &face,CMeshO::CoordType dir,float l,float t=1){

    dir=dir*l;
    Point3f new_pos;
    Point3f n= face->N();
    float a=n[0]*dir[0]+n[1]*dir[1]+n[2]*dir[2];



    Point3f f;
    //Point3f vel=GetVelocityComponents(v,l,face);
    Point3f vel;
    vel[0]=0.0f;
    vel[1]=0.0f;
    vel[2]=0.0f;

    //Compute force component along the face
    f[0]=dir[0]-a*n[0];
    f[1]=dir[1]-a*n[1];
    f[2]=dir[2]-a*n[2];

    new_pos[0]=p[0]+vel[0]*t+0.5*(f[0]/m)*pow(t,2);
    new_pos[1]=p[1]+vel[1]*t+0.5*(f[1]/m)*pow(t,2);
    new_pos[2]=p[2]+vel[2]*t+0.5*(f[2]/m)*pow(t,2);

    return new_pos;
};

void DrawDust(MeshModel *base_mesh,MeshModel *cloud_mesh){
    if(base_mesh->cm.textures.size()>0){
        base_mesh->updateDataMask(MeshModel::MM_VERTTEXCOORD);

        CMeshO::PerVertexAttributeHandle<Particle<CMeshO> > ph= tri::Allocator<CMeshO>::AddPerVertexAttribute<Particle<CMeshO> > (cloud_mesh->cm,std::string("ParticleInfo"));

        CMeshO::VertexIterator vi;
        for(vi=cloud_mesh->cm.vert.begin();vi!=cloud_mesh->cm.vert.end();++vi){
            CMeshO::FacePointer f=ph[vi].face;
            TexCoord2f p0=f->V(0)->T();
            TexCoord2f p1=f->V(1)->T();
            TexCoord2f p2=f->V(2)->T();


        }
    }


    /* float base_color=255;
        float s_color;
        std::pair<float,float> minmax = tri::Stat<CMeshO>::ComputePerFaceQualityMinMax(m->cm);
        CMeshO::FaceIterator fi;
        for(fi = m->cm.face.begin(); fi != m->cm.face.end(); ++fi)
        {
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

*/
int ComputeIntersection(CMeshO::CoordType p1,CMeshO::CoordType p2,CMeshO::FacePointer &f,CMeshO::FacePointer &new_f,CMeshO::CoordType &int_point)
{




    CMeshO::CoordType fv0 =f->V(0)->P();
    CMeshO::CoordType fv1 =f->V(1)->P();
    CMeshO::CoordType fv2 =f->V(2)->P();

    int edge=-1;
    float dist[3];
    int axis=-1;
    Point3f tmp;
    Segment2f seg;
    Line2f line;

    dist[0]=PSDist(p2,fv0,fv1,tmp);
    dist[1]=PSDist(p2,fv1,fv2,tmp);
    dist[2]=PSDist(p2,fv2,fv0,tmp);

    if(dist[0]<dist[1]){
        if(dist[0]<dist[2]) edge=0;
        else edge=2;
    }else{
        if(dist[1]<dist[2]) edge=1;
        else edge=2;

    }

    new_f=f->FFp(edge);

    Point3f n=f->cN();
    float n0=math::Abs(n[0]);
    float n1=math::Abs(n[1]);
    float n2=math::Abs(n[2]);



    if(n0>n1){
         if(n0>n2) axis=0;
         else axis=2;
    }else{
        if(n1>n2) axis=1;
        else axis=2;
        }



    Point3f e1;
    Point3f e2;

    switch(edge){
    case 0:{
        e1=fv0;
        e2=fv1;
        break;
        }
    case 1:{
            e1=fv1;
            e2=fv2;
            break;
        }
    case 2:{
            e1=fv2;
            e2=fv0;
            break;
        }

    }

    switch(axis){
    case 0:{
        seg=Segment2f(Point2f(e1[1],e1[2]),Point2f(e2[1],e2[2]));
        line=Line2f(Point2f(p1[1],p1[2]),Point2f(p2[1],p2[2]));
        break;
        }
    case 1:{
        seg=Segment2f(Point2f(e1[0],e1[2]),Point2f(e2[0],e2[2]));
        line=Line2f(Point2f(p1[0],p1[2]),Point2f(p2[0],p2[2]));

        break;
        }
    case 2:{
        seg=Segment2f(Point2f(e1[0],e1[1]),Point2f(e2[0],e2[1]));
        line=Line2f(Point2f(p1[0],p1[1]),Point2f(p2[0],p2[1]));
        break;
        }


    }
    Point2f int_p;
    LineSegmentIntersection(line,seg,int_p);
    Point3f bc;

    switch(axis){
        case 0:{
           InterpolationParameters2(Point2f(fv0[1],fv0[2]),Point2f(fv1[1],fv1[2]),Point2f(fv2[1],fv2[2]),int_p,bc);
           break;
               }
        case 1:{
            InterpolationParameters2(Point2f(fv0[0],fv0[2]),Point2f(fv1[0],fv1[2]),Point2f(fv2[0],fv2[2]),int_p,bc);
            break;
           }
        case 2:{
            InterpolationParameters2(Point2f(fv0[0],fv0[1]),Point2f(fv1[0],fv1[1]),Point2f(fv2[0],fv2[1]),int_p,bc);
            break;
        }

    }

   int_point= fromBarCoords(bc,f);

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
        //For every f_face  get the central point
        p_c=Barycenter(*fi);
        //Create a ray with p_c as origin and direction N
        p_c=p_c+NormalizedNormal(*fi)*0.1;
        Ray3<float> ray=Ray3<float>(p_c,fi->N());
        xi=0;
        di=0;
        face=0;
        eh[fi]=0;
        face=f_grid.DoRay<RayTriangleIntersectionFunctor<false>,MarkerFace>(RSectFunct,markerFunctor,ray,30,di);
        if(di!=0){
            fi->C()=Color4b::Blue;
            xi=xi+(dh/(dh-di));
        }

        exp=1-xi;
        eh[fi]=exp;
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

        int n_dust=(int)d*fi->Q()*a;

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
        part.mass=m;
        part.vel=v;
        part.face->C()=Color4b::Blue;
        ph[vi]=part;
    }
};


/**
@def This function initialize the mesh m in order to respect some prerequisites of the filter

@param MeshModel* m - Pointer to the Mesh

@return nothing
*/
void prepareMesh(MeshModel* m){
    //clean flags

    tri::UpdateFlags<CMeshO>::FaceClear(m->cm);

    m->updateDataMask(MeshModel::MM_FACEFACETOPO);
    m->updateDataMask(MeshModel::MM_FACEMARK);
    m->updateDataMask(MeshModel::MM_FACECOLOR);
    m->updateDataMask(MeshModel::MM_VERTQUALITY);
    m->updateDataMask(MeshModel::MM_FACEQUALITY);
    //m->updateDataMask(MeshModel::MM_WEDGTEXCOORD);
    tri::UnMarkAll(m->cm);

    //clean Mesh
    tri::Allocator<CMeshO>::CompactFaceVector(m->cm);

    tri::Clean<CMeshO>::RemoveUnreferencedVertex(m->cm);
    tri::Clean<CMeshO>::RemoveDuplicateVertex(m->cm);
    tri::Allocator<CMeshO>::CompactVertexVector(m->cm);



    //update Mesh
    tri::UpdateTopology<CMeshO>::FaceFace(m->cm);
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
    new_pos=StepForward(p->P(),velocity,mass,current_face,dir,l,t);

    while(!IsOnFace(new_pos,current_face)){
        int edge=ComputeIntersection(current_pos,new_pos,current_face,new_face,int_pos);

        time=time/2;
        if(time>0.05){
            current_face->C()=Color4b::Blue;
            current_face=new_face;
            current_face->C()=Color4b::Yellow;
            new_pos=StepForward(int_pos,velocity,mass,current_face,dir,l,time);

        }else{
            new_pos=GetSafePosition(new_face,current_face->FFi(edge));
            current_face=new_face;

           }

         }

    p->P()=new_pos;
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
        vcg::tri::GetKClosestVertex(c_m->cm,v_grid,k,vi->P(),0.05f,vp,distances,v_points);
        for(int i=0;i<v_points.size();i++){
            Point3f dir = fromBarCoords(RandomBaricentric(),ph[vp[i]].face);
            Ray3<float> ray=Ray3<float>(vi->P(),dir);
            MoveParticle(ph[vp[i]],vp[i],0.01f,1,ray.Direction());
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
void MoveCloudMeshForward(MeshModel *cloud,Point3f force,float l,float t){

    CMeshO::PerVertexAttributeHandle<Particle<CMeshO> > ph = Allocator<CMeshO>::GetPerVertexAttribute<Particle<CMeshO> >(cloud->cm,"ParticleInfo");
    CMeshO::VertexIterator vi;

    for(vi=cloud->cm.vert.begin();vi!=cloud->cm.vert.end();++vi)
        MoveParticle(ph[vi],&*vi,l,t,force);

    for(int i=0;i<4;i++)
       ComputeRepulsion(cloud,10);

};

#endif // DIRT_UTILS_H
