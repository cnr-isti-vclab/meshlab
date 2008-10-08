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

#include "apss.h"
#include "kdtree.h"
#include "vcg_addons.h"
#include <iostream>

template<typename _Scalar>
APSS<_Scalar>::APSS(const MeshModel& m)
  : Base(m)
{
  mSphericalParameter = 1;
  mAccurateGradient = false;
}

template<typename Scalar>
void APSS<Scalar>::setSphericalParameter(Scalar v)
{
  mSphericalParameter = v;
  mCachedQueryPointIsOK = false;
}

template<typename Scalar>
void APSS<Scalar>::setAccurateGradient(bool on)
{
  mAccurateGradient = on;
  mCachedQueryPointIsOK = false;
}

template<typename Scalar>
Scalar APSS<Scalar>::potential(const VectorType& x) const
{
//   if ((!mCachedQueryPointIsOK) || mCachedQueryPoint!=x)
  {
    if (!fit(x))
			return -1e9;
  }

	LVector lx(x.X(), x.Y(), x.Z());

	if (mStatus==ASS_SPHERE)
	{
		Scalar aux = vcg::Norm(lx - mCenter) - mRadius;
		if (uQuad<0.)
			aux = -aux;
		return aux;
	}
	else if (mStatus==ASS_PLANE)
		return vcg::Dot(lx,uLinear) + uConstant;
	else
	{
		return uConstant + vcg::Dot(lx,uLinear) + uQuad * vcg::SquaredNorm(lx);
	}
}

template<typename Scalar>
typename APSS<Scalar>::VectorType APSS<Scalar>::gradient(const VectorType& x) const
{
  if ((!mCachedQueryPointIsOK) || mCachedQueryPoint!=x)
  {
    fit(x);
  }

	LVector lx(x.X(), x.Y(), x.Z());
  if (mStatus==ASS_PLANE)
		return VectorType(uLinear.X(), uLinear.Y(), uLinear.Z());
	else
	{
		LVector g = uLinear + lx * (Scalar(2) * uQuad);
		return VectorType(g.X(), g.Y(), g.Z());
	}
}

template<typename Scalar>
typename APSS<Scalar>::VectorType APSS<Scalar>::project(const VectorType& x, VectorType* pNormal) const
{
  int iterationCount = 0;
	LVector lx(x.X(), x.Y(), x.Z());
  LVector position = lx;
  LVector normal;
	LVector previousPosition;
  LScalar delta2;
  LScalar epsilon2 = mAveragePointSpacing * mProjectionAccuracy;
	epsilon2 = epsilon2 * epsilon2;
  do {
      if (!fit(VectorType(position.X(), position.Y(), position.Z())))
      {
        std::cout << " proj failed\n";
        return x;
      }

			previousPosition = position;
			// local projection
			if (mStatus==ASS_SPHERE)
			{
				normal = lx - mCenter;
				normal.Normalize();
				position = mCenter + normal * mRadius;

				normal = uLinear + position * (LScalar(2) * uQuad);
				normal.Normalize();
			}
			else if (mStatus==ASS_PLANE)
			{
				normal = uLinear;
				position = lx - uLinear * (vcg::Dot(lx,uLinear) + uConstant);
			}
			else
			{
				// Newton iterations
				LVector grad;
				LVector dir = uLinear+lx*(2.*uQuad);
				LScalar ilg = 1./vcg::Norm(dir);
				dir *= ilg;
				LScalar ad = uConstant + vcg::Dot(uLinear,lx) + uQuad * vcg::SquaredNorm(lx);
				LScalar delta = -ad*std::min<Scalar>(ilg,1.);
				LVector p = lx + dir*delta;
				for (int i=0 ; i<2 ; ++i)
				{
					grad = uLinear+p*(2.*uQuad);
					ilg = 1./vcg::Norm(grad);
					delta = -(uConstant + vcg::Dot(uLinear,p) + uQuad * vcg::SquaredNorm(p))*std::min<Scalar>(ilg,1.);
					p += dir*delta;
				}
				position = p;

				normal = uLinear + position * (Scalar(2) * uQuad);
				normal.Normalize();
			}

      delta2 = vcg::SquaredNorm(previousPosition - position);
  } while ( delta2>epsilon2 && ++iterationCount<mMaxNofProjectionIterations);
  if (pNormal)
	{
		for (int k=0; k<3; ++k)
			(*pNormal)[k] = normal[k];
	}

  return VectorType(position.X(), position.Y(), position.Z());
}

template<typename Scalar>
bool APSS<Scalar>::fit(const VectorType& x) const
{
	Base::computeNeighborhood(x, true);
	uint nofSamples = mNeighborhood.size();

	if (nofSamples==0)
	{
		return false;
	}
	else if (nofSamples==1)
	{
		int id = mNeighborhood.index(0);
		uLinear = LVector(mNormals[id].X(), mNormals[id].Y(), mNormals[id].Z());
		uConstant = -vcg::Dot(LVector(mPoints[id].X(), mPoints[id].Y(), mPoints[id].Z()), uLinear);
		uQuad = 0;
		mStatus = ASS_PLANE;
		return true;
	}

	LVector sumP; sumP.Zero();
	LVector sumN; sumN.Zero();
	LScalar sumDotPN = 0.;
	LScalar sumDotPP = 0.;
	LScalar sumW = 0.;
	for (uint i=0; i<nofSamples; i++)
	{
		int id = mNeighborhood.index(i);
		LVector p(mPoints[id].X(), mPoints[id].Y(), mPoints[id].Z());
		LVector n(mNormals[id].X(), mNormals[id].Y(), mNormals[id].Z());
		LScalar w = mCachedWeights.at(i);

		sumP += p * w;
		sumN += n * w;
		sumDotPN += w * vcg::Dot(n,p);
		sumDotPP += w * vcg::SquaredNorm(p);
		sumW += w;
	}

	LScalar invSumW = Scalar(1)/sumW;
	LScalar aux4 = mSphericalParameter * LScalar(0.5) *
								 (sumDotPN - invSumW*vcg::Dot(sumP,sumN))
								/(sumDotPP - invSumW*vcg::SquaredNorm(sumP));
	uLinear = (sumN-sumP*(Scalar(2)*aux4))*invSumW;
	uConstant = -invSumW*(Dot(uLinear,sumP) + sumDotPP*aux4);
	uQuad = aux4;

	// finalize

	if (fabs(uQuad)>1e-7)
	{
		mStatus = ASS_SPHERE;
		LScalar b = 1./uQuad;
		mCenter = uLinear*(-0.5*b);
		mRadius = sqrt( vcg::SquaredNorm(mCenter) - b*uConstant );
	}
	else if (uQuad==0.)
	{
		mStatus = ASS_PLANE;
		LScalar s = LScalar(1)/vcg::Norm(uLinear);
		uLinear *= s;
		uConstant *= s;
	}
	else
	{
		mStatus = ASS_UNDETERMINED;
		// normalize the gradient
		LScalar f = 1./sqrt(vcg::SquaredNorm(uLinear) - Scalar(4)*uConstant*uQuad);
		uConstant *= f;
		uLinear *= f;
		uQuad *= f;
	}

	mCachedQueryPoint = x;
	mCachedQueryPointIsOK = true;
	return true;
}

template class APSS<float>;
// template class APSS<double>;
