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

/*#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>*/

#include "filter_trioptimize.h"
#include "curvedgeflip.h"

#include <vcg/complex/trimesh/update/normal.h>


// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to
//    your filtering actions you can do here by construction the QActions accordingly
TriOptimizePlugin::TriOptimizePlugin() 
{ 
	typeList << FP_EDGE_FLIP;
	
	foreach(FilterIDType tt , types())
		actionList << new QAction(filterName(tt), this);
}


// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
const QString TriOptimizePlugin::filterName(FilterIDType filterId) 
{
	switch(filterId) {
		case FP_EDGE_FLIP:    return QString("Edge flipping optimization");
		default : assert(0); 
	}
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
const QString TriOptimizePlugin::filterInfo(FilterIDType filterId)
{
	switch(filterId) {
		case FP_EDGE_FLIP:
			return QString("Mesh optimization by edge flipping, which improves triangles quality");
		default : assert(0); 
	}
}

const PluginInfo &TriOptimizePlugin::pluginInfo()
{
	static PluginInfo ai;
	ai.Date = tr(__DATE__);
	ai.Version = tr("1.0");
	ai.Author = ("Michele Onnis");
	return ai;
}

// This function define the needed parameters for each filter.
// Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parmeter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void TriOptimizePlugin::initParameterSet(QAction *action,MeshModel &m, FilterParameterSet & parlst)
{
	switch(ID(action))	 {
		case FP_EDGE_FLIP:
			/*parlst.addBool (
				"UpdateNormals",
				true,
				"Recompute normals",
				"Toggle the recomputation of the normals after the random displacement.\n\n"
				"If disabled the face normals will remains unchanged resulting in a visually pleasant effect."
			);*/
			break;
		default : assert(0); 
	}
}

// The Real Core Function doing the actual mesh processing.
// Run mesh optimization
bool TriOptimizePlugin::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & par, vcg::CallBackPos *cb)
{
	vcg::LocalOptimization<CMeshO> optimization(m.cm);
	//cb(1,"Initializing simplification");
	optimization.Init<CurvEdgeFlip>();
	
	// stop when flips become harmful:
	// != 0.0f to avoid same flips in every run 
	optimization.SetTargetMetric(-0.001f);
	optimization.DoOptimization();
	//optimization.Finalize<CurvEdgeFlip>();
	vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
  
	return true;
}

Q_EXPORT_PLUGIN(TriOptimizePlugin)
