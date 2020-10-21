/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _VITELOTTE_FV_ELEMENT_
#define _VITELOTTE_FV_ELEMENT_


#include <Eigen/Core>

#include "../../common/defines.h"

#include "linearElement.h"


namespace Vitelotte
{


/**
 * \brief Provides method to evaluate FV elements and their derivatives.
 */
template < typename _Scalar >
class FVElement : protected LinearElement<_Scalar>
{
public:
    typedef _Scalar Scalar;
    typedef LinearElement<Scalar> Base;

    typedef Eigen::Matrix<Scalar, 2, 1> Vector;
    typedef Eigen::Matrix<Scalar, 9, 1> Values;
    typedef Eigen::Matrix<Scalar, 9, 2> Jacobian;
    typedef Eigen::Matrix<Scalar, 2, 2> Hessian;

    typedef Eigen::Matrix<Scalar, 3, 1> BarycentricCoord;

protected:
    typedef Eigen::Matrix<Scalar, 3, 1> Vector3;
    typedef Eigen::Matrix<Scalar, 3, 3> Matrix3;
    typedef Eigen::Matrix<Scalar, 2, 3> Matrix2x3;

public:
    template < typename It >
    MULTIARCH inline FVElement(It pointIt)
        : Base(pointIt)
    {
        computeFromPoints();
    }

    template < typename Derived0, typename Derived1, typename Derived2 >
    MULTIARCH inline FVElement(
            const Eigen::MatrixBase<Derived0>& p0,
            const Eigen::MatrixBase<Derived1>& p1,
            const Eigen::MatrixBase<Derived2>& p2)
        : Base(p0, p1, p2)
    {
        computeFromPoints();
    }

    using Base::projPoint;
    using Base::edgeLength;
    using Base::doubleArea;

    MULTIARCH inline const Matrix3& dldn() const
    {
        return m_dldn;
    }

    MULTIARCH inline Scalar dldn(unsigned li, unsigned ni) const
    {
        assert(li < 3 && ni < 3);
        return m_dldn(li, ni);
    }

    using Base::bcProj;

    MULTIARCH inline Values eval(const BarycentricCoord& bc) const
    {
        Values eb;
        Scalar bubble = _bubble(bc);

        for(int i = 0; i < 3; ++i)
        {
            Scalar gse = _gradientSubExpr(i, bc);
            eb(i + 3) = 4 * _edgeSubExpr(i, bc)
                      + 4 * gse
                      - 12 * bubble;
            eb(i + 6) = _gradientFactor(i) * gse;
        }

        for(int i = 0; i < 3; ++i)
        {
            unsigned i1 = (i+1) % 3;
            unsigned i2 = (i+2) % 3;
            eb(i) = _vertexSubExpr(i, bc)
                  + 3 * bubble
                  + _vertexGradientFactor(i, i1) * eb(i1 + 6)
                  + _vertexGradientFactor(i, i2) * eb(i2 + 6);
        }

        return eb;
    }

    MULTIARCH inline Scalar eval(unsigned bi, const BarycentricCoord& bc) const
    {
        assert(bi < 9);

        Scalar v;
        unsigned i = bi % 3;
        switch(bi/3)
        {
        case 0:
        {
            unsigned i1 = (i+1) % 3;
            unsigned i2 = (i+2) % 3;
            v = _vertexSubExpr(i, bc)
              + 3 * _bubble(bc)
              + _vertexGradientFactor(i, i1) * _gradientFactor(i1) * _gradientSubExpr(i1, bc)
              + _vertexGradientFactor(i, i2) * _gradientFactor(i2) * _gradientSubExpr(i2, bc);
            break;
        }
        case 1:
        {
            v =  4 * _edgeSubExpr(i, bc)
              +  4 * _gradientSubExpr(i, bc)
              - 12 * _bubble(bc);
            break;
        }
        case 2:
        {
            v = _gradientFactor(i) * _gradientSubExpr(i, bc);
            break;
        }
        }

        return v;
    }

    MULTIARCH inline Jacobian jacobian(const BarycentricCoord& bc) const
    {
        Jacobian grad;
        Vector bubbleGradient = _bubbleGradient(bc);

        for(int i = 0; i < 3; ++i)
        {
            Vector gseGradient = _gradientSubExprGradient(i, bc);
            grad.row(i + 3) = 4 * _edgeSubExprGradient(i, bc)
                            + 4 * gseGradient
                            - 12 * bubbleGradient;
            grad.row(i + 6) = _gradientFactor(i) * gseGradient;
        }

        for(int i = 0; i < 3; ++i)
        {
            unsigned i1 = (i+1) % 3;
            unsigned i2 = (i+2) % 3;
            Vector v1 = grad.row(i1 + 6);
            Vector v2 = grad.row(i2 + 6);
            grad.row(i) = _vertexSubExprGradient(i, bc)
                        + 3 * bubbleGradient
                        + _vertexGradientFactor(i, i1) * v1
                        + _vertexGradientFactor(i, i2) * v2;
        }

        return grad;
    }

    MULTIARCH inline Vector gradient(unsigned bi, const BarycentricCoord& bc) const
    {
        assert(bi < 9);

        Vector grad;
        unsigned i = bi % 3;
        switch(bi/3)
        {
        case 0:
        {
            unsigned i1 = (i+1) % 3;
            unsigned i2 = (i+2) % 3;
            grad = _vertexSubExprGradient(i, bc)
                 + 3 * _bubbleGradient(bc)
                 + _vertexGradientFactor(i, i1) * _gradientFactor(i1) * _gradientSubExprGradient(i1, bc)
                 + _vertexGradientFactor(i, i2) * _gradientFactor(i2) * _gradientSubExprGradient(i2, bc);
            break;
        }
        case 1:
        {
            grad =  4 * _edgeSubExprGradient(i, bc)
                 +  4 * _gradientSubExprGradient(i, bc)
                 - 12 * _bubbleGradient(bc);
            break;
        }
        case 2:
        {
            grad = _gradientFactor(i) * _gradientSubExprGradient(i, bc);
            break;
        }
        }

        return grad;
    }

    MULTIARCH inline void hessian(const BarycentricCoord& bc, Hessian* h) const
    {
        Hessian bubbleHessian = _bubbleHessian(bc);

        for(int i = 0; i < 3; ++i)
        {
            Hessian gseHessian = _gradientSubExprHessian(i, bc);
            h[i + 3] = 4 * _edgeSubExprHessian(i, bc)
                     + 4 * gseHessian
                     - 12 * bubbleHessian;
            h[i + 6] = _gradientFactor(i) * gseHessian;
        }

        for(int i = 0; i < 3; ++i)
        {
            unsigned i1 = (i+1) % 3;
            unsigned i2 = (i+2) % 3;
            Hessian& v1 = h[i1 + 6];
            Hessian& v2 = h[i2 + 6];
            h[i] = _vertexSubExprHessian(i, bc)
                 + 3 * bubbleHessian
                 + _vertexGradientFactor(i, i1) * v1
                 + _vertexGradientFactor(i, i2) * v2;
        }
    }

    MULTIARCH inline Hessian hessian(unsigned bi,
                                     const BarycentricCoord& bc) const
    {
        assert(bi < 9);

        Hessian h;
        unsigned i = bi % 3;
        switch(bi/3)
        {
        case 0:
        {
            unsigned i1 = (i+1) % 3;
            unsigned i2 = (i+2) % 3;
            h = _vertexSubExprHessian(i, bc)
              + 3 * _bubbleHessian(bc)
              + _vertexGradientFactor(i, i1) * _gradientFactor(i1) * _gradientSubExprHessian(i1, bc)
              + _vertexGradientFactor(i, i2) * _gradientFactor(i2) * _gradientSubExprHessian(i2, bc);
            break;
        }
        case 1:
        {
            h =  4 * _edgeSubExprHessian(i, bc)
              +  4 * _gradientSubExprHessian(i, bc)
              - 12 * _bubbleHessian(bc);
            break;
        }
        case 2:
        {
            h = _gradientFactor(i) * _gradientSubExprHessian(i, bc);
            break;
        }
        }

        return h;
    }


public:
    MULTIARCH inline Scalar _gradientFactor(unsigned bi) const
    {
        return - m_2delta / edgeLength(bi);
    }

    MULTIARCH inline Scalar _vertexGradientFactor(unsigned bi,
                                                  unsigned ei) const
    {
        return dldn(bi, ei) + dldn(ei, ei) / 2;
    }

    MULTIARCH inline Scalar _bubble(const BarycentricCoord& bc) const
    {
        return bc.prod();
    }

    MULTIARCH inline Vector _bubbleGradient(const BarycentricCoord& bc) const
    {
        return Base::jacobian().row(0) * bc(1) * bc(2)
             + Base::jacobian().row(1) * bc(2) * bc(0)
             + Base::jacobian().row(2) * bc(0) * bc(1);
    }

    MULTIARCH inline Hessian _bubbleHessian(const BarycentricCoord& bc) const
    {
        Hessian h;
        for(int d0 = 0; d0 < 2; ++d0)
        {
            for(int d1 = d0; d1 < 2; ++d1)
            {
                h(d0, d1) = Base::jacobian()(0, d0) * Base::jacobian()(1, d1) * bc(2)
                          + Base::jacobian()(0, d0) * Base::jacobian()(2, d1) * bc(1)
                          + Base::jacobian()(1, d0) * Base::jacobian()(0, d1) * bc(2)
                          + Base::jacobian()(1, d0) * Base::jacobian()(2, d1) * bc(0)
                          + Base::jacobian()(2, d0) * Base::jacobian()(0, d1) * bc(1)
                          + Base::jacobian()(2, d0) * Base::jacobian()(1, d1) * bc(0);
            }
        }
        h(1, 0) = h(0, 1);
        return h;
    }

    MULTIARCH inline Scalar _vertexSubExpr(unsigned i,
                                           const BarycentricCoord& bc) const
    {
        return bc(i) * (bc(i) - .5) * (bc(i) + 1);
    }

    MULTIARCH inline Vector _vertexSubExprGradient(
            unsigned i,const BarycentricCoord& bc) const
    {
        return Base::jacobian().row(i) * (3 * bc[i] * bc[i] + bc[i] - .5);
    }

    MULTIARCH inline Hessian _vertexSubExprHessian(
            unsigned i, const BarycentricCoord& bc) const
    {
        Hessian h;
        for(int d0 = 0; d0 < 2; ++d0)
        {
            for(int d1 = d0; d1 < 2; ++d1)
            {
                h(d0, d1) = Base::jacobian()(i, d0)
                          * Base::jacobian()(i, d1)
                          * (6 * bc(i) + 1);
            }
        }
        h(1, 0) = h(0, 1);
        return h;
    }

    MULTIARCH inline Scalar _edgeSubExpr(unsigned i,
                                         const BarycentricCoord& bc) const
    {
        return bc((i+1)%3) * bc((i+2)%3);
    }

    MULTIARCH inline Vector _edgeSubExprGradient(
            unsigned i, const BarycentricCoord& bc) const
    {
        unsigned i1 = (i+1) % 3;
        unsigned i2 = (i+2) % 3;
        return Base::jacobian().row(i1) * bc(i2)
             + Base::jacobian().row(i2) * bc(i1);
    }

    MULTIARCH inline Hessian _edgeSubExprHessian(
            unsigned i, const BarycentricCoord& /*bc*/) const
    {
        unsigned i1 = (i+1) % 3;
        unsigned i2 = (i+2) % 3;
        Hessian h;
        for(int d0 = 0; d0 < 2; ++d0)
        {
            for(int d1 = d0; d1 < 2; ++d1)
            {
                h(d0, d1) = Base::jacobian()(i1, d0) * Base::jacobian()(i2, d1)
                          + Base::jacobian()(i2, d0) * Base::jacobian()(i1, d1);
            }
        }
        h(1, 0) = h(0, 1);
        return h;
    }

    MULTIARCH inline Scalar _gradientSubExpr(unsigned i,
                                             const BarycentricCoord& bc) const
    {
        return bc(i) * (2.*bc(i) - 1.) * (bc(i) - 1.);
    }

    MULTIARCH inline Vector _gradientSubExprGradient(
            unsigned i, const BarycentricCoord& bc) const
    {
        return Base::jacobian().row(i) * (6 * bc(i) * bc(i) - 6 * bc(i) + 1);
    }

    MULTIARCH inline Hessian _gradientSubExprHessian(
            unsigned i, const BarycentricCoord& bc) const
    {
        Hessian h;
        for(int d0 = 0; d0 < 2; ++d0)
        {
            for(int d1 = d0; d1 < 2; ++d1)
            {
                h(d0, d1) = Base::jacobian()(i, d0)
                          * Base::jacobian()(i, d1)
                          * (12 * bc(i) - 6);
            }
        }
        h(1, 0) = h(0, 1);
        return h;
    }

protected:
    MULTIARCH inline void computeFromPoints()
    {
        Matrix2x3 vs;
        for(int i = 0; i < 3; ++i)
            vs.col(i) = projPoint(i, 2) - projPoint(i, 1);

        for(int ni = 0; ni < 3; ++ni)
            for(int li = 0; li < 3; ++li)
                m_dldn(li, ni) =
                        vs.col(li).dot(vs.col(ni)) / (m_2delta * edgeLength(ni));
    }


protected:
    using Base::m_2delta;

    Matrix3 m_dldn;
};


} // namespace Vitelotte


#endif
