/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include <cctype>

#include "objReader.h"


namespace PatateCommon
{


bool defaultErrorCallback(const std::string& msg, unsigned line, void* /*ptr*/)
{
    std::cerr << "Parse error: " << line << ": " << msg << "\n";
    return true;
}


bool defaultWarningCallback(const std::string& msg, unsigned line, void* /*ptr*/)
{
    std::cerr << "Warning: " << line << ": " << msg << "\n";
    return false;
}


template < typename _Mesh >
bool
OBJBaseReader<_Mesh>::read(std::istream& in, Mesh& mesh)
{
    m_lineNb = 0;
    m_error = false;
    m_line.reserve(256);
    std::string spec;

    in.imbue(std::locale::classic());

    if(!in.good())
    {
        error("Can not read input");
    }

    readLine(in);
    if(in.good() && !m_error)
        parseHeader(in, mesh);

    while(in.good() && !m_error)
    {
        // comment
        if(!m_line.empty() && m_line[0] != '#' && !std::isspace(m_line[0]))
        {
            m_lineStream >> spec;
            parseDefinition(spec, m_lineStream, mesh);
        }

        readLine(in);
    }

    return !m_error;
}


template < typename _Mesh >
void
OBJBaseReader<_Mesh>::setErrorCallback(ErrorCallback error,
                                       ErrorCallback warning, void* ptr)
{
    m_errorCallback    = error;
    m_warningCallback  = warning;
    m_errorCallbackPtr = ptr;
}


template < typename _Mesh >
void
OBJBaseReader<_Mesh>::parseHeader(std::istream& /*in*/, Mesh& /*mesh*/)
{
}


template < typename _Mesh >
bool
OBJBaseReader<_Mesh>::readLine(std::istream& in)
{
    bool ok = std::getline(in, m_line).good();
    ++m_lineNb;
    m_lineStream.str(m_line);
    m_lineStream.seekg(0);
    return ok;
}


template < typename _Mesh >
void
OBJBaseReader<_Mesh>::parseVector(std::istream& in) {
    for(unsigned i = 0; i < m_vector.size(); ++i) {
        in >> m_vector(i);
    }
    if(!in) error("Invalid point/vector specification");
    in >> std::ws;
}


template < typename _Mesh >
void
OBJBaseReader<_Mesh>::parseIndicesList(const std::string& _list,
                                       std::vector<unsigned>& _indices)
{
    _indices.clear();
    m_indicesStream.str(_list);
    m_indicesStream.seekg(0);
    while(m_indicesStream.good())
    {
        int i;
        if(m_indicesStream.peek() == '/' || m_indicesStream.peek() == 'x')
        {
            i = -1;
            m_indicesStream.get();
        }
        else
            m_indicesStream >> i;
        _indices.push_back(i);

        if(m_indicesStream.good() && m_indicesStream.peek() == '/')
            m_indicesStream.get();
        else if(!m_indicesStream.eof())
            error("Unexpected character in indices list");
    }
    if(!m_indicesStream)
        error("Failed to read indices list");
}


template < typename _Mesh >
void
OBJBaseReader<_Mesh>::error(const std::string& msg)
{
    if(m_errorCallback)
    {
        m_error = m_errorCallback(msg, m_lineNb, m_errorCallbackPtr) || m_error;
    }
}


template < typename _Mesh >
void
OBJBaseReader<_Mesh>::warning(const std::string& msg)
{
    if(m_warningCallback)
    {
        m_error = m_warningCallback(msg, m_lineNb, m_errorCallbackPtr) || m_error;
    }
}


template < typename _Mesh >
OBJReader<_Mesh>::OBJReader()
{
}


template < typename _Mesh >
void
OBJReader<_Mesh>::parseHeader(std::istream& /*in*/, Mesh& /*mesh*/) {
    m_vector.resize(3);
}


//template < typename _Scalar, int _Dims, int _Coeffs >
//void
//OBJReader<Vitelotte::VGMesh<_Scalar, _Dims, _Coeffs> >::parseHeader(
//        std::istream& /*in*/, Vitelotte::VGMesh<_Scalar, _Dims, _Coeffs>& mesh) {
//    mesh.setNDims(3);
//    m_vector.resize(3);
//}


template < typename _Mesh >
bool
OBJReader<_Mesh>::parseDefinition(const std::string& spec,
                                  std::istream& def, Mesh& mesh)
{
    // vertex
    if (spec == "v")
    {
        parseVector(def);
        if(!def.eof()) warning("Too much components");
        mesh.addVertex(m_vector);
    }
    // normal
//        else if (strncmp(s, "vn ", 3) == 0)
//        {
//            if (sscanf(s, "vn %f %f %f", &x, &y, &z))
//            {
//                // problematic as it can be either a vertex property when interpolated
//                // or a halfedge property for hard edges
//            }
//        }

    // texture coordinate
//        else if (strncmp(s, "vt ", 3) == 0)
//        {
//            if (sscanf(s, "vt %f %f", &x, &y))
//            {
//                z=1;
//                all_tex_coords.push_back(Texture_coordinate(x,y,z));
//            }
//        }

    // face
    else if (spec == "f")
    {
        m_fVertices.clear();

        def >> std::ws;

        while(def.good())
        {
            // TODO: Use parseIndiceList to read indices
            unsigned idx;
            def >> idx;
            idx -= 1;
            if(!def || idx >= mesh.nVertices())
            {
                error("Invalid vertex index");
                return true;
            }
            m_fVertices.push_back(SurfaceMesh::Vertex(idx));
        }

        mesh.addFace(m_fVertices);
    }
    else
    {
        warning("Unknown spec: " + spec);
        return false;
    }
    return true;
}


}
