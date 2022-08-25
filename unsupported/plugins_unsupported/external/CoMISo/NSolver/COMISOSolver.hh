//=============================================================================
//
//  CLASS COMISOSolver
//
//=============================================================================


#ifndef COMISO_COMISOSOLVER_HH
#define COMISO_COMISOSOLVER_HH


//== COMPILE-TIME PACKAGE REQUIREMENTS ========================================
#include <CoMISo/Config/config.hh>

//== INCLUDES =================================================================

#include <CoMISo/Config/CoMISoDefines.hh>
#include <CoMISo/Solver/ConstrainedSolver.hh>
#include <vector>
#include "NProblemInterface.hh"
#include "NConstraintInterface.hh"
#include "VariableType.hh"


//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace COMISO {

//== CLASS DEFINITION =========================================================

	      

/** \class NewtonSolver GUROBISolver.hh

    Brief Description.
  
    A more elaborate description follows.
*/
class COMISODLLEXPORT COMISOSolver
{
public:
   
  typedef std::pair<unsigned int, VariableType> PairUiV;

  /// Default constructor
  COMISOSolver() {}
 
  /// Destructor
  ~COMISOSolver() {}

  // ********** SOLVE **************** //
  void solve(NProblemInterface*                  _problem,                      // problem instance
             std::vector<NConstraintInterface*>& _constraints,                  // linear constraints
             std::vector<PairUiV>&               _discrete_constraints,         // discrete constraint
             double                              _reg_factor = 0.0,             // reguluarization factor
             bool                                _show_miso_settings = true,    // show settings dialog
             bool                                _show_timings = true        ); // show timings


  // get reference to ConstrainedSolver to manipulate parameters
  ConstrainedSolver& solver() { return cs_;}

protected:
  double* P(std::vector<double>& _v)
  {
    if( !_v.empty())
      return ((double*)&_v[0]);
    else
      return 0;
  }

private:
  ConstrainedSolver cs_;
};



//=============================================================================
} // namespace COMISO
//=============================================================================
#endif // ACG_GUROBISOLVER_HH defined
//=============================================================================

