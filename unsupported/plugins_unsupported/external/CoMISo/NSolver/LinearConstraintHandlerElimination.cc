//=============================================================================
//
//  CLASS LinearConstraintHandlerElimination - IMPLEMENTATION TEMPLATES
//
//=============================================================================



//== INCLUDES =================================================================

#include "LinearConstraintHandlerElimination.hh"

//== NAMESPACES ===============================================================

namespace COMISO {

//== IMPLEMENTATION ==========================================================


void
LinearConstraintHandlerElimination::
initialize_identity(int _n)
{
  n_ = _n;
  n_red_ = _n;
  m_ = 0;
  b_.resize(n_);
  gmm::resize(C_, n_, n_);
  gmm::resize(Ct_, n_, n_);
  gmm::clear(C_);
  gmm::clear(Ct_);
  for(int i=0; i<n_; ++i)
  {
    b_[i] = 0.0;
    Ct_(i,i) = 1.0;
    C_(i,i) = 1.0;
  }
}

//-----------------------------------------------------------------------------

void
LinearConstraintHandlerElimination::
initialize( const std::vector<double>& _c)
{
 if( _c.size() )
   initialize( (double*) &(_c[0]));
}


//-----------------------------------------------------------------------------


void
LinearConstraintHandlerElimination::
initialize( double* _c)
{
  // TODO
}

//-----------------------------------------------------------------------------

void
LinearConstraintHandlerElimination::
transform_x( const std::vector<double>& _x, std::vector<double>& _xC)
{
  _xC.resize(n_red_);
  if( _x.size() && _xC.size())
    transform_x((double*)&(_x[0]), &(_xC[0]));
}


//-----------------------------------------------------------------------------


void
LinearConstraintHandlerElimination::
transform_x( double* _x, double* _xC)
{
  // directly exploit orthogonality of QR-factorization
  // _xC = Ct_*(_x-b_)

  Vtemp_.resize(n_);
  gmm::add(VectorPT(_x,n_), gmm::scaled(b_,-1.0), Vtemp_);
  gmm::mult(Ct_,Vtemp_, VectorPT(_xC,n_red_));

  //  // set up least squares problem
//  // Ct (_x - b_) = CtC _xC
//
//  Vtemp_.resize(n_);
//  Vtemp2_.resize(n_red_);
//  gmm::add(VectorPT(_x,n_), gmm::scaled(b_,-1.0), Vtemp_);
//  gmm::mult(Ct_,Vtemp_, Vtemp2_);
//
//  // solve least squares problem
//  if( n_red_)
//    chol_CtC_.solve(_xC, (double*)(&(Vtemp2_[0])));
}


//-----------------------------------------------------------------------------


void
LinearConstraintHandlerElimination::
inv_transform_x( const std::vector<double>& _xC, std::vector<double>& _x)
{
  _x.resize(n_);
  if( _x.size())
    inv_transform_x( (double*)&(_xC[0]), &(_x[0]));
}


//-----------------------------------------------------------------------------


void
LinearConstraintHandlerElimination::
inv_transform_x( double* _xC, double* _x)
{
  gmm::copy(b_, VectorPT(_x, n_));
  gmm::mult_add(C_, VectorPT(_xC, n_red_), VectorPT(_x, n_));
}


//-----------------------------------------------------------------------------


void
LinearConstraintHandlerElimination::
project_x( const std::vector<double>& _x, std::vector<double>& _xp)
{
  _xp.resize(n_);
  if( _x.size())
    project_x((double*)&(_x[0]), &(_xp[0]));
}


//-----------------------------------------------------------------------------


void
LinearConstraintHandlerElimination::
project_x( double* _x, double* _xp)
{
  if( n_ == n_red_)   // special case of no constraints
  {
    // just copy
    gmm::copy(VectorPT(_x, n_), VectorPT(_xp, n_));
  }
  else
  {
    Vtemp_.resize( n_red_);

    transform_x    (_x           , &(Vtemp_[0]));
    inv_transform_x( &(Vtemp_[0]), _xp       );
  }
}


//-----------------------------------------------------------------------------


void
LinearConstraintHandlerElimination::
project_dx( const std::vector<double>& _x, std::vector<double>& _xp)
{
  _xp.resize(n_);
  if( _x.size())
    project_dx((double*)&(_x[0]), &(_xp[0]));
}


//-----------------------------------------------------------------------------


void
LinearConstraintHandlerElimination::
project_dx( double* _x, double* _xp)
{
  if( n_ == n_red_)   // special case of no constraints
  {
    // just copy
    gmm::copy(VectorPT(_x, n_), VectorPT(_xp, n_));
  }
  else
  {
    // idea: C_ is an orthonormal basis of the kernel
    // -> simply project out the non-constraint part

    Vtemp_.resize( n_red_);

    gmm::mult(Ct_, VectorPT(_x, n_), Vtemp_);
    gmm::mult(C_, Vtemp_, VectorPT(_xp, n_));

//    // check result
//    std::cerr << "check result..." << std::endl;
//    Vtemp_.resize(b_orig_.size());
//    gmm::mult(A_orig_, VectorPT(_xp, n_), Vtemp_);
////    gmm::add(gmm::scaled(b_orig_, -1.0), Vtemp_);
//    std::cerr << "check constraint update... " << gmm::vect_norm2(Vtemp_) << std::endl;
//
////    std::cerr << "check constraint update... " << COMISO_GMM::residuum_norm<RMatrix,std::vector<double> >(A_orig_,Vtemp_, b_orig_) << std::endl;
  }
}


//-----------------------------------------------------------------------------


void
LinearConstraintHandlerElimination::
transform_gradient( const std::vector<double>& _g, std::vector<double>& _gC)
{
  _gC.resize(n_red_);
  if( _g.size() && _gC.size())
    transform_gradient( (double*)&(_g[0]), &(_gC[0]));
}


//-----------------------------------------------------------------------------


void
LinearConstraintHandlerElimination::
transform_gradient( double* _g, double* _gC)
{
  gmm::mult( Ct_, VectorPT(_g, n_), VectorPT(_gC, n_red_));
}


//-----------------------------------------------------------------------------


void
LinearConstraintHandlerElimination::
transform_hessian( const RMatrix& _H, RMatrix& _HC)
{
  // resize and clear matrix
  gmm::resize(_HC, n_red_, n_red_);
  gmm::clear(_HC);

  gmm::resize(Mtemp_, n_, n_red_);
  gmm::mult(_H,C_, Mtemp_);
  gmm::mult(Ct_, Mtemp_, _HC);
}


//-----------------------------------------------------------------------------


//=============================================================================
} // namespace COMISO
//=============================================================================
