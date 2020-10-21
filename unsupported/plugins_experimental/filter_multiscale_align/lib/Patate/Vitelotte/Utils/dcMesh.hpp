/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "dcMesh.h"


namespace Vitelotte
{


template <typename _Value>
typename PiecewiseLinearFunction<_Value>::Value
    PiecewiseLinearFunction<_Value>::operator()(float x) const
{
    assert(!empty());

    if(x <= m_samples.begin()->first)
        return m_samples.begin()->second;
    if(x >= m_samples.rbegin()->first)
        return m_samples.rbegin()->second;

    ConstIterator next = m_samples.upper_bound(x);
    ConstIterator prev = next;
    --next;

    float alpha = (x - prev->first) / (next->first - prev->first);
    return (1 - alpha) * prev->second + alpha * next->second;
}


template < typename _Scalar, int _Dim, int _Chan >
DCMesh<_Scalar, _Dim, _Chan>::DCMesh()
{
    m_pointConstraintConn = Base::template addVertexProperty<PointConstraint>(
                                                "v:pointConstraintConnectivity");
    m_halfedgeCurveConn = Base::template addHalfedgeProperty<HalfedgeCurveConnectivity>(
                                                "h:curveConnectivity");
}


template < typename _Scalar, int _Dim, int _Chan >
DCMesh<_Scalar, _Dim, _Chan>::DCMesh(unsigned nDims, unsigned nCoeffs)
    : Base(nDims, nCoeffs, 0)
{
    m_pointConstraintConn = Base::template addVertexProperty<PointConstraint>(
                                                "v:pointConstraintConnectivity");
    m_halfedgeCurveConn = Base::template addHalfedgeProperty<HalfedgeCurveConnectivity>(
                                                "h:curveConnectivity");
}


template < typename _Scalar, int _Dim, int _Chan >
DCMesh<_Scalar, _Dim, _Chan>::DCMesh(const DCMesh& other)
    : Base(other)
{
    copyVGMeshWithCurvesMembers(other);
}


template < typename _Scalar, int _Dim, int _Chan >
DCMesh<_Scalar, _Dim, _Chan>&
DCMesh<_Scalar, _Dim, _Chan>::operator=(const DCMesh& other)
{
    if(&other == this) return *this;

    Base::operator=(other);
    copyVGMeshWithCurvesMembers(other);

    return *this;
}


template < typename _Scalar, int _Dim, int _Chan >
DCMesh<_Scalar, _Dim, _Chan>&
DCMesh<_Scalar, _Dim, _Chan>::assign(const DCMesh& other)
{
    if(&other == this) return *this;

    Base::assign(other);
    copyVGMeshWithCurvesMembers(other);

    return *this;
}


template < typename _Scalar, int _Dim, int _Chan >
typename DCMesh<_Scalar, _Dim, _Chan>::Curve
DCMesh<_Scalar, _Dim, _Chan>::addCurve(unsigned flags)
{
    Curve c(nCurves());
    CurveInfo ci;
    ci.flags = flags;
    m_curves.push_back(ci);
    return c;
}


template < typename _Scalar, int _Dim, int _Chan >
void
DCMesh<_Scalar, _Dim, _Chan>::addHalfedgeToCurve(Curve c, Halfedge h, float from, float to)
{
    assert(isValid(c));
    assert(isValid(h));

    CurveInfo& ci = m_curves[c.idx()];
    HalfedgeCurveConnectivity& hcc = m_halfedgeCurveConn[h];
    HalfedgeCurveConnectivity& ohcc = m_halfedgeCurveConn[oppositeHalfedge(h)];

    assert(!hcc.curve.isValid());
    assert(!ohcc.curve.isValid());

    hcc.curve = c;
    hcc.pos = to;
    ohcc.curve = c;
    ohcc.pos = from;
    if(ci.firstHalfedge.isValid())
    {
        assert(ci.lastHalfedge.isValid());
        HalfedgeCurveConnectivity& phcc = m_halfedgeCurveConn[ci.lastHalfedge];

        phcc.next = h;
        ohcc.next = ci.lastHalfedge;
        ci.lastHalfedge = h;
    }
    else
    {
        ci.firstHalfedge = h;
        ci.lastHalfedge = h;
    }
}


template < typename _Scalar, int _Dim, int _Chan >
void
DCMesh<_Scalar, _Dim, _Chan>::setFlags(Curve c, unsigned flags)
{
    assert(isValid(c));

    CurveInfo& ci = m_curves[c.idx()];
    if(flags & VALUE_TEAR && !(ci.flags & VALUE_TEAR))
        ci.gradient[VALUE_RIGHT] = ci.gradient[VALUE_LEFT];
    if(flags & GRADIENT_TEAR && !(ci.flags & GRADIENT_TEAR))
        ci.gradient[GRADIENT_RIGHT] = ci.gradient[GRADIENT_LEFT];
    ci.flags = flags;
}


template < typename _Scalar, int _Dim, int _Chan >
void
DCMesh<_Scalar, _Dim, _Chan>::setFlagsRaw(Curve c, unsigned flags)
{
    assert(isValid(c));

    CurveInfo& ci = m_curves[c.idx()];
    ci.flags = flags;
}


template < typename _Scalar, int _Dim, int _Chan >
const typename DCMesh<_Scalar, _Dim, _Chan>::ValueFunction&
DCMesh<_Scalar, _Dim, _Chan>::valueFunction(Curve c, unsigned which) const
{
    return const_cast<DCMesh*>(this)->valueFunction(c, which);
}


template < typename _Scalar, int _Dim, int _Chan >
typename DCMesh<_Scalar, _Dim, _Chan>::ValueFunction&
DCMesh<_Scalar, _Dim, _Chan>::valueFunction(Curve c, unsigned which)
{
    assert(isValid(c));
    assert(which < 4);

    switch(which)
    {
    case VALUE_RIGHT:
        if(!valueTear(c))
            which = VALUE_LEFT;
        break;
    case GRADIENT_RIGHT:
        if(!gradientTear(c))
            which = GRADIENT_LEFT;
        break;
    }

    return m_curves[c.idx()].gradient[which];
}


template < typename _Scalar, int _Dim, int _Chan >
const typename DCMesh<_Scalar, _Dim, _Chan>::ValueFunction&
DCMesh<_Scalar, _Dim, _Chan>::valueFunctionRaw(Curve c, unsigned which) const
{
    return const_cast<DCMesh*>(this)->valueFunctionRaw(c, which);
}


template < typename _Scalar, int _Dim, int _Chan >
typename DCMesh<_Scalar, _Dim, _Chan>::ValueFunction&
DCMesh<_Scalar, _Dim, _Chan>::valueFunctionRaw(Curve c, unsigned which)
{
    assert(isValid(c));
    assert(which < 4);

    return m_curves[c.idx()].gradient[which];
}


template < typename _Scalar, int _Dim, int _Chan >
void
DCMesh<_Scalar, _Dim, _Chan>::setVertex(PointConstraint pc, Vertex vx) {
    assert(!m_pointConstraintConn[vx].isValid());
    if(vertex(pc).isValid()) {
        m_pointConstraintConn[vertex(pc)] = PointConstraint();
    }
    m_pointConstraints[pc.idx()].vertex = vx;
    m_pointConstraintConn[vx] = pc;
}


template < typename _Scalar, int _Dim, int _Chan >
typename DCMesh<_Scalar, _Dim, _Chan>::PointConstraint
DCMesh<_Scalar, _Dim, _Chan>::addPointConstraint(Vertex vx)
{
    PointConstraint pc(nPointConstraints());
    PointConstraintInfo pci;
    pci.value = unconstrainedValue();
    pci.gradient = unconstrainedGradientValue();
    m_pointConstraints.push_back(pci);
    setVertex(pc, vx);
    return pc;
}


template < typename _Scalar, int _Dim, int _Chan >
void
DCMesh<_Scalar, _Dim, _Chan>::clear()
{
    Base::clear();
    m_pointConstraints.clear();
    m_curves.clear();
}


template < typename _Scalar, int _Dim, int _Chan >
void
DCMesh<_Scalar, _Dim, _Chan>::setNodesFromCurves()
{
    m_nprops.resize(0);
    for(HalfedgeIterator hit = halfedgesBegin();
        hit != halfedgesEnd(); ++hit)
    {
        if(hasFromVertexValue())    fromVertexValueNode(*hit)   = Node();
        if(hasToVertexValue())      toVertexValueNode(*hit)     = Node();
        if(hasEdgeValue())          edgeValueNode(*hit)         = Node();
        if(hasEdgeGradient())       edgeGradientNode(*hit)      = Node();
    }
    if(hasVertexGradientConstraint()) {
        for(VertexIterator vit = verticesBegin();
            vit != verticesEnd(); ++vit)
        {
            if(isGradientConstraint(*vit))
            {
                removeGradientConstraint(*vit);
            }
        }
    }

    for(unsigned pci = 0; pci < nPointConstraints(); ++pci)
    {
        PointConstraint pc(pci);
        Vertex vx = vertex(pc);
        assert(isValid(vx));

        Node vn;
        if(isValueConstraint(pc))
            vn = addNode(value(pc));

        HalfedgeAroundVertexCirculator hit = halfedges(vx);
        HalfedgeAroundVertexCirculator hend = hit;
        do {
            Halfedge opp = oppositeHalfedge(*hit);
            if(isValueConstraint(pc))
            {
                if(!isBoundary(*hit))
                    halfedgeNode(*hit, Base::FROM_VERTEX_VALUE) = vn;
                if(!isBoundary(opp))
                    halfedgeOppositeNode(*hit, Base::FROM_VERTEX_VALUE) = vn;
            }
            ++hit;
        } while(hit != hend);

        if(hasVertexGradientConstraint() && isGradientConstraint(pc))
        {
            setGradientConstraint(vertex(pc), gradient(pc));
        }
    }

    for(unsigned ci = 0; ci < nCurves(); ++ci)
    {
        Curve c(ci);

        Halfedge lh = firstHalfedge(c);
        if(!lh.isValid())
            continue;

        const BezierPath& path = bezierPath(c);
        unsigned si = 0;
        Scalar splitPos = 0;
        CurvedEdge head;
        CurvedEdge tail = path.nSegments()? path.getSegment(si): CurvedEdge();

        Node fromNode[2];
        addGradientNodes(fromNode, c, VALUE, fromCurvePos(lh));
        do {
            Node toNode[2];
            addGradientNodes(toNode, c, VALUE, toCurvePos(lh));

            Halfedge rh = oppositeHalfedge(lh);
            float midPos = (fromCurvePos(lh) + toCurvePos(lh)) / 2.f;

            bool lhnb = !isBoundary(lh);
            bool rhnb = !isBoundary(rh);

            if(hasFromVertexValue())
            {
                if(lhnb) fromVertexValueNode(lh) = fromNode[LEFT];
                if(rhnb) fromVertexValueNode(rh) =   toNode[RIGHT];
            }
            if(hasToVertexValue())
            {
                if(lhnb) toVertexValueNode(lh) =   toNode[LEFT];
                if(rhnb) toVertexValueNode(rh) = fromNode[RIGHT];
            }

            if(hasEdgeValue())
            {
                Node midNode[2];
                addGradientNodes(midNode, c, VALUE, midPos);
                if(lhnb) edgeValueNode(lh) = midNode[LEFT];
                if(rhnb) edgeValueNode(rh) = midNode[RIGHT];
            }

            if(hasEdgeGradient())
            {
                Node gNode[2];
                addGradientNodes(gNode, c, GRADIENT, midPos);
                if(lhnb) edgeGradientNode(lh) = gNode[LEFT];
                if(rhnb) edgeGradientNode(rh) = gNode[RIGHT];

                if(halfedgeOrientation(lh) && lhnb)
                    value(gNode[LEFT]) *= -1;
                if(gNode[LEFT] != gNode[RIGHT] && halfedgeOrientation(lh) && rhnb)
                    value(gNode[RIGHT]) *= -1;

            }

            if(tail.type() != BEZIER_EMPTY) {
                Scalar csi;
                Scalar curvePos = std::modf(toCurvePos(lh) * path.nSegments() + 1.e-5, &csi);

                if(unsigned(csi) == si) {
                    Scalar pos = (curvePos - splitPos) / (1 - splitPos);
                    tail.split(pos, head, tail);
                    setEdgeCurve(lh, head);
                    splitPos = curvePos;
                } else {
                    setEdgeCurve(lh, tail);
                    ++si;
                    tail = (si < path.nSegments())?
                                path.getSegment(si):
                                CurvedEdge();
                    splitPos = 0;
                }
            }


            fromNode[0] = toNode[0];
            fromNode[1] = toNode[1];
            lh = nextCurveHalfedge(lh);
        } while(lh.isValid());
    }
}


template < typename _Scalar, int _Dim, int _Chan >
typename DCMesh<_Scalar, _Dim, _Chan>::Value
DCMesh<_Scalar, _Dim, _Chan>::evalValueFunction(
        Curve c, unsigned which, float pos) const
{
    return valueFunction(c, which)(pos);
}


template < typename _Scalar, int _Dim, int _Chan >
void
DCMesh<_Scalar, _Dim, _Chan>::copyVGMeshWithCurvesMembers(const DCMesh& other)
{
    m_pointConstraintConn = Base::template vertexProperty<PointConstraint>(
                                                "v:pointConstraintConnectivity");
    m_halfedgeCurveConn   = Base::template halfedgeProperty<HalfedgeCurveConnectivity>(
                                                "h:curveConnectivity");

    m_pointConstraints = other.m_pointConstraints;
    m_curves = other.m_curves;
}


template < typename _Scalar, int _Dim, int _Chan >
void
DCMesh<_Scalar, _Dim, _Chan>::addGradientNodes(
        Node nodes[2], Curve c, unsigned gType, float pos)
{
    bool tear = (gType == VALUE)? valueTear(c): gradientTear(c);

    nodes[LEFT] = valueFunction(c, gType | LEFT).empty()?
                addNode():
                addNode(evalValueFunction(c, gType | LEFT, pos));
    nodes[RIGHT] =
            (!tear)?
                nodes[LEFT]:
                valueFunction(c, gType | RIGHT).empty()?
                            addNode():
                            addNode(evalValueFunction(c, gType | RIGHT, pos));
}


}
