//=============================================================================
//
//  CLASS NConstraintInterface
//
//=============================================================================


#ifndef COMISO_NCONSTRAINTINTERFACE_HH
#define COMISO_NCONSTRAINTINTERFACE_HH

//== COMPILE-TIME PACKAGE REQUIREMENTS ========================================
#include <CoMISo/Config/config.hh>
#if COMISO_EIGEN3_AVAILABLE

//== INCLUDES =================================================================

#include <CoMISo/Config/CoMISoDefines.hh>
#include "SuperSparseMatrixT.hh"


#define EIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET
#include <Eigen/Sparse>


//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace COMISO { 

//== CLASS DEFINITION =========================================================

	      

/** \class NProblemGmmInterface NProblemGmmInterface.hh <ACG/.../NPRoblemGmmInterface.hh>

    Brief Description.
  
    A more elaborate description follows.
*/
class COMISODLLEXPORT NConstraintInterface
{
public:
  
  // define Sparse Datatypes
  typedef Eigen::SparseVector<double> SVectorNC;
  typedef SuperSparseMatrixT<double>  SMatrixNC;

  // different types of constraints
  enum ConstraintType {NC_EQUAL, NC_LESS_EQUAL, NC_GREATER_EQUAL};

  /// Default constructor
  NConstraintInterface(const ConstraintType _type = NC_EQUAL, double _eps = 1e-6) : type_(_type) {}
 
  /// Destructor
  virtual ~NConstraintInterface() {}

  virtual int             n_unknowns      (                                      ) = 0;
  virtual double          eval_constraint ( const double* _x                     ) = 0;
  virtual void            eval_gradient   ( const double* _x, SVectorNC& _g      ) = 0;
  virtual void            eval_hessian    ( const double* _x, SMatrixNC& _h      ) = 0;

  virtual ConstraintType& constraint_type (                                      ) { return type_; }

  virtual bool            is_satisfied    ( const double* _x, double _eps = 1e-6)
  {
    switch( type_)
    {
      case NC_EQUAL        : return (fabs(eval_constraint(_x)) <=  _eps); break;
      case NC_LESS_EQUAL   : return (     eval_constraint(_x)  <=  _eps); break;
      case NC_GREATER_EQUAL: return (     eval_constraint(_x)  >= -_eps); break;
    }
    return false;
  }

  // provide special properties
  virtual bool   is_linear()         const { return false;}
  virtual bool   constant_gradient() const { return false;}
  virtual bool   constant_hessian () const { return false;}

  virtual double gradient_update_factor( const double* _x, double _eps = 1e-6)
  {
    double val = eval_constraint(_x);
    bool   upper_bound_ok = ( val <=  _eps);
    bool   lower_bound_ok = ( val >= -_eps);

    if(upper_bound_ok)
    {
      if(lower_bound_ok || type_ == NC_LESS_EQUAL) return 0.0;
      else                                         return 1.0;
    }
    else
    {
      if(lower_bound_ok && type_ == NC_GREATER_EQUAL) return  0.0;
      else                                            return -1.0;
    }
  }


private:
  // constraint type
  ConstraintType type_;
};


//=============================================================================
} // namespace COMISO
//=============================================================================
#endif // COMISO_EIGEN3_AVAILABLE
//=============================================================================
#endif // ACG_NCONSTRAINTINTERFACE_HH defined
//=============================================================================

