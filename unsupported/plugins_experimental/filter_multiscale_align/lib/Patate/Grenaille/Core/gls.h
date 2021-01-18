/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/. 
*/

#include <Eigen/Eigenvalues>

#ifndef _GRENAILLE_GLS_
#define _GRENAILLE_GLS_

namespace Grenaille
{

/*!
    \brief Growing Least Squares reparemetrization of the OrientedSphereFit
    \inherit Concept::FittingExtensionConcept

    Method published in \cite Mellado:2012:GLS

    This class assumes that the WeightFunc defines the accessor
    \code
        w.evalScale();
    \endcode
    in order to access to the evaluation scale, needed to compute the 
    scale invariant GLS reparametrization (all *_normalized methods).

    Computed values:
    - tau(), eta() and kappa(): the GLS descriptor 
    \f$ \left[ \tau \; \eta \; \kappa \right]\f$ 
    - tau_normalized(), eta_normalized() and kappa_normalized(): 
    the scale invariant GLS descriptor
    \f$ \left[ \frac{\tau}{t} \; \eta \; t\kappa \right]\f$ 

    Requierement: 
    \verbatim PROVIDES_ALGEBRAIC_SPHERE \endverbatim
    Provide: 
    \verbatim PROVIDES_GLS_PARAMETRIZATION \endverbatim
*/
template < class DataPoint, class _WFunctor, typename T>
class GLSParam : public T
{
private:
    typedef T Base;

protected:
    enum
    {
        Check = Base::PROVIDES_ALGEBRAIC_SPHERE,
        PROVIDES_GLS_PARAMETRIZATION
    };

public:
    typedef typename Base::Scalar     Scalar;     /*!< \brief Inherited scalar type*/
    typedef typename Base::VectorType VectorType; /*!< \brief Inherited vector type*/
    typedef typename Base::WFunctor   WFunctor;   /*!< \brief Weight Function*/


protected:
    Scalar m_t;         /*!< \brief Evaluation scale. Needed to computed the normalized values*/
    Scalar m_fitness;   /*!< \brief Save the fitness value to avoid side effect with Pratt normalization*/

public:
    /*! \brief Default constructor */
    MULTIARCH inline GLSParam() : m_t(0) {}

    /**************************************************************************/
    /* Initialization                                                         */
    /**************************************************************************/
    /*! \copydoc Concept::FittingProcedureConcept::setWeightFunc() */
    MULTIARCH inline void setWeightFunc(const WFunctor& _w)
    {
        Base::setWeightFunc(_w);
        m_t = _w.evalScale();
    }

    /**************************************************************************/
    /* Processing                                                             */
    /**************************************************************************/
    /*! \copydoc Concept::FittingProcedureConcept::finalize() */
    MULTIARCH inline FIT_RESULT finalize()
    {
        FIT_RESULT bResult = Base::finalize();

        if(bResult != UNDEFINED)
        {
            m_fitness = Scalar(1.) - Base::prattNorm2();
        }

        return bResult;
    }

    /**************************************************************************/
    /* Use results                                                            */
    /**************************************************************************/
    /*! \brief Compute and return \f$ \tau \f$ */
    MULTIARCH inline Scalar tau() const 
    {
        return Base::isNormalized() ? Base::m_uc : Base::m_uc / Base::prattNorm();
    }

    /*! \brief Compute and return \f$ \eta \f$ */
    MULTIARCH inline VectorType eta() const { return Base::normal(); }

    /*! \brief Compute and return \f$ \kappa \f$ */
    MULTIARCH inline Scalar kappa() const 
    {
        return Scalar(2.) * (Base::isNormalized() ? Base::m_uq : Base::m_uq / Base::prattNorm());
    }

    /*! \brief Compute and return \f$ \frac{\tau}{t} \f$ */
    MULTIARCH inline Scalar tau_normalized() const { return tau() / m_t; }

    /*! \brief Compute and return \f$ \eta \f$ */    
    MULTIARCH inline VectorType eta_normalized() const { return eta(); }

    /*! \brief Compute and return \f$ t \kappa \f$ */
    MULTIARCH inline Scalar kappa_normalized() const { return kappa() * m_t; }    

    /*! \brief Return the fitness, e.g. the pratt norm of the initial scalar field */
    MULTIARCH inline Scalar fitness() const { return m_fitness; }

    /*! 
    \brief Compare current instance with other.
    \return a distance between two fits (0 correspond to two similar fits)
    \warning Use the same scale to have a useful comparison (normalized value are used)
    */
    MULTIARCH inline Scalar compareTo (const GLSParam<DataPoint, _WFunctor, T>& _other,
                                        bool _useFitness = true) const
    { 
        Scalar nTau     = this->tau_normalized()   - _other.tau_normalized();
        Scalar nKappa   = this->kappa_normalized() - _other.kappa_normalized();
        Scalar nFitness = _useFitness ? this->fitness() - _other.fitness() : Scalar(0.);

        return nTau * nTau + nKappa * nKappa + nFitness * nFitness;
    }

}; //class GLSParam


/*!
    \brief Differentiation of GLSParam
    \inherit Concept::FittingExtensionConcept

    Method published in \cite Mellado:2012:GLS
*/
template < class DataPoint, class _WFunctor, typename T>
class GLSDer : public T
{
private:
    typedef T Base;

protected:
    enum
    {
        Check = Base::PROVIDES_GLS_PARAMETRIZATION,
        PROVIDES_GLS_DERIVATIVE
    };

public:
    typedef typename Base::Scalar     Scalar;      /*!< \brief Inherited scalar type */
    typedef typename Base::VectorType VectorType;  /*!< \brief Inherited vector type */
    typedef typename Base::WFunctor   WFunctor;    /*!< \brief Weight Function */

    typedef typename Base::VectorArray VectorArray; /*!< \brief Inherited vector array type */
    typedef typename Base::ScalarArray ScalarArray; /*!< \brief Inherited scalar array type */ 

    MULTIARCH inline ScalarArray dtau()   const; /*!< \brief Compute and return \f$ \tau \f$ derivatives */
    MULTIARCH inline VectorArray deta()   const; /*!< \brief Compute and return \f$ \eta \f$ derivatives */
    MULTIARCH inline ScalarArray dkappa() const; /*!< \brief Compute and return \f$ \kappa \f$ derivatives */

    MULTIARCH inline ScalarArray dtau_normalized()   const; /*!< \brief Compute and return \f$ \tau \f$ derivatives */
    MULTIARCH inline VectorArray deta_normalized()   const; /*!< \brief Compute and return \f$ t * d\eta \f$ */ 
    MULTIARCH inline ScalarArray dkappa_normalized() const; /*!< \brief Compute and return \f$ d\kappa * t^{2} \f$ */
}; //class GLSScaleDer


/*!
    \brief Extension to compute the Geometric Variation of GLSParam
    \inherit Concept::FittingExtensionConcept
    
    The Geometric Variation is computed as the weighted sum of the
    GLS scale-invariant partial derivatives
    \f[
     \nu(\mathbf{p},t) = 
     w_\tau   \left(\frac{\delta\tau}{\delta t}\right)^2 +
     w_\eta   \left( t   \frac{\delta\eta}{\delta t}\right)^2 +
     w_\kappa \left( t^2 \frac{\delta\kappa}{\delta t}\right)^2
    \f]

    Method published in \cite Mellado:2012:GLS
    \todo Add more details
*/
template < class DataPoint, class _WFunctor, typename T>
class GLSGeomVar : public T
{
private:
    typedef T Base;

protected:
    enum
    {
        Check = Base::PROVIDES_ALGEBRAIC_SPHERE_SCALE_DERIVATIVE & Base::PROVIDES_GLS_DERIVATIVE,
        PROVIDES_GLS_GEOM_VAR
    };

public:
    typedef typename Base::Scalar Scalar;  /*!< \brief Inherited scalar type*/

    /*!
    \brief Compute and return the Geometric Variation
    */
    MULTIARCH inline Scalar geomVar(Scalar wtau   = Scalar(1), 
                                    Scalar weta   = Scalar(1),
                                    Scalar wkappa = Scalar(1)) const;
};


/*! \deprecated See class CurvatureEstimator */
template < class DataPoint, class _WFunctor, typename T>
class GLSCurvatureHelper : public CurvatureEstimator<DataPoint,_WFunctor,T>
{
    typedef CurvatureEstimator<DataPoint,_WFunctor,T> Base;
public:
  
    typedef typename Base::Scalar Scalar;
    typedef typename Base::VectorType VectorType;
    
    /*! \deprecated */
    MULTIARCH inline Scalar GLSk1() const { return Base::k1(); }

    /*! \deprecated */
    MULTIARCH inline Scalar GLSk2() const { return Base::k2(); }

    /*! \deprecated */
    MULTIARCH inline VectorType GLSk1Direction() const { return Base::k1Direction(); }
    
    /*! \deprecated */
    MULTIARCH inline VectorType GLSk2Direction() const { return Base::k2Direction(); }

    /*! \deprecated */
    MULTIARCH inline Scalar GLSGaussianCurvature() const { return Base::GaussianCurvature(); }    

};

#include "gls.hpp"

} //namespace Grenaille

#endif
