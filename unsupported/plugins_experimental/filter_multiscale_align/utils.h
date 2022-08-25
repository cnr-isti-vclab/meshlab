#ifndef UTILS_H
#define UTILS_H

#include <common/meshmodel.h>

#include <math.h>
//#include "localKdTree.h"

//! Compile time pow
template<typename base_t, typename expo_t>
base_t POW(base_t base, expo_t expo)
{
    return (expo != 0 )? base * POW(base, expo -1) : 1;
}



template<class Point>
inline int
UnrollIndexLoop(const Point& coord, int cdim, int gsize){
  return (cdim != 0)
    ? ( int(std::floor(coord[cdim]))*POW(gsize, cdim) +
        UnrollIndexLoop(coord, cdim-1, gsize) )
    : int(std::floor(coord[cdim]));
}

namespace Utils{

//// This creates the list of the scales, given the min, the max and the number of steps
template <typename Scalar, class Container>
inline
void sampleScaleInterval(
        Scalar minScale,
        Scalar /*coarseScale*/,
        Scalar multiplier,
        unsigned int nbScales,
        Container& scales){
    scales.reserve(nbScales);
    scales.push_back(minScale);
    for(unsigned int i = 1; i != nbScales; i++)
        scales.push_back(scales[i-1] * multiplier);
}

///// Calculate the angle between two normal vectors
template <typename Scalar, class vec>
inline
Scalar angleNorm(vec aN, vec rN)
{
    aN=aN.Normalize();
    rN=rN.Normalize();
    return acos(aN.X()*rN.X()+aN.Y()*rN.Y()+aN.Z()*rN.Z())*180.0/M_PI;
}


//////////////////////////////////////////////////////////
// This function creates a KNNtree for each "selected" point, storing the n nearest neighbors
///////////////////////////////////////////////////////////////


//template <typename Scalar>
//inline
//MultiscaleAlignNamespace::KdTree<Scalar>*
//makeKNNTree(MeshModel* model){
//
//    MultiscaleAlignNamespace::KdTree<Scalar>* tree =
//            new MultiscaleAlignNamespace::KdTree<Scalar> (model->cm.vert.size());
//
//    int i=0;
//    for (CMeshO::VertexIterator vi = model->cm.vert.begin();
//         vi != model->cm.vert.end();
//         vi++, ++i) {
//        tree->set(i, vi->cP());
//    }
//
//    tree->finalize();
//
//    return tree;
//}


namespace internal{
// Compute the closest points between two 3D line segments and obtain the two
// invariants corresponding to the closet points. This is the "intersection"
// point that determines the invariants. Since the 4 points are not exactly
// planar, we use the center of the line segment connecting the two closest
// points as the "intersection".
template <class Point>
inline
float _distSegmentToSegment(const Point& u,
                            const Point& v,
                            const Point& w,
                            float& invariant1,
                            float& invariant2) {
    const float kSmallNumber = 0.0000001;
    float a = u.dot(u);
    float b = u.dot(v);
    float c = v.dot(v);
    float d = u.dot(w);
    float e = v.dot(w);
    float f = a * c - b * b;
    // s1,s2 and t1,t2 are the parametric representation of the intersection.
    // they will be the invariants at the end of this simple computation.
    float s1 = 0.0;
    float s2 = f;
    float t1 = 0.0;
    float t2 = f;

    if (f < kSmallNumber) {
        s1 = 0.0;
        s2 = 1.0;
        t1 = e;
        t2 = c;
    } else {
        s1 = (b * e - c * d);
        t1 = (a * e - b * d);
        if (s1 < 0.0) {
            s1 = 0.0;
            t1 = e;
            t2 = c;
        } else if (s1 > s2) {
            s1 = s2;
            t1 = e + b;
            t2 = c;
        }
    }

    if (t1 < 0.0) {
        t1 = 0.0;
        if (-d < 0.0)
            s1 = 0.0;
        else if (-d > a)
            s1 = s2;
        else {
            s1 = -d;
            s2 = a;
        }
    } else if (t1 > t2) {
        t1 = t2;
        if ((-d + b) < 0.0)
            s1 = 0;
        else if ((-d + b) > a)
            s1 = s2;
        else {
            s1 = (-d + b);
            s2 = a;
        }
    }
    invariant1 = (abs(s1) < kSmallNumber ? 0.0 : s1 / s2);
    invariant2 = (abs(t1) < kSmallNumber ? 0.0 : t1 / t2);
    return (w + (invariant1*u) - (invariant2*v)).norm();
}
} // namespace internal


// Compute the closest points between two 3D line segments and obtain the two
// invariants corresponding to the closet points. This is the "intersection"
// point that determines the invariants. Since the 4 points are not exactly
// planar, we use the center of the line segment connecting the two closest
// points as the "intersection".
inline
float distSegmentToSegment(const vcg::Point3f& p1, const vcg::Point3f& p2,
                           const vcg::Point3f& q1, const vcg::Point3f& q2,
                           float& invariant1, float& invariant2) {
    Eigen::Vector3f u, v, w;

    (p2 - p1).ToEigenVector(u);
    (q2 - q1).ToEigenVector(v);
    (p1 - q1).ToEigenVector(w);

    return internal::_distSegmentToSegment(u, v, w, invariant1, invariant2);
}


// Compute the closest points between two 3D line segments and obtain the two
// invariants corresponding to the closet points. This is the "intersection"
// point that determines the invariants. Since the 4 points are not exactly
// planar, we use the center of the line segment connecting the two closest
// points as the "intersection".
inline
float distSegmentToSegment(const Eigen::Vector3f& p1, const Eigen::Vector3f& p2,
                           const Eigen::Vector3f& q1, const Eigen::Vector3f& q2,
                           float& invariant1, float& invariant2) {
    return internal::_distSegmentToSegment((p2 - p1).eval(),
                                           (q2 - q1).eval(),
                                           (p1 - q1).eval(),
                                           invariant1,
                                           invariant2);
}

} // namespace Utils

#endif // UTILS_H
