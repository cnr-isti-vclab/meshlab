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

#include "laplacianadjust.h"
#include "filter_trioptimize.h"
#include "curvedgeflip.h"

//#include <vcg/space/triangle3.h>

#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/topology.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/selection.h>
//#include <vcg/complex/trimesh/update/curvature.h>
using namespace vcg;

using namespace vcg;

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

/*typedef CurvData<CMeshO::ScalarType> CurvType;

class NSMCEdgeFlip : public vcg::tri::CurvEdgeFlip<CMeshO, NSMCEdgeFlip, NSMCurvEval<CurvType> >
{
public:
	NSMCEdgeFlip(PosType pos, int mark) : 
		vcg::tri::CurvEdgeFlip<CMeshO, NSMCEdgeFlip, NSMCurvEval<CurvType> >(pos, mark) {}
};

class MeanCEdgeFlip : public vcg::tri::CurvEdgeFlip<CMeshO, MeanCEdgeFlip, MeanCurvEval<CurvType> >
{
public:
	MeanCEdgeFlip(PosType pos, int mark) : 
		vcg::tri::CurvEdgeFlip<CMeshO, MeanCEdgeFlip, MeanCurvEval<CurvType> >(pos, mark) {}
};

class AbsCEdgeFlip : public vcg::tri::CurvEdgeFlip<CMeshO, AbsCEdgeFlip, AbsCurvEval<CurvType> >
{
public:
	AbsCEdgeFlip(PosType pos, int mark) : 
		vcg::tri::CurvEdgeFlip<CMeshO, AbsCEdgeFlip, AbsCurvEval<CurvType> >(pos, mark) {}
};*/

// forward declarations
class MyTriEdgeFlip;
class MyTopoEdgeFlip;
class QualityEdgeFlip;
class QualityRadiiEdgeFlip;
class QualityMeanRatioEdgeFlip;

class MyTriEdgeFlip : public vcg::tri::TriEdgeFlip<CMeshO, MyTriEdgeFlip >
{
public:
	MyTriEdgeFlip(PosType pos, int mark) : 
		vcg::tri::TriEdgeFlip<CMeshO, MyTriEdgeFlip>(pos, mark) {}
};

class MyTopoEdgeFlip : public vcg::tri::TopoEdgeFlip<CMeshO, MyTopoEdgeFlip >
{
public:
	MyTopoEdgeFlip(PosType pos, int mark) : 
		vcg::tri::TopoEdgeFlip<CMeshO, MyTopoEdgeFlip>(pos, mark) {}
}; 

class QualityEdgeFlip : public vcg::tri::PlanarEdgeFlip<CMeshO, QualityEdgeFlip >
{
public:	
	QualityEdgeFlip(PosType pos, int mark) : 
		vcg::tri::PlanarEdgeFlip<CMeshO, QualityEdgeFlip>(pos, mark) {}
};

class QualityRadiiEdgeFlip : public vcg::tri::PlanarEdgeFlip<CMeshO, QualityRadiiEdgeFlip, QualityRadii >
{
public:	
	QualityRadiiEdgeFlip(PosType pos, int mark) : 
		vcg::tri::PlanarEdgeFlip<CMeshO, QualityRadiiEdgeFlip, QualityRadii>(pos, mark) {}
};

class QualityMeanRatioEdgeFlip : public vcg::tri::PlanarEdgeFlip<CMeshO, QualityMeanRatioEdgeFlip, QualityMeanRatio >
{
public:	
	QualityMeanRatioEdgeFlip(PosType pos, int mark) : 
		vcg::tri::PlanarEdgeFlip<CMeshO, QualityMeanRatioEdgeFlip, QualityMeanRatio>(pos, mark) {}
};


// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to
//    your filtering actions you can do here by construction the QActions accordingly
TriOptimizePlugin::TriOptimizePlugin() 
{
	typeList << FP_EDGE_FLIP << FP_NEAR_LAPLACIAN_SMOOTH;
	
	foreach(FilterIDType tt , types())
		actionList << new QAction(filterName(tt), this);
}


// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
const QString TriOptimizePlugin::filterName(FilterIDType filterId) 
{
	switch (filterId) {
		case FP_EDGE_FLIP:
			return tr("Edge flipping optimization");
		case FP_NEAR_LAPLACIAN_SMOOTH:
			return tr("Laplacian smooth (surface preserve)");
		default:
			assert(0);
	}
}

const int TriOptimizePlugin::getRequirements(QAction *action)
{
	switch (ID(action)) {
		case FP_EDGE_FLIP:
			return MeshModel::MM_FACETOPO | 
					   MeshModel::MM_VERTFACETOPO |
					   MeshModel::MM_VERTMARK |
					   MeshModel::MM_BORDERFLAG;
		case FP_NEAR_LAPLACIAN_SMOOTH:
			return MeshModel::MM_BORDERFLAG;
	}
	
	return 0;
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
const QString TriOptimizePlugin::filterInfo(FilterIDType filterId)
{
	switch(filterId) {
		case FP_EDGE_FLIP:
			return tr("Mesh optimization by edge flipping, to improve local "
			           "mesh curvature or triangle quality");
		case FP_NEAR_LAPLACIAN_SMOOTH:
			return tr("Laplacian smooth without surface modification: move "
			           "each vertex in the average position of neighbors "
			           "vertices, only if the new position still (almost) lies "
			           "on original surface");
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
void TriOptimizePlugin::initParameterSet(QAction *action, MeshModel &/*m*/, FilterParameterSet & parlst)
{
	if (ID(action) == FP_EDGE_FLIP) {
		parlst.addBool("selection", false, tr("Update selection"),
				tr("Apply edge flip optimization on selected faces only"));
		
		parlst.addBool("cflips", true, tr("Curvature flips"),
				tr("Do edge flips based on local curvature minimization"));
		
		parlst.addBool("pflips", true, tr("Planar flips"),
				tr("Do edge flips to improve adjacent almost planar faces"));
		
		parlst.addAbsPerc("pthreshold", 1.0f, 0.1f, 90.0f,
				tr("Planar threshold"),
				tr("angle threshold for planar faces (degrees)"));
		
		QStringList cmetrics;
		cmetrics.push_back("mean");
		cmetrics.push_back("norm squared");
		cmetrics.push_back("absolute");
		parlst.addEnum("curvtype", 0, cmetrics, tr("Curvature"),
				tr("Choose a metric to compute surface curvature on vertices"));
		
		QStringList pmetrics;
		pmetrics.push_back("area/max side");
		pmetrics.push_back("inradius/circumradius");
		pmetrics.push_back("mean ratio");
		pmetrics.push_back("delaunay");
		pmetrics.push_back("topology");
		parlst.addEnum("planartype", 0, pmetrics, tr("Planar"),
				tr("Choose a metric to compute triangle quality."));
	}
	
	if (ID(action) == FP_NEAR_LAPLACIAN_SMOOTH) {
		parlst.addBool("selection", false, tr("Update selection"),
				tr("Apply laplacian smooth on selected faces only"));
		parlst.addAbsPerc("dthreshold", 1.0f, 0.0f, 3.0f,
				tr("Max Displacement"),
				tr("maximum mean normal angle displacement (degrees)"
				   "from old to new faces"));
		parlst.addInt("iterations", 1, "Iterations",
				"number of laplacian smooth iterations in every run");
	}
}

// The Real Core Function doing the actual mesh processing.
// Run mesh optimization
bool TriOptimizePlugin::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & par, vcg::CallBackPos */*cb*/)
{
	float epsilon = std::numeric_limits<float>::epsilon();
	
	if (ID(filter) == FP_EDGE_FLIP) {
		int delvert=tri::Clean<CMeshO>::RemoveUnreferencedVertex(m.cm);
		if (delvert)
			Log(GLLogStream::Info,
			    "Pre-Curvature Cleaning: Removed %d unreferenced vertices",
			    delvert);
		
		tri::Allocator<CMeshO>::CompactVertexVector(m.cm);
		
		// to fix topology relations
		// TODO: make this as optional
		vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
		vcg::tri::UpdateTopology<CMeshO>::VertexFace(m.cm);
		vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromFF(m.cm);
		
		// temporary test
		vcg::tri::UpdateTopology<CMeshO>::TestFaceFace(m.cm);
		vcg::tri::UpdateTopology<CMeshO>::TestVertexFace(m.cm);
		
		vcg::LocalOptimization<CMeshO> optimization(m.cm);
		float pthr = par.getAbsPerc("pthreshold");
		time_t start = clock();
		
		if (par.getBool("selection")) {
			// Mark not writable un-selected faces
			CMeshO::FaceIterator fi;
			for (fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
				if (!(*fi).IsD()) {
					if (!(*fi).IsS()) (*fi).ClearW();
					else(*fi).SetW();
				}
			
			// select vertices with at least one incident face selected
			tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(m.cm);
			
			// Mark not writable un-selected vertices
			CMeshO::VertexIterator vi;
			for (vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi)
				if (!(*vi).IsD())
					if (!(*vi).IsS()) (*vi).ClearW();
					else (*vi).SetW();
		}
		
		if (par.getBool("cflips")) {
			int metric = par.getEnum("curvtype");
			switch (metric) {
				case 0:
					MeanCEdgeFlip::CoplanarAngleThresholdDeg() = pthr;
					optimization.Init<MeanCEdgeFlip>();
					break;
				case 1:
					NSMCEdgeFlip::CoplanarAngleThresholdDeg() = pthr;
					optimization.Init<NSMCEdgeFlip>();
					break;
				case 2:
					AbsCEdgeFlip::CoplanarAngleThresholdDeg() = pthr;
					optimization.Init<AbsCEdgeFlip>();
					break;
			}
			
			// stop when flips become harmful: != 0.0f to avoid same flips in every run
			// TODO: set a better limit
			optimization.SetTargetMetric(-epsilon);
			optimization.DoOptimization();
			
			Log(GLLogStream::Info,
					"%i curvature edge flips performed in %i sec.",
					optimization.nPerfmormedOps,
					(int) (clock() - start) / 1000000);
		}
		
		start = clock();
		if (par.getBool("pflips")) {
			int metric = par.getEnum("planartype");
			switch (metric) {
				case 0:
					QualityEdgeFlip::CoplanarAngleThresholdDeg() = pthr;
					optimization.Init<QualityEdgeFlip>();
					break;
				case 1:
					QualityRadiiEdgeFlip::CoplanarAngleThresholdDeg() = pthr;
					optimization.Init<QualityRadiiEdgeFlip>();
					break;
				case 2:
					QualityMeanRatioEdgeFlip::CoplanarAngleThresholdDeg() = pthr;
					optimization.Init<QualityMeanRatioEdgeFlip>();
					break;
				case 3:
					MyTriEdgeFlip::CoplanarAngleThresholdDeg() = pthr;
					optimization.Init<MyTriEdgeFlip>();
					break;
				case 4:
					MyTopoEdgeFlip::CoplanarAngleThresholdDeg() = pthr;
					optimization.Init<MyTopoEdgeFlip>();
					break;
			}
			
			// stop when flips become harmful:
			// != 0.0f to avoid same flips in every run
			// TODO: set a better limit
			optimization.SetTargetMetric(-epsilon);
			optimization.DoOptimization();
			
			Log(GLLogStream::Info, "%i planar edge flips performed in %i sec.",
			    optimization.nPerfmormedOps,
			    (int) (clock() - start) / 1000000);
		}
		
		//optimization.Finalize<CurvEdgeFlip>();
		vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
		
		// Clear Writable flags
		if (par.getBool("selection")) {
			CMeshO::FaceIterator fi;
			for (fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
				if (!(*fi).IsD()) (*fi).SetW();
			
			CMeshO::VertexIterator vi;
			for (vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi)
				if (!(*vi).IsD()) (*vi).SetW();
		}
	}
	
	if (ID(filter) == FP_NEAR_LAPLACIAN_SMOOTH) {
		bool sel = par.getBool("selection");
		if(sel)
			vcg::tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m.cm);
		
		int iternum = par.getInt("iterations");
		float dthreshold = par.getAbsPerc("dthreshold");
		LaplacianAdjust(m.cm, iternum, dthreshold, sel);
		tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
	}
	
	return true;
}

Q_EXPORT_PLUGIN(TriOptimizePlugin)
