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

#include "kdtree.h"
#include <limits>
#include <iostream>

template<typename Scalar>
KdTree<Scalar>::KdTree(const ConstDataWrapper<VectorType>& points, unsigned int nofPointsPerCell, unsigned int maxDepth)
	: mPoints(points.size()), mIndices(points.size())
{
	// compute the AABB of the input
	mPoints[0] = points[0];
	mAABB.Set(mPoints[0]);
	for (unsigned int i=1 ; i<mPoints.size() ; ++i)
	{
		mPoints[i] = points[i];
		mIndices[i] = i;
		mAABB.Add(mPoints[i]);
	}

	mNodes.reserve(4*mPoints.size()/nofPointsPerCell);
	mNodes.resize(1);
	mNodes.back().leaf = 0;
	createTree(0, 0, mPoints.size(), 1, nofPointsPerCell, maxDepth);
}

template<typename Scalar>
KdTree<Scalar>::~KdTree()
{
}

template<typename Scalar>
void KdTree<Scalar>::setMaxNofNeighbors(unsigned int k)
{
	mNeighborQueue.setMaxSize(k);
}

/** Performs the kNN query.
	*
	* This algorithm uses the simple distance to the split plane to prune nodes.
	* A more elaborated approach consists to track the closest corner of the cell
	* relatively to the current query point. This strategy allows to save about 5%
	* of the leaves. However, in practice the slight overhead due to this tracking
	* reduces the overall performance.
	*
	* This algorithm also use a simple stack while a priority queue using the squared
	* distances to the cells as a priority values allows to save about 10% of the leaves.
	* But, again, priority queue insertions and deletions are quite involved, and therefore
	* a simple stack is by far much faster.
	*/
template<typename Scalar>
void KdTree<Scalar>::doQueryK(const VectorType& queryPoint)
{
	mNeighborQueue.init();
	mNeighborQueue.insert(0xffffffff, std::numeric_limits<float>::max());

	mNodeStack[0].nodeId = 0;
	mNodeStack[0].sq = 0.f;
	unsigned int count = 1;

	while (count)
	{
		QueryNode& qnode = mNodeStack[count-1];
		Node& node = mNodes[qnode.nodeId];

		if (qnode.sq < mNeighborQueue.getTopWeight())
		{
			if (node.leaf)
			{
				--count; // pop
				unsigned int end = node.start+node.size;
				for (unsigned int i=node.start ; i<end ; ++i)
						mNeighborQueue.insert(i, vcg::SquaredNorm(queryPoint - mPoints[i]));
			}
			else
			{
				// replace the stack top by the farthest and push the closest
				float new_off = queryPoint[node.dim] - node.splitValue;
				if (new_off < 0.)
				{
						mNodeStack[count].nodeId  = node.firstChildId;
						qnode.nodeId = node.firstChildId+1;
				}
				else
				{
						mNodeStack[count].nodeId  = node.firstChildId+1;
						qnode.nodeId = node.firstChildId;
				}
				mNodeStack[count].sq = qnode.sq;
				qnode.sq = new_off*new_off;
				++count;
			}
		}
		else
		{
			// pop
			--count;
		}
	}
}

template<typename Scalar>
unsigned int KdTree<Scalar>::split(int start, int end, unsigned int dim, float splitValue)
{
	int l(start), r(end-1);
	for ( ; l<r ; ++l, --r)
	{
		while (l < end && mPoints[l][dim] < splitValue)
			l++;
		while (r >= start && mPoints[r][dim] >= splitValue)
			r--;
		if (l > r)
			break;
		std::swap(mPoints[l],mPoints[r]);
		std::swap(mIndices[l],mIndices[r]);
	}
	return (mPoints[l][dim] < splitValue ? l+1 : l);
}

/** recursively builds the kdtree
	*
	*  The heuristic is the following:
	*   - if the number of points in the node is lower than targetCellsize then make a leaf
	*   - else compute the AABB of the points of the node and split it at the middle of
	*     the largest AABB dimension.
	*
	*  This strategy might look not optimal because it does not explicitly prune empty space,
	*  unlike more advanced SAH-like techniques used for RT. On the other hand it leads to a shorter tree,
	*  faster to traverse and our experience shown that in the special case of kNN queries,
	*  this strategy is indeed more efficient (and much faster to build). Moreover, for volume data
	*  (e.g., fluid simulation) pruning the empty space is useless.
	*
	*  Actually, storing at each node the exact AABB (we therefore have a binary BVH) allows
	*  to prune only about 10% of the leaves, but the overhead of this pruning (ball/ABBB intersection)
	*  is more expensive than the gain it provides and the memory consumption is x4 higher !
	*/
template<typename Scalar>
void KdTree<Scalar>::createTree(unsigned int nodeId, unsigned int start, unsigned int end, unsigned int level, unsigned int targetCellSize, unsigned int targetMaxDepth)
{
	Node& node = mNodes[nodeId];
	AxisAlignedBoxType aabb;
	aabb.Set(mPoints[start]);
	for (unsigned int i=start+1 ; i<end ; ++i)
		aabb.Add(mPoints[i]);

	VectorType diag = aabb.max - aabb.min;
	unsigned int dim = vcg::MaxCoeffId(diag);
	node.dim = dim;
	node.splitValue = Scalar(0.5*(aabb.max[dim] + aabb.min[dim]));

	unsigned int midId = split(start, end, dim, node.splitValue);

	node.firstChildId = mNodes.size();
	mNodes.resize(mNodes.size()+2);

	{
		// left child
		unsigned int childId = mNodes[nodeId].firstChildId;
		Node& child = mNodes[childId];
		if (midId-start <= targetCellSize || level>=targetMaxDepth)
		{
				child.leaf = 1;
				child.start = start;
				child.size = midId-start;
		}
		else
		{
				child.leaf = 0;
				createTree(childId, start, midId, level+1, targetCellSize, targetMaxDepth);
		}
	}

	{
		// right child
		unsigned int childId = mNodes[nodeId].firstChildId+1;
		Node& child = mNodes[childId];
		if (end-midId <= targetCellSize || level>=targetMaxDepth)
		{
			child.leaf = 1;
			child.start = midId;
			child.size = end-midId;
		}
		else
		{
			child.leaf = 0;
			createTree(childId, midId, end, level+1, targetCellSize, targetMaxDepth);
		}
	}
}

template class KdTree<float>;
template class KdTree<double>;
