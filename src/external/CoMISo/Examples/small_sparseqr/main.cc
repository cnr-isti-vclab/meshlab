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
#include <vector>
#include <cstdlib>
#include <iostream>

//------------------------------------------------------------------------------------------------------
#if COMISO_SUITESPARSE_SPQR_AVAILABLE // additional spqr library required
//------------------------------------------------------------------------------------------------------

#include <CoMISo/Utils/StopWatch.hh>
#include <Eigen/Sparse>
#include <CoMISo/Solver/SparseQRSolver.hh>
#include <CoMISo/Solver/Eigen_Tools.hh>


//------------------------------------------------------------------------------------------------------

// Example main
int main(void)
{
  std::cout << "---------- 0) Using Sparse QR for solving underdetermined equations and computing Null spaces " << std::endl;

  typedef Eigen::SparseMatrix< double > SpMatrix;
  typedef Eigen::MatrixXd DenMatrix;
  typedef Eigen::Triplet< double > Triplet;

  int dimr(4+1);
  int dimc(4+2);

  std::cout << "---------- 1) Creating matrix " << std::endl;
  std::vector< Triplet > triplets;
  for( int i = 0; i < dimc*dimr/2; ++i)
  {
    int x( rand()%(dimr-1));
    int y( rand()%dimc);
    double val( rand()%10);
    //std::cerr << " setting (" << x << ", " << y << ") to " << val << std::endl;
    triplets.push_back( Triplet( x, y, val));
  }
  SpMatrix A(dimr,dimc);
  A.setFromTriplets(triplets.begin(), triplets.end());
  
  std::cerr << DenMatrix(A) << std::endl;
  int m = dimr;
  int n = dimc;

  if( m < n )
  {
    std::swap( m,n);
    std::cerr << " ... m < n -> form transposed ..." << std::endl;
    A = SpMatrix(A.transpose());
    // test make also row -rank-deficinet
    A.middleCols(n-1,1) = A.middleCols(0,1);
    A.middleCols(0,1) = A.middleCols(n-2,1);
    std::cerr << DenMatrix(A) << std::endl;
  }


  std::cerr << " ... m = " << m << "; n = " << n << std::endl;
  std::cerr << std::endl;

  std::cout << "---------- 2) Sparse QR " << std::endl;
  COMISO::SparseQRSolver spqr;
  SpMatrix Q,R;
  std::vector< size_t > P;
  int rank = spqr.factorize_system_eigen( A, Q, R, P);
  int nullity(dimc-rank);
  // setup permutation matrix
  SpMatrix Pm( n, n);
  if( !P.empty())
  {
    for( size_t i = 0; i < P.size(); ++i)
    {
      Pm.coeffRef( i, P[i]) = 1;
    }
  }

  std::cout << "---------- 3) Result " << std::endl;
  std::cerr << " Q         " << std::endl << DenMatrix(Q) << std::endl;
  std::cerr << " R         " << std::endl << DenMatrix(R) << std::endl;
  std::cerr << " P         " << std::endl << P << std::endl;
  std::cerr << " P matrix  " << std::endl << DenMatrix(Pm) << std::endl;
  std::cerr << " Rank      " << rank << std::endl;
  std::cerr << " Nullity   " << nullity << std::endl;
  // extract nullspace
  SpMatrix NullSpace( Q.middleCols( std::max( 0, m-nullity), nullity));
  std::cerr << " Nullspace " << std::endl << DenMatrix(NullSpace) << std::endl;
  // non nullspace part of R
  //// assuming superflous column in R is the last (if A is also row deficient)
  //SpMatrix Rtmp(R.middleCols(0,std::min(n,n-(n-rank))).transpose());
  //SpMatrix R1( R.transpose().middleCols(0, m-nullity));
  SpMatrix Rtmp(R.transpose());
  SpMatrix R1t( Rtmp.middleCols(0,m-nullity));
  SpMatrix R1( R1t.transpose());
  std::cerr << " Non-Nullspace R " << std::endl << DenMatrix(R1) << std::endl;
  


  std::cout << "---------- 4) Verification " << std::endl;
  SpMatrix reconstructedA(Q*R*Pm.transpose());
  std::cerr << " Q orthogonal? \t " << ((fabs((Q.transpose()*Q).squaredNorm()-m) < 1e-8)?"yes":"no") << std::endl;
  std::cerr << " A = QR?       \t " << (((reconstructedA-A).squaredNorm() < 1e-8)? "yes":"no") << std::endl;


  std::cerr << std::endl << std::endl;
  std::cout << "---------- 5) Solving Ax=b (with x without nullspace component)" << std::endl;
  // NOTE: A was transposed above to be m>n
  SpMatrix b(n,1);
  SpMatrix x(m,1);
  for( int i = 0; i < n; ++i)
    b.coeffRef(i,0) = rand()%10;
  std::cerr << " ... System Ax = b .. \n";
  std::cerr << " A " << std::endl << DenMatrix(A.transpose()) << " x " << std::endl << DenMatrix(x) << " b " << std::endl << DenMatrix(b) << std::endl;

  std::cout << "---------- 5.1) test: solve using sparse QR solving .." << std::endl;
  SpMatrix At(A.transpose());
  spqr.solve_system_eigen( At, b, x);

  std::cerr << " ... solution x .. " << std::endl;
  std::cerr << DenMatrix(x) << std::endl;

  std::cerr << " ... test: is a solution ? " << (((A.transpose()*x-b).squaredNorm()<1e-8)?"yes":"no") << std::endl;
  std::cerr << " ... test: has nullspace component ? " << ((x.transpose()*NullSpace).squaredNorm()<1e-8?"yes":"no") << std::endl;
  std::cerr << " ... Nullspace projections : " << (x.transpose()*NullSpace) << std::endl;

  std::cout << "---------- 5.2) test: solve without nullspace .." << std::endl;
  SpMatrix Atnull(At);
  SpMatrix bnull(b);
  SpMatrix xnull(m,1);
  spqr.solve_system_eigen_min2norm( Atnull, bnull, xnull);
  std::cerr << " ... solution x .. " << std::endl;
  std::cerr << DenMatrix(xnull) << std::endl;

  std::cerr << " ... test: is a solution ? " << (((A.transpose()*xnull-bnull).squaredNorm()<1e-8)?"yes":"no") << std::endl;
  std::cerr << " ... test: has nullspace component ? " << ((xnull.transpose()*NullSpace).squaredNorm()<1e-8?"yes":"no") << std::endl;
  std::cerr << " ... Nullspace projections : " << (xnull.transpose()*NullSpace) << std::endl;



  return 0;
}

#else // COMISO_SUITESPARSE_SPQR_AVAILABLE

int main(void)
{
  std::cerr << " SUITESPARSE_SPQR not available, please re-configure!\n";
  return 0;
}

#endif  // COMISO_SUITESPARSE_SPQR_AVAILABLE

