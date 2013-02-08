//=============================================================================
//
//  CLASS COMISOSolver - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

//== COMPILE-TIME PACKAGE REQUIREMENTS ========================================
#include <CoMISo/Config/config.hh>
//=============================================================================

#include <vector>
#include "COMISOSolver.hh"

//== NAMESPACES ===============================================================

namespace COMISO {

//== IMPLEMENTATION ========================================================== 


// ********** SOLVE **************** //
void
COMISOSolver::
solve(NProblemInterface*                  _problem,
      std::vector<NConstraintInterface*>& _constraints,
      std::vector<PairUiV>&               _discrete_constraints,
      double                              _reg_factor,
      bool                                _show_miso_settings,
      bool                                _show_timings        )
{

  //----------------------------------------------
  // 1. identify integer variables
  //----------------------------------------------

  // identify integer variables
  std::vector<int> round_idxs;
  for(unsigned int i=0; i<_discrete_constraints.size(); ++i)
    switch(_discrete_constraints[i].second)
    {
      case Binary :
      case Integer:
        round_idxs.push_back(_discrete_constraints[i].first); break;
      default     : break;
    }


  //----------------------------------------------
  // 2. setup constraints
  //----------------------------------------------
  int n = _problem->n_unknowns();
  gmm::row_matrix< gmm::wsvector< double > > C(_constraints.size(), n+1);
  int n_constraints = 0;

  // get zero vector
  std::vector<double> x(n, 0.0);

  for(unsigned int i=0; i<_constraints.size();  ++i)
    if(_constraints[i]->constraint_type() == NConstraintInterface::NC_EQUAL)
    {
      if(!_constraints[i]->is_linear())
        std::cerr << "Warning: COMISOSolver received a problem with non-linear constraints!!!" << std::endl;

      // get linear part
      NConstraintInterface::SVectorNC gc;
      _constraints[i]->eval_gradient(P(x), gc);

      NConstraintInterface::SVectorNC::InnerIterator v_it(gc);
      for(; v_it; ++v_it)
        C(n_constraints, v_it.index()) = v_it.value();

      // get constant part
      C(n_constraints, n) = _constraints[i]->eval_constraint(P(x));

      // move to next constraint
      ++n_constraints;
    }

  // resize matrix to final number of constraints
  gmm::resize(C,n_constraints, n+1);


  //----------------------------------------------
  // 3. setup energy
  //----------------------------------------------

  if(!_problem->constant_hessian())
    std::cerr << "Warning: COMISOSolver received a problem with non-constant hessian!!!" << std::endl;


  // get hessian matrix
  gmm::col_matrix< gmm::wsvector< double > > A(n,n);
  NProblemInterface::SMatrixNP H;
  _problem->eval_hessian(P(x), H);
  for( int i=0; i<H.outerSize(); ++i)
    for (NProblemInterface::SMatrixNP::InnerIterator it(H,i); it; ++it)
      A(it.row(),it.col()) = it.value();


  // get negative gradient
  std::vector<double> rhs(_problem->n_unknowns());
  _problem->eval_gradient(P(x), P(rhs));
  for(unsigned int i=0; i<rhs.size(); ++i)
    rhs[i] = -rhs[i];

//  // add constant part
//  objective += _problem->eval_f(P(x));

  //----------------------------------------------
  // 4. solve problem
  //----------------------------------------------

  cs_.solve(C,A,x,rhs,round_idxs,
            _reg_factor, _show_miso_settings, _show_timings);

  //  void solve(
  //      RMatrixT& _constraints,
  //      CMatrixT& _A,
  //      VectorT&  _x,
  //      VectorT&  _rhs,
  //      VectorIT& _idx_to_round,
  //      double    _reg_factor = 0.0,
  //      bool      _show_miso_settings = true,
  //      bool      _show_timings = true );

  //----------------------------------------------
  // 5. store result
  //----------------------------------------------

  _problem->store_result(P(x));

//  std::cout << "COMISO Objective: " << model.get(GRB_DoubleAttr_ObjVal) << std::endl;
}


//=============================================================================
} // namespace COMISO
//=============================================================================
