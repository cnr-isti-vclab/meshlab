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

#include "rimls.h"
#include "kdtree.h"
#include "mlsutils.h"
#include <iostream>

namespace GaelMls {

// template<typename _Scalar>
// RIMLS<_Scalar>::RIMLS(const MeshModel& m)
//   : Base(m)
// {
//   mSigmaR = 0;
//   mSigmaN = 0.8;
//   mRefittingThreshold = 1e-3;
//   mMinRefittingIters = 1;
//   mMaxRefittingIters = 3;
// }

template<typename _MeshType>
void RIMLS<_MeshType>::setSigmaR(Scalar v)
{
  mSigmaR = v;
  mCachedQueryPointIsOK = false;
}

template<typename _MeshType>
void RIMLS<_MeshType>::setSigmaN(Scalar v)
{
  mSigmaN = v;
  mCachedQueryPointIsOK = false;
}

template<typename _MeshType>
void RIMLS<_MeshType>::setRefittingThreshold(Scalar v)
{
  mRefittingThreshold = v;
  mCachedQueryPointIsOK = false;
}

template<typename _MeshType>
void RIMLS<_MeshType>::setMinRefittingIters(int n)
{
  mMinRefittingIters = n;
  mCachedQueryPointIsOK = false;
}

template<typename _MeshType>
void RIMLS<_MeshType>::setMaxRefittingIters(int n)
{
  mMaxRefittingIters = n;
  mCachedQueryPointIsOK = false;
}

template<typename _MeshType>
typename RIMLS<_MeshType>::Scalar RIMLS<_MeshType>::potential(const VectorType& x, int* errorMask) const
{
  if ((!mCachedQueryPointIsOK) || mCachedQueryPoint!=x)
  {
    if (!computePotentialAndGradient(x))
    {
      if (errorMask)
        *errorMask = MLS_TOO_FAR;
      return Base::InvalidValue();
    }
  }

  return mCachedPotential;
}

template<typename _MeshType>
typename RIMLS<_MeshType>::VectorType RIMLS<_MeshType>::gradient(const VectorType& x, int* errorMask) const
{
  if ((!mCachedQueryPointIsOK) || mCachedQueryPoint!=x)
  {
    if (!computePotentialAndGradient(x))
    {
      if (errorMask)
        *errorMask = MLS_TOO_FAR;
      return VectorType(0,0,0);
    }
  }

  return mCachedGradient;
}

template<typename _MeshType>
typename RIMLS<_MeshType>::VectorType RIMLS<_MeshType>::project(const VectorType& x, VectorType* pNormal, int* errorMask) const
{
  int iterationCount = 0;
  VectorType position = x;
  VectorType normal;
  Scalar delta;
  Scalar epsilon = mAveragePointSpacing * mProjectionAccuracy;
  do {
      if (!computePotentialAndGradient(position))
      {
        if (errorMask)
          *errorMask = MLS_TOO_FAR;
        //std::cerr << " proj failed\n";
        return x;
      }

      normal = mCachedGradient;
      normal.Normalize();
      delta = mCachedPotential;
      position = position - normal*delta;
  } while ( fabs(delta)>epsilon && ++iterationCount<mMaxNofProjectionIterations);

  if (iterationCount>=mMaxNofProjectionIterations && errorMask)
    *errorMask = MLS_TOO_MANY_ITERS;
  
  if (pNormal)
    *pNormal = normal;

  return position;
}

template<typename _MeshType>
bool RIMLS<_MeshType>::computePotentialAndGradient(const VectorType& x) const
{
		Base::computeNeighborhood(x, true);
		unsigned int nofSamples = mNeighborhood.size();

    if (nofSamples<1)
    {
        mCachedGradient.Zero();
        mCachedQueryPoint = x;
        mCachedPotential  = 1e9;
        mCachedQueryPointIsOK = false;
        return false;
    }

    VectorType source     = x;
    VectorType grad; grad.Zero();
    VectorType previousGrad;
    VectorType sumN; sumN.Zero();
    Scalar potential      = 0.;
    Scalar invSigma2      = Scalar(1) / (mSigmaN*mSigmaN);
    Scalar invSigmaR2     = 0;
    if (mSigmaR>0)
      invSigmaR2 = Scalar(1) / (mSigmaR*mSigmaR);

    int iterationCount = 0;
    do
    {
        previousGrad = grad;
        VectorType sumGradWeight; sumGradWeight.Zero();
        VectorType sumGradWeightPotential; sumGradWeightPotential.Zero();
        sumN.Zero();
        potential = 0.;
        Scalar sumW = 0.;

        for (unsigned int i=0; i<nofSamples; i++)
        {
            int id = mNeighborhood.index(i);
            VectorType diff = source - mPoints[id].cP();
            VectorType normal = mPoints[id].cN();
            Scalar f = Dot(diff, normal);

            Scalar refittingWeight = 1;
            if (iterationCount > 0)
            {
                refittingWeight = exp(-vcg::SquaredNorm(normal - previousGrad) * invSigma2);
//                 if (mSigmaR>0)
//                 {
//                     Scalar residual = (ddotn - potentialPrev) / mRadii.at(id);
//                     refittingWeight *= exp(-residual*residual * invSigmaR2);
//                 }
            }
            Scalar w = mCachedWeights.at(i) * refittingWeight;
            VectorType gw = mCachedWeightGradients.at(i) * refittingWeight;

            sumGradWeight += gw;
            sumGradWeightPotential += gw * f;
            sumN += normal * w;
            potential += w * f;
            sumW += w;
        }

        if(sumW==0.)
        {
            return false;
        }

        potential /= sumW;
        grad = (-sumGradWeight*potential + sumGradWeightPotential + sumN) * (1./sumW);

        iterationCount++;

    } while ( (iterationCount < mMinRefittingIters)
         || ( vcg::SquaredNorm(grad - previousGrad) > mRefittingThreshold && iterationCount < mMaxRefittingIters) );

    mCachedGradient   = grad;
    mCachedPotential  = potential;
    mCachedQueryPoint = x;
    mCachedQueryPointIsOK = true;
    return true;
}

// template class RIMLS<float>;
// template class RIMLS<double>;

}

