//=============================================================================
//
//  CLASS CPLEXSolver - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

//== COMPILE-TIME PACKAGE REQUIREMENTS ========================================
#include "CPLEXSolver.hh"
#if COMISO_CPLEX_AVAILABLE
//=============================================================================


#include <stdexcept>

//== NAMESPACES ===============================================================

namespace COMISO {

//== IMPLEMENTATION ========================================================== 



CPLEXSolver::
CPLEXSolver()
{
}


//-----------------------------------------------------------------------------


//void
//CPLEXSolver::
//set_problem_output_path( const std::string &_problem_output_path)
//{
//  problem_output_path_ = _problem_output_path;
//}
//
//
////-----------------------------------------------------------------------------
//
//
//void
//CPLEXSolver::
//set_problem_env_output_path( const std::string &_problem_env_output_path)
//{
//  problem_env_output_path_ = _problem_env_output_path;
//}
//
//
////-----------------------------------------------------------------------------
//
//
//void
//CPLEXSolver::
//set_solution_input_path(const std::string &_solution_input_path)
//{
//  solution_input_path_ = _solution_input_path;
//}


//=============================================================================
} // namespace COMISO
//=============================================================================
#endif // COMISO_CPLEX_AVAILABLE
//=============================================================================
