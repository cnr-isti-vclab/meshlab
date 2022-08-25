/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _VITELOTTE_MVG_READER_
#define _VITELOTTE_MVG_READER_


#include <cassert>
#include <stdexcept>
#include <istream>
#include <string>
#include <sstream>

#include "../../common/surface_mesh/objReader.h"


namespace Vitelotte
{


//class QVGReadError : public std::runtime_error
//{
//public:
//    inline QVGReadError(const std::string& _what)
//        : std::runtime_error(_what)
//    {}
//};


/**
 * \brief Reads a VGMesh stored in the MVG file format.
 *
 * \see VGMesh MVGWriter OBJBaseReader
 */
template < typename _Mesh >
class MVGReader: public PatateCommon::OBJBaseReader<_Mesh>
{
public:
    typedef _Mesh Mesh;
    typedef PatateCommon::OBJBaseReader<Mesh> Base;

    typedef typename Mesh::Vector      Vector;
    typedef typename Mesh::Vertex      Vertex;
    typedef typename Mesh::Value       Value;
    typedef typename Mesh::Gradient    Gradient;

    enum {
        NO_WARN_UNKNOWN = 0x01
    };

public:

    /**
     * \brief Default constructor
     */
    inline MVGReader(unsigned flags = 0);

protected:
    using Base::readLine;
    using Base::parseVector;
    using Base::parseIndicesList;
    using Base::error;
    using Base::warning;

    virtual void parseHeader(std::istream& in, Mesh& mesh);
    virtual bool parseDefinition(const std::string& spec,
                                 std::istream& def, Mesh& mesh);

    void parseValue        (std::istream& in);
    void parseValueWithVoid(std::istream& in, Mesh& mesh);
    void parseGradient     (std::istream& in);

protected:
    using Base::m_error;
    using Base::m_lineStream;
    using Base::m_vector;

    unsigned               m_flags;
    std::vector<Vertex>    m_fVertices;
    std::string            m_tmp;
    Value                  m_value;
    Gradient               m_gradient;
    std::vector<unsigned>  m_faceIndices;
    std::vector<unsigned>  m_nodesIndices;
};


template < typename Mesh >
bool readMvg(std::istream& in, Mesh& mesh, unsigned flags=0);

template < typename Mesh >
bool readMvgFromFile(const std::string& filename, Mesh& mesh, unsigned flags=0);


} // namespace Vitelotte

#include "mvgReader.hpp"


#endif // _MVGREADER_H_

