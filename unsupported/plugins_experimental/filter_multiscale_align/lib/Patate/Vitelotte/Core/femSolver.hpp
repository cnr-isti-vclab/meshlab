/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "femSolver.h"


namespace Vitelotte
{

namespace internal
{

template <typename Solver>
struct CheckEigenSolverError
{
    static bool check(const Solver& solver, SolverError& error)
    {
        switch(solver.info()) {
        case Eigen::Success:        return false;
        case Eigen::NumericalIssue: error.error("Numerical issue"); return true;
        case Eigen::NoConvergence:  error.error("No convergence");  return true;
        case Eigen::InvalidInput:   error.error("Invalid matrix");  return true;
        }
        error.error("Unknown Eigen error");  return true;
        return true;
    }
};


template <typename Solver>
bool checkEigenSolverError(const Solver& solver, SolverError& error)
{
    return CheckEigenSolverError<Solver>::check(solver, error);
}


template < typename Solver >
class SolverInserter {
public:
    typedef typename Solver::Scalar         Scalar;

protected:
    typedef typename Solver::Mesh           Mesh;
    typedef typename Mesh::Face             Face;

    typedef typename Solver::Matrix         Matrix;

    typedef typename Solver::Triplet        Triplet;
    typedef typename Solver::TripletVector  TripletVector;

    typedef typename Solver::BlockIndex     BlockIndex;
    typedef typename Solver::NodeMap        NodeMap;

public:
    inline SolverInserter(TripletVector& blockCoeffs, TripletVector& consCoeffs,
                          Matrix& rhs, const NodeMap& nodeMap,
                          unsigned offset, unsigned extraOffset)
        : m_blockCoeffs (blockCoeffs),
          m_consCoeffs  (consCoeffs),
          m_rhs         (rhs),
          m_nodeMap     (nodeMap),
          m_offset      (offset),
          m_extraOffset (extraOffset) {
    }

#ifndef NDEBUG
    void debug(unsigned nUnk, unsigned nCons, unsigned bi, unsigned size) {
        m_nUnk  = nUnk;
        m_nCons = nCons;
        m_bi    = bi;
        m_size  = size;
    }
#endif

//#define DEBUG_INLINE __attribute__((always_inline))
//#define DEBUG_INLINE __attribute__((noinline))
#define DEBUG_INLINE

    DEBUG_INLINE void addCoeff(unsigned ni0, unsigned ni1, Scalar value)
    {
        BlockIndex bi0 = m_nodeMap[ni0];
        BlockIndex bi1 = m_nodeMap[ni1];

        unsigned id = (bi0.block < 0) | ((bi1.block < 0) << 1);
        switch(id) {
        // Both are unknowns
        case 0x00: {
            assert(unsigned(bi0.block) == m_bi && unsigned(bi1.block) == m_bi);
            assert(unsigned(bi0.index) < m_size);
            assert(unsigned(bi1.index) < m_size);
            m_blockCoeffs.push_back(Triplet(
                        std::max(bi0.index, bi1.index),
                        std::min(bi0.index, bi1.index),
                        value));
            break;
        }

        // 0 is a constraint, 1 is an unknown
        case 0x01: std::swap(bi0, bi1);     // Fall-through
         // 0 is an unknown, 1 is a constraint
        case 0x02: {
            assert(unsigned(bi0.block) == m_bi);
            unsigned row = bi0.index + m_offset;
            unsigned col = bi1.index;
            assert(row < m_nUnk);
            assert(col < m_nCons);
            m_consCoeffs.push_back(Triplet(row, col, value));
            break;
        }

        // Both are constraints
        case 0x03: break;

        // Should not happen
        default: assert(false);
        }
    }

    DEBUG_INLINE void addExtraCoeff(Face /*elem*/, unsigned ei, unsigned ni, Scalar value)
    {
        BlockIndex nbi = m_nodeMap[ni];
        unsigned eio = m_extraOffset + ei;
        assert(eio < m_size);

        if(nbi.block >= 0)
        {
            assert(unsigned(nbi.block) == m_bi);
            // extra should have greater indices
            assert(nbi.index >= 0 && unsigned(nbi.index) < m_extraOffset);
            m_blockCoeffs.push_back(Triplet(eio, nbi.index, value));
        }
        else
        {
            unsigned row = eio + m_offset;
            unsigned col = nbi.index;
            assert(row < m_nUnk);
            assert(col < m_nCons);
            m_consCoeffs.push_back(Triplet(row, col, value));
        }
    }

    template < typename Derived >
    DEBUG_INLINE void setExtraRhs(Face /*elem*/, unsigned ei, const Eigen::DenseBase<Derived>& value)
    {
        unsigned eio = m_extraOffset + ei;
        assert(eio < m_size);
        m_rhs.row(eio) = value;
    }


private:
    TripletVector&  m_blockCoeffs;
    TripletVector&  m_consCoeffs;
    Matrix&         m_rhs;
    const NodeMap&  m_nodeMap;
    unsigned        m_offset;
    unsigned        m_extraOffset;
#ifndef DNDEBUG
    unsigned        m_nUnk;
    unsigned        m_nCons;
    unsigned        m_bi;
    unsigned        m_size;
#endif
};

#undef DEBUG_INLINE

}


template < class _Mesh, class _ElementBuilder >
FemSolver<_Mesh, _ElementBuilder>::FemSolver(const ElementBuilder& elementBuilder)
  : m_elementBuilder(elementBuilder)
{
}


template < class _Mesh, class _ElementBuilder >
FemSolver<_Mesh, _ElementBuilder>::~FemSolver()
{
}


template < class _Mesh, class _ElementBuilder >
void
FemSolver<_Mesh, _ElementBuilder>::build(const Mesh& mesh)
{
    // Cleanup error status
    m_error.resetStatus();

    // Compute node id to row/col mapping and initialize blocks
    preSort(mesh);
    if(m_error.status() == SolverError::STATUS_ERROR) return;

    // Fill the blocks
    buildMatrix(mesh);
    if(m_error.status() == SolverError::STATUS_ERROR) return;

    // Factorize the lhs
    factorize();
}


template < class _Mesh, class _ElementBuilder >
void
FemSolver<_Mesh, _ElementBuilder>::solve(Mesh& mesh)
{
    if(m_error.status() == SolverError::STATUS_ERROR)
    {
        return;
    }

    // compute RHS
    Matrix constraints = Matrix::Zero(m_nConstraints, mesh.nCoeffs());

    for(typename Mesh::NodeIterator nit = mesh.nodesBegin();
        nit != mesh.nodesEnd(); ++nit)
    {
        BlockIndex bi = m_nodeMap[(*nit).idx()];
        if(mesh.isConstraint(*nit) && bi.block == -1 && bi.index >= 0)
        {
            constraints.row(bi.index) = mesh.value(*nit).template cast<Scalar>();
        }
    }

    m_x.resize(m_nUnknowns, mesh.nCoeffs());
    Matrix b = m_constraintBlock * constraints;

    // Clear extra constraints
    for(BlockIterator block = m_blocks.begin();
        block != m_blocks.end(); ++block)
    {
        block->rhs.setZero();
    }

    // Fill a Triplet vector with coefficients + fill m_b
    m_constraintTriplets.clear();
    for(FaceIterator elem = mesh.facesBegin();
        elem != mesh.facesEnd(); ++elem)
    {
        typedef internal::SolverInserter<Self> Inserter;

        int       bi           = m_faceBlockMap((*elem).idx());
        if(bi < 0) continue;
        Block&    block        = m_blocks[bi];
        int       extraIndex   = m_fExtraIndices((*elem).idx());
        unsigned  extraOffset  = (extraIndex < 0)? 0: m_fExtraMap[extraIndex].index;

        Inserter inserter(block.triplets,
                          m_constraintTriplets,
                          block.rhs,
                          m_nodeMap,
                          block.offset,
                          extraOffset);
#ifndef NDEBUG
        inserter.debug(m_nUnknowns, m_nConstraints, bi, block.size);
#endif
        m_elementBuilder.addExtraConstraints(inserter, mesh, *elem, &m_error);

        if(m_error.status() == SolverError::STATUS_ERROR)
        {
            return;
        }
    }

    // Solve block by block
    for(BlockIterator block = m_blocks.begin();
        block != m_blocks.end(); ++block)
    {
        unsigned start = block->offset;
        unsigned size  = block->size;

        m_x.middleRows(start, size) =
                block->decomposition->solve(block->rhs - b.middleRows(start, size));
        if(internal::checkEigenSolverError(*block->decomposition, m_error)) return;
    }

    // Update mesh nodes
    for(typename Mesh::NodeIterator nit = mesh.nodesBegin();
        nit != mesh.nodesEnd(); ++nit)
    {
        BlockIndex bi = m_nodeMap[(*nit).idx()];
        if(!mesh.isConstraint(*nit) && bi.block >= 0 && bi.index >= 0)
        {
            unsigned ri = m_blocks[bi.block].offset + bi.index;
            mesh.value(*nit) =
                    m_x.row(ri).template cast<typename Mesh::Scalar>();
        }
    }
}


template < class _Mesh, class _ElementBuilder >
void
FemSolver<_Mesh, _ElementBuilder>::preSort(const Mesh& mesh)
{
    // FIXME: This algorithm assume there is only local constraints.

    typedef typename Mesh::HalfedgeAroundFaceCirculator HalfedgeCirculator;
    typedef typename Mesh::HalfedgeAttribute HalfedgeAttribute;
    typedef typename Mesh::Node Node;

    // 1- Count the number of faces with extra constraints and the total number
    //    of constraints.
    unsigned nExtraFaces = 0;
    unsigned nExtraCons  = 0;
    for(FaceIterator elem = mesh.facesBegin();
        elem != mesh.facesEnd(); ++elem)
    {
        unsigned nCons = m_elementBuilder.nExtraConstraints(mesh, *elem);
        if(nCons)
        {
            ++nExtraFaces;
            nExtraCons += nCons;
        }
    }

    // Note: these are not defivitive values. There may be less nodes than
    // planed if not all of them are referenced by faces or some are deleted.
    unsigned nNodes           = mesh.nodesSize();   // Number of nodes (worst case)
    unsigned uPos             = 0; // r/c index of the last processed unknown node
    unsigned uCount           = 0; // total number of contributing unknown nodes
    unsigned cPos             = 0; // r/c index of the last processed constraint node
    unsigned count            = 0; // nb items in stack
    unsigned extraFacesCount  = 0; // nb faces with extra constrains
    unsigned extraRangeStart  = 0; // index of the first extra of the current range
    unsigned elemExtraCount   = 0; // nb extra node for the current block

    // Initialize some members
    m_blocks.clear();
    m_blocks.reserve(32);
    m_nodeMap.assign(nNodes, BlockIndex(-1, -1));
    m_fExtraIndices.resize(mesh.facesSize()); m_fExtraIndices.fill(-1);
    m_fExtraMap.assign(nExtraFaces, BlockIndex(-1, -1));
    m_faceBlockMap.resize(mesh.facesSize()); m_faceBlockMap.fill(-1);

    FaceIterator    faceIt = mesh.facesBegin();
    Eigen::VectorXi fStack(mesh.nFaces());
    m_fMask.assign(mesh.nFaces(), true);
    m_nMask.assign(nNodes, true);

    // 2- Compute a mapping from node indices to column/row indices with a
    //    breadth first search. May split the problem in independant blocks.

    // Push the first face
    m_blocks.push_back(Block());
    m_blocks.back().offset = uPos;
    m_fMask[(*faceIt).idx()] = false;
    fStack(count++) = (*faceIt).idx();

    while(count)
    {
        // Pop a face
        unsigned fi = fStack(--count);
        unsigned bi = m_blocks.size() - 1;
        Face face(fi);

        m_faceBlockMap(fi) = bi;

        // Process each node of each halfedges
        HalfedgeCirculator hc    = mesh.halfedges(face);
        HalfedgeCirculator hcEnd = hc;
        do {
            assert(!mesh.isBoundary(*hc));

            for(unsigned ai = 0; ai < Mesh::HALFEDGE_ATTRIB_COUNT; ++ai) {
                HalfedgeAttribute attr = HalfedgeAttribute(ai);
                if(!mesh.hasAttribute(attr)) continue;

                Node n        = mesh.halfedgeNode(*hc, attr);
                unsigned ni   = n.idx();
                Node on       = mesh.halfedgeOppositeNode(*hc, attr);
                Face of       = mesh.face(mesh.oppositeHalfedge(*hc));
                unsigned ofi  = of.idx();
                bool isCons   = mesh.isConstraint(n);

                if(!n.isValid()) {
                    m_error.error("Input mesh contains invalid nodes.");
                    return;
                }

                // If the node is not yet pushed, do it
                if(m_nMask[ni])
                {
                    m_nMask[ni] = false;
                    if(isCons) m_nodeMap[ni]  = BlockIndex(-1, cPos++);
                    else       m_nodeMap[ni]  = BlockIndex(bi, uPos++);
                }

                // If the node is used on the opposite face, this face must be
                // in the same range, so push it.
                if(!isCons && n == on && m_fMask[ofi])
                {
                    m_fMask[ofi]     = false;
                    fStack(count++)  = ofi;
                }
            }

            ++hc;
        } while(hc != hcEnd);

        unsigned nElemExtra = m_elementBuilder.nExtraConstraints(mesh, face);
        if(nElemExtra)
        {
            m_fExtraMap[extraFacesCount] = BlockIndex(bi, elemExtraCount);
            m_fExtraIndices(fi) = extraFacesCount++;
            elemExtraCount += nElemExtra;
        }


        // Finalize range when the stack is empty
        if(!count)
        {
            // Search for unprocessed face
            while(faceIt != mesh.facesEnd() && !m_fMask[(*faceIt).idx()]) ++faceIt;

            // NOTE: This bypass the bloc splitting mechanism, thus creating
            //   a single big bloc. For testing purpose and/or to allow
            //   non-local constraints.
//            if(faceIt != mesh.facesEnd()) {
//                fi = (*faceIt).idx();
//                m_fMask[fi]      = false;
//                fStack(count++)  = fi;
//                continue;
//            }

            // Offset extra constraints indices so that they are > to node
            // indices. It allow extra constraints (who have 0 on the diagonal)
            // to be on the bottom-right of the matrix.
            for(unsigned ei = extraRangeStart; ei < extraFacesCount; ++ei)
            {
                m_fExtraMap[ei].index += uPos;
            }
            uPos += elemExtraCount;

            // Finalize the block
            m_blocks.back().size = uPos;
            m_blocks.back().matrix.resize(uPos, uPos);
            m_blocks.back().rhs.resize(uPos, mesh.nCoeffs());
            m_blocks.back().rhs.fill(0);
            m_blocks.back().nCoeffs = 0;

            uCount          += uPos;
            uPos             = 0;
            extraRangeStart  = extraFacesCount;
            elemExtraCount   = 0;

            // Push the first face of the next range, if any.
            if(faceIt != mesh.facesEnd())
            {
                fi = (*faceIt).idx();
                m_blocks.push_back(Block());
                m_blocks.back().offset = uCount;
                m_fMask[fi]      = false;
                fStack(count++)  = fi;
            }
        }
    }

    // Compute the real number of unknowns and constraints.
    m_nUnknowns       = uCount;
    m_nConstraints    = cPos;

    m_constraintBlock.resize(m_nUnknowns, m_nConstraints);
}


template < class _Mesh, class _ElementBuilder >
void FemSolver<_Mesh, _ElementBuilder>::buildMatrix(const Mesh& mesh)
{
    // Pre-compute number of coefficients
    for(FaceIterator elem = mesh.facesBegin();
        elem != mesh.facesEnd(); ++elem)
    {
        int bii = m_faceBlockMap((*elem).idx());
        if(bii < 0) continue;
        Block& block = m_blocks[bii];
        block.nCoeffs += m_elementBuilder.nCoefficients(mesh, *elem, &m_error);
    }

    // Reserve memory for triplets
    for(BlockIterator block = m_blocks.begin();
        block != m_blocks.end(); ++block)
    {
        block->triplets.reserve(block->nCoeffs);
    }

    // Compute triplets for all blocks
    m_constraintTriplets.clear();
    for(FaceIterator elem = mesh.facesBegin();
        elem != mesh.facesEnd(); ++elem)
    {
        typedef internal::SolverInserter<Self> Inserter;

        int       bi           = m_faceBlockMap((*elem).idx());
        if(bi < 0) continue;
        Block&    block        = m_blocks[bi];
        int       extraIndex   = m_fExtraIndices((*elem).idx());
        unsigned  extraOffset  = (extraIndex < 0)? 0: m_fExtraMap[extraIndex].index;

        Inserter inserter(block.triplets,
                          m_constraintTriplets,
                          block.rhs,
                          m_nodeMap,
                          block.offset,
                          extraOffset);
#ifndef NDEBUG
        inserter.debug(m_nUnknowns, m_nConstraints, bi, block.size);
#endif
        m_elementBuilder.addCoefficients(inserter, mesh, *elem, &m_error);

        if(m_error.status() == SolverError::STATUS_ERROR)
        {
            return;
        }
    }

    m_constraintBlock.setFromTriplets(
                m_constraintTriplets.begin(), m_constraintTriplets.end());

    // Fill the matrix
    for(BlockIterator block = m_blocks.begin();
        block != m_blocks.end(); ++block)
    {
        block->matrix.setFromTriplets(
                    block->triplets.begin(), block->triplets.end());
    }
}


template < class _Mesh, class _ElementBuilder >
void FemSolver<_Mesh, _ElementBuilder>::factorize()
{
//#ifdef _OPENMP
//#pragma omp parallel for schedule(static,1)
//#endif
    for(BlockIterator block = m_blocks.begin();
        block != m_blocks.end(); ++block)
    {
        if(!block->decomposition) block->decomposition = new LDLT;
        block->decomposition->compute(block->matrix);
        if(internal::checkEigenSolverError(*block->decomposition, m_error)) return;
    }
}


}  // namespace Vitelotte
