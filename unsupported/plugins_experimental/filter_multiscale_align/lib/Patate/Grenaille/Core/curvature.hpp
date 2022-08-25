/*
 Copyright (C) 2015 Gael Guennebaud <gael.guennebaud@inria.fr>
 
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/. 
*/



template < class DataPoint, class _WFunctor, typename T>
FIT_RESULT
CurvatureEstimator<DataPoint, _WFunctor, T>::finalize()
{
    typedef typename VectorType::Index Index;
    typedef Eigen::Matrix<Scalar,3,2> Mat32;
    typedef Eigen::Matrix<Scalar,2,2> Mat22;
    
    MULTIARCH_STD_MATH(sqrt);
    MULTIARCH_STD_MATH(abs);

    FIT_RESULT bResult = Base::finalize();

    if(bResult != UNDEFINED)
    {
        // Get the object space Weingarten map dN
        MatrixType dN = Base::dNormal().template middleCols<DataPoint::Dim>(Base::isScaleDer() ? 1: 0);
        
        // Make sure dN is orthogonal to the normal: (optional, does not seem to improve accuracy)
//         VectorType n = Base::normal().normalized();
//         dN = dN - n * n.transpose() * dN;
        
        // Make sure that dN is symmetric:
        // FIXME check why dN is not already symmetric (i.e., round-off errors or error in derivative formulas?)
        dN = 0.5*(dN + dN.transpose().eval());
        
        // Compute tangent-space basis from dN
        //   1 - pick the column with maximal norm as the first tangent vector,
        Index i0, i1, i2;
        Scalar sqNorm = dN.colwise().squaredNorm().maxCoeff(&i0);
        Mat32 B;
        B.col(0) = dN.col(i0) / sqrt(sqNorm);
        //   2 - orthogonalize the other column vectors, and pick the most reliable one
        i1 = (i0+1)%3;
        i2 = (i0+2)%3;
        VectorType v1 = dN.col(i1) - B.col(0).dot(dN.col(i1)) * B.col(0);
        VectorType v2 = dN.col(i2) - B.col(0).dot(dN.col(i2)) * B.col(0);
        Scalar v1norm2 = v1.squaredNorm();
        Scalar v2norm2 = v2.squaredNorm();
        if(v1norm2 > v2norm2) B.col(1) = v1 / sqrt(v1norm2);
        else                  B.col(1) = v2 / sqrt(v2norm2);
        
        // Compute the 2x2 matrix representing the shape operator by transforming dN to the basis B.
        // Recall that dN is a bilinear form, it thus transforms as follows:
        Mat22 S = B.transpose() * dN * B;
        
        Eigen::SelfAdjointEigenSolver<Mat22> eig2;
        eig2.computeDirect(S);
        
        if (eig2.info() != Eigen::Success){
          return UNDEFINED;
        }
        
        m_k1 = eig2.eigenvalues()(0);
        m_k2 = eig2.eigenvalues()(1);
        
        m_v1 = B * eig2.eigenvectors().col(0);
        m_v2 = B * eig2.eigenvectors().col(1);
        
        if(abs(m_k1)<abs(m_k2))
        {
#ifdef __CUDACC__
          Scalar tmpk = m_k1;
          m_k1 = m_k2;
          m_k2 = tmpk;
          VectorType tmpv = m_v1;
          m_v1 = m_v2;
          m_v2 = tmpv;
#else
          std::swap(m_k1, m_k2);
          std::swap(m_v1, m_v2);
#endif
        }
    }

    return bResult;
}
