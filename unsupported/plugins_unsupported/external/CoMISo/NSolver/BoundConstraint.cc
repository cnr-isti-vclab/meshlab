//=============================================================================
//
//  CLASS BoundConstraint - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "BoundConstraint.hh"


//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace COMISO { 

//== CLASS DEFINITION =========================================================

	      

BoundConstraint::
BoundConstraint(const unsigned int  _var_idx,   // index of variable for bound constraint
                const double        _bound,     // bound: x(_var_idx) #_type, <,=,># _bound
                const unsigned int   _n,        // number of unknowns in problem
                const ConstraintType _type)     // type of bound upper, lower or both (equal)
  : NConstraintInterface(_type), idx_(_var_idx), bound_(_bound), n_(_n)
{
}


//-----------------------------------------------------------------------------

BoundConstraint::
~BoundConstraint()
{
}


//-----------------------------------------------------------------------------


int
BoundConstraint::
n_unknowns()
{
  return n_;
}


//-----------------------------------------------------------------------------

double
BoundConstraint::
eval_constraint ( const double* _x                     )
{
  return _x[idx_] - bound_;
}


//-----------------------------------------------------------------------------

void
BoundConstraint::
eval_gradient   ( const double* _x, SVectorNC& _g      )
{
  _g.resize(n_); _g.coeffRef(idx_) = 1.0;
}


//-----------------------------------------------------------------------------


void
BoundConstraint::
eval_hessian    ( const double* _x, SMatrixNC& _h      )
{
  _h.clear(); _h.resize(n_,n_);
}


//-----------------------------------------------------------------------------


bool
BoundConstraint::
is_linear() const
{
  return true;
}


//-----------------------------------------------------------------------------


bool
BoundConstraint::
constant_gradient() const
{
  return true;
}


//-----------------------------------------------------------------------------


bool
BoundConstraint::
constant_hessian() const
{
  return true;
}


//-----------------------------------------------------------------------------


unsigned int&
BoundConstraint::
idx()
{
  return idx_;
}

//-----------------------------------------------------------------------------


double&
BoundConstraint::
bound()
{
  return bound_;
}


//-----------------------------------------------------------------------------


unsigned int&
BoundConstraint::
n()
{
  return n_;
}


//-----------------------------------------------------------------------------


void
BoundConstraint::
resize(const unsigned int _n)
{
  n_ = _n;
}


//=============================================================================
} // namespace COMISO
//=============================================================================
