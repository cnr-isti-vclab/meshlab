#ifndef INTERCEPT_H
#define INTERCEPT_H

#include <algorithm>
#include <vector>
#include <vcg/complex/trimesh/base.h>
#include <vcg/space/box2.h>

#include "fixed.h"

#define OLD 1

#define p2print(point) ((point).X()) << ", " << ((point).Y())
#define p3print(point) p2print(point) << ", " << ((point).Z())

namespace vcg {
    namespace intercept {
        using namespace vcg::math;
        
        const bool debugRaster = true;
        ofstream debugRasterOut("/Users/ranma42/Desktop/out.raster.txt");
        ofstream debugInOut("/Users/ranma42/Desktop/out.in.txt");

        template <typename _dist_type, typename _scalar>
                class Intercept
        {
        public:
            typedef vcg::Point3<_scalar> Point3x;
            typedef _dist_type DistType;
            typedef _scalar Scalar;

        private:
            Point3x _norm;
            _dist_type _dist;
            _scalar _sort_norm;
            _scalar _quality;
            
        public:
            inline Intercept () { }
            
            inline Intercept (const DistType &dist, const Scalar &sort_norm) : _dist(dist), _sort_norm(sort_norm) { }

            inline Intercept (const DistType &dist, const Scalar &sort_norm, const Point3x &norm, _scalar quality) :
                    _norm(norm), _dist(dist), _sort_norm(sort_norm), _quality(quality) { }
            
            inline Intercept operator -() const { return Intercept(_dist, -_sort_norm, -_norm, _quality); }
            
#if OLD
            inline bool operator <(const Intercept &other) const { return _dist < other._dist || (_dist == other.dist() && _sort_norm < other._sort_norm); }
#else
            inline bool operator <(const Intercept &other) const { return _dist < other._dist; }
#endif
            inline bool operator <(const DistType &other) const { return _dist < other; }
            
            //inline friend bool operator<(const fraction &_dist, const Intercept &other) { return _dist < other._dist; }

            inline const DistType& dist() const { return _dist; }
            
            inline const Scalar& sort_norm() const { return _sort_norm; }

            inline const Scalar& quality() const { return _quality; }
            
            inline const Point3x& norm() const { return _norm; }
            
            friend ostream& operator<<(ostream &out, const Intercept<DistType, Scalar> &x) {
                out << "Intercept[" << x._dist << "[" << x._sort_norm << "], (" << p3print(x._norm) << "), " << x._quality << "]";
                return out;
            }
        };
        
        template <typename InterceptType>
                class InterceptRay
        {
            typedef typename InterceptType::DistType DistType;
            
            typedef std::vector<InterceptType> ContainerType;
            
        public:
            inline InterceptRay() { }
            
            inline InterceptRay(const ContainerType &set) : v(set) {
                cleanup();
                assert (isValid());
            }
            
#if OLD
            inline void cleanup() {
                std::sort(v.begin(), v.end());
                v.resize(v.size());
            }
#else
            inline void cleanup() {
                ContainerType newv;
                newv.reserve(v.size());

                std::sort(v.begin(), v.end());
                typename ContainerType::const_iterator from = v.begin();
                while (from != v.end()) {
                    typename ContainerType::const_iterator to = from + 1;
                    while (to != v.end() && from->dist() == to->dist())
                        to++;

                    size_t pcond = false, ncond = false;
                    typename ContainerType::const_iterator pos, neg;
                    while (from != to) {
                        if (from->sort_norm() > 0) {
                            pcond ^= true;
                            pos = from;
                        } else {
                            ncond ^= true;
                            neg = from;
                        }
                        from++;
                    }

                    if (pcond != ncond)
                        newv.push_back(pcond ? *pos : *neg);
                }
                newv.resize(newv.size());
                v = newv;
            }
#endif

            inline bool isValid() const {
                if (v.empty())
                    return true;

                if (v.size() % 2 != 0) {
                    cerr << "Not a solid! (size: " << v.size() << ")" << endl;
                    return false;
                }

                typename ContainerType::const_iterator curr = v.begin();
                typename ContainerType::const_iterator next = curr+1;
                while (next != v.end()) {
                    if (!(*curr < *next)) {
                        cerr << "Not sorted! (" << *curr << " >= " << *next << ")" << endl;
                        return false;
                    }
                    curr = next;
                    next++;
                }

                return true;
            }

            inline const InterceptType& GetIntercept(const DistType& s) const {
                typename ContainerType::const_iterator p = std::lower_bound(v.begin(), v.end(), s);
                assert (IsInExt(s) != IsInExt(s+1));
                assert (p != v.end());
                assert (s <= p->dist() && p->dist() <= s+1);
                return *p;
            }

            inline int IsInExt(const DistType& s) const {
                typename ContainerType::const_iterator p = std::lower_bound(v.begin(), v.end(), s);
                if (p == v.end())
                    return -1;
                else if (p->dist() == s)
                    return 0;
                else
                    return ((p - v.begin()) & 1) ? 1 : -1;
            }

            inline InterceptRay operator &(const InterceptRay & other) const {
                typename ContainerType::const_iterator i = v.begin(), j = other.v.begin(), endi = v.end(), endj = other.v.end();
                ContainerType newv;

                newv.reserve(v.size() + other.v.size());
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
                return InterceptRay(newv);
            }

            inline InterceptRay operator |(const InterceptRay & other) const {
                typename ContainerType::const_iterator i = v.begin(), j = other.v.begin(), endi = v.end(), endj = other.v.end();
                ContainerType newv;

                newv.reserve(v.size() + other.v.size());
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
                return InterceptRay(newv);
            }

            inline InterceptRay operator -(const InterceptRay & other) const {
                typename ContainerType::const_iterator i = v.begin(), j = other.v.begin(), endi = v.end(), endj = other.v.end();
                ContainerType newv;

                newv.reserve(v.size() + other.v.size());
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
                return InterceptRay(newv);
            }

            friend ostream& operator<<(ostream &out, const InterceptRay<InterceptType> &x) {
                typename ContainerType::const_iterator i;
                typename ContainerType::const_iterator end = x.v.end();
                out << "InterceptRay[";
                for (i = x.v.begin(); i != end; ++i)
                    out << *i;
                out << "]";
                assert (x.isValid());
                return out;
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

            inline InterceptBeam(const vcg::Box2i &box, const ContainerType &rays) : bbox(box), ray(rays) { }

            inline const IRayType& GetInterceptRay (const vcg::Point2i &p) const {
                assert(bbox.IsIn(p));
                vcg::Point2i c = p - bbox.min;
                assert(c.X() >= 0 && c.Y() >= 0);
                assert(size_t(c.X()) < ray.size() && size_t(c.Y()) < ray[c.X()].size());
                return ray[c.X()][c.Y()];
            }

            inline int IsInExt (const vcg::Point2i &p, const DistType& s) const {
                return bbox.IsIn(p) ? GetInterceptRay(p).IsInExt(s) : -1;
            }

            inline InterceptBeam &operator &=(const InterceptBeam & other) {
                vcg::Box2i newbbox(bbox);
                newbbox.Intersect(other.bbox);

                for(int i = 0; i <= newbbox.DimX(); ++i) {
                    for(int j = 0; j <= newbbox.DimY(); ++j) {
                        vcg::Point2i p = newbbox.min + vcg::Point2i(i,j);
                        ray[i][j] = GetInterceptRay(p) & other.GetInterceptRay(p);
                    }
                    ray[i].resize(newbbox.DimY() + 1);
                }
                ray.resize(newbbox.DimX() + 1);
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

            friend ostream& operator<<(ostream &out, const InterceptBeam<InterceptType> &x) {
                out << "InterceptBeam[" << p2print(x.bbox.min) << " - " << p2print(x.bbox.max) << "][" << endl;

                for(int i = x.bbox.min.X(); i <= x.bbox.max.X(); ++i) {
                    for(int j = x.bbox.min.Y(); j <= x.bbox.max.Y(); ++j) {
                        vcg::Point2i p(i,j);
                        out << p2print(p) << ": " << x.GetInterceptRay(p) << endl;
                    }
                }
                out << "]";
                return out;
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

            inline InterceptVolume(const Box3i &b, const Point3x &d, const ContainerType &beams) : delta(d), bbox(b), beam(beams) { assert(beams.size() == 3); };

            inline InterceptVolume & operator &=(const InterceptVolume & other) {
                assert(checkConsistency(other));
                for (int i = 0; i < 3; ++i)
                    beam[i] &= other.beam[i];
                bbox.Intersect(other.bbox);
                return *this;
            }

            inline InterceptVolume & operator |=(const InterceptVolume & other) {
                assert(checkConsistency(other));
                for (int i = 0; i < 3; ++i)
                    beam[i] |= other.beam[i];
                bbox.Add(other.bbox);
                return *this;
            }

            inline InterceptVolume & operator -=(const InterceptVolume & other) {
                assert(checkConsistency(other));
                for (int i = 0; i < 3; ++i)
                    beam[i] -= other.beam[i];
                return *this;
            }

            template <const int coord>
                    inline const InterceptType& GetIntercept (const Point3i &p1) const {
                assert(0 <= coord && coord < 3);
                assert(IsInExt(p1) != IsInExt(p1 + Point3i(coord == 0, coord == 1, coord == 2)));

                const int c1 = (coord + 1) % 3;
                const int c2 = (coord + 2) % 3;
                return beam[coord].GetInterceptRay(Point2i(p1.V(c1), p1.V(c2))).GetIntercept(p1.V(coord));
            }

            inline const InterceptRay<InterceptType> & GetInterceptRay (int coord, const vcg::Point3i &p) const {
                assert(0 <= coord && coord < 3);
                const int c1 = (coord + 1) % 3;
                const int c2 = (coord + 2) % 3;
                return beam[coord].GetInterceptRay(vcg::Point2i(p.V(c1), p.V(c2)));
            }

            inline int IsInExt (const vcg::Point3i &p) const {
                int r[3]; // TODO: simplify me!
                for (int i = 0; i < 3; ++i)
                    r[i] = beam[i].IsInExt(Point2i(p.V((i+1)%3), p.V((i+2)%3)), p.V(i));

                if (r[0] == 0)
                    r[0] += r[1] + r[2];
                if (r[1] == 0)
                    r[1] += r[0] + r[1];
                if (r[2] == 0)
                    r[2] += r[2] + r[0];

                if (r[0]>0 && r[1]>0 && r[2]>0)
                    return 1;
                else if ((r[0]<0 && r[1]<0 && r[2]<0) ||
                         r[0]==0 && r[1]==0 && r[2] == 0)
                    return -1;

                cerr << "Inconsistency: " << p3print(p) << p3print(delta) << endl;
                for (int i = 0; i < 3; ++i) {
                    cerr << beam[i].IsInExt(Point2i(p.V((i+1)%3), p.V((i+2)%3)), p.V(i));
                    cerr << ": " << beam[i].GetInterceptRay(Point2i(p.V((i+1)%3), p.V((i+2)%3))) << endl;
                }

                assert (false);
                return 0;
            }

            inline bool IsInOld (const vcg::Point3i &p) const {
                bool r[3]; // TODO: simplify me!
                for (int i = 0; i < 3; ++i)
                    r[i] = beam[i].IsInOld(Point2i(p.V((i+1)%3), p.V((i+2)%3)), p.V(i));
                if (!(r[0] == r[1] && r[1] == r[2])) {
                    cerr << "Inconsistency: " << p3print(p) << p3print(delta) << endl;
                    for (int i = 0; i < 3; ++i)
                        cerr << r[i] << ": " << beam[i].GetInterceptRay(Point2i(p.V((i+1)%3), p.V((i+2)%3))) << endl;
                }
                //assert(r[0] == r[1] && r[1] == r[2]);
                return r[0] & r[1] & r[2];
            }

            friend ostream& operator<<(ostream &out, const InterceptVolume<InterceptType> &x) {
                out << "InterceptVolume[" << p3print(x.delta) << "][" << endl;
                int coord = 0;
                for(typename ContainerType::const_iterator iter = x.beam.begin(); iter != x.beam.end(); ++iter) {
                    out << *iter << endl;
                    out << "Beam " << coord << endl;

                    for (int i=x.bbox.min[coord]; i<=x.bbox.max[coord]; i+=1) {
                        out << i << endl;

                        for (int k=x.bbox.min[(coord+2)%3]; k<=x.bbox.max[(coord+2)%3]+2; k+=1)
                            out << '+';
                        out << endl;

                        for (int j=x.bbox.min[(coord+1)%3]; j<=x.bbox.max[(coord+1)%3]; j+=1) {
                            out << '+';
                            for (int k=x.bbox.min[(coord+2)%3]; k<=x.bbox.max[(coord+2)%3]; k+=1) {
                                Point3i p(i,j,k);
                                int in = iter->IsInExt(Point2i(j, k), i);
                                char c = '?';
                                if (in < 0)
                                    c = ' ';
                                else if (in > 0)
                                    c = '#';
                                out << p3print(Point3i(i,j,k)) << " -> " << in << "[" << c << "]" << endl;
                                //out << c;
                            }
                            out << '+' << endl;
                        }

                        for (int k=x.bbox.min[(coord+2)%3]; k<x.bbox.max[(coord+2)%3]+2; k+=1)
                            out << '+';
                        out << endl;
                    }
                    coord++;
                }
                out << "]";

                return out;
            }

            const Point3x delta;
            Box3i bbox;
        private:
            ContainerType beam;
        };


        template <typename InterceptType>
                class InterceptSet
        {
            typedef std::vector<InterceptType> ContainerType;
            typedef InterceptRay<InterceptType> SortedType;

        public:
            inline InterceptSet() { }

            inline void AddIntercept (const InterceptType &x) { v.push_back(x); }
            inline operator SortedType() const { return SortedType(v); }

            friend ostream& operator<<(ostream &out, const InterceptSet<InterceptType> &x) {
                typename ContainerType::const_iterator i;
                typename ContainerType::const_iterator end = x.v.end();
                out << "InterceptSet[";
                for (i = x.v.begin(); i != end; ++i)
                    out << *i << endl;
                out << "]";
                return out;
            }

        private:
            ContainerType v;
        };

        template <typename InterceptType>
                class InterceptSet1
        {
            typedef std::vector<InterceptSet<InterceptType> > ContainerType;
            typedef std::vector<InterceptRay<InterceptType> > SortedType;

        public:
            inline InterceptSet1() { }

            inline operator SortedType() const { return SortedType(set.begin(), set.end()); }

            inline void resize(size_t size) { set.resize(size); }

            inline void AddIntercept (const int i, const InterceptType &x) {
                assert(i >= 0);
                assert(size_t(i) < set.size());
                set[i].AddIntercept(x);
            }

            friend ostream& operator<<(ostream &out, const InterceptSet1<InterceptType> &x) {
                typename ContainerType::const_iterator i;
                typename ContainerType::const_iterator end = x.set.end();
                out << "InterceptSet1[";
                for (i = x.set.begin(); i != end; ++i)
                    out << *i << endl;
                out << "]InterceptSet1";
                return out;
            }

        private:
            ContainerType set;
        };

        template <typename InterceptType>
                class InterceptSet2
        {
            typedef std::vector<InterceptSet1<InterceptType> > ContainerType;
            typedef std::vector<std::vector<InterceptSet<InterceptType> > > NewContainerType;
            typedef InterceptBeam<InterceptType> SortedType;

        public:
            inline InterceptSet2(const Box2i &box) : bbox(box), set(box.DimX() + 1) {
                typename ContainerType::iterator i;
                typename ContainerType::iterator end = set.end();
                for (i = set.begin(); i != end; ++i)
                    i->resize(box.DimY() + 1);
            }

            inline operator SortedType() const { return SortedType(bbox, typename SortedType::ContainerType(set.begin(), set.end())); }

            inline void AddIntercept (const vcg::Point2i &p, const InterceptType &x) {
                assert(bbox.IsIn(p));
                vcg::Point2i c = p - bbox.min;
                assert(c.X() >= 0);
                assert(size_t(c.X()) < set.size());
                set[c.X()].AddIntercept(c.Y(), x);
            }

            friend ostream& operator<<(ostream &out, const InterceptSet2<InterceptType> &x) {
                typename ContainerType::const_iterator i;
                typename ContainerType::const_iterator end = x.set.end();
                out << "InterceptSet2[";
                for (i = x.set.begin(); i != end; ++i)
                    out << *i << endl;
                out << "]InterceptSet2";
                return out;
            }

        private:
            Box2i bbox;
            ContainerType set;
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
            typedef std::vector<ISet2Type> ContainerType;

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

                const DistType det0 = d21[crd2] * d02[crd1] - d21[crd1] * d02[crd2];
                const DistType det1 = d21[crd0] * d02[crd2] - d21[crd2] * d02[crd0];
                const DistType det2 = d21[crd1] * d02[crd0] - d21[crd0] * d02[crd1];

                if (debugRaster) {
                    debugRasterOut << "Face [" << crd0 << "] ";
                    debugRasterOut << "(" << p3print(v0) << "), ";
                    debugRasterOut << "(" << p3print(v1) << "), ";
                    debugRasterOut << "(" << p3print(v2) << ")" << endl;

                    debugRasterOut << p3print(d10) << endl;
                }
                for(int x = ibox.min[crd1]; x <= ibox.max[crd1]; ++x) {
                    for(int y = ibox.min[crd2]; y <= ibox.max[crd2]; ++y) {
                        DistType n0 = (v1[crd1]-x)*d21[crd2] - (v1[crd2]-y)*d21[crd1];
                        DistType n1 = (v2[crd1]-x)*d02[crd2] - (v2[crd2]-y)*d02[crd1];
                        DistType n2 = (v0[crd1]-x)*d10[crd2] - (v0[crd2]-y)*d10[crd1];

                        if (debugRaster)
                            debugRasterOut << x << "," << y << ": " << p3print(Point3dt(n0,n1,n2)) << " -> ";

                        if (crd1 > crd2) {
                            if (n2 == 0)
                                n2 = d10[crd1];
                            if (n2 == 0)
                                n2 -= d10[crd2];

                            if (n0 == 0)
                                n0 = d21[crd1];
                            if (n0 == 0)
                                n0 -= d21[crd2];

                            if (n1 == 0)
                                n1 = d02[crd1];
                            if (n1 == 0)
                                n1 -= d02[crd2];
                        } else {
                            if (n2 == 0)
                                n2 -= d10[crd2];
                            if (n2 == 0)
                                n2 = d10[crd1];

                            if (n0 == 0)
                                n0 -= d21[crd2];
                            if (n0 == 0)
                                n0 = d21[crd1];

                            if (n1 == 0)
                                n1 -= d02[crd2];
                            if (n1 == 0)
                                n1 = d02[crd1];
                        }

                        if (debugRaster)
                            debugRasterOut << p3print(Point3dt(n0,n1,n2)) << endl;

                        if((n0>0 && n1>0 && n2>0) || (n0<0 && n1<0 && n2<0)) {
                            DistType d = (v0[crd2] - y) * det2 + (v0[crd1] - x) * det1;
                            d /= det0;
                            d += v0[crd0];
                            assert(d >= ibox.min[crd0] && d <= ibox.max[crd0]);
                            set[crd0].AddIntercept(vcg::Point2i(x, y), InterceptType(d, norm[crd0], norm, quality));
                        }
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

        public:
            template <class MeshType>
                    inline InterceptSet3(const MeshType &m, const Point3x &d) : delta(d),
                    bbox(Point3i(floor(m.bbox.min.X() / d.X()) -1,
                                 floor(m.bbox.min.Y() / d.Y()) -1,
                                 floor(m.bbox.min.Z() / d.Z()) -1),
                         Point3i(ceil(m.bbox.max.X() / d.X()),
                                 ceil(m.bbox.max.Y() / d.Y()),
                                 ceil(m.bbox.max.Z() / d.Z())))
            {
                const Point3x invDelta(Scalar(1) / delta.X(),
                                       Scalar(1) / delta.Y(),
                                       Scalar(1) / delta.Z());

                vcg::Box2i xy, yz, zx;
                yz.Set(bbox.min.Y(), bbox.min.Z(), bbox.max.Y(), bbox.max.Z());
                zx.Set(bbox.min.Z(), bbox.min.X(), bbox.max.Z(), bbox.max.X());
                xy.Set(bbox.min.X(), bbox.min.Y(), bbox.max.X(), bbox.max.Y());

                set.push_back(ISet2Type(yz));
                set.push_back(ISet2Type(zx));
                set.push_back(ISet2Type(xy));

                typename MeshType::ConstFaceIterator i, end = m.face.end();
                for (i = m.face.begin(); i != end; ++i) {
                    Point3x v0(i->V(0)->P()), v1(i->V(1)->P()), v2(i->V(2)->P());
                    v0.Scale(invDelta);
                    v1.Scale(invDelta);
                    v2.Scale(invDelta);
                    ScanFace (Point3dt(v0.X(), v0.Y(), v0.Z()),
                              Point3dt(v1.X(), v1.Y(), v1.Z()),
                              Point3dt(v2.X(), v2.Y(), v2.Z()),
                              0,
                              Point3x(i->cN()).Scale(delta).Normalize());
                }
            }

            inline operator SortedType() const { return SortedType(bbox, delta, typename SortedType::ContainerType(set.begin(), set.end())); }

            friend ostream& operator<<(ostream &out, const InterceptSet3<InterceptType> &x) {
                typename ContainerType::const_iterator i;
                typename ContainerType::const_iterator end = x.set.end();
                out << "InterceptSet3[";
                for (i = x.set.begin(); i != end; ++i)
                    out << *i << endl;
                out << "]InterceptSet3";
                return out;
            }

            const Point3x delta;
            const Box3i bbox;
        private:
            ContainerType set;
        };

        template <typename MeshType, typename InterceptType>
                class Walker
        {
            typedef typename MeshType::VertexPointer VertexPointer;
            typedef typename MeshType::CoordType CoordType;

        public:
            template<typename EXTRACTOR_TYPE>
            void BuildMesh(MeshType &mesh, InterceptVolume<InterceptType> &volume, EXTRACTOR_TYPE &extractor)
            {
                _volume = &volume;
                _mesh = &mesh;
                _mesh->Clear();

                extractor.Initialize();
                for (int j=_volume->bbox.min.Y(); j<=_volume->bbox.max.Y(); ++j)
                {
                    for (int i=_volume->bbox.min.X(); i<=_volume->bbox.max.X(); ++i)
                        for (int k=_volume->bbox.min.Z(); k<=_volume->bbox.max.Z(); ++k)
                            extractor.ProcessCell(vcg::Point3i(i,j,k),
                                                  vcg::Point3i(i+1,j+1,k+1));
                }
                extractor.Finalize();

                _volume = NULL;
                _mesh = NULL;
            }

            const float V(int i, int j, int k) const { return _volume->IsInExt(vcg::Point3i(i, j, k)); }

            template <const int coord>
                    void GetIntercept(const vcg::Point3i &p1, const vcg::Point3i &p2, VertexPointer& p) {
                assert(p2 == p1 + vcg::Point3i(coord == 0, coord == 1, coord == 2));
                assert(_volume->IsInExt(p1) != _volume->IsInExt(p2));

                p = &*vcg::tri::Allocator<MeshType>::AddVertices(*_mesh, 1);
                const InterceptType& i = _volume->GetIntercept<coord>(p1);
                p->P().V(coord) = i.dist().toFloat();
                p->P().V((coord+1)%3) = p1[(coord+1)%3];
                p->P().V((coord+2)%3) = p1[(coord+2)%3];

                assert(p1.V(coord) <= p->P().V(coord) && p->P().V(coord) <= p2.V(coord));

                p->P().Scale(_volume->delta);
                p->N() = i.norm();
                p->Q() = i.quality();
            }

            void GetXIntercept(const vcg::Point3i &p1, const vcg::Point3i &p2, VertexPointer& p) { GetIntercept<0>(p1, p2, p); }

            void GetYIntercept(const vcg::Point3i &p1, const vcg::Point3i &p2, VertexPointer& p) { GetIntercept<1>(p1, p2, p); }

            void GetZIntercept(const vcg::Point3i &p1, const vcg::Point3i &p2, VertexPointer& p) { GetIntercept<2>(p1, p2, p); }

            bool Exist(const Point3i &p1, const Point3i &p2, VertexPointer& p) {
                bool in1 = V(p1.X(), p1.Y(), p1.Z());
                bool in2 = V(p2.X(), p2.Y(), p2.Z());
                if (in1 == in2)
                    return false;

                Point3i d = p2 - p1;
                if (d.X())
                    GetXIntercept(p1, p2, p);
                else if (d.Y())
                    GetYIntercept(p1, p2, p);
                else if (d.Z())
                    GetZIntercept(p1, p2, p);

                return true;
            }

        private:
            InterceptVolume<InterceptType> *_volume;
            MeshType *_mesh;
        };
    };
};
#endif // INTERCEPT_H
