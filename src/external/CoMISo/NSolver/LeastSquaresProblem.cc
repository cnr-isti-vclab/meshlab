//=============================================================================
//
//  CLASS LeastSquaresProblem - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include "LeastSquaresProblem.hh"


//== NAMESPACES ===============================================================

namespace COMISO {

//== IMPLEMENTATION ========================================================== 


//-----------------------------------------------------------------------------


void
LeastSquaresProblem::
add_term(NConstraintInterface* _term)
{
  terms_.push_back(_term);
}


//-----------------------------------------------------------------------------


double
LeastSquaresProblem::
eval_term(const unsigned int _i, const double* _x)
{
  if(_i >= terms_.size())
  {
    std::cerr << "ERROR: bad index in LeastSquaresProblem::eval_term" << std::endl;
    return 0.0;
  }

  double vterm = terms_[_i]->eval_constraint(_x);

  return vterm*vterm;
}


//-----------------------------------------------------------------------------


double
LeastSquaresProblem::
eval_term(const unsigned int _i)
{
  if(!x_.empty())
    return eval_term(_i, &(x_[0]));
  else
  {
    std::cerr << "warning: called eval_term with zero unknowns..." << std::endl;
    return 0.0;
  }
}


//-----------------------------------------------------------------------------


int
LeastSquaresProblem::
n_unknowns   (                                )
{
  return n_;
}


//-----------------------------------------------------------------------------


void
LeastSquaresProblem::
initial_x    (       double* _x               )
{
  for( int i=0; i<this->n_unknowns(); ++i)
    _x[i] = x_[i];
}


//-----------------------------------------------------------------------------


double
LeastSquaresProblem::
eval_f       ( const double* _x )
{
  double vtot(0.0);

  for(unsigned int i=0; i<terms_.size(); ++i)
  {
    double vterm = terms_[i]->eval_constraint(_x);
    vtot += vterm*vterm;
  }

  return vtot;
}


//-----------------------------------------------------------------------------


void
LeastSquaresProblem::
eval_gradient( const double* _x, double*    _g)
{
  // clear gradient
  for( int i=0; i<this->n_unknowns(); ++i)
    _g[i] = 0.0;

  for(unsigned int i=0; i<terms_.size(); ++i)
  {
    // get local function value
    double vterm = terms_[i]->eval_constraint(_x);

    // get local gradient
    NConstraintInterface::SVectorNC gterm;
    terms_[i]->eval_gradient(_x, gterm);

    // add terms to global gradient
    NConstraintInterface::SVectorNC::InnerIterator v_it(gterm);
    for( ; v_it; ++v_it)
    {
      _g[v_it.index()] += 2.0*vterm*v_it.value();
    }
  }
}


//-----------------------------------------------------------------------------


void
LeastSquaresProblem::
eval_hessian ( const double* _x, SMatrixNP& _H)
{
  // clear old data
  _H.resize(n_unknowns(), n_unknowns());
  _H.setZero();

  for(unsigned int i=0; i<terms_.size(); ++i)
  {
    // get local function value
    double vterm = terms_[i]->eval_constraint(_x);

    // get local gradient
    NConstraintInterface::SVectorNC gterm;
    terms_[i]->eval_gradient(_x, gterm);

    // add terms to global gradient
    NConstraintInterface::SVectorNC::InnerIterator v_it (gterm);
    for( ; v_it; ++v_it)
    {
      NConstraintInterface::SVectorNC::InnerIterator v_it2(gterm);
      for( ; v_it2; ++v_it2)
        _H.coeffRef(v_it.index(), v_it2.index()) += 2.0*v_it.value()*v_it2.value();
    }

    NConstraintInterface::SMatrixNC Hterm;
    terms_[i]->eval_hessian(_x, Hterm);

    NConstraintInterface::SMatrixNC::iterator h_it  = Hterm.begin();
    NConstraintInterface::SMatrixNC::iterator h_end = Hterm.end();
    for(; h_it != h_end; ++h_it)
      _H.coeffRef(h_it.row(),h_it.col()) += 2.0*vterm*(*h_it);
  }
}


//-----------------------------------------------------------------------------


void
LeastSquaresProblem::
store_result ( const double* _x               )
{
  for( int i=0; i<this->n_unknowns(); ++i)
    x_[i] = _x[i];
}


//-----------------------------------------------------------------------------

bool
LeastSquaresProblem::
constant_hessian()
{
  for(unsigned int i=0; i<terms_.size(); ++i)
  {
    if(!terms_[i]->is_linear())
      return false;
  }

  return true;
}


//-----------------------------------------------------------------------------



//=============================================================================
} // namespace COMISO
//=============================================================================
