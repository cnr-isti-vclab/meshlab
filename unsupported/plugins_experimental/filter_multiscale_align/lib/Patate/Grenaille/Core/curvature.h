/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/. 
*/

#include <Eigen/Eigenvalues>

#ifndef _GRENAILLE_CURVATURE_
#define _GRENAILLE_CURVATURE_

namespace Grenaille
{


/*!
    \brief Extension to compute curvature values from the Weingarten map \f$ \frac{d N}{d \mathbf{x}} \f$
    \inherit Concept::FittingExtensionConcept

    This class extracts curvature informations from the spatial derivatives of the normal field \f$ N \f$.
    It first assemble a 2x2 matrix representation of the shape operator, and then performs an eigenvalue decomposition
    using Eigen::SelfAdjointEigenSolver::computeDirect.
    
    The previous basket elements must provide a \c dNormal() method returning a 3x3 matrix.
    If more than one basket element provide a \c dNormal() member, then the last one will be used.

    \warning This class is valid only in 3D.
    \todo Add a compile time check for the working dimension
*/
template < class DataPoint, class _WFunctor, typename T>
class CurvatureEstimator : public T
{
private:
    typedef T Base;

protected:
    enum
    {
        Check = Base::PROVIDES_NORMAL_SPACE_DERIVATIVE,
        PROVIDES_PRINCIPALE_CURVATURES
    };

public:
    typedef typename Base::Scalar          Scalar;      /*!< \brief Inherited scalar type*/
    typedef typename Base::VectorType      VectorType;  /*!< \brief Inherited vector type*/
    typedef typename DataPoint::MatrixType MatrixType;  /*!< \brief Matrix type inherited from DataPoint*/

private:
    Scalar m_k1, m_k2;
    VectorType m_v1, m_v2;

public:
    /*! \brief Default constructor */
    MULTIARCH inline CurvatureEstimator() : m_k1(0), m_k2(0) {}

    /**************************************************************************/
    /* Processing                                                             */
    /**************************************************************************/
    /*! \copydoc Concept::FittingProcedureConcept::finalize() */
    MULTIARCH inline FIT_RESULT finalize();

    /**************************************************************************/
    /* Use results                                                            */
    /**************************************************************************/
    //! \brief Returns an estimate of the first principal curvature value
    MULTIARCH inline Scalar k1() const { return m_k1; }

    //! \brief Returns an estimate of the second principal curvature value
    MULTIARCH inline Scalar k2() const { return m_k2; }

    //! \brief Returns an estimate of the first principal curvature direction
    MULTIARCH inline VectorType k1Direction() const { return m_v1; }

    //! \brief Returns an estimate of the second principal curvature direction
    MULTIARCH inline VectorType k2Direction() const { return m_v2; }
    
    //! \brief Returns an estimate of the mean curvature
    MULTIARCH inline Scalar kMean() const { return (m_k1 + m_k2)/2.;}    

    //! \brief Returns an estimate of the Gaussian curvature
    MULTIARCH inline Scalar GaussianCurvature() const { return m_k1 * m_k2;}    
};

#include "curvature.hpp"

} //namespace Grenaille

#endif
