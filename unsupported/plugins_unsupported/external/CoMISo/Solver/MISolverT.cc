/*===========================================================================*\
 *                                                                           *
 *                               CoMISo                                      *
 *      Copyright (C) 2008-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.rwth-graphics.de                            *
 *                                                                           *
 *---------------------------------------------------------------------------* 
 *  This file is part of CoMISo.                                             *
 *                                                                           *
 *  CoMISo is free software: you can redistribute it and/or modify           *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation, either version 3 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  CoMISo is distributed in the hope that it will be useful,                *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with CoMISo.  If not, see <http://www.gnu.org/licenses/>.          *
 *                                                                           *
\*===========================================================================*/ 



//=============================================================================
//
//  CLASS MISolver - IMPLEMENTATION
//
//=============================================================================

#define COMISO_MISOLVER_C
//== INCLUDES =================================================================

#include "MISolver.hh"

#if COMISO_CPLEX_AVAILABLE
  #include <ilcplex/ilocplex.h>
  ILOSTLBEGIN
#endif


//== NAMESPACES ===============================================================

namespace COMISO {

//== IMPLEMENTATION ==========================================================



// inline function...
void
MISolver::solve(
    CSCMatrix& _A,
    Vecd&      _x,
    Vecd&      _rhs,
    Veci&      _to_round,
    bool       _fixed_order )
{
  std::cerr << "# integer    variables: " << _to_round.size() << std::endl;
  std::cerr << "# continuous variables: " << _x.size()-_to_round.size() << std::endl;
  
  // nothing to solve?
  if( gmm::mat_ncols(_A) == 0 || gmm::mat_nrows(_A) == 0)
    return;

  if( gurobi_rounding_)
    solve_gurobi(_A, _x, _rhs, _to_round);
  else
    if( cplex_rounding_)
      solve_cplex(_A, _x, _rhs, _to_round);
    else
      if( no_rounding_ || _to_round.size() == 0)
        solve_no_rounding( _A, _x, _rhs);
      else
        if( direct_rounding_)
          solve_direct_rounding( _A, _x, _rhs, _to_round);
        else
          if( multiple_rounding_)
            solve_multiple_rounding( _A, _x, _rhs, _to_round);
          else
            solve_iterative( _A, _x, _rhs, _to_round, _fixed_order);
}


//-----------------------------------------------------------------------------

// inline function...
void
MISolver::solve_cplex(
    CSCMatrix& _A,
    Vecd&      _x,
    Vecd&      _rhs,
    Veci&      _to_round)
{
#if COMISO_CPLEX_AVAILABLE

  // get round-indices in set
  std::set<int> to_round;
  for(unsigned int i=0; i<_to_round.size();++i)
    to_round.insert(_to_round[i]);

  try {

    IloEnv env_;
    IloModel model(env_);

    // set time limite
    //    model.getEnv().set(GRB_DoubleParam_TimeLimit, gurobi_max_time_);

    unsigned int n = _rhs.size();

    // 1. allocate variables
    std::vector<IloNumVar> vars;
    for( unsigned int i=0; i<n; ++i)
      if( to_round.count(i))
        vars.push_back( IloNumVar(env_,  -IloIntMax,   IloIntMax, IloNumVar::Int)   );
      else
        vars.push_back( IloNumVar(env_,-IloInfinity, IloInfinity, IloNumVar::Float) );


    // 2. setup_energy

    // build objective function from linear system E = x^tAx - 2x^t*rhs
    IloExpr objective(env_);

    for(unsigned int i=0; i<_A.nc; ++i)
      for(unsigned int j=_A.jc[i]; j<_A.jc[i+1]; ++j)
      {
        objective += _A.pr[j]*vars[_A.ir[j]]*vars[i];
      }
    for(unsigned int i=0; i<n; ++i)
      objective -= 2*_rhs[i]*vars[i];

    // ToDo: objective correction!!!

    //    _A.jc[c+1]
    //    _A.pr[write]
    //    _A.ir[write]
    //    _A.nc
    //    _A.nr

    // minimize
    model.add(IloMinimize(env_,objective));

    // 4. solve
    IloCplex cplex(model);
    cplex.setParam(IloCplex::TiLim, gurobi_max_time_);

//    // set parameters comparable to CoMISo
//    {
//      cplex.setParam(IloCplex::MIPSearch  , 1);  // Traditional Branch-and-Cut
//      cplex.setParam(IloCplex::NodeSel    , 0);  // Depth-First
//      cplex.setParam(IloCplex::VarSel     , -1);  // closest to integer
//      cplex.setParam(IloCplex::MIPEmphasis, 1);  // concentrate on feasibility
//    }

    cplex.solve();

    // 5. store result
    _x.resize(n);
    for(unsigned int i=0; i<n; ++i)
      _x[i] = cplex.getValue(vars[i]);

    std::cout << "CPLEX objective: " << cplex.getObjValue() << std::endl;

  }
  catch (IloException& e)
  {
    cerr << "CPLEX Concert exception caught: " << e << endl;
  }
  catch (...)
  {
    cerr << "CPLEX Unknown exception caught" << endl;
  }


#else
  std::cerr << "CPLEX solver is not available, please install it..." << std::endl;
#endif
}

//-----------------------------------------------------------------------------


//=============================================================================
} // namespace COMISO
//=============================================================================
