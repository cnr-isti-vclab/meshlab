//=============================================================================
//
//  CLASS LinearConstraint
//
//=============================================================================


#ifndef COMISO_LINEARCONSTRAINT_HH
#define COMISO_LINEARCONSTRAINT_HH


//== INCLUDES =================================================================

#include <CoMISo/Config/CoMISoDefines.hh>
#include "NConstraintInterface.hh"
#include <Eigen/StdVector>


//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace COMISO {

//== CLASS DEFINITION =========================================================

	      

/** \class NProblemGmmInterface NProblemGmmInterface.hh <ACG/.../NPRoblemGmmInterface.hh>

    Brief Description.
  
    A more elaborate description follows.
*/
class COMISODLLEXPORT LinearConstraint : public NConstraintInterface
{
public:

  // sparse vector type
  typedef NConstraintInterface::SVectorNC SVectorNC;

  // different types of constraints
//  enum ConstraintType {NC_EQUAL, NC_LESS_EQUAL, NC_GREATER_EQUAL};

  /// Default constructor
  LinearConstraint(const ConstraintType _type = NC_EQUAL);

  // linear equation of the form -> coeffs_^T *x  + b_=_type= 0
  LinearConstraint(const SVectorNC& _coeffs, const double _b, const ConstraintType _type = NC_EQUAL);

  /// Destructor
  virtual ~LinearConstraint();

  virtual int n_unknowns();

  // resize coefficient vector = #unknowns
  void  resize(const unsigned int _n);

  // clear to zero constraint 0 =_type 0
  void  clear();

  const SVectorNC& coeffs() const;
        SVectorNC& coeffs();

  const double&    b() const;
        double&    b();

  virtual double eval_constraint ( const double* _x );
  
  virtual void eval_gradient( const double* _x, SVectorNC& _g      );

  virtual void eval_hessian    ( const double* _x, SMatrixNC& _h      );

  virtual bool   is_linear()         const { return true;}
  virtual bool   constant_gradient() const { return true;}
  virtual bool   constant_hessian () const { return true;}

  // inherited from base
//  virtual ConstraintType  constraint_type (                                      ) { return type_; }

private:

  // linear equation of the form -> coeffs_^T * x + b_
  SVectorNC coeffs_;
  double    b_;
};


//=============================================================================
} // namespace COMISO
//=============================================================================
// support std vectors
EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(COMISO::LinearConstraint);
//=============================================================================
#endif // ACG_LINEARCONSTRAINT_HH defined
//=============================================================================

