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
#include <vcg/complex/algorithms/update/color.h>
#include <vcg/complex/algorithms/update/bounding.h>
#include <vcg/complex/algorithms/update/normal.h>
#include <vcg/complex/algorithms/point_sampling.h>
#include <vcg/space/triangle3.h>
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

QString FilterDirt::pluginName() const
{
    return "FilterDirt";
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

void FilterDirt::initParameterList(const QAction* filter,MeshDocument & /*md*/, RichParameterList &par){

    switch(ID(filter)){

    case FP_DIRT:{
        par.addParam(RichPoint3f("dust_dir", Point3m(0, 1, 0), "Direction", "Direction of the dust source"));
        par.addParam(RichInt("nparticles", 3, "max particles x face", "Max Number of Dust Particles to Generate Per Face"));
        par.addParam(RichFloat("slippiness", 1.0f, "s", "The surface slippines(large s means less sticky)"));
        par.addParam(RichFloat("adhesion", 0.2f, "k", "Factor to model the general adhesion"));
        par.addParam(RichBool("draw_texture", false, "Draw Dust", "create a new texture saved in dirt_texture.png"));
        // par.addParam(RichBool("colorize_mesh",false,"Map to Color","Color the mesh with colors based on the movement of the particle"));
        break;
    }
    case FP_CLOUD_MOVEMENT:{
        par.addParam(RichPoint3f("gravity_dir", Point3m(0, -1, 0), "g", "Direction of gravity"));
        par.addParam(RichPoint3f("force_dir", Point3m(0, 0, 0), "force", "Direction of the force acting on the points cloud"));
        par.addParam(RichInt("steps", 1, "s", "Simulation Steps"));
        par.addParam(RichDynamicFloat("adhesion", 1.0f, 0.0f, 1.0f, "adhesion", "Factor to model the general adhesion."));
        par.addParam(RichFloat("velocity", 0, "v", "Initial velocity of the particle"));
        par.addParam(RichFloat("mass", 1, "m", "Mass of the particle"));
        par.addParam(RichBool("colorize_mesh", false, "Map to Color", "Color the mesh with colors based on the movement of the particle"));
		break;
    }
    default:{
        break;
    }
    }
}

int FilterDirt::getRequirements(const QAction * /*action*/)
{
    return MeshModel::MM_FACEFACETOPO | MeshModel::MM_VERTCOLOR |MeshModel::MM_FACECOLOR;
}

bool FilterDirt::applyFilter(const QAction *filter, MeshDocument &md, const RichParameterList &par, vcg::CallBackPos *cb){


    switch(ID(filter)){

    case FP_DIRT:{
        /*Get Parameters*/

        Point3m dir=par.getPoint3m("dust_dir");
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

        vector<Point3m> dust_points;
        prepareMesh(currMM);
        if(cb) (*cb)(10,"Computing Dust Amount...");

        ComputeNormalDustAmount(currMM,dir,k,s);
        if(cb) (*cb)(30,"Computing Mesh Exposure...");

        ComputeSurfaceExposure(currMM,1,1);

        if(cb) (*cb)(50,"Generating Particles...");

		GenerateParticles(currMM, dust_points,/*dust_particles,*/n_p, 0.6f);
        MeshModel* dmm=md.addNewMesh("","dust_mesh",true);
        dmm->cm.Clear();
        tri::Allocator<CMeshO>::AddVertices(dmm->cm,dust_points.size());
        CMeshO::VertexIterator vi;
        vector<Point3m>::iterator dvi=dust_points.begin();
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
        Point3m dir=par.getPoint3m("force_dir");
        Point3m g=par.getPoint3m("gravity_dir");
        Scalarm adhesion =par.getDynamicFloat("adhesion");
        Scalarm l=base_mesh->cm.bbox.Diag()*0.01; //mm()->cm.bbox.Diag();
        Scalarm v=par.getFloat("velocity");
        Scalarm m=par.getFloat("mass");
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

int FilterDirt::postCondition(const QAction *a) const
{
	switch (ID(a)){
	case FP_DIRT: return MeshModel::MM_ALL;
	case FP_CLOUD_MOVEMENT: return MeshModel::MM_ALL;
	default: assert(0);
	}
	return MeshModel::MM_ALL;
}

FilterPluginInterface::FilterClass FilterDirt::getClass(const QAction *filter) const
{
	switch (ID(filter))	{
	case FP_DIRT:return FilterPluginInterface::Sampling;
	case FP_CLOUD_MOVEMENT:return FilterPluginInterface::Remeshing;
	default:assert(0);
	}
	return FilterPluginInterface::Generic;
}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterDirt)
