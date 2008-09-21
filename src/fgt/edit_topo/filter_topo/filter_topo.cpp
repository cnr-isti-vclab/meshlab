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

const QString FilterTopoPlugin::filterName(FilterIDType filterId) 
{
  switch(filterId) {
		case FP_RE_TOPO :  return QString("Create mesh from basic topology"); 
		default : assert(0); 
	}
}

const QString FilterTopoPlugin::filterInfo(FilterIDType filterId)
{
  switch(filterId) {
		case FP_RE_TOPO :  return QString("Creates a new mesh from a basic topology model applyed to an high resolution model"); 
		default : assert(0); 
	}
}

const PluginInfo &FilterTopoPlugin::pluginInfo()
{
   static PluginInfo ai;
   ai.Date=tr(__DATE__);
	 ai.Version = tr("1.0");
	 ai.Author = ("daniele bonetta");
   return ai;
 }

void FilterTopoPlugin::initParameterSet(QAction *action, MeshDocument & md, FilterParameterSet & parlst) 
{
	MeshModel *target= md.mm();
	foreach (target, md.meshList) 
		if (target != md.mm())  break;

	doc = &md;

	 switch(ID(action))	 {
		case FP_RE_TOPO :  

			parlst.addInt(	"it", 
							4,
							"Number of refinement iterations used to build the new mesh", 
							"As higher is this value, as well defined will be the new mesh. Consider that more than 5 iterations may slow down your system");

			parlst.addAbsPerc(	"dist", 0.3f, 0.01f, 0.99f, 
								"Incremental distance %", 
								"This param represents the % distance for the local search algorithm used for new vertices allocation. Generally, 0.25-0.30 is a good value");

			parlst.addMesh(	"userMesh", md.mm(), 
							"New topology mesh",
							"This mesh will be used as the new base topology");

			parlst.addMesh( "inMesh", target,
							"Original mesh",
							"The new mesh will be elaborated using this model");
		break;
											
		default : assert(0); 
	}
}

bool FilterTopoPlugin::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & par, vcg::CallBackPos *cb)
{

	MeshModel *userMesh = par.getMesh("userMesh");
	MeshModel *inMesh = par.getMesh("inMesh");
	int it = par.getInt("it");
	float dist = par.getAbsPerc("dist");

	MeshModel * outM = par.getMesh("userMesh"); // = *doc->addNewMesh("Retopped.ply");

	bool r = RetopMeshBuilder::applyTopoMesh(*userMesh, *inMesh, it, dist, *outM);
/*
	if(r)
	{
		doc->meshList.push_back
	} */

	return true;
}

Q_EXPORT_PLUGIN(FilterTopoPlugin)
