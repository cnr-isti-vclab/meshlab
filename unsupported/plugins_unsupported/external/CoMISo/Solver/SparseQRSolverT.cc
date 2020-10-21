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



#define COMISO_SPARSE_QR_SOLVER_TEMPLATES_C

#include "SparseQRSolver.hh"
#include <CoMISo/Solver/Eigen_Tools.hh>


namespace COMISO {


template< class GMM_MatrixT>
bool SparseQRSolver::calc_system_gmm( const GMM_MatrixT& _mat)
{
//   std::vector<int>    colptr;
//   std::vector<int>    rowind;
//   std::vector<double> values;
    

    if(show_timings_) sw_.start();

    COMISO_GMM::get_ccs_symmetric_data( _mat,
					'c',
					values_, 
					rowind_, 
					colptr_ );
    
    if(show_timings_)
    {
      std::cerr << "SparseQRSolver Timing GMM convert: " << sw_.stop()/1000.0 << "s\n";
    }

    return calc_system( colptr_, rowind_, values_);
}
  

//-----------------------------------------------------------------------------


template< class GMM_MatrixT>
bool SparseQRSolver::update_system_gmm( const GMM_MatrixT& _mat)
{
//   std::vector<int>    colptr;
//   std::vector<int>    rowind;
//   std::vector<double> values;
    
  COMISO_GMM::get_ccs_symmetric_data( _mat,
				      'c',
				      values_, 
				      rowind_, 
				      colptr_ );

    return update_system( colptr_, rowind_, values_);
}


//-----------------------------------------------------------------------------
  
template< class Eigen_MatrixT>
bool SparseQRSolver::calc_system_eigen( const Eigen_MatrixT& _mat)
{
    if(show_timings_) sw_.start();

    COMISO_EIGEN::get_ccs_symmetric_data( _mat,
					 'c',
					 values_, 
					 rowind_, 
					 colptr_ );
    
    if(show_timings_)
    {
      std::cerr << "SparseQRSolver Timing EIGEN convert: " << sw_.stop()/1000.0 << "s\n";
    }

    return calc_system( colptr_, rowind_, values_);
}
  
//-----------------------------------------------------------------------------

template< class Eigen_MatrixT>
bool SparseQRSolver::update_system_eigen( const Eigen_MatrixT& _mat)
{
    
  COMISO_EIGEN::get_ccs_symmetric_data( _mat,
				      'c',
				       values_, 
				       rowind_, 
				       colptr_ );

  return update_system( colptr_, rowind_, values_);
}


//----------------------------------------------------------------------------- 


template< class GMM_MatrixT, class GMM_MatrixT2, class GMM_MatrixT3, class IntT>
int
SparseQRSolver::
factorize_system_gmm( const GMM_MatrixT& _A, GMM_MatrixT2& _Q, GMM_MatrixT3& _R, std::vector<IntT>& _P)
{
  std::cerr << "factorize_system_gmm" << std::endl;
  // get dimensions
  int m = gmm::mat_nrows(_A);
  int n = gmm::mat_ncols(_A);

  // 1. _A -> cholmod_sparse A
  cholmod_sparse* AC(0);
  COMISO_GMM::gmm_to_cholmod(_A, AC, mp_cholmodCommon, 0, true);
  std::cerr << "gmm_to_cholmod finished" << std::endl;
  cholmod_print_sparse(AC, "AC", mp_cholmodCommon);

  // 2. factorize A -> Q,R,P
  UF_long econ = m;
  cholmod_sparse *Q, *R;
//  UF_long *P = new UF_long[n];
  UF_long *P;
  double rank = SuiteSparseQR<double>(ordering_, tolerance_, econ, AC, &Q, &R, &P, mp_cholmodCommon);
  std::cerr << "factorization finished" << std::endl;
  std::cerr << "rank: " << rank << std::endl;
  cholmod_print_sparse(Q, "Q", mp_cholmodCommon);

  // 3. convert Q,R,P -> _Q, _R, _P
  COMISO_GMM::cholmod_to_gmm(*Q, _Q);
  COMISO_GMM::cholmod_to_gmm(*R, _R);
  std::cerr << "cholmod_to_gmm finished" << std::endl;

  _P.clear(); _P.resize(n);
  for( int i=0; i<n; ++i)
    _P[i] = P[i];
  std::cerr << "coy vector finished" << std::endl;

  cholmod_l_free_sparse(&Q, mp_cholmodCommon);
  cholmod_l_free_sparse(&R, mp_cholmodCommon);
  std::cerr << "free1 finished" << std::endl;

  // TODO: alloc or free P ???
  cholmod_free(n, sizeof(UF_long), P, mp_cholmodCommon);
  std::cerr << "free2 finished" << std::endl;


  //// [Q,R,E] = qr(A), returning Q as a sparse matrix
//template <typename Entry> UF_long SuiteSparseQR     // returns rank(A) estimate
//(
//    int ordering,           // all, except 3:given treated as 0:fixed
//    double tol,
//    UF_long econ,
//    cholmod_sparse *A,      // m-by-n sparse matrix
//    // outputs
//    cholmod_sparse **Q,     // m-by-e sparse matrix where e=max(econ,rank(A))
//    cholmod_sparse **R,     // e-by-n sparse matrix
//    UF_long **E,            // permutation of 0:n-1, NULL if identity
//    cholmod_common *cc      // workspace and parameters
//) ;

  std::cerr << " ############## QR Factorization Info #############\n";
  std::cerr << " m: " << m << ", n: " << n << ", rank: " << rank << std::endl;

  return rank;
}


//----------------------------------------------------------------------------- 


template< class Eigen_MatrixT, class IntT >
int
SparseQRSolver::
factorize_system_eigen( const Eigen_MatrixT& _A, Eigen_MatrixT& _Q, Eigen_MatrixT& _R, std::vector<IntT>& _P)
{
  std::cerr << "factorize_system_eigen" << std::endl;
  // get dimensions
  int m = _A.innerSize();
  int n = _A.outerSize();
  std::cerr << " m " << m << " n " << n << std::endl;

  // 1. _A -> cholmod_sparse A
  cholmod_sparse* AC(0);
  COMISO_EIGEN::eigen_to_cholmod(_A, AC, mp_cholmodCommon, 0, true);
  //COMISO_GMM::gmm_to_cholmod(_A, AC, mp_cholmodCommon, 0, true);
  std::cerr << "eigen_to_cholmod finished" << std::endl;
  cholmod_print_sparse(AC, "AC", mp_cholmodCommon);

  // 2. factorize A -> Q,R,P
  UF_long econ = m;
  cholmod_sparse *Q, *R;
//  UF_long *P = new UF_long[n];
  UF_long *P;
  double rank = SuiteSparseQR<double>(ordering_, tolerance_, econ, AC, &Q, &R, &P, mp_cholmodCommon);
  std::cerr << "factorization finished" << std::endl;
  std::cerr << "rank: " << rank << std::endl;
  cholmod_print_sparse(Q, "Q", mp_cholmodCommon);

  // 3. convert Q,R,P -> _Q, _R, _P
  COMISO_EIGEN::cholmod_to_eigen(*Q, _Q);
  COMISO_EIGEN::cholmod_to_eigen(*R, _R);
  std::cerr << "cholmod_to_eigen finished" << std::endl;

  _P.clear(); _P.resize(n);
  for( int i=0; i<n; ++i)
    _P[i] = P[i];
  std::cerr << "copy vector finished" << std::endl;

  cholmod_l_free_sparse(&Q, mp_cholmodCommon);
  cholmod_l_free_sparse(&R, mp_cholmodCommon);
  cholmod_l_free_sparse(&AC, mp_cholmodCommon);
  std::cerr << "free1 finished" << std::endl;

  // TODO: alloc or free P ???
  cholmod_free(n, sizeof(UF_long), P, mp_cholmodCommon);
  std::cerr << "free2 finished" << std::endl;


  //// [Q,R,E] = qr(A), returning Q as a sparse matrix
//template <typename Entry> UF_long SuiteSparseQR     // returns rank(A) estimate
//(
//    int ordering,           // all, except 3:given treated as 0:fixed
//    double tol,
//    UF_long econ,
//    cholmod_sparse *A,      // m-by-n sparse matrix
//    // outputs
//    cholmod_sparse **Q,     // m-by-e sparse matrix where e=max(econ,rank(A))
//    cholmod_sparse **R,     // e-by-n sparse matrix
//    UF_long **E,            // permutation of 0:n-1, NULL if identity
//    cholmod_common *cc      // workspace and parameters
//) ;

  std::cerr << " ############## QR Factorization Info #############\n";
  std::cerr << " m: " << m << ", n: " << n << ", rank: " << rank << std::endl;

  return rank;
}


//----------------------------------------------------------------------------- 


template< class Eigen_MatrixT >
void
SparseQRSolver::
solve_system_eigen( const Eigen_MatrixT& _A, const Eigen_MatrixT& _b, Eigen_MatrixT& _x)
{
  std::cerr << __FUNCTION__ << std::endl;

  // 1. _A -> cholmod_sparse A, x, b
  cholmod_sparse* AC(0);
  COMISO_EIGEN::eigen_to_cholmod(_A, AC, mp_cholmodCommon, 0, true);
  cholmod_print_sparse(AC, "AC", mp_cholmodCommon);

  cholmod_sparse* bC(0);
  COMISO_EIGEN::eigen_to_cholmod(_b, bC, mp_cholmodCommon, 0, true);
  cholmod_print_sparse(AC, "bC", mp_cholmodCommon);

  cholmod_sparse* xC(0);
  // allocation of X done internally
  //COMISO_EIGEN::eigen_to_cholmod_sparse(_x, xC, mp_cholmodCommon, 0, true);
  //cholmod_print_sparse(AC, "xC", mp_cholmodCommon);


  // 2. solve Ax=b
  xC = SuiteSparseQR<double>(ordering_, tolerance_, AC, bC, mp_cholmodCommon);

  std::cerr << "solve finished" << std::endl;
  cholmod_print_sparse(xC, "xC", mp_cholmodCommon);

  // 3. convert solution xC to eigen
  COMISO_EIGEN::cholmod_to_eigen(*xC, _x);
  std::cerr << "cholmod_to_eigen finished" << std::endl;

  cholmod_l_free_sparse(&AC, mp_cholmodCommon);
  cholmod_l_free_sparse(&bC, mp_cholmodCommon);
  cholmod_l_free_sparse(&xC, mp_cholmodCommon);
  std::cerr << "free1 finished" << std::endl;



//// X = A\sparse(B)
//template <typename Entry> cholmod_sparse *SuiteSparseQR
//(
//    int ordering,           // all, except 3:given treated as 0:fixed
//    double tol,
//    cholmod_sparse *A,      // m-by-n sparse matrix
//    cholmod_sparse *B,      // m-by-nrhs
//    cholmod_common *cc      // workspace and parameters
//) ;
}


//----------------------------------------------------------------------------- 


template< class Eigen_MatrixT >
void
SparseQRSolver::
solve_system_eigen_min2norm( const Eigen_MatrixT& _A, const Eigen_MatrixT& _b, Eigen_MatrixT& _x)
{
  std::cerr << __FUNCTION__ << std::endl;

  // 1. _A -> cholmod_sparse A, x, b
  cholmod_sparse* AC(0);
  COMISO_EIGEN::eigen_to_cholmod(_A, AC, mp_cholmodCommon, 0, true);
  //cholmod_print_sparse(AC, "AC", mp_cholmodCommon);

  cholmod_sparse* bC(0);
  COMISO_EIGEN::eigen_to_cholmod(_b, bC, mp_cholmodCommon, 0, true);
  //cholmod_print_sparse(AC, "bC", mp_cholmodCommon);

  cholmod_sparse* xC(0);
  // allocation of X done internally
  //COMISO_EIGEN::eigen_to_cholmod_sparse(_x, xC, mp_cholmodCommon, 0, true);
  //cholmod_print_sparse(AC, "xC", mp_cholmodCommon);


  // 2. solve Ax=b
  xC = SuiteSparseQR_min2norm<double>(ordering_, tolerance_, AC, bC, mp_cholmodCommon);

  //std::cerr << "solve finished" << std::endl;
  //cholmod_print_sparse(xC, "xC", mp_cholmodCommon);

  // 3. convert solution xC to eigen
  COMISO_EIGEN::cholmod_to_eigen(*xC, _x);
  //std::cerr << "cholmod_to_eigen finished" << std::endl;

  cholmod_l_free_sparse(&AC, mp_cholmodCommon);
  cholmod_l_free_sparse(&bC, mp_cholmodCommon);
  cholmod_l_free_sparse(&xC, mp_cholmodCommon);
  //std::cerr << "free1 finished" << std::endl;



//template <typename Entry> cholmod_sparse *SuiteSparseQR_min2norm
//(
//    int ordering,           // all, except 3:given treated as 0:fixed
//    double tol,
//    cholmod_sparse *A,
//    cholmod_sparse *B,
//    cholmod_common *cc
//) ;

}



}
