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
#include "mlsutils.h"
#include <iostream>

namespace GaelMls {

template<typename _MeshType>
void APSS<_MeshType>::setSphericalParameter(Scalar v)
{
	mSphericalParameter = v;
	mCachedQueryPointIsOK = false;
}

template<typename _MeshType>
typename APSS<_MeshType>::Scalar APSS<_MeshType>::potential(const VectorType& x, int* errorMask) const
{
	if ((!mCachedQueryPointIsOK) || mCachedQueryPoint!=x)
	{
		if (!fit(x))
		{
			if (errorMask)
				*errorMask = MLS_TOO_FAR;
			return Base::InvalidValue();
		}
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

template<typename _MeshType>
typename APSS<_MeshType>::Scalar APSS<_MeshType>::approxMeanCurvature(const VectorType& x, int* errorMask) const
{
	if ((!mCachedQueryPointIsOK) || mCachedQueryPoint!=x)
	{
		if (!fit(x))
		{
			if (errorMask)
				*errorMask = MLS_TOO_FAR;
			return Base::InvalidValue();
		}
	}

	if (mStatus==ASS_SPHERE)
		return (uQuad>0.?1.0:-1.0)/mRadius;
	else
		return 0.;
}

template<typename _MeshType>
typename APSS<_MeshType>::VectorType APSS<_MeshType>::gradient(const VectorType& x, int* errorMask) const
{
	if ((!mCachedQueryPointIsOK) || mCachedQueryPoint!=x)
	{
		if (!fit(x))
		{
			if (errorMask)
				*errorMask = MLS_TOO_FAR;
			return VectorType(0,0,0);
		}
	}

	if (mGradientHint==MLS_DERIVATIVE_ACCURATE)
	{
		VectorType grad;
		mlsGradient(x,grad);
		return grad;
	}
	else
	{
		LVector lx(x.X(), x.Y(), x.Z());
		if (mStatus==ASS_PLANE)
			return VectorType(uLinear.X(), uLinear.Y(), uLinear.Z());
		else
		{
			LVector g = uLinear + lx * (Scalar(2) * uQuad);
			return VectorType(g.X(), g.Y(), g.Z());
		}
	}
}

template<typename _MeshType>
typename APSS<_MeshType>::MatrixType APSS<_MeshType>::hessian(const VectorType& x, int* errorMask) const
{
	if ((!mCachedQueryPointIsOK) || mCachedQueryPoint!=x)
	{
		if (!fit(x))
		{
			if (errorMask)
				*errorMask = MLS_TOO_FAR;
			return MatrixType();
		}
	}

	MatrixType hessian;
	if (Base::mHessianHint==MLS_DERIVATIVE_ACCURATE)
	{
		mlsHessian(x, hessian);
	}
	else
	{
		// this is very approximate !!
		Scalar c = Scalar(2) * uQuad;
		for (int i=0; i<3; ++i)
		for (int j=0; j<3; ++j)
		{
			if (i==j)
				hessian[i][j] = c;
			else
				hessian[i][j] = 0;
		}
	}
	return hessian;
}

template<typename _MeshType>
typename APSS<_MeshType>::VectorType APSS<_MeshType>::project(const VectorType& x, VectorType* pNormal, int* errorMask) const
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
			if (errorMask)
				*errorMask = MLS_TOO_FAR;
			//std::cout << " proj failed\n";
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
		if (mGradientHint==MLS_DERIVATIVE_ACCURATE)
		{
			VectorType grad;
			mlsGradient(vcg::Point3Cast<Scalar>(position), grad);
			grad.Normalize();
			*pNormal = grad;
		}
		else
		{
			for (int k=0; k<3; ++k)
				(*pNormal)[k] = normal[k];
		}
	}

	if (iterationCount>=mMaxNofProjectionIterations && errorMask)
			*errorMask = MLS_TOO_MANY_ITERS;

	return VectorType(position.X(), position.Y(), position.Z());
}

template<typename _MeshType>
bool APSS<_MeshType>::fit(const VectorType& x) const
{
	Base::computeNeighborhood(x, true);
	unsigned int nofSamples = mNeighborhood.size();

	if (nofSamples==0)
	{
		mCachedQueryPointIsOK = false;
		return false;
	}
	else if (nofSamples==1)
	{
		int id = mNeighborhood.index(0);
		LVector p = vcg::Point3Cast<LScalar>(mPoints[id].cP());
		LVector n = vcg::Point3Cast<LScalar>(mPoints[id].cN());

		uLinear = n;
		uConstant = -vcg::Dot(p, uLinear);
		uQuad = 0;
		mStatus = ASS_PLANE;
		return true;
	}

	LVector sumP; sumP.Zero();
	LVector sumN; sumN.Zero();
	LScalar sumDotPN = 0.;
	LScalar sumDotPP = 0.;
	LScalar sumW = 0.;
	for (unsigned int i=0; i<nofSamples; i++)
	{
		int id = mNeighborhood.index(i);
		LVector p = vcg::Point3Cast<LScalar>(mPoints[id].cP());
		LVector n = vcg::Point3Cast<LScalar>(mPoints[id].cN());
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

	// cache some values to be used by the mls gradient
	mCachedSumP = sumP;
	mCachedSumN = sumN;
	mCachedSumW = sumW;
	mCachedSumDotPP = sumDotPP;
	mCachedSumDotPN = sumDotPN;

	mCachedQueryPoint = x;
	mCachedQueryPointIsOK = true;
	return true;
}

template<typename _MeshType>
bool APSS<_MeshType>::mlsGradient(const VectorType& x, VectorType& grad) const
{
	unsigned int nofSamples = mNeighborhood.size();

	const LVector& sumP = mCachedSumP;
	const LVector& sumN = mCachedSumN;
	const LScalar& sumDotPN = mCachedSumDotPN;
	const LScalar& sumDotPP = mCachedSumDotPP;
	const LScalar& sumW = mCachedSumW;
	const LScalar invSumW = 1.f/sumW;

	for (uint k=0 ; k<3 ; ++k)
	{
		LVector dSumP; dSumP.Zero();
		LVector dSumN; dSumN.Zero();
		LScalar dSumDotPN = 0.;
		LScalar dSumDotPP = 0.;
		LScalar dSumW = 0.;
		for (unsigned int i=0; i<nofSamples; i++)
		{
			int id = mNeighborhood.index(i);
			LVector p = vcg::Point3Cast<LScalar>(mPoints[id].cP());
			LVector n = vcg::Point3Cast<LScalar>(mPoints[id].cN());
			LScalar dw = mCachedWeightGradients.at(i)[k];

			dSumW += dw;
			dSumP += p*dw;
			dSumN += n*dw;
			dSumDotPN += dw * vcg::Dot(n,p);
			dSumDotPP += dw * vcg::SquaredNorm(p);
		}

		mCachedGradSumP[k] = dSumP;
		mCachedGradSumN[k] = dSumN;
		mCachedGradSumDotPN[k] = dSumDotPN;
		mCachedGradSumDotPP[k] = dSumDotPP;
		mCachedGradSumW[k] = dSumW;

		LScalar dVecU0;
		LVector dVecU13;
		LScalar dVecU4;

		LScalar nume = sumDotPN - invSumW * vcg::Dot(sumP, sumN);
		LScalar deno = sumDotPP - invSumW * vcg::Dot(sumP, sumP);
		LScalar dNume = dSumDotPN - invSumW*invSumW*( sumW*(vcg::Dot(dSumP,sumN) + vcg::Dot(sumP,dSumN)) - dSumW*vcg::Dot(sumP,sumN));
		LScalar dDeno = dSumDotPP - invSumW*invSumW*( 2.*sumW*vcg::Dot(dSumP,sumP)                       - dSumW*vcg::Dot(sumP,sumP));

		dVecU4 = mSphericalParameter * 0.5 * (deno * dNume - dDeno * nume)/(deno*deno);
		dVecU13 = ((dSumN - (dSumP*uQuad + sumP*dVecU4)*2.0) - uLinear * dSumW) * invSumW;
		dVecU0 = -invSumW*( vcg::Dot(dVecU13,sumP) + dVecU4*sumDotPP + vcg::Dot(uLinear,dSumP) + uQuad*dSumDotPP + dSumW*uConstant);

		grad[k] = dVecU0 + vcg::Dot(dVecU13,vcg::Point3Cast<LScalar>(x)) + dVecU4*vcg::SquaredNorm(x) + uLinear[k] + 2.*x[k]*uQuad;

		mCachedGradUConstant[k] = dVecU0;
		mCachedGradULinear[k] = dVecU13;
		mCachedGradUQuad[k] = dVecU4;
	}

	return true;
}

template<typename _MeshType>
bool APSS<_MeshType>::mlsHessian(const VectorType& x, MatrixType& hessian) const
{
	this->requestSecondDerivatives();

	// TODO call mlsGradient first
	VectorType grad;
	mlsGradient(x,grad);
	
	uint nofSamples = mNeighborhood.size();
	
	const LVector& sumP = mCachedSumP;
	const LVector& sumN = mCachedSumN;
	const LScalar& sumDotPN = mCachedSumDotPN;
	const LScalar& sumDotPP = mCachedSumDotPP;
	const LScalar& sumW = mCachedSumW;
	const LScalar invSumW = 1.f/sumW;

	for (uint k=0 ; k<3 ; ++k)
	{
		const LVector& dSumP = mCachedGradSumP[k];
		const LVector& dSumN = mCachedGradSumN[k];
		const LScalar& dSumDotPN = mCachedGradSumDotPN[k];
		const LScalar& dSumDotPP = mCachedGradSumDotPP[k];
		const LScalar& dSumW = mCachedGradSumW[k];
		
		LScalar dVecU0 = mCachedGradUConstant[k];
		LVector dVecU13 = mCachedGradULinear[k];
		LScalar dVecU4 = mCachedGradUQuad[k];

		LScalar nume = sumDotPN - invSumW * vcg::Dot(sumP, sumN);
		LScalar deno = sumDotPP - invSumW * vcg::Dot(sumP, sumP);
		LScalar dNume = dSumDotPN - invSumW*invSumW*( sumW*(vcg::Dot(dSumP,sumN) + vcg::Dot(sumP,dSumN)) - dSumW*vcg::Dot(sumP,sumN));
		LScalar dDeno = dSumDotPP - invSumW*invSumW*( 2.*sumW*vcg::Dot(dSumP,sumP)                       - dSumW*vcg::Dot(sumP,sumP));

// 		dVecU4 = mSphericalParameter * 0.5 * (deno * dNume - dDeno * nume)/(deno*deno);
// 		dVecU13 = ((dSumN - (dSumP*uQuad + sumP*dVecU4)*2.0) - uLinear * dSumW) * invSumW;
// 		dVecU0 = -invSumW*( vcg::Dot(dVecU13,sumP) + dVecU4*sumDotPP + vcg::Dot(uLinear,dSumP) + uQuad*dSumDotPP + dSumW*uConstant);
		
		// second order derivatives
		for (uint j=0 ; j<3 ; ++j)
		{
			LVector d2SumP; d2SumP.Zero();
			LVector d2SumN; d2SumN.Zero();
			LScalar d2SumDotPN = 0.;
			LScalar d2SumDotPP = 0.;
			LScalar d2SumW = 0.;
			for (unsigned int i=0; i<nofSamples; i++)
			{
				int id = mNeighborhood.index(i);
				LVector p = vcg::Point3Cast<LScalar>(mPoints[id].cP());
				LVector n = vcg::Point3Cast<LScalar>(mPoints[id].cN());
				LScalar dw = mCachedWeightGradients.at(i)[j];
				LScalar d2w = ((x[k]-p[k]))*((x[j]-p[j])) * mCachedWeightSecondDerivatives.at(i);

				if (j==k)
					d2w += mCachedWeightDerivatives.at(i);

				d2SumW += d2w;
				d2SumP += p*d2w;
				d2SumN += n*d2w;
				d2SumDotPN += d2w * vcg::Dot(n,p);
				d2SumDotPP += d2w * vcg::Dot(p,p);
			}

			LScalar d2u0;
			LVector d2u13;
			LScalar d2u4;

			LScalar d2Nume = d2SumDotPN - invSumW*invSumW*invSumW*invSumW*(
					- 2.*sumW*dSumW*( sumW*(vcg::Dot(dSumP,sumN)+vcg::Dot(sumP,dSumN)) - dSumW* vcg::Dot(sumP,sumN))
					+    sumW*sumW*( dSumW*(vcg::Dot(dSumP,sumN)+vcg::Dot(sumP,dSumN))
													+ sumW*(vcg::Dot(d2SumP,sumN)	+ vcg::Dot(sumP,d2SumN) + 2.*vcg::Dot(dSumP,dSumN))
													- d2SumW*vcg::Dot(sumP,sumN)
													- dSumW*(vcg::Dot(dSumP,sumN)+vcg::Dot(sumP,dSumN)) ));
			// simplify to:
			/*LocalFloat d2Nume = d2SumDotPN - invSumW*invSumW*invSumW*invSumW*(
							2.*sumW*dSumW * ( sumW*(dSumP.dot(sumN)+sumP.dot(dSumN)) - dSumW*sumP.dot(sumN))
					+   sumW*(d2SumP.dot(sumN) + sumP.dot(d2SumN) + 2.*dSumP.dot(dSumN))
					-   d2SumW*sumP.dot(sumN));*/

			LScalar d2Deno = d2SumDotPP - invSumW*invSumW*invSumW*invSumW*(
					-   2.*sumW*dSumW * ( 2.*sumW*vcg::Dot(dSumP,sumP) - dSumW*vcg::Dot(sumP,sumP))
					+   sumW*sumW*(   2.*dSumW*(vcg::Dot(dSumP,sumP)) + 2.*sumW*(vcg::Dot(dSumP,dSumP)+vcg::Dot(d2SumP,sumP))
													- d2SumW*vcg::Dot(sumP,sumP) - dSumW*(2.*vcg::Dot(dSumP,sumP))) );
			// could also be simplified ^^

			LScalar deno2 = deno*deno;
			d2u4 = mSphericalParameter * 0.5 * (deno2*(deno*d2Nume - d2Deno*nume) - 2.*deno*dDeno*(deno * dNume - dDeno * nume))/(deno2*deno2);

			d2u13 = ( -dVecU13*dSumW + ( d2SumN - (dSumP*(2.0*dVecU4) + d2SumP*uQuad + sumP*d2u4)*2.0 ) - uLinear*d2SumW - dVecU13*dSumW)*invSumW;

			d2u0 = - invSumW*( -dSumW*dVecU4 + ( vcg::Dot(dVecU13,dSumP) + vcg::Dot(d2u13,sumP)
												+ d2u4*sumDotPP + dVecU4*dSumDotPP + vcg::Dot(uLinear,d2SumP) + vcg::Dot(dVecU13,dSumP)
					   + dVecU4*dSumDotPP + uQuad*d2SumDotPP + d2SumW*uConstant + dSumW*dVecU0) );

			hessian[j][k] =
							dVecU13[j] + 2.*dVecU4*x[j]
						+ d2u0 + vcg::Dot(d2u13,vcg::Point3Cast<LScalar>(x)) + d2u4*vcg::Dot(x,x)
						+ mCachedGradULinear[j][k] + (j==k ? 2.*uQuad : 0.) + 2.*x[k]*dVecU4;

		}
	}
	
	return true;
}

}
