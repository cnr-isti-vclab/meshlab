/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "linearElementBuilder.h"


namespace Vitelotte
{


template < class _Mesh, typename _Scalar >
LinearElementBuilder<_Mesh, _Scalar>::LinearElementBuilder()
{
}

template < class _Mesh, typename _Scalar >
unsigned
LinearElementBuilder<_Mesh, _Scalar>::
    nCoefficients(const Mesh& /*mesh*/, Face /*element*/,
                  SolverError* /*error*/) const
{
    return 6;
}


template < class _Mesh, typename _Scalar >
template < typename Inserter >
void
LinearElementBuilder<_Mesh, _Scalar>::
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
    int nodes[3];

    typename Mesh::HalfedgeAroundFaceCirculator hit = mesh.halfedges(element);
    --hit;
    for(int i = 0; i < 3; ++i)
    {
        v[i] = (mesh.position(mesh.toVertex(*hit)) -
                mesh.position(mesh.fromVertex(*hit))).template cast<Scalar>();
        ++hit;
        nodes[i] = mesh.toVertexValueNode(*hit).idx();
        if(nodes[i] < 0)
        {
            if(error) error->error("Invalid node");
            return;
        }
    }

    Scalar _2area = det2(v[0], v[1]);
    Scalar inv4A = 1. / (2. * _2area);

    if(_2area <= 0 && error)
    {
        if(error) error->error("Degenerated or reversed triangle");
    }

    for(int i = 0; i < 3; ++i)
    {
        for(int j = i; j < 3; ++j)
        {
            inserter.addCoeff(nodes[i], nodes[j], v[i].dot(v[j]) * inv4A);
        }
    }
}


}
