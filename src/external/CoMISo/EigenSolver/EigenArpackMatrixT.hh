//=============================================================================
//
//  CLASS EigenArpackMatrixT
//
//=============================================================================


#ifndef COMISO_EIGENARPACKMATRIXT_HH
#define COMISO_EIGENARPACKMATRIXT_HH

//== COMPILE-TIME PACKAGE REQUIREMENTS ========================================
#include <CoMISo/Config/config.hh>
#if (COMISO_SUITESPARSE_AVAILABLE && COMISO_EIGEN3_AVAILABLE)
//=============================================================================


//== INCLUDES =================================================================

#include <iostream>
#include <Eigen/Eigen>

#if EIGEN_VERSION_AT_LEAST(3,1,0)
  #include <Eigen/CholmodSupport>
#else
  #define EIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET
  #include <unsupported/Eigen/CholmodSupport>
#endif
#include <Eigen/Sparse>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace COMISO {

//== CLASS DEFINITION =========================================================


/** \class EigenArpackMatrixT EigenArpackMatrixT.hh <COMISO/.../EigenArpackMatrixT.hh>

    Brief Description.
  
    A more elaborate description follows.
*/

template <class RealT,class MatrixT>
class EigenArpackMatrixT
{
public:

  typedef MatrixT Matrix;
  typedef RealT   Real;
   
  /// Constructor
  template<class MatrixT2>
  EigenArpackMatrixT(const MatrixT2& _m, bool _use_inverse = false)
  {
    mat_ = _m;

    if(_use_inverse)
    {
      sllt_.compute(mat_);

#if EIGEN_VERSION_AT_LEAST(3,1,0)      
      if ( !sllt_.info() != Eigen::Success )
#else
      if ( !sllt_.succeeded() )
#endif      
        std::cout << "[ERROR] EigenArpackMatrix(): Could not compute llt factorization." << std::endl;
    }
  }
 
  /// Destructor
  ~EigenArpackMatrixT() {}
  
  // get reference on matrix
  Matrix& matrix() { return mat_; }

  // matrix-vector multiplication _w = mat_*_v
  void mult_Mv(Real* _v, Real* _w)
  {
    Eigen::Map<Eigen::Matrix<Real, Eigen::Dynamic, 1> >  v(_v,mat_.rows());     // uses v as a ArrayXf object
    Eigen::Map<Eigen::Matrix<Real, Eigen::Dynamic, 1> >  w(_w,mat_.cols());     // uses w as a ArrayXf object

    w = mat_*v;
  }

  // matrix-vector multiplication _w = mat_*_v
  void mult_M_inv_v(Real* _v, Real* _w)
  {
    Eigen::Map<Eigen::Matrix<Real, Eigen::Dynamic, 1> >  v(_v,mat_.rows());     // uses v as a ArrayXf object
    Eigen::Map<Eigen::Matrix<Real, Eigen::Dynamic, 1> >  w(_w,mat_.cols());     // uses w as a ArrayXf object

    w = sllt_.solve(v);

//    std::cerr << "input:" << std::endl;
//    std::cerr << v << std::endl;
//    std::cerr << "output:" << std::endl;
//    std::cerr << w << std::endl;
  }

private:

  Matrix mat_;

#if EIGEN_VERSION_AT_LEAST(3,1,0) 
  Eigen::CholmodSupernodalLLT<Eigen::SparseMatrix<Real> > sllt_;
#else
  Eigen::SparseLLT<Eigen::SparseMatrix<Real>, Eigen::Cholmod> sllt_;
#endif
};


//=============================================================================
} // namespace COMISO
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(COMISO_EIGENARPACKMATRIXT_C)
#define COMISO_EIGENARPACKMATRIXT_TEMPLATES
#include "EigenArpackMatrixT.cc"
#endif
//=============================================================================
#endif // COMISO_SUITESPARSE_AVAILABLE
//=============================================================================
#endif // COMISO_EIGENARPACKMATRIXT_HH defined
//=============================================================================

