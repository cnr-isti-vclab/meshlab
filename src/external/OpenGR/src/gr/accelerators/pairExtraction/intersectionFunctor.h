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


#ifndef _OPENGR_ACCELERATORS_INTERSECTION_FUNCTOR_H_
#define _OPENGR_ACCELERATORS_INTERSECTION_FUNCTOR_H_

#include "gr/accelerators/pairExtraction/intersectionNode.h"
#include <list>
#include <iostream>

namespace gr{
namespace Accelerators{
namespace PairExtraction{

template <typename Scalar>
static Scalar GetRoundedEpsilonValue(Scalar epsilon, int* lvl = nullptr) {
    const int lvlMax = -std::log2(epsilon); //!< Maximum level

    if (lvl != nullptr) *lvl = lvlMax;

    // Refine epsilon by the closest conservative values
    return 1.f/pow(2,lvlMax);
}

//! \brief Extract pairs of points by rasterizing primitives and collect points
/*!
 * Acceleration technique used in Super4PCS
 * \todo Use Traits to allow custom parameters but similar API between variants
 * \see BruteForceFunctor
 */
template <class _Primitive, class _Point, int _dim, typename _Scalar>
struct IntersectionFunctor{
  typedef _Point Point;
  typedef _Primitive Primitive;
  typedef _Scalar Scalar;
  enum { dim = _dim };

  template <class PrimitiveContainer,
            class PointContainer,
            class ProcessingFunctor> //!< Process the extracted pairs
  void
  process(
    const PrimitiveContainer& M, //!< Input primitives to intersect with Q
    const PointContainer    & Q, //!< Normalized innput point set \in [0:1]^d
    Scalar &epsilon,              //!< Intersection accuracy, refined
    unsigned int minNodeSize,    //!< Min number of points in nodes
    ProcessingFunctor& functor
  );

};


/*!
   \return Pairs< PointId, PrimitiveId>
 */
template <class Primitive, class Point, int dim, typename Scalar>
template <class PrimitiveContainer,
          class PointContainer,
          class ProcessingFunctor>
void
IntersectionFunctor<Primitive, Point, dim, Scalar>::process(
    const PrimitiveContainer& M, //!< Input primitives to intersect with Q
    const PointContainer    & Q, //!< Normalized innput point set \in [0:1]^d
    Scalar &epsilon,              //!< Intersection accuracy in [0:1]
    unsigned int minNodeSize,    //!< Min number of points in nodes
    ProcessingFunctor& functor
    )
{
  using std::pow;

  // types definitions
  typedef NdNode<Point, dim, Scalar, PointContainer> Node;
  typedef typename std::vector<Node> NodeContainer;

  typedef typename Node::IdContainer IdContainer;
  typedef typename std::pair<unsigned int, unsigned int> ResPair;
  typedef typename std::vector<ResPair> ResContainer;

  // Global variables
  const unsigned int nbPoint = Q.size();    //!< Number of points
  int lvlMax = 0;
  epsilon = GetRoundedEpsilonValue(epsilon, &lvlMax);

  int clvl                   = 0;           //!< Current level

  // Use local array and manipulate references to avoid array copies
  NodeContainer ping, pong;
  NodeContainer* nodes      = &ping; //!< Nodes of the current level
  NodeContainer* childNodes = &pong; //!< Child nodes for the next level

  //! Nodes too small for split
  std::vector< std::pair<Node, Scalar> > earlyNodes;
//
//  // Fill the idContainer with identity values
  if (functor.ids.size() != nbPoint){
    std::cout << "[IntersectionFunctor] Init id array" << std::endl;
    functor.ids.clear();
    for(unsigned int i = 0; i < nbPoint; i++)
      functor.ids.push_back(i);
  }


  // Buid root node in the child node, will be copied to the current nodes
  childNodes->push_back(Node::buildUnitRootNode(Q, functor.ids));

  Scalar edgeLength = 0.f;
  Scalar edgeHalfLength = 0.f;

  // First Loop
  while (clvl != lvlMax-1){
    // Stop if we not have any nodes to checks
    if (childNodes->empty())
      break;

    edgeLength     = Scalar(1.f)/pow(2, clvl);
    edgeHalfLength = edgeLength/Scalar(2.f);

    // swap pointers
    std::swap(nodes, childNodes);
    childNodes->clear();

//#pragma omp parallel
    for(typename NodeContainer::iterator nit  = nodes->begin();
                                               nit != nodes->end(); nit++){
      Node &n = *nit;

      // Check if the current node intersect one of the primitives
      // In this case, subdivide, store new nodes and stop the loop
      for(typename PrimitiveContainer::const_iterator pit = M.begin();
          pit != M.end(); pit++){

        if ((*pit).intersect(n.center(), edgeHalfLength+epsilon)){
          // There is two options now: either there is already few points in the
          // current node, in that case we stop splitting it, or we split.
          if (n.rangeLength() > int(minNodeSize)){
//#pragma omp critical
            n.split(*childNodes, edgeHalfLength);
          }else{
//#pragma omp critical
            earlyNodes.emplace_back(n, edgeHalfLength+epsilon);
          }
          break;
        }
      }
    }
    clvl++;
  }

  // Second Loop
  ResContainer results;
  results.reserve(childNodes->size());

  unsigned int pId = 0;
  for(typename PrimitiveContainer::const_iterator itP = M.begin();
      itP != M.end(); itP++, pId++){
    // add childs
    for(typename NodeContainer::const_iterator itN = childNodes->begin();
        itN != childNodes->end(); itN++){
      if ((*itP).intersect((*itN).center(), epsilon*2.f)){

        functor.beginPrimitiveCollect(pId);
        for(unsigned int j = 0; j!= (*itN).rangeLength(); j++){
          if(pId>(*itN).idInRange(j))
            if((*itP).intersectPoint((*itN).pointInRange(j),epsilon))
              functor.process(pId, (*itN).idInRange(j));
        }
        functor.endPrimitiveCollect(pId);
      }
    }

    // add other leafs
    for(typename std::vector< std::pair<Node, Scalar> >::const_iterator itPairs =
                   earlyNodes.begin();
        itPairs != earlyNodes.end();
        itPairs++){
      if((*itP).intersect((*itPairs).first.center(), (*itPairs).second)){

        // Notice the functor we are collecting points for the current primitive
        functor.beginPrimitiveCollect(pId);
        for(unsigned int j = 0; j!= (*itPairs).first.rangeLength(); j++){
          if(pId>(*itPairs).first.idInRange(j))
            if((*itP).intersectPoint((*itPairs).first.pointInRange(j),epsilon))
              functor.process(pId, (*itPairs).first.idInRange(j));

        }
        functor.endPrimitiveCollect(pId);
      }
    }
  }
}

} // namespace PairExtraction
} // namespace Accelerators
} // namespace Super4PCS

#endif

