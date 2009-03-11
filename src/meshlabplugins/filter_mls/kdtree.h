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

#ifndef KDTREE_H
#define KDTREE_H

#include <vcg/space/point3.h>
#include <vcg/space/box3.h>
#include "mlsutils.h"
#include "priorityqueue.h"
#include <vector>

template<typename _Scalar>
class KdTree
{
public:

	typedef _Scalar Scalar;
	typedef vcg::Point3<Scalar> VectorType;
	typedef vcg::Box3<Scalar> AxisAlignedBoxType;

	struct Node
	{
		union {
			struct {
				Scalar splitValue;
				unsigned int firstChildId:24;
				unsigned int dim:2;
				unsigned int leaf:1;
			};
			struct {
				unsigned int start;
				unsigned short size;
			};
		};
	};
	typedef std::vector<Node> NodeList;

	inline const NodeList& _getNodes(void) { return mNodes; }
	inline const std::vector<VectorType>& _getPoints(void) { return mPoints; }

	void setMaxNofNeighbors(unsigned int k);
	inline int getNofFoundNeighbors(void) { return mNeighborQueue.getNofElements(); }
	inline const VectorType& getNeighbor(int i) { return mPoints[ mNeighborQueue.getIndex(i) ]; }
	inline unsigned int getNeighborId(int i) { return mIndices[mNeighborQueue.getIndex(i)]; }
	inline float getNeighborSquaredDistance(int i) { return mNeighborQueue.getWeight(i); }

public:

	KdTree(const ConstDataWrapper<VectorType>& points, unsigned int nofPointsPerCell = 16, unsigned int maxDepth = 64);

	~KdTree();

	void doQueryK(const VectorType& p);

protected:

	// element of the stack
	struct QueryNode
	{
			QueryNode() {}
			QueryNode(unsigned int id) : nodeId(id) {}
			unsigned int nodeId;  // id of the next node
			Scalar sq;            // squared distance to the next node
	};

	// used to build the tree: split the subset [start..end[ according to dim and splitValue,
	// and returns the index of the first element of the second subset
	unsigned int split(int start, int end, unsigned int dim, float splitValue);

	void createTree(unsigned int nodeId, unsigned int start, unsigned int end, unsigned int level, unsigned int targetCellsize, unsigned int targetMaxDepth);

protected:

	AxisAlignedBoxType mAABB;
	NodeList mNodes;
	std::vector<VectorType> mPoints;
	std::vector<int> mIndices;
	HeapMaxPriorityQueue<int,Scalar> mNeighborQueue;
	QueryNode mNodeStack[64];
};

#endif

