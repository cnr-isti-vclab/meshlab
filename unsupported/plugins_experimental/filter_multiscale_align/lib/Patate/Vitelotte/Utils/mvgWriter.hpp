/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/


#include <vector>

#include "mvgWriter.h"


namespace Vitelotte {

template < typename _Mesh >
void
MVGWriter<_Mesh>::write(std::ostream& _out, const Mesh& mesh)
{
    typedef typename Mesh::HalfedgeAttribute HalfedgeAttribute;
    typedef typename Mesh::VertexIterator VertexIterator;
    typedef typename Mesh::FaceIterator FaceIterator;
    typedef typename Mesh::HalfedgeAroundFaceCirculator
            HalfedgeAroundFaceCirculator;
    typedef std::vector<bool> BoolMap;

    assert(m_version == VERSION_1_0);

    // Ensure that the stream we read encode numbers using the C locale
    _out.imbue(std::locale::classic());

    int iOffset = 0;

    _out << "mvg 1.0\n";
    _out << "dimensions " << mesh.nDims() << "\n";
    _out << "coefficients " << mesh.nCoeffs() << "\n";

    if(mesh.getAttributes() == 0)
        _out << "attributes none\n";
    else if(mesh.getAttributes() == Mesh::LINEAR_FLAGS)
        _out << "attributes linear\n";
    else if(mesh.getAttributes() == Mesh::QUADRATIC_FLAGS)
        _out << "attributes quadratic\n";
    else if(mesh.getAttributes() == Mesh::MORLEY_FLAGS)
        _out << "attributes morley\n";
    else if(mesh.getAttributes() == Mesh::FV_FLAGS)
        _out << "attributes fv\n";
    else
        _out << "attributes " << mesh.getAttributes() << "\n";

    _out << "colorSpace " << PatateCommon::getColorSpaceName(mesh.colorSpace()) << "\n";

    _out << "vertices " << mesh.nVertices() << "\n";
    _out << "nodes " << mesh.nNodes() << "\n";
    _out << "faces " << mesh.nFaces() << "\n";

    unsigned count = 0;
    m_vxMap.resize(mesh.nVertices(), -1);
    for(VertexIterator vit = mesh.verticesBegin();
        vit != mesh.verticesEnd(); ++vit)
    {
        m_vxMap[(*vit).idx()] = (count++);
        _out << "v " << mesh.position(*vit).transpose().format(m_format) << "\n";
    }

    // Find used nodes and only output them.
    BoolMap nodeUsed(mesh.nNodes(), false);
    for(FaceIterator fit = mesh.facesBegin();
         fit != mesh.facesEnd(); ++fit)
    {
        HalfedgeAroundFaceCirculator
                hit  = mesh.halfedges(*fit),
                hend = hit;
        do
        {
            for(unsigned aid = 0; aid < Mesh::HALFEDGE_ATTRIB_COUNT; ++aid) {
                HalfedgeAttribute attr = HalfedgeAttribute(aid);
                if(mesh.hasAttribute(attr) && mesh.halfedgeNode(*hit, attr).isValid())
                {
                    nodeUsed[mesh.halfedgeNode(*hit, attr).idx()] = true;
                }
            }
        }
        while(++hit != hend);
    }

    // Node printing conserve ordering
    count = 0;
    m_nodeMap.resize(mesh.nNodes(), -1);
    for(unsigned i = 0; i < mesh.nNodes(); ++i)
    {
        if(!nodeUsed[i]) continue;
        m_nodeMap[i] = (count++);
        if(mesh.isConstraint(Node(i)))
        {
            _out << "n " << mesh.value(Node(i)).transpose().format(m_format) << "\n";
        }
        else
        {
            _out << "n void\n";
        }
    }

    for(FaceIterator fit = mesh.facesBegin();
         fit != mesh.facesEnd(); ++fit)
    {
        _out << "f";

        HalfedgeAroundFaceCirculator
                hit  = mesh.halfedges(*fit),
                hend = hit;
        do
        {
            assert(vertexIndex(mesh.toVertex(*hit)) != -1);
            _out << " " << vertexIndex(mesh.toVertex(*hit)) + iOffset;

            if(mesh.hasToVertexValue())
            {
                Node vn = mesh.toVertexValueNode(*hit);
                _out << "/";
                printNode(_out, vn);

                // VertexFromValue only makes sense if vertexValue in enable.
                if(mesh.hasFromVertexValue())
                {
                    Node fn = mesh.fromVertexValueNode(mesh.nextHalfedge(*hit));
                    if(vn != fn)
                    {
                        _out << "/";
                        printNode(_out, fn);
                    }
                }
            }
        }
        while(++hit != hend);

        if(mesh.hasEdgeValue() || mesh.hasEdgeGradient())
        {
            _out << " -";

            ++hit; hend = hit;
            do
            {
                char sep = ' ';
                if(mesh.hasEdgeValue())
                {
                    _out << sep;
                    sep = '/';
                    printNode(_out, mesh.edgeValueNode(*hit));
                }
                if(mesh.hasEdgeGradient())
                {
                    _out << sep;
                    sep = '/';
                    printNode(_out, mesh.edgeGradientNode(*hit));
                }
            }
            while(++hit != hend);
        }

        _out << "\n";
    }

    for(typename Mesh::HalfedgeIterator hit = mesh.halfedgesBegin();
        hit != mesh.halfedgesEnd(); ++hit)
    {
        if(!mesh.isCurved(*hit) || mesh.halfedgeOrientation(*hit)) continue;
        const typename Mesh::CurvedEdge& ce = mesh.edgeCurve(*hit);
        if(ce.type() == BEZIER_EMPTY || ce.type() == BEZIER_LINEAR) continue;
        _out << "ce " << vertexIndex(mesh.fromVertex(*hit))
             << " "   << vertexIndex(mesh.toVertex(*hit))
             << " "   << ce.point(1).transpose().format(m_format);
        if(ce.type() == BEZIER_CUBIC) {
            _out << " " << ce.point(2).transpose().format(m_format);
        }
        _out << "\n";
    }

    if(mesh.hasVertexGradientConstraint())
    {
        for(VertexIterator vit = mesh.verticesBegin();
            vit != mesh.verticesEnd(); ++vit)
        {
            if(mesh.isGradientConstraint(*vit))
            {
                _out << "vgc " << vertexIndex(*vit) << " "
                     << mesh.gradientConstraint(*vit).transpose().format(m_format) << "\n";
            }
        }
    }
}


template < typename _Mesh >
int
MVGWriter<_Mesh>::vertexIndex(Vertex vx) const
{
    return m_vxMap[vx.idx()];
}


template < typename _Mesh >
int
MVGWriter<_Mesh>::nodeIndex(Node node) const
{
    return m_nodeMap[node.idx()];
}


template < typename _Mesh >
void
MVGWriter<_Mesh>::printNode(std::ostream& _out, Node n) const
{
    if(n.isValid())
    {
        assert(nodeIndex(n) != -1);
        _out << nodeIndex(n);
    }
    else
    {
        _out << "x";
    }
}


template < typename Mesh >
void writeMvg(std::ostream& out, const Mesh& mesh,
              typename MVGWriter<Mesh>::Version version)
{
    MVGWriter<Mesh> writer(version);
    writer.write(out, mesh);
}

template < typename Mesh >
void writeMvgToFile(const std::string& filename, const Mesh& mesh,
                    typename MVGWriter<Mesh>::Version version)
{
    std::ofstream out(filename.c_str());
    writeMvg(out, mesh, version);
}


}  // namespace Vitelotte
