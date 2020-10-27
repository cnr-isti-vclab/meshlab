/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "singularElementDecorator.h"


namespace Vitelotte
{


namespace internal {

template < typename Mesh, typename Inserter >
class SingularInserter
{
public:
    typedef typename Inserter::Scalar Scalar;
    typedef typename Mesh::Face Face;
    typedef typename Mesh::HalfedgeAroundFaceCirculator HCirc;

public:
    SingularInserter(Inserter& inserter, const Mesh& mesh, Face face)
        : m_inserter(inserter), m_from(-1), m_to(-1) {

        HCirc hit  = mesh.halfedges(face);
        HCirc hend = hit;
        do
        {
            if(mesh.isSingular(*hit)) break;
            ++hit;
        } while(hit != hend);
        assert(mesh.isSingular(*hit));

        m_from = mesh.toVertexValueNode(*hit).idx();
        m_to   = mesh.fromVertexValueNode(mesh.nextHalfedge(*hit)).idx();
    }

    inline unsigned map(unsigned ni) {
        return (ni == unsigned(m_from))? m_to: ni;
    }

    inline void addCoeff(unsigned ni0, unsigned ni1, Scalar value) {
        m_inserter.addCoeff(    ni0 ,     ni1 , value);
        m_inserter.addCoeff(map(ni0), map(ni1), value);
    }

    inline void addExtraCoeff(Face elem, unsigned ei, unsigned ni, Scalar value) {
        m_inserter.addExtraCoeff(elem, ei*2 + 0,     ni , value);
        m_inserter.addExtraCoeff(elem, ei*2 + 1, map(ni), value);
    }

    template < typename Derived >
    inline void setExtraRhs(Face elem, unsigned ei, const Eigen::DenseBase<Derived>& value) {
        m_inserter.setExtraRhs(elem, ei*2 + 0, value);
        m_inserter.setExtraRhs(elem, ei*2 + 1, value);
    }

private:
    Inserter& m_inserter;
    int m_from;
    int m_to;
};

}


template < typename _Element >
unsigned
SingularElementDecorator<_Element>::nCoefficients(
        const Mesh& mesh, Face element, SolverError* error) const
{
    return Base::nCoefficients(mesh, element, error) *
            (mesh.nSingulars(element)? 2: 1);
}


template < typename _Element >
unsigned
SingularElementDecorator<_Element>::nExtraConstraints(
        const Mesh& mesh, Face element) const
{
    return Base::nExtraConstraints(mesh, element) *
            (mesh.nSingulars(element)? 2: 1);
}


template < typename _Element >
template < typename Inserter >
void
SingularElementDecorator<_Element>::addCoefficients(
        Inserter& inserter, const Mesh& mesh,
        Face element, SolverError* error)
{
    unsigned nSingular = mesh.nSingulars(element);

    if(nSingular > 1 && error)
        error->warning("Element with more than one singular vertex");

    if(!nSingular) {
        Base::addCoefficients(inserter, mesh, element, error);
    } else {
        typedef internal::SingularInserter<Mesh, Inserter> SInserter;
        SInserter sInserter(inserter, mesh, element);
        Base::addCoefficients(sInserter, mesh, element, error);
    }
}


}
