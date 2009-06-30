/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004                                                \/)\/    *
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

#ifndef __IMPLICITS
#define __IMPLICITS

#include <vcg/space/point2.h>
#include <vcg/space/point3.h>
#include <vcg/math/lin_algebra.h>

namespace vcg
{
namespace implicits
{

/** \returns the Gauss curvature directly from the gradient and hessian */
template<typename Scalar>
Scalar GaussCurvature(const Point3<Scalar>& gradient, const Matrix33<Scalar>& hessian)
{
	Scalar l2 = gradient.SquaredNorm();
	Matrix33<Scalar> adjugate;
	adjugate[0][0] = hessian[1][1]*hessian[2][2] - hessian[1][2]*hessian[2][1];
	adjugate[1][0] = hessian[0][2]*hessian[2][1] - hessian[0][1]*hessian[2][2];
	adjugate[2][0] = hessian[0][1]*hessian[1][2] - hessian[0][2]*hessian[1][1];

	adjugate[0][1] = hessian[1][2]*hessian[2][0] - hessian[1][0]*hessian[2][2];
	adjugate[1][1] = hessian[0][0]*hessian[2][2] - hessian[0][2]*hessian[2][0];
	adjugate[2][1] = hessian[1][0]*hessian[0][2] - hessian[0][0]*hessian[1][2];

	adjugate[0][2] = hessian[1][0]*hessian[2][1] - hessian[1][1]*hessian[2][0];
	adjugate[1][2] = hessian[0][1]*hessian[2][0] - hessian[0][0]*hessian[2][1];
	adjugate[2][2] = hessian[0][0]*hessian[1][1] - hessian[0][1]*hessian[1][0];
	return (vcg::Dot(gradient, adjugate * gradient)) / (l2*l2);
}

/** \returns the mean curvature directly from the gradient and hessian */
template<typename Scalar>
Scalar MeanCurvature(const Point3<Scalar>& gradient, const Matrix33<Scalar>& hessian)
{
	Scalar l = gradient.Norm();
	return (l*l*hessian.Trace() - vcg::Dot(gradient, hessian * gradient)) / (2.*l*l*l);
}

/** This class computes the Weingarten map of a scalar field and provides
	* methods to extract curvatures from it.
	*
	* The Weingarten map is equal to the gradient of the normal vector:
	* \f$ W = \nabla \mathbf(n)
	*       = \nabla \frac{\mathbf{g}}{\Vert \mathbf{g} \Vert}
	*       = \frac{(I - n n^T) H}{\Vert \mathbf{g} \Vert} \f$
	* This matrix can also be seen as the projection of the hessian
	* matrix onto the tangent plane of normal n.
	*/
template<typename Scalar> class WeingartenMap
{
public:
	typedef Point3<Scalar> VectorType;
	typedef Matrix33<Scalar> MatrixType;

	/** Default constructor computing the Weingarten map from the
	  * first and second derivatives, namely the gradient vector
	  * and hessian matrix of the scalar field.
	  */
	WeingartenMap(const VectorType& grad, const MatrixType& hess)
	{
		Scalar invL = 1.0/grad.Norm();
		assert(grad.Norm()>1e-8);
		m_normal = grad * invL;
    assert(!math::IsNAN(invL) && "gradient should not be zero!");
					
		Matrix33<Scalar> I; I.SetIdentity();
		m_nnT.ExternalProduct(m_normal,m_normal);

		m_w = (I-m_nnT) * hess * invL;

		m_kgIsDirty = true;
		m_kmIsDirty = true;
		m_kpAreDirty = true;
		m_kdirAreDirty = true;
	}

	/** \returns the Weingarten map matrix */
	const MatrixType& W() const { return m_w; }

	/** \returns the Gauss curvature = k1 * k2 */
	Scalar GaussCurvature() const
	{
		if (m_kgIsDirty)
		{
			// we add nn^T to W such that the third eigenvalue becomes 1
			// then det(W) = k1 * k2 * 1 = Gauss curvature !
			m_kg = (m_w + m_nnT).Determinant();
			m_kgIsDirty = false;
		}
		return m_kg;
	}

	/** \returns the mean curvature = (k1 + k2)/2 */
	Scalar MeanCurvature() const
	{
		if (m_kmIsDirty)
		{
			// the third eigenvalue of W is 0, then tr(W) = k1 + k2 + 0 = 2 k mean !
			m_km = m_w.Trace();
			m_kmIsDirty = false;
		}
		return m_km;
	}

	/** \returns the first principal curvature */
	Scalar K1() const { updateKp(); return m_k1; }

	/** \returns the second principal curvature */
	Scalar K2() const { updateKp(); return m_k2; }

	/** \returns the direction of the first principal curvature */
	const VectorType& K1Dir() const { extractEigenvectors(); return m_k1dir; }

	/** \returns the direction of the second principal curvature */
	const VectorType& K2Dir() const { extractEigenvectors(); return m_k2dir; }

protected:

	// direct computation of principal curvatures if needed
	inline void updateKp() const
	{
		if (m_kpAreDirty)
		{
			Scalar delta = sqrt(MeanCurvature()*m_km - 4.0*GaussCurvature());
			m_k1 = 0.5*(m_km + delta);
			m_k2 = 0.5*(m_km - delta);
			if (fabs(m_k1)<fabs(m_k2))
				std::swap(m_k1,m_k2);
			m_kpAreDirty = false;
		}
	}

	inline void extractEigenvectors() const
	{
		if (m_kdirAreDirty)
		{
			MatrixType copy = m_w;
			int mrot = 0;
			VectorType evals;
			MatrixType evecs;
			Jacobi(copy, evals, evecs, mrot);
			VectorType evalsAbs(fabs(evals[0]),fabs(evals[0]),fabs(evals[0]));
			SortEigenvaluesAndEigenvectors(evals,evecs,true);
			m_k1 = evals[0];
			m_k2 = evals[1];
			m_k1dir = evecs[0];
			m_k2dir = evecs[1];
			m_kdirAreDirty = false;
		}
	}

protected:
	VectorType m_normal;
	MatrixType m_nnT;
	MatrixType m_w;
	mutable VectorType m_k1dir, m_k2dir;
	mutable Scalar m_kg, m_km, m_k1, m_k2;
	mutable bool m_kgIsDirty, m_kmIsDirty, m_kpAreDirty, m_kdirAreDirty;
};

} // namespace implicits
} // namespace vcg

#endif //__IMPLICITS
