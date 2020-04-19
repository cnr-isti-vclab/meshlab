//=============================================================================
//
//  CLASS LinearConstraintHandlerPenalty - IMPLEMENTATION TEMPLATES
//
//=============================================================================



//== INCLUDES =================================================================

#include "LinearConstraintHandlerPenalty.hh"

//== NAMESPACES ===============================================================

namespace COMISO {

//== IMPLEMENTATION ==========================================================


/// Constructor
LinearConstraintHandlerPenalty::LinearConstraintHandlerPenalty() : penalty_(10000) {}


//-----------------------------------------------------------------------------

/// Destructor
LinearConstraintHandlerPenalty::~LinearConstraintHandlerPenalty()
{}

//-----------------------------------------------------------------------------

// penalty weight
double& LinearConstraintHandlerPenalty::penalty()
{ return penalty_; }

//-----------------------------------------------------------------------------

// number of variables
int LinearConstraintHandlerPenalty::n()
{return n_;}

//-----------------------------------------------------------------------------

// number of linearly independent constraints (n-n_reduced)
int LinearConstraintHandlerPenalty::n_constraints()
{ return m_;}

//-----------------------------------------------------------------------------

void
LinearConstraintHandlerPenalty::
initialize( const std::vector<double>& _c)
{
 if( _c.size() )
   initialize( (double*) &(_c[0]));
}


//-----------------------------------------------------------------------------


void
LinearConstraintHandlerPenalty::
initialize( double* _c)
{
  for(unsigned int i=0; i<b_.size(); ++i)
    b_[i] = _c[i];

  penalty_grad_b_.resize(n_);
  gmm::mult(gmm::transposed(C_), b_, penalty_grad_b_);
  gmm::scale(penalty_grad_b_, -2.0*penalty_);
}

//-----------------------------------------------------------------------------

double
LinearConstraintHandlerPenalty::
add_penalty_f( double* _x, const double _f)
{

  temp_.resize(b_.size());
  gmm::mult(C_, VectorPT(_x, n_), temp_);
  gmm::add(gmm::scaled(b_, -1.0), temp_);

  double fp = ( penalty_* (gmm::vect_sp(temp_, temp_)) );

  return _f + fp;
}

//-----------------------------------------------------------------------------


void
LinearConstraintHandlerPenalty::
add_penalty_gradient( const std::vector<double>& _x, std::vector<double>& _g)
{
  _g.resize(n_);
  if( _x.size() && _g.size())
    add_penalty_gradient( (double*)&(_x[0]), &(_g[0]));
}


//-----------------------------------------------------------------------------


void
LinearConstraintHandlerPenalty::
add_penalty_gradient( double* _x, double* _g)
{
  gmm::add( penalty_grad_b_, VectorPT(_g, n_));
  gmm::mult_add(penalty_H_, VectorPT(_x, n_), VectorPT(_g, n_));
}


//-----------------------------------------------------------------------------


void
LinearConstraintHandlerPenalty::
add_penalty_hessian( RMatrix& _H)
{
  gmm::add(penalty_H_, _H);
}


//-----------------------------------------------------------------------------


//=============================================================================
} // namespace COMISO
//=============================================================================
