#ifndef INTERCEPT_H
#define INTERCEPT_H

#include <algorithm>
#include <vector>
#include <vcg/complex/trimesh/base.h>
#include <vcg/space/box2.h>

template <typename scalar>
        class Intercept
{
    typedef vcg::Point3<scalar> Point3x;

public:
    inline Intercept (scalar dist, const Point3x &norm, scalar quality) : norm(norm), dist(dist), quality(quality) {};

    inline Intercept operator -() const { return Intercept(dist, -norm, quality); }

    inline bool operator <(const Intercept &other) const { return dist < other.dist; }

private:
    Point3x norm;
    scalar dist, quality;
};

template <typename scalar>
        class InterceptRay
{
    typedef Intercept<scalar> InterceptType;
    typedef std::vector<InterceptType> ContainerType;

public:
    inline InterceptRay(const ContainerType &set) : v(set) {
        std::sort(v.begin(), v.end());
        //TODO: cleanup
    }

    inline InterceptRay & operator &=(const InterceptRay & other) {
        ContainerType newv(v.size() + other.v.size());
        typename ContainerType::const_iterator i = v.begin(), j = other.v.begin(), endi = v.end(), endj = v.end();
        while (i != endi && j != endj) {
            if (*i < *j) {
                if (*j < *(i+1)) {
                    while (j != endj && *(j+1) < *(i+1)) {
                        newv.push_back(*j);
                        newv.push_back(*(j+1));
                        j += 2;
                    }
                    if (j != endj && *j < *(i+1)) {
                        newv.push_back(*j);
                        newv.push_back(*(i+1));
                    }
                }
                i += 2;
            } else {
                if (*i < *(j+1)) {
                    while (i != endi && *(i+1) < *(j+1)) {
                        newv.push_back(*i);
                        newv.push_back(*(i+1));
                        i += 2;
                    }
                    if (i != endi && *i < *(j+1)) {
                        newv.push_back(*i);
                        newv.push_back(*(j+1));
                    }
                }
                j += 2;
            }
        }
        this->v = newv;
        return *this;
    }

    inline InterceptRay & operator |=(const InterceptRay & other) {
        return *this;
    }

    inline InterceptRay & operator ^=(const InterceptRay & other) {
        return *this;
    }

private:
    ContainerType v;
};


template <typename scalar>
        class InterceptBeam
{
    typedef Intercept<scalar> InterceptType;

public:
    typedef std::vector<std::vector<InterceptRay<scalar> > > ContainerType;

    inline InterceptBeam(const vcg::Box2i &box, const ContainerType &rays) : bbox(box), ray(rays) {};

    inline InterceptBeam & operator &=(const InterceptBeam & other) {
        return *this;
    }

    inline InterceptBeam & operator |=(const InterceptBeam & other) {
        return *this;
    }

    inline InterceptBeam & operator ^=(const InterceptBeam & other) {
        return *this;
    }

private:
    vcg::Box2i bbox;
    ContainerType ray;
};

template <typename scalar>
        class InterceptVolume
{
    typedef Intercept<scalar> InterceptType;
    typedef vcg::Point3<scalar> Point3x;

    inline bool checkConsistency (const InterceptVolume & other) const {
        return delta == other.delta;
    }

public:
    typedef typename std::vector<InterceptBeam<scalar> > ContainerType;

    inline InterceptVolume(const Point3x &d, const ContainerType &beams) : delta(d), beam(beams) { assert(beams.size() == 3); };

    inline InterceptVolume & operator &=(const InterceptVolume & other) {
        assert(checkConsistency(other));
        for (int i = 0; i < 3; ++i)
            beam[i] &= other.beam[i];
        return *this;
    }

    inline InterceptVolume & operator |=(const InterceptVolume & other) {
        assert(checkConsistency(other));
        for (int i = 0; i < 3; ++i)
            beam[i] |= other.beam[i];
        return *this;
    }

    inline InterceptVolume & operator ^=(const InterceptVolume & other) {
        assert(checkConsistency(other));
        for (int i = 0; i < 3; ++i)
            beam[i] ^= other.beam[i];
        return *this;
    }

    const Point3x delta;
private:
    ContainerType beam;
};


template <typename scalar>
        class InterceptSet
{
    typedef Intercept<scalar> InterceptType;
    typedef std::vector<InterceptType> ContainerType;
    typedef InterceptRay<scalar> SortedType;

public:
    inline void AddIntercept (const InterceptType &x) { v.push_back(x); }
    inline operator SortedType() const { return SortedType(v); }

private:
    ContainerType v;
};

template <typename scalar>
        class InterceptSet1
{
    typedef Intercept<scalar> InterceptType;
    typedef std::vector<InterceptRay<scalar> > SortedType;

public:
    inline InterceptSet1() {};

    inline operator SortedType() const { return SortedType(set.begin(), set.end()); }

    inline void AddIntercept (const int i, const InterceptType &x) {
        set[i].AddIntercept(x);
    }

private:
    std::vector<InterceptSet<scalar> > set;
};

template <typename scalar>
        class InterceptSet2
{
    typedef Intercept<scalar> InterceptType;
    typedef InterceptBeam<scalar> SortedType;

public:
    inline InterceptSet2(const vcg::Box2i box) : bbox(box) { set.reserve(bbox.DimX()); };

    inline operator SortedType() const { return SortedType(bbox, typename SortedType::ContainerType(set.begin(), set.end())); }

    inline void AddIntercept (const vcg::Point2i &p, const InterceptType &x) {
        assert(bbox.IsIn(p));
        vcg::Point2i c = p - bbox.min;
        set[c.X()].AddIntercept(c.Y(), x);
    }

    const vcg::Box2i bbox;
private:
    std::vector<InterceptSet1<scalar> > set;
};

template <typename scalar>
        class InterceptSet3
{
    typedef Intercept<scalar> InterceptType;
    typedef vcg::Point3<scalar> Point3x;
    typedef InterceptVolume<scalar> SortedType;

    template <const int CoordZ>
            void RasterFace(const vcg::Box3i &ibox,
                            scalar dist, const Point3x &norm, scalar quality,
                            const Point3x &v0, const Point3x &v1, const Point3x &v2)
    {
        const scalar EPS = scalar(1e-12);
        const int crd0 = (CoordZ+0)%3;
        const int crd1 = (CoordZ+1)%3;
        const int crd2 = (CoordZ+2)%3;
        const Point3x d10 = v1 - v0;
        const Point3x d21 = v2 - v1;
        const Point3x d02 = v0 - v2;

        for(int x = ibox.min[crd1]; x <= ibox.max[crd1]; ++x) {
            for(int y = ibox.min[crd2]; y <= ibox.max[crd2]; ++y) {
                scalar n0 = (x-v0[crd1])*d10[crd2] - (y-v0[crd2])*d10[crd1];
                scalar n1 = (x-v1[crd1])*d21[crd2] - (y-v1[crd2])*d21[crd1];
                scalar n2 = (x-v2[crd1])*d02[crd2] - (y-v2[crd2])*d02[crd1];

                if( (n0>-EPS && n1>-EPS && n2>-EPS) ||
                    (n0< EPS && n1< EPS && n2< EPS))
                    set[crd0].AddIntercept(vcg::Point2i(x, y),
                                           InterceptType((dist - x*norm[crd1] - y*norm[crd2]) / norm[crd0], norm, quality));
            }
        }
    }

    void ScanFace(const Point3x &v0, const Point3x &v1, const Point3x &v2,
                  scalar quality, const Point3x &norm) {
        vcg::Box3<scalar> fbox;
        fbox.Set(v0);
        fbox.Add(v1);
        fbox.Add(v2);

        vcg::Box3i ibox(vcg::Point3i(ceil(fbox.min.X()), ceil(fbox.min.Y()), ceil(fbox.min.Z())),
                        vcg::Point3i(floor(fbox.max.X()), floor(fbox.max.Y()), floor(fbox.max.Z())));

        assert(norm.Norm() >0.999f && norm.Norm()<1.001f);
        scalar dist = norm * v0;

        RasterFace<0>(ibox, dist, norm, quality, v0, v1, v2);
        RasterFace<1>(ibox, dist, norm, quality, v0, v1, v2);
        RasterFace<2>(ibox, dist, norm, quality, v0, v1, v2);
    }

public:
    template <class MeshType>
            inline InterceptSet3(const MeshType &m, const Point3x &delta) : delta(delta) {
        //TODO: init bbox
        const Point3x invDelta(scalar(1) / delta.X(), scalar(1) / delta.Y(), scalar(1) / delta.Z());
        typename MeshType::ConstFaceIterator i, end = m.face.end();
        for (i = m.face.begin(); i != end; ++i)
            ScanFace (Point3x(i->V(0)->P()).Scale(invDelta),
                      Point3x(i->V(1)->P()).Scale(invDelta),
                      Point3x(i->V(2)->P()).Scale(invDelta),
                      0,
                      Point3x(i->cN()).Scale(delta).Normalize());
    }

    inline operator SortedType() const { return SortedType(delta, typename SortedType::ContainerType(set.begin(), set.end())); }

    const Point3x delta;
private:
    std::vector<InterceptSet2<scalar> > set;
};
#endif // INTERCEPT_H
