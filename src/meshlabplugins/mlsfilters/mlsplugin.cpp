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

#define NDEBUG

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/refine.h>
#include <vcg/complex/trimesh/update/selection.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/append.h>
#include <vcg/complex/trimesh/create/advancing_front.h>
#include <vcg/complex/trimesh/create/marching_cubes.h>
// #include <apps/sample/trimesh_isosurface/simple_volume.h>
// #include <apps/sample/trimesh_isosurface/trivial_walker.h>
#include "mlsmarchingcube.h"

#include "mlsplugin.h"
#include "rimls.h"
#include "apss.h"
#include "mlsutils.h"
#include "../meshfilter/refine_loop.h"

#include "smallcomponentselection.h"

using namespace GaelMls;

// Constructor usually performs only two simple tasks of filling the two lists
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

MlsPlugin::MlsPlugin()
{
	typeList
		<< FP_RIMLS_PROJECTION << FP_APSS_PROJECTION
// 		<< FP_RIMLS_AFRONT << FP_APSS_AFRONT
		<< FP_RIMLS_MCUBE << FP_APSS_MCUBE
		<< FP_RADIUS_FROM_DENSITY
		<< FP_SELECT_SMALL_COMPONENTS;

// 	initFilterList(this);
	foreach(FilterIDType tt , types())
		actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action
// (this string is used also to define the menu entry)
const QString MlsPlugin::filterName(FilterIDType filterId)
{
	switch(filterId) {
		case FP_APSS_PROJECTION         : return QString("MLS projection (APSS)");
		case FP_RIMLS_PROJECTION        : return QString("MLS projection (#####)");
		case FP_APSS_AFRONT             : return QString("MLS meshing/APSS Advancing Front");
		case FP_RIMLS_AFRONT            : return QString("MLS meshing/##### Advancing Front");
		case FP_APSS_MCUBE              : return QString("Marching Cubes (APSS)");
		case FP_RIMLS_MCUBE             : return QString("Marching Cubes (#####)");
		case FP_RADIUS_FROM_DENSITY     : return QString("Estimate radius from density");
		case FP_SELECT_SMALL_COMPONENTS : return QString("Small component selection");
		default : assert(0);
	}
}

const MeshFilterInterface::FilterClass MlsPlugin::getClass(QAction *a)
{
	int id = ID(a);
	if (id == FP_SELECT_SMALL_COMPONENTS)
	{
		return MeshFilterInterface::Selection;
	}
	
	return MeshFilterInterface::PointSet;
}

// Info() must return the longer string describing each filtering action
// (this string is used in the About plugin dialog)
const QString MlsPlugin::filterInfo(FilterIDType filterId)
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

	if (filterId & _APSS_)
	{
		str +=
			"<br>This is the <i>algebraic point set surfaces</i> (APSS) variant which is based on"
			"the local fitting of algebraic spheres. It requires points equipped with oriented normals.<br>"
			"See [Guennebaud and Gross, Algebraic Point Set Surfaces, Siggraph 2007]"
			"and [Guennebaud et al., Dynamic Sampling and Rendering of APSS, Eurographics 2008]"
			"for all the details about APSS.";
	}

	if (filterId & _RIMLS_)
	{
		str +=
			"<br>This is the ##### MLS variant.";
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
void MlsPlugin::initParameterSet(QAction* action, MeshDocument& md, FilterParameterSet& parlst)
//void ExtraSamplePlugin::initParList(QAction *action, MeshModel &m, FilterParameterSet &parlst)
{
	int id = ID(action);
	MeshModel *target = md.mm();

	if (id == FP_SELECT_SMALL_COMPONENTS)
	{
		parlst.addFloat("NbFaceRatio",
										0.1,
										"Small component ratio",
										"This ratio (between 0 and 1) defines the meaning of <i>small</i> as the threshold ratio between the number of faces"
										"of the largest component and the other ones. A larger value will select more components.");
		parlst.addBool( "NonClosedOnly",
										false,
										"Select only non closed components",
										"");
		return;
	}
	else if (id == FP_RADIUS_FROM_DENSITY)
	{
		parlst.addInt("NbNeighbors",
									16,
									"Number of neighbors",
									"Number of neighbors used to estimate the local density. Larger values lead to smoother variations.");
		return;
	}

	if (id & _PROJECTION_)
	{
		parlst.addMesh( "ControlMesh", target, "Point set",
										"The point set (or mesh) which defines the MLS surface.");
		parlst.addMesh( "ProxyMesh", target, "Proxy Mesh",
										"The mesh that will be projected/resampled onto the MLS surface.");
		parlst.addBool( "SelectionOnly",
										target->cm.sfn>0,
										"Selection only",
										"If checked, only selected vertices will be projected.");
	}

	if ( (id & _APSS_) || (id & _RIMLS_) )
	{
		parlst.addFloat("FilterScale",
										2.0,
										"MLS - Filter scale",
										"Scale of the spatial low pass filter.\n"
										"It is relative to the radius (local point spacing) of the vertices.");
		parlst.addFloat("ProjectionAccuracy",
										1e-4,
										"Projection - Accuracy (adv)",
										"Threshold value used to stop the projections.\n"
										"This value is scaled by the mean point spacing to get the actual threshold.");
		parlst.addInt(  "MaxProjectionIters",
										15,
										"Projection - Max iterations (adv)",
										"Max number of iterations for the projection.");
	}

	if (id & _APSS_)
	{
		parlst.addFloat("SphericalParameter",
										1,
										"MLS - Spherical parameter",
										"Control the curvature of the fitted spheres: 0 is equivalent to a pure plane fit,"
										"1 to a pure spherical fit, values between 0 and 1 gives intermediate results,"
										"while others real values might give interresting results, but take care with extreme"
										"settings !");
		parlst.addBool( "AccurateNormal",
										true,
										"Accurate normals",
										"If checked, use the accurate MLS gradient instead of the local approximation"
										"to compute the normals.");
	}

	if (id & _RIMLS_)
	{
		parlst.addFloat("SigmaN",
										0.75,
										"MLS - Sharpness",
										"Width of the filter used by the normal refitting weight."
										"This weight function is a Gaussian on the distance between two unit vectors:"
										"the current gradient and the input normal. Therefore, typical value range between 0.5 (sharp) to 2 (smooth).");
		parlst.addInt(  "MaxRefittingIters",
										3,
										"MLS - Max fitting iterations",
										"Max number of fitting iterations. (0 or 1 is equivalent to the standard IMLS)");
	}

	if (id & _PROJECTION_)
	{
		parlst.addInt(  "MaxSubdivisions",
										0,
										"Refinement - Max subdivisions",
										"Max number of subdivisions.");
		parlst.addFloat("ThAngleInDegree",
										2,
										"Refinement - Crease angle (degree)",
										"Threshold angle between two faces controlling the refinement.");
	}

	if (id & _AFRONT_)
	{
	}

	if (id & _MCUBE_)
	{
		parlst.addInt(  "Resolution",
										200,
										"Grid Resolution",
										"The resolution of the grid on which we run the marching cubes."
										"This marching cube is memory friendly, so you can safely set large values up to 1000 or even more.");
	}
}

const int MlsPlugin::getRequirements(QAction *action)
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

#if 0
namespace vcg {
namespace tri {

template <class MeshType> class AdvancingMLS : public AdvancingFront<MeshType> {
public:
	typedef typename MeshType::VertexType     VertexType;
	typedef typename MeshType::VertexIterator VertexIterator;
	typedef typename MeshType::FaceType       FaceType;
	typedef typename MeshType::FaceIterator   FaceIterator;

	typedef typename MeshType::ScalarType     ScalarType;
	typedef typename MeshType::VertexType::CoordType   VectorType;

	typedef MlsSurface<ScalarType> MlsType;

	AdvancingMLS(MeshType &_mesh, MlsType& mls)
		: AdvancingFront<MeshType>(_mesh), mMls(mls)
	{}
	bool _SeedFace() {return this->SeedFace();}
	bool Seed(int &v0, int &v1, int &v2)
	{
		// pick the first poin and build a face around
		VectorType p0 = mMls.points()[0];
		VectorType n0 = mMls.normals()[0];
		ScalarType r0 = mMls.radii()[0];

		p0 = mMls.project(p0, &n0);
		VectorType vu = vcg::Cross(n0,VectorType(0,1,0));
		if (Norm(vu)<0.1)
			vu = Cross(n0,VectorType(0,0,1));
		vu.Normalize();
		VectorType vv = Cross(n0, vu);
		VertexType v[3];
		v[0].P() = VectorType(p0[0], p0[1], p0[2]);
		v[0].N() = n0;
		v[1].P() = mMls.project(v[0].P() + vu * r0, &v[1].N());
		v[2].P() = mMls.project(v[0].P() + vu * (r0*0.5) + vv * (r0*sqrt(3)*0.5), &v[2].N() );

		std::cout << r0 << " "
			<< vcg::Distance(v[0].P(), v[1].P()) << " "
			<< vcg::Distance(v[1].P(), v[2].P()) << " "
			<< vcg::Distance(v[2].P(), v[0].P()) << "\n";

		v[0].ClearFlags();
		v[1].ClearFlags();
		v[2].ClearFlags();

		v0 = this->mesh.vert.size();
		AddVertex(v[0]);
		v1 = this->mesh.vert.size();
		AddVertex(v[1]);
		v2 = this->mesh.vert.size();
		AddVertex(v[2]);

		std::cout << "seed created\n";

		return true;
	}

	int Place(FrontEdge &e, std::list<FrontEdge>::iterator &touch)
	{
		VectorType p[3];
		p[0] = this->mesh.vert[e.v0].P();
		p[1] = this->mesh.vert[e.v1].P();
		p[2] = this->mesh.vert[e.v2].P();
		VectorType point = p[0] + p[1] - p[2];
		ScalarType scale = 0.33 * (vcg::Distance(p[0], p[1]) + vcg::Distance(p[1], p[2]) + vcg::Distance(p[0], p[2]));
		std::cout
		<< vcg::Distance(p[0], p[1]) << " "
		<< vcg::Distance(p[1], p[2]) << " "
		<< vcg::Distance(p[2], p[0]) << "\n";

		// project p2 on the tangent plane
		VectorType m = (p[0]+p[1])*0.5;
		VectorType n0 = this->mesh.vert[e.v0].N() + this->mesh.vert[e.v1].N();
		n0.Normalize();
// 		VectorType h = vcg::Cross(p[0]-p[1],n0);
// 		h.Normalize();
// 		VectorType pp2 = p[2] - n0 * (vcg::Dot(p[2]-m,n0));
// 		if (vcg::Dot(pp2-m, h)>0)
// 			h = -h;
// 		VectorType point = m + h * (scale*sqrt(3)*0.5);

		point = mMls.project(point, &n0);

		int vn = this->mesh.vert.size();
		// find closest
		int minId = -1;
		ScalarType minD2 = 0.5*scale;
		ScalarType bm = 1.4*scale;
		bm = bm * bm;
		minD2 = minD2*minD2;
		for(int i = 0; i < this->mesh.vert.size(); i++)
		{
			ScalarType d2 = (this->mesh.vert[i].P() - point).SquaredNorm();
			if (d2<minD2 && i!= e.v0 && i!=e.v1 && i!=e.v2 && this->mesh.vert[i].IsB()
					&& (this->mesh.vert[i].P() - m).SquaredNorm() < bm )
			{
				minId = i;
				minD2 = d2;
			}
		}
		if (minId>=0)
		{
// 			if((this->mesh.vert[i].P() - point).Norm() < 0.5*scale && i!= e.v0 && i!=e.v1 && i!=e.v2) {
				std::cout << "reuse existing vertex\n";
				vn = minId;
				//find the border
				//assert(this->mesh.vert[i].IsB());
				for(std::list<FrontEdge>::iterator k = this->front.begin(); k != this->front.end(); k++)
					if((*k).v0 == vn) touch = k;
				for(std::list<FrontEdge>::iterator k = this->deads.begin(); k != this->deads.end(); k++)
					if((*k).v0 == vn) touch = k;
// 				break;
// 			}
		}
		else //if(vn == this->mesh.vert.size())
		{
		std::cout << "create new vertex\n";
			VertexType v;
			v.P() = point;//mMls.project(point, &v.N());
			v.N() = n0;
			v.ClearFlags();
			AddVertex(v);
		}
		return vn;
	}

	MlsType& mMls;
// 	ScalarType r;
};

}
}

#endif

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
		n.Zero();
		for(int j=0; j<3; ++j)
			n += f->V(j)->cN();
		n.Normalize();
		f->N() = n;
	}
}

bool MlsPlugin::applyFilter(QAction* filter, MeshDocument& md, FilterParameterSet& par, vcg::CallBackPos* cb)
{
	int id = ID(filter);

	// get source and dest meshes
	if (id == FP_RADIUS_FROM_DENSITY)
	{
		CMeshO::VertContainer& points = md.mm()->cm.vert;
		if (!points.RadiusEnabled)
		{
			points.EnableRadius();
		}
		int nbNeighbors = par.getInt("NbNeighbors");

		assert(points.size()>=2);
		KdTree<float> knn(ConstDataWrapper<vcg::Point3f>(&points[0].cP(), points.size(),
																										size_t(&points[1].cP()[0]) - size_t(&points[0].cP()[0])));

		knn.setMaxNofNeighbors(nbNeighbors);
		for (size_t i = 0; i< points.size(); i++)
		{
			knn.doQueryK(points[i].cP());
			points[i].R() = 2. * sqrt(knn.getNeighborSquaredDistance(0)/float(knn.getNofFoundNeighbors()));
		}
	}
	else if (id == FP_SELECT_SMALL_COMPONENTS)
	{
		MeshModel* mesh = md.mm();
		mesh->updateDataMask(MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG);
		bool nonClosedOnly = par.getBool("NonClosedOnly");
		float ratio = par.getFloat("NbFaceRatio");
		vcg::tri::SmallComponent<CMeshO>::Select(mesh->cm, ratio, nonClosedOnly);
	}
	else
	{
		// we are doing some MLS based stuff

		MeshModel* pPoints = 0;
		if (id & _PROJECTION_)
		{
			if (par.getMesh("ControlMesh") == par.getMesh("ProxyMesh"))
			{
				// clone the control mesh
				MeshModel* ref = par.getMesh("ControlMesh");
				pPoints = new MeshModel();
				pPoints->updateDataMask(ref->currentDataMask);
				vcg::tri::Append<CMeshO,CMeshO>::Mesh(pPoints->cm, ref->cm, false);
				vcg::tri::UpdateBounding<CMeshO>::Box(pPoints->cm);
				pPoints->cm.Tr = ref->cm.Tr;
			}
			else
				pPoints = par.getMesh("ControlMesh");
		}
		else
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
					mesh->updateDataMask(MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG);

					vcg::tri::UpdateNormals<CMeshO>::PerFace(mesh->cm);
					vcg::tri::UpdateNormals<CMeshO>::NormalizeFace(mesh->cm);
					//vcg::RefineE<CMeshO,vcg::MidPoint<CMeshO> >(m.cm, vcg::MidPoint<CMeshO>(), edgePred, false, cb);
					vcg::RefineOddEvenE<CMeshO, vcg::OddPointLoop<CMeshO>, vcg::EvenPointLoop<CMeshO> >
							(mesh->cm, vcg::OddPointLoop<CMeshO>(), vcg::EvenPointLoop<CMeshO>(), edgePred, selectionOnly, cb);
				}
				// project all vertices onto the MLS surface
				for (unsigned int i = 0; i< mesh->cm.vert.size(); i++)
				{
					cb(1+98*i/mesh->cm.vert.size(), "MLS projection...");

					if ( (!selectionOnly) || (mesh->cm.vert[i].IsS()) )
						mesh->cm.vert[i].P() = mls->project(mesh->cm.vert[i].P(), &mesh->cm.vert[i].N());
				}
			}

			Log(0, "Successfully projected %i vertices", mesh->cm.vn);
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
			using namespace vcg;
			// create a new mesh
			mesh = md.addNewMesh("mc_mesh");

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
				mesh->updateDataMask(MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG);
				// selection...
				vcg::tri::SmallComponent<CMeshO>::Select(mesh->cm, 0.1);
				// deletion...
				vcg::tri::SmallComponent<CMeshO>::DeleteFaceVert(mesh->cm);
				mesh->clearDataMask(MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG);
			}

			Log(0, "Marching cubes MLS meshing done.");
		}

		delete mls;
		if ( (id & _PROJECTION_) && par.getMesh("ControlMesh")!=pPoints)
		{
			delete pPoints;
		}

		cb(99, "Update face normals...");
		vcg::tri::UpdateNormals<CMeshO>::PerFace(mesh->cm);
		cb(100, "Update box...");
		vcg::tri::UpdateBounding<CMeshO>::Box(mesh->cm);

	} // end MLS based stuff

	return true;
}

Q_EXPORT_PLUGIN(MlsPlugin)
