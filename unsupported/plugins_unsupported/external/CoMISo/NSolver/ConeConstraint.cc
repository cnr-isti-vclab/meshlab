//=============================================================================
//
//  CLASS ConerConstraint
//
//=============================================================================


#ifndef COMISO_CONECONSTRAINT_CC
#define COMISO_CONECONSTRAINT_CC


//== INCLUDES =================================================================

#include <CoMISo/Config/CoMISoDefines.hh>
#include "NConstraintInterface.hh"
#include "ConeConstraint.hh"

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace COMISO {

//== Implementation =========================================================

      
/// Default constructor
ConeConstraint::ConeConstraint()
: NConstraintInterface(NConstraintInterface::NC_GREATER_EQUAL)
{
  Q_.clear();
  i_ = 1.0;
  c_ = 1.0;
}

// cone constraint of the form -> 0.5*(c_ * x(i_)^2 - x^T Q_ x) >= 0
ConeConstraint::ConeConstraint(const double _c, const int _i, const SMatrixNC& _Q)
: NConstraintInterface(NConstraintInterface::NC_GREATER_EQUAL),
  c_(_c), i_(_i), Q_(_Q)
{
}

/// Destructor
ConeConstraint::~ConeConstraint() {}

int ConeConstraint::n_unknowns()
{
  return Q_.cols();
}

void ConeConstraint::resize(const unsigned int _n)
{
  Q_.resize(_n,_n);
}

void ConeConstraint::clear()
{
  Q_.clear();
}


const ConeConstraint::SMatrixNC& ConeConstraint::Q() const
{
  return Q_;
}
ConeConstraint::SMatrixNC& ConeConstraint::Q()
{
  return Q_;
}

const int&    ConeConstraint::i() const
{
  return i_;
}
int&    ConeConstraint::i()
{ 
  return i_;
}

const double&    ConeConstraint::c() const
{
  return c_;
}
double&    ConeConstraint::c()
{ 
  return c_;
}

double ConeConstraint::eval_constraint ( const double* _x )
{
  // cone constraint of the form -> 0.5*(c_ * x(i_)^2 - x^T Q_ x) >= 0
  double v = c_*_x[i_]*_x[i_];

  SMatrixNC::iterator m_it  = Q_.begin();
  SMatrixNC::iterator m_end = Q_.end();

  for(; m_it != m_end; ++m_it)
  {
    v -= (*m_it)*_x[m_it.row()]*_x[m_it.col()];
  }

  return 0.5*v;
}

void ConeConstraint::eval_gradient( const double* _x, SVectorNC& _g      )
{
  _g.setZero();
  _g.resize(Q_.rows());

  SMatrixNC::iterator m_it  = Q_.begin();
  SMatrixNC::iterator m_end = Q_.end();

  for(; m_it != m_end; ++m_it)
  {
    _g.coeffRef(m_it.row()) -= (*m_it)*_x[m_it.col()];
  }

  _g.coeffRef(i_) += c_*_x[i_];
}

void ConeConstraint::eval_hessian    ( const double* _x, SMatrixNC& _h      )
{
  _h = Q_;
  _h.scale(-1.0);
  _h(i_,i_) += c_;
}


//=============================================================================
} // namespace COMISO
//=============================================================================
#endif // ACG_ConeConstraint_HH defined
//=============================================================================

