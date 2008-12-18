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

#include "mlssurface.h"
#include "kdtree.h"
#include "mlsutils.h"
#include <iostream>
#include <limits>
#include <vcg/space/index/octree.h>

namespace GaelMls {

// template<typename _Scalar>
// MlsSurface<_Scalar>::MlsSurface(const MeshModel& m)
// 	: mMesh(m)
// {
// 	mCachedQueryPointIsOK = false;
//
// 	mPoints.resize(m.cm.vert.size());
// 	mNormals.resize(m.cm.vert.size());
// 	mRadii.resize(m.cm.vert.size());
// 	mAABB.Set(mMesh.cm.vert[0].cP());
// 	for (uint i = 0; i< m.cm.vert.size(); i++)
// 	{
// 		mPoints[i] = /*vcg::vector_cast<Scalar>*/(mMesh.cm.vert[i].cP());
// 		mNormals[i] = /*vcg::vector_cast<Scalar>*/(mMesh.cm.vert[i].cN());
// 		mAABB.Add(mMesh.cm.vert[i].cP());
// 	}
//
// 	// compute radii using a basic meshless density estimator
// 	computeVertexRaddi();
//
// 	mFilterScale = 4.0;
// 	mMaxNofProjectionIterations = 20;
// 	mProjectionAccuracy = 1e-4;
// 	mBallTree = 0;
// }

template<typename _MeshType>
void MlsSurface<_MeshType>::setFilterScale(Scalar v)
{
	mFilterScale = v;
	mCachedQueryPointIsOK = false;
	if (mBallTree)
		mBallTree->setRadiusScale(mFilterScale);
}

template<typename _MeshType>
void MlsSurface<_MeshType>::setMaxProjectionIters(int n)
{
	mMaxNofProjectionIterations = n;
	mCachedQueryPointIsOK = false;
}

template<typename _MeshType>
void MlsSurface<_MeshType>::setProjectionAccuracy(Scalar v)
{
	mProjectionAccuracy = v;
	mCachedQueryPointIsOK = false;
}

template<typename _MeshType>
void MlsSurface<_MeshType>::setGradientHint(int h)
{
	mGradientHint = h;
	mCachedQueryPointIsOK = false;
}

template<typename _MeshType>
void MlsSurface<_MeshType>::setHessianHint(int h)
{
	mHessianHint = h;
	mCachedQueryPointIsOK = false;
}

template<typename _MeshType>
void MlsSurface<_MeshType>::computeVertexRaddi(const int nbNeighbors)
{
	#if 0
	int nbNeighbors = 16;
	vcg::Octree<VectorType, Scalar> knn;
	knn.Set(mPoints.begin(), mPoints.end());
	std::vector<VectorType*> nearest_objects;
	std::vector<VectorType> nearest_points;
	std::vector<Scalar> sqDistances;
	mAveragePointSpacing = 0;
	for (uint i = 0; i< mPoints.size(); i++)
	{
		DummyObjectMarker dom;
		PointToPointSqDist dfunc;
		Scalar max_dist2 = 1e9;//std::numeric_limits<Scalar>::max();
		knn.GetKClosest(dfunc, dom, nbNeighbors, mPoints[i],
										max_dist2, nearest_objects, sqDistances, nearest_points);
// 		for (int j=0; i<sqDistances.size(); ++j)
// 			std::cout << sqDistances[j] << " ";
// 		std::cout << "$\n";
		mRadii[i] = 2. * sqrt(sqDistances.at(0)/nearest_objects.size());
		mAveragePointSpacing += mRadii[i];
	}
	mAveragePointSpacing /= Scalar(mPoints.size());

	#else

//	int nbNeighbors = 16;

	assert(mPoints.size()>=2);
	KdTree<Scalar> knn(positions());

	knn.setMaxNofNeighbors(nbNeighbors);
	mAveragePointSpacing = 0;
	for (size_t i = 0; i< mPoints.size(); i++)
	{
		knn.doQueryK(mPoints[i].cP());
		const_cast<PointsType&>(mPoints)[i].R() = 2. * sqrt(knn.getNeighborSquaredDistance(0)/Scalar(knn.getNofFoundNeighbors()));
		mAveragePointSpacing += mPoints[i].cR();
	}
	mAveragePointSpacing /= Scalar(mPoints.size());

	#endif
}

template<typename _MeshType>
void MlsSurface<_MeshType>::computeNeighborhood(const VectorType& x, bool computeDerivatives) const
{
	if (!mBallTree)
	{
		const_cast<BallTree<Scalar>*&>(mBallTree) = new BallTree<Scalar>(positions(), radii());
		const_cast<BallTree<Scalar>*>(mBallTree)->setRadiusScale(mFilterScale);
	}
	mBallTree->computeNeighbors(x, &mNeighborhood);
	size_t nofSamples = mNeighborhood.size();

	// compute spatial weights and partial derivatives
	mCachedWeights.resize(nofSamples);
	if (computeDerivatives)
	{
		mCachedWeightDerivatives.resize(nofSamples);
		mCachedWeightGradients.resize(nofSamples);
	}
	else
		mCachedWeightGradients.clear();

	for (size_t i=0; i<nofSamples; i++)
	{
		int id = mNeighborhood.index(i);
		Scalar s = 1./(mPoints[id].cR()*mFilterScale);
		s = s*s;
		Scalar w = Scalar(1) - mNeighborhood.squaredDistance(i) * s;
		if (w<0)
			w = 0;
		Scalar aux = w;
		w = w * w;
		w = w * w;
		mCachedWeights[i] = w;

		if (computeDerivatives)
		{
			mCachedWeightDerivatives[i] = (-2. * s) * (4. * aux * aux * aux);
			mCachedWeightGradients[i]  = (x - mPoints[id].cP()) * mCachedWeightDerivatives[i];
		}
	}
}

template<typename _MeshType>
void MlsSurface<_MeshType>::requestSecondDerivatives() const
{
	//if (!mSecondDerivativeUptodate)
	{
		size_t nofSamples = mNeighborhood.size();
		if (nofSamples>mCachedWeightSecondDerivatives.size())
			mCachedWeightSecondDerivatives.resize(nofSamples+10);

		{
			for (size_t i=0 ; i<nofSamples ; ++i)
			{
				int id = mNeighborhood.index(i);
				Scalar s = 1./(mPoints[id].cR()*mFilterScale);
				s = s*s;
				Scalar x2 = s * mNeighborhood.squaredDistance(i);
				x2 = 1.0 - x2;
				if (x2<0)
					x2 = 0.;
				mCachedWeightSecondDerivatives[i] = (4.0*s*s) * (12.0 * x2 * x2);
			}
		}
		//mSecondDerivativeUptodate = true;
	}
}

template<typename _MeshType>
typename MlsSurface<_MeshType>::Scalar
MlsSurface<_MeshType>::meanCurvature(const VectorType& gradient, const MatrixType& hessian) const
{
	Scalar gl = gradient.Norm();
	return (gl*gl*hessian.Trace() - vcg::Dot(gradient, VectorType(hessian * gradient))) / (2.*gl*gl*gl);
}

template<typename _MeshType>
bool MlsSurface<_MeshType>::isInDomain(const VectorType& x) const
{
	if ((!mCachedQueryPointIsOK) || mCachedQueryPoint!=x)
	{
		computeNeighborhood(x, false);
	}
	int nb = mNeighborhood.size();
	if (nb<mDomainMinNofNeighbors)
		return false;

	int i=0;
	bool out = true;
	bool hasNormal = true;
	if ((mDomainNormalScale==1.f) || (!hasNormal))
	{
		while (out && i<nb)
		{
			int id = mNeighborhood.index(i);
			Scalar rs2 = mPoints[id].cR() * mDomainRadiusScale;
			rs2 = rs2*rs2;
			out = mNeighborhood.squaredDistance(i) > rs2;
			++i;
		}
	}
	else
	{
		Scalar s = 1./(mDomainNormalScale*mDomainNormalScale) - 1.f;
		while (out && i<nb)
		{
			int id = mNeighborhood.index(i);
			Scalar rs2 = mPoints[id].cR() * mDomainRadiusScale;
			rs2 = rs2*rs2;
			Scalar dn = mPoints[id].cN().dot(x-mPoints[id].cP());
			out = (mNeighborhood.squaredDistance(i) + s*dn*dn) > rs2;
			++i;
		}
	}
	return !out;
}

// template class MlsSurface<float>;
// template class MlsSurface<double>;

}
