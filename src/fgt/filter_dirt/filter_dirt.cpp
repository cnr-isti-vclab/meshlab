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
#include "dustparticle.h"
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
#include <muParser.h>

using namespace std;
using namespace vcg;
using namespace mu;


FilterDirt::FilterDirt()
{

    typeList << FP_DIRT;
    
    FilterIDType tt;
    foreach(tt , types())
	actionList << new QAction(filterName(tt), this);
}

QString FilterDirt::filterName(FilterIDType filterId) const
{
    switch (filterId) {
    case FP_DIRT:
        return QString("Dust Accumulation");
        break;
    default:
        assert(0); return QString("error");
        break;
    }
}

QString FilterDirt::filterInfo(FilterIDType filterId) const
{
    switch (filterId) {
    case FP_DIRT:
        return QString("Simulate dust accumulation over the mesh");
        break;
    default:
        assert(0); return QString("error");
        break;
    }
}

void FilterDirt::initParameterSet(QAction* filter,MeshDocument &md, RichParameterSet &par){



    par.addParam(new RichFloat("dir_x",0.0,"x","x direction of dust source"));
    par.addParam(new RichFloat("dir_y",0.0,"y","y direction of dust source"));
    par.addParam(new RichFloat("dir_z",0.0,"z","z direction of dust source"));
    par.addParam(new RichInt("nparticles",10,"particles","Max Number of Dust Particles to Generate Per Face"));
    par.addParam(new RichFloat("slippiness",1,"s","The surface slippines"));
    par.addParam(new RichFloat("adhesion",0.2,"k","Factor to model the general adhesion"));
    par.addParam(new RichBool("mtc",false,"Map To Colors",""));
    par.addParam(new RichBool("gt",false,"Generate Texture",""));
    //par.addParam(new RichDynamicFloat("step",0,0,10,"Steps","Steps of simulation"));
    return;
}

int FilterDirt::getRequirements(QAction */*action*/)
{	
    return MeshModel::MM_FACEFACETOPO | MeshModel::MM_VERTCOLOR |MeshModel::MM_FACECOLOR;
}

bool FilterDirt::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet &par, vcg::CallBackPos *cb)
{


    if(md.size()>=2){//This is temporary, just to try new steps of simulation


        MeshModel* dmesh=md.getMesh("Dust Mesh");
        Point3f dir;
        Point3f new_bar_coords;
        dir[0]=0;
        dir[1]=-1;
        dir[2]=0;
        if(dmesh!=0){
                   CMeshO::VertexIterator vi;//= dmesh->cm.vert.begin();
                   CMeshO::PerVertexAttributeHandle<DustParticle<CMeshO> > pi = tri::Allocator<CMeshO>::GetPerVertexAttribute<DustParticle<CMeshO> >(dmesh->cm,"ParticleInfo");
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
                            pi[vi].face=new_face;
                            Segment3f s1=Segment3f((*vi).P(),new_pos);
                            Segment3f s2=Segment3f((*vi).P(),int_p);
                            new_pos=int_p;
                            (*vi).P()=new_pos;
                            if(old_face!=new_face)
                            {
                                float t=s2.Length()/s1.Length();
                                //Updating quality for face
                                old_face->Q()=old_face->Q()+1;
                                //Updating veloctiy of particle
                                //pi[vi].speed=UpdateVelocity(pi[vi].speed,pi[vi].mass,old_face,dir,(1-t));
                                //New Movement
                                old_face=new_face;
                                new_pos=StepForward((*vi).P(),pi[vi].speed,pi[vi].mass,pi[vi].face,dir,t);
                            }
                            else stop_movement=true;
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
        vector<DustParticle<CMeshO> > dustParticleVec;
        MeshModel* currMM=md.mm();

        if (currMM->cm.fn==0) {
                errorMessage = "This filter requires a mesh with some faces,<br> it does not work on PointSet";
                return false;
        }
        std::string func_d = "ny";

        currMM->updateDataMask(MeshModel::MM_FACEFACETOPO);
        currMM->updateDataMask(MeshModel::MM_FACEMARK);
        currMM->updateDataMask(MeshModel::MM_FACECOLOR);
        currMM->updateDataMask(MeshModel::MM_VERTQUALITY);
        currMM->updateDataMask(MeshModel::MM_FACEQUALITY);
        //currMM->updateDataMask(MeshModel::MM_WEDGTEXCOORD);
        tri::UnMarkAll(currMM->cm);

        //clean Mesh
        tri::Allocator<CMeshO>::CompactFaceVector(currMM->cm);
        tri::Clean<CMeshO>::RemoveUnreferencedVertex(currMM->cm);
        tri::Clean<CMeshO>::RemoveDuplicateVertex(currMM->cm);
        tri::Allocator<CMeshO>::CompactVertexVector(currMM->cm);

        //clean flags
        tri::UpdateFlags<CMeshO>::FaceClear(currMM->cm);

        //update Mesh
        tri::UpdateTopology<CMeshO>::FaceFace(currMM->cm);
        tri::UpdateNormals<CMeshO>::PerFaceNormalized(currMM->cm);
        tri::UpdateFlags<CMeshO>::FaceProjection(currMM->cm);

        //Initialize quality for face
        /*CMeshO::FaceIterator fIter;
        for(fIter=currMM->cm.face.begin();fIter!=currMM->cm.face.end();++fIter){
            fIter->Q()=0;
        }*/

        ResetFaceQuality(currMM);
        ComputeNormalDustAmount(currMM,dir,par.getFloat("adhesion"),par.getFloat("slippiness"));
        ComputeSurfaceExposure(currMM,1,1);
        GenerateDustParticles(currMM,dustVertexVec,dustParticleVec,par.getInt("nparticles"),0.6);
        ResetFaceQuality(currMM);
        //dmm-> Dust Mesh Model
        MeshModel* dmm=md.addNewMesh("Dust Mesh");

        dmm->cm.Clear();
        tri::Allocator<CMeshO>::AddVertices(dmm->cm,dustVertexVec.size());

        CMeshO::PerVertexAttributeHandle<DustParticle<CMeshO> > ph= tri::Allocator<CMeshO>::AddPerVertexAttribute<DustParticle<CMeshO> > (dmm->cm,std::string("ParticleInfo"));

        CMeshO::VertexIterator vIter=dmm->cm.vert.begin();
        vector<Point3f>::iterator dvIter;
        std::vector< DustParticle<CMeshO> >::iterator dpIter=dustParticleVec.begin();

        for(dvIter=dustVertexVec.begin();dvIter!=dustVertexVec.end();++dvIter){
            (*vIter).P()=CMeshO::CoordType ((*dvIter)[0],(*dvIter)[1],(*dvIter)[2]);
            ph[vIter]=(*dpIter);
            ++dpIter;
            ++vIter;
        }
    }
    return true;
}

MeshFilterInterface::FilterClass FilterDirt::getClass(QAction *)
{
    return MeshFilterInterface::VertexColoring;
}




Q_EXPORT_PLUGIN(FilterDirt)
