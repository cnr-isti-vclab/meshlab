/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "vgMesh.h"


namespace Vitelotte
{


template < typename _Scalar, int _Dim, int _Chan >
VGMesh<_Scalar, _Dim, _Chan>::VGMesh(unsigned attributes)
    : m_attributes(0),
      m_deletedNodes(0)
{
    m_ndeleted = addNodeProperty<bool>("n:deleted", false);
    setAttributes(attributes);
}


template < typename _Scalar, int _Dim, int _Chan >
VGMesh<_Scalar, _Dim, _Chan>::VGMesh(
        unsigned nDims, unsigned nCoeffs, unsigned attributes)
    : m_attributes(0),
      m_deletedNodes(0)
{
    setNDims(nDims);
    setNCoeffs(nCoeffs);
    m_ndeleted = addNodeProperty<bool>("n:deleted", false);
    setAttributes(attributes);
}


template < typename _Scalar, int _Dim, int _Chan >
VGMesh<_Scalar, _Dim, _Chan>::VGMesh(const Self& other)
    : PatateCommon::SurfaceMesh(other)
{
    operator=(other);
    copyVGMeshMembers(other);
}


template < typename _Scalar, int _Dim, int _Chan >
VGMesh<_Scalar, _Dim, _Chan>&
VGMesh<_Scalar, _Dim, _Chan>::operator=(const Self& rhs)
{
    if(&rhs != this)
    {
        // FIXME: SurfaceMesh's operator= wont work with properties of different types.
        PatateCommon::SurfaceMesh::operator=(rhs);
        copyVGMeshMembers(rhs);
    }
    return *this;
}


template < typename _Scalar, int _Dim, int _Chan >
VGMesh<_Scalar, _Dim, _Chan>&
VGMesh<_Scalar, _Dim, _Chan>::assign(const Self& rhs)
{
    // FIXME: Implement this properly, ie. copy VGMesh properties without other
    //   custom properties. Disable it in the meantime.
    assert(false);
    if(&rhs != this)
    {
        // FIXME: SurfaceMesh's operator= wont work with properties of different types.
        PatateCommon::SurfaceMesh::assign(rhs);
        // Note: this function is unsuitable for assing()
        copyVGMeshMembers(rhs);
    }
    return *this;
}


template < typename _Scalar, int _Dim, int _Chan >
void
VGMesh<_Scalar, _Dim, _Chan>::setNDims(unsigned nDims)
{
    assert(int(DimsAtCompileTime) == int(Dynamic) ||
           int(nDims) == int(DimsAtCompileTime));
    resizePositionsMatrix(nDims, positionsCapacity());
}


template < typename _Scalar, int _Dim, int _Chan >
void
VGMesh<_Scalar, _Dim, _Chan>::setNCoeffs(unsigned nCoeffs)
{
    assert(int(CoeffsAtCompileTime) == int(Dynamic) ||
           int(nCoeffs) == int(CoeffsAtCompileTime));
    resizeNodesMatrix(nCoeffs, nodesCapacity());
}


template < typename _Scalar, int _Dim, int _Chan >
void
VGMesh<_Scalar, _Dim, _Chan>::reserve(
        unsigned nvertices, unsigned nedges, unsigned nfaces, unsigned nnodes)
{
    PatateCommon::SurfaceMesh::reserve(nvertices, nedges, nfaces);
    if(nnodes > nodesCapacity())
    {
        if(nnodes > nodesCapacity()) resizeNodesMatrix(nCoeffs(), nnodes);
        m_nprops.reserve(nnodes);
    }
}


template < typename _Scalar, int _Dim, int _Chan >
void
VGMesh<_Scalar, _Dim, _Chan>::clear()
{
    PatateCommon::SurfaceMesh::clear();
    m_nprops.resize(0);
    m_nprops.freeMemory();
    m_deletedNodes = 0;
    resizePositionsMatrix(nDims(), 0);
    resizeNodesMatrix(nCoeffs(), 0);
}


template < typename _Scalar, int _Dim, int _Chan >
void
VGMesh<_Scalar, _Dim, _Chan>::garbageCollection(unsigned flags)
{
    // FIXME: this operation changes vertex order, so it can break edge
    // orientation. A solution is to invert gradient nodes of affected edges
    PatateCommon::SurfaceMesh::garbageCollection(GC_DONT_RELEASE_INDEX_MAPS);

    unsigned nN = nodesSize();

    m_gcNodeMap.resize(nN);
    for(unsigned i = 0; i < nN; ++i) m_gcNodeMap[i] = Node(i);


    // remove deleted nodes
    if(nN > 0)
    {
        int i0 = 0;
        int i1 = nN - 1;

        while (1)
        {
            // find first deleted and last un-deleted
            while(!m_ndeleted[Node(i0)] && i0 < i1)  ++i0;
            while( m_ndeleted[Node(i1)] && i0 < i1)  --i1;
            if(i0 >= i1) break;

            // swap
            m_nprops.swap(i0, i1);
            std::swap(m_gcNodeMap[i0], m_gcNodeMap[i1]);
        };

        // remember new size
        nN = m_ndeleted[Node(i0)] ? i0 : i0+1;
    }



    // remap vertices
    for(unsigned vi = 0; vi < nVertices(); ++vi) {
        Vertex v(vi);
        if(v != gcMap(v))
        {
            m_positions.col(vi) = m_positions.col(gcMap(v).idx());
        }
    }

    VertexGradientMap vxGradConstraints;
    for(typename VertexGradientMap::const_iterator vxGrad = m_vertexGradientConstraints.begin();
        vxGrad != m_vertexGradientConstraints.end(); ++vxGrad)
    {
        vxGradConstraints.insert(std::make_pair(gcMap(vxGrad->first), vxGrad->second));
    }
    m_vertexGradientConstraints.swap(vxGradConstraints);

    // remap halfedges
    for(HalfedgeIterator hit = halfedgesBegin(); hit != halfedgesEnd(); ++hit)
    {
        for(unsigned ai = 0; ai < HALFEDGE_ATTRIB_COUNT; ++ai)
        {
            HalfedgeAttribute attr = HalfedgeAttribute(ai);
            if(hasAttribute(attr))
            {
                Node& n = halfedgeNode(*hit, attr);
                if(n.isValid()) n = gcMap(n);
            }
        }
    }

    // remap nodes
    for(unsigned ni = 0; ni < nN; ++ni)
    {
        Node n(ni);
        if(n != gcMap(n))
        {
            m_nodes.col(ni) = m_nodes.col(gcMap(n).idx());
        }
    }

    if(!(flags & GC_DONT_RELEASE_INDEX_MAPS))
    {
        releaseGCIndexMaps();
    }

    m_nprops.resize(nN); m_nprops.freeMemory();

    m_deletedNodes = 0;
    m_garbage = false;
}


template < typename _Scalar, int _Dim, int _Chan >
void
VGMesh<_Scalar, _Dim, _Chan>::releaseGCIndexMaps()
{
    PatateCommon::SurfaceMesh::releaseGCIndexMaps();
    std::vector<Node> nMap;
    m_gcNodeMap.swap(nMap);
}


template < typename _Scalar, int _Dim, int _Chan >
template < typename Derived >
PatateCommon::SurfaceMesh::Vertex
VGMesh<_Scalar, _Dim, _Chan>::addVertex(const Eigen::DenseBase<Derived>& pos)
{
    if(positionsCapacity() == verticesSize())
    {
        unsigned size = std::max(16u, verticesSize() * 2);
        resizePositionsMatrix(nDims(), size);
    }
    m_positions.col(verticesSize()) = pos;
    Vertex v = PatateCommon::SurfaceMesh::addVertex();
    assert(nodesSize() <= positionsCapacity());
    return v;
}


template < typename _Scalar, int _Dim, int _Chan >
bool
VGMesh<_Scalar, _Dim, _Chan>::isValid(Node n) const
{
    return 0 <= n.idx() && unsigned(n.idx()) < nodesSize();
}


template < typename _Scalar, int _Dim, int _Chan >
void VGMesh<_Scalar, _Dim, _Chan>::setAttributes(unsigned attributes)
{
    for(int ai = 0; ai < HALFEDGE_ATTRIB_COUNT; ++ai) {
        unsigned flag = PATATE_VG_MESH_HALFEDGE_ATTR_FLAG(ai);
        if(!(m_attributes & flag) && (attributes & flag)) {
            m_halfedgeAttributes[ai] =
                    addHalfedgeProperty<Node>(_halfedgeAttrName[ai], Node());
        } else if((m_attributes & flag) && !(attributes & flag)) {
            removeHalfedgeProperty(m_halfedgeAttributes[ai]);
        }
    }


    m_attributes = attributes;
}


template < typename _Scalar, int _Dim, int _Chan >
typename VGMesh<_Scalar, _Dim, _Chan>::HalfedgeAttribute
VGMesh<_Scalar, _Dim, _Chan>::
    oppositeAttribute(HalfedgeAttribute attr) const
{
    switch(attr)
    {
    case TO_VERTEX_VALUE:
        return FROM_VERTEX_VALUE;
    case FROM_VERTEX_VALUE:
        return TO_VERTEX_VALUE;
    case EDGE_VALUE:
    case EDGE_GRADIENT:
        return attr;
    default:
        break;
    }
    abort();
}


template < typename _Scalar, int _Dim, int _Chan >
typename VGMesh<_Scalar, _Dim, _Chan>::Node
VGMesh<_Scalar, _Dim, _Chan>::
    halfedgeNode(Halfedge h, HalfedgeAttribute attr) const
{
    return const_cast<Self*>(this)->halfedgeNode(h, attr);
}


template < typename _Scalar, int _Dim, int _Chan >
typename VGMesh<_Scalar, _Dim, _Chan>::Node&
VGMesh<_Scalar, _Dim, _Chan>::
    halfedgeNode(Halfedge h, HalfedgeAttribute attr)
{
    if(attr == FROM_VERTEX_VALUE && !hasFromVertexValue())
        attr = TO_VERTEX_VALUE;

    assert(hasAttribute(attr));
    return m_halfedgeAttributes[attr][h];
}


template < typename _Scalar, int _Dim, int _Chan >
typename VGMesh<_Scalar, _Dim, _Chan>::Node
VGMesh<_Scalar, _Dim, _Chan>::
    halfedgeOppositeNode(Halfedge h, HalfedgeAttribute attr) const
{
    return const_cast<Self*>(this)->halfedgeOppositeNode(h, attr);
}


template < typename _Scalar, int _Dim, int _Chan >
typename VGMesh<_Scalar, _Dim, _Chan>::Node&
VGMesh<_Scalar, _Dim, _Chan>::
    halfedgeOppositeNode(Halfedge h, HalfedgeAttribute attr)
{
    return halfedgeNode(oppositeHalfedge(h), oppositeAttribute(attr));
}


template < typename _Scalar, int _Dim, int _Chan >
void VGMesh<_Scalar, _Dim, _Chan>::
    setGradientConstraint(Vertex v, const Gradient& grad)
{
    assert(hasVertexGradientConstraint());

    m_vertexGradientConstraints[v] = grad;
}


template < typename _Scalar, int _Dim, int _Chan >
void VGMesh<_Scalar, _Dim, _Chan>::removeGradientConstraint(Vertex v)
{
    assert(hasVertexGradientConstraint());

    m_vertexGradientConstraints.erase(v);
}


template < typename _Scalar, int _Dim, int _Chan >
unsigned VGMesh<_Scalar, _Dim, _Chan>::nVertexGradientConstraints(Halfedge h) const
{
    assert(hasVertexGradientConstraint());

    return isGradientConstraint(fromVertex(h)) + isGradientConstraint(toVertex(h));
}


template < typename _Scalar, int _Dim, int _Chan >
unsigned VGMesh<_Scalar, _Dim, _Chan>::nVertexGradientConstraints(Face f) const
{
    assert(hasVertexGradientConstraint());

    VertexAroundFaceCirculator vit = vertices(f);
    VertexAroundFaceCirculator vend = vit;
    unsigned count = 0;

    do
    {
        if(isGradientConstraint(*vit)) ++count;
        ++vit;
    } while(vit != vend);

    return count;
}


template < typename _Scalar, int _Dim, int _Chan >
void
VGMesh<_Scalar, _Dim, _Chan>::deleteUnusedNodes()
{
    for(unsigned i = 0; i < nodesSize(); ++i)
    {
        m_ndeleted[Node(i)] = true;
    }

    HalfedgeIterator hBegin = halfedgesBegin(),
                     hEnd   = halfedgesEnd();
    for(HalfedgeIterator hit = hBegin; hit != hEnd; ++hit)
    {
        if(!isBoundary(*hit))
        {
            if(hasToVertexValue() && toVertexValueNode(*hit).isValid())
                m_ndeleted[toVertexValueNode(*hit)]   = false;
            if(hasFromVertexValue() && fromVertexValueNode(*hit).isValid())
                m_ndeleted[fromVertexValueNode(*hit)] = false;
            if(hasEdgeValue() && edgeValueNode(*hit).isValid())
                m_ndeleted[edgeValueNode(*hit)]       = false;
            if(hasEdgeGradient() && edgeGradientNode(*hit).isValid())
                m_ndeleted[edgeGradientNode(*hit)]    = false;
        }
    }
    m_deletedNodes = 0;
    for(unsigned ni = 0; ni < nodesSize(); ++ni)
    {
        if(m_ndeleted[Node(ni)]) ++m_deletedNodes;
    }
    if(m_deletedNodes) m_garbage = true;
}


template < typename _Scalar, int _Dim, int _Chan >
template <typename Derived>
typename VGMesh<_Scalar, _Dim, _Chan>::Node
VGMesh<_Scalar, _Dim, _Chan>::addNode(const Eigen::DenseBase<Derived>& value)
{
    if(nodesCapacity() == nodesSize())
    {
        unsigned size = std::max(16u, nodesSize() * 2);
        resizeNodesMatrix(nCoeffs(), size);
    }
    m_nodes.col(nodesSize()) = value;
    m_nprops.pushBack();
    assert(nodesSize() <= nodesCapacity());
    return Node(nodesSize() - 1);
}


template < typename _Scalar, int _Dim, int _Chan >
bool
VGMesh<_Scalar, _Dim, _Chan>::hasUnknowns() const
{
    for(NodeIterator nit = nodesBegin();
            nit != nodesEnd(); ++nit)
        if(!isConstraint(*nit))
            return true;
    return false;
}


template < typename _Scalar, int _Dim, int _Chan >
unsigned
VGMesh<_Scalar, _Dim, _Chan>::nUnknowns() const
{
    unsigned nUnk = 0;
    for(NodeIterator nit = nodesBegin();
            nit != nodesEnd(); ++nit)
        if(!isConstraint(*nit))
            ++nUnk;
    return nUnk;
}


template < typename _Scalar, int _Dim, int _Chan >
void
VGMesh<_Scalar, _Dim, _Chan>::
    setVertexNodes(Node node, Halfedge from, Halfedge to)
{
    assert(fromVertex(from) == fromVertex(to));
    assert(hasToVertexValue());

    Halfedge h = from;
    do
    {
        if(hasFromVertexValue() && !isBoundary(h))
        {
            fromVertexValueNode(h) = node;
        }

        h = prevHalfedge(h);

        if(!isBoundary(h))
        {
            toVertexValueNode(h) = node;
        }

        h = oppositeHalfedge(h);
    }
    while(h != to && h != from);
}


template < typename _Scalar, int _Dim, int _Chan >
void
VGMesh<_Scalar, _Dim, _Chan>::
    setSingularity(Node fromNode, Node toNode, Halfedge from, Halfedge to)
{
    assert(fromVertex(from) == fromVertex(to));
    assert(hasToVertexValue() && hasFromVertexValue());
    assert(isConstraint(fromNode) && isConstraint(toNode));

    // Must be copies, not refs because m_nodes can be reallocated.
    Value fromValue = value(fromNode);
    Value   toValue = value(  toNode);

    const Vector& v = position(fromVertex(from));
    Vector fromVec = position(toVertex(from)) - v;
    Vector   toVec = position(toVertex(  to)) - v;

    Scalar fromAngle = std::atan2(fromVec.y(), fromVec.x());
    Scalar toAngle   = std::atan2(  toVec.y(),   toVec.x());
    Scalar totalAngle = toAngle - fromAngle;
    if(totalAngle < Scalar(1.e-8)) totalAngle += Scalar(2.*M_PI);

    Node n = fromNode;
    Halfedge h = from;
    Halfedge last = oppositeHalfedge(to);
    do
    {
        if(!isBoundary(h))
            fromVertexValueNode(h) = n;

        h = prevHalfedge(h);

        if(h != last)
        {
            // Current Halfedge is reversed.
            Vector vec = position(fromVertex(h)) - v;
            Scalar angle = std::atan2(vec.y(), vec.x()) - fromAngle;
            if(angle < Scalar(1.e-8)) angle += Scalar(2.*M_PI);
            Scalar a = angle / totalAngle;
            n = addNode((Scalar(1)-a) * fromValue + a * toValue);
        }
        else
            n = toNode;

        if(!isBoundary(h))
            toVertexValueNode(h) = n;

        h = oppositeHalfedge(h);
    }
    while(h != to && h != from);
}


template < typename _Scalar, int _Dim, int _Chan >
void
VGMesh<_Scalar, _Dim, _Chan>::simplifyConstraints()
{
    assert(hasToVertexValue());

    std::vector<Halfedge> consEdges;
    consEdges.reserve(12);

    for(VertexIterator vit = verticesBegin();
        vit != verticesEnd(); ++vit)
    {
        consEdges.clear();
        findConstrainedEdgesSimplify(*vit, consEdges);

        Halfedge prev = consEdges.back();
        std::vector<Halfedge>::iterator cit = consEdges.begin();
        for(; cit != consEdges.end(); ++cit)
        {
            simplifyVertexArcConstraints(prev, *cit);
            prev = *cit;
        }
    }

    if(hasEdgeValue() || hasEdgeGradient())
    {
        for(EdgeIterator eit = edgesBegin();
            eit != edgesEnd(); ++eit)
        {
            simplifyEdgeConstraints(*eit);
        }
    }
}


template < typename _Scalar, int _Dim, int _Chan >
void
VGMesh<_Scalar, _Dim, _Chan>::simplifyVertexArcConstraints(
        Halfedge from, Halfedge to)
{
    assert(hasToVertexValue());
    assert(fromVertex(from) == fromVertex(to));

    Node& n0 = halfedgeNode(from, FROM_VERTEX_VALUE);
    Node& n1 = halfedgeOppositeNode(to, FROM_VERTEX_VALUE);

    bool n0c = n0.isValid() && isConstraint(n0);
    bool n1c = n1.isValid() && isConstraint(n1);

    if((!n0c && !n1c) ||
       (n0c && !n1c) ||
       (n0c && n1c && value(n0) == value(n1)))
    {
        Node& n1o = halfedgeNode(to, FROM_VERTEX_VALUE);
        if(n1o == n1)
        {
            n1o = n0;
        }
        n1 = n0;
    }
    else if(!n0c && n1c)
    {
        Node toReplace = n0;
        Halfedge h = from;
        while(true) {
            Node& n = halfedgeNode(h, FROM_VERTEX_VALUE);
            if(n == toReplace)
                n = n1;
            else
                break;

            Node& no = halfedgeOppositeNode(h, FROM_VERTEX_VALUE);
            if(no == toReplace)
                n = n1;
            else
                break;

            h = nextHalfedge(h);
        }
    }
}


template < typename _Scalar, int _Dim, int _Chan >
void
VGMesh<_Scalar, _Dim, _Chan>::simplifyEdgeConstraints(Edge e)
{
    Halfedge h0 = halfedge(e, 0);
    Halfedge h1 = halfedge(e, 1);
    if(hasEdgeValue())
    {
        Node& n0 = edgeValueNode(h0);
        Node& n1 = edgeValueNode(h1);
        simplifyOppositeNodes(n0, n1, isBoundary(h0), isBoundary(h1));
    }
    if(hasEdgeGradient())
    {
        Node& n0 = edgeGradientNode(h0);
        Node& n1 = edgeGradientNode(h1);
        simplifyOppositeNodes(n0, n1, isBoundary(h0), isBoundary(h1));
    }
}


template < typename _Scalar, int _Dim, int _Chan >
void
VGMesh<_Scalar, _Dim, _Chan>::simplifyOppositeNodes(Node& n0, Node& n1,
                                                    bool b0, bool b1) const
{
    bool n0c = !b0 && n0.isValid() && isConstraint(n0);
    bool n1c = !b1 && n1.isValid() && isConstraint(n1);

    // if not a discontinuity, merge nodes.
    if(n0c && n1c && value(n0) == value(n1))
    {
        if(!b0) n0 = Node(std::min(n1.idx(), n0.idx()));
        if(!b1) n1 = Node(std::min(n1.idx(), n0.idx()));
    }
    else if(!n0c && !n1c && n0 == n1)
    {
        // It is useless to use an unknown node here
        // FIXME: Assume that these unknown node are only used
        // around this vertex.
        if(!b0) n0 = Node();
        if(!b1) n1 = Node();
    }
    else if(n0c && !n1.isValid())
    {
        if(!b1) n1 = n0;
    }
    else if(n1c && !n0.isValid())
    {
        if(!b0) n0 = n1;
    }
}


template < typename _Scalar, int _Dim, int _Chan >
void
VGMesh<_Scalar, _Dim, _Chan>::finalize()
{
    assert(hasToVertexValue());

    std::vector<Halfedge> consEdges;
    consEdges.reserve(12);

    for(VertexIterator vit = verticesBegin();
        vit != verticesEnd(); ++vit)
    {
        consEdges.clear();
        findConstrainedEdgesSimplify(*vit, consEdges);

        if(consEdges.empty())
            consEdges.push_back(halfedge(*vit));

        Halfedge prev = consEdges.back();
        std::vector<Halfedge>::iterator cit = consEdges.begin();
        for(; cit != consEdges.end(); ++cit)
        {
            if(!isBoundary(prev))
                finalizeVertexArc(prev, *cit);

            prev = *cit;
        }
    }

    if(hasEdgeValue() || hasEdgeGradient())
    {
        for(EdgeIterator eit = edgesBegin();
            eit != edgesEnd(); ++eit)
        {
            finalizeEdge(*eit);
        }
    }
}


template < typename _Scalar, int _Dim, int _Chan >
void
VGMesh<_Scalar, _Dim, _Chan>::finalizeVertexArc(Halfedge from, Halfedge to)
{
    Node n0 = halfedgeNode(from, FROM_VERTEX_VALUE);
    Node n1 = halfedgeOppositeNode(to, FROM_VERTEX_VALUE);

    bool n0c = n0.isValid() && isConstraint(n0);
    bool n1c = n1.isValid() && isConstraint(n1);

    Node n = Node();
    if(!n0c && !n1c)
    {
        // Free nodes, choose one valid or create a new one.
        if     (n0.isValid()) n = n0;
        else if(n1.isValid()) n = n1;
        else                  n = addNode();
    }
    else if(n0c != n1c)
        n = n0c? n0: n1;  // One constraint, choose it
    else if(n0 == n1 || value(n0) == value(n1))
        n = n0;  // Same constraints, choose one arbitrarily

    // The remaining option is a singularity, that require special
    // processing.
    if(isValid(n))
        setVertexNodes(n, from, to);
    else
        setSingularity(n0, n1, from, to);
}


template < typename _Scalar, int _Dim, int _Chan >
void
VGMesh<_Scalar, _Dim, _Chan>::finalizeEdge(Edge e)
{
    Halfedge h0 = halfedge(e, 0);
    Halfedge h1 = halfedge(e, 1);
    if(hasEdgeValue())
    {
        Node& n0 = edgeValueNode(h0);
        Node& n1 = edgeValueNode(h1);
        bool n0v = n0.isValid();
        bool n1v = n1.isValid();

        if(!n0v && !isBoundary(h0)) n0 = n1v? n1: addNode();
        if(!n1v && !isBoundary(h1)) n1 = n0.isValid()? n0: addNode();
    }
    if(hasEdgeGradient())
    {
        Node& n0 = edgeGradientNode(h0);
        Node& n1 = edgeGradientNode(h1);
        bool n0v = n0.isValid();
        bool n1v = n1.isValid();

        if(!n0v && !isBoundary(h0)) n0 = n1v? n1: addNode();
        if(!n1v && !isBoundary(h1)) n1 = n0.isValid()? n0: addNode();
    }
}


template < typename _Scalar, int _Dim, int _Chan >
bool
VGMesh<_Scalar, _Dim, _Chan>::isSingular(Halfedge h) const
{
    return hasFromVertexValue() &&
            toVertexValueNode(h) != fromVertexValueNode(nextHalfedge(h));
}


template < typename _Scalar, int _Dim, int _Chan >
unsigned
VGMesh<_Scalar, _Dim, _Chan>::nSingulars(Face f) const
{
    unsigned nSingulars = 0;
    HalfedgeAroundFaceCirculator
            hit  = halfedges(f),
            hend = hit;
    do
    {
        nSingulars += isSingular(*hit);
    }
    while(++hit != hend);

    return nSingulars;
}


template < typename _Scalar, int _Dim, int _Chan >
unsigned
VGMesh<_Scalar, _Dim, _Chan>::nSingularFaces() const
{
    unsigned n = 0;
    for(FaceIterator fit = facesBegin();
        fit != facesEnd(); ++fit)
    {
        if(nSingulars(*fit))
            ++n;
    }
    return n;
}


template < typename _Scalar, int _Dim, int _Chan >
void
VGMesh<_Scalar, _Dim, _Chan>::copyVGMeshMembers(const Self& rhs)
{
    m_attributes = rhs.m_attributes;

    m_nprops = rhs.m_nprops;

    m_positions = rhs.m_positions;
    m_vertexGradientConstraints = rhs.m_vertexGradientConstraints;
    m_curvedEdges = rhs.m_curvedEdges;

    for(int ai = 0; ai < HALFEDGE_ATTRIB_COUNT; ++ai) {
        m_halfedgeAttributes[ai] = getHalfedgeProperty<Node>(_halfedgeAttrName[ai]);
    }

    m_deletedNodes = rhs.m_deletedNodes;
    m_nodes = rhs.m_nodes/*.template cast<Scalar>()*/;
    m_ndeleted = nodeProperty<bool>("n:deleted");

    assert(verticesSize() <= positionsCapacity());
    assert(nodesSize() <= nodesCapacity());
}


template < typename _Scalar, int _Dim, int _Chan >
void
VGMesh<_Scalar, _Dim, _Chan>::
    findConstrainedEdgesSimplify(Vertex vx,
                                 std::vector<Halfedge>& consEdges)
{
    HalfedgeAroundVertexCirculator hit = halfedges(vx),
                                   hEnd = hit;
    do
    {
        Node& np = halfedgeOppositeNode(*hit, FROM_VERTEX_VALUE);
        Node& n0 = halfedgeNode(*hit, FROM_VERTEX_VALUE);
        bool boundary = isBoundary(edge(*hit));
        if(np.isValid() || n0.isValid() || boundary)
        {
            simplifyOppositeNodes(np, n0, isBoundary(oppositeHalfedge(*hit)),
                                  isBoundary(*hit));

            if(np.isValid() || n0.isValid() || boundary)
            {
                consEdges.push_back(*hit);
            }
        }
        ++hit;
    }
    while(hit != hEnd);
}


template < typename _Scalar, int _Dim, int _Chan >
void
VGMesh<_Scalar, _Dim, _Chan>::resizePositionsMatrix(unsigned rows, unsigned cols)
{
    VectorMatrix positions(rows, cols);
    unsigned minRows = std::min(rows, unsigned(m_positions.rows()));
    unsigned minCols = std::min(cols, unsigned(m_positions.cols()));
    positions.block(0, 0, minRows, minCols) = m_positions.block(0, 0, minRows, minCols);
    m_positions.swap(positions);
}


template < typename _Scalar, int _Dim, int _Chan >
void
VGMesh<_Scalar, _Dim, _Chan>::resizeNodesMatrix(unsigned rows, unsigned cols)
{
    NodeMatrix nodes(rows, cols);
    unsigned minRows = std::min(rows, unsigned(m_nodes.rows()));
    unsigned minCols = std::min(cols, unsigned(m_nodes.cols()));
    nodes.block(0, 0, minRows, minCols) = m_nodes;
    m_nodes.swap(nodes);
}


template < typename _Scalar, int _Dim, int _Chan >
const char*
VGMesh<_Scalar, _Dim, _Chan>::_halfedgeAttrName[] = {
    "h:toVertexValueNode",
    "h:fromVertexValueNode",
    "h:edgeValueNode",
    "h:edgeGradientNode"
};



}  // namespace Vitelotte
