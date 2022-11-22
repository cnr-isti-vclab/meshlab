/*****************************************************************************
 * MeshLab                                                           o o     *
 * A versatile mesh processing toolbox                             o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2005-2021                                           \/)\/    *
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

#include "filter_qhull.h"

#include "qhull_tools.h"

#include <vcg/complex/algorithms/convex_hull.h>

using namespace std;
using namespace vcg;

QhullPlugin::QhullPlugin()
{
	typeList = {
		FP_QHULL_CONVEX_HULL,
		FP_QHULL_VORONOI_FILTERING,
		FP_QHULL_ALPHA_COMPLEX_AND_SHAPE,
		FP_QHULL_VISIBLE_POINTS};

	for (ActionIDType tt : types())
		actionList.push_back(new QAction(filterName(tt), this));
}

QhullPlugin::~QhullPlugin()
{
}

QString QhullPlugin::pluginName() const
{
	return "FilterQHull";
}

QString QhullPlugin::filterName(ActionIDType f) const
{
	switch (f) {
	case FP_QHULL_CONVEX_HULL: return QString("Convex Hull");
	case FP_QHULL_VORONOI_FILTERING: return QString("Voronoi Filtering");
	case FP_QHULL_ALPHA_COMPLEX_AND_SHAPE: return QString("Alpha Complex/Shape");
	case FP_QHULL_VISIBLE_POINTS: return QString("Select Convex Hull Visible Points");
	default: assert(0); return QString();
	}
}

QString QhullPlugin::pythonFilterName(ActionIDType filterId) const
{
	switch (filterId) {
	case FP_QHULL_CONVEX_HULL: return QString("generate_convex_hull");
	case FP_QHULL_VORONOI_FILTERING: return QString("generate_voronoi_filtering");
	case FP_QHULL_ALPHA_COMPLEX_AND_SHAPE: return QString("generate_alpha_shape");
	case FP_QHULL_VISIBLE_POINTS: return QString("compute_selection_of_visible_convex_hull_per_vertex");
	default: assert(0); return QString();
	}
}

QString QhullPlugin::filterInfo(ActionIDType filterId) const
{
	switch (filterId) {
	case FP_QHULL_CONVEX_HULL:
		return QString(
			"Calculate the <b>convex hull</b> with Qhull library "
			"(http://www.qhull.org/html/qconvex.htm).<br><br> "
			"The convex hull of a set of points is the boundary of the minimal convex set "
			"containing the given non-empty finite set of points.");
	case FP_QHULL_VORONOI_FILTERING:
		return QString(
			"Compute a <b>Voronoi filtering</b> (Amenta and Bern 1998) with Qhull library "
			"(http://www.qhull.org/). <br><br>"
			"The algorithm calculates a triangulation of the input point cloud without requiring "
			"vertex normals."
			"It uses a subset of the Voronoi vertices to remove triangles from the Delaunay "
			"triangulation. <br>"
			"After computing the Voronoi diagram, foreach sample point it chooses the two farthest "
			"opposite Voronoi vertices."
			"Then computes a Delaunay triangulation of the sample points and the selected Voronoi "
			"vertices, and keep "
			"only those triangles in witch all three vertices are sample points.");
	case FP_QHULL_ALPHA_COMPLEX_AND_SHAPE:
		return QString(
			"Calculate the <b>Alpha Shape</b> of the mesh(Edelsbrunner and P.Mucke 1994) with "
			"Qhull library (http://www.qhull.org/). <br><br>"
			"From a given finite point set in the space it computes 'the shape' of the set."
			"The Alpha Shape is the boundary of the alpha complex, that is a subcomplex of the "
			"Delaunay triangulation of the given point set.<br>"
			"For a given value of 'alpha', the alpha complex includes all the simplices in the "
			"Delaunay "
			"triangulation which have an empty circumsphere with radius equal or smaller than "
			"'alpha'.<br>"
			"The filter inserts the minimum value of alpha (the circumradius of the triangle) in "
			"attribute Quality foreach face.");
	case FP_QHULL_VISIBLE_POINTS:
		return QString(
			"Select the <b>visible points</b> in the convex hull of a point cloud, as viewed from "
			"a given viewpoint.<br>"
			"It uses the Qhull library (http://www.qhull.org/ <br><br>"
			"The algorithm used (Katz, Tal and Basri 2007) determines visibility without "
			"reconstructing a surface or estimating normals."
			"A point is considered visible if its transformed point lies on the convex hull of a "
			"transformed points cloud from the original mesh points.");
	default: assert(0);
	}
	return QString("Error: Unknown Filter");
}

QhullPlugin::FilterClass QhullPlugin::getClass(const QAction* a) const
{
	switch (ID(a)) {
	case FP_QHULL_CONVEX_HULL:
	case FP_QHULL_VORONOI_FILTERING:
	case FP_QHULL_ALPHA_COMPLEX_AND_SHAPE: return FilterClass(FilterPlugin::Remeshing);
	case FP_QHULL_VISIBLE_POINTS:
		return FilterClass(FilterPlugin::Selection + FilterPlugin::PointSet);
	default: assert(0);
	}
	return FilterClass(0);
}

RichParameterList QhullPlugin::initParameterList(const QAction* action, const MeshModel& m)
{
	RichParameterList parlst;
	switch (ID(action)) {
	case FP_QHULL_CONVEX_HULL: {
		// parlst.addParam(RichBool("reorient", false,"Re-orient all faces coherently","Re-orient
		// all faces coherently"));
		break;
	}
	case FP_QHULL_VORONOI_FILTERING: {
		parlst.addParam(RichDynamicFloat(
			"threshold",
			10.0f,
			0.0f,
			2000.0f,
			"Pole Discard Thr",
			"Threshold used to discard the Voronoi vertices too far from the origin."
			"We discard vertices are further than this factor times the bbox diagonal <br>"
			"Growing values of this value will add more Voronoi vertices for a better tightier "
			"surface reconstruction."
			"On the other hand they will increase processing time and could cause numerical "
			"problems to the qhull library.<br>"));
		break;
	}
	case FP_QHULL_ALPHA_COMPLEX_AND_SHAPE: {
		parlst.addParam(RichPercentage(
			"alpha",
			m.cm.bbox.Diag() / 100.0,
			0,
			m.cm.bbox.Diag(),
			tr("Alpha value"),
			tr("Compute the alpha value as percentage of the diagonal of the bbox")));
		parlst.addParam(RichEnum(
			"Filtering",
			0,
			QStringList() << "Alpha Complex"
						  << "Alpha Shape",
			tr("Get:"),
			tr("Select the output. The Alpha Shape is the boundary of the Alpha Complex")));
		break;
	}
	case FP_QHULL_VISIBLE_POINTS: {
		parlst.addParam(RichDynamicFloat(
			"radiusThreshold",
			0.0f,
			0.0f,
			7.0f,
			"radius threshold ",
			"Bounds the radius of the sphere used to select visible points."
			"It is used to adjust the radius of the sphere (calculated as distance between the "
			"center and the farthest point from it) "
			"according to the following equation: <br>"
			"radius = radius * pow(10,threshold); <br>"
			"As the radius increases more points are marked as visible."
			"Use a big threshold for dense point clouds, a small one for sparse clouds."));

		parlst.addParam(RichBool(
			"usecamera",
			false,
			"Use ViewPoint from Mesh Camera",
			"Uses the ViewPoint from the camera associated to the current mesh\n if there is no "
			"camera, an error occurs"));
		parlst.addParam(RichDirection(
			"viewpoint",
			Point3f(0.0f, 0.0f, 0.0f),
			"ViewPoint",
			"if UseCamera is true, this value is ignored"));

		parlst.addParam(RichBool(
			"convex_hullFP",
			false,
			"Show Partial Convex Hull of flipped points",
			"Show Partial Convex Hull of the transformed point cloud"));
		parlst.addParam(RichBool(
			"triangVP",
			false,
			"Show a triangulation of the visible points",
			"Show a triangulation of the visible points"));
		// parlst.addParam(RichBool("reorient", false,"Re-orient all faces of the CH
		// coherently","Re-orient all faces of the CH coherently."
		//                "If no Convex Hulls are selected , this value is ignored"));
		break;
		break;
	}
	default: break; // do not add any parameter for the other filters
	}
	return parlst;
}

std::map<std::string, QVariant> QhullPlugin::applyFilter(
	const QAction*           filter,
	const RichParameterList& par,
	MeshDocument&            md,
	unsigned int& /*postConditionMask*/,
	vcg::CallBackPos* /* cb*/)
{
	qhT  qh_qh = {};
	qhT* qh    = &qh_qh;

	switch (ID(filter)) {
	case FP_QHULL_CONVEX_HULL: {
		MeshModel& m  = *md.mm();
		MeshModel& pm = *md.addNewMesh("", "Convex Hull");
		pm.updateDataMask(MeshModel::MM_FACEFACETOPO);
		bool result = vcg::tri::ConvexHull<CMeshO, CMeshO>::ComputeConvexHull(m.cm, pm.cm);
		pm.clearDataMask(MeshModel::MM_FACEFACETOPO);
		pm.updateBoxAndNormals();
		if (!result)
			throw MLException("Failed computing convex hull.");
	} break;
	case FP_QHULL_VORONOI_FILTERING: {
		MeshModel& m  = *md.mm();
		MeshModel& pm = *md.addNewMesh("", "Voronoi Filtering");

		m.clearDataMask(MeshModel::MM_WEDGTEXCOORD);
		m.clearDataMask(MeshModel::MM_VERTTEXCOORD);

		int dim       = 3;       /* dimension of points */
		int numpoints = m.cm.vn; /* number of mesh vertices */

		Scalarm threshold = par.getDynamicFloat("threshold");

		bool result = compute_voronoi(qh, dim, numpoints, m, pm, threshold);

		if (result) {
			// vcg::tri::UpdateBounding<CMeshO>::Box(pm.cm);
			// vcg::tri::UpdateNormal<CMeshO>::PerVertexNormalizedPerFace(pm.cm);
			pm.updateBoxAndNormals();
			log("Successfully created a mesh of %i vert and %i faces", pm.cm.vn, pm.cm.fn);
		}
		else
			throw MLException("Failed computing voronoi filtering.");
	} break;
	case FP_QHULL_ALPHA_COMPLEX_AND_SHAPE: {
		MeshModel& m = *md.mm();

		if (m.hasDataMask(MeshModel::MM_WEDGTEXCOORD)) {
			m.clearDataMask(MeshModel::MM_WEDGTEXCOORD);
		}
		if (m.hasDataMask(MeshModel::MM_VERTTEXCOORD)) {
			m.clearDataMask(MeshModel::MM_VERTTEXCOORD);
		}

		int dim       = 3;       /* dimension of points */
		int numpoints = m.cm.vn; /* number of mesh vertices */

		double alpha = par.getAbsPerc("alpha");

		bool    alphashape = false;
		QString name;
		switch (par.getEnum("Filtering")) {
		case 0:
			alphashape = false;
			name       = QString("Alpha Complex");
			break;
		case 1:
			alphashape = true;
			name       = QString("Alpha Shapes");
			break;
		}

		MeshModel& pm = *md.addNewMesh("", qUtf8Printable(name));

		if (!alphashape && !pm.hasDataMask(MeshModel::MM_FACEQUALITY)) {
			pm.updateDataMask(MeshModel::MM_FACEQUALITY);
		}

		bool result = compute_alpha_shapes(qh, dim, numpoints, m, pm, alpha, alphashape);

		if (result) {
			// vcg::tri::UpdateBounding<CMeshO>::Box(pm.cm);
			// vcg::tri::UpdateNormal<CMeshO>::PerVertexNormalizedPerFace(pm.cm);
			pm.updateBoxAndNormals();
			log("Successfully created a mesh of %i vert and %i faces", pm.cm.vn, pm.cm.fn);
			log("Alpha = %f ", alpha);
			// m.cm.Clear();
		}
		else
			throw MLException("Failed computing alpha complex shape.");
	} break;
	case FP_QHULL_VISIBLE_POINTS: {
		MeshModel& m = *md.mm();
		m.updateDataMask(MeshModel::MM_VERTFLAGSELECT);

		// Clear old selection
		tri::UpdateSelection<CMeshO>::VertexClear(m.cm);

		bool    usecam    = par.getBool("usecamera");
		Point3m viewpoint = par.getPoint3m("viewpoint");
		Scalarm threshold = par.getDynamicFloat("radiusThreshold");

		// if usecamera but mesh does not have one
		if (usecam && !m.hasDataMask(MeshModel::MM_CAMERA)) {
			throw MLException(
				"Mesh has not a camera that can be used to compute view direction. Please set a "
				"view direction.");
		}
		if (usecam) {
			viewpoint = m.cm.shot.GetViewPoint();
		}

		// MeshModel &pm2 =*md.addNewMesh("","Visible Points Triangulation");
		CMeshO visiblePointsTriangulationMesh;
		visiblePointsTriangulationMesh.face.EnableFFAdjacency();

		bool convex_hullFP = par.getBool("convex_hullFP");
		bool triangVP      = par.getBool("triangVP");
		vcg::tri::ConvexHull<CMeshO, CMeshO>::ComputePointVisibility(
			m.cm, visiblePointsTriangulationMesh, viewpoint, threshold);
		if (convex_hullFP) {
			MeshModel& pm = *md.addNewMesh("", "CH Flipped Points");
			pm.updateDataMask(MeshModel::MM_FACEFACETOPO);
			vcg::tri::ConvexHull<CMeshO, CMeshO>::ComputeConvexHull(
				visiblePointsTriangulationMesh, pm.cm);
		}
		if (triangVP) {
			MeshModel* tm =
				md.addNewMesh(visiblePointsTriangulationMesh, "Visible Points Triangulation");
			tm->clearDataMask(MeshModel::MM_VERTCOLOR);
			tm->clearDataMask(MeshModel::MM_VERTQUALITY);
		}

		int result = visiblePointsTriangulationMesh.vert.size();
		if (result >= 0) {
			log("Selected %i visible points", result);
		}
		else {
			throw MLException("Failed computing " + filter->text());
		}
	} break;
	default: wrongActionCalled(filter);
	}

	return std::map<std::string, QVariant>();
}
MESHLAB_PLUGIN_NAME_EXPORTER(QhullPlugin)
