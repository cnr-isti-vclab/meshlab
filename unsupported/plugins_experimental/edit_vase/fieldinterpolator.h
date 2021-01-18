#ifndef FIELDINTERPOLATOR_H
#define FIELDINTERPOLATOR_H

#include <vector>              // STL container
#include <common/interfaces.h> // Meshlab stuff

// Note that these includes are system-wide, not the ones of Meshlab
#define EIGEN_CHOLMOD_SUPPORT

#ifndef USE_EIGEN_LOCAL
    //--- Eigen shipping with meshlab
    #include <vcg/Eigen/Cholesky>
    #include <vcg/Eigen/Sparse>
#else  
    //--- DEV version of eigen
    #include <Eigen/Cholesky>
    #include <Eigen/Sparse>
#endif

namespace vcg{

/// The type of laplacian weights the solver will use
enum LAPLACIAN {COMBINATORIAL, COTANGENT};

/// Interpolates a scalar field on a surface according to laplacian criterions
class FieldInterpolator{
public:
    /// CHOLMOD can only work on the DOUBLE type. NaN otherwise
    typedef double FieldType;
    typedef Eigen::DynamicSparseMatrix<FieldType> AType;
    typedef Eigen::Matrix<FieldType, Eigen::Dynamic, 1> XBType;

private:
    /// The domain over which the interpolation is defined
    CMeshO* mesh;
    /// The EIGEN "A" square matrix to be involved in the interpolation
    AType* A;
    /// The EIGEN "b" coefficient vector/matrix to be involved
    XBType* xb0;
    XBType* xb1;
    XBType* xb2;
    
public:
    /// Refer to Init
    FieldInterpolator(){
        A = NULL;
        xb0   = NULL;
        xb1   = NULL;
        xb2   = NULL;
    }
    /// Free the memory stored for sparse matrixes
    ~FieldInterpolator(){
        if( A   != NULL ) delete A;
        if( xb0 != NULL ) delete xb0;
        if( xb1 != NULL ) delete xb1;
        if( xb2 != NULL ) delete xb2;
    }
    /// Creates the sparse laplacian matrixes used by the solver using the specified on the domain of mesh
    /// @param numVars number of solves to be executed on the same domain/factorization
    /// @return false if initialization has failed for bad-conditioned triangles
    bool Init(CMeshO* mesh, int numVars=1, LAPLACIAN laptype=COMBINATORIAL);
    /// Assigns color to vertexes and faces highlighting ill-conditioned elements according to the selected laplacian
    void ColorizeIllConditioned(LAPLACIAN laptype);
    /// Adds a constraint to vertex vidx
    void AddConstraint( unsigned int vidx, FieldType omega, FieldType val);
    /// Adds a constraint to vertex vidx for the pos-th set of data to be solved  
    void AddConstraint3( unsigned int vidx, FieldType omega, FieldType val0, FieldType val1, FieldType val2 );
            
    /// Solves the interpolation problem by Cholesky returning the solution in the Q() vertex property
    void SolveInQuality();
    /// Solves the interpolation problem by Cholesky returning the solution in passed vector
    /// The returned solution vector is valid up to the next time Init is called or the object is destroyed
    void Solve( XBType* sols[3] );
    /// Solves the interpolation problem by Cholesky returning the solution in the additional attribute
    void SolveInAttribute( CMeshO::PerVertexAttributeHandle<float>& attrib );
};

} // vcg::

#endif // FIELDINTERPOLATOR_H
