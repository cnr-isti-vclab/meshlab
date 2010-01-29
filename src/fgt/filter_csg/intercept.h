#ifndef INTERCEPT_H
#define INTERCEPT_H

#include <algorithm>
#include <vector>
#include <vcg/complex/trimesh/base.h>
#include <vcg/space/box2.h>

#include "fixed.h"

namespace vcg {
    namespace intercept {
        using namespace vcg::math;

        template <typename _dist_type, typename _scalar>
                class Intercept
        {
            typedef vcg::Point3<_scalar> Point3x;

        private:
            Point3x norm;
            _scalar quality;

        public:
            typedef _dist_type DistType;
            typedef _scalar Scalar;

            DistType dist;

            inline Intercept () {};

            inline Intercept (DistType dist, const Point3x &norm, Scalar quality) : norm(norm), quality(quality), dist(dist) {};

            inline Intercept operator -() const { return Intercept(dist, -norm, quality); }

            inline bool operator <(const Intercept &other) const { return dist < other.dist; }

            inline bool operator <(DistType other) const { return dist < other; }
        };

        template <typename InterceptType>
                class InterceptRay
        {
            typedef typename InterceptType::DistType DistType;

            typedef std::vector<InterceptType> ContainerType;

        public:
            inline InterceptRay() {};

            inline InterceptRay(const ContainerType &set) : v(set) {
                std::sort(v.begin(), v.end());
                assert(v.size() % 2 == 0);
            }

            inline const InterceptType& GetIntercept(DistType s) const {
                // TODO: s==p.dist
                typename ContainerType::const_iterator p = std::lower_bound(v.begin(), v.end(), s);
                assert (p != v.end());
                return *p;
            }

            inline bool IsIn(DistType s) const {
                // TODO: s==p.dist
                typename ContainerType::const_iterator p = std::lower_bound(v.begin(), v.end(), s);
                return p != v.end() && (p - v.begin()) & 1;
            }

            inline InterceptRay operator &(const InterceptRay & other) const {
                ContainerType newv(v.size() + other.v.size());
                typename ContainerType::const_iterator i = v.begin(), j = other.v.begin(), endi = v.end(), endj = v.end();
                while (i != endi && j != endj) {
                    if (*j < *i) {
                        std::swap(i, j);
                        std::swap(endi, endj);
                    }
                    // i <= j < J
                    if (*j < *(i+1)) { // i <= j < I & i <= j < J
                        while (j != endj && *(j+1) < *(i+1)) { // i <= j < J < I
                            newv.push_back(*j);
                            newv.push_back(*(j+1));
                            j += 2;
                        }
                        // i < I <= J & i <= j < J
                        if (j != endj && *j < *(i+1)) {  // i <= j < I <= J
                            newv.push_back(*j);
                            newv.push_back(*(i+1));
                        }
                    }
                    i += 2;
                }
                newv.resize(newv.size());
                return InterceptRay(newv);
            }

            inline InterceptRay operator |(const InterceptRay & other) const {
                ContainerType newv(v.size() + other.v.size());
                typename ContainerType::const_iterator i = v.begin(), j = other.v.begin(), endi = v.end(), endj = v.end();
                while (i != endi && j != endj) {
                    if (*j < *i) {
                        std::swap(i, j);
                        std::swap(endi, endj);
                    }
                    // i <= j < J
                    newv.push_back(*i);
                    while (i != endi && j != endj) {
                        while (j != endj && *(j+1) < *(i+1)) // j < J < I
                            j += 2;
                        // i < I <= J & i <= j < J
                        if (j == endj || *(i+1) < *j) { // i < I < j < J
                            newv.push_back(*(i+1));
                            i += 2;
                        } else { // i < j <= I < J
                            std::swap(i, j);
                            std::swap(endi, endj);
                        }
                    }
                }
                newv.insert(newv.end(), i, endi);
                newv.insert(newv.end(), j, endj);
                newv.resize(newv.size());
                return InterceptRay(newv);
            }

            inline InterceptRay operator -(const InterceptRay & other) const {
                ContainerType newv(v.size() + other.v.size());
                typename ContainerType::const_iterator i = v.begin(), j = other.v.begin(), endi = v.end(), endj = v.end();
                while (i != endi && j != endj) {
                    while (j != endj && !(*i < *(j+1))) // j < J <= i
                        j += 2;
                    if (j == endj)
                        break;
                    else if (*i < *j) // i < j < J
                        newv.push_back(*i);
                    else { // j <= i < J
                        while (i != endi && !(*(j+1) < *(i+1))) // j <= i < I <= J
                            i += 2;
                        if (i != endi && *i < *(j+1)) { // j <= i < J < I
                            newv.push_back(-*(j+1));
                            j += 2;
                        } else  // j <= J <= i < I
                            continue;
                    }

                    while (j != endj && *(j+1) < *(i+1)) { // i < j < J < I
                        newv.push_back(-*j);
                        newv.push_back(-*(j+1));
                        j += 2;
                    }

                    if (j != endj && *j < *(i+1)) // i < j < I <= J
                        newv.push_back(-*j);
                    else  // i < I <= j < J
                        newv.push_back(*(i+1));
                    i += 2;
                }
                newv.insert(newv.end(), i, endi);
                newv.resize(newv.size());
                return InterceptRay(newv);
            }

        private:
            ContainerType v;
        };


        template <typename InterceptType>
                class InterceptBeam
        {
            typedef typename InterceptType::DistType DistType;
            typedef InterceptRay<InterceptType> IRayType;

        public:
            typedef std::vector<std::vector<IRayType > > ContainerType;

            inline InterceptBeam(const vcg::Box2i &box, const ContainerType &rays) : bbox(box), ray(rays) {};

            inline const IRayType& GetInterceptRay (const vcg::Point2i &p) const {
                assert(bbox.IsIn(p));
                vcg::Point2i c = p - bbox.min;
                return ray[c.X()][c.Y()];
            }

            inline bool IsIn (const vcg::Point2i &p, DistType s) const {
                return bbox.IsIn(p) && GetInterceptRay(p).IsIn(s);
            }

            inline InterceptBeam &operator &=(const InterceptBeam & other) {
                vcg::Box2i newbbox(bbox);
                newbbox.Intersect(other.bbox);

                for(int i = 0; i < newbbox.DimX(); ++i) {
                    for(int j = 0; j < newbbox.DimY(); ++j) {
                        vcg::Point2i p = newbbox.min + vcg::Point2i(i,j);
                        ray[i][j] = GetInterceptRay(p) & other.GetInterceptRay(p);
                    }
                    ray[i].resize(newbbox.DimY());
                }
                ray.resize(newbbox.DimX());
                bbox = newbbox;
                return *this;
            }

            inline InterceptBeam & operator |=(const InterceptBeam & other) {
                vcg::Box2i newbbox(bbox);
                newbbox.Add(other.bbox);

                ray.resize(newbbox.DimX());
                for(int i = newbbox.DimX() - 1; i >= 0; --i) {
                    ray[i].resize(newbbox.DimY());
                    for(int j = newbbox.DimY() - 1; j >= 0; --j) {
                        vcg::Point2i p = newbbox.min + vcg::Point2i(i,j);
                        ray[i][j] = (bbox.IsIn(p) ? GetInterceptRay(p) : IRayType()) &
                                    (other.bbox.IsIn(p) ? other.GetInterceptRay(p) : IRayType());
                    }
                }
                bbox = newbbox;
                return *this;
            }

            inline InterceptBeam & operator -=(const InterceptBeam & other) {
                vcg::Box2i damage(bbox);
                damage.Intersect(other.bbox);

                for(int i = 0; i < damage.DimX(); ++i) {
                    for(int j = 0; j < damage.DimY(); ++j) {
                        vcg::Point2i p = damage.min + vcg::Point2i(i,j);
                        vcg::Point2i p2 = p - bbox.min;
                        ray[p2.X()][p2.Y()] = GetInterceptRay(p) - other.GetInterceptRay(p);
                    }
                }
                return *this;
            }

        private:
            vcg::Box2i bbox;
            ContainerType ray;
        };

        template <typename InterceptType>
                class InterceptVolume
        {
            typedef typename InterceptType::DistType DistType;
            typedef typename InterceptType::Scalar Scalar;
            typedef vcg::Point3<Scalar> Point3x;

            inline bool checkConsistency (const InterceptVolume & other) const {
                return delta == other.delta;
            }

        public:
            typedef typename std::vector<InterceptBeam<InterceptType> > ContainerType;

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

            inline InterceptVolume & operator -=(const InterceptVolume & other) {
                assert(checkConsistency(other));
                for (int i = 0; i < 3; ++i)
                    beam[i] -= other.beam[i];
                return *this;
            }

            template <const int coord>
                    inline const InterceptType& GetIntercept (const vcg::Point3i &p) const {
                assert(0 <= coord && coord < 3);
                const int c1 = (coord + 1) % 3;
                const int c2 = (coord + 2) % 3;
                return beam[coord].GetInterceptRay(vcg::Point2i(p.V(c1), p.V(c2))).GetIntercept(p.V(coord));
            }

            inline bool IsIn (const vcg::Point3i &p) const {
                bool r[3]; // TODO: simplify me!
                for (int i = 0; i < 3; ++i)
                    r[i] = beam[i].IsIn(p);
                assert(r[0] == r[1] && r[1] == r[2]);
                return r[0];
            }

            const Point3x delta;
        private:
            ContainerType beam;
        };


        template <typename InterceptType>
                class InterceptSet
        {
            typedef InterceptRay<InterceptType> SortedType;

        public:
            inline void AddIntercept (const InterceptType &x) { v.push_back(x); }
            inline operator SortedType() const { return SortedType(v); }

        private:
            std::vector<InterceptType> v;
        };

        template <typename InterceptType>
                class InterceptSet1
        {
            typedef std::vector<InterceptRay<InterceptType> > SortedType;

        public:
            inline InterceptSet1() {};

            inline operator SortedType() const { return SortedType(set.begin(), set.end()); }

            inline void AddIntercept (const int i, const InterceptType &x) {
                set[i].AddIntercept(x);
            }

        private:
            std::vector<InterceptSet<InterceptType> > set;
        };

        template <typename InterceptType>
                class InterceptSet2
        {
            typedef InterceptBeam<InterceptType> SortedType;

        public:
            inline InterceptSet2(const vcg::Box2i &box) : bbox(box) { set.reserve(bbox.DimX()); };

            inline operator SortedType() const { return SortedType(bbox, typename SortedType::ContainerType(set.begin(), set.end())); }

            inline void AddIntercept (const vcg::Point2i &p, const InterceptType &x) {
                assert(bbox.IsIn(p));
                vcg::Point2i c = p - bbox.min;
                set[c.X()].AddIntercept(c.Y(), x);
            }

        private:
            vcg::Box2i bbox;
            std::vector<InterceptSet1<InterceptType> > set;
        };

        template <typename InterceptType>
                class InterceptSet3
        {
            typedef typename InterceptType::Scalar Scalar;
            typedef typename InterceptType::DistType DistType;
            typedef vcg::Point3<DistType> Point3dt;
            typedef vcg::Point3<Scalar> Point3x;
            typedef InterceptSet2<InterceptType> ISet2Type;
            typedef InterceptVolume<InterceptType> SortedType;

            template <const int CoordZ>
                    void RasterFace(const Point3dt &v0, const Point3dt &v1, const Point3dt &v2,
                                    const vcg::Box3i &ibox, const Point3x &norm, Scalar quality)
            {
                const int crd0 = (CoordZ+0)%3;
                const int crd1 = (CoordZ+1)%3;
                const int crd2 = (CoordZ+2)%3;
                const Point3dt d10 = v1 - v0;
                const Point3dt d21 = v2 - v1;
                const Point3dt d02 = v0 - v2;

                for(int x = ibox.min[crd1]; x <= ibox.max[crd1]; ++x) {
                    for(int y = ibox.min[crd2]; y <= ibox.max[crd2]; ++y) {
                        /*
                scalar n0 = (x-v0[crd1])*d10[crd2] - (y-v0[crd2])*d10[crd1];
                scalar n1 = (x-v1[crd1])*d21[crd2] - (y-v1[crd2])*d21[crd1];
                scalar n2 = (x-v2[crd1])*d02[crd2] - (y-v2[crd2])*d02[crd1];

                if( (n0>-EPS && n1>-EPS && n2>-EPS) ||
                    (n0< EPS && n1< EPS && n2< EPS))
                    set[crd0].AddIntercept(vcg::Point2i(x, y),
                                           InterceptType((dist - x*norm[crd1] - y*norm[crd2]) / norm[crd0], norm, quality));
                                           */
                    }
                }
            }

            void ScanFace(const Point3dt &v0, const Point3dt &v1, const Point3dt &v2,
                          Scalar quality, const Point3x &norm) {
                vcg::Box3<DistType> fbox;
                fbox.Set(v0);
                fbox.Add(v1);
                fbox.Add(v2);

                vcg::Box3i ibox(vcg::Point3i(ceil(fbox.min.X()), ceil(fbox.min.Y()), ceil(fbox.min.Z())),
                                vcg::Point3i(floor(fbox.max.X()), floor(fbox.max.Y()), floor(fbox.max.Z())));

                RasterFace<0>(v0, v1, v2, ibox, norm, quality);
                RasterFace<1>(v0, v1, v2, ibox, norm, quality);
                RasterFace<2>(v0, v1, v2, ibox, norm, quality);
            }

            void ScanFace(const Point3x &v0, const Point3x &v1, const Point3x &v2,
                          Scalar quality, const Point3x &norm) {
                ScanFace(Point3dt(v0.X(), v0.Y(), v0.Z()),
                         Point3dt(v1.X(), v1.Y(), v1.Z()),
                         Point3dt(v2.X(), v2.Y(), v2.Z()),
                         quality, norm);
            }

        public:
            template <class MeshType>
                    inline InterceptSet3(const MeshType &m, const Point3x &d) : delta(d) {
                const Point3x invDelta(Scalar(1) / delta.X(),
                                       Scalar(1) / delta.Y(),
                                       Scalar(1) / delta.Z());

                set.push_back(ISet2Type(vcg::Box2i(/*TODO*/)));
                set.push_back(ISet2Type(vcg::Box2i(/*TODO*/)));
                set.push_back(ISet2Type(vcg::Box2i(/*TODO*/)));

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
            std::vector<ISet2Type> set;
        };

        template <typename MeshType, typename scalar>
                class Walker
        {
            typedef typename MeshType::VertexPointer VertexPointer;
            typedef Intercept<double,scalar> InterceptType;

        public:
            inline Walker(MeshType &mesh, const InterceptVolume<InterceptType> &volume) : m(mesh), v(volume) {}

            const scalar V(int i, int j, int k) const { return scalar(v.IsIn(vcg::Point3i(i, j, k)) ? 1.0 : -1.0); }

            template <const int coord>
                    void GetIntercept(const vcg::Point3i &p1, const vcg::Point3i &p2, VertexPointer p) {
                assert(p1 == p2 + vcg::Point3i(coord == 0, coord == 1, coord == 2));
                p = m.vert.end();
                //        MeshType::Allocator<MeshType>::AddVertices(m, 1);
                p->P() = MeshType::CoordType(p1);
                p->P().V(coord) = v.GetIntercept<coord>(p1).dist;
                assert(p1.V(coord) <= p->P().V(coord) && p->P().V(coord) <= p2.V(coord));
            }

            void GetXIntercept(const vcg::Point3i &p1, const vcg::Point3i &p2, VertexPointer p) { GetIntercept<0>(p1, p2, v); }

            void GetYIntercept(const vcg::Point3i &p1, const vcg::Point3i &p2, VertexPointer p) { GetIntercept<1>(p1, p2, v); }

            void GetZIntercept(const vcg::Point3i &p1, const vcg::Point3i &p2, VertexPointer p) { GetIntercept<2>(p1, p2, v); }

        private:
            MeshType &m;
            InterceptVolume<InterceptType> v;
        };
    };
};
#endif // INTERCEPT_H
