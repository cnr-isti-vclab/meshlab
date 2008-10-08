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

#ifndef MLSSURFACE_H
#define MLSSURFACE_H

#include "balltree.h"
#include <vcg/space/box3.h>
#include <iostream>

namespace GaelMls {

template<typename _Scalar>
class MlsSurface
{
	public:
		typedef _Scalar Scalar;
		typedef vcg::Point3<Scalar> VectorType;

    template<typename MeshType>
		MlsSurface(const MeshType& mesh)
    {
      mCachedQueryPointIsOK = false;

      mPoints.resize(mesh.vert.size());
      mNormals.resize(mesh.vert.size());
      mRadii.resize(mesh.vert.size());
      mAABB.Set(mesh.vert[0].cP());
      for (int i = 0; i< mesh.vert.size(); i++)
      {
        mPoints[i] = /*vcg::vector_cast<Scalar>*/(mesh.vert[i].cP());
        mNormals[i] = /*vcg::vector_cast<Scalar>*/(mesh.vert[i].cN());
        mAABB.Add(mesh.vert[i].cP());
      }

      // compute radii using a basic meshless density estimator
      computeVertexRaddi();

      mFilterScale = 4.0;
      mMaxNofProjectionIterations = 20;
      mProjectionAccuracy = 1e-4;
      mBallTree = 0;
    }

		virtual Scalar potential(const VectorType& x) const = 0;
		virtual VectorType gradient(const VectorType& x) const = 0;
		virtual VectorType project(const VectorType& x, VectorType* pNormal = 0) const = 0;

		void setFilterScale(Scalar v);
		void setMaxProjectionIters(int n);
		void setProjectionAccuracy(Scalar v);

		const std::vector<VectorType>& points() const { return mPoints; }
		const std::vector<VectorType>& normals() const { return mNormals; }
		const std::vector<Scalar>& radii() const { return mRadii; }
		const vcg::Box3<Scalar>& boundingBox() const { return mAABB; }

	protected:
		void computeNeighborhood(const VectorType& x, bool computeDerivatives) const;
		void computeVertexRaddi();

		struct PointToPointSqDist
		{
			inline bool operator()(const VectorType &a, const VectorType &b, Scalar& refD2, VectorType &q) const
			{
// 				std::cout << a.X() << a.Y() << a.Z() << "  -  " << b.X() << b.Y() << b.Z() <<
// 					" => " <<  vcg::Distance(a, b) << " < " << refD2 << "\n";
				Scalar d2 = vcg::SquaredDistance(a, b);
				if (d2>refD2)
					return false;

				refD2 = d2;
				q = a;
				return true;
			}
		};

		class DummyObjectMarker {};

	protected:
		//const MeshType& mMesh;
		std::vector<VectorType> mPoints;
		std::vector<VectorType> mNormals;
		std::vector<Scalar> mRadii;
		vcg::Box3<Scalar> mAABB;

		BallTree<Scalar>* mBallTree;

		int mMaxNofProjectionIterations;
		Scalar mFilterScale;
		Scalar mAveragePointSpacing;
		Scalar mProjectionAccuracy;

		// cached values:
		mutable bool mCachedQueryPointIsOK;
		mutable VectorType mCachedQueryPoint;
//     mutable VectorType mCachedGradient;
//     mutable Scalar mCachedPotential;
		mutable Neighborhood<Scalar> mNeighborhood;
		mutable std::vector<Scalar>  mCachedWeights;
		mutable std::vector<VectorType>  mCachedWeightGradients;
};

} // namespace

#endif // MLSSURFACE_H
