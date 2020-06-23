// Copyright 2014 Gael Guennebaud, Nicolas Mellado
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
// Authors: Gael Guennebaud, Nicolas Mellado
//
// Part of the implementation of the Super 4-points Congruent Sets (Super 4PCS)
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

#ifndef _OPENGR_ACCELERATORS_KDTREE_H
#define _OPENGR_ACCELERATORS_KDTREE_H

#include "gr/utils/disablewarnings.h"

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <limits>
#include <iostream>
#include <numeric>  //iota

// max depth of the tree
#define KD_MAX_DEPTH 32

// number of neighbors
#define KD_POINT_PER_CELL 64


namespace gr{

/*!
  \brief 3D Kdtree with reentrant queries
  */
template<typename _Scalar, typename _Index = int >
class KdTree
{
public:
    struct KdNode
    {
        union {
            struct {
                float splitValue;
                unsigned int firstChildId:24;
                unsigned int dim:2;
                unsigned int leaf:1;
            };
            struct {
                unsigned int start;
                unsigned short size;
            };
        };
    };

    typedef _Scalar Scalar;
    typedef _Index  Index;

    static constexpr Index invalidIndex() { return -1; }

    typedef Eigen::Matrix<Scalar,3,1> VectorType;
    typedef Eigen::AlignedBox<_Scalar, 3> AxisAlignedBoxType;

    typedef std::vector<KdNode>      NodeList;
    typedef std::vector<VectorType>  PointList;
    typedef std::vector<Index>       IndexList;

    //! element of the stack
    struct QueryNode
    {
        inline QueryNode() {}
        inline QueryNode(unsigned int id) : nodeId(id) {}
        //! id of the next node
        unsigned int nodeId;
        //! squared distance to the next node
        Scalar sq;
    };

    template <int _stackSize = 64>
    struct RangeQuery
    {
        VectorType queryPoint;
        Scalar     sqdist;
        QueryNode  nodeStack[_stackSize];
    };

    inline const NodeList&   _getNodes   (void) { return mNodes;   }
    inline const PointList&  _getPoints  (void) { return mPoints;  }
    inline const PointList&  _getIndices (void) { return mIndices;  }


public:
    //! Create the Kd-Tree using memory copy.
    KdTree(const PointList& points,
           unsigned int nofPointsPerCell = KD_POINT_PER_CELL,
           unsigned int maxDepth = KD_MAX_DEPTH );

    //! Create a void KdTree
    KdTree( unsigned int size = 0,
            unsigned int nofPointsPerCell = KD_POINT_PER_CELL,
            unsigned int maxDepth = KD_MAX_DEPTH );

    //! Add a new vertex in the KdTree
    template <class VectorDerived>
    inline void add( const VectorDerived &p ){
         // this is ok since the memory has been reserved at construction time
        mPoints.push_back(p);
        mIndices.push_back(mIndices.size());
        mAABB.extend(p);
    }

    inline void add(Scalar *position){
        add(Eigen::Map< Eigen::Matrix<Scalar, 3, 1> >(position));
    }

    //! Finalize the creation of the KdTree
    inline
    void finalize( );

    inline const AxisAlignedBoxType& aabb() const  {return mAABB; }

    ~KdTree();

    /*!
     * \brief Performs distance query and return vector coordinates
     */
    template<int stackSize, typename Container = std::vector<VectorType> >
    inline void
    doQueryDist(RangeQuery<stackSize>& query,
                Container& result) const {
        _doQueryDistIndicesWithFunctor(query, [&result,this](unsigned int i){
            result.push_back(mPoints[i]);
        });
    }

    /*!
     * \brief Performs distance query and return indices
     */
    template<int stackSize, typename IndexContainer = std::vector<Index> >
    inline void
    doQueryDistIndices(RangeQuery<stackSize>& query,
                       IndexContainer& result) const {
        _doQueryDistIndicesWithFunctor(query, [&result,this](unsigned int i){
            result.push_back(mIndices[i]);
        });
    }

    /*!
     * \brief Performs distance query and return indices
     */
    template<int stackSize, typename Functor>
    inline void
    doQueryDistProcessIndices(RangeQuery<stackSize> &query,
                              Functor f) const {
        _doQueryDistIndicesWithFunctor(query, [f,this](unsigned int i){
            f(mIndices[i]);
        });
    }

    /*!
     * \brief Finds the closest element index within the range [0:sqrt(sqdist)]
     * \param currentId Index of the querypoint if it belongs to the tree
     */
    template<int stackSize>
    inline std::pair<Index, Scalar>
    doQueryRestrictedClosestIndex(RangeQuery<stackSize> &query,
                                  int currentId = -1) const;

     EIGEN_MAKE_ALIGNED_OPERATOR_NEW

protected:

    /*!
      Used to build the tree: split the subset [start..end[ according to dim
      and splitValue, and returns the index of the first element of the second
      subset.
      */
    inline
    unsigned int split(int start, int end, unsigned int dim, Scalar splitValue);

    void createTree(unsigned int nodeId,
                    unsigned int start,
                    unsigned int end,
                    unsigned int level,
                    unsigned int targetCellsize,
                    unsigned int targetMaxDepth);


    /*!
     * \brief Performs distance query and pass the internal id to a functor
     */
    template<int stackSize, typename Functor >
    inline void
    _doQueryDistIndicesWithFunctor(RangeQuery<stackSize>& query,
                                   Functor f) const;
protected:

    PointList  mPoints;
    IndexList  mIndices;
    AxisAlignedBoxType mAABB;
    NodeList   mNodes;

    unsigned int _nofPointsPerCell;
    unsigned int _maxDepth;
};





/*!
  \see KdTree(unsigned int size, unsigned int nofPointsPerCell, unsigned int maxDepth)
  */
template<typename Scalar, typename Index>
KdTree<Scalar, Index>::KdTree(const PointList& points,
                       unsigned int nofPointsPerCell,
                       unsigned int maxDepth)
    : mPoints(points),
      mIndices(points.size()),
      _nofPointsPerCell(nofPointsPerCell),
      _maxDepth(maxDepth)
{
    mAABB.extend(points.cbegin(), points.cend());
    std::iota (mIndices.begin(), mIndices.end(), 0); // Fill with 0, 1, ..., 99.
    finalize();
}

/*!
  Second way to create the KdTree, in two time. You must call finalize()
  before requesting for closest points.

  \see finalize()
  */
template<typename Scalar, typename Index>
KdTree<Scalar, Index>::KdTree(unsigned int size,
                       unsigned int nofPointsPerCell,
                       unsigned int maxDepth)
    : _nofPointsPerCell(nofPointsPerCell),
      _maxDepth(maxDepth)
{
    mPoints.reserve(size);
    mIndices.reserve(size);
}

template<typename Scalar, typename Index>
void
KdTree<Scalar, Index>::finalize()
{
    mNodes.clear();
    mNodes.reserve(4*mPoints.size()/_nofPointsPerCell);
    mNodes.emplace_back();
    mNodes.back().leaf = 0;
#ifdef DEBUG
    std::cout << "create tree" << std::endl;
#endif
    createTree(0, 0, mPoints.size(), 1, _nofPointsPerCell, _maxDepth);
#ifdef DEBUG
    std::cout << "create tree ... DONE (" << mPoints.size() << " points)" << std::endl;
#endif
}

template<typename Scalar, typename Index>
KdTree<Scalar, Index>::~KdTree()
{
}


/*!

  This algorithm uses the simple distance to the split plane to prune nodes.
  A more elaborated approach consists to track the closest corner of the cell
  relatively to the current query point. This strategy allows to save about 5%
  of the leaves. However, in practice the slight overhead due to this tracking
  reduces the overall performance.

  This algorithm also use a simple stack while a priority queue using the squared
  distances to the cells as a priority values allows to save about 10% of the leaves.
  But, again, priority queue insertions and deletions are quite involved, and therefore
  a simple stack is by far much faster.

  The optionnal parameter currentId is used when the query point is
  stored in the tree, and must thus be avoided during the query
*/
template<typename Scalar, typename Index>
template<int stackSize>
std::pair<Index, Scalar>
KdTree<Scalar, Index>::doQueryRestrictedClosestIndex(
        RangeQuery<stackSize>& query,
        int currentId) const
{

    Index  cl_id   = invalidIndex();
    Scalar cl_dist = query.sqdist;

    query.nodeStack[0].nodeId = 0;
    query.nodeStack[0].sq = 0.f;
    unsigned int count = 1;

    //int nbLoop = 0;
    while (count)
    {
        //nbLoop++;
        QueryNode&    qnode = query.nodeStack[count-1];
        const KdNode& node  = mNodes[qnode.nodeId];

        if (qnode.sq < cl_dist)
        {
            if (node.leaf)
            {
                --count; // pop
                const int end = node.start+node.size;
                for (int i=node.start ; i<end ; ++i){
                    const Scalar sqdist = (query.queryPoint - mPoints[i]).squaredNorm();
                    if (sqdist <= cl_dist && mIndices[i] != currentId){
                        cl_dist = sqdist;
                        cl_id   = mIndices[i];
                    }
                }
            }
            else
            {
                // replace the stack top by the farthest and push the closest
                const Scalar new_off = query.queryPoint[node.dim] - node.splitValue;

                //std::cout << "new_off = " << new_off << std::endl;

                if (new_off < 0.)
                {
                    query.nodeStack[count].nodeId  = node.firstChildId; // stack top the farthest
                    qnode.nodeId = node.firstChildId+1;            // push the closest
                }
                else
                {
                    query.nodeStack[count].nodeId  = node.firstChildId+1;
                    qnode.nodeId = node.firstChildId;
                }
                query.nodeStack[count].sq = qnode.sq;
                qnode.sq = new_off*new_off;
                ++count;
            }
        }
        else
        {
            // pop
            --count;
        }
    }
    return std::make_pair(cl_id, cl_dist);
}

/*!
  \see doQueryRestrictedClosestIndex For more information about the algorithm.

  This function is an alternative to doQueryDist(const VectorType& queryPoint)
  that allow to perform the query by requesting a maximum distance instead of
  neighborhood size.
 */
template<typename Scalar, typename Index>
template<int stackSize, typename Functor >
void
KdTree<Scalar, Index>::_doQueryDistIndicesWithFunctor(
        RangeQuery<stackSize>& query,
        Functor f) const
{
    query.nodeStack[0].nodeId = 0;
    query.nodeStack[0].sq = 0.f;
    unsigned int count = 1;

    while (count)
    {
        QueryNode&    qnode = query.nodeStack[count-1];
        const KdNode & node = mNodes[qnode.nodeId];

        if (qnode.sq < query.sqdist)
        {
            if (node.leaf)
            {
                --count; // pop
                unsigned int end = node.start+node.size;
                for (unsigned int i=node.start ; i<end ; ++i)
                    if ( (query.queryPoint - mPoints[i]).squaredNorm() < query.sqdist){
                        f(i);
                    }
            }
            else
            {
                // replace the stack top by the farthest and push the closest
                Scalar new_off = query.queryPoint[node.dim] - node.splitValue;
                if (new_off < 0.)
                {
                    query.nodeStack[count].nodeId  = node.firstChildId;
                    qnode.nodeId = node.firstChildId+1;
                }
                else
                {
                    query.nodeStack[count].nodeId  = node.firstChildId+1;
                    qnode.nodeId = node.firstChildId;
                }
                query.nodeStack[count].sq = qnode.sq;
                qnode.sq = new_off*new_off;
                ++count;
            }
        }
        else
        {
            // pop
            --count;
        }
    }
}

template<typename Scalar, typename Index>
unsigned int KdTree<Scalar, Index>::split(int start, int end, unsigned int dim, Scalar splitValue)
{
    int l(start), r(end-1);
    for ( ; l<r ; ++l, --r)
    {
        while (l < end && mPoints[l][dim] < splitValue)
            l++;
        while (r >= start && mPoints[r][dim] >= splitValue)
            r--;
        if (l > r)
            break;
        std::swap(mPoints[l],mPoints[r]);
        std::swap(mIndices[l],mIndices[r]);
    }
    return (mPoints[l][dim] < splitValue ? l+1 : l);
}

/*!

    Recursively builds the kdtree


   The heuristic is the following:
    - if the number of points in the node is lower than targetCellsize then make a leaf
    - else compute the AABB of the points of the node and split it at the middle of
      the largest AABB dimension.

   This strategy might look not optimal because it does not explicitly prune empty space,
   unlike more advanced SAH-like techniques used for RT. On the other hand it leads to a shorter tree,
   faster to traverse and our experience shown that in the special case of kNN queries,
   this strategy is indeed more efficient (and much faster to build). Moreover, for volume data
   (e.g., fluid simulation) pruning the empty space is useless.

   Actually, storing at each node the exact AABB (we therefore have a binary BVH) allows
   to prune only about 10% of the leaves, but the overhead of this pruning (ball/ABBB intersection)
   is more expensive than the gain it provides and the memory consumption is x4 higher !
*/
template<typename Scalar, typename Index>
void KdTree<Scalar, Index>::createTree(unsigned int nodeId, unsigned int start, unsigned int end, unsigned int level, unsigned int targetCellSize, unsigned int targetMaxDepth)
{

    KdNode& node = mNodes[nodeId];
    AxisAlignedBoxType aabb;
    //aabb.Set(mPoints[start]);
    for (unsigned int i=start ; i<end ; ++i)
        aabb.extend(mPoints[i]);

    VectorType diag =  aabb.diagonal();
    typename VectorType::Index dim;

#ifdef DEBUG

//    std::cout << "createTree("
//              << nodeId << ", "
//              << start << ", "
//              << end << ", "
//              << level << ")"
//              << std::endl;

    if (std::isnan(diag.maxCoeff(&dim))){
        std::cerr << "NaN values discovered in the tree, abort" << std::endl;
        return;
    }
#else
    diag.maxCoeff(&dim);
#endif


#undef DEBUG
    node.dim = dim;
    node.splitValue = aabb.center()(dim);

    unsigned int midId = split(start, end, dim, node.splitValue);

    node.firstChildId = mNodes.size();

    {
        KdNode n;
        n.size = 0;
        mNodes.push_back(n);
        mNodes.push_back(n);
    }
    //mNodes << Node() << Node();
    //mNodes.resize(mNodes.size()+2);

    {
        // left child
        unsigned int childId = mNodes[nodeId].firstChildId;
        KdNode& child = mNodes[childId];
        if (midId-start <= targetCellSize || level>=targetMaxDepth)
        {
            child.leaf = 1;
            child.start = start;
            child.size = midId-start;
        }
        else
        {
            child.leaf = 0;
            createTree(childId, start, midId, level+1, targetCellSize, targetMaxDepth);
        }
    }

    {
        // right child
        unsigned int childId = mNodes[nodeId].firstChildId+1;
        KdNode& child = mNodes[childId];
        if (end-midId <= targetCellSize || level>=targetMaxDepth)
        {
            child.leaf = 1;
            child.start = midId;
            child.size = end-midId;
        }
        else
        {
            child.leaf = 0;
            createTree(childId, midId, end, level+1, targetCellSize, targetMaxDepth);
        }
    }
}
} //namespace Super4PCS


#endif // KDTREE_H
