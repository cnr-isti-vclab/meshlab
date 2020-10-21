/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _VITELOTTE_DC_MESH_
#define _VITELOTTE_DC_MESH_


#include <vector>

#include <Eigen/StdVector>

#include "../Core/vgMesh.h"


namespace Vitelotte
{


/**
 * \brief A 1D piecewise linear function.
 */
template <typename _Value>
class PiecewiseLinearFunction
{
public:
    typedef _Value Value;

private:
    typedef std::pair<float, Value> FloatValuePair;
    typedef std::map<float, Value, std::less<float>, Eigen::aligned_allocator<FloatValuePair> > Samples;

public:
    typedef typename Samples::iterator Iterator;
    typedef typename Samples::const_iterator ConstIterator;

public:
    PiecewiseLinearFunction() {}

    bool     empty() const { return m_samples.empty(); }
    unsigned size()  const { return m_samples.size(); }

    bool         has   (float x) const { return m_samples.count(x); }
          Value& sample(float x)       { return m_samples.at(x); }
    const Value& sample(float x) const { return m_samples.at(x); }

    void add(float x, const Value& v) { m_samples.insert(std::make_pair(x, v)); }
    void remove(float x) { m_samples.erase(x); }
    void clear() { m_samples.clear(); }

    Iterator      begin()       { return m_samples.begin(); }
    ConstIterator begin() const { return m_samples.begin(); }
    Iterator      end()         { return m_samples.end(); }
    ConstIterator end()   const { return m_samples.end(); }

    Value operator()(float x) const;

private:
    Samples m_samples;
};


/**
 * \brief A VGMesh with diffusion curves.
 */
template < typename _Scalar, int _Dims=2, int _Coeffs=4 >
class DCMesh : public Vitelotte::VGMesh<_Scalar, _Dims, _Coeffs>
{
public:
    typedef Vitelotte::VGMesh<_Scalar, _Dims, _Coeffs> Base;

    typedef typename Base::Scalar   Scalar;
    typedef typename Base::Vector   Vector;
    typedef typename Base::Value    Value;
    typedef typename Base::Gradient Gradient;

    typedef typename Base::CurvedEdge CurvedEdge;

    typedef typename Base::BaseHandle BaseHandle;
    typedef typename Base::Vertex     Vertex;
    typedef typename Base::Halfedge   Halfedge;
    typedef typename Base::Edge       Edge;
    typedef typename Base::Face       Face;
    typedef typename Base::Node       Node;

    typedef typename Base::VertexIterator     VertexIterator;
    typedef typename Base::HalfedgeIterator   HalfedgeIterator;
    typedef typename Base::HalfedgeAroundVertexCirculator
                                              HalfedgeAroundVertexCirculator;

    using Base::verticesBegin;
    using Base::verticesEnd;
    using Base::halfedges;
    using Base::halfedgesBegin;
    using Base::halfedgesEnd;
    using Base::setEdgeCurve;
    using Base::oppositeHalfedge;
    using Base::isBoundary;

    using Base::nCoeffs;
    using Base::nDims;
    using Base::unconstrainedValue;
    using Base::hasFromVertexValue;
    using Base::hasToVertexValue;
    using Base::hasEdgeValue;
    using Base::hasEdgeGradient;
    using Base::fromVertexValueNode;
    using Base::toVertexValueNode;
    using Base::edgeValueNode;
    using Base::edgeGradientNode;
    using Base::halfedgeNode;
    using Base::halfedgeOppositeNode;
    using Base::halfedgeOrientation;
    using Base::addNode;
    using Base::hasVertexGradientConstraint;
    using Base::setGradientConstraint;
    using Base::removeGradientConstraint;

    typedef PiecewiseLinearFunction<Value> ValueFunction;

    typedef Vitelotte::BezierPath<Vector> BezierPath;

    typedef typename Gradient::ConstantReturnType UnconstrainedGradientType;

    struct Curve : public BaseHandle
    {
        using BaseHandle::idx;
        explicit Curve(int _idx = -1) : BaseHandle(_idx) {}
        std::ostream& operator<<(std::ostream& os) const { return os << 'c' << idx(); }
    };

    struct PointConstraint : public BaseHandle
    {
        using BaseHandle::idx;
        explicit PointConstraint(int _idx = -1) : BaseHandle(_idx) {}
        std::ostream& operator<<(std::ostream& os) const { return os << "pc" << idx(); }
    };

    struct HalfedgeCurveConnectivity
    {
        Curve     curve;
        Halfedge  next;
        float     pos;
    };

    struct CurveInfo
    {
        Halfedge       firstHalfedge;
        Halfedge       lastHalfedge;
        unsigned       flags;
        ValueFunction  gradient[4];
        BezierPath     bezierPath;
    };

    struct PointConstraintInfo
    {
        Vertex    vertex;
        Value     value;
        Gradient  gradient;
    };

    enum
    {
        VALUE_TEAR      = 0x01,
        GRADIENT_TEAR   = 0x02
    };

    enum
    {
        LEFT        = 0x00,
        RIGHT       = 0x01,
        VALUE       = 0x00,
        GRADIENT    = 0x02,

        VALUE_LEFT      = VALUE     | LEFT,
        VALUE_RIGHT     = VALUE     | RIGHT,
        GRADIENT_LEFT   = GRADIENT  | LEFT,
        GRADIENT_RIGHT  = GRADIENT  | RIGHT
    };

public:
    DCMesh();
    DCMesh(unsigned nDims, unsigned nCoeffs);
    DCMesh(const DCMesh& other);

    DCMesh& operator=(const DCMesh& other);
    DCMesh& assign(const DCMesh& other);

    inline Curve  curve(Halfedge h) const { return m_halfedgeCurveConn[h].curve; }
    inline Curve& curve(Halfedge h)       { return m_halfedgeCurveConn[h].curve; }

    inline float  fromCurvePos(Halfedge h) const
        { return m_halfedgeCurveConn[oppositeHalfedge(h)].pos; }
    inline float& fromCurvePos(Halfedge h)
        { return m_halfedgeCurveConn[oppositeHalfedge(h)].pos; }
    inline float  toCurvePos(Halfedge h) const { return m_halfedgeCurveConn[h].pos; }
    inline float& toCurvePos(Halfedge h)       { return m_halfedgeCurveConn[h].pos; }

    inline Halfedge  nextCurveHalfedge(Halfedge h) const { return m_halfedgeCurveConn[h].next; }
    inline Halfedge& nextCurveHalfedge(Halfedge h)       { return m_halfedgeCurveConn[h].next; }
    inline Halfedge  prevCurveHalfedge(Halfedge h) const
        { return m_halfedgeCurveConn[oppositeHalfedge(h)].next; }
    inline Halfedge& prevCurveHalfedge(Halfedge h)
        { return m_halfedgeCurveConn[oppositeHalfedge(h)].next; }

    inline unsigned nCurves() const { return m_curves.size(); }
    Curve addCurve(unsigned flags);

    using Base::isValid;
    inline bool isValid(PointConstraint pc) const
        { return pc.isValid() && unsigned(pc.idx()) < nPointConstraints(); }
    inline bool isValid(Curve c) const { return c.isValid() && unsigned(c.idx()) < nCurves(); }

    void addHalfedgeToCurve(Curve c, Halfedge h, float from, float to);

    inline Halfedge  firstHalfedge(Curve c) const { return m_curves.at(c.idx()).firstHalfedge; }
    inline Halfedge& firstHalfedge(Curve c)       { return m_curves.at(c.idx()).firstHalfedge; }
    inline Halfedge   lastHalfedge(Curve c) const { return m_curves.at(c.idx()). lastHalfedge; }
    inline Halfedge&  lastHalfedge(Curve c)       { return m_curves.at(c.idx()). lastHalfedge; }

    inline bool valueTear(Curve c)    const { return m_curves.at(c.idx()).flags & VALUE_TEAR;    }
    inline bool gradientTear(Curve c) const { return m_curves.at(c.idx()).flags & GRADIENT_TEAR; }

    inline unsigned flags(Curve c) const { return m_curves.at(c.idx()).flags; }
    void setFlags(Curve c, unsigned flags);
    void setFlagsRaw(Curve c, unsigned flags);

    const ValueFunction& valueFunction(Curve c, unsigned which) const;
          ValueFunction& valueFunction(Curve c, unsigned which);
    const ValueFunction& valueFunctionRaw(Curve c, unsigned which) const;
          ValueFunction& valueFunctionRaw(Curve c, unsigned which);

    const BezierPath& bezierPath(Curve c) const { return m_curves.at(c.idx()).bezierPath; }
          BezierPath& bezierPath(Curve c)       { return m_curves.at(c.idx()).bezierPath; }

    using Base::vertex;
    inline Vertex vertex(PointConstraint pc) const { return m_pointConstraints[pc.idx()].vertex; }
    inline void setVertex(PointConstraint pc, Vertex vx);

    inline PointConstraint pointConstraint(Vertex vx) { return m_pointConstraintConn[vx]; }

    inline bool isValueConstraint(PointConstraint pc) const
        { return !isnan(m_pointConstraints[pc.idx()].value(0)); }
    using Base::isGradientConstraint;
    inline bool isGradientConstraint(PointConstraint pc) const
        { return !isnan(m_pointConstraints[pc.idx()].gradient(0, 0)); }

    using Base::value;
    inline const Value& value(PointConstraint pc) const
        { return m_pointConstraints[pc.idx()].value; }
    inline       Value& value(PointConstraint pc)
        { return m_pointConstraints[pc.idx()].value; }

    inline const Gradient& gradient(PointConstraint pc) const
        { return m_pointConstraints[pc.idx()].gradient; }
    inline       Gradient& gradient(PointConstraint pc)
        { return m_pointConstraints[pc.idx()].gradient; }

    inline unsigned nPointConstraints() const { return m_pointConstraints.size(); }
    PointConstraint addPointConstraint(Vertex vx);


    void clear();
    void setNodesFromCurves();

    inline UnconstrainedGradientType unconstrainedGradientValue() const
    { return Gradient::Constant(nCoeffs(), nDims(), std::numeric_limits<Scalar>::quiet_NaN()); }

    Value evalValueFunction(Curve c, unsigned which, float pos) const;


protected:
    using Base::m_nprops;

protected:
    void copyVGMeshWithCurvesMembers(const DCMesh& other);

    typedef Node NodePair[2];
    void addGradientNodes(Node nodes[2], Curve c, unsigned gType, float pos);

protected:
    typename Base::template VertexProperty<PointConstraint> m_pointConstraintConn;
    typename Base::template HalfedgeProperty<HalfedgeCurveConnectivity> m_halfedgeCurveConn;

    std::vector<PointConstraintInfo, Eigen::aligned_allocator<PointConstraintInfo> > m_pointConstraints;
    std::vector<CurveInfo> m_curves;
};

}

#include "dcMesh.hpp"


#endif
