/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _VITELOTTE_QUADRATIC_ELEMENT_BUILDER_
#define _VITELOTTE_QUADRATIC_ELEMENT_BUILDER_


#include <Eigen/Core>
#include <Eigen/Sparse>


#include "solverError.h"
#include "elementBuilderBase.h"


namespace Vitelotte
{

/**
 * \brief Quadratic element builder, for harmonic interpolation with quadratic
 * triangles output.
 */
template < class _Mesh, typename _Scalar = typename _Mesh::Scalar >
class QuadraticElementBuilder : public ElementBuilderBase<_Mesh, _Scalar>
{
public:
    typedef _Scalar Scalar;
    typedef _Mesh Mesh;

    typedef ElementBuilderBase<_Mesh, _Scalar> Base;

    typedef typename Base::Vector Vector;
    typedef typename Base::Matrix Matrix;
    typedef typename Base::Triplet Triplet;

    typedef typename Base::Face Face;

protected:
    typedef Eigen::Matrix<Scalar, 6, 6> ElementStiffnessMatrix;

protected:
    static void initializeMatrices();

public:
    inline QuadraticElementBuilder();

    unsigned nCoefficients(const Mesh& mesh, Face element,
                           SolverError* error=0) const;

    template < typename Inserter >
    void addCoefficients(Inserter& inserter, const Mesh& mesh,
                         Face element, SolverError* error=0);

private:
    static bool m_matricesInitialized;
    static ElementStiffnessMatrix m_quadM1;
    static ElementStiffnessMatrix m_quadM2;
    static ElementStiffnessMatrix m_quadM3;
};


} // namespace Vitelotte

#include "quadraticElementBuilder.hpp"


#endif
