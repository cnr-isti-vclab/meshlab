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
#include <common/meshmodel.h>
#include <common/interfaces.h>
#include<vector>
#include<vcg/simplex/vertex/base.h>
#include<vcg/simplex/face/base.h>
#include<vcg/complex/trimesh/base.h>
#include <vcg/space/point3.h>
#include <vcg/space/intersection2.h>
#include <vcg/complex/trimesh/allocate.h>
#include "dustparticle.h"

void ResetFaceQuality(MeshModel *m){

    CMeshO::FaceIterator fi;

    for(fi = m->cm.face.begin(); fi != m->cm.face.end(); ++fi)
        (*fi).Q()=0;


};

CMeshO::CoordType UpdateVelocity(CMeshO::CoordType v,
                              float m,
                              CMeshO::FacePointer &face,
                              CMeshO::CoordType dir,
                              float t=1){
    CMeshO::CoordType new_vel;
    Point3<float> n= face->cN();
    float a=n[0]*dir[0]+n[1]*dir[1]+n[2]*dir[2];


    Point3<float> f;
    //Calcolo le componenti della forza lungo il piano
    f[0]=dir[0]-a*n[0];
    f[1]=dir[1]-a*n[1];
    f[2]=dir[2]-a*n[2];

    new_vel[0]=v[0]+(f[0]/m)*t;
    new_vel[1]=v[1]+(f[0]/m)*t;
    new_vel[2]=v[2]+(f[0]/m)*t;

    return new_vel;
};


/*
  @description Simulate the movement of a point, affected by a force "dir" on a face.

  @param CoordType p   - coordinates of the point
  @param CoordType v   - velocity of the particle
  @param float     m   - mass of the particle
  @param FaceType face - pointer to the face
  @param CoordType dir - direction of the force
  @param float     t   - time step

  @return new coordinates of the point
*/
CMeshO::CoordType StepForward(CMeshO::CoordType p,
                              CMeshO::CoordType v,
                              float m,
                              CMeshO::FacePointer &face,
                              CMeshO::CoordType dir,
                              float t=1){
    //int t=1;
    Point3<float> new_pos;
    Point3<float> n= face->cN();
    float a=n[0]*dir[0]+n[1]*dir[1]+n[2]*dir[2];


    Point3<float> f;
    //Calcolo le componenti della forza lungo il piano
    f[0]=dir[0]-a*n[0];
    f[1]=dir[1]-a*n[1];
    f[2]=dir[2]-a*n[2];



    new_pos[0]=p[0]+v[0]*t+0.5*(f[0]/m)*pow(t,2);
    new_pos[1]=p[1]+v[1]*t+0.5*(f[1]/m)*pow(t,2);
    new_pos[2]=p[2]+v[2]*t+0.5*(f[2]/m)*pow(t,2);

    return new_pos;
};

void DrawDirt(MeshModel *m/*,std::vector<Point3f> &dp*/){
        float base_color=255;
        float s_color;
        std::pair<float,float> minmax = tri::Stat<CMeshO>::ComputePerFaceQualityMinMax(m->cm);
        CMeshO::FaceIterator fi;
        for(fi = m->cm.face.begin(); fi != m->cm.face.end(); ++fi)
        {
            s_color=base_color*(1-(((*fi).Q()-minmax.first)/(minmax.second-minmax.first)));
            (*fi).C()=Color4b(s_color, s_color, s_color, 0);
        }
};


/**
@description Compute the intersection of the segment from p1 to p2 and the face f

@param CoordType p1 - position of the first point
@param Coordtype p2 - position of the second poin
@param Facepointer f - pointer to the face
@param CoordType int_point - intersection point this is a return parameter for the function.
@param FacePointer face - pointer to the new face

@return true if there is an intersection
*/
bool ComputeIntersection(CMeshO::CoordType p1,CMeshO::CoordType p2,CMeshO::FacePointer &f,CMeshO::CoordType &int_point,CMeshO::FacePointer &new_f)
{

    CMeshO::CoordType n=f->N();

    int max_c;
    /*float n0=math::Abs(n[0]);
    float n1=math::Abs(n[1]);
    float n2=math::Abs(n[2]);

*/
    float n0=n[0];
    float n1=n[1];
    float n2=n[2];

    if(n0>n1){
          if(n0>n2) max_c=0;
          else max_c=2;
    }else{
        if(n1>n2) max_c=1;
        else max_c=2;
              }
    Point2f int_p;

    Segment2f seg;

    CMeshO::CoordType fv0 = (f->V(0))->P();
    CMeshO::CoordType fv1 = (f->V(1))->P();
    CMeshO::CoordType fv2 = (f->V(2))->P();
    Segment2f line0;
    Segment2f line1;
    Segment2f line2;
    Point2f p1_2d;
    Point2f p2_2d;
    switch(max_c){
    case 0:{
            line0=Segment2f(Point2f(fv0[1],fv0[2]),Point2f(fv1[1],fv1[2]));
            line1=Segment2f(Point2f(fv1[1],fv1[2]),Point2f(fv2[1],fv2[2]));
            line2=Segment2f(Point2f(fv2[1],fv2[2]),Point2f(fv0[1],fv0[2]));
            p1_2d=Point2f(p1[1],p1[2]);
            p2_2d=Point2f(p2[1],p2[2]);
            seg=Segment2f(p1_2d,p2_2d);

            break;
        }
    case 1:{
            line0=Segment2f(Point2f(fv0[0],fv0[2]),Point2f(fv1[0],fv1[2]));
            line1=Segment2f(Point2f(fv1[0],fv1[2]),Point2f(fv2[0],fv2[2]));
            line2=Segment2f(Point2f(fv2[0],fv2[2]),Point2f(fv0[0],fv0[2]));
            p1_2d=Point2f(p1[0],p1[2]);
            p2_2d=Point2f(p2[0],p2[2]);
            seg=Segment2f(p1_2d,p2_2d);
            break;
        }
    case 2:{
            line0=Segment2f(Point2f(fv0[0],fv0[1]),Point2f(fv1[0],fv1[1]));
            line1=Segment2f(Point2f(fv1[0],fv1[1]),Point2f(fv2[0],fv2[1]));
            line2=Segment2f(Point2f(fv2[0],fv2[1]),Point2f(fv0[0],fv0[1]));
            p1_2d=Point2f(p1[0],p1[1]);
            p2_2d=Point2f(p2[0],p2[1]);
            seg=Segment2f(p1_2d,p2_2d);
            break;
        }
    }
    bool int_found=false;
    int edge=-1;
    Point2f tmp;
    if(SegmentSegmentIntersection(line0,seg,tmp)){
       int_found=true;
       int_p=tmp;
       edge=0;
    }
    if(SegmentSegmentIntersection(line1,seg,tmp)){
        if(int_found){

            if(Segment2f(tmp,p2_2d).Length()<Segment2f(int_p,p2_2d).Length()){
            int_found=true;
            int_p=tmp;
            edge=1;
            }
        }else{
            int_found=true;
            int_p=tmp;
            edge=1;
        }
    }
    if(SegmentSegmentIntersection(line2,seg,tmp)){
        if(int_found){
            if(Segment2f(tmp,p2_2d).Length()<Segment2f(int_p,p2_2d).Length()){
            int_found=true;
            int_p=tmp;
            edge=2;
            }
        }else{
            int_found=true;
            int_p=tmp;
            edge=2;
        }
    }
    if(int_found){
        new_f=f->FFp(edge);
        switch(max_c){
            case 0:{
                int_point[0]=(n[0]*fv0[0]-n[1]*(int_p[0]-fv0[1])-n[2]*(int_p[1]-fv0[2]))/n[0];
                int_point[1]=int_p[0];
                int_point[2]=int_p[1];
                break;
                }
            case 1:{
                int_point[0]=int_p[0];
                int_point[1]=(n[1]*fv0[1]-n[0]*(int_p[0]-fv0[0])-n[2]*(int_p[1]-fv0[2]))/n[1];
                int_point[2]=int_p[1];
                break;
                }
            case 2:{
                int_point[0]=int_p[0];
                int_point[1]=int_p[1];
                int_point[2]=(n[2]*fv0[2]-n[0]*(int_p[0]-fv0[0])-n[1]*(int_p[1]-fv0[1]))/n[2];
                break;
              }
          }
    }
    return int_found;
};

/**
@description generate random baricentric coordinate
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

/*
@description Compute the Normal Dust Amount Function per face of a Mesh m

@param m MeshModel
@param u CoordType dust direction
@param k float
@param s float
*/
bool ComputeNormalDustAmount(MeshModel* m,CMeshO::CoordType u,float k,float s){

    //Verify if FaceQualty is enabled




    CMeshO::FaceIterator fi;
    float d;
    for(fi=m->cm.face.begin();fi!=m->cm.face.end();++fi){
        d=k/s+(1+k/s)*pow(fi->N().dot(u),s);

        fi->Q()=d;
    }
    return true;
};

/*
@description This function compute the Surface Exposure per face of a Mesh m
@param(MeshModel

*/
void ComputeSurfaceExposure(MeshModel* m,int r,int n_ray){
    CMeshO::FaceIterator fi;
    CMeshO::FaceIterator fi0;

    CMeshO::CoordType b_c;
    CMeshO::CoordType bp_i; //Barycentric coordinates
    b_c[0]=0.3f;
    b_c[1]=0.3f;
    b_c[2]=1-(b_c[0]+b_c[1]);
    CMeshO::CoordType p_c;
    float dh=1.2;
    CMeshO::PerFaceAttributeHandle<float> eh=vcg::tri::Allocator<CMeshO>::AddPerFaceAttribute<float>(m->cm,std::string("exposure"));

    float exp=0;
    float distance=0;
    float u=0;
    float v=0;
    float t=0;

    for(fi=m->cm.face.begin();fi!=m->cm.face.end();++fi){
        eh[fi]=1;
    }
/*
    for(fi=m->cm.face.begin();fi!=m->cm.face.end();++fi){
        if(fi->Q()!=0){

        //For every face  get the central point
        p_c[0]=fi->P(0)[0]*b_c[0]+fi->P(1)[0]*b_c[1]+fi->P(2)[0]*b_c[2];
        p_c[1]=fi->P(0)[1]*b_c[0]+fi->P(1)[1]*b_c[1]+fi->P(2)[1]*b_c[2];
        p_c[2]=fi->P(0)[2]*b_c[0]+fi->P(1)[2]*b_c[1]+fi->P(2)[2]*b_c[2];

        //Create a ray with p_c as origin and direction dir
        Ray3<float> ray=Ray3<float>(p_c,fi->N());

        exp=0;
        distance=0;



        for(fi0=m->cm.face.begin();fi0!=m->cm.face.end();++fi0){
            u=0;
            v=0;
            t=0;
            if(fi!=fi0 && IntersectionRayTriangle(ray,fi0->P(0),fi0->P(1),fi0->P(2),t,u,v)){
                if(distance==0 || t<distance ){
                    distance=t;
                    exp=dh/(dh+distance);
                }
            }

          }
        eh[fi]=1-(exp/n_ray);
    }else{
        eh[fi]=0;
     }
    }
*/

};

void CreateDustTexture(MeshModel* m){
    QString textName="dust_texture";
    QString fileName(m->fullName());
    fileName = fileName.left(std::max<int>(fileName.lastIndexOf('\\'),fileName.lastIndexOf('/'))+1).append(textName);
    QFile textFile(fileName);
    QImage img(640, 640, QImage::Format_RGB32);
    img.fill(qRgb(0,0,0)); // black
    img.save(fileName,"jpg");
    m->cm.textures.clear();
    m->cm.textures.push_back(textName.toStdString());

}

bool GenerateDustParticles(MeshModel* m,std::vector<CMeshO::CoordType> &cpv,std::vector<DustParticle <CMeshO> > &dpv,int d,float threshold){

    //Exposure Handler
    CMeshO::PerFaceAttributeHandle<float> eh=vcg::tri::Allocator<CMeshO>::GetPerFaceAttribute<float>(m->cm,std::string("exposure"));

    CMeshO::FaceIterator fi;
    CMeshO::CoordType p;
    cpv.clear();
    dpv.clear();



    /*float a1=1+1*eh[fi];
    if(a1<0){
    a=0;
    }
    if(a1>1){
    a=1;
    }
    if(a1>=0 && a1<=1){
       a=a1;
    }
*/
    for(fi=m->cm.face.begin();fi!=m->cm.face.end();++fi){
        float a=0;
        if(eh[fi]<1) a=0;
        else a=1;
        int n_dust=(int)d*fi->Q()*a;

        if(fi->Q()>threshold){
            for(int i=0;i<n_dust;i++){
               p=RandomBaricentric();
               CMeshO::CoordType n_p;
               n_p[0]=fi->P(0)[0]*p[0]+fi->P(1)[0]*p[1]+fi->P(2)[0]*p[2];
               n_p[1]=fi->P(0)[1]*p[0]+fi->P(1)[1]*p[1]+fi->P(2)[1]*p[2];
               n_p[2]=fi->P(0)[2]*p[0]+fi->P(1)[2]*p[1]+fi->P(2)[2]*p[2];
               cpv.push_back(n_p);
               DustParticle<CMeshO> part;
               part.face=&(*fi);
               part.bar_coord=p;
               dpv.push_back(part);
               }
            }
        }


return true;
};

/*
@description

@param
@param
*/
bool IsOnFace(CMeshO::CoordType &p, CMeshO::FacePointer f){

    CMeshO::CoordType b;
    CMeshO::CoordType p0=f->P(0);
    CMeshO::CoordType p1=f->P(1);
    CMeshO::CoordType p2=f->P(2);

    //x
    double A=p0[0]-p2[0];
    double B=p1[0]-p2[0];
    double C=p2[0]-p[0];
    //y
    double D=p0[1]-p2[1];
    double E=p1[1]-p2[1];
    double F=p2[1]-p[1];
    //z
    double G=p0[2]-p2[2];
    double H=p1[2]-p2[2];
    double I=p2[2]-p[2];

    b[0]=( B*(F+I)-C*(E+H))/(  A*(E+H)-B*(D+G) );
    b[1]=( A*(F+I)-C*(D+G))/( B*(D+G)- A*(E+H) );
    b[2]=1.0-b[0]-b[1];

    if(b[0]<0 || b[1]<0 || b[2]<0) return false;

    return true;
};




#endif // DIRT_UTILS_H
