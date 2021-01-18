/*
 Copyright (C) 2014 Nicolas Mellado <nmellado0@gmail.com>
 
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/


#ifndef _GRENAILLE_PRIMITIVE_
#define _GRENAILLE_PRIMITIVE_

namespace Grenaille
{

/*!
    \brief Primitive base class.
    
    This class stores and provides public access to the fitting state, and must
    be inherited by classes implementing new primitives. 
    
    Protected fields #m_eCurrentState and #m_nbNeighbors should be updated
    during the fitting process by the inheriting class.
*/
template < class DataPoint, class _WFunctor, typename T = void  >
class PrimitiveBase
{
    
protected:
    
    //! \brief Represent the current state of the fit (finalize function 
    //! update the state)
    FIT_RESULT m_eCurrentState;

    //! \brief Give the number of neighbors
    int m_nbNeighbors;
    
public:

    /*! \brief Default constructor */
    MULTIARCH inline PrimitiveBase() { }
    
    /*! \brief Reset fitting state 
         status */
    MULTIARCH inline void resetPrimitive()
    {
        m_eCurrentState = UNDEFINED;
        m_nbNeighbors = 0;
    }
    
    /*! \brief Is the primitive well fitted an ready to use (finalize has been
    called)
    \warning The fit can be unstable (having neighbors between 3 and 6) */
    MULTIARCH inline bool isReady() const 
    { 
        return (m_eCurrentState == STABLE) || (m_eCurrentState == UNSTABLE); 
    }

    /*! \brief Is the plane fitted an ready to use (finalize has been called 
    and the result is stable, eq. having more than 6 neighbors) */
    MULTIARCH inline bool isStable() const { return m_eCurrentState == STABLE; }

    /*! \return the current test of the fit */
    MULTIARCH inline FIT_RESULT getCurrentState() const 
    { 
        return m_eCurrentState; 
    }
        
}; //class Plane

}
#endif  // _GRENAILLE_PLANE_
