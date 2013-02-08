//=============================================================================
//
//  CLASS ArpackSolver - IMPLEMENTATION
//
//=============================================================================

#define COMISO_ARPACKSOLVER_C

//== INCLUDES =================================================================

#include "ArpackSolver.hh"

//== COMPILE-TIME PACKAGE REQUIREMENTS ========================================
#if (COMISO_ARPACK_AVAILABLE && COMISO_SUITESPARSE_AVAILABLE && COMISO_EIGEN3_AVAILABLE)

//== NAMESPACES ===============================================================

namespace COMISO {

//== IMPLEMENTATION ==========================================================


template<class MatrixT,class MatrixT2>
void
ArpackSolver::
solve(const MatrixT&       _A,
      std::vector<double>& _eigenvalues,
      MatrixT2&            _eigenvectors,
      const int            _n_eigvalues,
      const char*          _which_eigs  )
{
    Matrix A(_A);
//    ARSymStdEig<double, Matrix> eig_prob(A.matrix().cols(), _n_eigvalues, &A, &Matrix::mult_Mv, (char*)_which_eigs,
//                                         0, 0.0, 2000);

    ARSymStdEig<double, Matrix> eig_prob(A.matrix().cols(), _n_eigvalues, &A, &Matrix::mult_Mv, (char*)_which_eigs,
                                         0, 0.0, 100000);

    int n_converged = eig_prob.FindEigenvectors();

    // store result
    _eigenvalues.resize(n_converged);
    _eigenvectors.resize(A.matrix().rows(),n_converged);

    for( int i=0; i<n_converged; ++i)
    {
      _eigenvalues[i] = eig_prob.Eigenvalue(i);
      for(int j = 0; j<A.matrix().rows(); ++j)
        _eigenvectors.coeffRef(j,i) = eig_prob.RawEigenvector(i)[j];
    }
}


//-----------------------------------------------------------------------------


template<class MatrixT,class MatrixT2>
void
ArpackSolver::
solve_inverse(const MatrixT&       _A,
    std::vector<double>& _eigenvalues,
    MatrixT2&            _eigenvectors,
    const int            _n_eigvalues,
    const char*          _which_eigs)
{
  Matrix A(_A,true);
  ARSymStdEig<double, Matrix> eig_prob(A.matrix().cols(), _n_eigvalues, &A, &Matrix::mult_M_inv_v, (char*)_which_eigs,
      0, 0.0, 2000);

  //    ARSymStdEig(int np, int nevp, ARFOP* objOPp,
  //                void (ARFOP::* MultOPxp)(ARFLOAT[], ARFLOAT[]),
  //                char* whichp = "LM", int ncvp = 0, ARFLOAT tolp = 0.0,
  //                int maxitp = 0, ARFLOAT* residp = NULL, bool ishiftp = true);

  int n_converged = eig_prob.FindEigenvectors();

  // store result
  _eigenvalues.resize(n_converged);
  _eigenvectors.resize(A.matrix().rows(),n_converged);

  for( int i=0; i<n_converged; ++i)
  {
    _eigenvalues[i] = eig_prob.Eigenvalue(i);
    for(int j = 0; j<A.matrix().rows(); ++j)
      _eigenvectors.coeffRef(j,i) = eig_prob.RawEigenvector(i)[j];
  }
}


//-----------------------------------------------------------------------------


template<class MatrixT,class MatrixT2>
void
ArpackSolver::
check_result(const MatrixT& _A, std::vector<double>& _eigenvalues, MatrixT2& _eigenvectors)
{
  int n=_eigenvectors.rows();

  if(n<20)
    std::cerr << _A << std::endl;

  for(unsigned int i=0; i<_eigenvalues.size(); ++i)
  {
    std::cerr << "eigenvalue " << i << ": " << _eigenvalues[i] << ", ";
    if(n < 20)
    {
      std::cerr << "eigenvector: ";
      for(int j=0; j<n; ++j)
      {
        std::cerr << _eigenvectors.coeffRef(j,i) << ", ";
      }
    }

    // compute residuum
    Eigen::Matrix<double, Eigen::Dynamic, 1> v = _eigenvectors.block(0,i,n,1);
    std::cerr << "residuum norm: " << (_A*v - _eigenvalues[i]*v).norm() << std::endl;
  }
}

//-----------------------------------------------------------------------------



//=============================================================================
} // namespace COMISO
//=============================================================================

//=============================================================================
#endif // COMISO_SUITESPARSE_AVAILABLE
//=============================================================================
