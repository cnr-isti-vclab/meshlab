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
#include <vcg/complex/algorithms/closest.h>
#include <vcg/space/index/spatial_hashing.h>
#include <vcg/complex/algorithms/stat.h>
#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/space/box2.h>
#include <vcg/math/base.h>
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/stat.h>
#include <vcg/complex/algorithms/smooth.h>
#include <vcg/complex/algorithms/update/flag.h>
#include <vcg/complex/algorithms/update/selection.h> 
#include <vcg/complex/algorithms/update/color.h>
#include <vcg/complex/algorithms/update/flag.h>
#include <vcg/complex/algorithms/update/bounding.h>
#include <vcg/complex/algorithms/update/normal.h>
#include <vcg/complex/algorithms/point_sampling.h>
#include <vcg/space/triangle3.h>
#include <vcg/complex/allocate.h>
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
        return QString("Simulate dust accumulation over the mesh generating a cloud of points lying on the current mesh");
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
        par.addParam(new RichInt("nparticles",3,"max particles x face","Max Number of Dust Particles to Generate Per Face"));
        par.addParam(new RichFloat("slippiness",1,"s","The surface slippines(large s means less sticky)"));
        par.addParam(new RichFloat("adhesion",0.2,"k","Factor to model the general adhesion"));
        par.addParam(new RichBool("draw_texture",false,"Draw Dust","create a new texture saved in dirt_texture.png"));
       // par.addParam(new RichBool("colorize_mesh",false,"Map to Color","Color the mesh with colors based on the movement of the particle"));
        break;
    }
    case FP_CLOUD_MOVEMENT:{
        par.addParam(new RichPoint3f("gravity_dir",Point3f(0,-1,0),"g","Direction of gravity"));
        par.addParam(new RichPoint3f("force_dir",Point3f(0,0,0),"force","Direction of the force acting on the points cloud"));
        par.addParam(new RichInt("steps",1,"s","Simulation Steps"));
        par.addParam(new RichDynamicFloat("adhesion", 1.0f, 0.0f, 1.0f,"adhesion","Factor to model the general adhesion."));
        par.addParam(new RichFloat("velocity",0,"v","Initial velocity of the particle"));
        par.addParam(new RichFloat("mass",1,"m","Mass of the particle"));
        par.addParam(new RichBool("colorize_mesh",false,"Map to Color","Color the mesh with colors based on the movement of the particle"));
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
        bool draw=par.getBool("draw_texture");
        //bool colorize=par.getBool("colorize_mesh");
        int n_p=par.getInt("nparticles");

        MeshModel* currMM=md.mm();

        if (currMM->cm.fn==0) {
            errorMessage = "This filter requires a mesh with some faces, it does not work on PointSet";
            return false;
        }

        if(draw && !tri::HasPerWedgeTexCoord(currMM->cm)){
            errorMessage = "Current Mesh does not have per Wedge Tex Coordinates";
            return false;

        }
        
        vector<Point3f> dust_points;
        prepareMesh(currMM);
        if(cb) (*cb)(10,"Computing Dust Amount...");

        ComputeNormalDustAmount(currMM,dir,k,s);
        if(cb) (*cb)(30,"Computing Mesh Exposure...");

        ComputeSurfaceExposure(currMM,1,1);

        if(cb) (*cb)(50,"Generating Particles...");

        GenerateParticles(currMM,dust_points,/*dust_particles,*/n_p,0.6);

        MeshModel* dmm=md.addNewMesh("","dust_mesh");
        dmm->cm.Clear();
        tri::Allocator<CMeshO>::AddVertices(dmm->cm,dust_points.size());
        CMeshO::VertexIterator vi;
        vector<Point3f>::iterator dvi=dust_points.begin();
        if(cb) (*cb)(70,"Creating cloud Mesh...");
        for(vi=dmm->cm.vert.begin();vi!=dmm->cm.vert.end();++vi){
            vi->P()=(*dvi);
            ++dvi;
        }

        if(draw) DrawDust(currMM,dmm);
        //if(colorize) ColorizeMesh(currMM);


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

        //Get Parameters
        Point3f dir=par.getPoint3f("force_dir");
        Point3f g=par.getPoint3f("gravity_dir");
        float adhesion =par.getDynamicFloat("adhesion");
        float l=base_mesh->cm.bbox.Diag()*0.01; //mm()->cm.bbox.Diag();
        float v=par.getFloat("velocity");
        float m=par.getFloat("mass");
        int s=par.getInt("steps");
        bool colorize=par.getBool("colorize_mesh");
        if(!HasPerVertexAttribute(cloud_mesh->cm,"ParticleInfo")){
            prepareMesh(base_mesh);
            //Associate every point to a mesh and a Particle to every point
            associateParticles(base_mesh,cloud_mesh,m,v,g);
        }

        //Move Cloud Mesh
        float frac=100/s;
        for(int i=0;i<s;i++){
            MoveCloudMeshForward(cloud_mesh,base_mesh,g,dir,l,adhesion,1,1);
            if(cb) (*cb)(i*frac,"Moving...");
        }
        if(colorize) ColorizeMesh(base_mesh);
        break;
    }
    
    default:{
        break;
    }
    
    }


    return true;
}//End applyFilter




int FilterDirt::postCondition( QAction *a) const
{
    switch (ID(a)){
    case FP_DIRT : return MeshModel::MM_UNKNOWN;
    case FP_CLOUD_MOVEMENT : return MeshModel::MM_UNKNOWN;
    default: assert(0);
    }

    return MeshModel::MM_NONE;
}


MeshFilterInterface::FilterClass FilterDirt::getClass(QAction *filter)
{
    switch(ID(filter)){
    case FP_DIRT:return MeshFilterInterface::Sampling;
    case FP_CLOUD_MOVEMENT:return MeshFilterInterface::Remeshing;
    default:assert(0);
    }
}

Q_EXPORT_PLUGIN(FilterDirt)
