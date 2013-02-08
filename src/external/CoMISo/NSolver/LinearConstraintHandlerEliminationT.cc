//=============================================================================
//
//  CLASS LinearConstraintHandlerElimination - IMPLEMENTATION
//
//=============================================================================

#define COMISO_LINEARCONSTRAINTHANDLERELIMINATION_C

//== INCLUDES =================================================================

#include "LinearConstraintHandlerElimination.hh"

#include <CoMISo/Solver/SparseQRSolver.hh>

//== NAMESPACES ===============================================================

namespace COMISO {

//== IMPLEMENTATION ==========================================================

template<class MatrixT, class VectorT>
void
LinearConstraintHandlerElimination::
initialize( const MatrixT& _C, const VectorT& _c)
{
  // no constraints?
  if( gmm::mat_nrows(_C) == 0)
  {
    initialize_identity( gmm::mat_ncols(_C));
  }
  else
  {

	  // require SPQR SOLVER!!!
#if(COMISO_SUITESPARSE_SPQR_AVAILABLE)
    // Construct constraints basis form via QR-factorization (see Nocedal 426...)
    // Constraints in basis transformation form x_orig = b_ + C_*x_reduced
    // notice that C_ is a basis of the nullspace of the constraints
    // and _C*b_ = _c (means b_ is one solution of the constraints)

    std::cerr << "Initialize Linear Constraint handler...\n";
    COMISO::SparseQRSolver sqr;
    //  sqr.calc_system_gmm(_C);
    CMatrix Q;
    CMatrix R;
    std::vector<int> P;
    int rank = sqr.factorize_system_gmm(gmm::transposed(_C), Q, R, P);

    // Q[0..m-1,rank..m-1] is basis of the nullspace -> C_, Ct_
    int m = gmm::mat_nrows(Q);
    gmm::resize(C_, m, m-rank);
    gmm::clear (C_);
    gmm::copy( gmm::sub_matrix(Q, gmm::sub_interval(0,m), gmm::sub_interval(rank,m-rank)), C_);
    gmm::resize(Ct_, gmm::mat_ncols(C_), gmm::mat_nrows(C_));
    gmm::copy( gmm::transposed(C_), Ct_);

    // compute b_
    b_.resize(gmm::mat_ncols(_C));
    // hack (too expensive, directly exploit Q,R,P from above)
    sqr.calc_system_gmm(_C);
    sqr.solve(b_, _c);

    n_     = gmm::mat_nrows(C_);
    n_red_ = gmm::mat_ncols(C_);
    m_     = n_ - n_red_;

    // hack -> store initial system
    if(0)
    {
      gmm::resize(A_orig_, gmm::mat_nrows(_C), gmm::mat_ncols(_C));
      gmm::resize(b_orig_, _c.size());
      gmm::copy(_C, A_orig_);
      gmm::copy(_c, b_orig_);
    }

//    RMatrix CtC(n_red_, n_red_);
//    gmm::mult(Ct_,C_, CtC);
//    std::cerr << "CtC\n";
//    std::cerr << CtC << std::endl;


    /*
  // set up least squares problem
  gmm::resize(Mtemp_, gmm::mat_ncols(C_), gmm::mat_ncols(C_));
  gmm::mult(Ct_, C_, Mtemp_);
  chol_CtC_.calc_system_gmm(Mtemp_);
     */

    //  std::cerr << "Q: " << Q  << std::endl;
    //  std::cerr << "R: " << R  << std::endl;
    //  std::cerr << "P: " << P  << std::endl;
    //  std::cerr << "C_:" << C_ << std::endl;
    //  std::cerr << "b_:" << b_ << std::endl;
    //
    //  std::cerr << "#rows: " << gmm::mat_nrows(_C) << std::endl;
    //  std::cerr << "#nullspace: " << m << std::endl;
    //  std::cerr << "rank: " << rank << std::endl;
    //  std::cerr << "dim Q = " << gmm::mat_nrows(Q) << " x " << gmm::mat_ncols(Q) << std::endl;
    //  std::cerr << "dim R = " << gmm::mat_nrows(R) << " x " << gmm::mat_ncols(R) << std::endl;
#else
    std::cerr << "ERROR: SQPR-Solver required by LinearConstraintHandlerElimination not available !!!" << std::endl;
#endif
  }
}

//-----------------------------------------------------------------------------



//=============================================================================
} // namespace COMISO
//=============================================================================
