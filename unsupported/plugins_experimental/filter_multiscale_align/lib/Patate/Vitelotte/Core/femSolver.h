/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _VITELOTTE_FEM_SOLVER_
#define _VITELOTTE_FEM_SOLVER_

#include <cassert>

#include <Eigen/Core>
#include <Eigen/Sparse>

#include "solverError.h"
#include "femUtils.h"
#include "vgMesh.h"


namespace Vitelotte
{


/**
 * \brief A diffusion solver based on the Finite Element Method.
 *
 * \tparam _Mesh The VGMesh type on which the solver operates.
 * \tparam _ElementBuilder An ElementBuilder class that describe the type of
 * elements used by the solver, and by extension the kind of diffusion to do.
 */
template < class _Mesh, class _ElementBuilder >
class FemSolver
{
public:
    typedef _Mesh Mesh;
    typedef _ElementBuilder ElementBuilder;
    typedef FemSolver<Mesh, ElementBuilder> Self;

    typedef typename ElementBuilder::Scalar Scalar;

protected:
    typedef typename Mesh::Node Node;
    typedef typename Mesh::Vertex Vertex;
    typedef typename Mesh::Face Face;
    typedef typename Mesh::FaceIterator FaceIterator;

public:
    inline FemSolver(const ElementBuilder& elementBuilder = ElementBuilder());
    ~FemSolver();

    /// \brief Build the internal stiffness matrix from `mesh`.
    void build(const Mesh& mesh);

    /// \brief Solve the diffusion problem created by `build()` on `mesh`.
    ///
    /// Parameters of `build()` and `solve()` do not need to be the same, but
    /// they must have the same connectivity. (In other words, only constrained
    /// nodes values should differ.)
    void solve(Mesh& mesh);

    /// \brief Returns a SolverError object describing the state of the solver.
    inline const SolverError error() { return m_error; }

public:
    typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
    typedef Eigen::Triplet<Scalar> Triplet;
    typedef Eigen::SparseMatrix<Scalar> StiffnessMatrix;

    typedef std::vector<Triplet> TripletVector;
    typedef typename TripletVector::iterator TripletVectorIterator;

    typedef std::vector<bool> BoolVector;
    typedef std::vector<unsigned> IndexMap;

    typedef Eigen::SimplicialLDLT<StiffnessMatrix, Eigen::Lower> LDLT;

    struct Block
    {
        TripletVector    triplets;
        StiffnessMatrix  matrix;
        LDLT*            decomposition;
        Matrix           rhs;      // Contains the rhs "extra" constraints
        unsigned         offset;   // Offset to apply to block indices to get global indices
        unsigned         size;
        unsigned         nCoeffs;

        inline Block() : decomposition(0) {}
        inline ~Block() { delete decomposition; }
    };
    typedef std::vector<Block> BlockVector;
    typedef typename BlockVector::iterator BlockIterator;

    struct BlockIndex
    {
        int block;
        int index;

        inline BlockIndex(int block, int index) : block(block), index(index) {}
    };
    typedef std::vector<BlockIndex> NodeMap;

protected:
    // Map each node to a row/column an split the problem in a set of
    // independant sub-problems.
    void preSort(const Mesh& mesh);

    // Compute the stiffness matrix coefficients.
    void buildMatrix(const Mesh& mesh);

    // Factorize the stiffness matrix.
    void factorize();

protected:
    ElementBuilder m_elementBuilder;

    SolverError m_error;

    unsigned m_nUnknowns;
    unsigned m_nConstraints;

    BlockVector     m_blocks;
    Eigen::VectorXi m_faceBlockMap;  // The face -> block index map
    TripletVector   m_constraintTriplets;
    StiffnessMatrix m_constraintBlock;
    NodeMap         m_nodeMap;       // Map node->idx() -> (block id, index)
    BoolVector      m_fMask;         // Mark faces processed by preSort
    BoolVector      m_nMask;         // Mark nodes processed by preSort
    Eigen::VectorXi m_fExtraIndices; // Each face with extra constraint is
                                     //   given a unique index in the range [0..n]
    NodeMap         m_fExtraMap;     // Map m_fExtraIndex to a column index
                                     //   (no perm required)
    Matrix          m_x;
};


} // namespace Vitelotte

#include "femSolver.hpp"


#endif
