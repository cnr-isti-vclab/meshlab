/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2008                                                \/)\/    *
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


#include <QtGui>

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>

#include "filter_topo.h"
#include "../edit_topo/edit_topomeshbuilder.h"



FilterTopoPlugin::FilterTopoPlugin() 
{ 
	typeList << FP_RE_TOPO;
  
  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

 QString FilterTopoPlugin::filterName(FilterIDType filterId) 
{
  switch(filterId) {
		case FP_RE_TOPO :  return QString("Create mesh from basic topology"); 
		default : assert(0); 
	}
}

 QString FilterTopoPlugin::filterInfo(FilterIDType filterId)
{
  switch(filterId) {
		case FP_RE_TOPO :  return QString("Creates a new mesh from a basic topology model applied to an high resolution model"); 
		default : assert(0); 
	}
}

//
// Filter interface start up
//
void FilterTopoPlugin::initParameterSet(QAction *action, MeshDocument & md, RichParameterSet & parlst) 
{
	MeshModel *target= md.mm();
	foreach (target, md.meshList) 
		if (target != md.mm())  break;

	doc = &md;

	 switch(ID(action))	 {
		case FP_RE_TOPO :  
			// Iterations editbox
			parlst.addParam(new RichInt(	"it", 
							4,
							"Number of refinement iterations used to build the new mesh", 
							"As higher is this value, as well defined will be the new mesh. Consider that more than 5 iterations may slow down your system"));
			// Distance editbox
			parlst.addParam(new RichAbsPerc(	"dist", 0.3f, 0.01f, 0.99f, 
								"Incremental distance %", 
								"This param represents the % distance for the local search algorithm used for new vertices allocation. Generally, 0.25-0.30 is a good value"));
			// Topology mesh list
			parlst.addParam(new RichMesh(	"userMesh", md.mm(), 
							"Topology mesh",
							"This mesh will be used as the new base topology, and will be replaced by the new mesh"));
			// Original mesh list
			parlst.addParam(new RichMesh( "inMesh", target,
							"Original mesh",
							"The new mesh will be elaborated using this model"));
		break;
											
		default : assert(0); 
	}
}

//
// Apply filter
//
bool FilterTopoPlugin::applyFilter(QAction *filter, MeshModel &m, RichParameterSet & par, vcg::CallBackPos *cb)
{
	// To run the retopology algorithm an istance of RetopoMeshBuilder is needed
	RetopMeshBuilder rm;

	// Load topology mesh
	MeshModel *userMesh = par.getMesh("userMesh");
	// Load (input) original mesh
	MeshModel *inMesh = par.getMesh("inMesh");
	// Load iterations value
	int it = par.getInt("it");
	// Load distance value
	float dist = par.getAbsPerc("dist");

	// Destination meshmodel: retopology mesh will replace flat topology mesh
	MeshModel * outM = par.getMesh("userMesh");

	// Prepare mesh
	inMesh->updateDataMask(MeshModel::MM_FACEMARK);
	tri::UpdateNormals<CMeshO>::PerFaceNormalized(inMesh->cm);
	tri::UpdateFlags<CMeshO>::FaceProjection(inMesh->cm);

	// Init the retopology builder with original input mesh and distance value
	rm.init(inMesh, dist);
	// Apply the algorithm
	return rm.applyTopoMesh(*userMesh, *inMesh, it, dist, *outM);
}

Q_EXPORT_PLUGIN(FilterTopoPlugin)
