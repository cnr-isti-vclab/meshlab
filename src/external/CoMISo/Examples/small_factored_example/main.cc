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


#include <CoMISo/Utils/StopWatch.hh>
#include <gmm/gmm.h>
#include <vector>
#include <CoMISo/Solver/ConstrainedSolver.hh>
#include <CoMISo/Solver/MISolver.hh>
#include <CoMISo/Solver/GMM_Tools.hh>


/// function to initialize a simple row matrix of equations
template<class MatrixT>
void init_fac( MatrixT& _B )
{
  _B(0,0) = 0   ; _B(0,1) = 4 ; _B(0,2) = -2 ; _B(0,3) = 0 ; _B(0,4) = -1;
  _B(1,0) = 0   ; _B(1,1) = 0 ; _B(1,2) = 0  ; _B(1,3) = 0 ; _B(1,4) = 0 ;
  _B(2,0) = 5   ; _B(2,1) = 0 ; _B(2,2) = -3 ; _B(2,3) = 0 ; _B(2,4) = 0 ; 
  _B(3,0) = 0   ; _B(3,1) = 0 ; _B(3,2) = -2 ; _B(3,3) = 0 ; _B(3,4) = 0 ; 
  _B(4,0) = 0   ; _B(4,1) = -2; _B(4,2) = 0  ; _B(4,3) = 2 ; _B(4,4) = 0 ; 
}

/// function to print the equations corresponding to the matrices of an equation system
template<class MatrixT>
void print_equations( const MatrixT& _B)
{
  int m = gmm::mat_nrows( _B);
  int n = gmm::mat_ncols( _B);
  for( int i = 0; i < m; ++i)
  {
    for( int j = 0; j < n-1; ++j)
    {
      if( _B(i,j) != 0.0)
        std::cout << _B(i,j) << "*x" << j;
      else
        std::cout << "   0 ";
      if( j < n-2 ) std::cout << " + ";
    }
    std::cout << " = " << _B(i, n-1) << std::endl;
  }
}


// Example main
int main(void)
{
  std::cout << "---------- 1) Setup a number of equations (i.e. the B matrix of a factored system of linear equations B^tB)..." << std::endl;
  int n = 4;
  gmm::row_matrix< gmm::wsvector< double > > B(n+1,n+1);
  std::vector< double > x(n);

  init_fac( B );

  // create an empty constraint matrix (will be used later)
  gmm::row_matrix< gmm::wsvector< double > > constraints(0,n+1); //n+1 because of right hand side
  // create an empty vector of variable indices to be rounded (will be used later)
  std::vector< int > ids_to_round;

  std::cout << B << std::endl << std::endl;


  std::cout << "---------- 2) The original solution to this system is..." << std::endl;

  COMISO::ConstrainedSolver cs;
  //  void solve( RMatrixT& _constraints, RMatrixT& _B, VectorT&  _x, VectorIT& _idx_to_round, double    _reg_factor = 0.0, bool      _show_miso_settings = true, bool      _show_timings = true );
  //_show_miso_settings requires a QT context and hence must be false in this example
  cs.solve( constraints, B, x, ids_to_round, 0.0, false, true);  
  // copy this solution for later
  std::vector< double > org_x( x); 
  std::cout << x << std::endl;


  std::cout << "---------- 3) Rounding: forcing the second variable to lie on an integer, changes the solution to..." << std::endl;
  // reset system 
  init_fac( B );
  ids_to_round.push_back(1);
  cs.solve( constraints, B, x, ids_to_round, 0.0, false, true);  
  std::cout << x << std::endl;


  std::cout << "---------- 4) Constraining: forcing the first variable to equal the second changes the solution to..." << std::endl;
  // reset system 
  init_fac( B );
  ids_to_round.clear();
  ids_to_round.push_back(1);
  // setup constraint x0*1+x1*0+x2*(-1)+x3*0=0
  gmm::resize( constraints, 1, n+1);
  constraints( 0, 0 ) = 1.0;
  constraints( 0, 1 ) = -1.0;
  std::cout << "           the constraint equation looks like this:" << std::endl;
  print_equations( constraints);
  cs.solve( constraints, B, x, ids_to_round, 0.0, false, true);  
  std::cout << x << std::endl;

  return -1;
}

