/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _VITELOTTE_MVG_WRITER_
#define _VITELOTTE_MVG_WRITER_


#include <cassert>
#include <ostream>
#include <fstream>
#include <vector>

#include <Eigen/Dense>


namespace Vitelotte
{


/**
 * \brief The MVGWriter class writes VGMesh objects in `.mvg` files.
 *
 * \see VGMesh MVGReader
 */
template < typename _Mesh >
class MVGWriter
{
public:
    typedef _Mesh Mesh;

    typedef typename Mesh::Vertex      Vertex;
    typedef typename Mesh::Node        Node;

    typedef std::vector<int> IndexMap;

    enum Version
    {
        VERSION_1_0 = 0x100,
        LATEST_VERSION = VERSION_1_0
    };


public:

    /**
     * \brief Constructor
     */
    inline MVGWriter(Version version=LATEST_VERSION)
        : m_version(version),
          m_format(Eigen::StreamPrecision, Eigen::DontAlignCols, " ", " ")
    {}

    /**
     * \brief Write `mesh` in the stream `out` as a `.mvg` file.
     */
    void write(std::ostream& out, const Mesh& mesh);

protected:
    int vertexIndex(Vertex vx) const;
    int nodeIndex(Node node) const;

    void printNode(std::ostream& _out, Node n) const;

protected:
    Version m_version;
    Eigen::IOFormat m_format;
    IndexMap m_vxMap;
    IndexMap m_nodeMap;
};


template < typename Mesh >
void writeMvg(std::ostream& out, const Mesh& mesh,
              typename MVGWriter<Mesh>::Version version=MVGWriter<Mesh>::LATEST_VERSION);

template < typename Mesh >
void writeMvgToFile(const std::string& filename, const Mesh& mesh,
                    typename MVGWriter<Mesh>::Version version=MVGWriter<Mesh>::LATEST_VERSION);


} // namespace Vitelotte

#include "mvgWriter.hpp"


#endif // _QVGWRITER_H_
