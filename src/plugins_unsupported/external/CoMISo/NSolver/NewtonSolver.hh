//=============================================================================
//
//  CLASS NewtonSolver
//
//=============================================================================


#ifndef COMISO_NEWTONSOLVER_HH
#define COMISO_NEWTONSOLVER_HH

//== COMPILE-TIME PACKAGE REQUIREMENTS ========================================
#include <CoMISo/Config/config.hh>

//== INCLUDES =================================================================

#include <CoMISo/Config/CoMISoDefines.hh>
#include <gmm/gmm.h>
#include "NProblemGmmInterface.hh"

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace COMISO {

//== CLASS DEFINITION =========================================================

	      

/** \class NewtonSolver NewtonSolver.hh <ACG/.../NewtonSolver.hh>

    Brief Description.
  
    A more elaborate description follows.
*/
class COMISODLLEXPORT NewtonSolver
{
public:
   
  /// Default constructor
  NewtonSolver() : max_iter_(20), convergence_eps_(1e-6), constant_hessian_structure_(false) {}
 
  /// Destructor
  ~NewtonSolver() {}

  // solve
  int solve(NProblemGmmInterface* _problem);

  // solve specifying parameters
  int solve(NProblemGmmInterface* _problem, int _max_iter, double _eps)
  {
    max_iter_ = _max_iter;
    convergence_eps_ = _eps;
    return solve(_problem);
  }

  bool& constant_hessian_structure() { return constant_hessian_structure_; }

protected:
  double* P(std::vector<double>& _v)
  {
    if( !_v.empty())
      return ((double*)&_v[0]);
    else
      return 0;
  }

private:
  int    max_iter_;
  double convergence_eps_;
  bool   constant_hessian_structure_;
};


//=============================================================================
} // namespace COMISO
//=============================================================================
#endif // ACG_NEWTONSOLVER_HH defined
//=============================================================================

