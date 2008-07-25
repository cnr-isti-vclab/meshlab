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
//#include <time.h>


#include <vcg/math/base.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/stat.h>
#include <vcg/complex/trimesh/smooth.h>
#include <vcg/complex/trimesh/update/flag.h>
#include <vcg/complex/trimesh/update/selection.h> 
#include <vcg/complex/trimesh/update/color.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/point_sampling.h>
#include <vcg/space/triangle3.h>


using namespace std;
using namespace vcg;

class BaseSampler
{
public:
	BaseSampler(CMeshO* _m){m=_m; uvSpaceFlag = false;};
	CMeshO *m;
	bool uvSpaceFlag;
	void AddVert(const CMeshO::VertexType &p) 
	{
		tri::Allocator<CMeshO>::AddVertices(*m,1);
		m->vert.back().ImportLocal(p);
	}
	
	void AddFace(const CMeshO::FaceType &f, CMeshO::CoordType p) 
	{
		tri::Allocator<CMeshO>::AddVertices(*m,1);
		m->vert.back().P() = f.P(0)*p[0] + f.P(1)*p[1] +f.P(2)*p[2];
		m->vert.back().N() = f.V(0)->N()*p[0] + f.V(1)->N()*p[1] +f.V(2)->N()*p[2];
	}
	
	void AddTextureSample(const CMeshO::FaceType &f, const CMeshO::CoordType &p, const Point2i &tp)
	{
		tri::Allocator<CMeshO>::AddVertices(*m,1);
		
		if(uvSpaceFlag) m->vert.back().P() = Point3f(float(tp[0]),float(tp[1]),0); 
		else m->vert.back().P() = f.P(0)*p[0] + f.P(1)*p[1] +f.P(2)*p[2];
		m->vert.back().N() = f.V(0)->N()*p[0] + f.V(1)->N()*p[1] +f.V(2)->N()*p[2];
	}
}; // end class BaseSampler

FilterDirt::FilterDirt(): defaultGammaTon(1000)
{
	
    typeList << 
    FP_DIRT;
    
	FilterIDType tt;
	foreach(tt , types())
	actionList << new QAction(filterName(tt), this);
}

const QString FilterDirt::filterName(FilterIDType filterId) 
{
	if(filterId!= FP_DIRT)
	{
		return QString("error!");
	}
	return QString("Dirt Maker");
}
const QString FilterDirt::filterInfo(FilterIDType filterId) 
{
	
	if(filterId!= FP_DIRT)
	{
		return QString("error!");
	}
	return QString("Simulate dirt accumolation over the mesh");
}

const PluginInfo &FilterDirt::pluginInfo() 
{
	static PluginInfo ai; 
	ai.Date=tr("July 2008");
	ai.Version = tr("0.2");
	ai.Author = ("Luca Bartoletti");
	return ai;
}

const int FilterDirt::getRequirements(QAction */*action*/)
{
	
	return MeshModel::MM_FACETOPO | MeshModel::MM_FACECOLOR | MeshModel::MM_FACEMARK;	
}

bool FilterDirt::applyFilter(QAction * /*filter*/, MeshDocument &md, FilterParameterSet & /*par*/, vcg::CallBackPos */*cb*/)
{
	typedef GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType > MetroMeshGrid;
	typedef trimesh::FaceTmark<CMeshO> MarkerFace;
	
	MarkerFace markerFunctor;
	
	vcg::tri::UpdateColor<CMeshO>::FaceConstant(md.mm()->cm,vcg::Color4b::White); //!DEBUG! painting white for debug
	
	MeshModel *curMM= md.mm();				
	MeshModel *mm= md.addNewMesh("Dust gamma-ton"); // After Adding a mesh to a MeshDocument the new mesh is the current one 
	
	BaseSampler mps(&(mm->cm));
	vcg::tri::SurfaceSampling<CMeshO,BaseSampler>::WeightedMontecarlo(curMM->cm, mps, defaultGammaTon);
	vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);
	
	//calculate mean of face's edge segment perimeter
	CMeshO::FaceIterator fi;
	double perimeterSum;
	for(fi=curMM->cm.face.begin();fi!=curMM->cm.face.end();++fi)
	if(!(*fi).IsD())
	{
		perimeterSum += vcg::Perimeter((*fi));
	}
	
	//calculate single gamma-ton step size
	float stepSize = (perimeterSum/3)/curMM->cm.fn;
	
		
	//Main filter cicle	
	CMeshO::VertexIterator vi;	
	CMeshO::FaceType   *nearestF=NULL;
	float maxDist=(*curMM).cm.bbox.Diag()/10, dist;
	MetroMeshGrid   unifGrid;
	vcg::Point3f closestPt;
	
	//setting up grid for space indexing
	unifGrid.Set((*curMM).cm.face.begin(),(*curMM).cm.face.end());
	markerFunctor.SetMesh(&((*curMM).cm));	
	//for (int i=0; i<100; ++i)
	//{	
		for (vi=mm->cm.vert.begin();vi!=mm->cm.vert.end();++vi)
		{
			//get nearest face for every gamma-ton
			vcg::face::PointDistanceBaseFunctor PDistFunct;
			//dist=maxDist;
			vcg::Point3f test = (*vi).P();
			nearestF =  unifGrid.GetClosest(PDistFunct,markerFunctor,(*vi).P(),maxDist,dist,closestPt);
			assert (nearestF)
			if (!(*nearestF).IsD())
				(*nearestF).C() = Color4b::Red;
		}
	//}//end Main filter cicle
	
		
	return true;
}

/*!!! I don't understand why if i did't return MeshFilterInterface::FaceColoring all my function call
 to FaceConstant (et simila) don't take effect over the mesh. I've read from Interface.c comment getClass is
 only to decide where my plugin label is in Plugin submenu of meshlab menubar.*/
const MeshFilterInterface::FilterClass FilterDirt::getClass(QAction *)
{
	return MeshFilterInterface::FaceColoring;
}


Q_EXPORT_PLUGIN(FilterDirt)