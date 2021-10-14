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

#include "mlssurface.h"
#include <iostream>
#include <limits>
#include <vcg/space/index/kdtree/kdtree.h>
#include <vcg/space/index/octree.h>
#include <vcg/complex/base.h>
#include <vcg/complex/allocate.h>

namespace GaelMls {

template<typename _MeshType>
void computeVertexRadius(_MeshType& mesh, int nNeighbors)
{
	typedef typename _MeshType::ScalarType    Scalar;
	if (!vcg::tri::HasPerVertexAttribute(mesh, "radius")) {
		vcg::tri::Allocator<_MeshType>::template AddPerVertexAttribute<Scalar>(mesh, "radius");
	}

	typename _MeshType::template PerVertexAttributeHandle<Scalar> h;
	h = vcg::tri::Allocator<_MeshType>::template FindPerVertexAttribute<Scalar>(mesh, "radius");
	assert(vcg::tri::Allocator<_MeshType>::template IsValidHandle<Scalar>(mesh, h));

	auto positions = vcg::ConstDataWrapper<vcg::Point3<Scalar>>(
		&mesh.vert[0].P(),
		mesh.vert.size(),
		size_t(mesh.vert[1].P().V()) - size_t(mesh.vert[0].P().V()));

	vcg::KdTree<Scalar> knn(positions);
	typename vcg::KdTree<Scalar>::PriorityQueue pq;
	for (size_t i = 0; i < mesh.vert.size(); i++) {
		knn.doQueryK(mesh.vert[i].cP(), nNeighbors, pq);
		h[i] = 2. * sqrt(pq.getTopWeight() / Scalarm(pq.getNofElements()));
	}
}

template<typename _MeshType>
void MlsSurface<_MeshType>::setFilterScale(Scalar v)
{
	mFilterScale          = v;
	mCachedQueryPointIsOK = false;
	if (mBallTree)
		mBallTree->setRadiusScale(mFilterScale);
}

template<typename _MeshType>
void MlsSurface<_MeshType>::setMaxProjectionIters(int n)
{
	mMaxNofProjectionIterations = n;
	mCachedQueryPointIsOK       = false;
}

template<typename _MeshType>
void MlsSurface<_MeshType>::setProjectionAccuracy(Scalar v)
{
	mProjectionAccuracy   = v;
	mCachedQueryPointIsOK = false;
}

template<typename _MeshType>
void MlsSurface<_MeshType>::setGradientHint(int h)
{
	mGradientHint         = h;
	mCachedQueryPointIsOK = false;
}

template<typename _MeshType>
void MlsSurface<_MeshType>::setHessianHint(int h)
{
	mHessianHint          = h;
	mCachedQueryPointIsOK = false;
}

template<typename _MeshType>
void MlsSurface<_MeshType>::computeNeighborhood(const VectorType& x, bool computeDerivatives) const
{
	if (!mBallTree) {
		const_cast<BallTree<Scalar>*&>(mBallTree) = new BallTree<Scalar>(positions(), radii());
		const_cast<BallTree<Scalar>*>(mBallTree)->setRadiusScale(mFilterScale);
	}
	mBallTree->computeNeighbors(x, &mNeighborhood);
	size_t nofSamples = mNeighborhood.size();

	// compute spatial weights and partial derivatives
	mCachedWeights.resize(nofSamples);
	if (computeDerivatives) {
		mCachedWeightDerivatives.resize(nofSamples);
		mCachedWeightGradients.resize(nofSamples);
	}
	else
		mCachedWeightGradients.clear();

	typename _MeshType::template ConstPerVertexAttributeHandle<Scalar> h;
	h = vcg::tri::Allocator<_MeshType>::template FindPerVertexAttribute<Scalar>(mMesh, "radius");
	assert(vcg::tri::Allocator<_MeshType>::template IsValidHandle<Scalar>(mMesh, h));

	for (size_t i = 0; i < nofSamples; i++) {
		int    id = mNeighborhood.index(i);
		Scalar s  = 1. / (h[id] * mFilterScale);
		s         = s * s;
		Scalar w  = Scalar(1) - mNeighborhood.squaredDistance(i) * s;
		if (w < 0)
			w = 0;
		Scalar aux        = w;
		w                 = w * w;
		w                 = w * w;
		mCachedWeights[i] = w;

		if (computeDerivatives) {
			mCachedWeightDerivatives[i] = (-2. * s) * (4. * aux * aux * aux);
			mCachedWeightGradients[i]   = (x - mMesh.vert[id].cP()) * mCachedWeightDerivatives[i];
		}
	}
}

template<typename _MeshType>
void MlsSurface<_MeshType>::requestSecondDerivatives() const
{
	typename _MeshType::template ConstPerVertexAttributeHandle<Scalar> h;
	h = vcg::tri::Allocator<_MeshType>::template FindPerVertexAttribute<Scalar>(mMesh, "radius");
	assert(vcg::tri::Allocator<_MeshType>::template IsValidHandle<Scalar>(mMesh, h));

	// if (!mSecondDerivativeUptodate)
	{
		size_t nofSamples = mNeighborhood.size();
		if (nofSamples > mCachedWeightSecondDerivatives.size())
			mCachedWeightSecondDerivatives.resize(nofSamples + 10);

		{
			for (size_t i = 0; i < nofSamples; ++i) {
				int    id = mNeighborhood.index(i);
				Scalar s  = 1. / (h[id] * mFilterScale);
				s         = s * s;
				Scalar x2 = s * mNeighborhood.squaredDistance(i);
				x2        = 1.0 - x2;
				if (x2 < 0)
					x2 = 0.;
				mCachedWeightSecondDerivatives[i] = (4.0 * s * s) * (12.0 * x2 * x2);
			}
		}
		// mSecondDerivativeUptodate = true;
	}
}

template<typename _MeshType>
typename MlsSurface<_MeshType>::Scalar
MlsSurface<_MeshType>::meanCurvature(const VectorType& gradient, const MatrixType& hessian) const
{
	Scalar gl = gradient.Norm();
	//	return (gl*gl*hessian.Trace() - vcg::Dot(gradient, VectorType(hessian * gradient))) /
	//(2.*gl*gl*gl);
	return (gl * gl * hessian.Trace() - (gradient * VectorType(hessian * gradient))) /
		   (2. * gl * gl * gl);
}

template<typename _MeshType>
bool MlsSurface<_MeshType>::isInDomain(const VectorType& x) const
{
	typename _MeshType::template ConstPerVertexAttributeHandle<Scalar> h;
	h = vcg::tri::Allocator<_MeshType>::template FindPerVertexAttribute<Scalar>(mMesh, "radius");
	assert(vcg::tri::Allocator<_MeshType>::template IsValidHandle<Scalar>(mMesh, h));


	if ((!mCachedQueryPointIsOK) || mCachedQueryPoint != x) {
		computeNeighborhood(x, false);
	}
	int nb = mNeighborhood.size();
	if (nb < mDomainMinNofNeighbors)
		return false;

	int  i         = 0;
	bool out       = true;
	bool hasNormal = true;
	if ((mDomainNormalScale == 1.f) || (!hasNormal)) {
		while (out && i < nb) {
			int    id  = mNeighborhood.index(i);
			Scalar rs2 = h[id] * mDomainRadiusScale;
			rs2        = rs2 * rs2;
			out        = mNeighborhood.squaredDistance(i) > rs2;
			++i;
		}
	}
	else {
		Scalar s = 1. / (mDomainNormalScale * mDomainNormalScale) - 1.f;
		while (out && i < nb) {
			int    id  = mNeighborhood.index(i);
			Scalar rs2 = h[id] * mDomainRadiusScale;
			rs2        = rs2 * rs2;
			Scalar dn  = mMesh.vert[id].cN().dot(x - mMesh.vert[id].cP());
			out        = (mNeighborhood.squaredDistance(i) + s * dn * dn) > rs2;
			++i;
		}
	}
	return !out;
}

} // namespace GaelMls
