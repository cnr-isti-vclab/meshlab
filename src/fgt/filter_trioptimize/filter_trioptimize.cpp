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

#include "filter_trioptimize.h"
#include "curvedgeflip.h"

#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/smooth.h>
#include <vcg/complex/trimesh/update/topology.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/selection.h>

using namespace vcg;

// forward declarations
class NSMCEFlip;
class MeanCEFlip;
class AbsCEFlip;

class NSMCEFlip : public vcg::tri::CurvEdgeFlip<CMeshO, NSMCEFlip, NSMCEval >
{
public:
	NSMCEFlip(PosType pos, int mark) : 
		vcg::tri::CurvEdgeFlip<CMeshO, NSMCEFlip, NSMCEval >(pos, mark) {}
};

class MeanCEFlip : public vcg::tri::CurvEdgeFlip<CMeshO, MeanCEFlip, MeanCEval >
{
public:
	MeanCEFlip(PosType pos, int mark) : 
		vcg::tri::CurvEdgeFlip<CMeshO, MeanCEFlip, MeanCEval >(pos, mark) {}
};

class AbsCEFlip : public vcg::tri::CurvEdgeFlip<CMeshO, AbsCEFlip, AbsCEval >
{
public:
	AbsCEFlip(PosType pos, int mark) : 
		vcg::tri::CurvEdgeFlip<CMeshO, AbsCEFlip, AbsCEval >(pos, mark) {}
};

// forward declarations
class MyTriEFlip;
class MyTopoEFlip;
class QEFlip;
class QRadiiEFlip;
class QMeanRatioEFlip;

class MyTriEFlip : public vcg::tri::TriEdgeFlip<CMeshO, MyTriEFlip>
{
public:
	MyTriEFlip(PosType pos, int mark) :
		vcg::tri::TriEdgeFlip<CMeshO, MyTriEFlip>(pos, mark) {}
};

class MyTopoEFlip : public vcg::tri::TopoEdgeFlip<CMeshO, MyTopoEFlip>
{
public:
	MyTopoEFlip(PosType pos, int mark) :
		vcg::tri::TopoEdgeFlip<CMeshO, MyTopoEFlip>(pos, mark) {}
};

class QEFlip : public vcg::tri::PlanarEdgeFlip<CMeshO, QEFlip>
{
public:
	QEFlip(PosType pos, int mark) :
		vcg::tri::PlanarEdgeFlip<CMeshO, QEFlip>(pos, mark) {}
};

class QRadiiEFlip :
public vcg::tri::PlanarEdgeFlip<CMeshO, QRadiiEFlip, QualityRadii>
{
public:
	QRadiiEFlip(PosType pos, int mark) :
		vcg::tri::PlanarEdgeFlip<CMeshO, QRadiiEFlip, QualityRadii>(pos, mark) {}
};

class QMeanRatioEFlip :
public vcg::tri::PlanarEdgeFlip<CMeshO, QMeanRatioEFlip, QualityMeanRatio>
{
public:
	QMeanRatioEFlip(PosType pos, int mark) :
		vcg::tri::PlanarEdgeFlip<CMeshO, QMeanRatioEFlip, QualityMeanRatio>(pos, mark) {}
};


// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to
//    your filtering actions you can do here by construction the QActions accordingly
TriOptimizePlugin::TriOptimizePlugin() 
{
	typeList 
	<< FP_PLANAR_EDGE_FLIP 
	<< FP_CURVATURE_EDGE_FLIP 
	<< FP_NEAR_LAPLACIAN_SMOOTH;
	
	foreach(FilterIDType tt , types())
		actionList << new QAction(filterName(tt), this);
}


// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
 QString TriOptimizePlugin::filterName(FilterIDType filterId) const 
{
	switch (filterId) {
		case FP_PLANAR_EDGE_FLIP:				return tr("Planar flipping optimization");
		case FP_CURVATURE_EDGE_FLIP:		return tr("Curvature flipping optimization");
		case FP_NEAR_LAPLACIAN_SMOOTH: 	return tr("Laplacian smooth (surface preserve)");
		default:		assert(0);
	}
}

 int TriOptimizePlugin::getRequirements(QAction *action)
{
	switch (ID(action)) {
		case FP_PLANAR_EDGE_FLIP:
		case FP_CURVATURE_EDGE_FLIP:
			return MeshModel::MM_FACEFACETOPO |
			       MeshModel::MM_VERTFACETOPO | 
			       MeshModel::MM_VERTMARK | 
			       MeshModel::MM_FACEFLAGBORDER;
		case FP_NEAR_LAPLACIAN_SMOOTH:
			return MeshModel::MM_FACEFLAGBORDER;
	}
	
	return 0;
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
 QString TriOptimizePlugin::filterInfo(FilterIDType filterId) const
{
	switch(filterId) {
		case FP_PLANAR_EDGE_FLIP:
			return tr("Mesh optimization by edge flipping, to improve local triangle quality");
		case FP_CURVATURE_EDGE_FLIP:
			return tr("Mesh optimization by edge flipping, to improve local "
								"mesh curvature");
		case FP_NEAR_LAPLACIAN_SMOOTH:
			return tr("Laplacian smooth without surface modification: move "
			           "each vertex in the average position of neighbors "
			           "vertices, only if the new position still (almost) lies "
			           "on original surface");
		default : assert(0); 
	}
}

 TriOptimizePlugin::FilterClass TriOptimizePlugin::getClass(QAction *action)
{
	switch(ID(action)) {
		case FP_PLANAR_EDGE_FLIP:             return MeshFilterInterface::Remeshing;
		case FP_CURVATURE_EDGE_FLIP:             return MeshFilterInterface::Remeshing;
		case FP_NEAR_LAPLACIAN_SMOOTH: return MeshFilterInterface::Smoothing;
	}
 return MeshFilterInterface::Generic;
}
int TriOptimizePlugin::postCondition(QAction *a) const
{
  switch(ID(a))
  {
			case FP_PLANAR_EDGE_FLIP:
			case FP_CURVATURE_EDGE_FLIP:
					return MeshModel::MM_UNKNOWN;
			case FP_NEAR_LAPLACIAN_SMOOTH:	     
					return MeshModel::MM_VERTCOORD | MeshModel::MM_VERTNORMAL;
					
			default: assert(0);
	}
}

// This function define the needed parameters for each filter.
// Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parmeter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void TriOptimizePlugin::initParameterSet(QAction *action, MeshModel &m, RichParameterSet & parlst)
{
	if (ID(action) == FP_CURVATURE_EDGE_FLIP) {
		parlst.addParam(new RichBool("selection", m.cm.sfn > 0, tr("Update selection"), tr("Apply edge flip optimization on selected faces only")));
		parlst.addParam(new RichFloat("pthreshold", 1.0f,
											tr("Angle Thr (deg)"),
											tr("To avoid excessive flipping/swapping we consider only couple of faces with a significant diedral angle (e.g. greater than the indicated threshold). ")));
		
		QStringList cmetrics;
		cmetrics.push_back("mean");
		cmetrics.push_back("norm squared");
		cmetrics.push_back("absolute");
		
		parlst.addParam(new RichEnum("curvtype", 0, cmetrics, tr("Curvature metric"),
		        tr("<p style=\'white-space:pre\'>"
			        "Choose a metric to compute surface curvature on vertices<br>"
			        "H = mean curv, K = gaussian curv, A = area per vertex<br><br>"
			        "1: Mean curvature = H<br>"
			        "2: Norm squared mean curvature = (H * H) / A<br>"
			        "3: Absolute curvature:<br>"
			        "     if(K >= 0) return 2 * H<br>"
			        "     else return 2 * sqrt(H ^ 2 - A * K)")));
		}
	
		if (ID(action) == FP_PLANAR_EDGE_FLIP) {
			parlst.addParam(new RichBool("selection", m.cm.sfn > 0, tr("Update selection"), tr("Apply edge flip optimization on selected faces only")));

		parlst.addParam(new RichFloat("pthreshold", 1.0f,
											tr("Planar threshold (deg)"),
											tr("angle threshold for planar faces (degrees)")));
			
		QStringList pmetrics;
		pmetrics.push_back("area/max side");
		pmetrics.push_back("inradius/circumradius");
		pmetrics.push_back("mean ratio");
		pmetrics.push_back("delaunay");
		pmetrics.push_back("topology");
		parlst.addParam(new RichEnum("planartype", 0, pmetrics, tr("Planar metric"),
		        tr("<p style=\'white-space:pre\'>"
			        "Choose a metric to define the planar flip operation<br><br>"
			        "Triangle quality based<br>"
			        "1: minimum ratio height/edge among the edges<br>"
			        "2: ratio between radii of incenter and circumcenter<br>"
			        "3: 2*sqrt(a, b)/(a+b), a, b the eigenvalues of M^tM,<br>"
			        "     M transform triangle into equilateral<br><br>"
			        "Others<br>"
			        "4: Fix the Delaunay condition between two faces<br>"
			        "5: Do the flip to improve local topology<br>")));
		parlst.addParam(new RichInt("iterations", 1, "Post optimization relax iter", tr("number of a planar laplacian smooth iterations that have to be performed after every run")));

	}
	
	if (ID(action) == FP_NEAR_LAPLACIAN_SMOOTH) {
		parlst.addParam(new RichBool("selection", false, tr("Update selection"),	tr("Apply laplacian smooth on selected faces only")));
		parlst.addParam(new RichFloat("AngleDeg", 0.5f,	tr("Max Normal Dev (deg)"),	tr("maximum mean normal angle displacement (degrees) from old to new faces")));
		parlst.addParam(new RichInt("iterations", 1, "Iterations", tr("number of laplacian smooth iterations in every run")));
	}
}

// The Real Core Function doing the actual mesh processing.
// Run mesh optimization
bool TriOptimizePlugin::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb)
{
    MeshModel &m=*(md.mm());
    float limit = -std::numeric_limits<float>::epsilon();
	
	if (ID(filter) == FP_CURVATURE_EDGE_FLIP) {		
		int delvert = tri::Clean<CMeshO>::RemoveUnreferencedVertex(m.cm);
		if (delvert)
			Log(GLLogStream::FILTER,
			    "Pre-Curvature Cleaning: Removed %d unreferenced vertices",
			    delvert);

		tri::Allocator<CMeshO>::CompactVertexVector(m.cm);
		tri::Allocator<CMeshO>::CompactFaceVector(m.cm);
		vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
		vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromFF(m.cm);
		
    if ( tri::Clean<CMeshO>::CountNonManifoldEdgeFF(m.cm) >0) {
			errorMessage = "Mesh has some not 2-manifold faces, edge flips requires manifoldness";
			return false; // can't continue, mesh can't be processed
		}
		vcg::LocalOptimization<CMeshO> optimiz(m.cm);
		float pthr = par.getFloat("pthreshold");
		time_t start = clock();

		if (par.getBool("selection")) {
			// Mark not writable un-selected faces
			for (CMeshO::FaceIterator fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi) {
					if (!(*fi).IsS()) (*fi).ClearW();
					else (*fi).SetW();
				}

			// select vertices with at least one incident face selected
			tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(m.cm);

			// Mark not writable un-selected vertices
			for (CMeshO::VertexIterator vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi){
					if (!(*vi).IsS()) (*vi).ClearW();
					else (*vi).SetW();
				}
		}

			// VF adjacency needed for edge flips based on vertex curvature 
			vcg::tri::UpdateTopology<CMeshO>::VertexFace(m.cm);
			vcg::tri::UpdateTopology<CMeshO>::TestVertexFace(m.cm);

			int metric = par.getEnum("curvtype");
			switch (metric) {
				case 0: MeanCEFlip::CoplanarAngleThresholdDeg() = pthr; optimiz.Init<MeanCEFlip>(); break;
				case 1:  NSMCEFlip::CoplanarAngleThresholdDeg() = pthr; optimiz.Init<NSMCEFlip>(); 	break;
				case 2:  AbsCEFlip::CoplanarAngleThresholdDeg() = pthr; optimiz.Init<AbsCEFlip>();  break;
			}
			
			// stop when flips become harmful
			optimiz.SetTargetMetric(limit);
			//optimiz.SetTargetOperations(10);
			optimiz.DoOptimization();
			optimiz.h.clear();

			Log(GLLogStream::FILTER, "%d curvature edge flips performed in %.2f sec.",  optimiz.nPerfmormedOps, (clock() - start) / (float) CLOCKS_PER_SEC);
		}
	if (ID(filter) == FP_PLANAR_EDGE_FLIP) {
    if ( tri::Clean<CMeshO>::CountNonManifoldEdgeFF(m.cm) >0) {
					errorMessage = "Mesh has some not 2-manifold faces, edge flips requires manifoldness";
					return false; // can't continue, mesh can't be processed
				}
		
		bool selection = par.getBool("selection");
		
		tri::Allocator<CMeshO>::CompactVertexVector(m.cm);
		tri::Allocator<CMeshO>::CompactFaceVector(m.cm);
		vcg::tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
		vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromFF(m.cm);
		
		vcg::LocalOptimization<CMeshO> optimiz(m.cm);
		float pthr = par.getFloat("pthreshold");
	  time_t	start = clock();
		
		
			int metric = par.getEnum("planartype");
			switch (metric) {
				case 0:          QEFlip::CoplanarAngleThresholdDeg() = pthr; optimiz.Init<QEFlip>(); break;
				case 1:     QRadiiEFlip::CoplanarAngleThresholdDeg() = pthr; optimiz.Init<QRadiiEFlip>(); break;
				case 2: QMeanRatioEFlip::CoplanarAngleThresholdDeg() = pthr; optimiz.Init<QMeanRatioEFlip>(); break;
				case 3:      MyTriEFlip::CoplanarAngleThresholdDeg() = pthr; optimiz.Init<MyTriEFlip>(); break;
				case 4:     MyTopoEFlip::CoplanarAngleThresholdDeg() = pthr; optimiz.Init<MyTopoEFlip>(); 	break;
			}
			// stop when flips become harmful
			optimiz.SetTargetMetric(limit);
			optimiz.DoOptimization();
			optimiz.h.clear();

			Log(GLLogStream::FILTER, "%d planar edge flips performed in %.2f sec.", optimiz.nPerfmormedOps, (clock() - start) / (float) CLOCKS_PER_SEC);
			int iternum = par.getInt("iterations");
			
			tri::Smooth<CMeshO>::VertexCoordPlanarLaplacian(m.cm, iternum, 0.0001f, selection,cb);
			
		
		vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);

		if (par.getBool("selection")) {
			// Clear Writable flags (faces)
			CMeshO::FaceIterator fi;
			for (fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
				if (!(*fi).IsD())
					(*fi).SetW();

			// Clear Writable flags (vertices)
			CMeshO::VertexIterator vi;
			for (vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi)
				if (!(*vi).IsD())
					(*vi).SetW();
			
			// restore "default" selection  for vertices
			vcg::tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m.cm);
		}
	}

	if (ID(filter) == FP_NEAR_LAPLACIAN_SMOOTH) {
		bool selection = par.getBool("selection");
		if (selection)
			vcg::tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m.cm);

		int iternum = par.getInt("iterations");
		float dthreshold = par.getFloat("AngleDeg");
		tri::Smooth<CMeshO>::VertexCoordPlanarLaplacian(m.cm, iternum, math::ToRad(dthreshold), selection,cb);
		tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
	}

	return true;
}


Q_EXPORT_PLUGIN(TriOptimizePlugin)
