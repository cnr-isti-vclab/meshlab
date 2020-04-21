//=============================================================================
//
//  CLASS LinearConstraintHandlerPenalty - IMPLEMENTATION
//
//=============================================================================

#define COMISO_LINEARCONSTRAINTHANDLERPENALTY_C

//== INCLUDES =================================================================

#include "LinearConstraintHandlerPenalty.hh"

//== NAMESPACES ===============================================================

namespace COMISO {

//== IMPLEMENTATION ==========================================================

// initialize Constructor
template<class MatrixT, class VectorT>
LinearConstraintHandlerPenalty::LinearConstraintHandlerPenalty( const MatrixT& _C, const VectorT& _c) :
  penalty_(10000)
{initialize(_C, _c); }

template<class MatrixT, class VectorT>
void
LinearConstraintHandlerPenalty::
initialize( const MatrixT& _C, const VectorT& _c)
{
  gmm::resize(C_, gmm::mat_nrows(_C), gmm::mat_ncols(_C));
  gmm::copy(_C,C_);
  b_ = _c;

  n_     = gmm::mat_ncols(_C);
  m_     = b_.size();

  // initialize penalty stuff
  // penalty_H_      = 2*penalty*A^t A
  // penalty_grad_b_ = -2*penalty*A^t b
  gmm::resize(penalty_H_, n_, n_);
  gmm::clear(penalty_H_);
  RMatrix temp(n_,m_);
  gmm::copy( gmm::transposed(C_), temp);
  gmm::mult( temp,C_, penalty_H_);
  gmm::scale(penalty_H_, 2.0*penalty_);

  penalty_grad_b_.clear();
  penalty_grad_b_.resize(n_);
  gmm::mult(gmm::transposed(C_), b_, penalty_grad_b_);
  gmm::scale(penalty_grad_b_, -2.0*penalty_);
}

//-----------------------------------------------------------------------------



//=============================================================================
} // namespace COMISO
//=============================================================================
