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
#include <CoMISo/NSolver/LeastSquaresProblem.hh>
#include <CoMISo/NSolver/LinearConstraint.hh>
#include <CoMISo/NSolver/NPDerivativeChecker.hh>
#include <CoMISo/NSolver/CPLEXSolver.hh>
#include <CoMISo/NSolver/IPOPTSolver.hh>

// solve least squares problem for x=1, y=2 and x-2y+z = 1
// with hard constraints x =-3, z>=3, z^2 >= x^2+y^2


// Example main
int main(void)
{
  std::cout << "---------- 1) Problem description..." << std::endl;
  std::cout << "Least squares terms: x=1, y=2 and x-2y+z = 1" << std::endl;
  std::cout << "Constraints        : x =-3, z>=3, z^2 >= x^2+y^2" << std::endl;

  std::cout << "---------- 1) Get an instance of a LeastSquaresProblem..." << std::endl;
  // number of unknowns
  const int n = 3;
  COMISO::LeastSquaresProblem lsqp(n);

  // term0
  COMISO::LinearConstraint::SVectorNC coeffs0(n);
  coeffs0.coeffRef(0) = 1.0;
  COMISO::LinearConstraint term0(coeffs0,-1.0,COMISO::NConstraintInterface::NC_EQUAL);
  lsqp.add_term(&term0);

    // term1
  COMISO::LinearConstraint::SVectorNC coeffs1(n);
  coeffs1.coeffRef(1) = 1.0;
  COMISO::LinearConstraint term1(coeffs1,-2.0,COMISO::NConstraintInterface::NC_EQUAL);
  lsqp.add_term(&term1);
  
    // term2
  COMISO::LinearConstraint::SVectorNC coeffs2(n);
  coeffs2.coeffRef(0) =  1.0;
  coeffs2.coeffRef(1) = -2.0;
  coeffs2.coeffRef(2) =  1.0;
  COMISO::LinearConstraint term2(coeffs2,-1.0,COMISO::NConstraintInterface::NC_EQUAL);
  lsqp.add_term(&term2);
  
  std::cout << "---------- 2) set up constraints" << std::endl;

  // set x = -3.0
  COMISO::LinearConstraint lc;
  lc.coeffs().coeffRef(0) = 1.0;
  lc.b()                  = 3.0;

  // set z>=3 (cone constraint requires that z>=0 !!!)
  COMISO::BoundConstraint bc(2,3,3,COMISO::NConstraintInterface::NC_GREATER_EQUAL);

  // set z^2 >= x^2+y^2
  COMISO::ConeConstraint cc;
  cc.resize(3);
  cc.i() = 2;
  cc.c() = 4.0;
  cc.Q()(0,0) = 2.0;
  cc.Q()(1,1) = 4.0;
  cc.Q()(0,1) = 1.0;
  cc.Q()(1,0) = 1.0;

  // fill constraint vector
  std::vector<COMISO::NConstraintInterface*> constraints;
  constraints.push_back(&lc);
  constraints.push_back(&bc);
  constraints.push_back(&cc);

// check if CPLEX solver available in current configuration
#if( COMISO_CPLEX_AVAILABLE)
  std::cout << "---------- 3) Solve with CPLEX solver... " << std::endl;

  COMISO::CPLEXSolver cplx;
  cplx.solve(&lsqp, constraints);
#endif

  std::cout << "---------- 4) Print solution CPLEX..." << std::endl;
  for( int i=0; i<n; ++i)
    std::cerr << "x_" << i << " = " << lsqp.x()[i] << std::endl;

  // check if IPOPT solver available in current configuration
#if( COMISO_IPOPT_AVAILABLE)
  std::cout << "---------- 5) Solve with IPOPT solver... " << std::endl;

  COMISO::IPOPTSolver ipopt;
  ipopt.app().Options()->SetStringValue("derivative_test", "second-order");
  ipopt.solve(&lsqp, constraints);
#endif

  std::cout << "---------- 6) Print solution..." << std::endl;
  for( int i=0; i<n; ++i)
    std::cerr << "x_" << i << " = " << lsqp.x()[i] << std::endl;
  
  return 0;
}

