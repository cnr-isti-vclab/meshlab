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
//  CLASS Eigen_Tools - IMPLEMENTATION
//
//=============================================================================

//== COMPILE-TIME PACKAGE REQUIREMENTS ========================================
#include <CoMISo/Config/config.hh>
#if COMISO_EIGEN3_AVAILABLE


#define COMISO_Eigen_TOOLS_C

//== INCLUDES =================================================================

#include "Eigen_Tools.hh"
#include <queue>
#include <CoMISo/Utils/StopWatch.hh>
#include <CoMISo/Utils/VSToolsT.hh>
#include <gmm/gmm.h>


//== NAMESPACES ===============================================================

namespace COMISO_EIGEN
{

//== IMPLEMENTATION ==========================================================


//-----------------------------------------------------------------------------

template<class MatrixT, class REALT, class INTT>
void get_ccs_symmetric_data( const MatrixT&      _mat,
                             const char          _uplo,
                             std::vector<REALT>& _values,
                             std::vector<INTT>&  _rowind,
                             std::vector<INTT>&  _colptr )
{
  // Assumes col major
  
   int m = _mat.innerSize();
   int n = _mat.outerSize();

   _values.resize( 0 );
   _rowind.resize( 0 );
   _colptr.resize( 0 );

   INTT iv( 0 );

   typedef typename MatrixT::InnerIterator It;

   switch ( _uplo )
   {
      case 'l':
      case 'L':
         // for all columns
         for ( int i=0; i<n; ++i )
         {
            _colptr.push_back( iv );

            // row it
            It it(_mat, i);

            for( ; it; ++it)
            {
              if( it.index() >= i )
              {
                _values.push_back( it.value());
                _rowind.push_back( it.index());
                ++iv;
              }
            }
         }
         _colptr.push_back( iv );
         break;

      case 'u':
      case 'U':
         // for all columns
         for ( int i=0; i<n; ++i )
         {
            _colptr.push_back( iv );

            // row it
            It it(_mat, i);

            for( ; it; ++it)
            {
              if( it.index() <= i )
              {
                _values.push_back( it.value());
                _rowind.push_back( it.index());
                ++iv;
              }
            }
         }
         _colptr.push_back( iv );
         break;

      case 'c':
      case 'C':
         // for all columns
         for ( int i=0; i<n; ++i )
         {
            _colptr.push_back( iv );

            // row it
            It it(_mat, i);

            for( ; it; ++it)
            {
              _values.push_back( it.value());
              _rowind.push_back( it.index());
              ++iv;
            }
         }
         _colptr.push_back( iv );
         break;

      default:
         std::cerr << "ERROR: parameter uplo must bei either 'U' or 'L' or 'C'!!!\n";
         break;
   }
}


//-----------------------------------------------------------------------------


// inspect the matrix: dimension, symmetry, zero_rows, zero_cols, nnz, max, min, max_abs, min_abs, NAN, INF
template<class MatrixT>
void inspect_matrix( const MatrixT& _A)
{


  std::cerr << "################### INSPECT MATRIX ##################\n";
  std::cerr << "#outer size  : " << _A.outerSize() << std::endl;
  std::cerr << "#inner size  : " << _A.innerSize() << std::endl;
  std::cerr << "#rows        : " << _A.rows() << std::endl;
  std::cerr << "#cols        : " << _A.cols() << std::endl;
  std::cerr << "#nonzeros    : " << _A.nonZeros() << std::endl;
  std::cerr << "#nonzeros/row: " << (double(_A.nonZeros())/double(_A.rows())) << std::endl;
  std::cerr << "symmetric    : " << is_symmetric( _A) << std::endl;

  MatrixT trans( _A.transpose());

  int zero_rows = 0;
  int zero_cols = 0;

  for(int i=0; i<_A.outerSize(); ++i)
  {
    typename MatrixT::InnerIterator it(_A, i); 
    if( !it) ++zero_rows;
  }

  for(int i=0; i<trans.outerSize(); ++i)
  {
    typename MatrixT::InnerIterator it(trans, i); 
    if( !it) ++zero_cols;
  }

  std::cerr << "zero rows    : " << zero_rows << std::endl;
  std::cerr << "zero cols    : " << zero_cols << std::endl;

  typedef typename MatrixT::Scalar Scalar;
  Scalar vmin     = std::numeric_limits<Scalar>::max();
  Scalar vmax     = std::numeric_limits<Scalar>::min();
  Scalar vmin_abs = std::numeric_limits<Scalar>::max();
  Scalar vmax_abs = 0;

  int n_nan = 0;
  int n_inf = 0;
  
  // inspect elements
  for(int i=0; i<_A.outerSize(); ++i)
  {
    typename MatrixT::InnerIterator it( _A, i);
    
    for(; it ; ++it)
    {
      if( it.value() < vmin ) vmin = it.value();
      if( it.value() > vmax ) vmax = it.value();

      if( fabs(it.value()) < vmin_abs) vmin_abs = fabs(it.value());
      if( fabs(it.value()) > vmax_abs) vmax_abs = fabs(it.value());

      if( std::isnan(it.value())) ++n_nan;
      if( std::isinf(it.value())) ++n_inf;
    }
  }
  
  std::cerr << "min  val     : " << vmin << std::endl;
  std::cerr << "max  val     : " << vmax << std::endl;
  std::cerr << "min |val|    : " << vmin_abs << std::endl;
  std::cerr << "max |val|    : " << vmax_abs << std::endl;
  std::cerr << "#nan         : " << n_nan << std::endl;
  std::cerr << "#inf         : " << n_inf << std::endl;
  
  std::cerr << "min eval     : " << "..." << std::endl;
  std::cerr << "max eval     : " << "..." << std::endl;
  std::cerr << "min|eval|    : " << "..." << std::endl;
  std::cerr << "max|eval|    : " << "..." << std::endl;
}

//-----------------------------------------------------------------------------


// symmetric ?
template<class MatrixT>
bool is_symmetric( const MatrixT& _A)
{
  typedef typename MatrixT::InnerIterator It;
  typedef typename MatrixT::Scalar Scalar;

  int nouter( _A.outerSize());
  int ninner( _A.innerSize());
  
  if( nouter != ninner )
    return false;

  bool symmetric(true);

  for( int c = 0; c < nouter; ++c)
  {
    for( It it(_A,c); it; ++it)
    {
      int r(it.index());

      Scalar val(it.value());

      // find diagonal partner element
      bool found(false);
      for( It dit(_A,r); dit; ++dit)
      {
        if( dit.index() < c )
        {}
        else if( dit.index() == c)
        {
          if( dit.value() == val) 
            found = true;
          break;
        }
        else 
        {
          break;
        }
      }
      if( !found) 
      {
        symmetric = false;
        break;
      }
    }
  }
  return symmetric;
}

//-----------------------------------------------------------------------------


template< class Eigen_MatrixT, class IntT >
void permute( const Eigen_MatrixT& _QR, const std::vector< IntT>& _Pvec, Eigen_MatrixT& _A)
{
#ifdef COMISO_EIGEN3_AVAILABLE
  typedef typename Eigen_MatrixT::Scalar Scalar;

  int m = _QR.innerSize();
  int n = _QR.outerSize();
  
  if( _Pvec.size() == 0)
  {
    _A = _QR;
    return;
  }
    
  if( _Pvec.size() != (size_t)_QR.cols() && _Pvec.size() != 0)
  {
    std::cerr << __FUNCTION__ << " wrong size of permutation vector, should have #cols length (or zero)" << std::endl;
  }

  // build sparse permutation matrix
  typedef Eigen::Triplet< Scalar > Triplet;
  std::vector< Triplet > triplets;
  triplets.reserve(_QR.nonZeros());
  _A = Eigen_MatrixT( m, n);

  typedef typename Eigen_MatrixT::InnerIterator It;

  
  for( int c = 0; c < n; ++c) // cols
  {
    for( It it(_QR,c); it; ++it) // rows
    {
      int r(it.index());

      Scalar val(it.value());

      int newcol( _Pvec[c]);

      triplets.push_back( Triplet( r, newcol, val));
    }
  }
  _A.setFromTriplets( triplets.begin(), triplets.end());
#endif
}

//-----------------------------------------------------------------------------

#if COMISO_SUITESPARSE_AVAILABLE

/// Eigen to Cholmod_sparse interface
template<class MatrixT>
void cholmod_to_eigen( const cholmod_sparse& _AC, MatrixT& _A)
{
#ifdef COMISO_EIGEN3_AVAILABLE
  // initialize dimensions
  typedef typename MatrixT::Scalar Scalar;
  typedef Eigen::Triplet< Scalar > Triplet;
  size_t nzmax( _AC.nzmax);
  std::cerr << __FUNCTION__ << " row " << _AC.nrow << " col " << _AC.ncol << " stype " << _AC.stype << std::endl;
  _A = MatrixT(_AC.nrow, _AC.ncol);
  std::vector< Triplet > triplets;
  triplets.reserve(nzmax);

  if(!_AC.packed)
  {
    std::cerr << "Warning: " << __FUNCTION__ << " does not support unpacked matrices yet!!!" << std::endl;
    return;
  }

  // Pointer to data
  double* X((double*)_AC.x);

  // complete matrix stored
  if(_AC.stype == 0)
  {
    // which integer type?
    if(_AC.itype == CHOLMOD_LONG)
    {
      UF_long* P((UF_long*)_AC.p);
      UF_long* I((UF_long*)_AC.i);

      for(UF_long i=0; i<(UF_long)_AC.ncol; ++i)
        for(UF_long j= P[i]; j< P[i+1]; ++j)
          //_A( I[j], i) += X[j]; // += really needed?
          triplets.push_back( Triplet( I[j], i, X[j]));
    }
    else
    {
      int* P((int*)_AC.p);
      int* I((int*)_AC.i);

      for(int i=0; i<(int)_AC.ncol; ++i)
        for(int j= P[i]; j< P[i+1]; ++j)
          triplets.push_back( Triplet( I[j], i, X[j]));
      //_A( I[j], i) += X[j];
    }

  }
  else // only upper or lower diagonal stored
  {
    // which integer type?
    if(_AC.itype == CHOLMOD_LONG)
    {
      UF_long* P((UF_long*)_AC.p);
      UF_long* I((UF_long*)_AC.i);

      for(UF_long i=0; i<(UF_long)_AC.ncol; ++i)
        for(UF_long j=P[i]; j<P[i+1]; ++j)
        {
          //_A(I[j], i) += X[j];
          triplets.push_back( Triplet( I[j], i, X[j]));

          // add up symmetric part
          if( I[j] != i)
            triplets.push_back( Triplet( i, I[j], X[j]));
          //_A(i,I[j]) += X[j];
        }
    }
    else
    {
      int* P((int*)_AC.p);
      int* I((int*)_AC.i);

      for(int i=0; i<(int)_AC.ncol; ++i)
        for(int j=P[i]; j<P[i+1]; ++j)
        {
          //_A(I[j], i) += X[j];
          triplets.push_back( Triplet( I[j], i, X[j]));

          // add up symmetric part
          if( I[j] != i)
            //  _A(i,I[j]) += X[j];
            triplets.push_back( Triplet( i, I[j], X[j]));
        }
    }
  }
  _A.setFromTriplets( triplets.begin(), triplets.end());
#endif
}


/// GMM to Cholmod_sparse interface
template<class MatrixT>
void eigen_to_cholmod( const MatrixT& _A, cholmod_sparse* &_AC, cholmod_common* _common, int _sparsity_type, bool _long_int)
{
  /* _sparsity_type
          * 0:  matrix is "unsymmetric": use both upper and lower triangular parts
          *     (the matrix may actually be symmetric in pattern and value, but
          *     both parts are explicitly stored and used).  May be square or
          *     rectangular.
          * >0: matrix is square and symmetric, use upper triangular part.
          *     Entries in the lower triangular part are ignored.
          * <0: matrix is square and symmetric, use lower triangular part.
          *     Entries in the upper triangular part are ignored. */

  int m = _A.innerSize();
  int n = _A.outerSize();

  // get upper or lower
  char uplo = 'c';
  if(_sparsity_type < 0) uplo = 'l';
  if(_sparsity_type > 0) uplo = 'u';


  if( _long_int) // long int version
  {
    std::vector<double> values;
    std::vector<UF_long> rowind;
    std::vector<UF_long> colptr;

    // get data of gmm matrix
    COMISO_EIGEN::get_ccs_symmetric_data( _A, uplo, values, rowind, colptr);

    // allocate cholmod matrix
    _AC = cholmod_l_allocate_sparse(m,n,values.size(),true,true,_sparsity_type,CHOLMOD_REAL, _common);

    // copy data to cholmod matrix
    for(UF_long i=0; i<(UF_long)values.size(); ++i)
    {
      ((double*) (_AC->x))[i] = values[i];
      ((UF_long*)(_AC->i))[i] = rowind[i];
    }

    for(UF_long i=0; i<(UF_long)colptr.size(); ++i)
      ((UF_long*)(_AC->p))[i] = colptr[i];
  }
  else // int version
  {
     std::vector<double> values;
     std::vector<int> rowind;
     std::vector<int> colptr;

     // get data of gmm matrix
     COMISO_EIGEN::get_ccs_symmetric_data( _A, uplo, values, rowind, colptr);

     // allocate cholmod matrix
     _AC = cholmod_allocate_sparse(m,n,values.size(),true,true,_sparsity_type,CHOLMOD_REAL, _common);

     // copy data to cholmod matrix
     for(unsigned int i=0; i<values.size(); ++i)
     {
       ((double*)(_AC->x))[i] = values[i];
       ((int*)   (_AC->i))[i] = rowind[i];
     }
     for(unsigned int i=0; i<colptr.size(); ++i)
       ((int*)(_AC->p))[i] = colptr[i];
  }

}
#endif

/*
/// Eigen to Cholmod_dense interface
template<class MatrixT>
void cholmod_to_eigen_dense( const cholmod_dense& _AC, MatrixT& _A)
{
  // initialize dimensions
  typedef typename MatrixT::Scalar Scalar;
  typedef Eigen::Triplet< Scalar > Triplet;
  size_t nzmax( _AC.nzmax);
  _A = MatrixT(_AC.nrow, _AC.ncol);
  std::vector< Triplet > triplets;
  triplets.reserve(nzmax);

  if(!_AC.packed)
  {
    std::cerr << "Warning: " << __FUNCTION__ << " does not support unpacked matrices yet!!!" << std::endl;
    return;
  }

  // Pointer to data
  double* X((double*)_AC.x);

  // complete matrix stored
  if(_AC.stype == 0)
  {
    // which integer type?
    if(_AC.itype == CHOLMOD_LONG)
    {
      UF_long* P((UF_long*)_AC.p);
      UF_long* I((UF_long*)_AC.i);

      for(UF_long i=0; i<(UF_long)_AC.ncol; ++i)
        for(UF_long j= P[i]; j< P[i+1]; ++j)
          //_A( I[j], i) += X[j]; // += really needed?
          triplets.push_back( Triplet( I[j], i, X[j]));
    }
    else
    {
      int* P((int*)_AC.p);
      int* I((int*)_AC.i);

      for(int i=0; i<(int)_AC.ncol; ++i)
        for(int j= P[i]; j< P[i+1]; ++j)
          triplets.push_back( Triplet( I[j], i, X[j]));
      //_A( I[j], i) += X[j];
    }

  }
  else // only upper or lower diagonal stored
  {
    // which integer type?
    if(_AC.itype == CHOLMOD_LONG)
    {
      UF_long* P((UF_long*)_AC.p);
      UF_long* I((UF_long*)_AC.i);

      for(UF_long i=0; i<(UF_long)_AC.ncol; ++i)
        for(UF_long j=P[i]; j<P[i+1]; ++j)
        {
          //_A(I[j], i) += X[j];
          triplets.push_back( Triplet( I[j], i, X[j]));

          // add up symmetric part
          if( I[j] != i)
            triplets.push_back( Triplet( i, I[j], X[j]));
          //_A(i,I[j]) += X[j];
        }
    }
    else
    {
      int* P((int*)_AC.p);
      int* I((int*)_AC.i);

      for(int i=0; i<(int)_AC.ncol; ++i)
        for(int j=P[i]; j<P[i+1]; ++j)
        {
          //_A(I[j], i) += X[j];
          triplets.push_back( Triplet( I[j], i, X[j]));

          // add up symmetric part
          if( I[j] != i)
            //  _A(i,I[j]) += X[j];
            triplets.push_back( Triplet( i, I[j], X[j]));
        }
    }
  }
  _A.setFromTriplets( triplets.begin(), triplets.end());
}


/// GMM to Cholmod_sparse interface
template<class MatrixT>
void eigen_to_cholmod_dense( const MatrixT& _A, cholmod_dense* &_AC, cholmod_common* _common, bool _long_int)
{
  int m = _A.innerSize();
  int n = _A.outerSize();

  // allocate cholmod matrix
  _AC = cholmod_l_allocate_sparse(m,n,values.size(),true,true,_sparsity_type,CHOLMOD_REAL, _common);
  _AC = cholmod_l_allocate_dense (m,n,n, xtype, cc) 

  if( _long_int) // long int version
  {
    std::vector<double> values;
    std::vector<UF_long> rowind;
    std::vector<UF_long> colptr;

    // get data of gmm matrix
    COMISO_EIGEN::get_ccs_symmetric_data( _A, uplo, values, rowind, colptr);

    // allocate cholmod matrix
    _AC = cholmod_l_allocate_sparse(m,n,values.size(),true,true,_sparsity_type,CHOLMOD_REAL, _common);

    // copy data to cholmod matrix
    for(UF_long i=0; i<(UF_long)values.size(); ++i)
    {
      ((double*) (_AC->x))[i] = values[i];
      ((UF_long*)(_AC->i))[i] = rowind[i];
    }

    for(UF_long i=0; i<(UF_long)colptr.size(); ++i)
      ((UF_long*)(_AC->p))[i] = colptr[i];
  }
  else // int version
  {
     std::vector<double> values;
     std::vector<int> rowind;
     std::vector<int> colptr;

     // get data of gmm matrix
     COMISO_EIGEN::get_ccs_symmetric_data( _A, uplo, values, rowind, colptr);

     // allocate cholmod matrix
     _AC = cholmod_allocate_sparse(m,n,values.size(),true,true,_sparsity_type,CHOLMOD_REAL, _common);

     // copy data to cholmod matrix
     for(unsigned int i=0; i<values.size(); ++i)
     {
       ((double*)(_AC->x))[i] = values[i];
       ((int*)   (_AC->i))[i] = rowind[i];
     }
     for(unsigned int i=0; i<colptr.size(); ++i)
       ((int*)(_AC->p))[i] = colptr[i];
  }

}*/

// convert a gmm col-sparse matrix into an eigen sparse matrix
template<class GMM_MatrixT, class EIGEN_MatrixT>
void gmm_to_eigen( const GMM_MatrixT& _G, EIGEN_MatrixT& _E)
{
#ifdef COMISO_EIGEN3_AVAILABLE
  typedef typename EIGEN_MatrixT::Scalar Scalar;

  typedef typename gmm::linalg_traits<GMM_MatrixT>::const_sub_col_type ColT;
  typedef typename gmm::linalg_traits<ColT>::const_iterator CIter;

  // build matrix triplets
  typedef Eigen::Triplet< Scalar > Triplet;
  std::vector< Triplet > triplets;
  triplets.reserve(gmm::nnz(_G));

  for(unsigned int i=0; i<gmm::mat_ncols(_G); ++i)
  {
     ColT col = mat_const_col( _G, i );

     CIter it  = gmm::vect_const_begin( col );
     CIter ite = gmm::vect_const_end( col );
     for ( ; it!=ite; ++it )
       triplets.push_back( Triplet( it.index(), i, *it));
  }

  // generate eigen matrix
  _E = EIGEN_MatrixT( gmm::mat_nrows(_G), gmm::mat_ncols(_G));
  _E.setFromTriplets( triplets.begin(), triplets.end());
#endif
}

//=============================================================================
} // namespace COMISO
//=============================================================================

//=============================================================================
#endif // COMISO_EIGEN3_AVAILABLE
//=============================================================================
