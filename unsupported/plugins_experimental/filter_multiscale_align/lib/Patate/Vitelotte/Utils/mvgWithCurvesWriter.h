/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _VITELOTTE_MVG_WITH_CURVES_WRITER_
#define _VITELOTTE_MVG_WITH_CURVES_WRITER_


#include "mvgWriter.h"


namespace Vitelotte
{


template < typename _Mesh >
class MVGWithCurvesWriter: public Vitelotte::MVGWriter<_Mesh>
{
public:
    typedef Vitelotte::MVGWriter<_Mesh> Base;

    typedef _Mesh Mesh;

    typedef typename Mesh::Vector         Vector;
    typedef typename Mesh::Value          Value;
    typedef typename Mesh::ValueFunction  ValueFunction;

    typedef typename Mesh::Vertex Vertex;
    typedef typename Mesh::Halfedge Halfedge;
    typedef typename Mesh::PointConstraint PointConstraint;
    typedef typename Mesh::Curve Curve;

    typedef typename Base::Version        Version;

public:
    MVGWithCurvesWriter(Version version=Base::LATEST_VERSION);

    void write(std::ostream& _out, const Mesh& mesh);

protected:
    void writeValueFunction(std::ostream& out, const ValueFunction& vg) const;

protected:
    using Base::vertexIndex;

    using Base::m_format;
};

}

#include "mvgWithCurvesWriter.hpp"


#endif
