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

/// function to setup a random sparse row matrix of dimension _m x _n
/// for the simplicity of this example only integer valued entries are used
template<class MatrixT>
void random_sparse_row_matrix( MatrixT& _B, int _m, int _n, double _density = 0.7)
{
  gmm::resize(_B,  _m, _n);

  for( int i=0; i<_m; ++i)
    for( int j=0; j<_n; ++j)
      if( (rand()-1.0*_density*RAND_MAX)/RAND_MAX> 0) // for sparseness
        _B(i,j) = round(((rand()-0.4*RAND_MAX)/RAND_MAX)*10.0);
}

/// function to setup a random sparse constraint row matrix of dimension _c x _n
/// for the simplicity of the example only -1, 0, 1 constraints are used
template<class MatrixT>
void simple_constraint_row_matrix( MatrixT& _C, int _c, int _n, double _distribution = 0.2)
{
  gmm::resize( _C, _c, _n);
  for( int i=0; i<_c; ++i)
    for( int j=0; j<_n; ++j)
    {
      double randnum = (double(rand())/double(RAND_MAX));
      if ( randnum < _distribution)
        _C( i,j) = -1;
      else if( randnum > (1.0-_distribution))
        _C( i,j) = 1;
      else
        _C( i,j) = 0;
    }
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
  std::cout << "---------- 1) setup an (m x n) sparse row matrix B (i.e. the B in the system ((Bx)^T)Bx)" << std::endl;
  int m = 9;
  int n = 5+1;
  gmm::row_matrix< gmm::wsvector< double > > B;
  random_sparse_row_matrix( B, m, n, 0.85);
  std::cout << B << std::endl << std::endl;
  //gmm::inspect_matrix( B );
  std::cout << std::endl;

  std::cout << "---------- 2) define a set of linear constraints as an (c x n) row matrix C" << std::endl;
  int c = 2;
  gmm::row_matrix< gmm::wsvector< double > > C;
  simple_constraint_row_matrix( C, c, n);
  std::cout << C << std::endl;
  std::cout << "corresponding to the following linear equations : " << std::endl;
  print_equations( C );
  std::cout << std::endl;

  std::cout << "---------- 3) we now explicitly carry out the steps performed internally by the constrained solver and compare the two results at the end..." << std::endl;
  // copy the matrices
  gmm::row_matrix< gmm::wsvector< double > > Bcpy( B );
  gmm::row_matrix< gmm::wsvector< double > > Ccpy( C );

  // create a constrained solver
  COMISO::ConstrainedSolver cs;
  // vector of indices to round (this is the mixed-integer part)
  std::vector< int > ids_to_round;
  // lets say we want to round the third variable
  ids_to_round.push_back(2);

  // vector of independent variables to be eliminated (computed by the make_constraints_independent function)
  std::vector< int > ids_to_elim;

  std::cout << "---------- ---------- 3.1) make the constraints independent (gauss elimination on C)" << std::endl;
  print_equations( Ccpy );
  cs.make_constraints_independent( Ccpy, ids_to_round, ids_to_elim);
  std::cout << "                      constraints after gauss elimination..." << std::endl;
  std::cout << Ccpy << std::endl;
  std::cout << "                      the variables to be eliminated are: " << std::endl;
  std::cout << ids_to_elim << std::endl << std::endl;

  std::cout << "---------- ---------- 3.2) eliminate constraints from system matrix B" << std::endl;
  // this is the column matrix later used by the solver, it is setup by the eliminate_constraints function
  gmm::col_matrix< gmm::wsvector< double > > Bcol;

  // this re-indexing is also used by the solver, to know which variables are still there (!=-1) and which have been eliminated (=-1) it is setup by eliminate_constraints
  std::vector< int > new_idx;
  
  cs.eliminate_constraints( Ccpy, Bcpy, ids_to_round, ids_to_elim, new_idx, Bcol);
  std::cout << "                      B matrix after elimination of constraints..." << std::endl;
  std::cout << Bcol << std::endl;

  std::cout << "---------- ---------- 3.3) setup the linear system Ax=b, where by forming B^TB and extracting the right hand side" << std::endl;
  
  // this is the solution vector x
  std::vector< double > x;

  int new_n = gmm::mat_ncols( Bcol);
  // set up B transposed
  gmm::col_matrix< gmm::wsvector< double > > Bt( new_n, m);
  gmm::copy( gmm::transposed( Bcol), Bt);

  // setup BtB
  gmm::col_matrix< gmm::wsvector< double > > BtB( new_n, new_n);
  gmm::mult( Bt, Bcol, BtB);

  // extract rhs
  std::vector< double > rhs( new_n);
  gmm::copy( gmm::scaled(gmm::mat_const_col( BtB, new_n - 1),-1.0), rhs);
  rhs.resize( new_n - 1);

  // resize BtB to only contain the actual system matrix (and not the rhs)
  gmm::resize( BtB, new_n - 1, new_n - 1);
  x.resize( new_n - 1);

  // BtB -> CSC
  gmm::csc_matrix<double> BtBCSC;
  BtBCSC.init_with_good_format( BtB);

  std::cout << "                      the linear system now looks like..." << std::endl;
  std::cout << "                      Matrix A\n " << BtBCSC << std::endl;
  std::cout << "                      Right hand side b\n" << rhs << std::endl << std::endl;

  std::cout << "---------- ---------- 3.4) solve the system using the mixed-integer solver..." << std::endl;
  // create solver
  COMISO::MISolver miso;
  // miso solve
  miso.solve( BtBCSC, x, rhs, ids_to_round);
  std::cout << "                      solution vector x is\n" << x << std::endl << std::endl;

  std::cout << "---------- ---------- 3.5) now the solution must be re-indexed to the expected/original form/size...." << std::endl;
  cs.restore_eliminated_vars( Ccpy, x, ids_to_elim, new_idx);
  std::cout << "                      fullsize solution vector x is\n" << x << std::endl << std::endl;

  std::cout << "---------- ---------- 4) the same result is obtained by one call to the constrained solver, which takes care of re-indexing etc. internally..." << std::endl;
  // ids_to_round is altered by previous steps...
  ids_to_round.clear();
  ids_to_round.push_back(2);
  cs.solve( C, B, x, ids_to_round, 0.0, false, true);
  std::cout << "                      fullsize solution vector x is\n" << x << std::endl << std::endl;

  return -1;
}

