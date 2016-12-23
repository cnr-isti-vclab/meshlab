/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _VITELOTTE_MORLEY_ELEMENT_BUILDER_
#define _VITELOTTE_MORLEY_ELEMENT_BUILDER_


#include <Eigen/Core>
#include <Eigen/Sparse>


#include "solverError.h"
#include "elementBuilderBase.h"


namespace Vitelotte
{


/**
 * \brief Morley element builder, for harmonic interpolation with linear
 * triangles output.
 */
template < class _Mesh, typename _Scalar = typename _Mesh::Scalar >
class MorleyElementBuilder : public ElementBuilderBase<_Mesh, _Scalar>
{
public:
    typedef _Scalar Scalar;
    typedef _Mesh Mesh;

    typedef ElementBuilderBase<_Mesh, _Scalar> Base;

    typedef typename Base::Vector Vector;
    typedef typename Base::Matrix Matrix;
    typedef typename Base::Triplet Triplet;

    typedef typename Base::Face Face;

    typedef Eigen::Matrix<Scalar, 6, 1> Vector6;

public:
    inline MorleyElementBuilder(Scalar sigma = Scalar(.5));

    unsigned nCoefficients(const Mesh& mesh, Face element,
                           SolverError* error=0) const;

    template < typename Inserter >
    void addCoefficients(Inserter& inserter, const Mesh& mesh,
                         Face element, SolverError* error=0);

private:
    Scalar m_sigma;
};


} // namespace Vitelotte

#include "morleyElementBuilder.hpp"


#endif
