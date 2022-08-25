/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _VITELOTTE_LINEAR_ELEMENT_
#define _VITELOTTE_LINEAR_ELEMENT_


#include <Eigen/Core>

#include "../../common/defines.h"


namespace Vitelotte
{


/**
 * \brief Provides method to evaluate linear elements and their derivatives.
 */
template < typename _Scalar >
class LinearElement
{
public:
    typedef _Scalar Scalar;

    typedef Eigen::Matrix<Scalar, 2, 1> Vector;
    typedef Eigen::Matrix<Scalar, 3, 1> Values;
    typedef Eigen::Matrix<Scalar, 3, 2> Jacobian;
    typedef Eigen::Matrix<Scalar, 2, 2> Hessian;

    typedef Eigen::Matrix<Scalar, 3, 1> BarycentricCoord;

protected:
    typedef Eigen::Matrix<Scalar, 3, 1> Vector3;

    typedef Eigen::Matrix<Scalar, 2, 2> Matrix2;
    typedef Eigen::Matrix<Scalar, 3, 3> Matrix3;

    typedef Eigen::Matrix<Scalar, 3, 2> Matrix3x2;
    typedef Eigen::Matrix<Scalar, 2, 3> Matrix2x3;

public:
    template < typename It >
    MULTIARCH inline LinearElement(It pointIt)
    {
        It it0 = pointIt;
        It it1 = it0; ++it1;
        It it2 = it1; ++it2;
        computeFromPoints(*it0, *it1, *it2);
    }

    template < typename Derived0, typename Derived1, typename Derived2 >
    MULTIARCH inline LinearElement(
            const Eigen::MatrixBase<Derived0>& p0,
            const Eigen::MatrixBase<Derived1>& p1,
            const Eigen::MatrixBase<Derived2>& p2)
    {
        computeFromPoints(p0, p1, p2);
    }

    MULTIARCH inline Vector projPoint(unsigned pi, unsigned offset=0) const
    {
        assert(pi < 3 && offset < 3);
        switch((pi + offset) % 3)
        {
        case 0: return Vector::Zero();
        case 1: return Vector(edgeLength(2), 0);
        case 2: return m_p2;
        }
        return Vector();
    }

    MULTIARCH inline Scalar edgeLength(unsigned ei, unsigned offset=0) const
    {
        assert(ei < 3 && offset < 3);
        return m_eLen((ei + offset) % 3);
    }

    MULTIARCH inline Scalar doubleArea() const { return m_2delta; }

    MULTIARCH inline BarycentricCoord bcProj(const Vector& p) const
    {
        return m_lbf * (BarycentricCoord() << p, 1).finished();
    }

    MULTIARCH inline Values eval(const BarycentricCoord& bc) const
    {
        return bc;
    }

    MULTIARCH inline Scalar eval(unsigned bi, const BarycentricCoord& bc) const
    {
        assert(bi < 3);
        return bc(bi);
    }

    MULTIARCH inline const Eigen::Block<const Matrix3, 3, 2> jacobian(
            const BarycentricCoord& /*bc*/ = BarycentricCoord()) const
    {
        return m_lbf.template block<3, 2>(0, 0);
    }

    MULTIARCH inline const Vector gradient(
            unsigned bi,
            const BarycentricCoord& /*bc*/ = BarycentricCoord()) const
    {
        assert(bi < 3);
        return m_lbf.template block<1, 2>(bi, 0);
    }

protected:
    template < typename Derived0, typename Derived1, typename Derived2 >
    MULTIARCH inline void computeFromPoints(
            const Eigen::MatrixBase<Derived0>& p0,
            const Eigen::MatrixBase<Derived1>& p1,
            const Eigen::MatrixBase<Derived2>& p2)
    {
        m_eLen(0) = (p2-p1).norm();
        m_eLen(1) = (p0-p2).norm();
        m_eLen(2) = (p1-p0).norm();

        m_p2(0) = (p2-p0).dot(p1-p0) / edgeLength(2);
        m_p2(1) = std::sqrt(edgeLength(1)*edgeLength(1) - m_p2(0)*m_p2(0));

        m_2delta = edgeLength(2) * m_p2(1);

        for(int li = 0; li < 3; ++li)
        {
            m_lbf(li, 0) = projPoint(li, 1).y() - projPoint(li, 2).y();
            m_lbf(li, 1) = projPoint(li, 2).x() - projPoint(li, 1).x();
            m_lbf(li, 2) = projPoint(li, 1).x() * projPoint(li, 2).y()
                         - projPoint(li, 2).x() * projPoint(li, 1).y();
        }
        m_lbf /= m_2delta;
    }

protected:
//    Matrix2x3 m_points;
    Vector m_p2;
    Vector3 m_eLen;

    Scalar m_2delta;
    Matrix3 m_lbf;
};


} // namespace Vitelotte


#endif
