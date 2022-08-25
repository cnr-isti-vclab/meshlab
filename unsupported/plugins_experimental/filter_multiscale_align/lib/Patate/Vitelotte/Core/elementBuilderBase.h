/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _VITELOTTE_ELEMENT_BUILDER_BASE_
#define _VITELOTTE_ELEMENT_BUILDER_BASE_


#include <string>

#include <Eigen/Dense>
#include <Eigen/Sparse>


namespace Vitelotte
{


/**
 * \brief The base of every element builder. Provides default implementations
 * of some methods.
 */
template < class _Mesh, typename _Scalar >
class ElementBuilderBase
{
public:
    typedef _Scalar Scalar;
    typedef _Mesh Mesh;

    typedef Eigen::Matrix<Scalar, Mesh::DimsAtCompileTime, 1> Vector;
    typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
    typedef Eigen::Triplet<Scalar> Triplet;

    typedef typename Mesh::Face Face;

public:
    inline ElementBuilderBase() {}

    inline unsigned nExtraConstraints(const Mesh& /*mesh*/, Face /*element*/) const
    { return 0; }

    template < typename Inserter >
    inline void addExtraConstraints(Inserter& /*inserter*/, const Mesh& /*mesh*/,
                                    Face /*element*/, SolverError* /*error*/)
    {}
};


}


#endif
