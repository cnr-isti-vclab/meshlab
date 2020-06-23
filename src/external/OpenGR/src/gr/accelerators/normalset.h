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
// Technically, we map the algorithm as an 'instance problem' and solve it
// efficiently using a smart indexing data organization. The algorithm is
// simple, memory-efficient, and fast. We demonstrate that Super 4PCS results in
// significant speedup over alternative approaches and allows unstructured
// efficient acquisition of scenes at scales previously not possible. Complete
// source code and datasets are available for research use at
// http://geometry.cs.ucl.ac.uk/projects/2014/super4PCS/.


#ifndef _OPENGR_ACCELERATORS_INDEXED_NORMAL_SET_H_
#define _OPENGR_ACCELERATORS_INDEXED_NORMAL_SET_H_

#include "gr/utils/disablewarnings.h"
#include "gr/accelerators/utils.h"

namespace gr{

/*!
  Normal set indexed by a position in euclidean space.
  The size used to hash euclidean coordinates is defined at runtime.
  The size used to hash the normals in an euclidean cell is defined at compile
  time

  Loops over dimensions used to compute index values are unrolled at compile
  time.
 */
template <
  class Point,      //! <\brief Type of point to work with
  int dim,          //! <\brief Number of dimension in ambient space
  int _ngSize,      //! <\brief Normal grid size in 1 dimension
  typename _Scalar   //! <\brief Scalar type
  >
struct IndexedNormalSet{
  typedef std::array< std::vector<unsigned int>,
                      Utils::POW(_ngSize, dim)> AngularGrid;

  enum MOVE_DIR { POSITIVE, NEGATIVE };
  using Scalar    = _Scalar;
  using NeiIdsBox = typename gr::Utils::OneRingNeighborhood::NeighborhoodType<dim>::type;

#ifdef DEBUG
#define VALIDATE_INDICES true
#pragma message "Index validation is enabled in debug mode"
#else
#define VALIDATE_INDICES false
#endif

private:
  const Scalar _nepsilon;
  std::vector<AngularGrid*> _grid;
  Scalar _epsilon;
  int _egSize;    //! <\brief Size of the euclidean grid for each dimension

  /// Get the index corresponding to position p \warning Bounds are not tested
  inline int indexPos   ( const Point& p) const;
  /// Get the index corresponding to normal n   \warning Bounds are not tested
  inline int indexNormal( const Point& n) const;

  /// Get the coordinates corresponding to position p \warning Bounds are not tested
  inline Point coordinatesPos   ( const Point& p) const
  { return p/_epsilon;  }
  /// Get the index corresponding to normal n   \warning Bounds are not tested
  inline Point coordinatesNormal( const Point& n) const
  {
    static const Point half = Point::Ones()/Scalar(2.);
    return (n/Scalar(2.) + half)/_nepsilon;
  }

  /// Get the coordinates corresponding to position p \warning Bounds are not tested
  inline int indexCoordinatesPos   ( const Point& pCoord) const;
  /// Get the index corresponding to normal n   \warning Bounds are not tested
  inline int indexCoordinatesNormal( const Point& nCoord) const;

  //inline Point indexToPos (int id) const;

public:
  inline IndexedNormalSet(const Scalar epsilon)
    : _nepsilon(Scalar(1.)/Scalar(_ngSize) + 0.00001),
      _epsilon(epsilon)
  {
    /// We need to check if epsilon is a power of two and correct it if needed
    const int gridDepth = -std::log2(epsilon);
    _egSize = std::pow(2,gridDepth);
    _epsilon = 1.f/_egSize;

    _grid = std::vector<AngularGrid*> (std::pow(_egSize, dim), NULL);
  }

  virtual inline ~IndexedNormalSet();

  //! \brief Add a new couple pos/normal, and its associated id
  inline bool addElement(const Point& pos,
                         const Point& normal,
                         unsigned int id);

  //! \return NULL if the grid does not exist or p is out of bound
  inline AngularGrid* angularGrid(const Point& p) {
    const int pId = indexPos(p);
    if (pId == -1) return NULL;
    return _grid[pId];
  }

  //! \return the Angular Grid contained in p cell + its n*n neighb
  inline std::vector<AngularGrid*> angularGrids(const Point&p) {
    std::vector<AngularGrid*> buf;
    const int pId = indexPos(p);
    if (pId != -1) {
        gr::Utils::OneRingNeighborhood neiFun;
        NeiIdsBox arr;
        neiFun.get<dim>( pId, _egSize, arr );
        buf.reserve(arr.size());
        for (auto& id : arr){
            if ( id != -1 && _grid[id] != nullptr){
                buf.push_back(_grid[id]);
              }
          }
    }
    return buf;
  }

  //! Get closest points in euclidean space
  inline void getNeighbors( const Point& p,
                            std::vector<unsigned int>&nei);
  //! Get closest points in euclidean an normal space
  inline void getNeighbors( const Point& p,
                            const Point& n,
                            std::vector<unsigned int>&nei);
  //! Get closest poitns in euclidean an normal space with angular deviation
  inline void getNeighbors( const Point& p,
                            const Point& n,
                            Scalar alpha,
                            std::vector<unsigned int>&nei,
                            bool tryReverse = false);
};

} // namespace Super4PCS

#include "gr/accelerators/normalset.hpp"

#endif // _INDEXED_NORMAL_SET_H_
