/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2013                                                \/)\/    *
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
#include "filter_harmonic.h"
#include <vcg/space/index/grid_static_ptr.h>
#include <vcg/simplex/face/topology.h>
#include <eigenlib/Eigen/Sparse>

#include <map>

// Constructor usually performs only two simple tasks of filling the two lists
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly
FilterHarmonicPlugin::FilterHarmonicPlugin()
{
	typeList << FP_SCALAR_HARMONIC_FIELD;

	foreach(FilterIDType tt , types())
		actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action
// (this string is used also to define the menu entry)
QString FilterHarmonicPlugin::filterName(FilterIDType filterId) const
{
	switch(filterId) {
	case FP_SCALAR_HARMONIC_FIELD : return QString("Generate Scalar Harmonic Field");
	default : assert(0);
	}
	return QString();
}

// Info() must return the longer string describing each filtering action
// (this string is used in the About plugin dialog)
QString FilterHarmonicPlugin::filterInfo(FilterIDType filterId) const
{
	switch(filterId)
	{
	case FP_SCALAR_HARMONIC_FIELD : return QString("Generates a scalar harmonic field over the mesh. Scalar values must be assigned to at least two vertices as Dirichlet boundary conditions. Applying the filter, a discrete Laplace operator generates the harmonic field values for all the mesh vertices. Note that the field values is stored in the quality per vertex attribute of the mesh\n\nFor more details see:\n Kai Xua, Hao Zhang, Daniel Cohen-Or, Yueshan Xionga,'Dynamic Harmonic Fields for Surface Processing'.\nin Computers & Graphics, 2009");
	default : assert(0);
	}
	return QString("Unknown Filter");
}


// The FilterClass describes in which generic class of filters it fits.
// This choice affect the submenu in which each filter will be placed
// More than a single class can be choosen.
FilterHarmonicPlugin::FilterClass FilterHarmonicPlugin::getClass(QAction * a)
{
	switch(ID(a))
	{
	case FP_SCALAR_HARMONIC_FIELD : return MeshFilterInterface::Remeshing;
	default : assert(0);
	}
	return MeshFilterInterface::Generic;
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parameter you need to define,
// - the name of the parameter,
// - the string shown in the dialog
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)

void FilterHarmonicPlugin::initParameterSet(QAction * action, MeshModel & m, RichParameterSet & parlst)
{
	switch(ID(action))
	{
	case FP_SCALAR_HARMONIC_FIELD :
		parlst.addParam(new RichPoint3f("point1", m.cm.bbox.min,"Point 1","A vertex on the mesh that represent one harmonic field boundary condition."));
		parlst.addParam(new RichPoint3f("point2", m.cm.bbox.min,"Point 2","A vertex on the mesh that represent one harmonic field boundary condition."));
		parlst.addParam(new RichDynamicFloat("value1", 0.0f, 0.0f, 1.0f,"value for the 1st point","Harmonic field value for the vertex."));
		parlst.addParam(new RichDynamicFloat("value2", 1.0f, 0.0f, 1.0f,"value for the 2nd point","Harmonic field value for the vertex."));
		parlst.addParam(new RichBool("colorize", true, "Colorize", "Colorize the mesh to provide an indication of the obtained harmonic field."));
		break;
	default : assert(0);
	}
}

enum WeightInfo
{
	Success            = 0,
	EdgeAlreadyVisited
};

// Function to compute the cotangent weighting function for an edge
// return Success             if everything is fine;
//        EdgeAlreadyVisited  if the weight for the considered edge have been already computed and assigned;
template <typename FaceType, typename ScalarType = double>
WeightInfo ComputeWeight(const FaceType &f, int edge, ScalarType & weight)
{
	typedef typename FaceType::VertexType VertexType;

	assert(edge >= 0 && edge < 3);

	// get the adjacent face
	const FaceType * fp = f.cFFp(edge);

	ScalarType cotA = 0;
	ScalarType cotB = 0;

	// Get the edge (a pair of vertices)
	VertexType * v0 = f.cV0(edge);
	VertexType * v1 = f.cV1(edge);

	if (fp != NULL &&
	    fp != &f)
	{
		// not a border edge
		if (fp->IsV()) return EdgeAlreadyVisited;

		VertexType * vb = fp->cV2(f.cFFi(edge));
		ScalarType angleB = ComputeAngle(v0, vb, v1);
		cotB = vcg::math::Cos(angleB) / vcg::math::Sin(angleB);
	}

	VertexType * va = f.cV2(edge);
	ScalarType angleA = ComputeAngle(v0, va, v1);
	cotA = vcg::math::Cos(angleA) / vcg::math::Sin(angleA);

	weight = (cotA + cotB) / 2;

	return Success;
}

template <typename VertexType, typename ScalarType = double>
ScalarType ComputeAngle(const VertexType * a, const VertexType * b, const VertexType * c)
{
	assert(a != NULL && b != NULL && c != NULL);
	ScalarType angle = vcg::Angle(a->P() - b->P(), c->P() - b->P());
	return angle;
}

// The Real Core Function doing the actual mesh processing.
bool FilterHarmonicPlugin::applyFilter(QAction * action, MeshDocument & md, RichParameterSet & par, vcg::CallBackPos * cb)
{
	switch(ID(action))
	{
	case FP_SCALAR_HARMONIC_FIELD :
	{
		typedef vcg::GridStaticPtr<CMeshO::VertexType, CMeshO::ScalarType> VertexGrid;

		typedef double                           CoeffScalar; // TODO, when moving the code to a class make it a template (CoeffScalar = double)

		typedef CMeshO::ScalarType               ScalarType;
		typedef CMeshO::CoordType                CoordType;
		typedef CMeshO::VertexType               VertexType;
		typedef CMeshO::FaceType                 FaceType;

		typedef Eigen::Triplet<CoeffScalar>      T;
		typedef Eigen::SparseMatrix<CoeffScalar> SpMat; //sparse matrix type of double


		CMeshO & m = md.mm()->cm;

		vcg::tri::Allocator<CMeshO>::CompactFaceVector(m);
		vcg::tri::Allocator<CMeshO>::CompactVertexVector(m);

		md.mm()->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_VERTMARK);
		vcg::tri::UpdateBounding<CMeshO>::Box(m);
		vcg::tri::UpdateTopology<CMeshO>::FaceFace(m);

		int n  = m.VN();
		int fn = m.FN();

		std::vector<T>             coeffs; // coefficients of the system
		std::map<size_t,CoeffScalar> sums; // row sum of the coefficient

		SpMat laplaceMat; // the system to be solved
		laplaceMat.resize(n, n);

		cb(0, "Generating coefficients...");
		vcg::tri::UpdateFlags<CMeshO>::FaceClearV(m);
		// Iterate over the faces
		for (size_t i = 0; i < m.face.size(); ++i)
		{
			CMeshO::FaceType & f = m.face[i];

			if (f.IsD())
			{
				assert(int(i) == fn);
				break; // TODO FIX the indexing of vertices
			}

			assert(!f.IsV());
			f.SetV();

			// Generate coefficients for each edge
			for (int idx = 0; idx < 3; ++idx)
			{
				CoeffScalar weight;
				WeightInfo res = ComputeWeight<FaceType, CoeffScalar>(f, idx, weight);

				switch (res)
				{
				case EdgeAlreadyVisited : continue;
				case Success            : break;
				default: assert(0);
				}

//				if (weight < 0) weight = 0; // TODO check if negative weight may be an issue

				// Add the weight to the coefficients vector for both the vertices of the considered edge
				size_t v0_idx = vcg::tri::Index(m, f.V0(idx));
				size_t v1_idx = vcg::tri::Index(m, f.V1(idx));

				coeffs.push_back(T(v0_idx, v1_idx, -weight));
				coeffs.push_back(T(v1_idx, v0_idx, -weight));

				// Add the weight to the row sum
				sums[v0_idx] += weight;
				sums[v1_idx] += weight;
			}

			f.SetV();
		}

		// Fill the system matrix
		cb(10, "Filling the system matrix...");
		laplaceMat.reserve(coeffs.size());
		for (std::map<size_t,CoeffScalar>::const_iterator it = sums.begin(); it != sums.end(); ++it)
		{
			coeffs.push_back(T(it->first, it->first, it->second));
		}
		laplaceMat.setFromTriplets(coeffs.begin(), coeffs.end());

		// Get the two vertices with value set
		VertexGrid vg;
		vg.Set(m.vert.begin(), m.vert.end());

		vcg::vertex::PointDistanceFunctor<ScalarType> pd;
		vcg::tri::Tmark<CMeshO, VertexType> mv;
		mv.SetMesh(&m);
		mv.UnMarkAll();
		CoordType  closestP;
		ScalarType minDist = 0;
		VertexType * vp0 = vcg::GridClosest(vg, pd, mv, par.getPoint3f("point1"), m.bbox.Diag(), minDist, closestP);
		VertexType * vp1 = vcg::GridClosest(vg, pd, mv, par.getPoint3f("point2"), m.bbox.Diag(), minDist, closestP);
		if (vp0 == NULL || vp1 == NULL || vp0 == vp1)
		{
			this->errorMessage = "Error occurred for selected points.";
			return false;
		}

		size_t v0_idx = vcg::tri::Index(m, vp0);
		size_t v1_idx = vcg::tri::Index(m, vp1);

		// Add penalty factor alpha
		const CoeffScalar alpha = pow(10, 8);

		Eigen::Matrix<CoeffScalar, Eigen::Dynamic, 1> b, x; // Unknown and known terms vectors
		b.setZero(n);
		b(v0_idx) = alpha * par.getFloat("value1");
		b(v1_idx) = alpha * par.getFloat("value2");

		laplaceMat.coeffRef(v0_idx, v0_idx) += alpha;
		laplaceMat.coeffRef(v1_idx, v1_idx) += alpha;

		// Solve system laplacianMat x = b
		cb(20, "System matrix decomposition...");
		Eigen::SimplicialLDLT<Eigen::SparseMatrix<CoeffScalar> > solver; // TODO eventually use another solver
		solver.compute(laplaceMat);
		if(solver.info() != Eigen::Success)
		{
			// decomposition failed
			this->errorMessage = "System matrix decomposition failed: ";
			if (solver.info() == Eigen::NumericalIssue)
				this->errorMessage += "numerical issue.";
			else if (solver.info() == Eigen::NoConvergence)
				this->errorMessage += "no convergence.";
			else if (solver.info() == Eigen::InvalidInput)
				this->errorMessage += "invalid input.";

			return false;
		}

		cb(85, "Solving the system...");
		x = solver.solve(b);
		if(solver.info() != Eigen::Success)
		{
			// solving failed
			this->errorMessage = "System solving failed.";
			return false;
		}

		// Colorize bands for the 0-1 interval
		if (par.getBool("colorize"))
		{
			float steps = 20.0f;
			for (size_t i = 0; int(i) < n; ++i)
			{
				bool on = (int)(x[i]*steps)%2 == 1;
				if (on)
				{
					m.vert[i].C() = vcg::Color4b::ColorRamp(0,2,x[i]);
				}
				else
				{
					m.vert[i].C() = vcg::Color4b::White;
				}
			}
		}

		// Set field value into vertex quality attribute
		for (size_t i = 0; int(i) < n; ++i)
		{
			m.vert[i].Q() = x[i];
		}

		cb(100, "Done.");

		return true;
	}
	default : assert(0);
	}
	return false;
}

QString FilterHarmonicPlugin::filterScriptFunctionName( FilterIDType filterID )
{
	switch(filterID)
	{
	case FP_SCALAR_HARMONIC_FIELD : return QString("scalarHarmonicField");
	default : assert(0);
	}
	return QString();
}

Q_EXPORT_PLUGIN(FilterHarmonicPlugin)
