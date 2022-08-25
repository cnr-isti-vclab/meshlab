//=============================================================================
//
//  CLASS GUROBISolver
//
//=============================================================================


#ifndef COMISO_GUROBISOLVER_HH
#define COMISO_GUROBISOLVER_HH


//== COMPILE-TIME PACKAGE REQUIREMENTS ========================================
#include <CoMISo/Config/config.hh>
#if COMISO_GUROBI_AVAILABLE

//== INCLUDES =================================================================

#include <CoMISo/Config/CoMISoDefines.hh>
#include <vector>
#include <string>
#include "NProblemInterface.hh"
#include "NConstraintInterface.hh"
#include "VariableType.hh"
#include "GurobiHelper.hh"

#include <gurobi_c++.h>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace COMISO {

//== CLASS DEFINITION =========================================================

	      

/** \class NewtonSolver GUROBISolver.hh

    Brief Description.
  
    A more elaborate description follows.
*/
class COMISODLLEXPORT GUROBISolver
{
public:

  /// Default constructor
  GUROBISolver();
 
  /// Destructor
  ~GUROBISolver() {}

  // ********** SOLVE **************** //
  bool solve(NProblemInterface*                  _problem,                // problem instance
             std::vector<NConstraintInterface*>& _constraints,            // linear constraints
             std::vector<PairIndexVtype>&        _discrete_constraints,   // discrete constraints
             const double                        _time_limit = 60     ); // time limit in seconds

  void set_problem_output_path    ( const std::string &_problem_output_path);
  void set_problem_env_output_path( const std::string &_problem_env_output_path);
  void set_solution_input_path    ( const std::string &_solution_input_path);

protected:
  double* P(std::vector<double>& _v)
  {
    if( !_v.empty())
      return ((double*)&_v[0]);
    else
      return 0;
  }

private:

  // filenames for exporting/importing gurobi solutions
  // if string is empty nothing is imported or exported
  std::string problem_output_path_;
  std::string problem_env_output_path_;
  std::string solution_input_path_;
};



//=============================================================================
} // namespace COMISO

//=============================================================================
#endif // COMISO_GUROBI_AVAILABLE
//=============================================================================
#endif // ACG_GUROBISOLVER_HH defined
//=============================================================================

