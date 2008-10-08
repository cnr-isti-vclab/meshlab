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
#include "vcg_addons.h"
#include <iostream>
#include <limits>
#include <vcg/space/index/octree.h>

template<typename _Scalar>
MlsSurface<_Scalar>::MlsSurface(const MeshModel& m)
	: mMesh(m)
{
	mCachedQueryPointIsOK = false;

	mPoints.resize(m.cm.vert.size());
	mNormals.resize(m.cm.vert.size());
	mRadii.resize(m.cm.vert.size());
	mAABB.Set(mMesh.cm.vert[0].cP());
	for (uint i = 0; i< m.cm.vert.size(); i++)
	{
		mPoints[i] = /*vcg::vector_cast<Scalar>*/(mMesh.cm.vert[i].cP());
		mNormals[i] = /*vcg::vector_cast<Scalar>*/(mMesh.cm.vert[i].cN());
		mAABB.Add(mMesh.cm.vert[i].cP());
	}

	// compute radii using a basic meshless density estimator
	computeVertexRaddi();

	mFilterScale = 4.0;
	mMaxNofProjectionIterations = 20;
	mProjectionAccuracy = 1e-4;
	mBallTree = 0;
}

template<typename Scalar>
void MlsSurface<Scalar>::setFilterScale(Scalar v)
{
	mFilterScale = v;
	mCachedQueryPointIsOK = false;
	if (mBallTree)
		mBallTree->setRadiusScale(mFilterScale);
}

template<typename Scalar>
void MlsSurface<Scalar>::setMaxProjectionIters(int n)
{
	mMaxNofProjectionIterations = n;
	mCachedQueryPointIsOK = false;
}

template<typename Scalar>
void MlsSurface<Scalar>::setProjectionAccuracy(Scalar v)
{
	mProjectionAccuracy = v;
	mCachedQueryPointIsOK = false;
}

template<typename Scalar>
void MlsSurface<Scalar>::computeVertexRaddi()
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

	int nbNeighbors = 16;
	KdTree<Scalar> knn(mPoints);
	knn.setMaxNofNeighbors(nbNeighbors);
	mAveragePointSpacing = 0;
	for (uint i = 0; i< mPoints.size(); i++)
	{
		knn.doQueryK(mPoints[i]);
		mRadii[i] = 2. * sqrt(knn.getNeighborSquaredDistance(0)/Scalar(knn.getNofFoundNeighbors()));
		mAveragePointSpacing += mRadii[i];
	}
	mAveragePointSpacing /= Scalar(mPoints.size());

	#endif
}

template<typename Scalar>
void MlsSurface<Scalar>::computeNeighborhood(const VectorType& x, bool computeDerivatives) const
{
	if (!mBallTree)
	{
		const_cast<BallTree<Scalar>*&>(mBallTree) = new BallTree<Scalar>(mPoints, mRadii);
		const_cast<BallTree<Scalar>*>(mBallTree)->setRadiusScale(mFilterScale);
	}
	mBallTree->computeNeighbors(x, &mNeighborhood);
	uint nofSamples = mNeighborhood.size();

	// compute spatial weights and partial derivatives
	mCachedWeights.resize(nofSamples);
	if (computeDerivatives)
		mCachedWeightGradients.resize(nofSamples);
	else
		mCachedWeightGradients.clear();

	for (uint i=0; i<nofSamples; i++)
	{
		int id = mNeighborhood.index(i);
		Scalar s = 1./(mRadii[id]*mFilterScale);
		s = s*s;
		Scalar w = Scalar(1) - mNeighborhood.squaredDistance(i) * s;
		if (w<0)
			w = 0;
		Scalar aux = w;
		w = w * w;
		w = w * w;
		mCachedWeights[i] = w;

		if (computeDerivatives)
			mCachedWeightGradients[i] = (x - mPoints[id]) * (-4. * 2. * s * aux * aux * aux);
	}
}

template class MlsSurface<float>;
// template class MlsSurface<double>;
