#ifndef FIELDINTERPOLATOR_H
#define FIELDINTERPOLATOR_H

#include <vector>              // STL container
#include <common/interfaces.h> // Meshlab stuff

// Note that these includes are system-wide, not the ones of Meshlab
#define EIGEN_CHOLMOD_SUPPORT
#include <vcg/Eigen/Cholesky>
#include <vcg/Eigen/Sparse>

namespace vcg{

/// The type of laplacian weights the solver will use
enum LAPLACIAN {COMBINATORIAL, COTANGENT};

/// CHOLMOD can only work on the DOUBLE type. NaN otherwise
typedef double FieldType;

/// Interpolates a scalar field on a surface according to laplacian criterions
class FieldInterpolator{
private:
    /// The domain over which the interpolation is defined
    CMeshO* mesh;
    /// The EIGEN matrices/vectors involved in the interpolation
    Eigen::DynamicSparseMatrix<FieldType>* A_dyn;
    Eigen::Matrix<FieldType, Eigen::Dynamic, 1>* xb;
public:
    /// Refer to Init
    FieldInterpolator(){
        A_dyn = 0;
        xb = 0;
    }
    /// Free the memory stored for sparse matrixes
    ~FieldInterpolator(){
        if( A_dyn!=0 ) delete A_dyn;
        if( xb!=0 ) delete xb;
    }
    /// Creates the sparse laplacian matrixes used by the solver using the specified on the domain of mesh
    void Init(CMeshO* mesh, LAPLACIAN laptype=COMBINATORIAL);
    /// Adds a constraint to vertex vidx
    void AddConstraint( unsigned int vidx, FieldType omega, FieldType val);
    /// Solves the interpolation problem by Cholesky returning the solution in the Q() vertex property
    void Solve();
};

} // vcg::

#endif // FIELDINTERPOLATOR_H
