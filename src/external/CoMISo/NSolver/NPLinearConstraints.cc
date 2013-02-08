//=============================================================================
//
//  CLASS NPLinearConstraints
//
//=============================================================================


//== INCLUDES =================================================================

#include "NPLinearConstraints.hh"
#include "NProblemGmmInterface.hh"
#include "LinearConstraintHandlerElimination.hh"
#include "LinearConstraintHandlerPenalty.hh"
//#include "LinearConstraintHandlerLagrange.hh"


//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace COMISO {

//== CLASS DEFINITION =========================================================

/// Default constructor having a pointer to the main problem
NPLinearConstraintsElimination::NPLinearConstraintsElimination( NProblemGmmInterface* _np) : base_(_np), cache_initialized_(false)
{
  if( !base_) std::cerr << "Warning: initialized NPLinearConstraints with zero pointer...\n";
  this->initialize_identity(base_->n_unknowns());
}

/// Destructor
NPLinearConstraintsElimination::~NPLinearConstraintsElimination() {}

// NSolverInterface
int NPLinearConstraintsElimination::n_unknowns   ()
{ return this->n_reduced(); }

void NPLinearConstraintsElimination::initial_x    ( double* _x )
{
  // transform initial x from parent NP
  x_.resize(this->n());
  base_->initial_x(P(x_));
  this->transform_x(P(x_), _x);
  cache_initialized_ = false;
  update_x(_x);
}

double NPLinearConstraintsElimination::eval_f( const double* _x )
{
  update_x(_x);

  return base_->eval_f(P(x_));
}

void   NPLinearConstraintsElimination::eval_gradient( const double* _x, double*    _g)
{
  update_x(_x);
  vtemp_.resize(this->n());
  base_->eval_gradient( P(x_), P(vtemp_));
  this->transform_gradient( P(vtemp_), _g);
}

void   NPLinearConstraintsElimination::eval_hessian ( const double* _x, SMatrixNP& _H)
{
  update_x(_x);
  SMatrixNP H;
  base_->eval_hessian(P(x_), H);
  this->transform_hessian(H,_H);
}

void   NPLinearConstraintsElimination::store_result ( const double* _x )
{
  update_x(_x);

  base_->store_result( P(x_));
}

void NPLinearConstraintsElimination::update_x(const double* _xr)
{
  if(!cache_initialized_)
  {
    x_.resize(this->n());
    xr_.resize(this->n_reduced());

    if(!xr_.empty())
      xr_[0] = _xr[0] + 1.0;
    else
    {
      x_.resize(this->n());
      this->inv_transform_x(xr_, x_);
      cache_initialized_ = true;
    }
  }

  for( int i=0; i<this->n_reduced(); ++i)
    if( _xr[i] != xr_[i])
    {
      gmm::copy( VectorPT((double*)_xr, this->n_reduced()), this->xr_);
      x_.resize(this->n());
      this->inv_transform_x(xr_, x_);

      cache_initialized_ = true;

      //quit
      return;
    }
}

// convert vector into pointer
double* NPLinearConstraintsElimination::P(const std::vector<double>& _v)
{
  if(_v.size())
    return (double*) (&(_v[0]));
  else
    return 0;
}


//=============================================================================


  /// Default constructor having a pointer to the main problem
NPLinearConstraintsPenalty::NPLinearConstraintsPenalty( NProblemGmmInterface* _np) : base_(_np)
{ if( !base_) std::cerr << "Warning: initialized NPLinearConstraints with zero pointer...\n"; }

  /// Destructor
NPLinearConstraintsPenalty::~NPLinearConstraintsPenalty()
{}

// NSolverInterface
int    NPLinearConstraintsPenalty::n_unknowns   ()
{ return base_->n_unknowns(); }

void   NPLinearConstraintsPenalty::initial_x    ( double* _x )
{
  base_->initial_x(_x);
}

double NPLinearConstraintsPenalty::eval_f( const double* _x )
{
  return this->add_penalty_f((double*)_x, base_->eval_f(_x));
}

void   NPLinearConstraintsPenalty::eval_gradient( const double* _x, double*    _g)
{
  base_->eval_gradient( _x, _g);
  this->add_penalty_gradient((double*)_x, _g);
}

void   NPLinearConstraintsPenalty::eval_hessian ( const double* _x, SMatrixNP& _H)
{
  base_->eval_hessian(_x, _H);
  this->add_penalty_hessian(_H);
}

void   NPLinearConstraintsPenalty::store_result ( const double* _x )
{
  base_->store_result( _x);
}


//=============================================================================
} // namespace ACG
//=============================================================================


