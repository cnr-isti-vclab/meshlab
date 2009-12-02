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
#include "dustsampler.h"


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



FilterDirt::FilterDirt()
{
	
    typeList << 
    FP_DIRT;
    
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
    par.addParam(new RichInt("nparticles",1000,"Number of Dust Particles",""));
    return;
 }

 int FilterDirt::getRequirements(QAction */*action*/)
{	
    return MeshModel::MM_FACEFACETOPO | MeshModel::MM_VERTCOLOR;
}

bool FilterDirt::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet &par, vcg::CallBackPos *cb)
{

    vector<Point3f> dustVertexVec;
    vector<DustParticle<CMeshO> > dustParticleVec;
    DustSampler<CMeshO> ts(dustVertexVec,dustParticleVec);
    MeshModel *currMM=md.mm();
    tri::SurfaceSampling<CMeshO,DustSampler<CMeshO> >::Montecarlo(currMM->cm,ts,par.getInt("nparticles"));

    //tri::SurfaceSampling<CMeshO,DustSampler<CMeshO> >::WeightedMontecarlo(currMM->cm,ts,par.getInt("nparticles"));


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

    return true;
}

 MeshFilterInterface::FilterClass FilterDirt::getClass(QAction *)
{
	return MeshFilterInterface::VertexColoring;
}


Q_EXPORT_PLUGIN(FilterDirt)
