/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _PATATE_COMMON_SURFACE_MESH_OBJ_READER_
#define _PATATE_COMMON_SURFACE_MESH_OBJ_READER_


#include <stdexcept>
#include <string>
#include <istream>
#include <fstream>
#include <sstream>
#include <vector>

#include "surfaceMesh.h"


namespace PatateCommon
{


inline bool defaultErrorCallback  (const std::string& msg, unsigned line, void* ptr);
inline bool defaultWarningCallback(const std::string& msg, unsigned line, void* ptr);


template < typename _Mesh >
class OBJBaseReader
{
public:
    typedef _Mesh Mesh;
    typedef typename Mesh::Vector Vector;

    typedef bool (*ErrorCallback)(const std::string& msg, unsigned line, void* ptr);

public:
    inline OBJBaseReader()
        : m_error(false),
          m_errorCallback(defaultErrorCallback),
          m_warningCallback(defaultWarningCallback),
          m_errorCallbackPtr(0) {}
    virtual ~OBJBaseReader() {}

    bool read(std::istream& in, Mesh& mesh);

    void setErrorCallback(ErrorCallback error, ErrorCallback warning, void* ptr);

protected:

    virtual void parseHeader(std::istream& in, Mesh& mesh);
    virtual bool parseDefinition(const std::string& spec,
                                 std::istream& def, Mesh& mesh) = 0;

    inline bool readLine(std::istream& in);
    inline void parseVector(std::istream& in);
    inline void parseIndicesList(const std::string& _list,
                                 std::vector<unsigned>& _indices);

    inline void error  (const std::string& msg);
    inline void warning(const std::string& msg);

protected:
    unsigned  m_lineNb;
    bool      m_error;

    std::string         m_line;
    std::istringstream  m_lineStream;
    std::istringstream  m_indicesStream;
    Vector              m_vector;

    ErrorCallback  m_errorCallback;
    ErrorCallback  m_warningCallback;
    void*          m_errorCallbackPtr;
};


template < typename _Mesh >
class OBJReader: public OBJBaseReader<_Mesh>
{
public:
    typedef _Mesh Mesh;
    typedef OBJBaseReader<Mesh> Base;

    typedef typename Mesh::Vector Vector;
    typedef typename Mesh::Vertex Vertex;

public:
    inline OBJReader();

protected:
    using Base::parseVector;
    using Base::error;
    using Base::warning;

    virtual void parseHeader(std::istream& in, Mesh& mesh);
    virtual bool parseDefinition(const std::string& spec,
                                 std::istream& def, Mesh& mesh);

protected:
    using Base::m_vector;

    std::vector<Vertex>  m_fVertices;

};


}  // namespace PatateCommon

#include "objReader.hpp"


#endif  // _OBJREADER_H_
