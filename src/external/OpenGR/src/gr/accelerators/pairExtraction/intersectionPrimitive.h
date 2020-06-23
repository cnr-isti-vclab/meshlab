// Copyright 2014 Nicolas Mellado
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// -------------------------------------------------------------------------- //
//
// Authors: Nicolas Mellado
//
// An implementation of the Super 4-points Congruent Sets (Super 4PCS)
// algorithm presented in:
//
// Super 4PCS: Fast Global Pointcloud Registration via Smart Indexing
// Nicolas Mellado, Dror Aiger, Niloy J. Mitra
// Symposium on Geometry Processing 2014.
//
// Data acquisition in large-scale scenes regularly involves accumulating
// information across multiple scans. A common approach is to locally align scan
// pairs using Iterative Closest Point (ICP) algorithm (or its variants), but
// requires static scenes and small motion between scan pairs. This prevents
// accumulating data across multiple scan sessions and/or different acquisition
// modalities (e.g., stereo, depth scans). Alternatively, one can use a global
// registration algorithm allowing scans to be in arbitrary initial poses. The
// state-of-the-art global registration algorithm, 4PCS, however has a quadratic
// time complexity in the number of data points. This vastly limits its
// applicability to acquisition of large environments. We present Super 4PCS for
// global pointcloud registration that is optimal, i.e., runs in linear time (in
// the number of data points) and is also output sensitive in the complexity of
// the alignment problem based on the (unknown) overlap across scan pairs.
// Technically, we map the algorithm as an ‘instance problem’ and solve it
// efficiently using a smart indexing data organization. The algorithm is
// simple, memory-efficient, and fast. We demonstrate that Super 4PCS results in
// significant speedup over alternative approaches and allows unstructured
// efficient acquisition of scenes at scales previously not possible. Complete
// source code and datasets are available for research use at
// http://geometry.cs.ucl.ac.uk/projects/2014/super4PCS/.


#ifndef _OPENGR_ACCELERATORS_INTERSECTION_PRIMITIVES_H_
#define _OPENGR_ACCELERATORS_INTERSECTION_PRIMITIVES_H_

#ifndef SQR
#define SQR(a)		((a)*(a))
#endif

#include <Eigen/Core>

namespace gr{
namespace Accelerators{
namespace PairExtraction{

template <class Point, int _dim, typename Scalar>
class HyperSphere{
private:
  const Point _center;
  Scalar _radius;


  //! \brief UnaryExpr to apply floor with arbitrary quantification step e
  struct CustomFloorExpr{
    inline CustomFloorExpr(Scalar e) : m_e(e) {}
    inline const Scalar operator()(const Scalar &x) const
    {
      return std::round(x/m_e) * m_e;
    }

    Scalar m_e;
  };

public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  enum { Dim = _dim};

  inline HyperSphere(const Point& center, Scalar radius)
    : _center(center), _radius(radius) { }

  inline HyperSphere(const HyperSphere<Point, _dim, Scalar>& other)
    : _center(other._center), _radius(other._radius) { }

  //! \brief Construct a copy of the instance with a quantified radius and pos
  inline HyperSphere<Point, _dim, Scalar> quantified ( Scalar eps ) const
  {
    CustomFloorExpr expr (eps);
    return HyperSphere<Point, _dim, Scalar>(_center.unaryExpr(expr),
                                            expr(_radius));
  }

  //! \brief Comparison operator comparing first the radius then the position
  inline bool operator<(const HyperSphere<Point, _dim, Scalar>& other) const{
    if (_radius != other._radius)
      return _radius < other._radius;
    for( int i = 0; i < Dim; i++ ) {
      if (_center[i] == other._center[i]) continue;
      if (_center[i] < other._center[i]) return true;
      return false;
    }
    return false;
  }

  //! Implicit conversion operator to Eigen vectors
  inline operator Point() const { return _center; }

  /*!
   * \implements Arvo, James,
   *             A Simple Method for Box-Sphere Intersection Testing,
   *             Graphics Gems, p. 335-339, code: p. 730-732, BoxSphere.c.
   */
  inline bool intersect( const Point& nodeCenter, Scalar halfEdgeLength) const
  {
    const Point min = nodeCenter.array() - halfEdgeLength;
    const Point max = nodeCenter.array() + halfEdgeLength;
    const Point sqmin = (_center.array() - min.array()).square();
    const Point sqmax = (_center.array() - max.array()).square();

    // The computation of dmin below is equivalent to:
    //for( int i = 0; i < Dim; i++ ) {
      //if( _center[i] < min[i] ) dmin += sqmin[i]; else
      //if( _center[i] > max[i] ) dmin += sqmax[i];
    //}

    Scalar dmin = (_center.array() < min.array())
           .select(
              sqmin,
              (_center.array() > max.array()).select(sqmax,Point::Zero())
           ).sum();

    return ( dmin < _radius*_radius &&
             _radius*_radius < sqmin.cwiseMax(sqmax).sum() );
  }

  /*!
   * \brief intersectFast Fast but inacurate intersection test
   * \param nodeCenter
   * \param halfEdgeLength
   * \return
   *
   * Check if the node center is inside the Hypersphere (radius grown by
   * halfEdgeLength.
   */
  inline bool intersectFast( const Point& nodeCenter, Scalar halfEdgeLength) const
  {
    return SQR((nodeCenter-_center).norm()-halfEdgeLength) <= _radius;
  }

  inline bool intersectPoint( const Point& pos, Scalar epsilon ) const
  {
    return SQR((pos - _center).norm()-_radius) < SQR(epsilon);
  }

  static inline bool intersectPoint( const Point& pos, Scalar epsilon,
                                     const Point& center, const Scalar &radius )
  {
    return SQR((pos - center).norm()-radius) < SQR(epsilon);
  }

  inline const Point& center() const {return _center;}
  inline const Scalar & radius() const {return _radius;}
  inline Scalar& radius() { return _radius; }
};

} // namespace Accelerators
} // namespace PairExtraction
} // namespace Super4PCS

#endif

