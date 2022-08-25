/*
 Copyright (C) 2014 Nicolas Mellado <nmellado0@gmail.com>
 Copyright (C) 2015 Gael Guennebaud <gael.guennebaud@inria.fr>
 
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/. 
*/


template < class DataPoint, class _WFunctor, typename T>
void 
CovariancePlaneFit<DataPoint, _WFunctor, T>::init(const VectorType& _evalPos)
{
    // Setup primitive
    Base::resetPrimitive();
    // Base::basisCenter() = _evalPos;

    // Setup fitting internal values
    m_sumW        = Scalar(0.0);
    m_evalPos     = _evalPos;
    m_cog         = VectorType::Zero();
    m_cov         = MatrixType::Zero();
}

template < class DataPoint, class _WFunctor, typename T>
bool 
CovariancePlaneFit<DataPoint, _WFunctor, T>::addNeighbor(const DataPoint& _nei)
{
    VectorType q = _nei.pos() - m_evalPos;
    // compute weight
    Scalar w = m_w.w(q, _nei);

    if (w > Scalar(0.))
    {
      m_cog  += w * q;
      m_sumW += w;
      m_cov  += w * q * q.transpose();
        
      ++(Base::m_nbNeighbors);
      return true;
    }
    return false;
}


template < class DataPoint, class _WFunctor, typename T>
FIT_RESULT
CovariancePlaneFit<DataPoint, _WFunctor, T>::finalize ()
{
    Scalar epsilon = Eigen::NumTraits<Scalar>::dummy_precision();

    // handle specific configurations
    // With less than 3 neighbors the fitting is undefined
    if(m_sumW == Scalar(0.) || Base::m_nbNeighbors < 3)
    {
      Base::resetPrimitive();
      Base::m_eCurrentState = UNDEFINED;
      return Base::m_eCurrentState;
    }

    // Center the covariance on the centroid:
    m_cov -= m_cog * m_cog.transpose() / m_sumW;
    // Finalize the centroid
    m_cog = m_cog/m_sumW + m_evalPos;
            
#ifdef __CUDACC__
    m_solver.computeDirect(m_cov);
#else
    m_solver.compute(m_cov);
#endif
        
    Base::setPlane(m_solver.eigenvectors().col(0), m_cog);

    // \todo Use the output of the solver to check stability
    Base::m_eCurrentState = STABLE;
    return Base::m_eCurrentState;
}


template < class DataPoint, class _WFunctor, typename T>
typename CovariancePlaneFit<DataPoint, _WFunctor, T>::Scalar
CovariancePlaneFit<DataPoint, _WFunctor, T>::surfaceVariation () const
{
    if( Base::m_eCurrentState == UNDEFINED )
      return 0;

    return m_solver.eigenvalues()(0) / m_solver.eigenvalues().mean();
}


namespace internal
{

template < class DataPoint, class _WFunctor, typename T, int Type>
void 
CovariancePlaneDer<DataPoint, _WFunctor, T, Type>::init(const VectorType& _evalPos)
{
    Base::init(_evalPos);

    m_dCog   = VectorArray::Zero();
    m_dSumW  = ScalarArray::Zero();
    for(int k=0; k<NbDerivatives; ++k)
      m_dCov[k].setZero();
}


template < class DataPoint, class _WFunctor, typename T, int Type>
bool 
CovariancePlaneDer<DataPoint, _WFunctor, T, Type>::addNeighbor(const DataPoint  &_nei)
{
    bool bResult = Base::addNeighbor(_nei);

    if(bResult)
    {
        int spaceId = (Type & FitScaleDer) ? 1 : 0;

        ScalarArray dw;

        // centered basis
        VectorType q = _nei.pos()-Base::m_evalPos;

        // compute weight
        if (Type & FitScaleDer)
            dw[0] = Base::m_w.scaledw(q, _nei);

        if (Type & FitSpaceDer)
            dw.template segment<int(DataPoint::Dim)>(spaceId) = -Base::m_w.spacedw(q, _nei).transpose();

        m_dSumW += dw;
        m_dCog  += q * dw;
        for(int k=0; k<NbDerivatives; ++k)
          m_dCov[k]  += dw[k] * q * q.transpose();

        return true;
    }

    return false;
}


template < class DataPoint, class _WFunctor, typename T, int Type>
FIT_RESULT 
CovariancePlaneDer<DataPoint, _WFunctor, T, Type>::finalize()
{
    MULTIARCH_STD_MATH(sqrt);

    Base::finalize();
    // Test if base finalize end on a viable case (stable / unstable)
    if (this->isReady())
    {
      // pre-compute shifted eigenvalues to apply the pseudo inverse of C - lambda_0 I
      Scalar epsilon          = Scalar(2) * Eigen::NumTraits<Scalar>::epsilon();
      Scalar consider_as_zero = Scalar(2) * std::numeric_limits<Scalar>::denorm_min();
      Eigen::Matrix<Scalar,2,1> shifted_eivals = Base::m_solver.eigenvalues().template tail<2>().array() - Base::m_solver.eigenvalues()(0);
      if(shifted_eivals(0) < consider_as_zero || shifted_eivals(0) < epsilon * shifted_eivals(1)) shifted_eivals(0) = 0;
      if(shifted_eivals(1) < consider_as_zero) shifted_eivals(1) = 0;
      
      for(int k=0; k<NbDerivatives; ++k)
      {
        // Finalize the computation of dCov.
        // Note that at this stage m_dCog = sum_i dw_i * (p_i-c).
        // Since the covariance matrix is translation invariant,
        // this step natuarally cancels the centered basis.
        VectorType shifted_cog = Base::m_cog - Base::m_evalPos;
        m_dCov[k] = m_dCov[k]
                  - shifted_cog * m_dCog.col(k).transpose()
                  - m_dCog.col(k) * shifted_cog.transpose()
                  + m_dSumW[k] * shifted_cog * shifted_cog.transpose();
                  
        // cancel centered basis of dCog:
        m_dCog.col(k) += m_dSumW[k] * Base::m_evalPos;
        
        // apply normalization by sumW:
        m_dCog.col(k) = (m_dCog.col(k) - m_dSumW(k) * Base::m_cog) / Base::m_sumW;

        VectorType normal = Base::normal();
        // The derivative of 'normal' is the derivative of the smallest eigenvector.
        // Since the covariance matrix is real and symmetric, it is equal to:
        //    n' = - (C - lambda_0 I)^+ C' n
        // Where ^+ denotes the pseudo-inverse.
        // Since we already performed the eigenvalue decomposition of the matrix C,
        // we can directly apply the pseudo inverse by observing that:
        //    (C - lambda_0 I) = V (L - lambda_0 I) V^T
        // where V is the eigenvector matrix, and L the eigenvalue diagonal matrix.
        Eigen::Matrix<Scalar,2,1> z = - Base::m_solver.eigenvectors().template rightCols<2>().transpose() * (m_dCov[k] * normal);
        if(shifted_eivals(0)>0) z(0) /= shifted_eivals(0);
        if(shifted_eivals(1)>0) z(1) /= shifted_eivals(1);
        m_dNormal.col(k) = Base::m_solver.eigenvectors().template rightCols<2>() * z;
        
        VectorType dDiff = -m_dCog.col(k);
        if(k>0 || !isScaleDer())
          dDiff(isScaleDer() ? k-1 : k) += 1;
        m_dDist(k) = m_dNormal.col(k).dot(Base::m_evalPos-Base::m_cog) + normal.dot(dDiff);
      }
    }

    return Base::m_eCurrentState;
}

}// namespace internal
