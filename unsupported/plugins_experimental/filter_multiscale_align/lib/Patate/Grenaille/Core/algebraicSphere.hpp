/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/


/*!
Use gradient descent
*/
template < class DataPoint, class _WFunctor, typename T>
typename DataPoint::VectorType
AlgebraicSphere<DataPoint, _WFunctor, T>::project( const VectorType& _q ) const
{
    MULTIARCH_STD_MATH(min)

    // turn to centered basis
    const VectorType lq = _q-m_p;

    //if(_isPlane)
    //{
    VectorType grad;
    VectorType dir  = m_ul+Scalar(2.)*m_uq*lq;
    Scalar ilg      = Scalar(1.)/dir.norm();
    dir             = dir*ilg;
    Scalar ad       = m_uc + m_ul.dot(lq) + m_uq * lq.squaredNorm();
    Scalar delta    = -ad*min(ilg,Scalar(1.));
    VectorType proj = lq + dir*delta;

    for (int i=0; i<16; ++i)
    {
        grad  = m_ul+Scalar(2.)*m_uq*proj;
        ilg   = Scalar(1.)/grad.norm();
        delta = -(m_uc + proj.dot(m_ul) + m_uq * proj.squaredNorm())*min(ilg,Scalar(1.));
        proj += dir*delta;
    }
    return proj + m_p;
    //}
    //return other - _ul * dot(other,_ul) + _uc;
    //return normalize(other-_center) * _r + _center;
}

template < class DataPoint, class _WFunctor, typename T>
typename DataPoint::Scalar
AlgebraicSphere<DataPoint, _WFunctor, T>::potential( const VectorType &_q ) const
{  
    // turn to centered basis
    const VectorType lq = _q-m_p;

    return m_uc + lq.dot(m_ul) + m_uq * lq.squaredNorm();
}


template < class DataPoint, class _WFunctor, typename T>
typename DataPoint::VectorType
AlgebraicSphere<DataPoint, _WFunctor, T>::primitiveGradient( const VectorType &_q ) const
{
        // turn to centered basis
        const VectorType lq = _q-m_p;  
        return (m_ul + Scalar(2.f) * m_uq * lq);
}

