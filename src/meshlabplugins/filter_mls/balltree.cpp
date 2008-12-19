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

#include "balltree.h"

namespace GaelMls {

template<typename _Scalar>
BallTree<_Scalar>::BallTree(const ConstDataWrapper<VectorType>& points, const ConstDataWrapper<Scalar>& radii)
	: mPoints(points), mRadii(radii), mRadiusScale(1.), mTreeIsUptodate(false)
{
	mRootNode = 0;
	mMaxTreeDepth = 12;
	mTargetCellSize = 24;
}

template<typename _Scalar>
void BallTree<_Scalar>::computeNeighbors(const VectorType& x, Neighborhood<Scalar>* pNei) const
{
	if (!mTreeIsUptodate)
		const_cast<BallTree*>(this)->rebuild();

	pNei->clear();
	mQueryPosition = x;
	queryNode(*mRootNode, pNei);
}

template<typename _Scalar>
void BallTree<_Scalar>::queryNode(Node& node, Neighborhood<Scalar>* pNei) const
{
	if (node.leaf)
	{
		for (unsigned int i=0 ; i<node.size ; ++i)
		{
			int id = node.indices[i];
			Scalar d2 = vcg::SquaredNorm(mQueryPosition - mPoints[id]);
			Scalar r = mRadiusScale * mRadii[id];
			if (d2<r*r)
				pNei->insert(id, d2);
		}
	}
	else
	{
		if (mQueryPosition[node.dim] - node.splitValue < 0)
			queryNode(*node.children[0], pNei);
		else
			queryNode(*node.children[1], pNei);
	}
}

template<typename _Scalar>
void BallTree<_Scalar>::rebuild(void)
{
		delete mRootNode;

		mRootNode = new Node();
		IndexArray indices(mPoints.size());
		AxisAlignedBoxType aabb;
		aabb.Set(mPoints[0]);
		for (unsigned int i=0 ; i<mPoints.size() ; ++i)
		{
				indices[i] = i;
				aabb.min = Min(aabb.min, CwiseAdd(mPoints[i], -mRadii[i]*mRadiusScale));
				aabb.max = Max(aabb.max, CwiseAdd(mPoints[i],  mRadii[i]*mRadiusScale));
		}
		buildNode(*mRootNode, indices, aabb, 0);

		mTreeIsUptodate = true;
}

template<typename _Scalar>
void BallTree<_Scalar>::split(const IndexArray& indices, const AxisAlignedBoxType& aabbLeft, const AxisAlignedBoxType& aabbRight, IndexArray& iLeft, IndexArray& iRight)
{
	for (std::vector<int>::const_iterator it=indices.begin(), end=indices.end() ; it!=end ; ++it)
	{
		unsigned int i = *it;
		if (vcg::Distance(mPoints[i], aabbLeft) < mRadii[i]*mRadiusScale)
			iLeft.push_back(i);

		if (vcg::Distance(mPoints[i], aabbRight) < mRadii[i]*mRadiusScale)
			iRight.push_back(i);
	}
}

template<typename _Scalar>
void BallTree<_Scalar>::buildNode(Node& node, std::vector<int>& indices, AxisAlignedBoxType aabb, int level)
{
	Scalar avgradius = 0.;
	for (std::vector<int>::const_iterator it=indices.begin(), end=indices.end() ; it!=end ; ++it)
			avgradius += mRadii[*it];
	avgradius = mRadiusScale * avgradius / Scalar(indices.size());
	VectorType diag = aabb.max - aabb.min;
	if  (int(indices.size())<mTargetCellSize
		|| avgradius*0.9 > std::max(std::max(diag.X(), diag.Y()), diag.Z())
		|| int(level)>=mMaxTreeDepth)
	{
		node.leaf = true;
		node.size = indices.size();
		node.indices = new unsigned int[node.size];
		for (unsigned int i=0 ; i<node.size ; ++i)
			node.indices[i] = indices[i];
		return;
	}
	unsigned int dim = vcg::MaxCoeffId(diag);
	node.dim = dim;
	node.splitValue = Scalar(0.5*(aabb.max[dim] + aabb.min[dim]));
  node.leaf = 0;

	AxisAlignedBoxType aabbLeft=aabb, aabbRight=aabb;
	aabbLeft.max[dim] = node.splitValue;
	aabbRight.min[dim] = node.splitValue;

	std::vector<int> iLeft, iRight;
	split(indices, aabbLeft, aabbRight, iLeft,iRight);

	// we don't need the index list anymore
	indices.clear();

	{
		// left child
		//mNodes.resize(mNodes.size()+1);
		Node* pChild = new Node();
		node.children[0] = pChild;
		buildNode(*pChild, iLeft, aabbLeft, level+1);
	}

	{
		// right child
		//mNodes.resize(mNodes.size()+1);
		Node* pChild = new Node();
		node.children[1] = pChild;
		buildNode(*pChild, iRight, aabbRight, level+1);
	}
}

template class BallTree<float>;
template class BallTree<double>;

}
