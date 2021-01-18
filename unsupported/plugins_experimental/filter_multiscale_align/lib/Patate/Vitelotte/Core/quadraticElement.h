/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _VITELOTTE_QUADRATIC_ELEMENT_
#define _VITELOTTE_QUADRATIC_ELEMENT_


#include <Eigen/Core>

#include "../../common/defines.h"

#include "linearElement.h"


namespace Vitelotte
{


/**
 * \brief Provides method to evaluate quadratic elements and their derivatives.
 */
template < typename _Scalar >
class QuadraticElement : protected LinearElement<_Scalar>
{
public:
    typedef _Scalar Scalar;
    typedef LinearElement<Scalar> Base;

    typedef Eigen::Matrix<Scalar, 2, 1> Vector;
    typedef Eigen::Matrix<Scalar, 6, 1> Values;
    typedef Eigen::Matrix<Scalar, 6, 2> Jacobian;
    typedef Eigen::Matrix<Scalar, 2, 2> Hessian;

    typedef Eigen::Matrix<Scalar, 3, 1> BarycentricCoord;

public:
    template < typename It >
    MULTIARCH inline QuadraticElement(It pointIt)
        : Base(pointIt)
    {}

    template < typename Derived0, typename Derived1, typename Derived2 >
    MULTIARCH inline QuadraticElement(
            const Eigen::MatrixBase<Derived0>& p0,
            const Eigen::MatrixBase<Derived1>& p1,
            const Eigen::MatrixBase<Derived2>& p2)
        : Base(p0, p1, p2)
    {}

    using Base::projPoint;
    using Base::edgeLength;
    using Base::doubleArea;

    using Base::bcProj;

    MULTIARCH inline Values eval(const BarycentricCoord& bc) const
    {
        Values basis;
        for(unsigned i = 0; i < 6; ++i)
            basis(i) = eval(i, bc);
        return basis;
    }

    MULTIARCH inline Scalar eval(unsigned bi, const BarycentricCoord& bc) const
    {
        assert(bi < 6);
        if(bi < 3)
            return bc(bi) * (2 * bc(bi) - 1);
        return 4 * bc((bi + 1) % 3) * bc((bi + 2) % 3);
    }

    MULTIARCH inline const Jacobian jacobian(const BarycentricCoord& bc) const
    {
        Jacobian j;
        for(unsigned i = 0; i < 6; ++i)
            j.row(i) = gradient(i, bc);
        return j;
    }

    MULTIARCH inline const Vector gradient(unsigned bi,
                                           const BarycentricCoord& bc) const
    {
        assert(bi < 6);
        if(bi < 3)
            return Base::gradient(bi, bc) * (4 * bc(bi) - 1);

        unsigned i1 = (bi + 1) % 3;
        unsigned i2 = (bi + 2) % 3;
        return 4 * (Base::gradient(i1, bc) * bc(i2) + Base::gradient(i2, bc) * bc(i1));
    }


protected:
    using Base::m_2delta;
};


} // namespace Vitelotte


#endif
