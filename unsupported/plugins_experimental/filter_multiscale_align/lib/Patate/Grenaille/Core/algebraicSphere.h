/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/. 
*/


#ifndef _GRENAILLE_ALGEBRAIC_SPHERE_
#define _GRENAILLE_ALGEBRAIC_SPHERE_

#include "primitive.h" // PrimitiveBase

namespace Grenaille
{

/*!
    \brief Algebraic Sphere primitive
    
    Method published in \cite Guennebaud:2007:APSS
    
    An algebraic hyper-sphere is defined as the \f$0\f$-isosurface of the scalar field
    
    \f$ s_\mathbf{u}(\mathbf{x}) = \left[ 1 \; \mathbf{x}^T \; \mathbf{x}^T\mathbf{x}\right]^T \cdot \mathbf{u} \f$    
    
    with \f$ \mathbf{u} \left[ u_c \; \mathbf{u_l} \; u_q\right]^T \f$ is the 
    vector of the constant, linear and quadratic parameters.
    
    \note If internally the scalar fields are stored in a local frame defined
    by the evaluation position, the public methods involving a query (such as
    project, potential, gradient) have to be defined in global 
    coordinates (e.g. you don't need to convert your query in the current locale
    frame).
    
    This primitive provides: 
    \verbatim PROVIDES_ALGEBRAIC_SPHERE \endverbatim

    \todo Deal with planar case (_uq == 0) and what about _ul == 0 ?
*/
template < class DataPoint, class _WFunctor, typename T = void  >
class AlgebraicSphere : public PrimitiveBase<DataPoint, _WFunctor>
{
private:

    typedef PrimitiveBase<DataPoint, _WFunctor> Base; 
    
protected:

    enum
    {
        PROVIDES_ALGEBRAIC_SPHERE /*!< \brief Provides Algebraic Sphere */
    };

public:

    /*! \brief Scalar type inherited from DataPoint */
    typedef typename DataPoint::Scalar     Scalar;     
    /*! \brief Vector type inherited from DataPoint */
    typedef typename DataPoint::VectorType VectorType;
    /*! \brief Weight Function */
    typedef _WFunctor                      WFunctor;  
    
private:

    //! \brief Evaluation position (needed for centered basis)
    VectorType m_p; 
    
protected:

    //! \brief Is the implicit scalar field normalized using Pratt
    bool m_isNormalized;

// results
public:

    Scalar m_uc,       /*!< \brief Constant parameter of the Algebraic hyper-sphere */
            m_uq;       /*!< \brief Quadratic parameter of the Algebraic hyper-sphere */
    VectorType m_ul;   /*!< \brief Linear parameter of the Algebraic hyper-sphere */
    
public:

    /*! \brief Default constructor */
    MULTIARCH inline AlgebraicSphere()
        : Base()
    {
        m_p = VectorType::Zero();
        resetPrimitive();
    }    
    
    /*! \brief Set the scalar field values to 0 and reset the isNormalized() status */
    MULTIARCH inline void resetPrimitive()
    {
        Base::resetPrimitive();
    
        m_uc = Scalar(0.0);
        m_ul = VectorType::Zero();
        m_uq = Scalar(0.0);
      
        m_isNormalized = false;
    }

    /*! \brief Reading access to the basis center (evaluation position) */
    MULTIARCH inline const VectorType& basisCenter () const { return m_p; }
    /*! \brief Writing access to the (evaluation position) */
    MULTIARCH inline       VectorType& basisCenter ()       { return m_p; }

    /*! \brief compute the Pratt norm of the implicit scalar field. */
    MULTIARCH inline Scalar prattNorm() const
    {
        MULTIARCH_STD_MATH(sqrt);
        return sqrt(prattNorm2());
    }
    
    /*! \brief compute the squared Pratt norm of the implicit scalar field. */
    MULTIARCH inline Scalar prattNorm2() const
    {
        return m_ul.squaredNorm() - Scalar(4.) * m_uc * m_uq;
    }

    /*!
        \brief Normalize the scalar field by the Pratt norm
        \return false when the normalization fails (sphere is already normalized)
    */
    MULTIARCH inline bool applyPrattNorm()
    {
        if (! m_isNormalized)
        {
            Scalar pn = prattNorm();
            m_uc /= pn;
            m_ul *= Scalar(1.)/pn;
            m_uq /= pn;

            m_isNormalized = true;
        }
        return true;
    }
    
    /*!
        \brief return the estimated radius of the sphere
        \warning return inf if the fitted surface is planar
    */
    MULTIARCH inline Scalar radius()
    {
        if(isPlane())
        {
            //return infinity (non-sense value)
#ifdef __CUDACC__
            Scalar inf = 0.;
            return Scalar(1.)/inf;
#else
            return std::numeric_limits<Scalar>::infinity();
#endif
        }

        MULTIARCH_STD_MATH(sqrt);
        Scalar b = Scalar(1.)/m_uq;
        return Scalar(sqrt( ((Scalar(-0.5)*b)*m_ul).squaredNorm() - m_uc*b ));
    }
    
    /*!
        \brief return the estimated center of the sphere
        \warning return Vector inf if the fitted surface is planar
    */
    MULTIARCH inline VectorType center()
    {
        if(isPlane())
        {
            //return infinity (non-sense value)
            Scalar inf = 0.;
            return VectorType::Constant(Scalar(1.)/inf);
        }

        Scalar b = Scalar(1.)/m_uq;
        return (Scalar(-0.5)*b)*m_ul + basisCenter();
    }
    
    //! \brief State indicating when the sphere has been normalized 
    MULTIARCH inline bool isNormalized() const { return m_isNormalized; }
    
    //! \brief Value of the scalar field at the location \f$ \mathbf{q} \f$
    MULTIARCH inline Scalar potential (const VectorType& _q) const;
    
    //! \brief Project a point on the sphere
    MULTIARCH inline VectorType project (const VectorType& _q) const;
    
    //! \brief Approximation of the scalar field gradient at \f$ \mathbf{q} (not normalized) \f$
    MULTIARCH inline VectorType primitiveGradient (const VectorType& _q) const;

    /*!
        \brief Used to know if the fitting result to a plane
        \return true if finalize() have been called and the fitting result to a plane
    */
    MULTIARCH inline bool isPlane() const
    {
        MULTIARCH_STD_MATH(abs);
        Scalar epsilon = Eigen::NumTraits<Scalar>::dummy_precision();
        bool bPlanar   = Eigen::internal::isMuchSmallerThan(abs(m_uq), 1., epsilon);
        bool bReady    = Base::isReady();

        return bReady && bPlanar;
    }

}; //class AlgebraicSphere

#include "algebraicSphere.hpp"

}
#endif  // _GRENAILLE_ALGEBRAIC_SPHERE_
