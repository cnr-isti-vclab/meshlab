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
//  CLASS GMM_Tools - IMPLEMENTATION
//
//=============================================================================

#define COMISO_GMM_TOOLS_C

//== INCLUDES =================================================================

#include "GMM_Tools.hh"
#define GMM_USES_LAPACK
#include <gmm/gmm_lapack_interface.h>
#include <queue>
#include <CoMISo/Utils/StopWatch.hh>
#include <CoMISo/Utils/VSToolsT.hh>


//== NAMESPACES ===============================================================

namespace COMISO_GMM
{

//== IMPLEMENTATION ==========================================================


//-----------------------------------------------------------------------------


// too many if-statements, seems slower in most cases
// use eliminate_csc_vars2
template<class ScalarT, class VectorT, class RealT, class IntegerT>
void eliminate_csc_vars(
    const std::vector<IntegerT>&     _evar,
    const std::vector<ScalarT>&      _eval,
    typename gmm::csc_matrix<RealT>&  _A,
    VectorT&                          _x,
    VectorT&                          _rhs )
{
  typedef unsigned int uint;
  typedef typename gmm::csc_matrix<RealT> MatrixT;

  unsigned int nc = _A.nc;
  unsigned int nr = _A.nr;

  unsigned int n_new = nc - _evar.size();

  // modify rhs
  for ( unsigned int k=0; k<_evar.size(); ++k )
  {
    IntegerT i = _evar[k];

    // number of elements in this column
    uint n_elem = _A.jc[i+1]-_A.jc[i];
    uint r_idx  = 0;
    RealT entry = 0.0;
    for( uint i_elem = _A.jc[i]; i_elem < _A.jc[i+1]; ++i_elem)
    {
      r_idx = _A.ir[i_elem];
      entry = _A.pr[i_elem];
      _rhs[r_idx] -= _eval[k]*( entry );
    }
  }

  // sort vector
  std::vector<IntegerT> evar( _evar );
  std::sort( evar.begin(), evar.end() );
  evar.push_back( std::numeric_limits<IntegerT>::max() );

  // build subindex set and update rhs
  std::vector<size_t> si( n_new );
  int cur_evar_idx=0;
  for ( unsigned int i=0; i<nc; ++i )
  {
    unsigned int next_i = evar[cur_evar_idx];
    if ( i != next_i )
    {
      _rhs[i-cur_evar_idx] = _rhs[i];
      _x  [i-cur_evar_idx] = _x  [i];
      si  [i-cur_evar_idx] = i;
    }
    else
      ++cur_evar_idx;
  }

  // delete last elements
  _rhs.resize( n_new );
  _x.resize( n_new );

  // csc erasing rows and columns
  uint offset(0);
  uint next_evar_row(0);
  uint next_evar_col(0);
  uint last_jc = _A.jc[0];
  uint col_offset(0);
  uint offset_update(0);
  uint last_evar_idx=evar.size()-2;

  for( uint c = 0; c < nc; ++c)
  {
    uint el_span( _A.jc[c+1]-last_jc);

    offset_update=0;
    if( c != (uint) evar[next_evar_col] )
    {
      next_evar_row=0;
      for( uint e = last_jc; e < _A.jc[c+1]; ++e)
      {
        while( _A.ir[e] > (uint) evar[next_evar_row])
        {
          ++next_evar_row;
          ++offset_update;
        }
        _A.pr[e-offset] = _A.pr[e];
        _A.ir[e-offset] = _A.ir[e]-offset_update;
        if( _A.ir[e] == (uint)evar[next_evar_row])
        {
          ++offset;
        }
      }
      last_jc = _A.jc[c+1];
      _A.jc[c+1-col_offset] = _A.jc[c+1]-offset;
    }
    else
    {
      ++col_offset;
      offset+=el_span;
      ++next_evar_col;
      last_jc = _A.jc[c+1];
    }
  }
  _A.nc = nc - evar.size()+1;
  _A.nr = nr - evar.size()+1;
}


//-----------------------------------------------------------------------------


template<class ScalarT, class VectorT, class RealT, class IntegerT>
void eliminate_csc_vars2(
    const std::vector<IntegerT>&     _evar,
    const std::vector<ScalarT>&      _eval,
    typename gmm::csc_matrix<RealT>&  _A,
    VectorT&                          _x,
    VectorT&                          _rhs )
{
  typedef unsigned int uint;
  typedef typename gmm::csc_matrix<RealT> MatrixT;

  unsigned int nc = _A.nc;
  unsigned int nr = _A.nr;

  unsigned int n_new = nc - _evar.size();

  // modify rhs
  for ( unsigned int k=0; k<_evar.size(); ++k )
  {
    IntegerT i = _evar[k];

    // number of elements in this column
    //    uint n_elem = _A.jc[i+1]-_A.jc[i];
    uint r_idx  = 0;
    RealT entry = 0.0;
    for( uint i_elem = _A.jc[i]; i_elem < _A.jc[i+1]; ++i_elem)
    {
      r_idx = _A.ir[i_elem];
      entry = _A.pr[i_elem];
      _rhs[r_idx] -= _eval[k]*( entry );
    }
  }

  // sort vector
  std::vector<IntegerT> evar( _evar );
  std::sort( evar.begin(), evar.end() );
  evar.push_back( std::numeric_limits<IntegerT>::max() );

  // build subindex set and update rhs
  std::vector<size_t> si( n_new );
  int cur_evar_idx=0;
  for ( unsigned int i=0; i<nc; ++i )
  {

    unsigned int next_i = evar[cur_evar_idx];

    if ( i != next_i )
    {

      _rhs[i-cur_evar_idx] = _rhs[i];
      _x  [i-cur_evar_idx] = _x  [i];
      si  [i-cur_evar_idx] = i;

    }
    else
    {
      ++cur_evar_idx;
    }
  }

  // delete last elements
  _rhs.resize( n_new );
  _x.resize( n_new );

  std::vector< int > new_row_idx_map( nr, -1);
  // build re-indexing map
  // -1 means deleted
  {
    int next_evar_idx(0);
    int offset(0);
    for( int i = 0; i < (int)nr; ++i)
    {
      if( i == (int)evar[next_evar_idx])
      {
        ++next_evar_idx;
        ++offset;
      }
      else
      {
        new_row_idx_map[i] = i-offset;
      }
    }
  }

  // csc erasing rows and columns
  int read(0), write(0), evar_col(0), last_jc(0);
  for( unsigned int c = 0; c < nc; ++c)
  {
    if( c == (unsigned int)evar[evar_col] )
    {
      ++evar_col;
      read += _A.jc[c+1]-last_jc;
    }
    else
    {
      while( read < (int)_A.jc[c+1] )
      {
        int new_idx = new_row_idx_map[_A.ir[read]];
        if( new_idx != -1)
        {
          _A.pr[write] = _A.pr[read];
          _A.ir[write] = new_idx;
          ++write;
        }
        ++read;
      }
   }
    last_jc = _A.jc[c+1];

    _A.jc[c+1-evar_col] = write;
  }

  _A.nc = nc - evar.size()+1;
  _A.nr = nr - evar.size()+1;
}


//-----------------------------------------------------------------------------


template<class MatrixT, class REALT, class INTT>
void get_ccs_symmetric_data( const MatrixT&      _mat,
                             const char          _uplo,
                             std::vector<REALT>& _values,
                             std::vector<INTT>&  _rowind,
                             std::vector<INTT>&  _colptr )
{
   unsigned int m = gmm::mat_nrows( _mat );
   unsigned int n = gmm::mat_ncols( _mat );

   gmm::csc_matrix<REALT> csc_mat( m,n );
   gmm::copy( _mat, csc_mat );

   _values.resize( 0 );
   _rowind.resize( 0 );
   _colptr.resize( 0 );

   INTT iv( 0 );

   switch ( _uplo )
   {
      case 'l':
      case 'L':
         // for all columns
         for ( unsigned int i=0; i<n; ++i )
         {
            _colptr.push_back( iv );
            for ( unsigned int j=( csc_mat.jc )[i]; j<( csc_mat.jc )[i+1]; ++j )
               if (( csc_mat.ir )[j] >= i )
               {
                  ++iv;
                  _values.push_back(( csc_mat.pr )[j] );
                  _rowind.push_back(( csc_mat.ir )[j] );
               }
         }
         _colptr.push_back( iv );
         break;

      case 'u':
      case 'U':
         // for all columns
         for ( unsigned int i=0; i<n; ++i )
         {
            _colptr.push_back( iv );
            for ( unsigned int j=( csc_mat.jc )[i]; j<( csc_mat.jc )[i+1]; ++j )
               if (( csc_mat.ir )[j] <= i )
               {
                  ++iv;
                  _values.push_back(( csc_mat.pr )[j] );
                  _rowind.push_back(( csc_mat.ir )[j] );
               }
         }
         _colptr.push_back( iv );
         break;

      case 'c':
      case 'C':
         // for all columns
         for ( unsigned int i=0; i<n; ++i )
         {
            _colptr.push_back( iv );
            for ( unsigned int j=( csc_mat.jc )[i]; j<( csc_mat.jc )[i+1]; ++j )
	      //               if (( csc_mat.ir )[j] <= i )
               {
                  ++iv;
                  _values.push_back(( csc_mat.pr )[j] );
                  _rowind.push_back(( csc_mat.ir )[j] );
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


template<class ScalarT, class VectorT, class SVT>
void eliminate_var( const unsigned int _j,
                    const ScalarT      _value_j,
                    gmm::col_matrix<SVT>& _A,
                    VectorT&      _x,
                    VectorT&      _rhs )
{
   //ACG::StopWatch sw1;

   //sw1.start();

   typedef typename gmm::linalg_traits< gmm::col_matrix< SVT > >::const_sub_col_type ColT;
   typedef typename gmm::linalg_traits<ColT>::const_iterator CIter;

   unsigned int m = gmm::mat_nrows( _A );
   unsigned int n = gmm::mat_ncols( _A );


   // update rhs
   ColT col = mat_const_col( _A, _j );
   CIter it  = gmm::vect_const_begin( col );
   CIter ite = gmm::vect_const_end( col );

   for ( ; it!=ite; ++it )
   {
      _rhs[it.index()] -= _value_j*( *it );
   }

   _rhs.erase( _rhs.begin() + _j );
   _x.erase( _x.begin() + _j );

   //std::cerr << " FIRST BLOCK " << sw1.stop()/1000.0 << std::endl;
   /*
   for(unsigned int i=_j; i<m-1; ++i)
   {
     _rhs[i] = _rhs[i+1];
     _x  [i] = _x  [i+1];
   }

   // delete last element
   _rhs.resize( _rhs.size()-1);
   _x.resize( _x.size()-1);
   */

   /*
   MatrixT A_temp( m, n);
   gmm::copy(_A, A_temp);
   gmm::clear(_A);

   for(unsigned int i=0; i<m; ++i)
   {
     // skip _j'th column
     if( i==_j) continue;

     ColT col = mat_const_col(A_temp, i);

     CIter it  = gmm::vect_const_begin(col);
     CIter ite = gmm::vect_const_end(col);
     
     // compute new index
     unsigned int i_new = i;
     if( i>_j) --i_new;

     for(; it!=ite; ++it)
     {
       unsigned int j_new = it.index();
       if( j_new == _j) continue;
       if( j_new > _j) --j_new;
       _A(j_new,i_new) = *it;
       //       if( it.index() != j_new || i_new != i)
       // 	_A(it.index(), i) = 0.0;

     }
   }

   gmm::resize( _A, m-1, n-1);
   */
   //sw1.start();

   typedef typename gmm::linalg_traits<SVT>::const_iterator SIter;

   for ( unsigned int i=0; i<m; ++i )
   {
      // skip _j'th column
      if ( i==_j ) continue;

      // compute new index
      unsigned int i_new = i;
      if ( i>_j ) --i_new;

      SVT* cur_col = &( _A.col( i ) );
      if ( i == i_new )
      {
         cur_col = new SVT( _A.col( i ) );
         //gmm::copy( _A.col(i), *cur_col);
      }

      SIter it  = gmm::vect_const_begin( *cur_col );
      SIter ite = gmm::vect_const_end( *cur_col );

      //mat_col(_A, i_new).clear();
      SVT& new_col = _A.col( i_new );
      new_col.clear();

      for ( ; it!=ite; ++it )
      {
         unsigned int j_new = it.index();
         if ( j_new == _j ) continue;
         if ( j_new > _j ) --j_new;
         //_A(j_new,i_new) = *it;
         new_col[j_new] = *it;
         //       if( it.index() != j_new || i_new != i)
         // 	_A(it.index(), i) = 0.0;

      }
      if ( i == i_new )
      {
         delete cur_col;
      }
   }
   //std::cerr << " BLOCK TWO " << sw1.stop()/1000.0 << std::endl;
   //sw1.start();

   gmm::resize( _A, m-1, n-1 );
   //std::cerr << "RESIZE " << sw1.stop()/1000.0 << std::endl;
}

//-----------------------------------------------------------------------------


template<class ScalarT, class VectorT, class RealT>
void eliminate_var( const unsigned int                _i,
                    const ScalarT                     _xi,
                    typename gmm::csc_matrix<RealT>&  _A,
                    VectorT&                          _x,
                    VectorT&                          _rhs )
{
   unsigned int n = _A.nc;

   unsigned int iv_old( 0 );
   unsigned int iv_new( 0 );

   // for all columns
   for ( unsigned int j=0; j<n; ++j )
   {
      // update x and rhs
      if ( j > _i )
      {
         _rhs[j-1] = _rhs[j];
         _x  [j-1] = _x  [j];
      }


      if ( j == _i )
      {
         // update rhs
         for ( unsigned int i=_A.jc[j]; i<_A.jc[j+1]; ++i )
         {
            _rhs[_A.ir[iv_old]] -= _xi*_A.pr[iv_old];
            ++iv_old;
         }
      }
      else
      {
         // store index to update colptr
         unsigned int iv_new_save = iv_new;

         for ( unsigned int i=_A.jc[j]; i<_A.jc[j+1]; ++i )
         {
            if ( _A.ir[iv_old] < _i )
            {
               _A.ir[iv_new] = _A.ir[iv_old];
               _A.pr[iv_new] = _A.pr[iv_old];
               ++iv_new;
            }
            else if ( _A.ir[iv_old] > _i )
            {
               _A.ir[iv_new] = _A.ir[iv_old]-1;
               _A.pr[iv_new] = _A.pr[iv_old];
               ++iv_new;
            }
            ++iv_old;
         }

         if ( j<_i )
            _A.jc[j] = iv_new_save;
         else
            if ( j>_i )
               _A.jc[j-1] = iv_new_save;
      }
   }

   // store index to end
   _A.jc[n-1] = iv_new;

   // resize matrix and vectors
   _A.nc = n-1;
   _A.nr = n-1;
   _x.resize( _x.size()-1 );
   _rhs.resize( _rhs.size()-1 );
}


//-----------------------------------------------------------------------------


template<class IntegerT, class ScalarT, class VectorT, class MatrixT>
void eliminate_vars( const std::vector<IntegerT>&     _evar,
                     const std::vector<ScalarT>&      _eval,
                     MatrixT&     _A,
                     VectorT&     _x,
                     VectorT&     _rhs )
{
  std::cerr << __FUNCTION__ << std::endl;
  typedef typename gmm::linalg_traits<MatrixT>::const_sub_col_type ColT;
  typedef typename gmm::linalg_traits<ColT>::const_iterator CIter;

  //  unsigned int m = gmm::mat_nrows( _A);
  unsigned int n = gmm::mat_ncols( _A );

  unsigned int n_new = n - _evar.size();

  // modify rhs
  for ( unsigned int k=0; k<_evar.size(); ++k )
  {
    IntegerT i = _evar[k];

    ColT col = mat_const_col( _A, i );

    CIter it  = gmm::vect_const_begin( col );
    CIter ite = gmm::vect_const_end( col );

    for ( ; it!=ite; ++it )
    {
      _rhs[it.index()] -= _eval[k]*( *it );
    }
  }

  // sort vector
  std::vector<IntegerT> evar( _evar );
  std::sort( evar.begin(), evar.end() );
  evar.push_back( std::numeric_limits<int>::max() );

  // build subindex set and update rhs
  std::vector<size_t> si( n_new );
  int cur_evar_idx=0;
  for ( unsigned int i=0; i<n; ++i )
  {
    unsigned int next_i = evar[cur_evar_idx];

    if ( i != next_i )
    {
      _rhs[i-cur_evar_idx] = _rhs[i];
      _x  [i-cur_evar_idx] = _x  [i];
      si  [i-cur_evar_idx] = i;
    }
    else
    {
      ++cur_evar_idx;
    }
  }

  // delete last element
  _rhs.resize( n_new );
  _x.resize( n_new );

  // copy and resize
  // ToDo: doesn't work without temp matrix ... (more efficient possibility?)
  //MatrixT A_temp( n_new, n_new );
  //gmm::copy( gmm::sub_matrix( _A, gmm::sub_index( si ), gmm::sub_index( si ) ), A_temp );
  //gmm::resize( _A, n_new, n_new );
  //gmm::copy( A_temp, _A );









  // to remove last "virtual" element
  evar.resize(evar.size()-1);


  //std::cerr << "THIS IS THE VECTOR OF COLUMNS THAT SHALL DISAPPEAR!s" << evar << std::endl;

  typename std::vector< IntegerT >::iterator col_it  = evar.begin();
  typename std::vector< IntegerT >::iterator col_end = evar.end();

  int next_i = *col_it;
  for( int i = *col_it; i < (int)_A.ncols(); ++i)
  {
    if( col_it != col_end && i == *col_it)
    {
      ++col_it;
    }
    else
    {
      _A.col(next_i) = _A.col(i);
      ++next_i;
    }
  }

  gmm::resize(_A, gmm::mat_nrows(_A), gmm::mat_ncols(_A)-evar.size());
  gmm::row_matrix< gmm::wsvector<double> > Arow( gmm::mat_nrows(_A), gmm::mat_ncols(_A));
  gmm::copy(_A, Arow);
  col_it  = evar.begin();
  col_end = evar.end();


  int next_ii = *col_it;
  for( int i = *col_it; i < (int)_A.nrows(); ++i)
  {
    if( col_it != col_end && i == *col_it)
    {
      ++col_it;
    }
    else
    {
      Arow.row(next_ii) = Arow.row(i);
      ++next_ii;
    }
  }

  gmm::resize( _A, gmm::mat_ncols(Arow), gmm::mat_ncols(Arow));
  gmm::resize( Arow, gmm::mat_ncols(Arow), gmm::mat_ncols(Arow));
  gmm::copy(   Arow, _A);




  //alternative (probably not working anymore)
  /*
     MatrixT Atmp( gmm::mat_nrows(_A) - evar.size(), gmm::mat_ncols(_A) - evar.size());

     evar.push_back(INT_MAX);
     unsigned int next_evar_row=0, next_evar_col=0;
     unsigned int col_offset = 0;
     for( unsigned int c = 0; c < mat_ncols(_A); ++c)
     {
     if( c != evar[next_evar_col] )
     {
     CIter it  = gmm::vect_const_begin( mat_const_col( _A, c ) );
     CIter ite = gmm::vect_const_end( mat_const_col( _A, c) );

     unsigned int row_offset = 0;
     next_evar_row=0;
     for( ; it != ite; ++it)
     {
     while( it.index() > evar[next_evar_row] )
     {
     ++row_offset;
     ++next_evar_row;
     }
     if( it.index() != evar[next_evar_row])
     {
     Atmp( it.index() - row_offset, c - col_offset) = *it;
     }
     }
     }
     else
     {
     ++col_offset;
     ++next_evar_col;
     }
     }
     gmm::resize( _A, gmm::mat_nrows(Atmp), gmm::mat_ncols(Atmp));
     gmm::copy( Atmp, _A);
   */

}


//-----------------------------------------------------------------------------


template<class IntegerT, class IntegerT2>
void eliminate_vars_idx( const std::vector<IntegerT >&     _evar,
                         std::vector<IntegerT2>&     _idx,
                         IntegerT2                   _dummy, 
			 IntegerT2                   _range )
{
   // sort input
   std::vector<IntegerT> evar( _evar );
   std::sort( evar.begin(), evar.end() );

   // precompute update
   IntegerT2 range = _range;
   if( range == -1 )
     range = _idx.size();
   std::vector<int> update_map( range );

   typename std::vector<IntegerT>::iterator cur_var = evar.begin();
   unsigned int deleted_between = 0;

   for ( unsigned int i=0; i<update_map.size(); ++i )
   {
     if ( cur_var != evar.end() && (IntegerT)i == *cur_var )
      {
         // mark as deleted
         update_map[i] = _dummy;

         ++deleted_between;
         ++cur_var;
      }
      else
      {
         update_map[i] = i-deleted_between;
      }
   }

   for ( unsigned int i=0; i<_idx.size(); ++i )
   {
     if ( (IntegerT2)_idx[i] != _dummy )
      {
         _idx[i] = update_map[_idx[i]];
      }
   }
}


//-----------------------------------------------------------------------------


template<class IntegerT, class IntegerT2>
void eliminate_var_idx( const IntegerT           _evar,
                        std::vector<IntegerT2>&  _idx,
                        IntegerT2                _dummy )
{
   for ( unsigned int i=0; i<_idx.size(); ++i )
   {
      if ( _idx[i] != _dummy )
      {
         if ( _idx[i] == ( IntegerT2 )_evar )
            _idx[i] = _dummy;
         else
         {
            if ( _idx[i] > ( IntegerT2 )_evar )
               --_idx[i];
         }
      }
   }
}



//-----------------------------------------------------------------------------


template<class ScalarT, class VectorT, class RealT>
void fix_var_csc_symmetric( const unsigned int                _i,
			    const ScalarT                     _xi,
			    typename gmm::csc_matrix<RealT>&  _A,
			    VectorT&                          _x,
			    VectorT&                          _rhs )
{
// GMM CSC FORMAT
//     T *pr;        // values.
//     IND_TYPE *ir; // row indices.
//     IND_TYPE *jc; // column repartition on pr and ir.

  unsigned int n = _A.nc;

  // update x
  _x[_i]   = _xi;

  std::vector<unsigned int> idx; idx.reserve(16);

  // clear i-th column and collect nonzeros
  for( unsigned int iv=_A.jc[_i]; iv<_A.jc[_i+1]; ++iv)
  {
    if(_A.ir[iv] == _i)
    {
      _A.pr[iv] = 1.0;
      _rhs[_i]  = _xi;
    }
    else
    {
      // update rhs
      _rhs[_A.ir[iv]] -= _A.pr[iv]*_xi;
      // clear entry
      _A.pr[iv] = 0;
      // store index
      idx.push_back( _A.ir[iv]);
    }

  }

  for(unsigned int i=0; i<idx.size(); ++i)
  {
    unsigned int col = idx[i];

    for( unsigned int j=_A.jc[col]; j<_A.jc[col+1]; ++j)
      if(_A.ir[j] == _i)
      {
	_A.pr[j] = 0.0;
	// move to next
	break;
      }
  }
}


//-----------------------------------------------------------------------------


template<class MatrixT>
void regularize_hack( MatrixT& _mat, double _v )
{
   unsigned int m = gmm::mat_nrows( _mat );
   unsigned int n = gmm::mat_ncols( _mat );

   if ( m!=n )
      std::cerr << "ERROR: regularizing works only for square matrices...\n";
   else
   {
      double sum = 0.0;
      for ( unsigned int i=0; i<n; ++i )
         sum += _mat( i,i );

      double v_hack = _v*sum/double( n );

      for ( unsigned int i=0; i<n; ++i )
         _mat( i,i ) += v_hack;

   }
}


//-----------------------------------------------------------------------------


template<class MatrixT, class VectorT>
int gauss_seidel_local( MatrixT& _A, VectorT& _x, VectorT& _rhs, std::vector<unsigned int> _idxs, int _max_iter, double _tolerance )
{
   typedef typename gmm::linalg_traits<MatrixT>::const_sub_col_type ColT;
   typedef typename gmm::linalg_traits<ColT>::const_iterator CIter;

   double t2 = _tolerance*_tolerance;

   // static variables to prevent memory allocation in each step
   std::vector<unsigned int> i_temp;
   std::queue<unsigned int>  q;

//    // clear old data
//    i_temp.clear();
//    q.clear();

   for ( unsigned int i=0; i<_idxs.size(); ++i )
      q.push( _idxs[i] );

   int it_count = 0;

   while ( !q.empty() && it_count < _max_iter )
   {
      ++it_count;
      unsigned int cur_i = q.front();
      q.pop();
      i_temp.clear();

      ColT col = mat_const_col( _A, cur_i );

      CIter it  = gmm::vect_const_begin( col );
      CIter ite = gmm::vect_const_end( col );

      double res_i   = -_rhs[cur_i];
      double x_i_new = _rhs[cur_i];
      double diag    = 1.0;
      for ( ; it!=ite; ++it )
      {
         res_i   += ( *it ) * _x[it.index()];
         x_i_new -= ( *it ) * _x[it.index()];
	 if( it.index() != cur_i)
	   i_temp.push_back( it.index() );
	 else
	   diag = *it;
      }

      // compare relative residuum normalized by diagonal entry
      if ( res_i*res_i/diag > t2 )
      {
         _x[cur_i] += x_i_new/_A( cur_i, cur_i );

         for ( unsigned int j=0; j<i_temp.size(); ++j )
            q.push( i_temp[j] );
      }
   }

   return it_count;
}

//-----------------------------------------------------------------------------

template<class MatrixT, class VectorT>
double residuum_norm( MatrixT& _A, VectorT& _x, VectorT& _rhs )
{
   if ( gmm::mat_ncols( _A ) != _x.size() )
      std::cerr << "DIM ERROR (residuum_norm): " << gmm::mat_ncols( _A ) << " vs " << _x.size() << std::endl;
   if ( gmm::mat_nrows( _A) !=_rhs.size() )
      std::cerr << "DIM ERROR 2 (residuum_norm): " << gmm::mat_nrows( _A) << " vs " << _rhs.size() << std::endl;

   // temp vectors
   VectorT Ax( _rhs.size());
   VectorT res( _rhs.size() );

   gmm::mult( _A,_x, Ax );
   gmm::add( Ax, gmm::scaled( _rhs, -1.0 ), res );

   return gmm::vect_norm2( res );
}


//-----------------------------------------------------------------------------


template<class MatrixT, class MatrixT2, class VectorT>
void factored_to_quadratic( MatrixT& _F, MatrixT2& _Q, VectorT& _rhs)
{
  unsigned int m = gmm::mat_nrows(_F);
  unsigned int n = gmm::mat_ncols(_F);

  // resize result matrix and vector
  gmm::resize(_Q, n-1, n-1);
  gmm::resize(_rhs, n);

//  // set up transposed
//  MatrixT Ft(n,m);
//  gmm::copy(gmm::transposed(_F), Ft);

  // compute quadratic matrix
  MatrixT Q(n,n);
//  gmm::mult(Ft,_F,Q);
  gmm::mult(gmm::transposed(_F),_F,Q);

  // extract rhs
  gmm::copy( gmm::scaled(gmm::mat_const_row( Q, n - 1),-1.0), _rhs);

  // resize and copy output
  gmm::resize( Q, n-1, n-1);
  _rhs.resize( n - 1);
  gmm::copy  ( Q, _Q);
}


//-----------------------------------------------------------------------------


// inspect the matrix: dimension, symmetry, zero_rows, zero_cols, nnz, max, min, max_abs, min_abs, NAN, INF
template<class MatrixT>
void inspect_matrix( const MatrixT& _A)
{
  typedef typename MatrixT::value_type VType;
  
  int m = gmm::mat_nrows(_A);
  int n = gmm::mat_ncols(_A);

  std::cerr << "################### INSPECT MATRIX ##################\n";
  std::cerr << "#rows        : " << m << std::endl;
  std::cerr << "#cols        : " << n << std::endl;
  std::cerr << "#nonzeros    : " << gmm::nnz(_A) << std::endl;
  std::cerr << "#nonzeros/row: " << (double(gmm::nnz(_A))/double(m)) << std::endl;
  std::cerr << "symmetric    : " << gmm::is_symmetric(_A) << std::endl;

  gmm::col_matrix<gmm::wsvector<VType> > Acol( m, n);
  gmm::row_matrix<gmm::wsvector<VType> > Arow( m, n);

  gmm::copy(_A, Acol);
  gmm::copy(_A, Arow);

  int zero_rows = 0;
  int zero_cols = 0;

  for(int i=0; i<m; ++i)
    if( Arow.row(i).size() == 0) ++zero_rows;

  for(int i=0; i<n; ++i)
    if( Acol.col(i).size() == 0) ++zero_cols;

  std::cerr << "zero rows    : " << zero_rows << std::endl;
  std::cerr << "zero cols    : " << zero_cols << std::endl;

  VType vmin     = std::numeric_limits<VType>::max();
  VType vmax     = std::numeric_limits<VType>::min();
  VType vmin_abs = std::numeric_limits<VType>::max();
  VType vmax_abs = 0;

  int n_nan = 0;
  int n_inf = 0;
  
  // inspect elements
  for(int i=0; i<n; ++i)
  {
    typedef typename gmm::linalg_traits< gmm::col_matrix< gmm::wsvector<VType> > >::const_sub_col_type ColT;
    typedef typename gmm::linalg_traits<ColT>::const_iterator CIter;

    ColT col = mat_const_col( Acol, i );
    CIter it  = gmm::vect_const_begin( col );
    CIter ite = gmm::vect_const_end( col );
    
    for(; it != ite; ++it)
    {
      if( *it < vmin ) vmin = *it;
      if( *it > vmax ) vmax = *it;

      if( fabs(*it) < vmin_abs) vmin_abs = fabs(*it);
      if( fabs(*it) > vmax_abs) vmax_abs = fabs(*it);

      if( std::isnan(*it)) ++n_nan;
      if( std::isinf(*it)) ++n_inf;
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


template<class MatrixT>
void print_dense( const MatrixT& _A)
{
  gmm::dense_matrix<double> A(gmm::mat_nrows(_A), gmm::mat_ncols(_A));
  gmm::copy(_A, A);
  std::cerr << A << std::endl;
}


//-----------------------------------------------------------------------------
#if COMISO_SUITESPARSE_AVAILABLE

/// GMM to Cholmod_sparse interface
template<class MatrixT>
void cholmod_to_gmm( const cholmod_sparse& _AC, MatrixT& _A)
{
  // initialize dimensions
  gmm::resize(_A,_AC.nrow, _AC.ncol);
  gmm::clear(_A);

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
          _A( I[j], i) += X[j];
    }
    else
    {
      int* P((int*)_AC.p);
      int* I((int*)_AC.i);

      for(int i=0; i<(int)_AC.ncol; ++i)
        for(int j= P[i]; j< P[i+1]; ++j)
          _A( I[j], i) += X[j];
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
          _A(I[j], i) += X[j];

          // add up symmetric part
          if( I[j] != i)
            _A(i,I[j]) += X[j];
        }
    }
    else
    {
      int* P((int*)_AC.p);
      int* I((int*)_AC.i);

      for(int i=0; i<(int)_AC.ncol; ++i)
        for(int j=P[i]; j<P[i+1]; ++j)
        {
          _A(I[j], i) += X[j];

          // add up symmetric part
          if( I[j] != i)
            _A(i,I[j]) += X[j];
        }
      }
  }
}


//-----------------------------------------------------------------------------


/// GMM to Cholmod_sparse interface
template<class MatrixT>
void gmm_to_cholmod( const MatrixT& _A, cholmod_sparse* &_AC, cholmod_common* _common, int _sparsity_type, bool _long_int)
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

  int m = gmm::mat_nrows(_A);
  int n = gmm::mat_ncols(_A);

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
    COMISO_GMM::get_ccs_symmetric_data( _A, uplo, values, rowind, colptr);

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
     COMISO_GMM::get_ccs_symmetric_data( _A, uplo, values, rowind, colptr);

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

//=============================================================================
} // namespace COMISO
//=============================================================================
