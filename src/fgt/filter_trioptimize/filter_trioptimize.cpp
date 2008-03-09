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


//#include <qstringlist.h>
/*#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>*/

#include "filter_trioptimize.h"
#include "curvedgeflip.h"

#include <vcg/complex/trimesh/update/topology.h>
#include <vcg/complex/trimesh/update/normal.h>

// instancing templates
/*template class CurvData<NSMCurvEval>;
template class CurvData<MeanCurvEval>;
template class CurvData<AbsCurvEval>;

typedef CurvData<NSMCurvEval>  NSMCurv;
typedef CurvData<MeanCurvEval> MeanCurv;
typedef CurvData<AbsCurvEval>  AbsCurv;*/

// forward declarations
class NSMCEdgeFlip;
class MeanCEdgeFlip;
class AbsCEdgeFlip;

class NSMCEdgeFlip : public vcg::tri::CurvEdgeFlip<CMeshO, NSMCEdgeFlip, NSMCurvEval >
{
public:
	NSMCEdgeFlip(PosType pos, int mark) : 
		vcg::tri::CurvEdgeFlip<CMeshO, NSMCEdgeFlip, NSMCurvEval >(pos, mark) {}
};

class MeanCEdgeFlip : public vcg::tri::CurvEdgeFlip<CMeshO, MeanCEdgeFlip, MeanCurvEval >
{
public:
	MeanCEdgeFlip(PosType pos, int mark) : 
		vcg::tri::CurvEdgeFlip<CMeshO, MeanCEdgeFlip, MeanCurvEval >(pos, mark) {}
};

class AbsCEdgeFlip : public vcg::tri::CurvEdgeFlip<CMeshO, AbsCEdgeFlip, AbsCurvEval >
{
public:
	AbsCEdgeFlip(PosType pos, int mark) : 
		vcg::tri::CurvEdgeFlip<CMeshO, AbsCEdgeFlip, AbsCurvEval >(pos, mark) {}
};

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
		case FP_EDGE_FLIP:  return QString("Edge flipping optimization");
		default : assert(0); 
	}
}

const int TriOptimizePlugin::getRequirements(QAction *)
{
	return (MeshModel::MM_FACETOPO |
	         MeshModel::MM_VERTFACETOPO |
	         MeshModel::MM_BORDERFLAG);
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
	switch(ID(action)) {
		case FP_EDGE_FLIP: {
			parlst.addBool(
			        "selection",
			        false,
			        "Update selection",
			        "Apply edge flip optimization on selected faces"
			        );
			
			QStringList metrics;
			metrics.push_back("Mean curvature");
			metrics.push_back("Normalized squared mean curvature");
			metrics.push_back("Absolute curvature");
			parlst.addEnum(
			        "metric",
			        0,
			        metrics,
			        tr("Metric:"),
			        tr("various ways to compute surface curvature on vertexes"));
			break;
		}
		default : assert(0); 
	}
}

// The Real Core Function doing the actual mesh processing.
// Run mesh optimization
bool TriOptimizePlugin::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & par, vcg::CallBackPos *cb)
{
	// to fix topology relations
	// TODO: make this as optional
	vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
	vcg::tri::UpdateTopology<CMeshO>::VertexFace(m.cm);
	vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromFF(m.cm);
	//vcg::tri::UpdateFlags<CMeshO>::VertexBorderFromFace(m.cm);
	
	// temporary test
	vcg::tri::UpdateTopology<CMeshO>::TestFaceFace(m.cm);
	vcg::tri::UpdateTopology<CMeshO>::TestVertexFace(m.cm);
	
	
	
	vcg::LocalOptimization<CMeshO> optimization(m.cm);
	
	int metric = par.getEnum("metric");
	switch(metric) {
		case 0: optimization.Init<MeanCEdgeFlip>(); break;
		case 1: optimization.Init<NSMCEdgeFlip>();  break;
		case 2: optimization.Init<AbsCEdgeFlip>();  break;
	}
	
	// stop when flips become harmful:
	// != 0.0f to avoid same flips in every run
	 // TODO: set a better limit
	optimization.SetTargetMetric(-std::numeric_limits<float>::epsilon());
	optimization.DoOptimization();
	//optimization.Finalize<CurvEdgeFlip>();
	
	
	vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
	return true;
}

Q_EXPORT_PLUGIN(TriOptimizePlugin)
