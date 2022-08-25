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

/// function to extract the actual system Ax=b of linear equation from a B^tB matrix
template<class RMatrixT, class CMatrixT>
void extract_Axb( const RMatrixT& _B, CMatrixT& _A, std::vector< double >& _b)
{
  int dimm = gmm::mat_nrows(_B);
  int dimn = gmm::mat_ncols(_B);
  gmm::col_matrix< gmm::wsvector< double > > Btcol;
  gmm::col_matrix< gmm::wsvector< double > > Bcol;
  gmm::resize( Btcol, dimn, dimm);
  gmm::resize( Bcol, dimm, dimn);
  gmm::resize( _A, dimn, dimn);
  gmm::copy( _B, Bcol);
  gmm::copy( gmm::transposed( Bcol), Btcol);
  gmm::mult( Btcol, Bcol, _A);
  _b.resize( dimn);
  gmm::copy( _A.col(dimn-1), _b);
  _b.resize( dimn-1);
  gmm::resize( _A, dimn-1, dimn-1);
  gmm::scale(_b, -1.0);
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

  std::cout << "---------- 2) extract the Ax=b equation system, A (n-1 x n-1)" << std::endl;
  gmm::col_matrix< gmm::wsvector< double > > A;
  std::vector< double > b;
  extract_Axb( B, A, b);
  std::cout << "           A " << std::endl;
  std::cout << A << " " << b << std::endl;
  
  //gmm::inspect_matrix( B );
  std::cout << std::endl;

  std::cout << "---------- 3) define a set of linear constraints as an (c x n) row matrix C" << std::endl;
  int c = 2;
  gmm::row_matrix< gmm::wsvector< double > > C;
  simple_constraint_row_matrix( C, c, n);
  std::cout << C << std::endl;
  std::cout << "corresponding to the following linear equations : " << std::endl;
  print_equations( C );
  std::cout << std::endl;

  std::cout << "---------- 4) we now explicitly carry out the steps performed internally by the constrained solver and compare the two results at the end..." << std::endl;
  // copy the matrices
  gmm::col_matrix< gmm::wsvector< double > > Acpy( A );

  // create a constrained solver
  COMISO::ConstrainedSolver cs;
  // vector of indices to round (this is the mixed-integer part)
  std::vector< int > ids_to_round;
  // lets say we want to round the third variable
  ids_to_round.push_back(2);

  // vector of independent variables to be eliminated (computed by the make_constraints_independent function)
  std::vector< int > ids_to_elim;

  std::cout << "---------- ---------- 4.1) make the constraints independent (gauss elimination on C)" << std::endl;
  print_equations( C );
  cs.make_constraints_independent( C, ids_to_round, ids_to_elim);
  std::cout << "                      constraints after gauss elimination..." << std::endl;
  std::cout << C << std::endl;
  std::cout << "                      the variables to be eliminated are: " << std::endl;
  std::cout << ids_to_elim << std::endl << std::endl;
  gmm::row_matrix< gmm::wsvector< double > > Ccpy( C );
  

  std::cout << "---------- ---------- 4.2) eliminate constraints from system matrix A" << std::endl;

  // CSC matrix later initialized and used by solver
  gmm::csc_matrix< double > Acsc;

  // this re-indexing is also used by the solver, to know which variables are still there (!=-1) and which have been eliminated (=-1) it is setup by eliminate_constraints
  std::vector< int > new_idx;
 
  std::vector< double > x(b.size()); 
  std::vector< double > b_cpy(b); 

  cs.eliminate_constraints( Ccpy, Acpy, x, b, ids_to_round, ids_to_elim, new_idx, Acsc);
  std::cout << "                      A matrix after elimination of constraints..." << std::endl;
  std::cout << Acsc << std::endl;


  std::cout << "---------- ---------- 4.3) solve the system using the mixed-integer solver..." << std::endl;
  // create solver
  COMISO::MISolver miso;
  // miso solve
  miso.solve( Acsc, x, b, ids_to_round);
  std::cout << "                      solution vector x is\n" << x << std::endl << std::endl;


  std::cout << "---------- ---------- 4.4) now the solution must be re-indexed to the expected/original form/size...." << std::endl;
  cs.restore_eliminated_vars( Ccpy, x, ids_to_elim, new_idx);
  std::cout << "                      fullsize solution vector x is\n" << x << std::endl << std::endl;

  
  std::cout << "---------- ---------- 5) the same result is obtained by one call to the constrained solver, which takes care of re-indexing etc. internally..." << std::endl;
  // ids_to_round is altered by previous steps...
  ids_to_round.clear();
  ids_to_round.push_back(2);
  x.resize(gmm::mat_nrows(A));
  b.resize(gmm::mat_nrows(A));
  cs.solve( C, A, x, b_cpy, ids_to_round, 0.0, false, true);
  std::cout << "                      fullsize solution vector x is\n" << x << std::endl << std::endl;

  return -1;
}

