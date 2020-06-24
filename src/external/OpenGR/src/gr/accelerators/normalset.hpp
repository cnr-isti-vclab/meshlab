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


#ifndef _OPENGR_ACCELERATORS_INDEXED_NORMAL_SET_HPP_
#define _OPENGR_ACCELERATORS_INDEXED_NORMAL_SET_HPP_

#include <math.h>
#include <set>
#include <Eigen/Geometry>
#include <gr/accelerators/utils.h>

namespace gr{

template <class Point, int dim, int _ngSize, typename Scalar>
IndexedNormalSet<Point, dim, _ngSize, Scalar>::~IndexedNormalSet(){
  for(unsigned int i = 0; i != _grid.size(); i++)
    delete(_grid[i]);
}

/*!
 \return Cell id corresponding to p in the euclidean grid
 \warning p must be normalized between 0 and 1
 */
template <class Point, int dim, int _ngSize, typename Scalar>
int
IndexedNormalSet<Point, dim, _ngSize, Scalar>::indexPos(
  const Point& p) const
{
  // Unroll the loop over the different dimensions at compile time
  return Utils::UnrollIndexLoop<VALIDATE_INDICES>( coordinatesPos(p),  dim-1,  _egSize );
}

/*!
 \return Cell id corresponding to p in the euclidean grid
 \warning p must be normalized between 0 and 1
 */
template <class Point, int dim, int _ngSize, typename Scalar>
int
IndexedNormalSet<Point, dim, _ngSize, Scalar>::indexNormal(
  const Point& n) const
{
  // Unroll the loop over the different dimension at compile time
  return Utils::UnrollIndexLoop<VALIDATE_INDICES>( coordinatesNormal(n), dim-1, _ngSize);
}


template <class Point, int dim, int _ngSize, typename Scalar>
int
IndexedNormalSet<Point, dim, _ngSize, Scalar>::indexCoordinatesPos(
  const Point& pCoord) const
{
  // Unroll the loop over the different dimensions at compile time
  return Utils::UnrollIndexLoop<VALIDATE_INDICES>( pCoord,  dim-1,  _egSize );
}


template <class Point, int dim, int _ngSize, typename Scalar>
int
IndexedNormalSet<Point, dim, _ngSize, Scalar>::indexCoordinatesNormal(
  const Point& nCoord) const
{
  // Unroll the loop over the different dimension at compile time
  return Utils::UnrollIndexLoop<VALIDATE_INDICES>( nCoord,  dim-1, _ngSize);
}


template <class Point, int dim, int _ngSize, typename Scalar>
bool
IndexedNormalSet<Point, dim, _ngSize, Scalar>::addElement(
  const Point& p,
  const Point& n,
  unsigned int id)
{
  const int pId = indexPos(p);
  if (pId == -1) return false;

  const int nId = indexNormal(n);
  if (nId == -1) return false;

  gr::Utils::OneRingNeighborhood neiFun;
  using neiArray = typename gr::Utils::OneRingNeighborhood::NeighborhoodType<dim>::type;
  neiArray arr;
  neiFun.get<dim>( pId, _egSize, arr );

  for (auto& gid : arr){
    if (gid != -1) {
      if (_grid[gid] == NULL) _grid[gid] = new AngularGrid;
        (_grid[gid])->at(nId).push_back(id);
      }
  }
//  if (_grid[pId] == NULL) _grid[pId] = new AngularGrid;
//  (_grid[pId])->at(nId).push_back(id);


  return true;
}


template <class Point, int dim, int _ngSize, typename Scalar>
void
IndexedNormalSet<Point, dim, _ngSize, Scalar>::getNeighbors(
  const Point& p,
  std::vector<unsigned int>&nei)
{
  AngularGrid* grid = angularGrid(p);
  if ( grid == NULL ) return;

  for(typename AngularGrid::const_iterator it = grid->cbegin();
      it != grid->cend(); it++){
    const std::vector<unsigned int>& lnei = *it;
    nei.insert( nei.end(), lnei.begin(), lnei.end() );
  }
}


template <class Point, int dim, int _ngSize, typename Scalar>
void
IndexedNormalSet<Point, dim, _ngSize, Scalar>::getNeighbors(
  const Point& p,
  const Point& n,
  std::vector<unsigned int>&nei)
{
  AngularGrid* grid = angularGrid(p);
  if ( grid == NULL ) return;

  const std::vector<unsigned int>& lnei = grid->at(indexNormal(n));
  nei.insert( nei.end(), lnei.begin(), lnei.end() );
}


template <class Point, int dim, int _ngSize, typename Scalar>
void
IndexedNormalSet<Point, dim, _ngSize, Scalar>::getNeighbors(
  const Point& p,
  const Point& n,
  Scalar cosAlpha,
  std::vector<unsigned int>&nei,
  bool tryReverse)
{
  // FIXME_REFACTORING
 //F    for(const auto&lnei : (*grid))
 //F      nei.insert( nei.end(), lnei.begin(), lnei.end() );
 //F    }
 //F  return;

  AngularGrid* grid = angularGrid(p);
  if ( grid == NULL ) return;
  {
  // END_FIXME_REFACTORING

    ////////////// TESTS
//    gr::Utils::OneRingNeighborhood neiFun;
//    static const int dddim = 3;
//    using neiArray = typename gr::Utils::OneRingNeighborhood::NeighborhoodType<dddim>::type;
//    neiArray arr;
//    neiFun.get<dddim>( 3, 4, arr );
//    for (const auto&e : arr) std::cout << e << " ";
//    std::cout << "\n";

    ////////////// TESTS END

  const Scalar alpha          = std::acos(cosAlpha);
  const Scalar perimeter      = Scalar(2) * M_PI * std::atan(alpha);
  const unsigned int nbSample = 2*std::ceil(perimeter*Scalar(_ngSize) /Scalar(2.));
  const Scalar angleStep      = Scalar(2) * M_PI / Scalar(nbSample);

  const Scalar sinAlpha       = std::sin(alpha);

  Eigen::Quaternion<Scalar> q;
  q.setFromTwoVectors(Point(0.,0.,1.), n);

  std::set<unsigned int> colored;
//  for (AngularGrid* grid: angularGrids(p)) {//_grid){
//  for (AngularGrid* grid: _grid){
//    if (grid == NULL) continue;

  // Do the rendering independently of the content
  for(unsigned int a = 0; a != nbSample; a++){
    Scalar theta    = Scalar(a) * angleStep;
    const Point dir = ( q * Point(sinAlpha*std::cos(theta),
                              sinAlpha*std::sin(theta),
                              cosAlpha ) ).normalized();
    int id = indexNormal( dir );
    if(grid->at(id).size() != 0){
      colored.insert(id);
    }

    if (tryReverse){
      id = indexNormal( -dir );
      if(grid->at(id).size() != 0){
        colored.insert(id);
      }
    }
  }

  for( std::set<unsigned int>::const_iterator it = colored.cbegin();
       it != colored.cend(); it++){
    const std::vector<unsigned int>& lnei = grid->at(*it);
    nei.insert( nei.end(), lnei.begin(), lnei.end() );
  }
    } //F
}

} // namespace Super4CS


#endif // _INDEXED_NORMAL_SET_HPP_

