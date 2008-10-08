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
#include "vcg_addons.h"
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

template<typename Scalar>
void RIMLS<Scalar>::setSigmaR(Scalar v)
{
  mSigmaR = v;
  mCachedQueryPointIsOK = false;
}

template<typename Scalar>
void RIMLS<Scalar>::setSigmaN(Scalar v)
{
  mSigmaN = v;
  mCachedQueryPointIsOK = false;
}

template<typename Scalar>
void RIMLS<Scalar>::setRefittingThreshold(Scalar v)
{
  mRefittingThreshold = v;
  mCachedQueryPointIsOK = false;
}

template<typename Scalar>
void RIMLS<Scalar>::setMinRefittingIters(int n)
{
  mMinRefittingIters = n;
  mCachedQueryPointIsOK = false;
}

template<typename Scalar>
void RIMLS<Scalar>::setMaxRefittingIters(int n)
{
  mMaxRefittingIters = n;
  mCachedQueryPointIsOK = false;
}

template<typename Scalar>
Scalar RIMLS<Scalar>::potential(const VectorType& x) const
{
  if ((!mCachedQueryPointIsOK) || mCachedQueryPoint!=x)
  {
    computePotentialAndGradient(x);
  }

  return mCachedPotential;
}

template<typename Scalar>
typename RIMLS<Scalar>::VectorType RIMLS<Scalar>::gradient(const VectorType& x) const
{
  if ((!mCachedQueryPointIsOK) || mCachedQueryPoint!=x)
  {
    computePotentialAndGradient(x);
  }

  return mCachedGradient;
}

template<typename Scalar>
typename RIMLS<Scalar>::VectorType RIMLS<Scalar>::project(const VectorType& x, VectorType* pNormal) const
{
  int iterationCount = 0;
  VectorType position = x;
  VectorType normal;
  Scalar delta;
  Scalar epsilon = mAveragePointSpacing * mProjectionAccuracy;
  do {
      if (!computePotentialAndGradient(position))
      {
        std::cerr << " proj failed\n";
        return x;
      }

      normal = mCachedGradient;
      normal.Normalize();
      delta = mCachedPotential;
      position = position - normal*delta;
  } while ( fabs(delta)>epsilon && ++iterationCount<mMaxNofProjectionIterations);
  if (pNormal)
    *pNormal = normal;

  return position;
}

template<typename Scalar>
bool RIMLS<Scalar>::computePotentialAndGradient(const VectorType& x) const
{
		Base::computeNeighborhood(x, true);
		unsigned int nofSamples = mNeighborhood.size();

    if (nofSamples<1)
    {
        mCachedGradient.Zero();
        mCachedQueryPoint = x;
        mCachedPotential  = 1e9;
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
            VectorType diff = source - mPoints[id];
            VectorType normal = mNormals[id];
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

template class RIMLS<float>;
// template class RIMLS<double>;

}

