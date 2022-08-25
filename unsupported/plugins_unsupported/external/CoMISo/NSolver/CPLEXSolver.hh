//=============================================================================
//
//  CLASS CPLEXSolver
//
//=============================================================================


#ifndef COMISO_CPLEXSOLVER_HH
#define COMISO_CPLEXSOLVER_HH


//== COMPILE-TIME PACKAGE REQUIREMENTS ========================================
#include <CoMISo/Config/config.hh>
#if COMISO_CPLEX_AVAILABLE

//== INCLUDES =================================================================

#include <CoMISo/Config/CoMISoDefines.hh>
#include <vector>
#include <string>
#include "NProblemInterface.hh"
#include "NConstraintInterface.hh"
#include "VariableType.hh"

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace COMISO {

//== CLASS DEFINITION =========================================================

	      

/** \class NewtonSolver CPLEXSolver.hh

    Brief Description.
  
    A more elaborate description follows.
*/
class COMISODLLEXPORT CPLEXSolver
{
public:

  /// Default constructor
  CPLEXSolver();
 
  /// Destructor
  ~CPLEXSolver() { /*env_.end();*/}

  // ********** SOLVE **************** //
  // this function has to be inline due to static linking issues
  inline bool solve(NProblemInterface*                  _problem,                // problem instance
                    std::vector<NConstraintInterface*>& _constraints,            // linear constraints
                    std::vector<PairIndexVtype>&        _discrete_constraints,   // discrete constraints
                    const double                        _time_limit = 60,
                    const bool                          _silent = false);  // time limit in seconds

  // same as above but without discrete constraints (for convenience)
  inline bool solve(NProblemInterface*                  _problem,                // problem instance
                    std::vector<NConstraintInterface*>& _constraints,            // linear constraints
                    const double                        _time_limit = 60,
                    const bool                          _silent = false)  // time limit in seconds
  { std::vector<PairIndexVtype> dc; return solve(_problem, _constraints, dc, _time_limit, _silent);}

  // with handling of cone constrints
  inline bool solve2(NProblemInterface*                  _problem,                // problem instance
                    std::vector<NConstraintInterface*>& _constraints,            // linear constraints
                    std::vector<PairIndexVtype>&        _discrete_constraints,   // discrete constraints
                    const double                        _time_limit = 60,
                    const bool                          _silent = false);  // time limit in seconds


//  void set_problem_output_path    ( const std::string &_problem_output_path);
//  void set_problem_env_output_path( const std::string &_problem_env_output_path);
//  void set_solution_input_path    ( const std::string &_solution_input_path);

protected:
  double* P(std::vector<double>& _v)
  {
    if( !_v.empty())
      return ((double*)&_v[0]);
    else
      return 0;
  }

private:

  // CPLEX environment
//  IloEnv env_;

  // filenames for exporting/importing gurobi solutions
  // if string is empty nothing is imported or exported
//  std::string problem_output_path_;
//  std::string problem_env_output_path_;
//  std::string solution_input_path_;
};



//=============================================================================
} // namespace COMISO

//=============================================================================
#endif // COMISO_CPLEX_AVAILABLE
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(COMISO_CPLEXSOLVER_C)
#define COMISO_CPLEXSOLVER_TEMPLATES
#include "CPLEXSolverT.cc"
#endif
//=============================================================================
#endif // ACG_CPLEXSOLVER_HH defined
//=============================================================================

