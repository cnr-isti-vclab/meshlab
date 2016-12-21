/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _VITELOTTE_BEZIER_PATH_
#define _VITELOTTE_BEZIER_PATH_


#include <cassert>
#include <vector>

#include <Eigen/Geometry>


namespace Vitelotte
{


enum BezierSegmentType
{
    BEZIER_EMPTY     = 0,
    BEZIER_LINEAR    = 2,
    BEZIER_QUADRATIC = 3,
    BEZIER_CUBIC     = 4
};


template < typename _Vector >
class BezierSegment {
public:
    typedef typename _Vector::Scalar Scalar;
    typedef _Vector Vector;

    typedef BezierSegment<Vector> Self;

public:
    inline BezierSegment()
        : m_type(BEZIER_EMPTY) {}
    inline BezierSegment(BezierSegmentType type, const Vector* points)
        : m_type(type) {
        std::copy(points, points + type, m_points);
    }

    inline BezierSegmentType type() const { return m_type; }
    inline void setType(BezierSegmentType type) { m_type = type; }

    inline const Vector& point(unsigned i) const {
        assert(i < m_type);
        return m_points[i];
    }
    inline Vector& point(unsigned i) {
        assert(i < m_type);
        return m_points[i];
    }

    inline Self getBackward() const {
        Self seg;
        seg.setType(type());
        for(unsigned i = 0; i < type(); ++i) {
            seg.point(type() - 1 - i) = point(i);
        }
        return seg;
    }

    void split(Scalar pos, Self& head, Self& tail) {
        assert(m_type != BEZIER_EMPTY);

        // All the points of de Casteljau Algorithm
        Vector pts[10];
        Vector* levels[] = {
            &pts[9],
            &pts[7],
            &pts[4],
            pts
        };

        // Initialize the current level.
        std::copy(m_points, m_points + type(), levels[type() - 1]);

        // Compute all the points
        for(int level = type()-1; level >= 0; --level) {
            for(int i = 0; i < level; ++i) {
                levels[level-1][i] = (Scalar(1) - pos) * levels[level][i]
                                   + pos               * levels[level][i+1];
            }
        }

        // Set the segments
        head.setType(type());
        tail.setType(type());

        const unsigned last = type() - 1;
        for(unsigned i = 0; i < type(); ++i) {
            head.point(i)  = levels[last - i][0];
            tail.point(i) = levels[i][i];
        }
    }

    template < typename InIt >
    void refineUniform(InIt out, unsigned nSplit) {
        Self head;
        Self tail = *this;
        *(out++) = std::make_pair(Scalar(0), point(0));
        for(unsigned i = 0; i < nSplit; ++i) {
            Scalar x = Scalar(i+1) / Scalar(nSplit + 1);
            tail.split(x, head, tail);
            *(out++) = std::make_pair(x, tail.point(0));
        }
        *(out++) = std::make_pair(Scalar(1), point(type()-1));
    }

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
    BezierSegmentType m_type;
    Vector m_points[4];
};


template < typename _Vector >
class BezierPath
{
public:
    typedef _Vector Vector;

public:
    static unsigned size(BezierSegmentType type) { return unsigned(type); }

public:
    inline BezierPath() {}

    inline unsigned nPoints()   const { return m_points.size(); }
    inline unsigned nSegments() const { return m_segments.size(); }

    inline BezierSegmentType type(unsigned si) const { return m_segments.at(si).type; }
    inline unsigned nPoints(unsigned si) const { return size(type(si)); }

    inline const Vector& point(unsigned pi) const { return m_points.at(pi); }
    inline       Vector& point(unsigned pi)       { return m_points.at(pi); }
    inline const Vector& point(unsigned si, unsigned pi) const
    {
        assert(si < nSegments() && pi < nPoints(si));
        return m_points.at(m_segments[si].firstPoint + pi);
    }
    inline       Vector& point(unsigned si, unsigned pi)
    {
        assert(si < nSegments() && pi < nPoints(si));
        return m_points.at(m_segments[si].firstPoint + pi);
    }

    inline void setFirstPoint(const Vector& point)
    {
        assert(nPoints() == 0);
        m_points.push_back(point);
    }

    unsigned addSegment(BezierSegmentType type, const Vector* points)
    {
        assert(nPoints() != 0);
        unsigned si = nSegments();
        Segment s;
        s.type       = type;
        s.firstPoint = nPoints() - 1;
        m_segments.push_back(s);

        for(unsigned i = 0; i < nPoints(si) - 1; ++i)
        {
            m_points.push_back(points[i]);
        }

        return si;
    }

    BezierSegment<Vector> getSegment(unsigned si) const {
        assert(si < nSegments());
        return BezierSegment<Vector>(type(si), &m_points[m_segments[si].firstPoint]);
    }

private:
    struct Segment {
        BezierSegmentType type;
        unsigned    firstPoint;
    };

    typedef std::vector<Vector>  PointList;
    typedef std::vector<Segment> SegmentList;

private:
    PointList   m_points;
    SegmentList m_segments;
};


}

#endif
