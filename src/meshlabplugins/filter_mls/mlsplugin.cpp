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

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "mlsmarchingcube.h"
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/create/advancing_front.h>
#include <vcg/complex/algorithms/create/marching_cubes.h>
#include <vcg/complex/algorithms/refine.h>
#include <vcg/complex/algorithms/refine_loop.h>
#include <vcg/complex/append.h>

#include "apss.h"
#include "implicits.h"
#include "mlsplugin.h"
#include "rimls.h"

#include "smallcomponentselection.h"

using namespace GaelMls;
using namespace vcg;

typedef Histogram<Scalarm> Histogramm;

enum { CT_MEAN = 0, CT_GAUSS = 1, CT_K1 = 2, CT_K2 = 3, CT_APSS = 4 };

MlsPlugin::MlsPlugin()
{
	typeList = {
		FP_RIMLS_PROJECTION,
		FP_APSS_PROJECTION,
		FP_RIMLS_MCUBE,
		FP_APSS_MCUBE,
		FP_RIMLS_COLORIZE,
		FP_APSS_COLORIZE,
		FP_RADIUS_FROM_DENSITY,
		FP_SELECT_SMALL_COMPONENTS};

	// 	initFilterList(this);
	for (ActionIDType tt : types())
		actionList.push_back(new QAction(filterName(tt), this));
}

QString MlsPlugin::pluginName() const
{
	return "FilterMLS";
}

QString MlsPlugin::filterName(ActionIDType filterId) const
{
	switch (filterId) {
	case FP_APSS_PROJECTION: return QString("MLS projection (APSS)");
	case FP_RIMLS_PROJECTION: return QString("MLS projection (RIMLS)");
	case FP_APSS_MCUBE: return QString("Marching Cubes (APSS)");
	case FP_RIMLS_MCUBE: return QString("Marching Cubes (RIMLS)");
	case FP_APSS_COLORIZE: return QString("Colorize curvature (APSS)");
	case FP_RIMLS_COLORIZE: return QString("Colorize curvature (RIMLS)");
	case FP_RADIUS_FROM_DENSITY: return QString("Estimate radius from density");
	case FP_SELECT_SMALL_COMPONENTS: return QString("Select small disconnected component");
	default: assert(0); return QString();
	}
}

QString MlsPlugin::pythonFilterName(ActionIDType f) const
{
	switch (f) {
	case FP_APSS_PROJECTION: return QString("compute_mls_projection_apss");
	case FP_RIMLS_PROJECTION: return QString("compute_mls_projection_rimls");
	case FP_APSS_MCUBE: return QString("generate_marching_cubes_apss");
	case FP_RIMLS_MCUBE: return QString("generate_marching_cubes_rimls");
	case FP_APSS_COLORIZE: return QString("compute_curvature_and_color_apss_per_vertex");
	case FP_RIMLS_COLORIZE: return QString("compute_curvature_and_color_rimls_per_vertex");
	case FP_RADIUS_FROM_DENSITY: return QString("compute_custom_radius_scalar_attribute_per_vertex");
	case FP_SELECT_SMALL_COMPONENTS: return QString("compute_selection_by_small_disconnected_components_per_face");
	default: assert(0); return QString();
	}
}

FilterPlugin::FilterClass MlsPlugin::getClass(const QAction* a) const
{
	int filterId = ID(a);

	switch (filterId) {
	case FP_APSS_PROJECTION:
	case FP_RIMLS_PROJECTION: return FilterClass(FilterPlugin::PointSet + FilterPlugin::Smoothing);
	case FP_APSS_MCUBE:
	case FP_RIMLS_MCUBE: return FilterClass(FilterPlugin::PointSet | FilterPlugin::Remeshing);
	case FP_APSS_COLORIZE:
	case FP_RIMLS_COLORIZE:
		return FilterClass(FilterPlugin::PointSet | FilterPlugin::VertexColoring);
	case FP_RADIUS_FROM_DENSITY: return FilterPlugin::PointSet;
	case FP_SELECT_SMALL_COMPONENTS: return FilterPlugin::Selection;
	}
	assert(0);
	return FilterPlugin::Generic;
}

QString MlsPlugin::filterInfo(ActionIDType filterId) const
{
	QString str = "";
	const QString proj = "Project a mesh (or a point set) onto the MLS surface defined by itself or another "
		"point set.<br>";
	const QString mcube = "Extract the iso-surface (as a mesh) of a MLS surface defined by the current point set "
		"(or mesh)"
		"using the marching cubes algorithm. The coarse extraction is followed by an accurate "
		"projection"
		"step onto the MLS, and an extra zero removal procedure.<br>";
	const QString colorize = "Colorize the vertices of a mesh or point set using the curvature of the underlying "
		"surface.<br>";
	const QString apss = "<br>This is the <i>algebraic point set surfaces</i> (APSS) variant which is based on "
		"the local fitting of algebraic spheres. It requires points equipped with oriented "
		"normals. <br>"
		"For all the details about APSS see: <br> Guennebaud and Gross, 'Algebraic Point Set "
		"Surfaces', Siggraph 2007, and<br>"
		"Guennebaud et al., 'Dynamic Sampling and Rendering of APSS', Eurographics 2008";
	const QString rimls = "<br>This is the Robust Implicit MLS (RIMLS) variant which is an extension of "
		"Implicit MLS preserving sharp features using non linear regression. For more details "
		"see: <br>"
		"Oztireli, Guennebaud and Gross, 'Feature Preserving Point Set Surfaces based on "
		"Non-Linear Kernel Regression' Eurographics 2009.";

	switch (filterId) {
	case FP_APSS_PROJECTION:
		return proj + apss;
	case FP_RIMLS_PROJECTION:
		return proj + rimls;
	case FP_APSS_MCUBE:
		return mcube + apss;
	case FP_RIMLS_MCUBE:
		return mcube + rimls;
	case FP_APSS_COLORIZE:
		return colorize + apss;
	case FP_RIMLS_COLORIZE:
		return colorize + rimls;
	case FP_RADIUS_FROM_DENSITY:
		return "Estimate the local point spacing (aka radius) around each vertex using a basic "
			   "estimate of the local density.";
	case FP_SELECT_SMALL_COMPONENTS:
		return "Select the small disconnected components of a mesh.";
	default:
		return "";
	}
}

RichParameterList MlsPlugin::initParameterList(const QAction* action, const MeshDocument& md)
{
	RichParameterList parlst;

	switch (ID(action)) {
	case FP_APSS_PROJECTION:
		addProjectionParameters(parlst, md);
		addMlsParameters(parlst);
		addApssParameters(parlst, false);
		break;
	case FP_RIMLS_PROJECTION:
		addProjectionParameters(parlst, md);
		addMlsParameters(parlst);
		addRimlsParameters(parlst);
		break;
	case FP_APSS_MCUBE:
		addMlsParameters(parlst);
		addApssParameters(parlst, false);
		addMarchingCubesParameters(parlst);
		break;
	case FP_RIMLS_MCUBE:
		addMlsParameters(parlst);
		addRimlsParameters(parlst);
		addMarchingCubesParameters(parlst);
		break;
	case FP_APSS_COLORIZE:
		addMlsParameters(parlst);
		addApssParameters(parlst, true);
		addColorizeParameters(parlst, true);
		break;
	case FP_RIMLS_COLORIZE:
		addMlsParameters(parlst);
		addRimlsParameters(parlst);
		addColorizeParameters(parlst, false);
		break;
	case FP_SELECT_SMALL_COMPONENTS:
		parlst.addParam(RichFloat(
			"NbFaceRatio",
			0.1f,
			"Small component ratio",
			"This ratio (between 0 and 1) defines the meaning of <i>small</i> as the threshold "
			"ratio between the number of faces"
			"of the largest component and the other ones. A larger value will select more "
			"components."));
		parlst.addParam(RichBool("NonClosedOnly", false, "Select only non closed components", ""));

	case FP_RADIUS_FROM_DENSITY:
		parlst.addParam(RichInt(
			"NbNeighbors",
			16,
			"Number of neighbors",
			"Number of neighbors used to estimate the local density. Larger values lead to "
			"smoother variations."));
		break;
	}

	return parlst;
}

int MlsPlugin::getRequirements(const QAction*)
{
	return 0;
}

/** Predicate functor for adaptive refinement according to crease angle.
 *
 */
template<class MESH_TYPE, typename Scalar>
struct EdgeAnglePredicate
{
	Scalar thCosAngle;
	bool   operator()(vcg::face::Pos<typename MESH_TYPE::FaceType> ep) const
	{
		// FIXME why does the following fails:
		//     vcg::face::Pos<typename MESH_TYPE::FaceType> op = ep;
		//     op.FlipF();
		//     if (op.f)
		//       return vcg::Dot(ep.f->cN(), op.f->cN()) < thCosAngle;
		//     else
		//       return true;

		return (ep.F()->cN() * ep.FFlip()->cN()) < thCosAngle;
	}
};

/** compute the normal of a face as the average of its vertices */
template<typename MeshType>
void UpdateFaceNormalFromVertex(MeshType& m)
{
	typedef typename MeshType::VertexType   VertexType;
	typedef typename VertexType::NormalType NormalType;
	// typedef typename VertexType::ScalarType   ScalarType;
	typedef typename MeshType::FaceIterator FaceIterator;

	for (FaceIterator f = m.face.begin(); f != m.face.end(); ++f) {
		NormalType n;
		n.SetZero();
		for (int j = 0; j < 3; ++j)
			n += f->V(j)->cN();
		n.Normalize();
		f->N() = n;
	}
}

std::map<std::string, QVariant> MlsPlugin::applyFilter(
	const QAction*           filter,
	const RichParameterList& par,
	MeshDocument&            md,
	unsigned int& /*postConditionMask*/,
	vcg::CallBackPos* cb)
{
	std::map<std::string, QVariant> outValues;
	MeshModel* pPoints = nullptr;
	MlsSurface<CMeshO>* mls = nullptr;

	switch (ID(filter)) {
	case FP_APSS_PROJECTION:
		initMLS(md);
		pPoints = getProjectionPointsMesh(md, par);
		cb(1, "Create the MLS data structures...");
		mls = createMlsApss(pPoints, par, false);
		computeProjection(md, par, mls, pPoints, cb);
		break;
	case FP_RIMLS_PROJECTION:
		initMLS(md);
		pPoints = getProjectionPointsMesh(md, par);
		cb(1, "Create the MLS data structures...");
		mls = createMlsRimls(pPoints, par);
		computeProjection(md, par, mls, pPoints, cb);
		break;
	case FP_APSS_MCUBE:
		initMLS(md);
		pPoints = md.mm();
		mls = createMlsApss(pPoints, par, false);
		computeMarchingCubes(md, par, mls, cb);
		break;
	case FP_RIMLS_MCUBE:
		initMLS(md);
		pPoints = md.mm();
		mls = createMlsRimls(pPoints, par);
		computeMarchingCubes(md, par, mls, cb);
		break;
	case FP_APSS_COLORIZE:
		initMLS(md);
		pPoints = md.mm();
		mls = createMlsApss(pPoints, par, true);
		computeColorize(md, par, mls, pPoints, cb);
		break;
	case FP_RIMLS_COLORIZE:
		initMLS(md);
		pPoints = md.mm();
		mls = createMlsRimls(pPoints, par);
		computeColorize(md, par, mls, pPoints, cb);
		break;
	case FP_RADIUS_FROM_DENSITY: {
		GaelMls::computeVertexRadius(md.mm()->cm, par.getInt("NbNeighbors"));
		break;
	}
	case FP_SELECT_SMALL_COMPONENTS:
		md.mm()->updateDataMask(MeshModel::MM_FACEFACETOPO);
		vcg::tri::SmallComponent<CMeshO>::Select(md.mm()->cm, par.getFloat("NbFaceRatio"), par.getBool("NonClosedOnly"));
		break;
	default:
		wrongActionCalled(filter);
	}
	delete mls;

	return outValues;
}

void MlsPlugin::addProjectionParameters(RichParameterList& parlst, const MeshDocument& md)
{
	parlst.addParam(RichMesh(
		"ControlMesh",
		md.mm()->id(),
		&md,
		"Point set",
		"The point set (or mesh) which defines the MLS surface."));
	parlst.addParam(RichMesh(
		"ProxyMesh",
		md.mm()->id(),
		&md,
		"Proxy Mesh",
		"The mesh that will be projected/resampled onto the MLS surface."));
	parlst.addParam(RichBool(
		"SelectionOnly",
		false,
		"Selection only",
		"If checked, only selected vertices will be projected."));
	parlst.addParam(RichInt(
		"MaxSubdivisions", 0, "Refinement - Max subdivisions", "Max number of subdivisions."));
	parlst.addParam(RichFloat(
		"ThAngleInDegree",
		2,
		"Refinement - Crease angle (degree)",
		"Threshold angle between two faces controlling the refinement."));

}

void MlsPlugin::addMlsParameters(RichParameterList& parlst)
{
	parlst.addParam(RichFloat(
		"FilterScale",
		2.0,
		"MLS - Filter scale",
		"Scale of the spatial low pass filter.\n"
		"It is relative to the radius (local point spacing) of the vertices."));
	parlst.addParam(RichFloat(
		"ProjectionAccuracy",
		1e-4f,
		"Projection - Accuracy (adv)",
		"Threshold value used to stop the projections.\n"
		"This value is scaled by the mean point spacing to get the actual threshold."));
	parlst.addParam(RichInt(
		"MaxProjectionIters",
		15,
		"Projection - Max iterations (adv)",
		"Max number of iterations for the projection."));
}

void MlsPlugin::addApssParameters(RichParameterList& parlst, bool colorize)
{
	parlst.addParam(RichFloat(
		"SphericalParameter",
		1,
		"MLS - Spherical parameter",
		"Control the curvature of the fitted spheres: 0 is equivalent to a pure plane fit,"
		"1 to a pure spherical fit, values between 0 and 1 gives intermediate results,"
		"while other real values might give interesting results, but take care with extreme"
		"settings !"));
	if (!colorize)
		parlst.addParam(RichBool(
			"AccurateNormal",
			true,
			"Accurate normals",
			"If checked, use the accurate MLS gradient instead of the local approximation"
			"to compute the normals."));
}

void MlsPlugin::addRimlsParameters(RichParameterList& parlst)
{
	parlst.addParam(RichFloat(
		"SigmaN",
		0.75,
		"MLS - Sharpness",
		"Width of the filter used by the normal refitting weight."
		"This weight function is a Gaussian on the distance between two unit vectors:"
		"the current gradient and the input normal. Therefore, typical value range between 0.5 "
		"(sharp) to 2 (smooth)."));
	parlst.addParam(RichInt(
		"MaxRefittingIters",
		3,
		"MLS - Max fitting iterations",
		"Max number of fitting iterations. (0 or 1 is equivalent to the standard IMLS)"));

}

void MlsPlugin::addColorizeParameters(RichParameterList& parlst, bool apss)
{
	parlst.addParam(RichBool(
		"SelectionOnly",
		false,
		"Selection only",
		"If checked, only selected vertices will be projected."));
	QStringList lst;
	lst << "Mean"
		<< "Gauss"
		<< "K1"
		<< "K2";
	if (apss)
		lst << "ApproxMean";

	parlst.addParam(RichEnum(
		"CurvatureType",
		CT_MEAN,
		lst,
		"Curvature type",
		QString("The type of the curvature to plot.") +
			(apss ? "<br>ApproxMean uses the radius of the fitted sphere as an "
							 "approximation of the mean curvature." : "")));
}

void MlsPlugin::addMarchingCubesParameters(RichParameterList& parlst)
{
	parlst.addParam(RichInt(
		"Resolution",
		200,
		"Grid Resolution",
		"The resolution of the grid on which we run the marching cubes."
		"This marching cube is memory friendly, so you can safely set large values up to 1000 "
		"or even more."));
}

void MlsPlugin::initMLS(MeshDocument& md)
{
	if (md.mm()->cm.fn > 0) { // if we start from a mesh, and it has unreferenced vertices
		// normals are undefined on that vertices.
		int delvert = tri::Clean<CMeshO>::RemoveUnreferencedVertex(md.mm()->cm);
		if (delvert)
			log("Pre-MLS Cleaning: Removed %d unreferenced vertices", delvert);
	}
	tri::Allocator<CMeshO>::CompactVertexVector(md.mm()->cm);

	GaelMls::computeVertexRadius(md.mm()->cm);
}

MeshModel* MlsPlugin::getProjectionPointsMesh(MeshDocument& md, const RichParameterList& params)
{
	MeshModel* pPoints = 0;
	if (params.getMeshId("ControlMesh") == params.getMeshId("ProxyMesh")) {
		// clone the control mesh
		MeshModel* ref = md.getMesh(params.getMeshId("ControlMesh"));
		pPoints        = md.addNewMesh("", "TempMesh", false);
		pPoints->updateDataMask(ref);
		vcg::tri::Append<CMeshO, CMeshO>::Mesh(
			pPoints->cm, ref->cm); // the last true means "copy all vertices"
		vcg::tri::UpdateBounding<CMeshO>::Box(pPoints->cm);
		pPoints->cm.Tr = ref->cm.Tr;
	}
	else
		pPoints = md.getMesh(params.getMeshId("ControlMesh"));
	return pPoints;
}

MlsSurface<CMeshO>* MlsPlugin::createMlsRimls(MeshModel* pPoints, const RichParameterList& par)
{
	RIMLS<CMeshO>* rimls = new RIMLS<CMeshO>(pPoints->cm);
	rimls->setFilterScale(par.getFloat("FilterScale"));
	rimls->setMaxProjectionIters(par.getInt("MaxProjectionIters"));
	rimls->setProjectionAccuracy(par.getFloat("ProjectionAccuracy"));
	rimls->setMaxRefittingIters(par.getInt("MaxRefittingIters"));
	rimls->setSigmaN(par.getFloat("SigmaN"));
	return rimls;
}

MlsSurface<CMeshO>*
MlsPlugin::createMlsApss(MeshModel* pPoints, const RichParameterList& par, bool colorize)
{
	APSS<CMeshO>* apss = new APSS<CMeshO>(pPoints->cm);
	apss->setFilterScale(par.getFloat("FilterScale"));
	apss->setMaxProjectionIters(par.getInt("MaxProjectionIters"));
	apss->setProjectionAccuracy(par.getFloat("ProjectionAccuracy"));
	apss->setSphericalParameter(par.getFloat("SphericalParameter"));
	if (!colorize)
		apss->setGradientHint(
			par.getBool("AccurateNormal") ? GaelMls::MLS_DERIVATIVE_ACCURATE :
                                            GaelMls::MLS_DERIVATIVE_APPROX);
	return apss;
}

void MlsPlugin::computeProjection(
	MeshDocument&                md,
	const RichParameterList&     par,
	GaelMls::MlsSurface<CMeshO>* mls,
	MeshModel*                   pPoints,
	vcg::CallBackPos*            cb)
{
	MeshModel* mesh          = md.getMesh(par.getMeshId("ProxyMesh"));
	bool       selectionOnly = par.getBool("SelectionOnly");

	if (selectionOnly)
		vcg::tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(mesh->cm);
	EdgeAnglePredicate<CMeshO, float> edgePred;
	edgePred.thCosAngle = cos(M_PI * par.getFloat("ThAngleInDegree") / 180.);

	int nbRefinements = par.getInt("MaxSubdivisions");
	for (int k = 0; k < nbRefinements + 1; ++k) {
		// UpdateFaceNormalFromVertex(m.cm);
		if (k != 0) {
			mesh->updateDataMask(MeshModel::MM_FACEFACETOPO);

			vcg::tri::UpdateNormal<CMeshO>::PerFace(mesh->cm);
			vcg::tri::UpdateNormal<CMeshO>::NormalizePerFace(mesh->cm);
			// vcg::RefineE<CMeshO,vcg::MidPoint<CMeshO> >(m.cm, vcg::MidPoint<CMeshO>(),
			// edgePred, false, cb);
			vcg::tri::RefineOddEvenE<CMeshO, tri::OddPointLoop<CMeshO>, tri::EvenPointLoop<CMeshO>>(
				mesh->cm,
				tri::OddPointLoop<CMeshO>(mesh->cm),
				tri::EvenPointLoop<CMeshO>(),
				edgePred,
				selectionOnly,
				cb);
		}
		// project all vertices onto the MLS surface
		for (unsigned int i = 0; i < mesh->cm.vert.size(); i++) {
			cb(1 + 98 * i / mesh->cm.vert.size(), "MLS projection...");

			if ((!selectionOnly) || (mesh->cm.vert[i].IsS()))
				mesh->cm.vert[i].P() = mls->project(mesh->cm.vert[i].P(), &mesh->cm.vert[i].N());
		}
	}

	log("Successfully projected %i vertices", mesh->cm.vn);

	if (pPoints != nullptr && md.getMesh(par.getMeshId("ControlMesh")) != pPoints) {
		MeshModel* mm = md.mm();
		md.delMesh(pPoints->id());
		if (mm != pPoints) // just to be sure
			md.setCurrentMesh(mm->id());
	}
	mesh->updateBoxAndNormals();
}

void MlsPlugin::computeColorize(
	MeshDocument&                md,
	const RichParameterList&     par,
	GaelMls::MlsSurface<CMeshO>* mls,
	MeshModel*                   pPoints,
	vcg::CallBackPos*            cb)
{
	MeshModel* mesh = md.mm();
	mesh->updateDataMask(MeshModel::MM_VERTCOLOR);
	mesh->updateDataMask(MeshModel::MM_VERTQUALITY);
	mesh->updateDataMask(MeshModel::MM_VERTCURVDIR);

	bool selectionOnly = par.getBool("SelectionOnly");
	// bool approx = apss && par.getBool("ApproxCurvature");
	int ct = par.getEnum("CurvatureType");

	uint size = mesh->cm.vert.size();
	// std::vector<float> curvatures(size);
	Scalarm   minc = 1e9, maxc = -1e9, minabsc = 1e9;
	Point3m   grad;
	Matrix33m hess;

	// pass 1: computes curvatures and statistics
	for (unsigned int i = 0; i < size; i++) {
		cb(1 + 98 * i / pPoints->cm.vert.size(), "MLS colorization...");

		if ((!selectionOnly) || (pPoints->cm.vert[i].IsS())) {
			Point3m p = mls->project(mesh->cm.vert[i].P());
			Scalarm c = 0;

			if (ct == CT_APSS) {
				APSS<CMeshO>* apss = dynamic_cast<APSS<CMeshO>*>(mls);
				c                  = apss->approxMeanCurvature(p);
			}
			else {
				int errorMask;
				grad = mls->gradient(p, &errorMask);
				if (errorMask == MLS_OK && grad.Norm() > 1e-8) {
					hess = mls->hessian(p);
					implicits::WeingartenMap<CMeshO::ScalarType> W(grad, hess);

					mesh->cm.vert[i].PD1() = W.K1Dir();
					mesh->cm.vert[i].PD2() = W.K2Dir();
					mesh->cm.vert[i].K1()  = W.K1();
					mesh->cm.vert[i].K2()  = W.K2();

					switch (ct) {
					case CT_MEAN: c = W.MeanCurvature(); break;
					case CT_GAUSS: c = W.GaussCurvature(); break;
					case CT_K1: c = W.K1(); break;
					case CT_K2: c = W.K2(); break;
					default: assert(0 && "invalid curvature type");
					}
				}
				assert(
					!math::IsNAN(c) &&
					"You should never try to compute Histogram with Invalid Floating "
					"points numbers (NaN)");
			}
			mesh->cm.vert[i].Q() = c;
			minc                 = std::min(c, minc);
			maxc                 = std::max(c, maxc);
			minabsc              = std::min(std::abs(c), minabsc);
		}
	}
	// pass 2: convert the curvature to color
	cb(99, "Curvature to color...");

	Histogramm H;
	vcg::tri::Stat<CMeshO>::ComputePerVertexQualityHistogram(mesh->cm, H);
	vcg::tri::UpdateColor<CMeshO>::PerVertexQualityRamp(
		mesh->cm, H.Percentile(0.01f), H.Percentile(0.99f));
	mesh->updateBoxAndNormals();
}

void MlsPlugin::computeMarchingCubes(
	MeshDocument&                md,
	const RichParameterList&     par,
	GaelMls::MlsSurface<CMeshO>* mls,
	vcg::CallBackPos*            cb)
{
	// create a new mesh
	MeshModel* mesh = md.addNewMesh("", "mc_mesh");

	typedef vcg::tri::MlsWalker<CMeshO, MlsSurface<CMeshO>> MlsWalker;
	typedef vcg::tri::MarchingCubes<CMeshO, MlsWalker>      MlsMarchingCubes;
	MlsWalker                                               walker;
	walker.resolution = par.getInt("Resolution");

	// iso extraction
	MlsMarchingCubes mc(mesh->cm, walker);
	walker.BuildMesh<MlsMarchingCubes>(mesh->cm, *mls, mc, cb);

	// accurate projection
	for (unsigned int i = 0; i < mesh->cm.vert.size(); i++) {
		cb(1 + 98 * i / mesh->cm.vert.size(), "MLS projection...");
		mesh->cm.vert[i].P() = mls->project(mesh->cm.vert[i].P(), &mesh->cm.vert[i].N());
	}

	// extra zero detection and removal
	{
		mesh->updateDataMask(MeshModel::MM_FACEFACETOPO);
		// selection...
		vcg::tri::SmallComponent<CMeshO>::Select(mesh->cm, 0.1f);
		// deletion...
		vcg::tri::SmallComponent<CMeshO>::DeleteFaceVert(mesh->cm);
		mesh->clearDataMask(MeshModel::MM_FACEFACETOPO);
	}

	log("Marching cubes MLS meshing done.");
	mesh->updateBoxAndNormals();
}

MESHLAB_PLUGIN_NAME_EXPORTER(MlsPlugin)
