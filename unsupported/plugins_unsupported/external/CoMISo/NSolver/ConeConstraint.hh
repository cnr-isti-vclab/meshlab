//=============================================================================
//
//  CLASS ConeConstraint
//
//=============================================================================


#ifndef COMISO_CONECONSTRAINT_HH
#define COMISO_CONECONSTRAINT_HH


//== INCLUDES =================================================================

#include <CoMISo/Config/CoMISoDefines.hh>
#include "NConstraintInterface.hh"
#include <Eigen/StdVector>


//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace COMISO {

//== CLASS DEFINITION =========================================================

	      
class COMISODLLEXPORT ConeConstraint : public NConstraintInterface
{
public:

  // sparse vector type
  typedef NConstraintInterface::SVectorNC SVectorNC;
  typedef NConstraintInterface::SMatrixNC SMatrixNC;

  /// Default constructor
  ConeConstraint();

  // cone constraint of the form -> 0.5*(c_ * x(i_)^2 - x^T Q_ x) >= 0
  ConeConstraint(const double _c, const int _i, const SMatrixNC& _Q);

  /// Destructor
  virtual ~ConeConstraint();

  virtual int n_unknowns();

  // resize coefficient vector = #unknowns
  void  resize(const unsigned int _n);

  // clear to zero constraint 0 =_type 0
  void  clear();

  const double&    c() const;
        double&    c();

  const int&       i() const;
        int&       i();

  const SMatrixNC& Q() const;
        SMatrixNC& Q();


  virtual double eval_constraint ( const double* _x );
  
  virtual void eval_gradient( const double* _x, SVectorNC& _g      );

  virtual void eval_hessian    ( const double* _x, SMatrixNC& _h      );

  virtual bool   is_linear()         const { return false;}
  virtual bool   constant_gradient() const { return false;}
  virtual bool   constant_hessian () const { return true;}

private:

  // cone constraint of the form -> 0.5*(c_ * x(i_)^2 - x^T Q_ x) >= 0
  double    c_;
  int       i_;
  SMatrixNC Q_;
};


//=============================================================================
} // namespace COMISO
//=============================================================================
// support std vectors
EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(COMISO::ConeConstraint);
//=============================================================================
#endif // ACG_CONECONSTRAINT_HH defined
//=============================================================================

