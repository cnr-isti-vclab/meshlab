/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _VITELOTTE_MVG_WITH_CURVES_READER_
#define _VITELOTTE_MVG_WITH_CURVES_READER_


#include "mvgReader.h"


namespace Vitelotte
{


template < typename _Mesh >
class MVGWithCurvesReader: public Vitelotte::MVGReader<_Mesh>
{
public:
    typedef Vitelotte::MVGReader<_Mesh> Base;

    typedef _Mesh Mesh;

    typedef typename Mesh::Vector Vector;
    typedef typename Mesh::Value Value;
    typedef typename Mesh::ValueFunction ValueFunction;

    typedef typename Mesh::Vertex Vertex;
    typedef typename Mesh::Halfedge Halfedge;
    typedef typename Mesh::PointConstraint PointConstraint;
    typedef typename Mesh::Curve Curve;

    using Base::parseDefinition;

public:
    MVGWithCurvesReader();


protected:
    enum
    {
        CONS_LEFT = 1,
        CONS_RIGHT = 2,
        TEAR = 4
    };

protected:
    using Base::parseVector;
    using Base::parseValue;
    using Base::parseValueWithVoid;
    using Base::parseGradient;
    using Base::error;
    using Base::warning;

    using Base::m_vector;
    using Base::m_value;
    using Base::m_gradient;
    using Base::m_error;

    virtual bool parseDefinition(const std::string& spec,
                                 std::istream& def, Mesh& mesh);

protected:
    std::string m_part;
    std::string m_token;
    std::istringstream m_in;

};

}

#include "mvgWithCurvesReader.hpp"


#endif
