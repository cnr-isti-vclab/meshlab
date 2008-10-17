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

#ifndef BALLTREE_H
#define BALLTREE_H

#include <vcg/space/point3.h>
#include <vcg/space/box3.h>
#include "mlsutils.h"

namespace GaelMls {

template<typename _Scalar>
class Neighborhood
{
	public:
		typedef _Scalar Scalar;

		int index(int i) const { return mIndices.at(i); }
		Scalar squaredDistance(int i) const { return mSqDists.at(i); }

		void clear() { mIndices.clear(); mSqDists.clear(); }
		void resize(int size) { mIndices.resize(size); mSqDists.resize(size); }
		void reserve(int size) { mIndices.reserve(size); mSqDists.reserve(size); }
		int size() { return mIndices.size(); }

		void insert(int id, Scalar d2) { mIndices.push_back(id); mSqDists.push_back(d2); }

	protected:
		std::vector<int> mIndices;
		std::vector<Scalar> mSqDists;
};

template<typename _Scalar>
class BallTree
{
	public:
		typedef _Scalar Scalar;
		typedef vcg::Point3<Scalar> VectorType;

		BallTree(const ConstDataWrapper<VectorType>& points, const ConstDataWrapper<Scalar>& radii);

		void computeNeighbors(const VectorType& x, Neighborhood<Scalar>* pNei) const;

		void setRadiusScale(Scalar v) { mRadiusScale = v; mTreeIsUptodate = false; }

	protected:

		struct Node
		{
			~Node()
			{
				if (!leaf)
				{
					delete children[0];
					delete children[1];
				}
				else
				{
					delete[] indices;
				}
			}
			Scalar splitValue;
			unsigned char dim:2;
			unsigned char leaf:1;
			union {
				Node* children[2];
				struct {
					unsigned int* indices;
					unsigned int size;
				};
			};
		};

		typedef std::vector<int> IndexArray;
		typedef vcg::Box3<Scalar> AxisAlignedBoxType;

		void rebuild();
		void split(const IndexArray& indices, const AxisAlignedBoxType& aabbLeft, const AxisAlignedBoxType& aabbRight,
							IndexArray& iLeft, IndexArray& iRight);
		void buildNode(Node& node, std::vector<int>& indices, AxisAlignedBoxType aabb, int level);
		void queryNode(Node& node, Neighborhood<Scalar>* pNei) const;

	protected:
		ConstDataWrapper<VectorType> mPoints;
		ConstDataWrapper<Scalar> mRadii;
		Scalar mRadiusScale;

		int mMaxTreeDepth;
		int mTargetCellSize;
		mutable bool mTreeIsUptodate;
		mutable VectorType mQueryPosition;

		Node* mRootNode;
};

}

#endif
