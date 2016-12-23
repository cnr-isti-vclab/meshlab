/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/. 
*/


#ifndef _GRENAILLE_WEIGHT_FUNC_
#define _GRENAILLE_WEIGHT_FUNC_

namespace Grenaille
{
/*!
    \brief Weighting function based on the euclidean distance between a query and a reference position

    The query is assumed to be expressed in centered coordinates (ie. relatively
    to the evaluation position).

    This class inherits BaseWeightFunc. It can be specialized for any DataPoint, 
    and uses a generic 1D BaseWeightKernel.

    \inherit Concept::WeightFuncConcept

    \warning it assumes that the evaluation scale t is strictly positive
*/
template <class DataPoint, class WeightKernel>
class DistWeightFunc
{
public:
    /*! \brief Scalar type from DataPoint */
    typedef typename DataPoint::Scalar Scalar;
    /*! \brief Vector type from DataPoint */
    typedef typename DataPoint::VectorType VectorType;

    /*! 
        \brief Constructor that defines the current evaluation scale
        \warning t > 0
    */
    MULTIARCH inline DistWeightFunc(const Scalar& _t = Scalar(1.))
    {
        //\todo manage that assrt on __host__ and __device__
        //assert(_t > Scalar(0));
        m_t = _t;
    }

    /*!
        \brief Compute the weight of the given query with respect to its coordinates.

        As the query \f$\mathbf{q}\f$ is expressed in a centered basis, the 
        WeightKernel is directly applied to the norm of its coordinates with 
        respect to the current scale  \f$ t \f$ :

        \f$ w(\frac{\left|\mathbf{q}_\mathsf{x}\right|}{t}) \f$ 
    */
    MULTIARCH inline Scalar w(const VectorType& _q, 
        const DataPoint&  /*attributes*/) const;


    /*!
        \brief First order derivative in space (for each spatial dimension \f$\mathsf{x})\f$

        \f$ \frac{\delta \frac{\left|\mathbf{q}_\mathsf{x}\right|}{t}}{\delta \mathsf{x}} 
        \nabla w(\frac{\left|\mathbf{q}_\mathsf{x}\right|}{t}) 
        = \frac{ \nabla{w(\frac{\left|\mathbf{q}_\mathsf{x}\right|}{t})}}{t}  \f$

        where \f$ \left|\mathbf{q}_\mathsf{x}\right| \f$ represents the norm of the
        query coordinates expressed in centered basis,
        for each spatial dimensions \f$ \mathsf{x}\f$.
    */
    MULTIARCH inline VectorType spacedw(const VectorType& _q, 
        const DataPoint&  /*attributes*/) const;


    /*!
        \brief First order derivative in scale  \f$t\f$

        \f$ \frac{\delta \frac{\left|\mathbf{q}\right|}{t}}{\delta t} 
        \nabla w(\frac{\left|\mathbf{q}\right|}{t}) 
        = - \frac{\left|\mathbf{q}\right|}{t^2} \nabla{w(\frac{\left|\mathbf{q}\right|}{t})} \f$

        where \f$ \left|\mathbf{q}\right| \f$ represents the norm of the
        query coordinates expressed in centered basis.
    */
    MULTIARCH inline Scalar scaledw(const VectorType& _q, 
        const DataPoint&  /*attributes*/) const;

    /*! \brief Access to the evaluation scale set during the initialization */
    MULTIARCH inline Scalar evalScale() const { return m_t; }

protected:
    Scalar       m_t;  /*!< \brief Evaluation scale */
    WeightKernel m_wk; /*!< \brief 1D function applied to weight queries */    

};// class DistWeightFunc

#include "weightFunc.hpp"

}// namespace Grenaille


#endif // _GRENAILLE_WEIGHT_FUNC_
