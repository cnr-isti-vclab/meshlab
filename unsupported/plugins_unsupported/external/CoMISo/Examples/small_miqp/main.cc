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

#include <CoMISo/Config/config.hh>

#include <CoMISo/Utils/StopWatch.hh>
#include <vector>
#include <CoMISo/NSolver/NProblemInterface.hh>
#include <CoMISo/NSolver/NPDerivativeChecker.hh>
#include <CoMISo/NSolver/GUROBISolver.hh>
#include <CoMISo/NSolver/CPLEXSolver.hh>
#include <CoMISo/NSolver/LinearConstraint.hh>
#include <CoMISo/NSolver/VariableType.hh>


// generate an instance of a nonlinear problem by deriving from base class NProblemInterface
// implement all virtual functions in order to solve this problem by any of the solvers located
// in CoMISo/NSolver

class SmallNProblem : public COMISO::NProblemInterface
{
public:

  // Sparse Matrix Type
  //  typedef Eigen::DynamicSparseMatrix<double,Eigen::ColMajor> SMatrixNP;


  // specify a function which has several local minima
  // f(x,y)=(x-2y+1)^2 + (x-5)^2

  // number of unknown variables, here x and y = 2
  virtual int    n_unknowns   (                                )
  {
    return 2;
  }

  // initial value where the optimization should start from
  virtual void   initial_x    (       double* _x               )
  {
    _x[0] = 0.0;
    _x[1] = 0.0;
  }

  // function evaluation at location _x
  virtual double eval_f       ( const double* _x               )
  {
    double term  = _x[0] - 2.0*_x[1] + 1.0;
    double term2 = _x[0] - 5.0;

    return term*term + term2*term2;
  }

  // gradient evaluation at location _x
  virtual void   eval_gradient( const double* _x, double*    _g)
  {
    double term  = _x[0] - 2.0*_x[1] + 1.0;
    double term2 = _x[0] - 5.0;

    _g[0] =  2.0*term + 2.0*term2;
    _g[1] = -4.0*term;
   }

  // hessian matrix evaluation at location _x
  virtual void   eval_hessian ( const double* _x, SMatrixNP& _H)
  {
    _H.resize(n_unknowns(), n_unknowns());
    _H.setZero();

    _H.coeffRef(0,0) =  4.0;
    _H.coeffRef(1,0) = -4.0;
    _H.coeffRef(0,1) = -4.0;
    _H.coeffRef(1,1) =  8.0;
  }

  // print result
  virtual void   store_result ( const double* _x               )
  {
    std::cerr << "Energy: " << eval_f(_x) << std::endl;
    std::cerr << "(x,y) = (" << _x[0] << "," << _x[1] << ")" << std::endl;
  }

  // advanced properties
  virtual bool   constant_hessian() { return true; }
};


//------------------------------------------------------------------------------------------------------

// Example main
int main(void)
{
  std::cout << "---------- 1) Get an instance of a NProblem..." << std::endl;
  SmallNProblem snp;

  std::cout << "---------- 2) (optional for debugging) Check derivatives of problem..." << std::endl;
  COMISO::NPDerivativeChecker npd;
  npd.check_all(&snp);

  std::cout << "---------- 3) setup list of integer variables..." << std::endl;
  std::vector<COMISO::PairIndexVtype> discrete_variables;
  discrete_variables.push_back( COMISO::PairIndexVtype(0,COMISO::Integer) );

  std::cout << "---------- 4) setup constraints..." << std::endl;
  std::vector<COMISO::NConstraintInterface*> constraints;
  // setup constraint x+y <= 6.5
  COMISO::LinearConstraint::SVectorNC coeffs(2);
  coeffs.coeffRef(0) = 1.0;
  coeffs.coeffRef(1) = 1.0;
  COMISO::LinearConstraint lc(coeffs, -6.5, COMISO::LinearConstraint::NC_LESS_EQUAL);
  constraints.push_back(&lc);


// check if IPOPT solver available in current configuration
#if( COMISO_GUROBI_AVAILABLE)
  std::cout << "---------- 5) Get GUROBI solver... " << std::endl;
  COMISO::GUROBISolver gsol;

  std::cout << "---------- 4) Solve..." << std::endl;

  gsol.solve(&snp, constraints, discrete_variables);
#endif

  // check if TAO solver available in current configuration
#if( COMISO_CPLEX_AVAILABLE)
  std::cout << "---------- 5) Solve with CPLEX solver... " << std::endl;
  COMISO::CPLEXSolver csol;

  std::cout << "---------- 4) Solve..." << std::endl;

  csol.solve(&snp, constraints, discrete_variables);
#endif

  return 0;
}

