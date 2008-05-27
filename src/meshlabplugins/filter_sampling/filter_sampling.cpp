/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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
/****************************************************************************
  History
$Log: samplefilter.cpp,v $
****************************************************************************/

#include <QtGui>

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/position.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/point_sampling.h>

#include "filter_sampling.h"

using namespace vcg;
using namespace std;
// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

SampleFilterDocPlugin::SampleFilterDocPlugin() 
{ 
	typeList << FP_GENERATE_SAMPLING;
  
  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
const QString SampleFilterDocPlugin::filterName(FilterIDType filterId) 
{
  switch(filterId) {
		case FP_GENERATE_SAMPLING :  return QString("Point Sampling"); 
		default : assert(0); 
	}
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
const QString SampleFilterDocPlugin::filterInfo(FilterIDType filterId)
{
  switch(filterId) {
		case FP_GENERATE_SAMPLING :  return QString("create a new layer populated with a point sampling of the current mesh"); 
		default : assert(0); 
	}
}

const PluginInfo &SampleFilterDocPlugin::pluginInfo()
{
   static PluginInfo ai;
   ai.Date=tr(__DATE__);
	 ai.Version = tr("1.0");
	 ai.Author = ("Paolo Cignoni");
   return ai;
 }

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parmeter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void SampleFilterDocPlugin::initParameterSet(QAction *action, MeshDocument & m, FilterParameterSet & parlst) 
//void ExtraSamplePlugin::initParList(QAction *action, MeshModel &m, FilterParameterSet &parlst)
{
	 switch(ID(action))	 {
		case FP_GENERATE_SAMPLING :  
 		  parlst.addInt ("SampleNum",
											m.mm()->cm.vn,
											"Number of samples",
											"The desired number of samples. It can be smaller or larger than the mesh size, and according to the choosed sampling strategy it will try to adapt.");
											break;
											
		default : assert(0); 
	}
}

class MyPointSampler
{
	public:
	MyPointSampler(CMeshO* _m){m=_m;};
	CMeshO *m;
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
};

bool SampleFilterDocPlugin::applyFilter(QAction *action, MeshDocument &md, FilterParameterSet & par, vcg::CallBackPos *cb)
{
	switch(ID(action))
		{
		case FP_GENERATE_SAMPLING :  
			MeshModel *mm= new MeshModel();	
			MeshModel *curMM= md.mm();				
			md.addMesh(mm); // After Adding a mesh to a MeshDocument the new meshi is the current one 
			MyPointSampler mps(&(mm->cm));
//			tri::SurfaceSampling<CMeshO,MyPointSampler>::AllVertex(curMM->cm,mps);
//			tri::SurfaceSampling<CMeshO,MyPointSampler>::AllEdge(curMM->cm,mps);
			tri::SurfaceSampling<CMeshO,MyPointSampler>::Montecarlo(curMM->cm,mps,100000);
			vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);

			Log(0,"Sampling created a new mesh of %i points",md.mm()->cm.vn);		
			break;
		}
		
	return true;
}

Q_EXPORT_PLUGIN(SampleFilterDocPlugin)
