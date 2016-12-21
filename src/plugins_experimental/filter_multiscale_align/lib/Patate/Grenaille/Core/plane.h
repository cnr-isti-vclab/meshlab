/*
 Copyright (C) 2014 Nicolas Mellado <nmellado0@gmail.com>
 
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/


#ifndef _GRENAILLE_PLANE_
#define _GRENAILLE_PLANE_

#include "primitive.h" // PrimitiveBase

namespace Grenaille
{

/*!
    \brief Implicit hyperplane defined by an homogeneous vector \f$\mathbf{p}\f$. 
    
    In n-dimensionnal space, the plane is defined as
    the \f$0\f$-isosurface of the scalar field
    
    \f$ s_\mathbf{u}(\mathbf{x}) = 
    \left[ \mathbf{x}^T \; 1 \;\right]^T \cdot \mathbf{p} \f$.
    
    This class uses a compact storage of n+1 scalars in n-dimensionnal space. It 
    can be sensitive to the data scale, leading to potential instabilities 
    due to round errors at large scales. 
    \todo Add standard plane storing 2n scalars (direction and center).    
    
    
    This primitive requires the definition of n-dimensionnal vectors 
    (VectorType) and homogeneous n-dimensionnal vectors (HVectorType) in 
    Concept::PointConcept.
    
    This primitive provides: 
    \verbatim PROVIDES_PLANE \endverbatim
    
    \note The first n-components of the plane must define a normalized vector
    
*/
template < class DataPoint, class _WFunctor, typename T = void  >
class CompactPlane : public PrimitiveBase<DataPoint, _WFunctor>
{
private:

    typedef PrimitiveBase<DataPoint, _WFunctor> Base; 
    
protected:

    enum
    {
        PROVIDES_PLANE /*!< \brief Provides a Plane primitive */
    };

public:

    /*! \brief Scalar type inherited from DataPoint */
    typedef typename DataPoint::Scalar      Scalar;
    /*! \brief Vector type inherited from DataPoint */
    typedef typename DataPoint::VectorType  VectorType;
    /*! \brief Matrix type inherited from DataPoint */
    typedef typename DataPoint::MatrixType  MatrixType;
    /*! \brief Homogeneous vector type inherited from DataPoint */
    typedef typename DataPoint::HVectorType HVectorType;
    /*! \brief Weight Function */
    typedef _WFunctor                       WFunctor;

// results
public:

    HVectorType m_p; /*!< \brief Homogeneous plane representation */
    
public:

    /*! \brief Default constructor */
    MULTIARCH inline CompactPlane()
        : Base()
    {
        resetPrimitive();
    }
    
    /*! \brief Set the scalar field values to 0 and reset the isNormalized() 
         status */
    MULTIARCH inline void resetPrimitive()
    {
        Base::resetPrimitive();
        m_p = HVectorType::Zero();
    }
    
    /* \brief Init the plane from a direction and a position
       \param _dir Orientation of the plane
       \param _pos Position of the plane
    */
    MULTIARCH inline void setPlane (const VectorType& _dir, 
                                    const VectorType& _pos)
    {
        m_p.template head<DataPoint::Dim>() = _dir.normalized();
        m_p.template tail<1>()<< -_pos.dot(m_p.template head<DataPoint::Dim>());
    }
        
    //! \brief Value of the scalar field at the location \f$ \mathbf{q} \f$
    MULTIARCH inline Scalar potential (const VectorType& _q) const
    {
        // Project on the normal vector and add the offset value
        return    m_p.template head<DataPoint::Dim>().dot(_q) + 
               *( m_p.template tail<1>().data() );
    }
    
    //! \brief Project a point on the plane
    MULTIARCH inline VectorType project (const VectorType& _q) const
    {
        // The potential is the distance from the point to the plane
        return _q - potential(_q) * m_p.template head<DataPoint::Dim>();
    }
    
    //! \brief Scalar field gradient direction at \f$ \mathbf{q}\f$
    MULTIARCH inline VectorType primitiveGradient (const VectorType&) const
    {
        // Uniform gradient defined only by the orientation of the plane
        return m_p.template head<DataPoint::Dim>();
    }


}; //class Plane

}
#endif  // _GRENAILLE_PLANE_
