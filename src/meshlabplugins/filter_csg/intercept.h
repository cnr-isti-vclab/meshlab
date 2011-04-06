#ifndef INTERCEPT_H
#define INTERCEPT_H

#include <algorithm>
#include <vector>
#ifdef _MSC_VER
    #include <unordered_map>
    #include <unordered_set>
#else
    #include <tr1/unordered_map>
    #include <tr1/unordered_set>
#endif

#include <vcg/complex/complex.h>
#include <vcg/complex/algorithms/clean.h>
#include <vcg/space/box2.h>
#include <wrap/callback.h>

#define p2print(point) ((point).X()) << ", " << ((point).Y())
#define p3print(point) p2print(point) << ", " << ((point).Z())

namespace std {
    namespace tr1 {
        template <>
                struct hash<vcg::Point3i> : public std::unary_function<vcg::Point3i, std::size_t>
        {
            std::size_t operator()(const vcg::Point3i &x) const
            {
                std::tr1::hash<int> h;
                return ((x[0]) * 131 + h(x[1])) * 131 + h(x[2]);
            }
        };
        
    }
}

/*
  This header contains an implementation of "Marching Intersections"
  For more details on this technique, see:
    C. Rocchini, P. Cignoni, F. Ganovelli, C. Montani, P. Pingi and R.Scopigno,
    'Marching Intersections: an Efficient Resampling Algorithm for Surface Management'
    In Proceedings of Shape Modeling International (SMI) 2001

  This technique is used to create a volume representation where CSG operations are simple.

  Given a mesh, InterceptSet3 "rasterizes" it by computing all the intersections between
  the mesh and 3 orthogonal families of lines (and storing them in the InterceptSet* containers).
  After sorting them, they are organized in an InterceptVolume, which contains 3 InterceptBeam
  objects, each containing the intersections with one family of lines.

  This makes it vary easy to perform boolean operations, since along each line we have
  a sequence of intervals that determine what is inside and what is outside of the volume.
  To compute the intersection of two volumes, just get their "interval" representation and
  for each line intersect the intervals (and similarly for union and difference).

  Two notable problems are mesh consistency and performance:
   - to guarantee that the interval representation is meaningful, the mesh needs to be watertight
     and the rasterization step must not introduce any inaccuracies; this can be accomplished by
     using arbitrary precision arithmetic (currently through libgmp)
   - when reconstructing the result mesh, visiting the whole volume is not efficient; it is
     possible (and usually much faster) to only visit the surface
  */

namespace vcg {
    /** Class Intercept
        Minimal data structure to collect the information about the intersection
        between a line and a meshThis is class for definition of a mesh.
        @param _dist_type (Template Parameter) Specifies the type of the distance value
        @param _scalar (Template Parameter) Specifies the type of the scalar elements
     */
    namespace intercept {
        template <typename _dist_type, typename _scalar>
                class Intercept
        {
        public:
            typedef _dist_type DistType;
            typedef _scalar Scalar;
            typedef vcg::Point3<Scalar> Point3x;
            
        private:
            DistType _dist; /* distance of the intersection from the reference point */
            Point3x _norm; /* normal of the mesh in the intersection point */
            Scalar _sort_norm; /* component of the normal used for sorting (needed to discriminate intercepts having the same distance) */
            Scalar _quality; /* quality of the mesh in the intersection point */
            
        public:
            inline Intercept() { }
            
            inline Intercept(const DistType &dist, const Point3x &norm, const Scalar &sort_norm, const Scalar &quality) :
                    _dist(dist), _norm(norm), _sort_norm(sort_norm), _quality(quality) { }
            
            inline Intercept operator -() const { return Intercept(_dist, -_norm, -_sort_norm, _quality); }
            
            inline bool operator <(const Intercept &other) const { return _dist < other._dist || (_dist == other.dist() && _sort_norm < other._sort_norm); }
            
            inline bool operator <(const DistType &other) const { return _dist < other; }
            
            inline const DistType& dist() const { return _dist; }
            
            inline const Scalar& sort_norm() const { return _sort_norm; }
            
            inline const Scalar& quality() const { return _quality; }
            
            inline const Point3x& norm() const { return _norm; }
            
            friend std::ostream& operator<<(std::ostream &out, const Intercept &x) {
                return out << "Intercept[" << x._dist << "[" << x._sort_norm << "], (" << p3print(x._norm) << "), " << x._quality << "]";
            }
        };
        
        /** Class InterceptRay
            Class to collect all the intersections between a mesh and a line,
            keeping them sorted to have efficient operations
            @param InterceptType (Template Parameter) Specifies the type of the intercepts of the ray
         */
        template <typename InterceptType>
                class InterceptRay
        {
        public:
            typedef std::vector<InterceptType> ContainerType;
            
        private:
            typedef typename InterceptType::DistType DistType;
            
            inline void cleanup() {
                std::sort(v.begin(), v.end());
                v.resize(v.size());
            }
            
            inline bool isValid() const {
                if (v.empty())
                    return true;
                
                if (v.size() % 2 != 0) {
                    std::cerr << "Not a solid! (size: " << v.size() << ")" << std::endl;
                    return false;
                }
                
                typename ContainerType::const_iterator curr = v.begin();
                typename ContainerType::const_iterator next = curr+1;
                while (next != v.end()) {
                    if (*next < *curr) {
                        std::cerr << "Not sorted! (" << *curr << " > " << *next << ")" << std::endl;
                        return false;
                    }
                    curr = next;
                    next++;
                }
                
                return true;
            }
            
        public:
            inline InterceptRay() { }
            
            inline InterceptRay(const ContainerType &set) : v(set) {
                cleanup();
                assert (isValid());
            }
            
            inline const ContainerType& container() const { return v; }
            
            inline const InterceptType& GetIntercept(const DistType &s) const {
                assert (IsIn(s) != IsIn(s+1) || IsIn(s) == 0);
                typename ContainerType::const_iterator p = std::lower_bound(v.begin(), v.end(), s);
                assert (p != v.end());
                assert (s <= p->dist() && p->dist() <= s+1);
                return *p;
            }
            
            /* Returns -1 if the point at the given distance along the ray is outside,
               0 if it is on the boundary, 1 if it is inside. */
            inline int IsIn(const DistType &s) const {
                typename ContainerType::const_iterator p = std::lower_bound(v.begin(), v.end(), s);
                if (p == v.end())
                    return -1;
                else if (p->dist() == s)
                    return 0;
                else
                    return ((p - v.begin()) & 1) ? 1 : -1;
            }
            
            inline InterceptRay operator &(const InterceptRay &other) const {
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
            
            inline InterceptRay operator |(const InterceptRay &other) const {
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
                    while (j != endj && !(*(i+1) < *j)) {
                         // i <= j <= I & i <= j < J
                        while (j != endj && *(j+1) < *(i+1)) // i <= j < J < I
                            j += 2;
                        // i < I <= J & i <= j < J
                        if (j != endj && !(*(i+1) < *j)) { // i < j <= I < J
                            std::swap(i, j);
                            std::swap(endi, endj);
                            // j < i <= J < I
                        }
                    }
                    // i < I < j < J
                    newv.push_back(*(i+1));
                    i += 2;
                }
                newv.insert(newv.end(), i, endi);
                newv.insert(newv.end(), j, endj);
                return InterceptRay(newv);
            }
            
            inline InterceptRay operator -(const InterceptRay &other) const {
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
                     // i < j < J
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
            
            friend std::ostream& operator<<(std::ostream &out, const InterceptRay &x) {
                typename ContainerType::const_iterator i, end = x.v.end();
                out << "InterceptRay[";
                for (i = x.v.begin(); i != end; ++i)
                    out << *i;
                assert (x.isValid());
                return out << "]";
            }
            
        private:
            ContainerType v;
        };
        
        /** Class InterceptBeam
            Class to collect all the intersections between a mesh and a family of
            parallel lines, with efficient intersection, union and difference
            operations.
            @param InterceptType (Template Parameter) Specifies the type of the intercepts of the ray
         */
        template <typename InterceptType>
                class InterceptBeam
        {
            typedef typename InterceptType::DistType DistType;
            typedef InterceptRay<InterceptType> IRayType;
            
        public:
            typedef std::vector<std::vector<IRayType > > ContainerType;
            
            inline InterceptBeam(const vcg::Box2i &box, const ContainerType &rays) : bbox(box), ray(rays) { }
            
            inline const IRayType& GetInterceptRay(const vcg::Point2i &p) const {
                assert (bbox.IsIn(p));
                vcg::Point2i c = p - bbox.min;
                assert (c.X() >= 0 && c.Y() >= 0);
                assert (size_t(c.X()) < ray.size() && size_t(c.Y()) < ray[c.X()].size());
                return ray[c.X()][c.Y()];
            }
            
            /* Returns -1 if the point at the given point is outside,
               0 if it is on the boundary, 1 if it is inside. */
            inline int IsIn(const vcg::Point2i &p, const DistType &s) const {
                return bbox.IsIn(p) ? GetInterceptRay(p).IsIn(s) : -1;
            }
            
            inline InterceptBeam& operator &=(const InterceptBeam &other) {
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
            
            inline InterceptBeam& operator |=(const InterceptBeam &other) {
                vcg::Box2i newbbox(bbox);
                newbbox.Add(other.bbox);
                
                ray.resize(newbbox.DimX() + 1);
                for(int i = newbbox.DimX(); i >= 0; --i) {
                    ray[i].resize(newbbox.DimY() + 1);
                    for(int j = newbbox.DimY(); j >= 0; --j) {
                        vcg::Point2i p = newbbox.min + vcg::Point2i(i,j);
                        ray[i][j] = (bbox.IsIn(p) ? GetInterceptRay(p) : IRayType()) |
                                    (other.bbox.IsIn(p) ? other.GetInterceptRay(p) : IRayType());
                    }
                }
                bbox = newbbox;
                return *this;
            }
            
            inline InterceptBeam& operator -=(const InterceptBeam &other) {
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
            
            friend std::ostream& operator<<(std::ostream &out, const InterceptBeam &x) {
                out << "InterceptBeam[" << p2print(x.bbox.min) << " - " << p2print(x.bbox.max) << "][" << std::endl;
                for(int i = x.bbox.min.X(); i <= x.bbox.max.X(); ++i) {
                    for(int j = x.bbox.min.Y(); j <= x.bbox.max.Y(); ++j) {
                        vcg::Point2i p(i,j);
                        out << p2print(p) << ": " << x.GetInterceptRay(p) << std::endl;
                    }
                }
                return out << "]";
            }
            
        private:
            vcg::Box2i bbox;
            ContainerType ray;
        };
        
        /** Class InterceptBeam
            Three orthogonal InterceptBeam instances, defining a volume
            @param InterceptType (Template Parameter) Specifies the type of the intercepts of the ray
         */
        template <typename InterceptType>
                class InterceptVolume
        {
            typedef typename InterceptType::DistType DistType;
            typedef typename InterceptType::Scalar Scalar;
            typedef vcg::Point3<Scalar> Point3x;
            
            /* To perform intersection/union/difference on different volumes, their
               rays need to match exactly */
            inline bool checkConsistency(const InterceptVolume &other) const {
                return delta == other.delta;
            }
            
        public:
            typedef typename std::vector<InterceptBeam<InterceptType> > ContainerType;
            
            inline InterceptVolume(const Box3i &b, const Point3x &d, const ContainerType &beams) : delta(d), bbox(b), beam(beams) { assert (beams.size() == 3); };
            
            inline InterceptVolume& operator &=(const InterceptVolume &other) {
                assert (checkConsistency(other));
                for (int i = 0; i < 3; ++i)
                    beam[i] &= other.beam[i];
                bbox.Intersect(other.bbox);
                return *this;
            }
            
            inline InterceptVolume& operator |=(const InterceptVolume &other) {
                assert (checkConsistency(other));
                for (int i = 0; i < 3; ++i)
                    beam[i] |= other.beam[i];
                bbox.Add(other.bbox);
                return *this;
            }
            
            inline InterceptVolume& operator -=(const InterceptVolume &other) {
                assert (checkConsistency(other));
                for (int i = 0; i < 3; ++i)
                    beam[i] -= other.beam[i];
                return *this;
            }
            
            inline const InterceptRay<InterceptType>& GetInterceptRay(int coord, const vcg::Point2i &p) const {
                assert (0 <= coord && coord < 3);
                return beam[coord].GetInterceptRay(p);
            }
            
            template <const int coord>
                    inline const InterceptType& GetIntercept(const vcg::Point3i &p1) const {
                assert (0 <= coord && coord < 3);
                assert (IsIn(p1) != IsIn(p1 + vcg::Point3i(coord == 0, coord == 1, coord == 2)));
                
                const int c1 = (coord + 1) % 3;
                const int c2 = (coord + 2) % 3;
                return GetInterceptRay(coord, vcg::Point2i(p1.V(c1), p1.V(c2))).GetIntercept(p1.V(coord));
            }
            
            /* Return 1 if the given point is in the volume, -1 if it is outside */
            inline int IsIn(const vcg::Point3i &p) const {
                int r[3];
                for (int i = 0; i < 3; ++i)
                    r[i] = beam[i].IsIn(vcg::Point2i(p.V((i+1)%3), p.V((i+2)%3)), p.V(i));
                
                /* If some beams are unable to tell whether a point is inside or outside
                   (i.e. they return 0), try to make them consistent with other beams */
                if (r[0] == 0)
                    r[0] += r[1] + r[2];
                if (r[1] == 0)
                    r[1] += r[0] + r[1];
                if (r[2] == 0)
                    r[2] += r[2] + r[0];
                
                if (r[0]>0 && r[1]>0 && r[2]>0) /* consistent: inside -> inside */
                    return 1;
                else if ((r[0]<0 && r[1]<0 && r[2]<0) || /* consistent: outside -> outside */
                         (r[0]==0 && r[1]==0 && r[2] == 0)) /* "consistent": unknown -> outside */
                    return -1;
                
                /* If the rasterization algorithm generates consistent volumes, this should never happen */
                std::cerr << "Inconsistency: " << p3print(p) << p3print(delta) << std::endl;
                for (int i = 0; i < 3; ++i) {
                    std::cerr << beam[i].IsIn(vcg::Point2i(p.V((i+1)%3), p.V((i+2)%3)), p.V(i));
                    std::cerr << ": " << beam[i].GetInterceptRay(vcg::Point2i(p.V((i+1)%3), p.V((i+2)%3))) << std::endl;
                }

                return 0;
            }
            
            friend std::ostream& operator<<(std::ostream &out, const InterceptVolume &x) {
                out << "InterceptVolume[" << p3print(x.delta) << "][" << std::endl;
                int coord = 0;
                for(typename ContainerType::const_iterator iter = x.beam.begin(); iter != x.beam.end(); ++iter) {
                    out << *iter << std::endl;
                    out << "Beam " << coord << std::endl;
                    
                    for (int i=x.bbox.min[coord]; i<=x.bbox.max[coord]; i+=1) {
                        out << i << std::endl;
                        
                        for (int k=x.bbox.min[(coord+2)%3]; k<=x.bbox.max[(coord+2)%3]+2; k+=1)
                            out << '+';
                        out << std::endl;
                        
                        for (int j=x.bbox.min[(coord+1)%3]; j<=x.bbox.max[(coord+1)%3]; j+=1) {
                            out << '+';
                            for (int k=x.bbox.min[(coord+2)%3]; k<=x.bbox.max[(coord+2)%3]; k+=1) {
                                vcg::Point3i p(i,j,k);
                                int in = iter->IsInExt(vcg::Point2i(j, k), i);
                                char c = '?';
                                if (in < 0)
                                    c = ' ';
                                else if (in > 0)
                                    c = '#';
                                out << c;
                            }
                            out << '+' << std::endl;
                        }
                        
                        for (int k=x.bbox.min[(coord+2)%3]; k<x.bbox.max[(coord+2)%3]+2; k+=1)
                            out << '+';
                        out << std::endl;
                    }
                    coord++;
                }
                out << "]";
                
                return out;
            }
            
            const Point3x delta;
            vcg::Box3i bbox;
        private:
            ContainerType beam;
        };
        
        /* Unsorted version of InterceptRay.
           Used to temporarily accumulate the intersections along a line before sorting them */
        template <typename InterceptType>
                class InterceptSet
        {
            typedef std::vector<InterceptType> ContainerType;
            typedef InterceptRay<InterceptType> SortedType;
            
        public:
            inline InterceptSet() { }
            
            inline operator SortedType() const { return SortedType(v); }
            
            inline void AddIntercept(const InterceptType &x) { v.push_back(x); }
            
            friend std::ostream& operator<<(std::ostream &out, const InterceptSet &x) {
                typename ContainerType::const_iterator i, end = x.v.end();
                out << "InterceptSet[";
                for (i = x.v.begin(); i != end; ++i)
                    out << *i << std::endl;
                return out << "]";
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
            
            inline void AddIntercept(const int i, const InterceptType &x) {
                assert (i >= 0);
                assert (size_t(i) < set.size());
                set[i].AddIntercept(x);
            }
            
            friend std::ostream& operator<<(std::ostream &out, const InterceptSet1 &x) {
                typename ContainerType::const_iterator i, end = x.set.end();
                out << "InterceptSet1[";
                for (i = x.set.begin(); i != end; ++i)
                    out << *i << std::endl;
                return out << "]InterceptSet1";
            }
            
        private:
            ContainerType set;
        };
        
        /* Unsorted version of InterceptBeam.
           Used to temporarily accumulate the intersections along a family of
           parallel lines before sorting them */
        template <typename InterceptType>
                class InterceptSet2
        {
            typedef std::vector<InterceptSet1<InterceptType> > ContainerType;
            typedef std::vector<std::vector<InterceptSet<InterceptType> > > NewContainerType;
            typedef InterceptBeam<InterceptType> SortedType;
            
        public:
            inline InterceptSet2(const vcg::Box2i &box) : bbox(box), set(box.DimX() + 1) {
                typename ContainerType::iterator i, end = set.end();
                for (i = set.begin(); i != end; ++i)
                    i->resize(box.DimY() + 1);
            }
            
            inline operator SortedType() const { return SortedType(bbox, typename SortedType::ContainerType(set.begin(), set.end())); }
            
            inline void AddIntercept (const vcg::Point2i &p, const InterceptType &x) {
                assert (bbox.IsIn(p));
                vcg::Point2i c = p - bbox.min;
                assert (c.X() >= 0);
                assert (size_t(c.X()) < set.size());
                set[c.X()].AddIntercept(c.Y(), x);
            }
            
            friend std::ostream& operator<<(std::ostream &out, const InterceptSet2 &x) {
                typename ContainerType::const_iterator i, end = x.set.end();
                out << "InterceptSet2[";
                for (i = x.set.begin(); i != end; ++i)
                    out << *i << std::endl;
                return out << "]InterceptSet2";
            }
            
        private:
            Box2i bbox;
            ContainerType set;
        };
        
        /* Unsorted version of InterceptVolume.
           Used to temporarily accumulate the intersections in a volume before sorting them.
           Rasterization is performed on faces after casting them to an integral type, so that no
           numerical instability can cause the volume to be inconsistent */
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
                                    const vcg::Box3i &ibox, const Point3x &norm, const Scalar &quality)
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
                
                DistType n0xy = (v1[crd1]-ibox.min[crd1])*d21[crd2] - (v1[crd2]-ibox.min[crd2])*d21[crd1];
                DistType n1xy = (v2[crd1]-ibox.min[crd1])*d02[crd2] - (v2[crd2]-ibox.min[crd2])*d02[crd1];
                DistType n2xy = (v0[crd1]-ibox.min[crd1])*d10[crd2] - (v0[crd2]-ibox.min[crd2])*d10[crd1];
                const DistType n0dx = d21[crd2] + (ibox.Dim()[crd2] + 1) * d21[crd1];
                const DistType n1dx = d02[crd2] + (ibox.Dim()[crd2] + 1) * d02[crd1];
                const DistType n2dx = d10[crd2] + (ibox.Dim()[crd2] + 1) * d10[crd1];
                for(int x = ibox.min[crd1]; x <= ibox.max[crd1]; ++x) {
                    for(int y = ibox.min[crd2]; y <= ibox.max[crd2]; ++y) {
                        DistType n0 = n0xy, n1 = n1xy, n2 = n2xy;

                        /* Solve the inside/outside problem for on-edge points.
                           The point (x,y,z) is actually considered to be
                           (x+eps, y+eps^2, z+eps^2) with eps->0. */
                        if (crd1 > crd2) {
                            if (n0 == 0)
                                n0 = d21[crd1];
                            if (n0 == 0)
                                n0 -= d21[crd2];
                            
                            if (n1 == 0)
                                n1 = d02[crd1];
                            if (n1 == 0)
                                n1 -= d02[crd2];
                            
                            if (n2 == 0)
                                n2 = d10[crd1];
                            if (n2 == 0)
                                n2 -= d10[crd2];
                        } else {
                            if (n0 == 0)
                                n0 -= d21[crd2];
                            if (n0 == 0)
                                n0 = d21[crd1];
                            
                            if (n1 == 0)
                                n1 -= d02[crd2];
                            if (n1 == 0)
                                n1 = d02[crd1];
                            
                            if (n2 == 0)
                                n2 -= d10[crd2];
                            if (n2 == 0)
                                n2 = d10[crd1];
                        }
                        
                        if((n0>0 && n1>0 && n2>0) || (n0<0 && n1<0 && n2<0)) {
                            DistType d = (v0[crd2] - y) * det2 + (v0[crd1] - x) * det1;
                            d /= det0;
                            d += v0[crd0];
                            assert (d >= ibox.min[crd0] && d <= ibox.max[crd0]);
                            set[crd0].AddIntercept(vcg::Point2i(x, y), InterceptType(d, norm, norm[crd0], quality));
                        }
                        n0xy += d21[crd1];
                        n1xy += d02[crd1];
                        n2xy += d10[crd1];
                    }
                    n0xy -= n0dx;
                    n1xy -= n1dx;
                    n2xy -= n2dx;
                }
            }
            
            void ScanFace(const Point3dt &v0, const Point3dt &v1, const Point3dt &v2,
                          const Point3x &norm, const Scalar &quality) {
                vcg::Box3<DistType> fbox;
                fbox.Add(v0);
                fbox.Add(v1);
                fbox.Add(v2);
                for (int i=0; i<3; ++i) {
                    assert (v0[i] >= bbox.min[i] && v0[i] <= bbox.max[i]);
                    assert (v1[i] >= bbox.min[i] && v1[i] <= bbox.max[i]);
                    assert (v2[i] >= bbox.min[i] && v2[i] <= bbox.max[i]);
                }
                vcg::Box3i ibox(vcg::Point3i(floor(fbox.min.X()), floor(fbox.min.Y()), floor(fbox.min.Z())),
                                vcg::Point3i(ceil(fbox.max.X()), ceil(fbox.max.Y()), ceil(fbox.max.Z())));
                
                RasterFace<0>(v0, v1, v2, ibox, norm, quality);
                RasterFace<1>(v0, v1, v2, ibox, norm, quality);
                RasterFace<2>(v0, v1, v2, ibox, norm, quality);
            }
            
        public:
            template <class MeshType>
                    inline InterceptSet3(const MeshType &m, const Point3x &d, int subCellPrecision=32, vcg::CallBackPos *cb=vcg::DummyCallBackPos) : delta(d),
                    bbox(Point3i(floor(m.bbox.min.X() / d.X()) - 1,
                                 floor(m.bbox.min.Y() / d.Y()) - 1,
                                 floor(m.bbox.min.Z() / d.Z()) - 1),
                         Point3i(ceil(m.bbox.max.X() / d.X()) + 1,
                                 ceil(m.bbox.max.Y() / d.Y()) + 1,
                                 ceil(m.bbox.max.Z() / d.Z()) + 1))
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
                const size_t nFaces = m.face.size();
                size_t f = 0;
                for (i = m.face.begin(); i != end; ++i, ++f) {
                    if (!cb (100.0 * f / nFaces, "Rasterizing mesh...")) {
                        set.clear();
                        set.push_back(ISet2Type(yz));
                        set.push_back(ISet2Type(zx));
                        set.push_back(ISet2Type(xy));
                        return;
                    }
                    Point3x v0(i->V(0)->P()), v1(i->V(1)->P()), v2(i->V(2)->P());
                    v0.Scale(invDelta);
                    v1.Scale(invDelta);
                    v2.Scale(invDelta);
                    for (int j=0; j<3; ++j) {
                        assert (v0[j] >= bbox.min[j] && v0[j] <= bbox.max[j]);
                        assert (v1[j] >= bbox.min[j] && v1[j] <= bbox.max[j]);
                        assert (v2[j] >= bbox.min[j] && v2[j] <= bbox.max[j]);
                    }                    
                    ScanFace (Point3dt(makeFraction(v0.X()*subCellPrecision, subCellPrecision),
                                       makeFraction(v0.Y()*subCellPrecision, subCellPrecision),
                                       makeFraction(v0.Z()*subCellPrecision, subCellPrecision)),
                              Point3dt(makeFraction(v1.X()*subCellPrecision, subCellPrecision),
                                       makeFraction(v1.Y()*subCellPrecision, subCellPrecision),
                                       makeFraction(v1.Z()*subCellPrecision, subCellPrecision)),
                              Point3dt(makeFraction(v2.X()*subCellPrecision, subCellPrecision),
                                       makeFraction(v2.Y()*subCellPrecision, subCellPrecision),
                                       makeFraction(v2.Z()*subCellPrecision, subCellPrecision)),
                              i->cN().Normalize(),
                              i->cQ());
                }
            }
            
            inline operator SortedType() const { return SortedType(bbox, delta, typename SortedType::ContainerType(set.begin(), set.end())); }
            
            friend std::ostream& operator<<(std::ostream &out, const InterceptSet3<InterceptType> &x) {
                typename ContainerType::const_iterator i, end = x.set.end();
                out << "InterceptSet3[";
                for (i = x.set.begin(); i != end; ++i)
                    out << *i << std::endl;
                return out << "]InterceptSet3";
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
            typedef typename std::tr1::unordered_map<const InterceptType*,size_t> VertexTable;
            typedef typename std::tr1::unordered_map<vcg::Point3i,float> SamplesTable;
            typedef typename std::tr1::unordered_set<vcg::Point3i> CellsSet;

            void clear() {
                _vertices.clear();
                _samples.clear();
                _volume = NULL;
                _mesh = NULL;
            }

        public:
            template<typename EXTRACTOR_TYPE>
            void BuildMesh(MeshType &mesh, InterceptVolume<InterceptType> &volume, EXTRACTOR_TYPE &extractor, vcg::CallBackPos *cb=vcg::DummyCallBackPos)
            {
                CellsSet cset; /* cells to be visited */
                vcg::Point3i p;
                
                _volume = &volume;
                _mesh = &mesh;
                _mesh->Clear();
                
                /* To improve performance, instead of visiting the whole volume, mark the cells
                   intersecting the surface so that they can be visited.
                   This usually lowers the complexity from n^3 to about n^2 (where n is the
                   number of samples along each direction */
                for (int c0=0; c0 < 3; ++c0) {
                    const int c1 = (c0 + 1) % 3, c2 = (c0 + 2) % 3;
                    for (p[c1]=_volume->bbox.min.V(c1); p[c1]<=_volume->bbox.max.V(c1); ++p[c1])
                        for (p[c2]=_volume->bbox.min.V(c2); p[c2]<=_volume->bbox.max.V(c2); ++p[c2]) {
                        const InterceptRay<InterceptType>& r =
                                _volume->GetInterceptRay(c0, vcg::Point2i(p[c1],p[c2]));
                        typename InterceptRay<InterceptType>::ContainerType::const_iterator curr, end = r.container().end();
                        for (curr = r.container().begin(); curr != end; ++curr) {
                            p[c0] = floor(curr->dist());
                            if (curr->dist() == p[c0]) {
                                p[c0]--;
                                p[c1]--;
                                p[c2]--;
                                cset.insert(p);
                                p[c2]++;
                                cset.insert(p);
                                p[c1]++;
                                p[c2]--;
                                cset.insert(p);
                                p[c2]++;
                                cset.insert(p);
                                p[c0]++;
                            }
                            p[c1]--;
                            p[c2]--;
                            cset.insert(p);
                            p[c2]++;
                            cset.insert(p);
                            p[c1]++;
                            p[c2]--;
                            cset.insert(p);
                            p[c2]++;
                            cset.insert(p);
                        }
                    }
                }

                /* Evaluating IsIn is "slow" (it requires binary search, so it's slower than an hashtable
                   access) and vertices of the cells are often shared, so precomputing them in the
                   _samples hashmap causes a performance improvement */
                const size_t n = cset.size();
                size_t i = 0;
                for (CellsSet::const_iterator cell = cset.begin(); cell != cset.end(); ++cell, ++i) {
                    if (!cb(100.0 * i / n, "Precomputing in/out table...")) {
                        clear();
                        return;
                    }
                    for (int i = 0; i < 2; ++i)
                        for (int j = 0; j < 2; ++j)
                            for (int k = 0; k < 2; ++k) {
                        vcg::Point3i p(*cell + vcg::Point3i(i, j, k));
                        if (_samples.find(p) == _samples.end())
                            _samples[p] = _volume->IsIn(p);
                    }
                }

                const vcg::Point3i diag(1, 1, 1);
                extractor.Initialize();
                i = 0;
                /* Only visit marked cells */
                for (CellsSet::const_iterator cell = cset.begin(); cell != cset.end(); ++cell, ++i) {
                    if (!cb(100.0 * i / n, "Reconstructing surface..."))
                        break;
                    extractor.ProcessCell(*cell, *cell + diag);
                }
                extractor.Finalize();

                clear();
            }
            
            inline float V(int i, int j, int k) const { return V(vcg::Point3i(i, j, k)); }

            inline float V(const vcg::Point3i &p) const {
                /* use precomputed in/out result */
                typename SamplesTable::const_iterator s = _samples.find(p);
                return s->second;
            }

            template <const int coord>
                    void GetIntercept(const vcg::Point3i &p1, const vcg::Point3i &p2, VertexPointer& p) {
                assert (p2 == p1 + vcg::Point3i(coord == 0, coord == 1, coord == 2));
                assert (_volume->IsIn(p1) != _volume->IsIn(p2));
                
                const InterceptType& i = _volume->GetIntercept<coord>(p1);
                typename VertexTable::const_iterator v = _vertices.find(&i);
                if (v == _vertices.end()) {
                    p = &*vcg::tri::Allocator<MeshType>::AddVertices(*_mesh, 1);
                    p->P().V(coord) = toFloat(i.dist());
                    p->P().V((coord+1)%3) = p1[(coord+1)%3];
                    p->P().V((coord+2)%3) = p1[(coord+2)%3];
                    p->P().Scale(_volume->delta);
                    p->N() = i.norm();
                    p->Q() = i.quality();
                    _vertices[&i] = p - &_mesh->vert[0];
                } else /* a vertex is already associated with the intercept. reuse it */
                    p = &_mesh->vert[v->second];
            }
            
            inline void GetXIntercept(const vcg::Point3i &p1, const vcg::Point3i &p2, VertexPointer& p) { GetIntercept<0>(p1, p2, p); }
            
            inline void GetYIntercept(const vcg::Point3i &p1, const vcg::Point3i &p2, VertexPointer& p) { GetIntercept<1>(p1, p2, p); }
            
            inline void GetZIntercept(const vcg::Point3i &p1, const vcg::Point3i &p2, VertexPointer& p) { GetIntercept<2>(p1, p2, p); }
            
            bool Exist(const vcg::Point3i &p1, const vcg::Point3i &p2, VertexPointer& p) {
                if (V(p1) == V(p2))
                    return false;
                
                vcg::Point3i d = p2 - p1;
                if (d.X())
                    GetXIntercept(p1, p2, p);
                else if (d.Y())
                    GetYIntercept(p1, p2, p);
                else if (d.Z())
                    GetZIntercept(p1, p2, p);
                
                return true;
            }
            
        private:
            VertexTable _vertices; /* maps intercept -> vertex of the reconstructed mesh */
            SamplesTable _samples; /* maps point -> in/out */
            InterceptVolume<InterceptType> *_volume;
            MeshType *_mesh;
        };
    };

    template <typename MeshType, typename StringType>
            bool isValid(MeshType &mesh, StringType &errorMessage) {
        if (vcg::tri::Clean<MeshType>::CountNonManifoldEdgeFF(mesh) > 0)
            errorMessage = "non manifold edges";
        else if (vcg::tri::Clean<MeshType>::CountNonManifoldVertexFF(mesh) > 0)
            errorMessage = "non manifold vertices";
        else if (!vcg::tri::Clean<MeshType>::IsSizeConsistent(mesh))
            errorMessage = "non size-consistent mesh";
        else {
            int boundaryEdgeNum, internalEdgeNum;
            vcg::tri::Clean<MeshType>::CountEdges(mesh, internalEdgeNum, boundaryEdgeNum);
            if (boundaryEdgeNum > 0)
                errorMessage = "non watertight mesh";
            else
                return true;
        }
        return false;
    }
};
#endif // INTERCEPT_H
