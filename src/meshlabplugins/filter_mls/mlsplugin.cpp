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
#include <iostream>

#include <common/interfaces.h>

#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/refine.h>
#include <vcg/complex/algorithms/refine_loop.h>
#include <vcg/complex/append.h>
#include <vcg/complex/algorithms/create/advancing_front.h>
#include <vcg/complex/algorithms/create/marching_cubes.h>
#include "mlsmarchingcube.h"

#include "mlsplugin.h"
#include "rimls.h"
#include "apss.h"
#include "implicits.h"


#include "smallcomponentselection.h"

using namespace GaelMls;
using namespace vcg;

// Constructor usually performs only two simple tasks of filling the two lists
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

enum {
	CT_MEAN  = 0,
	CT_GAUSS = 1,
	CT_K1    = 2,
	CT_K2    = 3,
	CT_APSS  = 4
};

MlsPlugin::MlsPlugin()
{
	typeList
		<< FP_RIMLS_PROJECTION << FP_APSS_PROJECTION
// 		<< FP_RIMLS_AFRONT << FP_APSS_AFRONT
		<< FP_RIMLS_MCUBE << FP_APSS_MCUBE
		<< FP_RIMLS_COLORIZE << FP_APSS_COLORIZE
		<< FP_RADIUS_FROM_DENSITY
		<< FP_SELECT_SMALL_COMPONENTS;

// 	initFilterList(this);
	foreach(FilterIDType tt , types())
		actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action
// (this string is used also to define the menu entry)
 QString MlsPlugin::filterName(FilterIDType filterId) const
{
	switch(filterId) {
		case FP_APSS_PROJECTION         : return QString("MLS projection (APSS)");
		case FP_RIMLS_PROJECTION        : return QString("MLS projection (RIMLS)");
		case FP_APSS_AFRONT             : return QString("MLS meshing/APSS Advancing Front");
		case FP_RIMLS_AFRONT            : return QString("MLS meshing/RIMLS Advancing Front");
		case FP_APSS_MCUBE              : return QString("Marching Cubes (APSS)");
		case FP_RIMLS_MCUBE             : return QString("Marching Cubes (RIMLS)");
		case FP_APSS_COLORIZE           : return QString("Colorize curvature (APSS)");
		case FP_RIMLS_COLORIZE          : return QString("Colorize curvature (RIMLS)");
		case FP_RADIUS_FROM_DENSITY     : return QString("Estimate radius from density");
		case FP_SELECT_SMALL_COMPONENTS : return QString("Small component selection");
		default : assert(0);
	}
return QString("Filter Unknown");
}

 MeshFilterInterface::FilterClass MlsPlugin::getClass(QAction *a)
{
	int filterId = ID(a);

	switch(filterId) {
		case FP_APSS_PROJECTION         :
		case FP_RIMLS_PROJECTION        : return FilterClass(MeshFilterInterface::PointSet + MeshFilterInterface::Smoothing);
		case FP_APSS_AFRONT             :
		case FP_RIMLS_AFRONT            :
		case FP_APSS_MCUBE              :
		case FP_RIMLS_MCUBE             : return FilterClass(MeshFilterInterface::PointSet | MeshFilterInterface::Remeshing);
		case FP_APSS_COLORIZE           :
		case FP_RIMLS_COLORIZE          : return FilterClass(MeshFilterInterface::PointSet | MeshFilterInterface::VertexColoring);
		case FP_RADIUS_FROM_DENSITY     : return MeshFilterInterface::PointSet;
		case FP_SELECT_SMALL_COMPONENTS : return MeshFilterInterface::Selection;
		}
	assert(0);
	return MeshFilterInterface::Generic;
}

// Info() must return the longer string describing each filtering action
// (this string is used in the About plugin dialog)
 QString MlsPlugin::filterInfo(FilterIDType filterId) const
{
	QString str = "";
	if (filterId & _PROJECTION_)
	{
		str += "Project a mesh (or a point set) onto the MLS surface defined by itself or another point set.<br>";
	}

	if (filterId & _MCUBE_)
	{
		str +=
			"Extract the iso-surface (as a mesh) of a MLS surface defined by the current point set (or mesh)"
			"using the marching cubes algorithm. The coarse extraction is followed by an accurate projection"
			"step onto the MLS, and an extra zero removal procedure.<br>";
	}

	if (filterId & _COLORIZE_)
	{
		str += "Colorize the vertices of a mesh or point set using the curfvature of the underlying surface.<br>";
	}

	if (filterId & _APSS_)
	{
		str +=
			"<br>This is the <i>algebraic point set surfaces</i> (APSS) variant which is based on "
			"the local fitting of algebraic spheres. It requires points equipped with oriented normals. <br>"
			"For all the details about APSS see: <br> Guennebaud and Gross, 'Algebraic Point Set Surfaces', Siggraph 2007, and<br>"
			"Guennebaud et al., 'Dynamic Sampling and Rendering of APSS', Eurographics 2008";
	}

	if (filterId & _RIMLS_)
	{
		str +=
			"<br>This is the Robust Implicit MLS (RIMLS) variant which is an extension of "
			"Implicit MLS preserving sharp features using non linear regression. For more details see: <br>"
			"Oztireli, Guennebaud and Gross, 'Feature Preserving Point Set Surfaces based on Non-Linear Kernel Regression' Eurographics 2009.";
	}

	if (filterId == FP_RADIUS_FROM_DENSITY)
		str = "Estimate the local point spacing (aka radius) around each vertex using a basic estimate of the local density.";
	else if (filterId == FP_SELECT_SMALL_COMPONENTS)
		str = "Select the small disconnected components of a mesh.";

	return str;
}


// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parmeter you need to define,
// - the name of the parameter,
// - the string shown in the dialog
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void MlsPlugin::initParameterSet(QAction* action, MeshDocument& md, RichParameterSet& parlst)
{
	int id = ID(action);
	MeshModel *target = md.mm();

	if (id == FP_SELECT_SMALL_COMPONENTS)
	{
		parlst.addParam(new RichFloat("NbFaceRatio",
										0.1f,
										"Small component ratio",
										"This ratio (between 0 and 1) defines the meaning of <i>small</i> as the threshold ratio between the number of faces"
										"of the largest component and the other ones. A larger value will select more components."));
		parlst.addParam(new RichBool( "NonClosedOnly",
										false,
										"Select only non closed components",
										""));
		return;
	}
	else if (id == FP_RADIUS_FROM_DENSITY)
	{
		parlst.addParam(new RichInt("NbNeighbors",
									16,
									"Number of neighbors",
									"Number of neighbors used to estimate the local density. Larger values lead to smoother variations."));
		return;
	}

	if ((id & _PROJECTION_))
	{
		parlst.addParam(new RichMesh( "ControlMesh", target,&md, "Point set",
										"The point set (or mesh) which defines the MLS surface."));
		parlst.addParam(new RichMesh( "ProxyMesh", target, &md, "Proxy Mesh",
										"The mesh that will be projected/resampled onto the MLS surface."));
	}
	if ((id & _PROJECTION_) || (id & _COLORIZE_))
	{
		parlst.addParam(new RichBool( "SelectionOnly",
										target->cm.sfn>0,
										"Selection only",
										"If checked, only selected vertices will be projected."));
	}

	if ( (id & _APSS_) || (id & _RIMLS_) )
	{
		parlst.addParam(new RichFloat("FilterScale",
										2.0,
										"MLS - Filter scale",
										"Scale of the spatial low pass filter.\n"
										"It is relative to the radius (local point spacing) of the vertices."));
		parlst.addParam(new RichFloat("ProjectionAccuracy",
										1e-4f,
										"Projection - Accuracy (adv)",
										"Threshold value used to stop the projections.\n"
										"This value is scaled by the mean point spacing to get the actual threshold."));
		parlst.addParam(new RichInt(  "MaxProjectionIters",
										15,
										"Projection - Max iterations (adv)",
										"Max number of iterations for the projection."));
	}

	if (id & _APSS_)
	{
		parlst.addParam(new RichFloat("SphericalParameter",
										1,
										"MLS - Spherical parameter",
										"Control the curvature of the fitted spheres: 0 is equivalent to a pure plane fit,"
										"1 to a pure spherical fit, values between 0 and 1 gives intermediate results,"
										"while others real values might give interresting results, but take care with extreme"
										"settings !"));
		if (!(id & _COLORIZE_))
			parlst.addParam(new RichBool( "AccurateNormal",
										true,
										"Accurate normals",
										"If checked, use the accurate MLS gradient instead of the local approximation"
										"to compute the normals."));
	}

	if (id & _RIMLS_)
	{
		parlst.addParam(new RichFloat("SigmaN",
										0.75,
										"MLS - Sharpness",
										"Width of the filter used by the normal refitting weight."
										"This weight function is a Gaussian on the distance between two unit vectors:"
										"the current gradient and the input normal. Therefore, typical value range between 0.5 (sharp) to 2 (smooth)."));
		parlst.addParam(new RichInt(  "MaxRefittingIters",
										3,
										"MLS - Max fitting iterations",
										"Max number of fitting iterations. (0 or 1 is equivalent to the standard IMLS)"));
	}

	if (id & _PROJECTION_)
	{
		parlst.addParam(new RichInt(  "MaxSubdivisions",
										0,
										"Refinement - Max subdivisions",
										"Max number of subdivisions."));
		parlst.addParam(new RichFloat("ThAngleInDegree",
										2,
										"Refinement - Crease angle (degree)",
										"Threshold angle between two faces controlling the refinement."));
	}

	if (id & _AFRONT_)
	{
	}

	if ((id & _COLORIZE_))
	{
		QStringList lst;
		lst << "Mean" << "Gauss" << "K1" << "K2";
		if (id & _APSS_)
			lst << "ApproxMean";

		parlst.addParam(new RichEnum("CurvatureType", CT_MEAN,
			lst,
			"Curvature type",
			QString("The type of the curvature to plot.")
			+ ((id & _APSS_) ? "<br>ApproxMean uses the radius of the fitted sphere as an approximation of the mean curvature." : "")));
// 		if ((id & _APSS_))
// 			parlst.addParam(new RichBool( "ApproxCurvature",
// 										false,
// 										"Approx mean curvature",
// 										"If checked, use the radius of the fitted sphere as an approximation of the mean curvature.");
	}

	if (id & _MCUBE_)
	{
		parlst.addParam(new RichInt(  "Resolution",
										200,
										"Grid Resolution",
										"The resolution of the grid on which we run the marching cubes."
										"This marching cube is memory friendly, so you can safely set large values up to 1000 or even more."));
	}
}

 int MlsPlugin::getRequirements(QAction *action)
{
	return 0;
}

/** Predicate functor for adaptive refinement according to crease angle.
	*
	*/
template <class MESH_TYPE, typename Scalar>
struct EdgeAnglePredicate
{
	Scalar thCosAngle;
	bool operator()(vcg::face::Pos<typename MESH_TYPE::FaceType> ep) const
	{
		// FIXME why does the following fails:
//     vcg::face::Pos<typename MESH_TYPE::FaceType> op = ep;
//     op.FlipF();
//     if (op.f)
//       return vcg::Dot(ep.f->cN(), op.f->cN()) < thCosAngle;
//     else
//       return true;

			return vcg::Dot(ep.F()->cN(), ep.FFlip()->cN()) < thCosAngle;
	}
};

/** compute the normal of a face as the average of its vertices */
template<typename MeshType>
void UpdateFaceNormalFromVertex(MeshType& m)
{
	typedef typename MeshType::VertexType     VertexType;
	typedef typename VertexType::NormalType   NormalType;
	typedef typename VertexType::ScalarType   ScalarType;
	typedef typename MeshType::FaceIterator   FaceIterator;

	for (FaceIterator f=m.face.begin(); f!=m.face.end(); ++f)
	{
		NormalType n;
		n.SetZero();
		for(int j=0; j<3; ++j)
			n += f->V(j)->cN();
		n.Normalize();
		f->N() = n;
	}
}

bool MlsPlugin::applyFilter(QAction* filter, MeshDocument& md, RichParameterSet& par, vcg::CallBackPos* cb)
{
	int id = ID(filter);

	if (id == FP_RADIUS_FROM_DENSITY)
	{
		md.mm()->updateDataMask(MeshModel::MM_VERTRADIUS);
		APSS<CMeshO> mls(md.mm()->cm);
		mls.computeVertexRaddi(par.getInt("NbNeighbors"));
		return true;
	}
	if (id == FP_SELECT_SMALL_COMPONENTS)
	{
		MeshModel* mesh = md.mm();
		mesh->updateDataMask(MeshModel::MM_FACEFACETOPO);
		bool nonClosedOnly = par.getBool("NonClosedOnly");
		float ratio = par.getFloat("NbFaceRatio");
		vcg::tri::SmallComponent<CMeshO>::Select(mesh->cm, ratio, nonClosedOnly);
		return true;
	}

	// we are doing some MLS based stuff
	{
		if(md.mm()->cm.fn > 0)
		{ // if we start from a mesh, and it has unreferenced vertices
		  // normals are undefined on that vertices.
			int delvert=tri::Clean<CMeshO>::RemoveUnreferencedVertex(md.mm()->cm);
			if(delvert) Log( "Pre-MLS Cleaning: Removed %d unreferenced vertices",delvert);
		}
		tri::Allocator<CMeshO>::CompactVertexVector(md.mm()->cm);

		// We require a per vertex radius so as a first thing check it
		if(!md.mm()->hasDataMask(MeshModel::MM_VERTRADIUS))
		{
			md.mm()->updateDataMask(MeshModel::MM_VERTRADIUS);
			APSS<CMeshO> mls(md.mm()->cm);
			mls.computeVertexRaddi();
			Log( "Mesh has no per vertex radius. Computed and added using default neighbourhood");
		}

		MeshModel* pPoints = 0;
		if (id & _PROJECTION_)
		{
			if (par.getMesh("ControlMesh") == par.getMesh("ProxyMesh"))
			{
				// clone the control mesh
				MeshModel* ref = par.getMesh("ControlMesh");
				pPoints = md.addNewMesh("","TempMesh");
				pPoints->updateDataMask(ref);
				vcg::tri::Append<CMeshO,CMeshO>::Mesh(pPoints->cm, ref->cm);  // the last true means "copy all vertices"
				vcg::tri::UpdateBounding<CMeshO>::Box(pPoints->cm);
				pPoints->cm.Tr = ref->cm.Tr;
			}
			else
				pPoints = par.getMesh("ControlMesh");
		}
		else // for curvature
			pPoints = md.mm();

		// create the MLS surface
		cb(1, "Create the MLS data structures...");
		MlsSurface<CMeshO>* mls = 0;

		RIMLS<CMeshO>* rimls = 0;
		APSS<CMeshO>* apss = 0;

		if (id & _RIMLS_)
			mls = rimls = new RIMLS<CMeshO>(pPoints->cm);
		else if (id & _APSS_)
			mls = apss = new APSS<CMeshO>(pPoints->cm);
		else
		{
			assert(0);
		}

		mls->setFilterScale(par.getFloat("FilterScale"));
		mls->setMaxProjectionIters(par.getInt("MaxProjectionIters"));
		mls->setProjectionAccuracy(par.getFloat("ProjectionAccuracy"));

		if (rimls)
		{
			rimls->setMaxRefittingIters(par.getInt("MaxRefittingIters"));
			//mls.setMinRefittingIters(par.getFloat("MinRefittingIters"));
			rimls->setSigmaN(par.getFloat("SigmaN"));
		}

		if (apss)
		{
			apss->setSphericalParameter(par.getFloat("SphericalParameter"));
			if (!(id & _COLORIZE_))
				apss->setGradientHint(par.getBool("AccurateNormal") ? GaelMls::MLS_DERIVATIVE_ACCURATE : GaelMls::MLS_DERIVATIVE_APPROX);
		}

		MeshModel * mesh = 0;

		if (id & _PROJECTION_)
		{
			mesh = par.getMesh("ProxyMesh");
			bool selectionOnly = par.getBool("SelectionOnly");

			if (selectionOnly)
				vcg::tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(mesh->cm);
			EdgeAnglePredicate<CMeshO,float> edgePred;
			edgePred.thCosAngle = cos(M_PI * par.getFloat("ThAngleInDegree")/180.);

			int nbRefinements = par.getInt("MaxSubdivisions");
			for (int k=0; k<nbRefinements+1; ++k)
			{
				//UpdateFaceNormalFromVertex(m.cm);
				if (k!=0)
				{
					mesh->updateDataMask(MeshModel::MM_FACEFACETOPO);

					vcg::tri::UpdateNormals<CMeshO>::PerFace(mesh->cm);
					vcg::tri::UpdateNormals<CMeshO>::NormalizeFace(mesh->cm);
					//vcg::RefineE<CMeshO,vcg::MidPoint<CMeshO> >(m.cm, vcg::MidPoint<CMeshO>(), edgePred, false, cb);
					vcg::tri::RefineOddEvenE<CMeshO, tri::OddPointLoop<CMeshO>, tri::EvenPointLoop<CMeshO> >
							(mesh->cm, tri::OddPointLoop<CMeshO>(mesh->cm), tri::EvenPointLoop<CMeshO>(), edgePred, selectionOnly, cb);
				}
				// project all vertices onto the MLS surface
				for (unsigned int i = 0; i< mesh->cm.vert.size(); i++)
				{
					cb(1+98*i/mesh->cm.vert.size(), "MLS projection...");

					if ( (!selectionOnly) || (mesh->cm.vert[i].IsS()) )
						mesh->cm.vert[i].P() = mls->project(mesh->cm.vert[i].P(), &mesh->cm.vert[i].N());
				}
			}

			Log( "Successfully projected %i vertices", mesh->cm.vn);
		}
		else if (id & _COLORIZE_)
		{
			mesh = md.mm();
			mesh->updateDataMask(MeshModel::MM_VERTCOLOR);
			mesh->updateDataMask(MeshModel::MM_VERTQUALITY);
			mesh->updateDataMask(MeshModel::MM_VERTCURVDIR);

			bool selectionOnly = par.getBool("SelectionOnly");
			//bool approx = apss && par.getBool("ApproxCurvature");
			int ct = par.getEnum("CurvatureType");

			uint size = mesh->cm.vert.size();
			//std::vector<float> curvatures(size);
			float minc=1e9, maxc=-1e9, minabsc=1e9;
			vcg::Point3f grad;
			vcg::Matrix33f hess;

			// pass 1: computes curvatures and statistics
			for (unsigned int i = 0; i< size; i++)
			{
				cb(1+98*i/pPoints->cm.vert.size(), "MLS colorization...");

				if ( (!selectionOnly) || (pPoints->cm.vert[i].IsS()) )
				{
					Point3f p = mls->project(mesh->cm.vert[i].P());
					float c = 0;

					if (ct==CT_APSS)
						c = apss->approxMeanCurvature(p);
					else
					{
						int errorMask;
						grad = mls->gradient(p, &errorMask);
						if (errorMask == MLS_OK && grad.Norm() > 1e-8)
						{
						  hess = mls->hessian(p);
						  implicits::WeingartenMap<float> W(grad,hess);

						  mesh->cm.vert[i].PD1() = W.K1Dir();
						  mesh->cm.vert[i].PD2() = W.K2Dir();
						  mesh->cm.vert[i].K1() =  W.K1();
						  mesh->cm.vert[i].K2() =  W.K2();

						  switch(ct)
						  {
							  case CT_MEAN: c = W.MeanCurvature(); break;
							  case CT_GAUSS: c = W.GaussCurvature(); break;
							  case CT_K1: c = W.K1(); break;
							  case CT_K2: c = W.K2(); break;
							  default: assert(0 && "invalid curvature type");
						  }
						}
						assert(!math::IsNAN(c) && "You should never try to compute Histogram with Invalid Floating points numbers (NaN)");
					}
					mesh->cm.vert[i].Q() = c;
					minc = std::min(c,minc);
					maxc = std::max(c,maxc);
					minabsc = std::min(fabsf(c),minabsc);
				}
			}
			// pass 2: convert the curvature to color
			cb(99, "Curvature to color...");
			float d = maxc-minc;
			minc += 0.05*d;
			maxc -= 0.05*d;

			vcg::Histogramf H;
      vcg::tri::Stat<CMeshO>::ComputePerVertexQualityHistogram(mesh->cm,H);
      vcg::tri::UpdateColor<CMeshO>::VertexQualityRamp(mesh->cm,H.Percentile(0.01f),H.Percentile(0.99f));
		}
	// 	else if (id & _AFRONT_)
	// 	{
	// 		// create a new mesh
	// 		mesh = md.addNewMesh("afront mesh");
	// 		vcg::tri::AdvancingMLS<CMeshO> afront(mesh->cm, *mls);
	// 		//afront.BuildMesh(cb);
	// 		afront._SeedFace();
	// 		for (int i=0; i<20120; ++i)
	// 			afront.AddFace();
	// 		Log(0, "Advancing front MLS meshing done.");
	// 	}
		else if (id & _MCUBE_)
		{
			// create a new mesh
			mesh = md.addNewMesh("","mc_mesh");

			typedef vcg::tri::MlsWalker<CMeshO,MlsSurface<CMeshO> > MlsWalker;
			typedef vcg::tri::MarchingCubes<CMeshO, MlsWalker> MlsMarchingCubes;
			MlsWalker walker;
			walker.resolution = par.getInt("Resolution");

			// iso extraction
			MlsMarchingCubes mc(mesh->cm, walker);
			walker.BuildMesh<MlsMarchingCubes>(mesh->cm, *mls, mc, cb);

			// accurate projection
			for (unsigned int i = 0; i< mesh->cm.vert.size(); i++)
			{
				cb(1+98*i/mesh->cm.vert.size(), "MLS projection...");
				mesh->cm.vert[i].P() = mls->project(mesh->cm.vert[i].P(), &mesh->cm.vert[i].N());
			}

			// extra zero detection and removal
			{
				mesh->updateDataMask(MeshModel::MM_FACEFACETOPO );
				// selection...
				vcg::tri::SmallComponent<CMeshO>::Select(mesh->cm, 0.1f);
				// deletion...
				vcg::tri::SmallComponent<CMeshO>::DeleteFaceVert(mesh->cm);
				mesh->clearDataMask(MeshModel::MM_FACEFACETOPO);
			}

			Log( "Marching cubes MLS meshing done.");
		}

		delete mls;
		if ( (id & _PROJECTION_) && par.getMesh("ControlMesh")!=pPoints)
		{
                        md.delMesh(pPoints);
		}

		if (mesh)
		{
			cb(99, "Update face normals...");
			vcg::tri::UpdateNormals<CMeshO>::PerFace(mesh->cm);
			cb(100, "Update box...");
			vcg::tri::UpdateBounding<CMeshO>::Box(mesh->cm);
		}

	} // end MLS based stuff

	return true;
}

Q_EXPORT_PLUGIN(MlsPlugin)
