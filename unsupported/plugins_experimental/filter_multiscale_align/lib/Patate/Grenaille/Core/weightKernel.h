/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/. 
*/


#ifndef _GRENAILLE_WEIGHT_KERNEL_
#define _GRENAILLE_WEIGHT_KERNEL_

/*!
    \file weightKernel.h Define 1D weight kernel functors
*/


namespace Grenaille
{
/*!
    \brief Concept::WeightKernelConcept returning a constant value

    \inherit Concept::WeightKernelConcept    
*/
template <typename _Scalar>
class ConstantWeightKernel
{
public:
    /*! \brief Scalar type defined outside the class */
    typedef _Scalar Scalar;

    // Init 
    //! \brief Default constructor that could be used to set the returned value
    MULTIARCH inline ConstantWeightKernel(const Scalar& _value = Scalar(1.)) : m_y(_value){}
    //! \brief Set the returned value
    MULTIARCH inline void setValue(const Scalar& _value){ m_y = _value; } 

    // Functor
    //! \brief Return the constant value
    MULTIARCH inline Scalar f  (const Scalar&) const { return m_y; }
    //! \brief Return \f$ 0 \f$
    MULTIARCH inline Scalar df (const Scalar&) const { return Scalar(0.); }
    //! \brief Return \f$ 0 \f$
    MULTIARCH inline Scalar ddf(const Scalar&) const { return Scalar(0.); }

private: 
    Scalar m_y; /*!< \brief Constant value returned by the kernel */
};// class ConstantWeightKernel


/*!
    \brief Smooth WeightKernel defined in \f$\left[0 : 1\right]\f$
    \todo Add a degree value as template parameter (in this class or another one), with specialized functions for 2

    \inherit Concept::WeightKernelConcept  
*/
template <typename _Scalar>
class SmoothWeightKernel
{
public:
    /*! \brief Scalar type defined outside the class*/
    typedef _Scalar Scalar;

    // Functor
    /*! \brief Defines the smooth weighting function \f$ w(x) = (x^2-1)^2 \f$ */
    MULTIARCH inline Scalar f  (const Scalar& _x) const { Scalar v = _x*_x - Scalar(1.); return v*v; }
    /*! \brief Defines the smooth first order weighting function \f$ \nabla w(x) = 4x(x^2-1) \f$ */
    MULTIARCH inline Scalar df (const Scalar& _x) const { return Scalar(4.)*_x*(_x*_x-Scalar(1.)); }
    /*! \brief Defines the smooth second order weighting function \f$ \nabla^2 w(x) = 12x^2-4 \f$ */
    MULTIARCH inline Scalar ddf(const Scalar& _x) const { return Scalar(12.)*_x*_x - Scalar(4.); }
};//class SmoothWeightKernel

}// namespace Grenaille


#endif //_GRENAILLE_WEIGHT_KERNEL_
