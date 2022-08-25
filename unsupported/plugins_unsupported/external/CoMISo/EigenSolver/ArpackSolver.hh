//=============================================================================
//
//  CLASS ArpackSolver
//
//=============================================================================


#ifndef COMISO_ARPACKSOLVER_HH
#define COMISO_ARPACKSOLVER_HH

//== COMPILE-TIME PACKAGE REQUIREMENTS ========================================
#include <CoMISo/Config/config.hh>
#if (COMISO_ARPACK_AVAILABLE && COMISO_SUITESPARSE_AVAILABLE && COMISO_EIGEN3_AVAILABLE)

//== INCLUDES =================================================================
#include <CoMISo/Config/CoMISoDefines.hh>

#include <Eigen/Eigen>
#include "EigenArpackMatrixT.hh"

#include <arpack++/arssym.h>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace COMISO {

//== CLASS DEFINITION =========================================================



	      
/** \class ArpackSolver ArpackSolver.hh <COMISO/.../ArpackSolver.hh>

    Brief Description.
  
    A more elaborate description follows.
*/


class COMISODLLEXPORT ArpackSolver
{
public:

  // sparse matrix type
  typedef EigenArpackMatrixT<double,Eigen::SparseMatrix<double,Eigen::ColMajor> > Matrix;


  /// Constructor
  ArpackSolver() {}
 
  /// Destructor
  ~ArpackSolver() {}

  // solve eigenproblem
  // number of desired eigenvalues -> _n_eigenvalues
  // which eigenvalues -> one of {LA (largest algebraic), SA (smalles algebraic), LM (largest magnitude), SM(smallest magnitued), BE(both ends)}
  template<class MatrixT,class MatrixT2>
  void solve(const MatrixT&       _A,
             std::vector<double>& _eigenvalues,
             MatrixT2&            _eigenvectors,
             const int            _n_eigvalues = 1,
             const char*          _which_eigs = "SM");

  // solve eigenproblem
  // number of desired eigenvalues -> _n_eigenvalues
  // which eigenvalues -> one of {LA (largest algebraic), SA (smalles algebraic), LM (largest magnitude), SM(smallest magnitued), BE(both ends)}
  template<class MatrixT,class MatrixT2>
  void solve_inverse(const MatrixT&       _A,
                     std::vector<double>& _eigenvalues,
                     MatrixT2&            _eigenvectors,
                     const int            _n_eigvalues = 1,
                     const char*          _which_eigs = "LM");


  // check resulting eigenvalues/eigenvectors
  template<class MatrixT,class MatrixT2>
  void check_result(const MatrixT& _A, std::vector<double>& _eigenvalues, MatrixT2& _eigenvectors);

private:
  
};


//=============================================================================
} // namespace ACG
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(COMISO_ARPACKSOLVER_C)
#define COMISO_ARPACKSOLVER_TEMPLATES
#include "ArpackSolver.cc"
#endif
//=============================================================================
#endif // COMISO_SUITESPARSE_AVAILABLE
//=============================================================================
#endif // ACG_ARPACKSOLVER_HH defined
//=============================================================================

