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

#include <iostream>


//== COMPILE-TIME PACKAGE REQUIREMENTS ========================================
#include <CoMISo/Config/config.hh>
#if (COMISO_ARPACK_AVAILABLE && COMISO_SUITESPARSE_AVAILABLE && COMISO_EIGEN3_AVAILABLE)
//=============================================================================

#include <CoMISo/Utils/StopWatch.hh>
#include <vector>
#include <CoMISo/EigenSolver/ArpackSolver.hh>
#define EIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET
#include <Eigen/Sparse>
#include <Eigen/Dense>


//------------------------------------------------------------------------------------------------------

// Example main
int main(void)
{
  // matrix types
#if EIGEN_VERSION_AT_LEAST(3,1,0)
  typedef Eigen::SparseMatrix<double,Eigen::ColMajor>           SMatrix;
#else
  typedef Eigen::DynamicSparseMatrix<double,Eigen::ColMajor>    SMatrix;
#endif
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>  Matrix;
  
  std::cout << "---------- 1) Setting up matrix..." << std::endl;
  unsigned int n=5;
  SMatrix A(n,n);
  // 1D Laplacian
  for(unsigned int i=0; i<n; ++i)
  {
    int count = 0;
    if( i > 0)
    {
      A.coeffRef(i,i-1) = -1.0;
      ++count;
    }
    if(i<n-1)
    {
      A.coeffRef(i,i+1) = -1.0;
      ++count;
    }

    A.coeffRef(i,i) = count;
  }
  
  
  std::cout << "---------- 2) Solving for m smallest eigenvalues and eigenvectors..." << std::endl;
  unsigned int m=3;
  COMISO::ArpackSolver arsolv;
  std::vector<double> evals;
  Matrix evects;
  arsolv.solve(A, evals, evects, m);
  
  std::cout << "---------- 3) printing results..." << std::endl;
  std::cerr << "********* eigenvalues: ";
  for(unsigned int i=0; i<evals.size(); ++i)
    std::cerr << evals[i] << ", ";
  std::cerr << std::endl;
  
  std::cerr <<"********* eigenvectors:" << std::endl;
  std::cerr << evects << std::endl;
 
  return 0;
}

//=============================================================================
#else
//=============================================================================

// Example main
int main(void)
{
  std::cerr << "Info: required dependencies are missing, abort...\n";
  return 0;
}
//=============================================================================
#endif // COMISO_SUITESPARSE_AVAILABLE
//=============================================================================
