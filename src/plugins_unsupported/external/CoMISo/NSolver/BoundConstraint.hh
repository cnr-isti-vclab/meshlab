//=============================================================================
//
//  CLASS BoundConstraint
//
//=============================================================================


#ifndef COMISO_BOUNDCONSTRAINT_HH
#define COMISO_BOUNDCONSTRAINT_HH


//== INCLUDES =================================================================

#include <CoMISo/Config/CoMISoDefines.hh>
#include "NConstraintInterface.hh"


//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace COMISO { 

//== CLASS DEFINITION =========================================================

	      

/** \class BoundConstraint

    Brief Description.
  
    A more elaborate description follows.
*/
class COMISODLLEXPORT BoundConstraint : public NConstraintInterface
{
public:
  
//  inherited from NConstraintInterface
//  typedef Eigen::SparseVector<double> SVectorNC;
//  typedef SuperSparseMatrixT<double>  SMatrixNC;
//  // different types of constraints
//  enum ConstraintType {NC_EQUAL, NC_LESS_EQUAL, NC_GREATER_EQUAL};

  /// Default constructor
  BoundConstraint(const unsigned int  _var_idx = 0,   // index of variable for bound constraint
                  const double        _bound   = 0.0, // bound: x(_var_idx) #_type, <,=,># _bound
                  const unsigned int   _n      = 0,   // number of unknowns in problem
                  const ConstraintType _type   = NC_LESS_EQUAL); // type of bound upper, lower or both (equal)

 
  /// Destructor
  virtual ~BoundConstraint();

  virtual int    n_unknowns      (                                      );
  virtual double eval_constraint ( const double* _x                     );
  virtual void   eval_gradient   ( const double* _x, SVectorNC& _g      );
  virtual void   eval_hessian    ( const double* _x, SMatrixNC& _h      );

  virtual bool   is_linear()         const;
  virtual bool   constant_gradient() const;
  virtual bool   constant_hessian () const;

  // set/get values
  unsigned int& idx();
  double&       bound();
  unsigned int& n();
  void resize(const unsigned int _n);

private:
  // variable idx
  unsigned int idx_;
  // variable bound
  double bound_;
  // number of unknowns
  unsigned int n_;
};


//=============================================================================
} // namespace COMISO
//=============================================================================
#endif // ACG_BOUNDCONSTRAINT_HH defined
//=============================================================================

