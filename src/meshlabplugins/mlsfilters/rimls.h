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

#ifndef RIMLS_H
#define RIMLS_H

#include "mlssurface.h"

namespace GaelMls {

template<typename _Scalar>
class RIMLS : public MlsSurface<_Scalar>
{
		typedef MlsSurface<_Scalar> Base;

		using Base::mCachedQueryPointIsOK;
		using Base::mCachedQueryPoint;
		using Base::mNeighborhood;
		using Base::mCachedWeights;
		using Base::mCachedWeightGradients;
		using Base::mBallTree;
		using Base::mPoints;
		using Base::mRadii;
		using Base::mNormals;
		using Base::mFilterScale;
		using Base::mMaxNofProjectionIterations;
		using Base::mAveragePointSpacing;
		using Base::mProjectionAccuracy;

	public:

		typedef _Scalar Scalar;
		typedef vcg::Point3<Scalar> VectorType;

    template<typename MeshType>
    RIMLS(const MeshType& mesh)
      : Base(mesh)
    {
      mSigmaR = 0;
      mSigmaN = 0.8;
      mRefittingThreshold = 1e-3;
      mMinRefittingIters = 1;
      mMaxRefittingIters = 3;
    }

		virtual Scalar potential(const VectorType& x) const;
		virtual VectorType gradient(const VectorType& x) const;
		virtual VectorType project(const VectorType& x, VectorType* pNormal = 0) const;

//     void setFilterScale(Scalar v);
		void setSigmaR(Scalar v);
		void setSigmaN(Scalar v);
		void setRefittingThreshold(Scalar v);
		void setMinRefittingIters(int n);
		void setMaxRefittingIters(int n);
//     void setMaxProjectionIters(int n);
//     void setProjectionAccuracy(Scalar v);

	protected:
//     void computeVertexRaddi();
		bool computePotentialAndGradient(const VectorType& x) const;

	protected:
//     const MeshModel& mMesh;
//     std::vector<VectorType> mPoints;
//     std::vector<VectorType> mNormals;
//     std::vector<Scalar> mRadii;
//
//     BallTree<Scalar>* mBallTree;

		int mMinRefittingIters;
		int mMaxRefittingIters;
//     int mMaxNofProjectionIterations;
		Scalar mRefittingThreshold;
//     Scalar mFilterScale;
		Scalar mSigmaN;
		Scalar mSigmaR;
//     Scalar mAveragePointSpacing;
//     Scalar mProjectionAccuracy;

		// cached values:
//     mutable bool mCachedQueryPointIsOK;
//     mutable VectorType mCachedQueryPoint;
		mutable VectorType mCachedGradient;
		mutable Scalar mCachedPotential;
//     mutable Neighborhood<Scalar> mNeighborhood;
//     mutable std::vector<Scalar>  mCachedWeights;
//     // mutable std::vector<Scalar>  mCachedWeightDerivatives;
//     mutable std::vector<VectorType>  mCachedWeightGradients;
};

}

#endif
