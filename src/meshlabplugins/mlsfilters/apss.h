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

#ifndef APSS_H
#define APSS_H

#include "mlssurface.h"

namespace GaelMls {

template<typename _MeshType>
class APSS : public MlsSurface<_MeshType>
{
    typedef MlsSurface<_MeshType> Base;

    typedef typename Base::Scalar Scalar;
    typedef typename Base::VectorType VectorType;
    typedef _MeshType MeshType;
    using Base::mCachedQueryPointIsOK;
    using Base::mCachedQueryPoint;
    using Base::mNeighborhood;
    using Base::mCachedWeights;
    using Base::mCachedWeightGradients;
    using Base::mBallTree;
    using Base::mPoints;
    using Base::mFilterScale;
    using Base::mMaxNofProjectionIterations;
    using Base::mAveragePointSpacing;
    using Base::mProjectionAccuracy;

    enum Status {ASS_SPHERE, ASS_PLANE, ASS_UNDETERMINED};

  public:

    APSS(const MeshType& m)
      : Base(m)
    {
      mSphericalParameter = 1;
      mAccurateGradient = false;
    }

    virtual Scalar potential(const VectorType& x, int* errorMask = 0) const;
    virtual VectorType gradient(const VectorType& x, int* errorMask = 0) const;
    virtual VectorType project(const VectorType& x, VectorType* pNormal = 0, int* errorMask = 0) const;

    void setSphericalParameter(Scalar v);
    void setAccurateGradient(bool on);

  protected:
    bool fit(const VectorType& x) const;

  protected:
    bool mAccurateGradient;
    Scalar mSphericalParameter;

    // use double precision anyway
    typedef double LScalar;
    typedef vcg::Point3<LScalar> LVector;

    // cached algebraic sphere coefficients
    mutable LScalar uConstant;
    mutable LVector uLinear;
    mutable LScalar uQuad;

    mutable LVector mCenter;
    mutable LScalar mRadius;
    mutable Status mStatus;
};

}

#include "apss.tpp"

#endif
