/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _VITELOTTE_LINEAR_ELEMENT_BUILDER_
#define _VITELOTTE_LINEAR_ELEMENT_BUILDER_


#include <Eigen/Core>
#include <Eigen/Sparse>


#include "solverError.h"
#include "elementBuilderBase.h"


namespace Vitelotte
{


/**
 * \brief Linear element builder, for harmonic interpolation with linear
 * triangles output.
 */
template < class _Mesh, typename _Scalar = typename _Mesh::Scalar >
class LinearElementBuilder : public ElementBuilderBase<_Mesh, _Scalar>
{
public:
    typedef _Scalar Scalar;
    typedef _Mesh Mesh;

    typedef ElementBuilderBase<_Mesh, _Scalar> Base;

    typedef typename Base::Vector Vector;
    typedef typename Base::Matrix Matrix;
    typedef typename Base::Triplet Triplet;

    typedef typename Base::Face Face;

public:
    inline LinearElementBuilder();

    unsigned nCoefficients(const Mesh& mesh, Face element,
                           SolverError* error=0) const;

    template < typename Inserter >
    void addCoefficients(Inserter& inserter, const Mesh& mesh,
                         Face element, SolverError* error=0);
};


} // namespace Vitelotte

#include "linearElementBuilder.hpp"


#endif
