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
using namespace mu;



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
    std::string func_d = "y*y";
    currMM->updateDataMask(MeshModel::MM_VERTQUALITY);
    mu::Parser p;
    setPerVertexVariables(p);
    p.SetExpr(func_d);

    CMeshO::VertexIterator vi;

    for(vi=currMM->cm.vert.begin();vi!=currMM->cm.vert.end();++vi)
    {
        setAttributes(vi,currMM->cm);
        try {
            (*vi).Q() = p.Eval();
        } catch(Parser::exception_type &e) {
            errorMessage = e.GetMsg().c_str();
            return false;
        }
    }


    //tri::SurfaceSampling<CMeshO,DustSampler<CMeshO> >::Montecarlo(currMM->cm,ts,par.getInt("nparticles"));

    tri::SurfaceSampling<CMeshO,DustSampler<CMeshO> >::WeightedMontecarlo(currMM->cm,ts,par.getInt("nparticles"));


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

void FilterDirt::setPerVertexVariables(Parser &p)
{
    p.DefineVar("x", &x);
    p.DefineVar("y", &y);
    p.DefineVar("z", &z);
    p.DefineVar("nx", &nx);
    p.DefineVar("ny", &ny);
    p.DefineVar("nz", &nz);
    p.DefineVar("r", &r);
    p.DefineVar("g", &g);
    p.DefineVar("b", &b);
    p.DefineVar("q", &q);
    p.DefineVar("vi",&v);
    p.DefineVar("rad",&rad);

    // define var for user-defined attributes (if any exists)
    // if vector is empty, code won't be executed
    for(int i = 0; i < (int) v_attrNames.size(); i++)
        p.DefineVar(v_attrNames[i],&v_attrValue[i]);
}

void FilterDirt::setAttributes(CMeshO::VertexIterator &vi, CMeshO &m)
{
    x = (*vi).P()[0]; // coord x
    y = (*vi).P()[1]; // coord y
    z = (*vi).P()[2]; // coord z

    nx = (*vi).N()[0]; // normal coord x
    ny = (*vi).N()[1]; // normal coord y
    nz = (*vi).N()[2]; // normal coord z

    r = (*vi).C()[0];  // color R
    g = (*vi).C()[1];  // color G
    b = (*vi).C()[2];  // color B

    q = (*vi).Q();     // quality

    if(tri::HasPerVertexRadius(m)) rad = (*vi).R();
    else rad=0;

    v = vi - m.vert.begin(); // zero based index of current vertex

    // if user-defined attributes exist (vector is not empty)
    //  set variables to explicit value obtained through attribute's handler
    for(int i = 0; i < (int) v_attrValue.size(); i++)
        v_attrValue[i] = vhandlers[i][vi];
}

Q_EXPORT_PLUGIN(FilterDirt)
