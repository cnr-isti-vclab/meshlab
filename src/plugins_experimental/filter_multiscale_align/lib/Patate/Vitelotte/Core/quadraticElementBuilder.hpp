/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "quadraticElementBuilder.h"


namespace Vitelotte
{


template < class _Mesh, typename _Scalar >
void
QuadraticElementBuilder<_Mesh, _Scalar>::initializeMatrices()
{
    if(m_matricesInitialized)
        return;
    m_matricesInitialized = true;

    // 'Magic' initialization of base matrices for quadratic elements.
    // See Paul Tsipouras, Compact representation of triangular finite
    //  elements for Poisson's equation, International Journal for
    //  Numerical Methods in Engineering, Volume 11, Issue 3, pages 419-430,
    //  1977

    const Scalar _1_6 = Scalar(1.) / Scalar(6.);
    const Scalar _2_3 = Scalar(2.) / Scalar(3.);
    const Scalar _4_3 = Scalar(4.) / Scalar(3.);

    m_quadM1 <<
            1,  _1_6,  _1_6,     0, -_2_3, -_2_3,
         _1_6,     0, -_1_6,  _2_3,     0, -_2_3,
         _1_6, -_1_6,     0,  _2_3, -_2_3,     0,
            0,  _2_3,  _2_3,  _4_3, -_4_3, -_4_3,
        -_2_3,     0, -_2_3, -_4_3,  _4_3,  _4_3,
        -_2_3, -_2_3,     0, -_4_3,  _4_3,  _4_3;

    m_quadM2 <<
            0,  _1_6, -_1_6,     0,  _2_3, -_2_3,
         _1_6,     1,  _1_6, -_2_3,     0, -_2_3,
        -_1_6,  _1_6,     0, -_2_3,  _2_3,     0,
            0, -_2_3, -_2_3,  _4_3, -_4_3,  _4_3,
         _2_3,     0,  _2_3, -_4_3,  _4_3, -_4_3,
        -_2_3, -_2_3,     0,  _4_3, -_4_3,  _4_3;

    m_quadM3 <<
            0, -_1_6,  _1_6,     0, -_2_3,  _2_3,
        -_1_6,     0,  _1_6, -_2_3,     0,  _2_3,
         _1_6,  _1_6,     1, -_2_3, -_2_3,     0,
            0, -_2_3, -_2_3,  _4_3,  _4_3, -_4_3,
        -_2_3,     0, -_2_3,  _4_3,  _4_3, -_4_3,
         _2_3,  _2_3,     0, -_4_3, -_4_3,  _4_3;
}

template < class _Mesh, typename _Scalar >
QuadraticElementBuilder<_Mesh, _Scalar>::QuadraticElementBuilder()
{
    initializeMatrices();
}

template < class _Mesh, typename _Scalar >
unsigned
QuadraticElementBuilder<_Mesh, _Scalar>::
    nCoefficients(const Mesh& /*mesh*/, Face /*element*/,
                  SolverError* /*error*/) const
{
    return 36;
}


template < class _Mesh, typename _Scalar >
template < typename Inserter >
void
QuadraticElementBuilder<_Mesh, _Scalar>::
    addCoefficients(Inserter& inserter, const Mesh& mesh,
                    Face element, SolverError* error)
{
    if(mesh.valence(element) != 3)
    {
        if(error) error->error("Non-triangular face");
        return;
    }

    // TODO: remove dynamic allocation with dynamic dims.
    Vector v[3];
    int nodes[6];

    typename Mesh::HalfedgeAroundFaceCirculator hit = mesh.halfedges(element);
    --hit;
    for(int i = 0; i < 3; ++i)
    {
        v[i] = (mesh.position(mesh.toVertex(*hit)) -
                mesh.position(mesh.fromVertex(*hit))).template cast<Scalar>();
        nodes[3+i] = mesh.edgeValueNode(*hit).idx();
        ++hit;
        nodes[i] = mesh.toVertexValueNode(*hit).idx();
    }

    for(int i = 0; i < 6; ++i)
    {
        if(nodes[i] < 0)
        {
            if(error) error->error("Invalid node");
            return;
        }
    }

    Scalar _2area = det2(v[0], v[1]);

    if(_2area <= 0 && error)
    {
        error->warning("Degenerated or reversed triangle");
    }

    Scalar inv4A = 1. / (2. * _2area);

    ElementStiffnessMatrix matrix = (m_quadM1 * v[0].squaredNorm() +
                                     m_quadM2 * v[1].squaredNorm() +
                                     m_quadM3 * v[2].squaredNorm()) * inv4A;

    for(int i = 0; i < 6; ++i)
    {
        for(int j = i; j < 6; ++j)
        {
            inserter.addCoeff(nodes[i], nodes[j], matrix(i, j));
        }
    }
}

template < class _Mesh, typename _Scalar >
bool QuadraticElementBuilder<_Mesh, _Scalar>::m_matricesInitialized = false;

template < class _Mesh, typename _Scalar >
typename QuadraticElementBuilder<_Mesh, _Scalar>::ElementStiffnessMatrix
    QuadraticElementBuilder<_Mesh, _Scalar>::m_quadM1;

template < class _Mesh, typename _Scalar >
typename QuadraticElementBuilder<_Mesh, _Scalar>::ElementStiffnessMatrix
    QuadraticElementBuilder<_Mesh, _Scalar>::m_quadM2;

template < class _Mesh, typename _Scalar >
typename QuadraticElementBuilder<_Mesh, _Scalar>::ElementStiffnessMatrix
    QuadraticElementBuilder<_Mesh, _Scalar>::m_quadM3;


}
