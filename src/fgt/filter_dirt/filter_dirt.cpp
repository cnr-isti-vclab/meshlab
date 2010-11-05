/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
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

#include <Qt>
#include <QtGui>
#include "filter_dirt.h"
#include "particle.h"
#include "dirt_utils.h"

#include <vcg/space/color4.h>
#include <vcg/math/random_generator.h>
#include <vcg/complex/trimesh/closest.h>
#include <vcg/space/index/spatial_hashing.h>
#include <vcg/complex/trimesh/stat.h>
#include <vcg/complex/trimesh/update/topology.h>
#include <vcg/space/box2.h>
#include <vcg/math/base.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/stat.h>
#include <vcg/complex/trimesh/smooth.h>
#include <vcg/complex/trimesh/update/flag.h>
#include <vcg/complex/trimesh/update/selection.h> 
#include <vcg/complex/trimesh/update/color.h>
#include <vcg/complex/trimesh/update/flag.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/point_sampling.h>
#include <vcg/space/triangle3.h>
#include <vcg/complex/trimesh/allocate.h>
#include <vector>

using namespace std;
using namespace vcg;
using namespace tri;

FilterDirt::FilterDirt()
{

    typeList << FP_DIRT
             <<FP_CLOUD_MOVEMENT;
    
    FilterIDType tt;
    foreach(tt , types())
	actionList << new QAction(filterName(tt), this);
}

QString FilterDirt::filterName(FilterIDType filterId) const
{
    switch (filterId) {
    case FP_DIRT:{
        return QString("Dust Accumulation");
        break;
        }
    case FP_CLOUD_MOVEMENT:
        {
        return QString("Points Cloud Movement");
        break;
        }
    default:{
        assert(0); return QString("error");
        break;
        }
    }
}

QString FilterDirt::filterInfo(FilterIDType filterId) const
{
    switch (filterId) {
    case FP_DIRT:{
            return QString("Simulate dust accumulation over the mesh");
            break;
        }
    case FP_CLOUD_MOVEMENT:{
            return QString("Simulate the movement of a points cloud over a mesh");
            break;
        }
    default:
        assert(0); return QString("error");
        break;
    }
}

void FilterDirt::initParameterSet(QAction* filter,MeshDocument &md, RichParameterSet &par){


    switch(ID(filter)){

    case FP_DIRT:{
            par.addParam(new RichPoint3f("dust_dir",Point3f(0,1,0),"Direction","Direction of the dust source"));
            par.addParam(new RichInt("nparticles",10,"particles","Max Number of Dust Particles to Generate Per Face"));
            par.addParam(new RichFloat("slippiness",1,"s","The surface slippines"));
            par.addParam(new RichFloat("adhesion",0.2,"k","Factor to model the general adhesion"));
            break;

        }
    case FP_CLOUD_MOVEMENT:{
            MeshModel* m=md.getMesh(0);
            m->cm.bbox.DimX();
            float perc=0.01;
            float max_value=m->cm.bbox.Dim()[m->cm.bbox.MaxDim()];
            par.addParam(new RichPoint3f("force_dir",Point3f(0,-1,0),"force","Direction of the force acting on the points cloud"));
            par.addParam(new RichAbsPerc("s_length",max_value*perc,0,max_value,"Movement Length",""));
            //par.addParam(new RichPoint3f("velocity",Point3f(0,0,0),"v","Initial velocity of the particle"));
            par.addParam(new RichFloat("velocity",5,"v","Initial velocity of the particle"));
            par.addParam(new RichFloat("mass",1,"m","Mass of the particle"));
            break;
        }
    default:{
        break;
        }
    }
}

int FilterDirt::getRequirements(QAction */*action*/)
{	
    return MeshModel::MM_FACEFACETOPO | MeshModel::MM_VERTCOLOR |MeshModel::MM_FACECOLOR;
}

bool FilterDirt::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet &par, vcg::CallBackPos *cb){


    switch(ID(filter)){

    case FP_DIRT:{
        /*Get Parameters*/
        Point3f dir=par.getPoint3f("dust_dir");
        float s=par.getFloat("slippiness");
        float k=par.getFloat("adhesion");
        int n_p=par.getInt("nparticles");

        MeshModel* currMM=md.mm();

        if (currMM->cm.fn==0) {
                errorMessage = "This filter requires a mesh with some faces, it does not work on PointSet";
                return false;
        }

        vector<Point3f> dust_points;
        vector<Particle<CMeshO> > dust_particles;

        prepareMesh(currMM);

        tri::UpdateFlags<CMeshO>::FaceClear(currMM->cm);

        ComputeNormalDustAmount(currMM,dir,k,s);
        ComputeSurfaceExposure(currMM,1,1);
        GenerateParticles(currMM,dust_points,dust_particles,n_p,0.6);

        tri::UpdateFlags<CMeshO>::FaceClear(currMM->cm);

        MeshModel* dmm=md.addNewMesh("dust_mesh");
        dmm->cm.Clear();

        tri::Allocator<CMeshO>::AddVertices(dmm->cm,dust_points.size());
        CMeshO::PerVertexAttributeHandle<Particle<CMeshO> > ph= tri::Allocator<CMeshO>::AddPerVertexAttribute<Particle<CMeshO> > (dmm->cm,std::string("ParticleInfo"));
        CMeshO::VertexIterator vi;
        vector<Point3f>::iterator dvi=dust_points.begin();
        vector<Point3f>::iterator dvIter;
        std::vector< Particle<CMeshO> >::iterator dpi=dust_particles.begin();

        for(vi=dmm->cm.vert.begin();vi!=dmm->cm.vert.end();++vi){
            (*vi).P()=(*dvi);
            ph[vi]=(*dpi);
            ++dvi;
            ++dpi;
        }

        break;
        }
    case FP_CLOUD_MOVEMENT:{
            if(md.size()!=2){
                errorMessage="This filter requires two mesh";
                return false;
            }

        MeshModel* base_mesh=md.getMesh(0);
        if(base_mesh->cm.fn==0){
            errorMessage="The filter requires that the first mesh has some faces";
            return false;
        }

        MeshModel* cloud_mesh=md.getMesh(1);
        if(cloud_mesh->cm.fn!=0){
            errorMessage="The filter requires that the second mesh is a Point Set";
            return false;
        }


        Point3f dir=par.getPoint3f("force_dir");
        float l =par.getAbsPerc("s_length");
        Point3f force=dir;
        float v=par.getFloat("velocity");
        //Point3f v=par.getPoint3f("velocity");
        float m=par.getFloat("mass");


        float delta_t=1;

        if(!HasPerVertexAttribute(cloud_mesh->cm,"ParticleInfo")){
            prepareMesh(base_mesh);
            //Associate every point to a mesh and a Particle to every point
            associateParticles(base_mesh,cloud_mesh,m,v);
        }

        float default_time=1;
        CMeshO::PerVertexAttributeHandle<Particle<CMeshO> > ph = Allocator<CMeshO>::GetPerVertexAttribute<Particle<CMeshO> >(cloud_mesh->cm,"ParticleInfo");
        CMeshO::VertexIterator vi;
        CMeshO::FacePointer current_face;
        float velocity;
        float mass;
        CMeshO::FacePointer new_face;
        CMeshO::CoordType   new_pos;
        CMeshO::CoordType int_pos;
        float t;
        for(vi=cloud_mesh->cm.vert.begin();vi!=cloud_mesh->cm.vert.end();++vi){
            t=default_time;
            new_face=0;
            current_face=ph[vi].face;
            velocity=ph[vi].vel;
            mass=ph[vi].mass;
            new_pos=StepForward(vi->P(),velocity,mass,current_face,force,l,t);
            Point3f acc=ComputeAcceleration(mass,current_face,force);
            while(!IsOnFace(new_pos,current_face)){


              bool int_found=ComputeIntersection(vi->P(),new_pos,current_face,int_pos,new_face);
              //  t=t-getElapsedTime(vi->P(),int_pos,velocity,acc);

                vi->P()=int_pos;
                //new_pos=int_pos;
                //Debugging
                current_face->C()=Color4b::Green;
                new_face->C()=Color4b::Red;
                //vi->P()=int_pos;
                //vi->P()=int_pos;
                //ph[vi].face=new_face;
                ph[vi].face=new_face;
                current_face=new_face;
                ph[vi].vel=ph[vi].vel/2;
                velocity=ph[vi].vel;
                t=t/2;
                if(t<0.05 || velocity<0.05){
                    new_pos=int_pos;
                }else{
                    new_pos=StepForward(vi->P(),velocity,mass,current_face,force,l,t);
                }
            }
            //ph[vi].velocity=UpdateVelocity(new_pos,vi->P(),velocity,mass,current_face,force);
            //ph[vi].velocity=ComputeVelocity(velocity,acc,t);
            vi->P()=new_pos;


            }

        break;
        }
    default:{
        break;
        }
    }


  return true;
}//End applyFilter

/*
        CMeshO::PerVertexAttributeHandle<Particle<CMeshO> > ph = Allocator<CMeshO>::GetPerVertexAttribute<Particle<CMeshO> >(cloud_mesh->cm,"ParticleInfo");


        CMeshO::FacePointer new_face;
        for(vi=cloud_mesh->cm.vert.begin();vi!=cloud_mesh->cm.vert.end();++vi){

        float t=delta_t;
        CMeshO::CoordType new_pos=StepForward(vi->P(),ph[vi].velocity,ph[vi].mass,ph[vi].face,force,t);


        while(!IsOnFace(new_pos,ph[vi].face )){
            CMeshO::CoordType int_point; //Intersection Point
            ComputeIntersection(vi->P(),new_pos,ph[vi].face,int_point,new_face);
            Point3f new_vel=UpdateVelocity(int_point,vi->P(),ph[vi].velocity,ph[vi].mass,ph[vi].face,force);
            t=t-getElapsedTime(ph[vi].velocity,new_vel,ph[vi].face,ph[vi].mass,force);
            if(t<0) t=0;
            ph[vi].velocity=new_vel;
            vi->P()=int_point;
            ph[vi].face=new_face;

            new_pos=StepForward(vi->P(),ph[vi].velocity,ph[vi].mass,ph[vi].face,force,t);
        }
        ph[vi].velocity=UpdateVelocity(new_pos,vi->P(),ph[vi].velocity,ph[vi].mass,ph[vi].face,force);
        vi->P()=new_pos;
        }
         break;
        }

        /*
        int counter = 0;
        while(!IsOnFace(new_pos,ph[vi].face) && counter <5){
            counter++;
            ComputeIntersection(vi->P(),new_pos,ph[vi].face,int_point,new_face);

           //Segment3f dist=Segment3f(vi->P(),new_pos);
           //Segment3f dist_int=Segment3f(vi->P(),int_point);
           //float res_t=1-getElapsedTime(int_point,vi->P(),pi[vi].mass,pi[vi].velocity,pi[vi].face,force);
           //pi[vi].velocity=UpdateVelocity(pi[vi].velocity,pi[vi].mass,pi[vi].face,force,res_t);
            vi->P()=int_point;
            ph[vi].face=new_face;
            new_pos=StepForward(vi->P(),ph[vi].velocity,ph[vi].mass,ph[vi].face,force,1);
           }
        vi->P()=new_pos;
        //pi[vi].velocity=UpdateVelocity(pi[vi].velocity,pi[vi].mass,pi[vi].face,force,1);

        }
*/
/*

    if(md.size()>=2){//This is temporary, just to try new steps of simulation


        MeshModel* dmesh=md.getMesh("Dust Mesh");
        Point3f dir;
        Point3f new_bar_coords;
        dir[0]=0;
        dir[1]=-1;
        dir[2]=0;
        if(dmesh!=0){
                   CMeshO::VertexIterator vi;//= dmesh->cm.vert.begin();
                   CMeshO::PerVertexAttributeHandle<Particle<CMeshO> > pi = tri::Allocator<CMeshO>::GetPerVertexAttribute<Particle<CMeshO> >(dmesh->cm,"ParticleInfo");
                   CMeshO::CoordType new_pos;
                   CMeshO::CoordType int_p;
                   CMeshO::FacePointer old_face;
                   CMeshO::FacePointer new_face;
                   bool stop_movement=false;
                   for(vi=dmesh->cm.vert.begin();vi!=dmesh->cm.vert.end();++vi){
                       stop_movement=false;
                       //First Movement
                       old_face=pi[vi].face;
                       new_pos=StepForward((*vi).P(),pi[vi].speed,pi[vi].mass,pi[vi].face,dir);
                       while(!stop_movement){

                           if(!IsOnFace(new_pos,pi[vi].face)){
                            ComputeIntersection((*vi).P(),new_pos,pi[vi].face,int_p,new_face);
                            pi[vi].face->Q()=pi[vi].face->Q()+1;
                            pi[vi].face=new_face;
                            Segment3f s1=Segment3f((*vi).P(),new_pos);
                            Segment3f s2=Segment3f((*vi).P(),int_p);
                            new_pos=int_p;
                            (*vi).P()=new_pos;
                            //if(old_face!=new_face)
                            //{
                                float t=s2.Length()/s1.Length();
                                //Updating quality for face
                                old_face->Q()=old_face->Q()+1;
                                //Updating veloctiy of particle
                                //pi[vi].speed=UpdateVelocity(pi[vi].speed,pi[vi].mass,old_face,dir,(1-t));
                                //New Movement
                                //old_face=new_face;
                                if(t>0.05)new_pos=StepForward((*vi).P(),pi[vi].speed,pi[vi].mass,pi[vi].face,dir,t);
                                else stop_movement=true;
                            //}
                            //else stop_movement=true;
                        }else{
                          stop_movement=true;
                          pi[vi].face->Q()=pi[vi].face->Q()+1;
                          (*vi).P()=new_pos;
                        }



                    }
              }
               }

    if(par.getBool("mtc"))DrawDirt(md.getMesh(0));
    }else{
        //First Application
        Point3f dir;
        dir[0]=par.getFloat("dir_x");
        dir[1]=par.getFloat("dir_y");
        dir[2]=par.getFloat("dir_z");

        vector<Point3f> dustVertexVec;
        vector<Particle<CMeshO> > particleVec;
        MeshModel* currMM=md.mm();

        if (currMM->cm.fn==0) {
                errorMessage = "This filter requires a mesh with some faces,<br> it does not work on PointSet";
                return false;
        }
        std::string func_d = "ny";

        //Initialize quality for face
        CMeshO::FaceIterator fIter;
        for(fIter=currMM->cm.face.begin();fIter!=currMM->cm.face.end();++fIter){
            fIter->Q()=0;
        }


        ComputeNormalDustAmount(currMM,dir,par.getFloat("adhesion"),par.getFloat("slippiness"));
        ComputeSurfaceExposure(currMM,1,1);
        GenerateParticles(currMM,dustVertexVec,particleVec,par.getInt("nparticles"),0.6);

        //dmm-> Dust Mesh Model
        MeshModel* dmm=md.addNewMesh("Dust Mesh");

        dmm->cm.Clear();
        tri::Allocator<CMeshO>::AddVertices(dmm->cm,dustVertexVec.size());

        CMeshO::PerVertexAttributeHandle<Particle<CMeshO> > ph= tri::Allocator<CMeshO>::AddPerVertexAttribute<Particle<CMeshO> > (dmm->cm,std::string("ParticleInfo"));

        CMeshO::VertexIterator vIter=dmm->cm.vert.begin();
        vector<Point3f>::iterator dvIter;
        std::vector< Particle<CMeshO> >::iterator dpIter=particleVec.begin();

        for(dvIter=dustVertexVec.begin();dvIter!=dustVertexVec.end();++dvIter){
            (*vIter).P()=CMeshO::CoordType ((*dvIter)[0],(*dvIter)[1],(*dvIter)[2]);
            ph[vIter]=(*dpIter);
            ++dpIter;
            ++vIter;
        }
    }
    return true;
*/


MeshFilterInterface::FilterClass FilterDirt::getClass(QAction *filter)
{
    switch(ID(filter)){
    case FP_DIRT:return MeshFilterInterface::Sampling;
    case FP_CLOUD_MOVEMENT:return MeshFilterInterface::VertexColoring;
    default:assert(0);
    }
}




Q_EXPORT_PLUGIN(FilterDirt)
